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
#ifndef RELOCATABLEINST_ARM_H
#define RELOCATABLEINST_ARM_H

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

class LoadShadowCC : public AutoClone<RelocatableInst, LoadShadowCC> {
  RegLLVM reg;
  uint16_t tag;
  unsigned cond;

public:
  LoadShadowCC(RegLLVM reg, Shadow tag, unsigned cond)
      : reg(reg), tag(tag.getTag()), cond(cond) {}

  // Load a value from the last shadow with the given tag
  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override;
};

class StoreShadowCC : public AutoClone<RelocatableInst, StoreShadowCC> {
  RegLLVM reg;
  uint16_t tag;
  bool create;
  unsigned cond;

public:
  StoreShadowCC(RegLLVM reg, Shadow tag, bool create, unsigned cond)
      : reg(reg), tag(tag.getTag()), create(create), cond(cond) {}

  // Store a value to a shadow
  // if create, the shadow is create in the ExecBlock with the given tag
  // otherwise, the last shadow with this tag is used
  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override;
};

class LoadDataBlockCC : public AutoClone<RelocatableInst, LoadDataBlockCC> {
  RegLLVM reg;
  int64_t offset;
  unsigned cond;

public:
  LoadDataBlockCC(RegLLVM reg, int64_t offset, unsigned cond)
      : reg(reg), offset(offset), cond(cond) {}

  // Load a value from the specified offset of the datablock
  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override;
};

class StoreDataBlockCC : public AutoClone<RelocatableInst, StoreDataBlockCC> {
  RegLLVM reg;
  int64_t offset;
  unsigned cond;

public:
  StoreDataBlockCC(RegLLVM reg, int64_t offset, unsigned cond)
      : reg(reg), offset(offset), cond(cond) {}

  // Store a value to the specified offset of the datablock
  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override;
};

class MovRegCC : public AutoClone<RelocatableInst, MovRegCC> {
  RegLLVM dst;
  RegLLVM src;
  unsigned cond;

public:
  MovRegCC(RegLLVM dst, RegLLVM src, unsigned cond)
      : dst(dst), src(src), cond(cond) {}

  // Move a value from a register to another
  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override;
};

class LoadImmCC : public AutoClone<RelocatableInst, LoadImmCC> {
  RegLLVM reg;
  Constant imm;
  unsigned cond;

public:
  LoadImmCC(RegLLVM reg, Constant imm, unsigned cond)
      : reg(reg), imm(imm), cond(cond) {}

  // Set the register to this value
  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override;
};

class EpilogueBranch : public AutoClone<RelocatableInst, EpilogueBranch> {

public:
  EpilogueBranch() {}

  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override;
};

class SetSREpilogue : public AutoClone<RelocatableInst, SetSREpilogue> {

public:
  SetSREpilogue() {}

  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override;
};

class SRBranch : public AutoClone<RelocatableInst, SRBranch> {

public:
  SRBranch() {}

  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override;
};

class DataBlockAddress : public AutoClone<RelocatableInst, DataBlockAddress> {
  RegLLVM reg;
  bool setScratchRegister;

public:
  /**
   * Set DataBlock approximative address in reg
   */
  DataBlockAddress(RegLLVM reg) : reg(reg), setScratchRegister(false) {}

  /**
   * Set DataBlock approximative address in scratch register
   */
  DataBlockAddress() : reg(0), setScratchRegister(true) {}

  /**
   * ADR [reg/sr], imm
   *
   * NOTE for ARM mode:
   *
   * ADR instruction may not set the register at the begin of the
   * datablock, as the encoding of the immediate don't allows any value.
   *
   * This relocatableInst must be follow by:
   *  - BIC reg, reg, 0xff
   * Used SetDataBlockAddress instead
   */
  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override;
};

class RelativeAddress : public AutoClone<RelocatableInst, RelativeAddress> {
  RegLLVM reg;
  sword offset;

public:
  /**
   * Set DataBlock approximative address in reg
   */
  RelativeAddress(RegLLVM reg, sword offset) : reg(reg), offset(offset) {}

  /**
   * ADR [reg/sr], imm
   */
  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override;
};

class MovToSavedScratchReg
    : public AutoClone<RelocatableInst, MovToSavedScratchReg> {
  RegLLVM reg;
  Reg candidateSC;
  unsigned cond;

public:
  /**
   * if candidateSC is the Scratch register, move the value hold by reg in the
   * datablock
   * else, move the value in candidateSC
   */
  MovToSavedScratchReg(RegLLVM reg, Reg candidateSC, unsigned cond)
      : reg(reg), candidateSC(candidateSC), cond(cond) {}

  /**
   */
  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override;
};

class MovFromSavedScratchReg
    : public AutoClone<RelocatableInst, MovFromSavedScratchReg> {
  RegLLVM reg;
  Reg candidateSC;
  unsigned cond;

public:
  /**
   * if candidateSC is the Scratch register, load the real value of the scratch
   * register from the datablock to the reg
   * else, move the value from candidateSC to reg
   */
  MovFromSavedScratchReg(Reg candidateSC, RegLLVM reg, unsigned cond)
      : reg(reg), candidateSC(candidateSC), cond(cond) {}

  /**
   */
  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override;
};

// class RelocateStratchRegister : public AutoClone<RelocatableInst,
// RelocateStratchRegister> {
//   llvm::MCInst inst;
//   unsigned int replaceReg;
//
// public:
//   RelocateStratchRegister(llvm::MCInst&& inst, unsigned int replaceReg) :
//   inst(inst), replaceReg(replaceReg) {}
//
//   llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;
// };

} // namespace QBDI

#endif
