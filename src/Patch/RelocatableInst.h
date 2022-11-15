/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2022 Quarkslab
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
#ifndef RELOCATABLEINST_H
#define RELOCATABLEINST_H

#include <memory>
#include <vector>

#include "llvm/MC/MCInst.h"

#include "QBDI/State.h"
#include "Patch/InstInfo.h"
#include "Patch/PatchUtils.h"
#include "Patch/Types.h"

namespace QBDI {
class ExecBlock;
class LLVMCPU;

class RelocatableInst {
public:
  using UniquePtr = std::unique_ptr<RelocatableInst>;
  using UniquePtrVec = std::vector<std::unique_ptr<RelocatableInst>>;

  RelocatableInst() {}

  virtual RelocatableInstTag getTag() const { return RelocInst; };

  virtual std::unique_ptr<RelocatableInst> clone() const = 0;

  virtual int getSize(const LLVMCPU &llvmcpu) const = 0;

  virtual llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const = 0;

  virtual ~RelocatableInst() = default;
};

static inline int getUniquePtrVecSize(const RelocatableInst::UniquePtrVec &vec,
                                      const LLVMCPU &llvmcpu) {
  int size = 0;
  for (const auto &r : vec) {
    size += r->getSize(llvmcpu);
  }
  return size;
}

class NoReloc : public AutoClone<RelocatableInst, NoReloc> {
  llvm::MCInst inst;

public:
  NoReloc(llvm::MCInst &&inst) : inst(std::forward<llvm::MCInst>(inst)) {}

  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override {
    return inst;
  }

  int getSize(const LLVMCPU &llvmcpu) const override {
    return getInstSize(inst, llvmcpu);
  };
};

// Generic RelocatableInst that must be implemented by each target

class RelocTag : public AutoClone<RelocatableInst, RelocTag> {
  RelocatableInstTag tag;

public:
  RelocTag(RelocatableInstTag t) : tag(t) {}

  RelocatableInstTag getTag() const override { return tag; }

  // The Execblock must skip the generation if a RelocatableInst doesn't return
  // RelocInst. Generate a NOP and a log Error.
  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override { return 0; }
};

class LoadShadow : public AutoClone<RelocatableInst, LoadShadow> {
  RegLLVM reg;
  uint16_t tag;

public:
  LoadShadow(RegLLVM reg, Shadow tag)
      : AutoClone<RelocatableInst, LoadShadow>(), reg(reg), tag(tag.getTag()) {}

  // Load a value from the last shadow with the given tag
  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override;
};

class StoreShadow : public AutoClone<RelocatableInst, StoreShadow> {
  RegLLVM reg;
  uint16_t tag;
  bool create;

public:
  StoreShadow(RegLLVM reg, Shadow tag, bool create)
      : AutoClone<RelocatableInst, StoreShadow>(), reg(reg), tag(tag.getTag()),
        create(create) {}

  // Store a value to a shadow
  // if create, the shadow is create in the ExecBlock with the given tag
  // otherwise, the last shadow with this tag is used
  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override;
};

class LoadDataBlock : public AutoClone<RelocatableInst, LoadDataBlock> {
  RegLLVM reg;
  int64_t offset;

public:
  LoadDataBlock(RegLLVM reg, int64_t offset)
      : AutoClone<RelocatableInst, LoadDataBlock>(), reg(reg), offset(offset) {}

  // Load a value from the specified offset of the datablock
  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override;
};

class StoreDataBlock : public AutoClone<RelocatableInst, StoreDataBlock> {
  RegLLVM reg;
  int64_t offset;

public:
  StoreDataBlock(RegLLVM reg, int64_t offset)
      : AutoClone<RelocatableInst, StoreDataBlock>(), reg(reg), offset(offset) {
  }

  // Store a value to the specified offset of the datablock
  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override;
};

class MovReg : public AutoClone<RelocatableInst, MovReg> {
  RegLLVM dst;
  RegLLVM src;

public:
  MovReg(RegLLVM dst, RegLLVM src)
      : AutoClone<RelocatableInst, MovReg>(), dst(dst), src(src) {}

  // Move a value from a register to another
  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override;
};

class LoadImm : public AutoClone<RelocatableInst, LoadImm> {
  RegLLVM reg;
  Constant imm;

public:
  LoadImm(RegLLVM reg, Constant imm)
      : AutoClone<RelocatableInst, LoadImm>(), reg(reg), imm(imm) {}

  // Set the register to this value
  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override;
};

class InstId : public AutoClone<RelocatableInst, InstId> {
  RegLLVM reg;

public:
  InstId(RegLLVM reg) : AutoClone<RelocatableInst, InstId>(), reg(reg) {}

  // Store the current instruction ID in the register
  llvm::MCInst reloc(ExecBlock *execBlock, CPUMode cpumode) const override;

  int getSize(const LLVMCPU &llvmcpu) const override;
};

} // namespace QBDI

#endif
