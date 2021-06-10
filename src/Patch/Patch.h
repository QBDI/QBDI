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
#ifndef PATCH_H
#define PATCH_H

#include <vector>
#include <iterator>

#include "llvm/MC/MCInst.h"

#include "Patch/InstMetadata.h"
#include "Patch/RelocatableInst.h"
#include "Patch/Types.h"

#include "QBDI/State.h"

namespace QBDI {

class Patch {
public:

    InstMetadata metadata;
    RelocatableInst::UniquePtrVec insts;

    using Vec = std::vector<Patch>;

    Patch() {
        metadata.patchSize = 0;
    }

    Patch(llvm::MCInst inst, rword address, rword instSize) {
        metadata.patchSize = 0;
        setInst(inst, address, instSize);
    }

    void setMerge(bool merge) {
        metadata.merge = merge;
    }

    void setModifyPC(bool modifyPC) {
        metadata.modifyPC = modifyPC;
    }

    void setInst(llvm::MCInst inst, rword address, rword instSize) {
        metadata.inst = inst;
        metadata.address = address;
        metadata.instSize = instSize;
    }

    void append(RelocatableInst::UniquePtrVec v) {
        metadata.patchSize += v.size();
        std::move(v.begin(), v.end(), std::back_inserter(insts));
    }

    void prepend(RelocatableInst::UniquePtrVec v) {
        metadata.patchSize += v.size();
        // front iterator on std::vector may need to move all value at each move
        // use a back_inserter in v and swap the vector at the end
        std::move(insts.begin(), insts.end(), std::back_inserter(v));
        v.swap(insts);
    }

    void append(RelocatableInst::UniquePtr&& r) {
        insts.push_back(std::forward<RelocatableInst::UniquePtr>(r));
        metadata.patchSize += 1;
    }

    void prepend(RelocatableInst::UniquePtr&& r) {
        insts.insert(insts.begin(), std::forward<RelocatableInst::UniquePtr>(r));
        metadata.patchSize += 1;
    }
};

}

#endif // PATCH_H
