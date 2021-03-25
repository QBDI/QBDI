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

#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"

#include "QBDI/Platform.h"
#include "Patch/PatchUtils.h"
#include "Patch/InstInfo.h"
#include "Patch/RegisterSize.h"
#include "Utility/LogSys.h"

#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
// skip RAX as it is very often used implicitly and LLVM
// sometimes don't tell us...
static constexpr unsigned int _QBDI_FIRST_FREE_REGISTER = 1;
#else
static constexpr unsigned int _QBDI_FIRST_FREE_REGISTER = 0;
#endif


namespace QBDI {


Reg TempManager::getRegForTemp(unsigned int id) {
    unsigned int i;

    // Check if the id is already alocated
    for(auto p : temps) {
        if(p.first == id) {
            return Reg(p.second);
        }
    }

    // Start from the last free register found (or default)
    if(temps.size() > 0) {
        i = temps.back().second + 1;
    }
    else {
        i = _QBDI_FIRST_FREE_REGISTER;
    }

    const llvm::MCInstrDesc &desc = MCII->get(inst.getOpcode());
    // Find a free register
    for(; i < AVAILABLE_GPR; i++) {
        bool free = true;
        // Check for explicit registers
        for(unsigned int j = 0; j < inst.getNumOperands(); j++) {
            const llvm::MCOperand &op = inst.getOperand(j);
            if (op.isReg() && MRI->isSubRegisterEq(GPR_ID[i], op.getReg())) {
                free = false;
                break;
            }
        }
        // Check for implicitly used registers
        if (free) {
            const uint16_t* implicitRegs = desc.getImplicitUses();
            for (; implicitRegs && *implicitRegs; ++implicitRegs) {
                if (MRI->isSubRegisterEq(GPR_ID[i], *implicitRegs)) {
                    free = false;
                    break;
                }
            }
        }
        // Check for implicitly modified registers
        if (free) {
            const uint16_t* implicitRegs = desc.getImplicitDefs();
            for (; implicitRegs && *implicitRegs; ++implicitRegs) {
                if (MRI->isSubRegisterEq(GPR_ID[i], *implicitRegs)) {
                    free = false;
                    break;
                }
            }
        }
        if(free) {
            // store it and return it
            temps.emplace_back(id, i);
            return Reg(i);
        }
    }

    // bypass for pusha and popa. MemoryAccess will not work on theses instruction
    if(allowInstRegister and useAllRegisters(inst)) {
        if(temps.size() > 0) {
            i = temps.back().second + 1;
        }
        else {
            i = _QBDI_FIRST_FREE_REGISTER;
        }
        // store it and return it
        if (i < AVAILABLE_GPR) {
            temps.emplace_back(id, i);
            return Reg(i);
        }
    }
    QBDI_ERROR("No free registers found");
    abort();
}

Reg::Vec TempManager::getUsedRegisters() const {
    Reg::Vec list;
    for(auto p: temps)
        list.push_back(Reg(p.second));
    return list;
}

size_t TempManager::getUsedRegisterNumber() const {
    return temps.size();
}

unsigned TempManager::getSizedSubReg(unsigned reg, unsigned size) const {
    if(getRegisterSize(reg) == size) {
        return reg;
    }
    for(unsigned i = 1; i < MRI->getNumSubRegIndices(); i++) {
        unsigned subreg = MRI->getSubReg(reg, i);
        if(subreg != 0 && getRegisterSize(subreg) == size) {
            return subreg;
        }
    }
    QBDI_ERROR("No sub register of size {} found for register {} ({})", size, reg, MRI->getName(reg));
    abort();
}

}
