/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2021 Quarkslab
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
#include "Patch/ARM/PatchGenerator_ARM.h"

namespace QBDI {

RelocatableInst::SharedPtrVec GetOperand::generate(const llvm::MCInst *inst,
    rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, Patch *toMerge) {

    if(inst->getOperand(op).isImm()) {
        return {Ldr(temp_manager->getRegForTemp(temp),
                   Constant(inst->getOperand(op).getImm()))
        };
    }
    else if(inst->getOperand(op).isReg()) {
        return {NoReloc(mov(temp_manager->getRegForTemp(temp), inst->getOperand(op).getReg()))};
    }
    else {
        LogError("GetOperand::generate", "Invalid operand type for GetOperand()");
        return {};
    }
}

RelocatableInst::SharedPtrVec GetConstant::generate(const llvm::MCInst *inst,
    rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, Patch *toMerge) {

    return{Ldr(temp_manager->getRegForTemp(temp), Constant(cst))};
}

RelocatableInst::SharedPtrVec GetPCOffset::generate(const llvm::MCInst *inst,
    rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, Patch *toMerge) {

    if(type == ConstantType) {
        return{Ldr(temp_manager->getRegForTemp(temp), Constant(address + 8 + cst))};
    }
    else if(type == OperandType) {
        if(inst->getOperand(op).isImm()) {
            return {Ldr(
                temp_manager->getRegForTemp(temp),
                Constant(address + 8 + inst->getOperand(op).getImm()))
            };
        }
        else if(inst->getOperand(op).isReg()) {
            return {
                Ldr(temp_manager->getRegForTemp(temp), Constant(address + 8)),
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

RelocatableInst::SharedPtrVec GetInstId::generate(const llvm::MCInst *inst,
    rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, Patch *toMerge) {

    return {InstId(ldri12(temp_manager->getRegForTemp(temp), Reg(REG_PC), 0), 2)};
}

RelocatableInst::SharedPtrVec CopyReg::generate(const llvm::MCInst *inst,
    rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, Patch *toMerge) {

    return {NoReloc(mov(temp_manager->getRegForTemp(temp), reg))};
}

RelocatableInst::SharedPtrVec WriteTemp::generate(const llvm::MCInst *inst,
    rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, Patch *toMerge) {

    return {Str(temp_manager->getRegForTemp(temp), offset)};
}

RelocatableInst::SharedPtrVec SaveReg::generate(const llvm::MCInst *inst,
    rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, Patch *toMerge) {

    return {Str(reg, offset)};
}

RelocatableInst::SharedPtrVec LoadReg::generate(const llvm::MCInst *inst,
    rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, Patch *toMerge) {

    return {Ldr(reg, offset)};
}

RelocatableInst::SharedPtrVec JmpEpilogue::generate(const llvm::MCInst *inst,
    rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, Patch *toMerge) {

    return {EpilogueRel(b(0), 0, -8)};
}

RelocatableInst::SharedPtrVec SimulateLink::generate(const llvm::MCInst *inst,
    rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, Patch *toMerge) {
    RelocatableInst::SharedPtrVec patch;

    append(patch, GetPCOffset(Temp(0), Constant(-4)).generate(inst, address, instSize, temp_manager, nullptr));
    patch.push_back(NoReloc(mov(Reg(REG_LR), temp_manager->getRegForTemp(temp))));

    return patch;
}

RelocatableInst::SharedPtrVec SimulatePopPC::generate(const llvm::MCInst *inst,
    rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, Patch *toMerge) {
    RelocatableInst::SharedPtrVec patch;
    int64_t cond = inst->getOperand(2).getImm();

    append(patch, GetPCOffset(temp, Constant(-4)).generate(inst, address, instSize, temp_manager, nullptr));
    patch.push_back(NoReloc(
        pop(temp_manager->getRegForTemp(temp), cond)
    ));
    append(patch, WriteTemp(temp, Offset(Reg(REG_PC))).generate(inst, address, instSize, temp_manager, nullptr));

    return patch;
}

}
