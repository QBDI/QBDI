/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2023 Quarkslab
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
#include <map>
#include <utility>

#include "QBDI/Bitmask.h"
#include "Patch/InstMetadata.h"
#include "Patch/PatchCondition.h"
#include "Patch/PatchGenerator.h"
#include "Patch/PatchRule.h"
#include "Patch/Register.h"
#include "Patch/RelocatableInst.h"
#include "Patch/TempManager.h"
#include "Patch/Types.h"

namespace QBDI {

PatchRule::PatchRule(PatchCondition::UniquePtr &&condition,
                     std::vector<std::unique_ptr<PatchGenerator>> &&generators)
    : condition(std::move(condition)), generators(std::move(generators)){};

PatchRule::~PatchRule() = default;

PatchRule::PatchRule(PatchRule &&) = default;

bool PatchRule::canBeApplied(const Patch &patch, const LLVMCPU &llvmcpu) const {
  return condition->test(patch, llvmcpu);
}

void PatchRule::apply(Patch &patch, const LLVMCPU &llvmcpu) const {

  TempManager temp_manager(patch);
  bool modifyPC = false;

  patch.patchGenFlags.emplace_back(patch.insts.size(),
                                   PatchGeneratorFlags::PatchRuleBegin);
  for (const auto &g : generators) {
    if (g->getPreFlags() != PatchGeneratorFlags::None) {
      patch.patchGenFlags.emplace_back(patch.insts.size(), g->getPreFlags());
    }
    patch.append(g->generate(patch, temp_manager));
    if (g->getPostFlags() != PatchGeneratorFlags::None) {
      patch.patchGenFlags.emplace_back(patch.insts.size(), g->getPostFlags());
    }
    modifyPC |= g->modifyPC();
  }
  patch.patchGenFlags.emplace_back(patch.insts.size(),
                                   PatchGeneratorFlags::PatchRuleEnd);
  patch.setModifyPC(modifyPC);

  RelocatableInst::UniquePtrVec saveReg, restoreReg;
  Reg::Vec unrestoredReg;

  temp_manager.generateSaveRestoreInstructions(0, saveReg, restoreReg,
                                               unrestoredReg);
  patch.prepend(std::move(saveReg));
  patch.append(std::move(restoreReg));
}

} // namespace QBDI
