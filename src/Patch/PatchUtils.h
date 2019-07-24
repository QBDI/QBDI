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

#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "Patch/Types.h"

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

    Reg getRegForTemp(unsigned int id);

    Reg::Vec getUsedRegisters();

    size_t getUsedRegisterNumber();

    unsigned getRegSize(unsigned reg);

    unsigned getSizedSubReg(unsigned reg, unsigned size);
};

}

#endif
