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
#ifndef PATCHUTILS_H
#define PATCHUTILS_H

#include <memory>
#include <utility>
#include <vector>

#include "Platform.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "Patch/Types.h"
#include "Utility/LogSys.h"

#if defined(QBDI_ARCH_X86_64)
// skip RAX as it is very often used implicitly and LLVM
// sometimes don't tell us...
#define _QBDI_FIRST_FREE_REGISTER 1
#else
#define _QBDI_FIRST_FREE_REGISTER 0
#endif

namespace QBDI {

class RelocatableInst;
class PatchGenerator;

// Helper template

template<typename T, typename U> class AutoAlloc {
public:

    operator std::shared_ptr<T>() {
        return std::shared_ptr<T>(new U(*static_cast<U*>(this)));
    }
};

void inline append(std::vector<std::shared_ptr<RelocatableInst>> &u, const std::vector<std::shared_ptr<RelocatableInst>> v) {
    u.insert(u.end(), v.begin(), v.end());
}

void inline prepend(std::vector<std::shared_ptr<RelocatableInst>> &u, const std::vector<std::shared_ptr<RelocatableInst>> v) {
    u.insert(u.begin(), v.begin(), v.end());
}

void inline insert(std::vector<std::shared_ptr<RelocatableInst>> &u, size_t pos, const std::vector<std::shared_ptr<RelocatableInst>> v) {
    u.insert(u.begin() + pos, v.begin(), v.end());
}

void inline append(std::vector<std::shared_ptr<PatchGenerator>> &u, const std::vector<std::shared_ptr<PatchGenerator>> v) {
    u.insert(u.end(), v.begin(), v.end());
}

void inline prepend(std::vector<std::shared_ptr<PatchGenerator>> &u, const std::vector<std::shared_ptr<PatchGenerator>> v) {
    u.insert(u.begin(), v.begin(), v.end());
}

void inline insert(std::vector<std::shared_ptr<PatchGenerator>> &u, size_t pos, const std::vector<std::shared_ptr<PatchGenerator>> v) {
    u.insert(u.begin() + pos, v.begin(), v.end());
}

// Helper classes

class TempManager {

    std::vector<std::pair<unsigned int, unsigned int>> temps;
    const llvm::MCInst* inst;
    llvm::MCInstrInfo* MCII;
    llvm::MCRegisterInfo* MRI;

public:

    TempManager(const llvm::MCInst *inst, llvm::MCInstrInfo* MCII, llvm::MCRegisterInfo *MRI) : inst(inst), MCII(MCII), MRI(MRI) {};

    Reg getRegForTemp(unsigned int id) {
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

        const llvm::MCInstrDesc &desc = MCII->get(inst->getOpcode());
        // Find a free register
        for(; i < AVAILABLE_GPR; i++) {
            bool free = true;
            // Check for explicit registers
            for(unsigned int j = 0; inst && j < inst->getNumOperands(); j++) {
                const llvm::MCOperand &op = inst->getOperand(j);
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
                temps.push_back(std::make_pair(id, i));
                return Reg(i);
            }
        }
        LogError("TempManager::getRegForTemp", "No free registers found");
        abort();
    }

    Reg::Vec getUsedRegisters() {
        Reg::Vec list;
        for(auto p: temps)
            list.push_back(Reg(p.second));
        return list;
    }

    size_t getUsedRegisterNumber() {
        return temps.size();
    }

    unsigned getRegSize(unsigned reg) {
        for(unsigned i = 0; i < MRI->getNumRegClasses(); i++) {
            if(MRI->getRegClass(i).contains(reg)) {
                return MRI->getRegClass(i).getSize();
            }
        }
        LogError("TempManager::getRegSize", "Register class not found");
        abort();
    }

    unsigned getSizedSubReg(unsigned reg, unsigned size) {
        if(getRegSize(reg) == size) {
            return reg;
        }
        for(unsigned i = 1; i < MRI->getNumSubRegIndices(); i++) {
            unsigned subreg = MRI->getSubReg(reg, i);
            if(getRegSize(subreg) == size) {
                return subreg;
            }
        }
        LogError("TempManager::getSizedSubReg", "No sub register of size %u found for register %u (%s)", size, reg, MRI->getName(reg), MRI->getRegClass(reg).getSize());
        abort();
    }
};

}

#endif
