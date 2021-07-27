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

#include "Patch/PatchRule.h"
#include "Patch/ExecBlockFlags.h"
#include "Patch/PatchGenerator.h"

namespace QBDI {

PatchRule::PatchRule(PatchCondition::UniquePtr &&condition,
                     std::vector<std::unique_ptr<PatchGenerator>> &&generators)
    : condition(std::move(condition)), generators(std::move(generators)){};

PatchRule::~PatchRule() = default;

PatchRule::PatchRule(PatchRule &&) = default;

Patch PatchRule::generate(const llvm::MCInst &inst, rword address,
                          rword instSize, const LLVMCPU &llvmcpu,
                          Patch *toMerge) const {

  Patch patch(inst, address, instSize);
  patch.metadata.execblockFlags = getExecBlockFlags(inst, llvmcpu);
  if (toMerge != nullptr) {
    patch.metadata.address = toMerge->metadata.address;
    patch.metadata.instSize += toMerge->metadata.instSize;
    patch.metadata.execblockFlags |= toMerge->metadata.execblockFlags;
  }
  TempManager temp_manager(inst, llvmcpu);
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
    patch.prepend(SaveReg(used_registers[i], Offset(used_registers[i])));
  }

  for (unsigned int i = 0; i < used_registers.size(); i++) {
    patch.append(LoadReg(used_registers[i], Offset(used_registers[i])));
  }

  return patch;
}

} // namespace QBDI
