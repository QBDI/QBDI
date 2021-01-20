/*
 * This file is part of QBDI.
 *
 * Copyright 2017 Quarkslab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef PATCHGENERATOR_ARM_H
#define PATCHGENERATOR_ARM_H

#include "Patch/ARM/Layer2_ARM.h"
#include "Patch/PatchUtils.h"
#include "Patch/RelocatableInst.h"
#include "Patch/ARM/RelocatableInst_ARM.h"
#include "Patch/PatchGenerator.h"

namespace QBDI {

class GetOperand : public AutoClone<PatchGenerator, GetOperand> {

    Temp temp;
    Operand op;

public:

    /*! Obtain the value of the operand op and copy it's value in a temporary. If op is an immediate
     * the immediate value is copied, if op is a register the register value is copied.
     *
     * @param[in] temp   A temporary where the value will be copied.
     * @param[in] op     The operand index (relative to the instruction LLVM MCInst representation)
     *                   to be copied.
    */
    GetOperand(Temp temp, Operand op): temp(temp), op(op) {}

    /*! Output:
     *
     * MOV REG32 temp, REG32 op
     * or
     * LDR REG32 temp, MEM32 Shadow(IMM32 op)
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst *inst,
        rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, Patch *toMerge);
};

class GetConstant : public AutoClone<PatchGenerator, GetConstant> {

    Temp temp;
    Constant cst;

public:

    /*! Copy a constant in a temporary.
     *
     * @param[in] temp   A temporary where the value will be copied.
     * @param[in] cst    The constant to copy.
    */
    GetConstant(Temp temp, Constant cst): temp(temp), cst(cst) {}

    /*! Output:
     *
     * LDR REG32 temp, MEM32 Shadow(IMM32 op)
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst *inst,
        rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, Patch *toMerge);
};

class GetPCOffset : public AutoClone<PatchGenerator, GetPCOffset> {

    Temp temp;
    Constant cst;
    Operand op;
    enum {
        ConstantType,
        OperandType,
    } type;

public:

    /*! Interpret a constant as a PC relative offset and copy it in a temporary. It can be used to
     * obtain the current value of PC by using a constant of 0.
     *
     * @param[in] temp     A temporary where the value will be copied.
     * @param[in] cst      The constant to be used.
    */
    GetPCOffset(Temp temp, Constant cst): temp(temp), cst(cst), op(0), type(ConstantType) {}

    /*! Interpret an operand as a PC relative offset and copy it in a temporary. It can be used to
     * obtain jump/call targets or relative memory access addresses.
     *
     * @param[in] temp     A temporary where the value will be copied.
     * @param[in] op       The  operand index (relative to the instruction LLVM MCInst
     *                     representation) to be used.
    */
    GetPCOffset(Temp temp, Operand op): temp(temp), cst(0), op(op), type(OperandType) {}

    /*! Output:
     *
     * If cst:
     * LDR REG32 temp, MEM32 Shadow(IMM32 (cst + address + 8))
     *
     * If op is an immediate:
     * LDR REG32 temp, MEM32 Shadow(IMM32 (op + address + 8))
     *
     * If op is a register:
     * LDR REG32 temp, MEM32 Shadow(IMM32 (address + 8))
     * ADD REG32 temp, REG32 op
     *
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst *inst,
        rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, Patch *toMerge);
};



class GetInstId : public AutoClone<PatchGenerator, GetInstId> {
    Temp temp;

public:

    /*! Copy an ExecBlock specific id for the current instruction in a temporary. This id is used to
     * identify the instruction responsible for a callback in the engine and is only meant for
     * internal use.
     *
     * @param[in] temp   A temporary where the id will be copied.
    */
    GetInstId(Temp temp): temp(temp) {}

    /*! Output:
     *
     * LDR REG32 temp, MEM32 Shadow(IMM32 instID)
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst *inst,
        rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, Patch *toMerge);
};

class CopyReg : public AutoClone<PatchGenerator, CopyReg> {
    Temp  temp;
    Reg reg;

public:

    /*! Copy a register in a temporary.
     *
     * @param[in] temp   A temporary where the register will be copied.
     * @param[in] reg    The register which will be copied
    */
    CopyReg(Temp temp, Reg reg): temp(temp), reg(reg) {}

    /*! Output:
     *
     * MOV REG32 temp, REG32 reg
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst *inst,
        rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, Patch *toMerge);
};

class WriteTemp: public AutoClone<PatchGenerator, WriteTemp> {
    Temp  temp;
    Offset offset;

public:

    /*! Write a temporary value in the data block at the specified offset. This can be used to overwrite
     * register values in the context part of the data block.
     *
     * @param[in] temp    A temporary which will be written.
     * @param[in] offset  The offset in the data block where the temporary will be written.
    */
    WriteTemp(Temp temp, Offset offset): temp(temp), offset(offset) {}

    /*! Output:
     *
     * STR MEM32 DataBlock[offset], REG32 temp
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst *inst,
        rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, Patch *toMerge);

    bool modifyPC() {return offset == Offset(Reg(REG_PC));}
};

class SaveReg: public AutoClone<PatchGenerator, SaveReg>,
    public PureEval<SaveReg> {

    Reg   reg;
    Offset offset;

public:

    /*! Save a register in the data block at the specified offset. This can be used to save
     * register values in the context part of the data block.
     *
     * @param[in] reg     A register which will be saved.
     * @param[in] offset  The offset in the data block where the register will be written.
    */
    SaveReg(Reg reg, Offset offset): reg(reg), offset(offset) {}

    /*! Output:
     *
     * STR MEM32 DataBlock[offset], REG32 reg
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst *inst,
        rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, Patch *toMerge);
};

class LoadReg: public AutoClone<PatchGenerator, LoadReg>,
    public PureEval<LoadReg> {

    Reg   reg;
    Offset offset;

public:

    /*! Load a register from the data block at the specified offset. This can be used to load
     * register values from the context part of the data block.
     *
     * @param[in] reg     A register where the value will be loaded.
     * @param[in] offset  The offset in the data block from where the value will be loaded.
    */
    LoadReg(Reg reg, Offset offset): reg(reg), offset(offset) {}

    /*! Output:
     *
     * LDR REG32 reg, MEM32 DataBlock[offset]
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst *inst,
        rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, Patch *toMerge);
};

class JmpEpilogue : public AutoClone<PatchGenerator, JmpEpilogue>,
    public PureEval<JmpEpilogue> {

public:

    /*! Generate a jump instruction which target the epilogue of the ExecBlock.
    */
    JmpEpilogue() {}

    /*! Output:
     *
     * B Offset(Epilogue)
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst *inst,
        rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, Patch *toMerge);
};

class SimulateLink : public AutoClone<PatchGenerator, SimulateLink> {
    Temp  temp;

public:

    /*! Simulate the effects of the link operation performed by BL and BLX instructions: the address of
     * the next instruction is copied into the LR register. A temp and a shadow are needed to
     * compute this address.
     *
     * @param[in] temp   Any unused temporary, overwritten by this generator.
    */
    SimulateLink(Temp temp): temp(temp) {}

    /*! Output:
     *
     * LDR REG32 temp, MEM32 Shadow(IMM32 (address + 4))
     * MOV REG32 LR, REG32 temp
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst *inst,
        rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, Patch *toMerge);
};

class SimulatePopPC : public AutoClone<PatchGenerator, SimulatePopPC> {
    Temp  temp;

public:

    /*! Simulate an (eventually conditional) return instruction (POPcc PC). The conditional code of the
     * current instruction is used by this generator. This generator signals a PC modification and
     * triggers an end of basic block.
     *
     * @param[in] temp   Any unused temporary, overwritten by this generator.
    */
    SimulatePopPC(Temp temp): temp(temp) {}

    /*! Output:
     *
     * LDR REG32 temp, MEM32 Shadow(IMM32 (address + 4))
     * POPcc REG32 temp
     * STR MEM32 DataBlock[Offset(PC)], REG32 temp
     *
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst *inst,
        rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, Patch *toMerge);

    bool modifyPC() {return true;}
};

}

#endif
