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

    /*! Output:
     *
     * MOV REG32 temp, REG32 op
     * or
     * LDR REG32 temp, MEM32 Shadow(IMM32 op)
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst *inst, rword address, rword instSize,
        CPUMode cpuMode, TempManager *temp_manager, const Patch *toMerge) {

        if(inst->getOperand(op).isImm()) {
            return {Ldr(cpuMode, temp_manager->getRegForTemp(temp), 
                       Constant(inst->getOperand(op).getImm()))
            };
        }
        else if(inst->getOperand(op).isReg()) {
            return {Mov(cpuMode, temp_manager->getRegForTemp(temp), inst->getOperand(op).getReg())};
        }
        else {
            LogError("GetOperand::generate", "Invalid operand type for GetOperand()");
            return {};
        }
    }
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
     * LDR REG32 temp, MEM32 Shadow(IMM32 op)
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst *inst, rword address, rword instSize,
        CPUMode cpuMode, TempManager *temp_manager, const Patch *toMerge) {
        if(cst < (2<<16)) {
            return{Mov(cpuMode, temp_manager->getRegForTemp(temp), Constant(cst))};
        }
        else {
            return{Ldr(cpuMode, temp_manager->getRegForTemp(temp), Constant(cst))};
        }
    }
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
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst *inst, rword address, rword instSize,
        CPUMode cpuMode, TempManager *temp_manager, const Patch *toMerge) {

        rword pcoffset = cpuMode == CPUMode::ARM ? 8 : 4;

        if(type == ConstantType) {
            return{Ldr(cpuMode, temp_manager->getRegForTemp(temp), Constant(address + pcoffset + cst))};
        }
        else if(type == OperandType) {
            if(inst->getOperand(op).isImm()) {
                return {Ldr(
                    cpuMode, 
                    temp_manager->getRegForTemp(temp), 
                    Constant(address + pcoffset + inst->getOperand(op).getImm()))
                };
            }
            else if(inst->getOperand(op).isReg()) {
                return {
                    Ldr(cpuMode, temp_manager->getRegForTemp(temp), Constant(address + pcoffset)),
                    NoReloc(add(temp_manager->getRegForTemp(temp), inst->getOperand(op).getReg()))
                };
            }
            else {
                LogError("GePCOffset::generate", "Invalid operand type for GetOperand()");
                return {};
            }
        }
        _QBDI_UNREACHABLE();
    }
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
     * LDR REG32 temp, MEM32 Shadow(IMM32 instID)
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst *inst, rword address, rword instSize,
        CPUMode cpuMode, TempManager *temp_manager, const Patch *toMerge) {

        return {LdrInstID(cpuMode, temp_manager->getRegForTemp(temp))};
    }
};

class CopyReg : public PatchGenerator, public AutoAlloc<PatchGenerator, CopyReg> {
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
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst *inst, rword address, rword instSize, 
        CPUMode cpuMode, TempManager *temp_manager, const Patch *toMerge) {

        return {Mov(cpuMode, temp_manager->getRegForTemp(temp), reg)};
    }
};

class WriteTemp: public PatchGenerator, public AutoAlloc<PatchGenerator, WriteTemp> {
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
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst *inst, rword address, rword instSize,
        CPUMode cpuMode, TempManager *temp_manager, const Patch *toMerge) {

        return {Str(cpuMode, temp_manager->getRegForTemp(temp), offset)};
    }

    bool modifyPC() {
        return offset == Offset(Reg(REG_PC));
    }
};

class SaveReg: public PatchGenerator, public AutoAlloc<PatchGenerator, SaveReg> {

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
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst *inst, rword address, rword instSize,
        CPUMode cpuMode, TempManager *temp_manager, const Patch *toMerge) {

        return {Str(cpuMode, reg, offset)};
    }

    RelocatableInst::SharedPtrVec generate(CPUMode cpuMode) {
        return this->generate(nullptr, 0, 0, cpuMode, nullptr, nullptr);
    }
};

class LoadReg: public PatchGenerator, public AutoAlloc<PatchGenerator, LoadReg> {

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
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst *inst, rword address, rword instSize,
        CPUMode cpuMode, TempManager *temp_manager, const Patch *toMerge) {

        return {Ldr(cpuMode, reg, offset)};
    }

    RelocatableInst::SharedPtrVec generate(CPUMode cpuMode) {
        return this->generate(nullptr, 0, 0, cpuMode, nullptr, nullptr);
    }
};

class JmpEpilogue : public PatchGenerator, public AutoAlloc<PatchGenerator, JmpEpilogue> {

public:
    
    /*! Generate a jump instruction which target the epilogue of the ExecBlock.
    */
    JmpEpilogue() {}

    /*! Output:
     *
     * B Offset(Epilogue)
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst *inst, rword address, rword instSize,
        CPUMode cpuMode, TempManager *temp_manager, const Patch *toMerge) {

        RelocatableInst::SharedPtrVec patch;
        patch.push_back(Str(cpuMode, Reg(REG_LR), Offset(Reg(REG_LR))));
        if(cpuMode == CPUMode::ARM) {
            patch.push_back(BlEpilogue(cpuMode));
        }
        else if(cpuMode == CPUMode::Thumb) {
            patch.push_back(BlxEpilogue(cpuMode));
        }
        return patch;
    }

    RelocatableInst::SharedPtrVec generate(CPUMode cpuMode) {
        return this->generate(nullptr, 0, 0, cpuMode, nullptr, nullptr);
    }
};

class SimulateLink : public PatchGenerator, public AutoAlloc<PatchGenerator, SimulateLink> {
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
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst *inst, rword address, rword instSize,
        CPUMode cpuMode, TempManager *temp_manager, const Patch *toMerge) {

        RelocatableInst::SharedPtrVec patch;

        if(cpuMode == CPUMode::ARM) {
            append(patch, GetPCOffset(temp, Constant(instSize - 8)).generate(inst, address, instSize, cpuMode, temp_manager, nullptr));
        }
        else {
            append(patch, GetPCOffset(temp, Constant(instSize - 4 + 1)).generate(inst, address, instSize, cpuMode, temp_manager, nullptr));
        }
        patch.push_back(Mov(cpuMode, Reg(REG_LR), temp_manager->getRegForTemp(temp)));

        return patch;
    }
};

class SimulateExchange : public PatchGenerator, public AutoAlloc<PatchGenerator, SimulateExchange> {
    Temp  temp;

public:

    /*! Simulate the effects of the link operation performed by BL and BLX instructions: the address of
     * the next instruction is copied into the LR register. A temp and a shadow are needed to
     * compute this address.
     *
     * @param[in] temp   Any unused temporary, overwritten by this generator.
    */
    SimulateExchange(Temp temp): temp(temp) {}

    /*! Output:
     *
     * LDR REG32 temp, MEM32 Shadow(IMM32 1)
     * STR REG32 temp, MEM32 DSataBlock[Offset(hostState.exchange)]
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst *inst, rword address, rword instSize, 
        CPUMode cpuMode, TempManager *temp_manager, const Patch *toMerge) {
        RelocatableInst::SharedPtrVec patch;

        append(patch, GetConstant(temp, Constant(1)).generate(inst, address, instSize, cpuMode, temp_manager, nullptr));
        append(patch, WriteTemp(temp, Offset(offsetof(Context, hostState.exchange))).generate(inst, address, instSize, cpuMode, temp_manager, nullptr));

        return patch;
    }
};

class SimulatePopPC : public PatchGenerator, public AutoAlloc<PatchGenerator, SimulatePopPC> {
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
     * STR REG32 temp, MEM32 DataBlock[Offset(PC)]
     *
    */
    RelocatableInst::SharedPtrVec generate(const llvm::MCInst *inst, rword address, rword instSize,
        CPUMode cpuMode, TempManager *temp_manager, const Patch *toMerge) {

        RelocatableInst::SharedPtrVec patch;
        int64_t cond = inst->getOperand(2).getImm();

        append(patch, GetPCOffset(temp, Constant(-4)).generate(inst, address, instSize, cpuMode, temp_manager, nullptr));
        patch.push_back(NoReloc(
            pop(temp_manager->getRegForTemp(temp), cond)
        ));
        append(patch, WriteTemp(temp, Offset(Reg(REG_PC))).generate(inst, address, instSize, cpuMode, temp_manager, nullptr));

        return patch;
    }

    bool modifyPC() {
        return true;
    }
};

}

#endif
