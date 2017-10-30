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
#ifndef PATCH_H
#define PATCH_H

#include <vector>

#include "Patch/Types.h"
#include "Patch/RelocatableInst.h"

namespace QBDI {

class Patch {
public:

    InstMetadata metadata;
    RelocatableInst::SharedPtrVec insts;

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

    void append(const RelocatableInst::SharedPtrVec v) {
        insts.insert(insts.end(), v.begin(), v.end());
        metadata.patchSize += v.size();
    }

    void prepend(const RelocatableInst::SharedPtrVec v) {
        insts.insert(insts.begin(), v.begin(), v.end());
        metadata.patchSize += v.size();
    } 

    void append(const RelocatableInst::SharedPtr r) {
        insts.push_back(r);
        metadata.patchSize += 1;
    }

    void prepend(const RelocatableInst::SharedPtr r) {
        insts.insert(insts.begin(), r);
        metadata.patchSize += 1;
    } 
};

}

#endif // PATCH_H
