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
#include <algorithm>
#include <map>
#include <memory>
#include <stdlib.h>

#include "llvm/MC/MCRegister.h"     // for MCRegister
#include "llvm/MC/MCRegisterInfo.h" // for MCRegisterInfo

#include "QBDI/Config.h"
#include "QBDI/State.h"
#include "Engine/LLVMCPU.h"
#include "Patch/InstInfo.h"
#include "Patch/InstMetadata.h"
#include "Patch/Patch.h"
#include "Patch/Register.h"
#include "Patch/TempManager.h"
#include "Patch/Types.h"
#include "Utility/LogSys.h"

#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
#include "Patch/X86_64/TempManagerImpl_X86_64.h"
#elif defined(QBDI_ARCH_ARM)
#include "Patch/ARM/TempManagerImpl_ARM.h"
#elif defined(QBDI_ARCH_AARCH64)
#include "Patch/AARCH64/TempManagerImpl_AARCH64.h"
#endif

#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
// skip RAX as it is very often used implicitly and LLVM
// sometimes don't tell us...
static constexpr unsigned int _QBDI_FIRST_FREE_REGISTER = 1;
#else
static constexpr unsigned int _QBDI_FIRST_FREE_REGISTER = 0;
#endif

namespace QBDI {

TempManager::TempManager(Patch &patch)
    : patch(patch), MRI(patch.llvmcpu->getMRI()), usedRegisterBitField(0),
      lockNewTmpReg(true) {}

bool TempManager::usedRegister(Reg reg) const {
  return (usedRegisterBitField & (((rword)1) << reg.getID())) != 0;
}

bool TempManager::isAllocatedId(unsigned int id) const {
  for (const auto &p : temps) {
    if (p.first == id) {
      return true;
    }
  }
  return false;
}

void TempManager::associatedReg(unsigned int id, Reg reg) {
  QBDI_REQUIRE_ABORT(
      lockNewTmpReg,
      "Cannot allocate new TempRegister after lockTempManager()");

  QBDI_REQUIRE_ABORT(not usedRegister(reg),
                     "Cannot associate twice the same register");
  QBDI_REQUIRE_ABORT(not isAllocatedId(id),
                     "Cannot reassociate an existing register");

  temps.emplace_back(id, reg);
  patch.tempReg.insert(reg);
  usedRegisterBitField |= (((rword)1) << reg.getID());
}

Reg TempManager::getRegForTemp(unsigned int id) {

  // Check if the id is already alocated
  for (const auto &p : temps) {
    if (p.first == id) {
      return p.second;
    }
  }

  QBDI_REQUIRE_ABORT(
      lockNewTmpReg,
      "Cannot allocate new TempRegister after lockTempManager()");

  // try to find a register that doesn't need to be restore
  for (const auto &r : TempManagerUnrestoreGPR) {
    if ((not usedRegister(r)) and patch.regUsage[r.getID()] == 0) {
      associatedReg(id, r);
      return r;
    }
  }

  // Find a free register
  for (unsigned i = _QBDI_FIRST_FREE_REGISTER; i < AVAILABLE_GPR; i++) {
    Reg r = Reg(i);
    if ((not usedRegister(r)) and patch.regUsage[i] == 0) {
      // store it and return it
      associatedReg(id, r);
      return r;
    }
  }

  // Find a register with RegisterUsage::RegisterSaved
  for (unsigned i = _QBDI_FIRST_FREE_REGISTER; i < AVAILABLE_GPR; i++) {
    Reg r = Reg(i);
    if ((not usedRegister(r)) and patch.regUsage[i] != 0 and
        (patch.regUsage[i] & RegisterUsage::RegisterSaved) != 0 and
        (patch.regUsage[i] & RegisterUsage::RegisterSavedScratch) == 0) {
      // store it and return it
      associatedReg(id, r);
      return r;
    }
  }

  QBDI_CRITICAL("No free registers found");
  QBDI_CRITICAL("current tmp reg :");
  for (const auto &p : temps) {
    QBDI_CRITICAL("- Temp({}) = GPR_ID[{}]", p.first, p.second.getID());
  }
  QBDI_ABORT("need Temp({})", id);
}

Reg::Vec TempManager::getUsedRegisters() const {
  Reg::Vec list;
  for (auto &p : temps) {
    list.push_back(p.second);
  }
  return list;
}

size_t TempManager::getUsedRegisterNumber() const { return temps.size(); }

bool TempManager::shouldRestore(Reg r) const {
  return TempManagerUnrestoreGPR.count(r) == 0;
}

RegLLVM TempManager::getSizedSubReg(RegLLVM reg, unsigned size) const {
  if (getRegisterSize(reg) == size) {
    return reg;
  }
  for (unsigned i = 1; i < MRI.getNumSubRegIndices(); i++) {
    RegLLVM subreg = MRI.getSubReg(reg.getValue(), i).id();
    if (subreg != 0 && getRegisterSize(subreg) == size) {
      return subreg;
    }
  }
  QBDI_ABORT("No sub register of size {} found for register {} ({})", size,
             reg.getValue(), MRI.getName(reg.getValue()));
}

} // namespace QBDI
