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
#include "Utility/LogSys.h"

#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
// skip RAX as it is very often used implicitly and LLVM
// sometimes don't tell us...
static constexpr unsigned int _QBDI_FIRST_FREE_REGISTER = 1;
#else
static constexpr unsigned int _QBDI_FIRST_FREE_REGISTER = 0;
#endif

namespace QBDI {

TempManager::TempManager(Patch &patch, bool allowInstRegister)
    : patch(patch), MRI(patch.llvmcpu->getMRI()),
      allowInstRegister(allowInstRegister) {}

Reg TempManager::getRegForTemp(unsigned int id) {

  // Check if the id is already alocated
  for (const auto &p : temps) {
    if (p.first == id) {
      return Reg(p.second);
    }
  }

  // try to find a register that doesn't need to be restore
  for (const auto &r : unrestoreGPR) {
    if (patch.regUsage.count(r) != 0) {
      continue;
    }
    bool freeReg = true;
    for (const auto &p : temps) {
      if (p.second == r.getID()) {
        freeReg = false;
        break;
      }
    }
    if (freeReg) {
      temps.emplace_back(id, r.getID());
      patch.tempReg.insert(r);
      return r;
    }
  }

  // Start from the last free register found (or default)
  unsigned int i;
  if (temps.size() > 0) {
    if (unrestoreGPR.count(temps.back().second) == 0) {
      i = temps.back().second + 1;
    } else {
      i = _QBDI_FIRST_FREE_REGISTER;
    }
  } else {
    i = _QBDI_FIRST_FREE_REGISTER;
  }

  // Find a free register
  for (; i < AVAILABLE_GPR; i++) {
    if (patch.regUsage.count(GPR_ID[i]) == 0) {
      // store it and return it
      temps.emplace_back(id, i);
      patch.tempReg.insert(GPR_ID[i]);
      return Reg(i);
    }
  }

  // bypass for pusha and popa. MemoryAccess will not work on theses instruction
  if (allowInstRegister and useAllRegisters(patch.metadata.inst)) {
    if (temps.size() > 0) {
      i = temps.back().second + 1;
    } else {
      i = _QBDI_FIRST_FREE_REGISTER;
    }
    // store it and return it
    if (i < AVAILABLE_GPR) {
      temps.emplace_back(id, i);
      patch.tempReg.insert(GPR_ID[i]);
      return Reg(i);
    }
  }
  QBDI_ERROR("No free registers found");
  abort();
}

Reg::Vec TempManager::getUsedRegisters() const {
  Reg::Vec list;
  for (auto p : temps) {
    list.push_back(Reg(p.second));
  }
  return list;
}

size_t TempManager::getUsedRegisterNumber() const { return temps.size(); }

unsigned TempManager::getSizedSubReg(unsigned reg, unsigned size) const {
  if (getRegisterSize(reg) == size) {
    return reg;
  }
  for (unsigned i = 1; i < MRI.getNumSubRegIndices(); i++) {
    unsigned subreg = MRI.getSubReg(reg, i);
    if (subreg != 0 && getRegisterSize(subreg) == size) {
      return subreg;
    }
  }
  QBDI_ERROR("No sub register of size {} found for register {} ({})", size, reg,
             MRI.getName(reg));
  abort();
}

} // namespace QBDI
