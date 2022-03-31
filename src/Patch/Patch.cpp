/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2022 Quarkslab
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
#include <algorithm>
#include <iterator>
#include <utility>

#include "Engine/LLVMCPU.h"
#include "Patch/ExecBlockFlags.h"
#include "Patch/Patch.h"
#include "Patch/PatchGenerator.h"
#include "Patch/Register.h"
#include "Patch/RelocatableInst.h"
#include "Utility/LogSys.h"

#include "llvm/MC/MCInst.h"

namespace QBDI {

Patch::Patch(const llvm::MCInst &inst, rword address, rword instSize,
             const LLVMCPU &llvmcpu)
    : metadata(inst, address, instSize), llvmcpu(&llvmcpu), finalize(false) {
  metadata.patchSize = 0;
  metadata.cpuMode = llvmcpu.getCPUMode();
  metadata.execblockFlags = getExecBlockFlags(inst, llvmcpu);

  regUsage = getUsedGPR(metadata.inst, llvmcpu);
}

Patch::~Patch() = default;

Patch::Patch(Patch &&) = default;

Patch &Patch::operator=(Patch &&) = default;

void Patch::setMerge(bool merge) { metadata.merge = merge; }

void Patch::setModifyPC(bool modifyPC) { metadata.modifyPC = modifyPC; }

void Patch::append(RelocatableInst::UniquePtr &&r) {
  insts.push_back(std::forward<RelocatableInst::UniquePtr>(r));
  metadata.patchSize += 1;
}

void Patch::append(RelocatableInst::UniquePtrVec v) {
  metadata.patchSize += v.size();
  if (insts.empty()) {
    insts.swap(v);
  } else {
    std::move(v.begin(), v.end(), std::back_inserter(insts));
  }
}

void Patch::prepend(RelocatableInst::UniquePtr &&r) {
  insts.insert(insts.begin(), std::forward<RelocatableInst::UniquePtr>(r));
  metadata.patchSize += 1;
}

void Patch::prepend(RelocatableInst::UniquePtrVec v) {
  if (not v.empty()) {
    metadata.patchSize += v.size();
    // front iterator on std::vector may need to move all value at each move
    // use a back_inserter in v and swap the vector at the end
    std::move(insts.begin(), insts.end(), std::back_inserter(v));
    v.swap(insts);
  }
}

void Patch::addInstsPatch(InstPosition position, int priority,
                          std::vector<std::unique_ptr<RelocatableInst>> v) {
  QBDI_REQUIRE(not finalize);

  InstrPatch el{position, priority, std::move(v)};

  auto it = std::upper_bound(instsPatchs.begin(), instsPatchs.end(), el,
                             [](const InstrPatch &a, const InstrPatch &b) {
                               return a.priority > b.priority;
                             });
  instsPatchs.insert(it, std::move(el));
}

void Patch::finalizeInstsPatch() {
  QBDI_REQUIRE(not finalize);
  // avoid to used prepend
  // The begin of the patch is a target for the prologue.
  std::vector<std::unique_ptr<RelocatableInst>> prePatch =
      TargetPrologue().generate(this, nullptr, nullptr);

  // Add PREINST callback by priority order
  for (InstrPatch &el : instsPatchs) {
    if (el.position == PREINST) {
      std::vector<std::unique_ptr<RelocatableInst>> v;
      v.swap(el.insts);
      std::move(v.begin(), v.end(), std::back_inserter(prePatch));
    } else if (el.position == POSTINST) {
      continue;
    } else {
      QBDI_ERROR("Invalid position 0x{:x}", el.position);
      abort();
    }
  }

  // add the tag RelocTagPatchBegin
  prePatch.push_back(RelocTag::unique(RelocTagPatchBegin));
  // prepend the current RelocInst to the Patch
  prepend(std::move(prePatch));
  // add the tag RelocTagPatchEnd
  append(RelocTag::unique(RelocTagPatchEnd));

  // append TargetPrologue for SKIP_INST
  append(TargetPrologue().generate(this, nullptr, nullptr));

  // Add POSTINST callback by priority order
  for (InstrPatch &el : instsPatchs) {
    if (el.position == PREINST) {
      continue;
    } else if (el.position == POSTINST) {
      std::vector<std::unique_ptr<RelocatableInst>> v;
      v.swap(el.insts);
      append(std::move(v));
    } else {
      QBDI_ERROR("Invalid position 0x{:x}", el.position);
      abort();
    }
  }

  instsPatchs.clear();
  finalize = true;
}

} // namespace QBDI
