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

#include "Patch/Types.h"

namespace llvm {
  class MCInst;
  class MCInstrInfo;
  class MCRegisterInfo;
}

namespace QBDI {

class RelocatableInst;
class PatchGenerator;

// Helper template

template<typename T, typename U> class AutoAlloc {
public:

    operator std::shared_ptr<T>() {
        return std::shared_ptr<T>(new U(*static_cast<U*>(this)));
    }

    operator std::unique_ptr<T>() {
        return std::make_unique<U>(*static_cast<U*>(this));
    }
};

template<class T, class U>
void _conv_unique(std::vector<std::unique_ptr<T>>& vec, U&& u) {
    vec.push_back(std::make_unique<U>(std::forward<U>(u)));
}

template<class T, class U, class ... Args>
void _conv_unique(std::vector<std::unique_ptr<T>>& vec, U&& u, Args... args) {
    vec.push_back(std::make_unique<U>(std::forward<U>(u)));
    _conv_unique<T>(vec, std::forward<Args>(args)...);
}

template<class T, class ... Args>
std::vector<std::unique_ptr<T>> conv_unique(Args... args) {
    std::vector<std::unique_ptr<T>> vec;
    _conv_unique<T>(vec, std::forward<Args>(args)...);
    return vec;
}

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
    const llvm::MCInstrInfo* MCII;
    const llvm::MCRegisterInfo* MRI;
    bool allowInstRegister;

public:

    TempManager(const llvm::MCInst * inst, const llvm::MCInstrInfo * MCII,
        const llvm::MCRegisterInfo * MRI, bool allowInstRegister=false)
        : inst(inst), MCII(MCII), MRI(MRI), allowInstRegister(allowInstRegister) {};

    Reg getRegForTemp(unsigned int id);

    Reg::Vec getUsedRegisters() const;

    size_t getUsedRegisterNumber() const;

    unsigned getSizedSubReg(unsigned reg, unsigned size) const;
};

}

#endif
