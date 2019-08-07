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
#ifndef PATCHGENERATOR_X86_64_H
#define PATCHGENERATOR_X86_64_H

#include "Patch/X86_64/Layer2_X86_64.h"
#include "Patch/PatchGenerator.h"
#include "Patch/InstInfo.h"

namespace QBDI {

class GetOperand : public PatchGenerator, public AutoAlloc<PatchGenerator, GetOperand> {
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

    /*!
     * Output:
     *   MOV REG64 temp, IMM64/REG64 op
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst* inst,
        rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge);
};


class GetConstant : public PatchGenerator, public AutoAlloc<PatchGenerator, GetConstant> {
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
     * MOV REG64 temp, IMM64 cst
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst* inst,
        rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge);
};

class GetPCOffset : public PatchGenerator, public AutoAlloc<PatchGenerator, GetPCOffset> {

    Temp temp;
    Constant cst;
    Operand op;
    enum {
        ConstantType,
        OperandType,
    } type;

public:

    /*! Interpret a constant as a RIP relative offset and copy it in a temporary. It can be used to
     * obtain the current value of RIP by using a constant of 0.
     *
     * @param[in] temp     A temporary where the value will be copied.
     * @param[in] cst      The constant to be used.
    */
    GetPCOffset(Temp temp, Constant cst): temp(temp), cst(cst), op(0), type(ConstantType) {}

    /*! Interpret an operand as a RIP relative offset and copy it in a temporary. It can be used to
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
     * MOV REG64 temp, IMM64 (cst + address + instSize)

     * If op is an immediate:
     * MOV REG64 temp, IMM64 (op + address + instSize)
     *
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst* inst,
        rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge);
};

class GetReadAddress : public PatchGenerator, public AutoAlloc<PatchGenerator, GetReadAddress> {

    Temp temp;
    size_t index;
    uint64_t TSFlags;

public:

    /*! Resolve the memory address where the instructions will read its value and copy the address in a
     * temporary. This PatchGenerator is only guaranteed to work before the instruction has been
     * executed.
     *
     * @param[in] temp      A temporary where the memory address will be copied.
     * @param[in] index     Index of access to saved when instruction does many read access
     * @param[in] TSFlags   Instruction architecture dependent flag from MCInstrDesc
    */
    GetReadAddress(Temp temp, size_t index, uint64_t TSFlags) : temp(temp), index(index), TSFlags(TSFlags) {}

    /*! Output:
     *
     * if stack access:
     * MOV REG64 temp, REG64 RSP
     *
     * else:
     * LEA REG64 temp, MEM64 addr
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst* inst,
        rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge);
};

class GetWriteAddress : public PatchGenerator, public AutoAlloc<PatchGenerator, GetWriteAddress> {

    Temp temp;
    uint64_t TSFlags;

public:

    /*! Resolve the memory address where the instructions will write its value and copy the address in a
     * temporary. This PatchGenerator is only guaranteed to work before the instruction has been
     * executed.
     *
     * @param[in] temp   A temporary where the memory address will be copied.
     * @param[in] TSFlags   Instruction architecture dependent flag from MCInstrDesc
    */
    GetWriteAddress(Temp temp, uint64_t TSFlags) : temp(temp), TSFlags(TSFlags) {}

    /*! Output:
     *
     * if stack access:
     * MOV REG64 temp, REG64 RSP
     *
     * else:
     * LEA REG64 temp, MEM64 addr
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst* inst,
        rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge);
};

class GetReadValue : public PatchGenerator, public AutoAlloc<PatchGenerator, GetReadValue> {

    Temp temp;
    size_t index;
    uint64_t TSFlags;

 public:

    /*! Resolve the memory address where the instructions will read its value and copy the value in a
     * temporary. This PatchGenerator is only guaranteed to work before the instruction has been
     * executed.
     *
     * @param[in] temp   A temporary where the memory value will be copied.
     * @param[in] index  index of access to saved when instruction does many read access
     * @param[in] TSFlags   Instruction architecture dependent flag from MCInstrDesc
    */
    GetReadValue(Temp temp, size_t index, uint64_t TSFlags) : temp(temp), index(index), TSFlags(TSFlags) {}

    /*! Output:
     *
     * MOV REG64 temp, MEM64 val
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst* inst,
         rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge);
};

class GetWriteValue : public PatchGenerator, public AutoAlloc<PatchGenerator, GetWriteValue> {

    Temp temp;
    uint64_t TSFlags;

public:

    /*! Resolve the memory address where the instructions has written its value and copy back the value
     * in a temporary. This PatchGenerator is only guaranteed to work after the instruction has been
     * executed.
     *
     * @param[in] temp   A temporary where the memory value will be copied.
     * @param[in] TSFlags   Instruction architecture dependent flag from MCInstrDesc
    */
    GetWriteValue(Temp temp, uint64_t TSFlags) : temp(temp), TSFlags(TSFlags) {}

    /*! Output:
     *
     * MOV REG64 temp, MEM64 val
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst* inst,
         rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge);
};

class CopyReg : public PatchGenerator, public AutoAlloc<PatchGenerator, CopyReg> {
    Temp dst;
    Reg src;

public:

    /*! Copy a register in a temporary.
     *
     * @param[in] dst    A temporary where the register will be copied.
     * @param[in] src    The register which will be copied
    */
    CopyReg(Temp dst, Reg src) : dst(dst), src(src) {}

    /*! Output:
     *
     * MOV REG64 temp, REG64 reg
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst* inst,
        rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge);
};

class GetInstId : public PatchGenerator, public AutoAlloc<PatchGenerator, GetInstId> {

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
     * MOV REG64 temp, IMM64 instID
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst* inst,
        rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge);
};

class WriteTemp : public PatchGenerator, public AutoAlloc<PatchGenerator, WriteTemp> {

    Temp  temp;
    enum {
        OffsetType,
        ShadowType
    } type;
    // Not working VS 2015
    // union {
        Offset offset;
        Shadow shadow;
    // }

public:

    /*! Write a temporary value in the data block at the specified offset. This can be used to overwrite
     * register values in the context part of the data block.
     *
     * @param[in] temp    A temporary which will be written.
     * @param[in] offset  The offset in the data block where the temporary will be written.
    */
    WriteTemp(Temp temp, Offset offset): temp(temp), type(OffsetType), offset(offset), shadow(0) {}

    /*! Write a temporary value in a shadow in the data block.
     *
     * @param[in] temp      A temporary which will be written.
     * @param[in] shadow    The shadow use to store the value.
    */
    WriteTemp(Temp temp, Shadow shadow): temp(temp), type(ShadowType), offset(0), shadow(shadow) {}

    /*! Output:
     *
     * MOV MEM64 DataBlock[offset], REG64 temp
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst* inst,
        rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge);

    bool modifyPC() {return offset == Offset(Reg(REG_PC));}
};

class SaveReg : public PatchGenerator, public AutoAlloc<PatchGenerator, SaveReg>,
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
     * MOV MEM64 DataBlock[offset], REG64 reg
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst* inst,
        rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge);
};

class LoadReg : public PatchGenerator, public AutoAlloc<PatchGenerator, LoadReg>,
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
     * MOV REG64 reg, MEM64 DataBlock[offset]
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst* inst,
        rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge);
};

class JmpEpilogue : public PatchGenerator, public AutoAlloc<PatchGenerator, JmpEpilogue>,
    public PureEval<JmpEpilogue> {

public:

    /*! Generate a jump instruction which target the epilogue of the ExecBlock.
    */
    JmpEpilogue() {}

    /*! Output:
     *
     * JMP Offset(Epilogue)
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst* inst,
        rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge);
};

class SimulateCall : public PatchGenerator, public AutoAlloc<PatchGenerator, SimulateCall> {

    Temp temp;

public:

    /*! Simulate the effects of a call to the address stored in a temporary. The target address
     * overwrites the stored value of RIP in the context part of the data block and the return address
     * is pushed onto the stack. This generator signals a PC modification and triggers and end of basic
     * block.
     *
     * @param[in] temp   Stores the call target address. Overwritten by this generator.
    */
    SimulateCall(Temp temp) : temp(temp) {}

    /*! Output:
     *
     * MOV MEM64 DataBlock[Offset(RIP)], REG64 temp
     * MOV REG64 temp, IMM64 (address + InstSize)
     * PUSH REG64 temp
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst* inst,
        rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge);

    bool modifyPC() {return true;}
};

class SimulateRet : public PatchGenerator, public AutoAlloc<PatchGenerator, SimulateRet> {

    Temp temp;

public:

    /*! Simulate the effects of a return instruction. First the return address is popped from the stack
     * into a temporary, then an optional stack deallocation is performed and finally the return
     * address is written in the stored value of RIP in the context part of the data block. This
     * generator signals a PC modification and triggers an end of basic block.
     *
     * The optional deallocation is performed if the current instruction has one single immediate
     * operand (which is the case of RETIQ and RETIW).
     *
     * @param[in] temp   Any unused temporary, overwritten by this generator.
    */
    SimulateRet(Temp temp) : temp(temp) {}

    /*! Output:
     *
     * POP REG64 temp
     * ADD RSP, IMM64 deallocation # Optional
     * MOV MEM64 DataBlock[Offset(RIP)], REG64 temp
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst* inst,
        rword address, rword instSize, TempManager *temp_manager, const Patch *toMerge);

    bool modifyPC() {return true;}
};

PatchGenerator::SharedPtrVec generateReadInstrumentPatch(Patch &patch, llvm::MCInstrInfo* MCII,
                                                         llvm::MCRegisterInfo* MRI);

PatchGenerator::SharedPtrVec generateWriteInstrumentPatch(Patch &patch, llvm::MCInstrInfo* MCII,
                                                          llvm::MCRegisterInfo* MRI);

}

#endif
