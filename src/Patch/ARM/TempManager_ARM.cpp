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

#include <set>
#include "Patch/Patch.h"
#include "Patch/PatchGenerator.h"
#include "Patch/RelocatableInst.h"
#include "Patch/TempManager.h"
#include "Patch/Types.h"
#include "Utility/LogSys.h"

namespace QBDI {

void TempManager::generateSaveRestoreInstructions(
    unsigned unrestoredRegNum, RelocatableInst::UniquePtrVec &saveInst,
    RelocatableInst::UniquePtrVec &restoreInst, Reg::Vec &unrestoredReg) const {

  saveInst.clear();
  restoreInst.clear();
  unrestoredReg.clear();

  Reg::Vec usedRegisters = getUsedRegisters();

  for (Reg r : usedRegisters) {
    if (shouldRestore(r)) {
      append(saveInst, SaveReg(r, Offset(r)).genReloc(*patch.llvmcpu));
      if (unrestoredReg.size() < unrestoredRegNum) {
        unrestoredReg.push_back(r);
      } else {
        append(restoreInst, LoadReg(r, Offset(r)).genReloc(*patch.llvmcpu));
      }
    } else {
      unrestoredReg.push_back(r);
    }
  }
}

void allocateConsecutiveTempRegister(TempManager &temp_manager, Temp temp1,
                                     Temp temp2) {
  const Patch &patch = temp_manager.getPatch();

  QBDI_REQUIRE_ABORT_PATCH(not temp_manager.isAllocatedId(temp1), patch,
                           "Temp is already register");
  QBDI_REQUIRE_ABORT_PATCH(not temp_manager.isAllocatedId(temp2), patch,
                           "Temp is already register");

  for (int i = 0; i < 12; i += 2) {
    if (temp_manager.usedRegister(Reg(i)) or
        temp_manager.usedRegister(Reg(i + 1)) or patch.regUsage[i] != 0 or
        patch.regUsage[i + 1] != 0) {
      continue;
    }
    temp_manager.associatedReg(temp1, Reg(i));
    temp_manager.associatedReg(temp2, Reg(i + 1));
    return;
  }
  QBDI_ABORT_PATCH(patch, "Fail to allocate consecutive TempRegister");
}

} // namespace QBDI
