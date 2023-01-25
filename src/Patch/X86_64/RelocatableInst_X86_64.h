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
#ifndef RELOCATABLEINST_X86_64_H
#define RELOCATABLEINST_X86_64_H

#include <memory>
#include <utility>

#include "llvm/MC/MCInst.h"

#include "QBDI/Config.h"
#include "QBDI/State.h"
#include "Patch/PatchUtils.h"
#include "Patch/RelocatableInst.h"
#include "Patch/Types.h"

namespace QBDI {
class ExecBlock;

class NoRelocSized : public AutoClone<RelocatableInst, NoRelocSized> {
  llvm::MCInst inst;
  int size;

public:
  NoRelocSized(llvm::MCInst &&inst, int size)
      : inst(std::forward<llvm::MCInst>(inst)), size(size) {}

  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override {
    return inst;
  }

  int getSize(const LLVMCPU &llvmcpu) const override { return size; };
};

class EpilogueJump : public AutoClone<RelocatableInst, EpilogueJump> {

public:
  EpilogueJump() : AutoClone<RelocatableInst, EpilogueJump>() {}

  // Set an operand to epilogueOffset + offset
  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override;
};

class SetRegtoPCRel : public AutoClone<RelocatableInst, SetRegtoPCRel> {
  Reg reg;
  rword offset;

public:
  SetRegtoPCRel(Reg reg, rword offset)
      : AutoClone<RelocatableInst, SetRegtoPCRel>(), reg(reg), offset(offset) {}

  // set reg to currentPC + offset
  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override;
};

class DataBlockRel : public AutoClone<RelocatableInst, DataBlockRel> {
  llvm::MCInst inst;
  unsigned int opn;
  rword offset;
  int size;

public:
  DataBlockRel(llvm::MCInst &&inst, unsigned int opn, rword offset, int size)
      : AutoClone<RelocatableInst, DataBlockRel>(),
        inst(std::forward<llvm::MCInst>(inst)), opn(opn), offset(offset),
        size(size) {}

  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override { return size; }
};

class DataBlockAbsRel : public AutoClone<RelocatableInst, DataBlockAbsRel> {
  llvm::MCInst inst;
  unsigned int opn;
  rword offset;
  int size;

public:
  DataBlockAbsRel(llvm::MCInst &&inst, unsigned int opn, rword offset, int size)
      : AutoClone<RelocatableInst, DataBlockAbsRel>(),
        inst(std::forward<llvm::MCInst>(inst)), opn(opn), offset(offset),
        size(size) {}

  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override { return size; }
};

} // namespace QBDI

#endif
