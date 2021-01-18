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

#include "llvm/MC/MCInst.h"

#include "Patch/InstTransform.h"
#include "Patch/Patch.h"
#include "Patch/PatchGenerator.h"
#include "Patch/RelocatableInst.h"

namespace QBDI {

RelocatableInst::SharedPtrVec ModifyInstruction::generate(const llvm::MCInst *inst,
    rword address, rword instSize, const llvm::MCInstrInfo* MCII, TempManager *temp_manager, const Patch *toMerge) const {

    llvm::MCInst a(*inst);
    for(auto t: transforms) {
        t->transform(a, address, instSize, temp_manager);
    }

    RelocatableInst::SharedPtrVec out;
    if(toMerge != nullptr) {
        append(out, toMerge->insts);
    }
    out.push_back(NoReloc(std::move(a)));
    return out;
}
}
