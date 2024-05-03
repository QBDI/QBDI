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
#ifndef TEMPMANAGER_H
#define TEMPMANAGER_H

#include <memory>
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
class RelocatableInst;

// Helper classes

class TempManager {

  Patch &patch;
  const llvm::MCRegisterInfo &MRI;

  // list of TmpRegister
  std::vector<std::pair<unsigned int, Reg>> temps;
  // bitfield of UsedTempReg
  rword usedRegisterBitField;
  // allowed to register a new TmpRegister
  bool lockNewTmpReg;

public:
  TempManager(Patch &patch);

  Reg getRegForTemp(unsigned int id);

  // allocate a new TmpRegiter
  void associatedReg(unsigned int id, Reg reg);

  Reg::Vec getUsedRegisters() const;

  void lockTempManager() { lockNewTmpReg = false; }

  bool shouldRestore(Reg r) const;

  bool usedRegister(Reg reg) const;

  bool isAllocatedId(unsigned int id) const;

  size_t getUsedRegisterNumber() const;

  RegLLVM getSizedSubReg(RegLLVM reg, unsigned size) const;

  const Patch &getPatch() const { return patch; };

  void generateSaveRestoreInstructions(
      unsigned unrestoredRegNum,
      std::vector<std::unique_ptr<RelocatableInst>> &saveInst,
      std::vector<std::unique_ptr<RelocatableInst>> &restoreInst,
      Reg::Vec &unrestoredReg) const;
};

} // namespace QBDI

#endif
