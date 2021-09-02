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
#ifndef TEMPMANAGER_H
#define TEMPMANAGER_H

#include <set>
#include <stddef.h>
#include <utility>
#include <vector>

#include "Patch/Types.h"

namespace llvm {
class MCInstrInfo;
class MCRegisterInfo;
} // namespace llvm

namespace QBDI {
class LLVMCPU;
class Patch;

// Helper classes

class TempManager {

  std::vector<std::pair<unsigned int, unsigned int>> temps;
  Patch &patch;
  bool allowInstRegister;

  // list of registers that doesn't need to be restore
  static const std::set<Reg> unrestoreGPR;

public:
  const llvm::MCInstrInfo &MCII;
  const llvm::MCRegisterInfo &MRI;

  TempManager(Patch &patch, const LLVMCPU &llvmcpu,
              bool allowInstRegister = false);

  Reg getRegForTemp(unsigned int id);

  Reg::Vec getUsedRegisters() const;

  bool shouldRestore(const Reg &r) const { return unrestoreGPR.count(r) == 0; }

  size_t getUsedRegisterNumber() const;

  unsigned getSizedSubReg(unsigned reg, unsigned size) const;

  const Patch &getPatch() const { return patch; };
};

} // namespace QBDI

#endif
