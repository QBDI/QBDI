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

#include "Patch/InstTransform.h"
#include "Patch/Patch.h"
#include "Patch/PatchGenerator.h"
#include "Patch/RelocatableInst.h"

namespace QBDI {

RelocatableInst::UniquePtrVec
ModifyInstruction::generate(const llvm::MCInst *inst, rword address,
                            rword instSize, const llvm::MCInstrInfo *MCII,
                            TempManager *temp_manager, Patch *toMerge) const {

  llvm::MCInst a(*inst);
  for (const auto &t : transforms) {
    t->transform(a, address, instSize, temp_manager);
  }

  RelocatableInst::UniquePtrVec out;
  if (toMerge != nullptr) {
    RelocatableInst::UniquePtrVec t;
    t.swap(toMerge->insts);
    toMerge->metadata.patchSize = 0;
    append(out, std::move(t));
  }
  out.push_back(NoReloc::unique(std::move(a)));
  return out;
}

RelocatableInst::UniquePtrVec
DoNotInstrument::generate(const llvm::MCInst *inst, rword address,
                          rword instSize, const llvm::MCInstrInfo *MCII,
                          TempManager *temp_manager, Patch *toMerge) const {

  return {};
}

} // namespace QBDI
