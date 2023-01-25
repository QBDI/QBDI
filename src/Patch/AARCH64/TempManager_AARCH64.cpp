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

#include <set>
#include "Patch/AARCH64/RelocatableInst_AARCH64.h"
#include "Patch/Patch.h"
#include "Patch/PatchGenerator.h"
#include "Patch/RelocatableInst.h"
#include "Patch/TempManager.h"
#include "Patch/Types.h"

namespace QBDI {

void TempManager::generateSaveRestoreInstructions(
    unsigned unrestoredRegNum, RelocatableInst::UniquePtrVec &saveInst,
    RelocatableInst::UniquePtrVec &restoreInst, Reg::Vec &unrestoredReg) const {

  saveInst.clear();
  restoreInst.clear();
  unrestoredReg.clear();

  Reg::Vec usedRegisters = getUsedRegisters();

  std::sort(usedRegisters.begin(), usedRegisters.end(),
            [](const Reg &a, const Reg &b) { return a.getID() < b.getID(); });

  for (Reg r : usedRegisters) {
    if (not shouldRestore(r)) {
      unrestoredReg.push_back(r);
    }
  }

  std::vector<std::pair<Reg, Reg>> pairRegister;

  for (unsigned i = 0; i < usedRegisters.size(); i++) {
    Reg r = usedRegisters[i];
    if (shouldRestore(r)) {
      // found a pair register that we may optimised with LDP/STP
      if (i + 1 < usedRegisters.size() and
          shouldRestore(usedRegisters[i + 1]) and
          r.getID() + 1 == usedRegisters[i + 1].getID()) {
        saveInst.push_back(
            StoreDataBlockX2::unique(r, usedRegisters[i + 1], Offset(r)));
        pairRegister.emplace_back(r, usedRegisters[i + 1]);
        i++;
      } else if (unrestoredReg.size() < unrestoredRegNum) {
        append(saveInst, SaveReg(r, Offset(r)).genReloc(*patch.llvmcpu));
        unrestoredReg.push_back(r);
      } else {
        append(saveInst, SaveReg(r, Offset(r)).genReloc(*patch.llvmcpu));
        append(restoreInst, LoadReg(r, Offset(r)).genReloc(*patch.llvmcpu));
      }
    }
  }

  for (const auto &p : pairRegister) {
    if (unrestoredReg.size() < unrestoredRegNum) {
      unrestoredReg.push_back(p.first);
      if (unrestoredReg.size() < unrestoredRegNum) {
        unrestoredReg.push_back(p.second);
      } else {
        append(restoreInst,
               LoadReg(p.second, Offset(p.second)).genReloc(*patch.llvmcpu));
      }
    } else {
      restoreInst.push_back(
          LoadDataBlockX2::unique(p.first, p.second, Offset(p.first)));
    }
  }
}

} // namespace QBDI
