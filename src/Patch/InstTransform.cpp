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

#include "Patch/InstTransform.h"

namespace QBDI {

void SetOperand::transform(llvm::MCInst &inst, rword address, size_t instSize, TempManager *temp_manager) {
    switch(type) {
        case TempOperandType:
            inst.getOperand(opn).setReg(temp_manager->getRegForTemp(temp));
            break;
        case RegOperandType:
            inst.getOperand(opn).setReg(reg);
            break;
        case ImmOperandType:
            inst.getOperand(opn).setImm(imm);
            break;
    }
}

void SubstituteWithTemp::transform(llvm::MCInst &inst, rword address, size_t instSize, TempManager *temp_manager) {
    for(unsigned int i = 0; i < inst.getNumOperands(); i++) {
        llvm::MCOperand &op = inst.getOperand(i);
        if(op.isReg() && op.getReg() == reg) {
            op.setReg(temp_manager->getRegForTemp(temp));
        }
    }
}

void AddOperand::transform(llvm::MCInst &inst, rword address, size_t instSize, TempManager *temp_manager) {
    switch(type) {
        case TempOperandType:
            inst.insert(inst.begin() + opn, llvm::MCOperand::createReg(temp_manager->getRegForTemp(temp)));
            break;
        case RegOperandType:
            inst.insert(inst.begin() + opn, llvm::MCOperand::createReg(reg));
            break;
        case ImmOperandType:
            inst.insert(inst.begin() + opn, llvm::MCOperand::createImm(imm));
            break;
    }
}

void RemoveOperand::transform(llvm::MCInst &inst, rword address, size_t instSize, TempManager *temp_manager) {
    for(auto it = inst.begin(); it != inst.end(); ++it) {
        if(it->isReg() && it->getReg() == reg) {
            inst.erase(it);
            break;
        }
    }
}

void SetOpcode::transform(llvm::MCInst &inst, rword address, size_t instSize, TempManager *temp_manager) {
    inst.setOpcode(opcode);
}

}
