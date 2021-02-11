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
#ifndef INSTMETADATA_H
#define INSTMETADATA_H

#include "llvm/MC/MCInst.h"

#include "Utility/InstAnalysis_prive.h"

#include "State.h"

namespace QBDI {

class InstMetadata {
public:
    llvm::MCInst inst;
    rword address;
    uint32_t instSize;
    uint32_t patchSize;
    bool modifyPC;
    bool merge;
    uint8_t execblockFlags;
    mutable InstAnalysisPtr analysis;

    inline rword endAddress() const {
        return address + instSize;
    }

    inline InstMetadata lightCopy() const {
        return {inst, address, instSize, patchSize, modifyPC, merge, execblockFlags, nullptr};
    }
};

}

#endif // INSTMETADATA_H
