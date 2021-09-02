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

bool PatchRule::canBeApplied(const llvm::MCInst &inst, rword address,
                             rword instSize, const LLVMCPU &llvmcpu) const {
  return condition->test(inst, address, instSize, llvmcpu);
}

Patch PatchRule::generate(const llvm::MCInst &inst, rword address,
                          rword instSize, const LLVMCPU &llvmcpu,
                          Patch *toMerge) const {

  Patch patch(inst, address, instSize, llvmcpu);
  if (toMerge != nullptr) {
    patch.metadata.address = toMerge->metadata.address;
    patch.metadata.instSize += toMerge->metadata.instSize;
    patch.metadata.execblockFlags |= toMerge->metadata.execblockFlags;
    for (const auto &e : toMerge->regUsage) {
      auto e2 = patch.regUsage.find(e.first);
      if (e2 != patch.regUsage.end()) {
        patch.regUsage[e.first] = e2->second | e.second;
      } else {
        patch.regUsage.insert(e);
      }
    }
  }

  TempManager temp_manager(patch, llvmcpu);
  bool modifyPC = false;
  bool merge = false;

  for (const auto &g : generators) {
    patch.append(g->generate(&inst, address, instSize, &temp_manager, toMerge));
    modifyPC |= g->modifyPC();
    merge |= g->doNotInstrument();
  }
  patch.setMerge(merge);
  patch.setModifyPC(modifyPC);

  Reg::Vec used_registers = temp_manager.getUsedRegisters();

  for (unsigned int i = 0; i < used_registers.size(); i++) {
    if (temp_manager.shouldRestore(used_registers[i])) {
      patch.prepend(SaveReg(used_registers[i], Offset(used_registers[i])));
      patch.append(LoadReg(used_registers[i], Offset(used_registers[i])));
    }
  }

  return patch;
}

} // namespace QBDI
