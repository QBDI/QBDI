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
#ifndef RELOCATABLEINST_H
#define RELOCATABLEINST_H

#include <memory>
#include <vector>

#include "llvm/MC/MCInst.h"

#include "ExecBlock/ExecBlock.h"
#include "Patch/PatchUtils.h"
#include "Patch/Types.h"

namespace QBDI {

class RelocatableInst {
protected:
  llvm::MCInst inst;

public:
  using UniquePtr = std::unique_ptr<RelocatableInst>;
  using UniquePtrVec = std::vector<std::unique_ptr<RelocatableInst>>;

  RelocatableInst(llvm::MCInst &&inst)
      : inst(std::forward<llvm::MCInst>(inst)) {}

  virtual std::unique_ptr<RelocatableInst> clone() const = 0;

  virtual llvm::MCInst reloc(ExecBlock *exec_block) const = 0;

  virtual ~RelocatableInst() = default;
};

class NoReloc : public AutoClone<RelocatableInst, NoReloc> {
public:
  NoReloc(llvm::MCInst &&inst)
      : AutoClone<RelocatableInst, NoReloc>(std::forward<llvm::MCInst>(inst)) {}

  llvm::MCInst reloc(ExecBlock *exec_block) const override { return inst; }
};

class DataBlockRel : public AutoClone<RelocatableInst, DataBlockRel> {
  unsigned int opn;
  rword offset;

public:
  DataBlockRel(llvm::MCInst &&inst, unsigned int opn, rword offset)
      : AutoClone<RelocatableInst, DataBlockRel>(
            std::forward<llvm::MCInst>(inst)),
        opn(opn), offset(offset){};

  llvm::MCInst reloc(ExecBlock *exec_block) const override {
    llvm::MCInst res = inst;
    res.getOperand(opn).setImm(offset + exec_block->getDataBlockOffset());
    return res;
  }
};

class DataBlockAbsRel : public AutoClone<RelocatableInst, DataBlockAbsRel> {
  unsigned int opn;
  rword offset;

public:
  DataBlockAbsRel(llvm::MCInst &&inst, unsigned int opn, rword offset)
      : AutoClone<RelocatableInst, DataBlockAbsRel>(
            std::forward<llvm::MCInst>(inst)),
        opn(opn), offset(offset){};

  llvm::MCInst reloc(ExecBlock *exec_block) const override {
    llvm::MCInst res = inst;
    res.getOperand(opn).setImm(exec_block->getDataBlockBase() + offset);
    return res;
  }
};

class EpilogueRel : public AutoClone<RelocatableInst, EpilogueRel> {
  unsigned int opn;
  rword offset;

public:
  EpilogueRel(llvm::MCInst &&inst, unsigned int opn, rword offset)
      : AutoClone<RelocatableInst, EpilogueRel>(
            std::forward<llvm::MCInst>(inst)),
        opn(opn), offset(offset){};

  llvm::MCInst reloc(ExecBlock *exec_block) const override {
    llvm::MCInst res = inst;
    res.getOperand(opn).setImm(offset + exec_block->getEpilogueOffset());
    return res;
  }
};

} // namespace QBDI

#endif
