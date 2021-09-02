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
#include <algorithm>
#include <iterator>
#include <utility>

#include "Engine/LLVMCPU.h"
#include "Patch/ExecBlockFlags.h"
#include "Patch/Patch.h"
#include "Patch/Register.h"
#include "Patch/RelocatableInst.h"

#include "llvm/MC/MCInst.h"

namespace QBDI {

Patch::Patch() {
  metadata.patchSize = 0;
  metadata.cpuMode = CPUMode::DEFAULT;
}

Patch::Patch(const llvm::MCInst &inst, rword address, rword instSize,
             const LLVMCPU &llvmcpu) {
  metadata.patchSize = 0;
  metadata.cpuMode = llvmcpu.getCPUMode();
  metadata.inst = inst;
  metadata.address = address;
  metadata.instSize = instSize;
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
  std::move(v.begin(), v.end(), std::back_inserter(insts));
}

void Patch::prepend(RelocatableInst::UniquePtr &&r) {
  insts.insert(insts.begin(), std::forward<RelocatableInst::UniquePtr>(r));
  metadata.patchSize += 1;
}

void Patch::prepend(RelocatableInst::UniquePtrVec v) {
  metadata.patchSize += v.size();
  // front iterator on std::vector may need to move all value at each move
  // use a back_inserter in v and swap the vector at the end
  std::move(insts.begin(), insts.end(), std::back_inserter(v));
  v.swap(insts);
}

} // namespace QBDI
