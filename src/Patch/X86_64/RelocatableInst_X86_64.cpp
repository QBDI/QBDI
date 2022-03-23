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

#include <stdint.h>

#include "ExecBlock/ExecBlock.h"
#include "Patch/X86_64/Layer2_X86_64.h"
#include "Patch/X86_64/RelocatableInst_X86_64.h"
#include "Utility/LogSys.h"

namespace QBDI {

// Generic RelocatableInst that must be implemented by each target

// RelocTag
// ========

llvm::MCInst RelocTag::reloc(ExecBlock *exec_block) const {
  QBDI_ERROR("Internal Error: Relocate a Tag instruction.");
  return nop();
}

// LoadShadow
// ==========

llvm::MCInst LoadShadow::reloc(ExecBlock *exec_block) const {
  uint16_t id = exec_block->getLastShadow(tag);
  unsigned int shadowOffset = exec_block->getShadowOffset(id);

  if constexpr (is_x86_64) {
    return movrm(reg, Reg(REG_PC), 1, 0,
                 exec_block->getDataBlockOffset() + shadowOffset - 7, 0);
  } else {
    return movrm(reg, 0, 0, 0, exec_block->getDataBlockBase() + shadowOffset,
                 0);
  }
}

// StoreShadow
// ===========

llvm::MCInst StoreShadow::reloc(ExecBlock *exec_block) const {
  uint16_t id;
  if (create) {
    id = exec_block->newShadow(tag);
  } else {
    id = exec_block->getLastShadow(tag);
  }
  unsigned int shadowOffset = exec_block->getShadowOffset(id);

  if constexpr (is_x86_64) {
    return movmr(Reg(REG_PC), 1, 0,
                 exec_block->getDataBlockOffset() + shadowOffset - 7, 0, reg);
  } else {
    return movmr(0, 0, 0, exec_block->getDataBlockBase() + shadowOffset, 0,
                 reg);
  }
}

// LoadDataBlock
// =============

llvm::MCInst LoadDataBlock::reloc(ExecBlock *exec_block) const {

  if constexpr (is_x86_64) {
    return movrm(reg, Reg(REG_PC), 1, 0,
                 exec_block->getDataBlockOffset() + offset - 7, 0);
  } else {
    return movrm(reg, 0, 0, 0, exec_block->getDataBlockBase() + offset, 0);
  }
}

// StoreDataBlock
// ==============

llvm::MCInst StoreDataBlock::reloc(ExecBlock *exec_block) const {

  if constexpr (is_x86_64) {
    return movmr(Reg(REG_PC), 1, 0,
                 exec_block->getDataBlockOffset() + offset - 7, 0, reg);
  } else {
    return movmr(0, 0, 0, exec_block->getDataBlockBase() + offset, 0, reg);
  }
}

// MovReg
// ======

llvm::MCInst MovReg::reloc(ExecBlock *exec_block) const {
  return movrr(dst, src);
}

// LoadImm
// =======

llvm::MCInst LoadImm::reloc(ExecBlock *exec_block) const {
  return movri(reg, imm);
}

// InstId
// ======

llvm::MCInst InstId::reloc(ExecBlock *exec_block) const {
  return movri(reg, exec_block->getNextInstID());
}

// Target Specific RelocatableInst

// EpilogueRel
// ===========

llvm::MCInst EpilogueRel::reloc(ExecBlock *exec_block) const {
  llvm::MCInst res = inst;
  res.getOperand(opn).setImm(offset + exec_block->getEpilogueOffset());
  return res;
}

// HostPCRel
// =========

llvm::MCInst HostPCRel::reloc(ExecBlock *exec_block) const {
  llvm::MCInst res = inst;
  res.getOperand(opn).setImm(offset + exec_block->getCurrentPC());
  return res;
}

// DataBlockRel
// ============

llvm::MCInst DataBlockRel::reloc(ExecBlock *exec_block) const {
  llvm::MCInst res = inst;
  res.getOperand(opn).setImm(offset + exec_block->getDataBlockOffset());
  return res;
}

// DataBlockAbsRel
// ===============

llvm::MCInst DataBlockAbsRel::reloc(ExecBlock *exec_block) const {
  llvm::MCInst res = inst;
  res.getOperand(opn).setImm(exec_block->getDataBlockBase() + offset);
  return res;
}

} // namespace QBDI
