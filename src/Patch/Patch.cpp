/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2024 Quarkslab
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
#include "Patch/Types.h"
#include "Utility/LogSys.h"

#include "llvm/MC/MCInst.h"

namespace QBDI {

Patch::Patch(const llvm::MCInst &inst, rword address, uint32_t instSize,
             const LLVMCPU &llvmcpu)
    : metadata(inst, address, instSize, llvmcpu.getCPUMode(),
               getExecBlockFlags(inst, llvmcpu)),
      regUsage({RegisterUnused}), llvmcpu(&llvmcpu), finalize(false) {
  metadata.patchSize = 0;

  getUsedGPR(metadata.inst, llvmcpu, regUsage, regUsageExtra);
}

Patch::~Patch() = default;

Patch::Patch(Patch &&) = default;

Patch &Patch::operator=(Patch &&) = default;

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
  patchGenFlagsOffset += 1;
}

void Patch::prepend(RelocatableInst::UniquePtrVec v) {
  if (not v.empty()) {
    metadata.patchSize += v.size();
    patchGenFlagsOffset += v.size();
    // front iterator on std::vector may need to move all value at each move
    // use a back_inserter in v and swap the vector at the end
    std::move(insts.begin(), insts.end(), std::back_inserter(v));
    v.swap(insts);
  }
}

void Patch::insertAt(unsigned position, RelocatableInst::UniquePtrVec v) {
  if (not v.empty()) {
    metadata.patchSize += v.size();
    for (auto &p : patchGenFlags) {
      if (p.first >= position) {
        p.first += v.size();
      }
    }
    std::move(
        v.begin(), v.end(),
        std::inserter(insts, insts.begin() + position + patchGenFlagsOffset));
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
  RelocatableInst::UniquePtrVec prePatch;
  // add the tag RelocTagPatchInstBegin
  prePatch.push_back(RelocTag::unique(RelocTagPatchBegin));

  // The begin of the patch is a target for the prologue.
  auto v = TargetPrologue().genReloc(*this);
  std::move(v.begin(), v.end(), std::back_inserter(prePatch));

  // Add PREINST callback by priority order
  for (InstrPatch &el : instsPatchs) {
    if (el.position == PREINST) {
      RelocatableInst::UniquePtrVec v;
      v.swap(el.insts);
      std::move(v.begin(), v.end(), std::back_inserter(prePatch));
    } else if (el.position == POSTINST) {
      continue;
    } else {
      QBDI_ABORT("Invalid position 0x{:x} {}", el.position, *this);
    }
  }

  // add the tag RelocTagPatchInstBegin
  prePatch.push_back(RelocTag::unique(RelocTagPatchInstBegin));
  // prepend the current RelocInst to the Patch
  prepend(std::move(prePatch));
  // add the tag RelocTagPatchInstEnd
  append(RelocTag::unique(RelocTagPatchInstEnd));

  // append TargetPrologue for SKIP_INST
  append(TargetPrologue().genReloc(*this));

  // Add POSTINST callback by priority order
  for (InstrPatch &el : instsPatchs) {
    if (el.position == PREINST) {
      continue;
    } else if (el.position == POSTINST) {
      RelocatableInst::UniquePtrVec v;
      v.swap(el.insts);
      append(std::move(v));
    } else {
      QBDI_ABORT("Invalid position 0x{:x} {}", el.position, *this);
    }
  }

  instsPatchs.clear();
  finalize = true;
}

} // namespace QBDI
