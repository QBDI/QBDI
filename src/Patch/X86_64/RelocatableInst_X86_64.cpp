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

#include <stdint.h>

#include "ExecBlock/ExecBlock.h"
#include "Patch/X86_64/Layer2_X86_64.h"
#include "Patch/X86_64/RelocatableInst_X86_64.h"
#include "Utility/LogSys.h"

namespace QBDI {

// Generic RelocatableInst that must be implemented by each target

// RelocTag
// ========

llvm::MCInst RelocTag::reloc(ExecBlock *execBlock, CPUMode cpumode) const {
  QBDI_ERROR("Internal Error: Relocate a Tag instruction.");
  return nop();
}

// LoadShadow
// ==========

llvm::MCInst LoadShadow::reloc(ExecBlock *execBlock, CPUMode cpumode) const {
  uint16_t id = execBlock->getLastShadow(tag);
  unsigned int shadowOffset = execBlock->getShadowOffset(id);

  if constexpr (is_x86_64) {
    return mov64rm(reg, Reg(REG_PC), 1, 0,
                   execBlock->getDataBlockOffset() + shadowOffset - 7, 0);
  } else {
    return mov32rm(reg, 0, 0, 0, execBlock->getDataBlockBase() + shadowOffset,
                   0);
  }
}

int LoadShadow::getSize(const LLVMCPU &llvmcpu) const {
  if constexpr (is_x86_64) {
    return 7;
  } else {
    return 6;
  }
}

// StoreShadow
// ===========

llvm::MCInst StoreShadow::reloc(ExecBlock *execBlock, CPUMode cpumode) const {
  uint16_t id;
  if (create) {
    id = execBlock->newShadow(tag);
  } else {
    id = execBlock->getLastShadow(tag);
  }
  unsigned int shadowOffset = execBlock->getShadowOffset(id);

  if constexpr (is_x86_64) {
    return mov64mr(Reg(REG_PC), 1, 0,
                   execBlock->getDataBlockOffset() + shadowOffset - 7, 0, reg);
  } else {
    return mov32mr(0, 0, 0, execBlock->getDataBlockBase() + shadowOffset, 0,
                   reg);
  }
}

int StoreShadow::getSize(const LLVMCPU &llvmcpu) const {
  if constexpr (is_x86_64) {
    return 7;
  } else {
    return 6;
  }
}

// LoadDataBlock
// =============

llvm::MCInst LoadDataBlock::reloc(ExecBlock *execBlock, CPUMode cpumode) const {

  if constexpr (is_x86_64) {
    return mov64rm(reg, Reg(REG_PC), 1, 0,
                   execBlock->getDataBlockOffset() + offset - 7, 0);
  } else {
    return mov32rm(reg, 0, 0, 0, execBlock->getDataBlockBase() + offset, 0);
  }
}

int LoadDataBlock::getSize(const LLVMCPU &llvmcpu) const {
  if constexpr (is_x86_64) {
    return 7;
  } else {
    return 6;
  }
}

// StoreDataBlock
// ==============

llvm::MCInst StoreDataBlock::reloc(ExecBlock *execBlock,
                                   CPUMode cpumode) const {

  if constexpr (is_x86_64) {
    return mov64mr(Reg(REG_PC), 1, 0,
                   execBlock->getDataBlockOffset() + offset - 7, 0, reg);
  } else {
    return mov32mr(0, 0, 0, execBlock->getDataBlockBase() + offset, 0, reg);
  }
}

int StoreDataBlock::getSize(const LLVMCPU &llvmcpu) const {
  if constexpr (is_x86_64) {
    return 7;
  } else {
    return 6;
  }
}

// MovReg
// ======

llvm::MCInst MovReg::reloc(ExecBlock *execBlock, CPUMode cpumode) const {
  if constexpr (is_x86_64)
    return mov64rr(dst, src);
  else
    return mov32rr(dst, src);
}

int MovReg::getSize(const LLVMCPU &llvmcpu) const {
  if constexpr (is_x86_64) {
    return 3;
  } else {
    return 2;
  }
}

// LoadImm
// =======

llvm::MCInst LoadImm::reloc(ExecBlock *execBlock, CPUMode cpumode) const {
  if constexpr (is_x86_64) {
    if (imm < -0x80000000ull or 0x80000000ull <= imm) {
      return mov64ri(reg, imm);
    } else {
      return mov64ri32(reg, imm);
    }
  } else {
    return mov32ri(reg, imm);
  }
}

int LoadImm::getSize(const LLVMCPU &llvmcpu) const {
  if constexpr (is_x86_64) {
    if (imm < -0x80000000ull or 0x80000000ull <= imm) {
      return 10;
    } else {
      return 7;
    }
  } else {
    return 5;
  }
}

// InstId
// ======

llvm::MCInst InstId::reloc(ExecBlock *execBlock, CPUMode cpumode) const {
  if constexpr (is_x86_64) {
    return mov64ri32(reg, execBlock->getNextInstID());
  } else {
    return mov32ri(reg, execBlock->getNextInstID());
  }
}

int InstId::getSize(const LLVMCPU &llvmcpu) const {
  if constexpr (is_x86_64) {
    return 7;
  } else {
    return 5;
  }
}

// Target Specific RelocatableInst

// EpilogueJump
// ============

llvm::MCInst EpilogueJump::reloc(ExecBlock *execBlock, CPUMode cpumode) const {
  return jmp(execBlock->getEpilogueOffset() - 1);
}

int EpilogueJump::getSize(const LLVMCPU &llvmcpu) const { return 5; }

// SetRegtoPCRel
// =============

llvm::MCInst SetRegtoPCRel::reloc(ExecBlock *execBlock, CPUMode cpumode) const {
  if constexpr (is_x86_64)
    return mov64ri(reg, offset + execBlock->getCurrentPC());
  else
    return mov32ri(reg, offset + execBlock->getCurrentPC());
}

int SetRegtoPCRel::getSize(const LLVMCPU &llvmcpu) const {
  if constexpr (is_x86_64) {
    return 10;
  } else {
    return 5;
  }
}

// DataBlockRel
// ============

llvm::MCInst DataBlockRel::reloc(ExecBlock *execBlock, CPUMode cpumode) const {
  llvm::MCInst res = inst;
  QBDI_REQUIRE_ABORT(opn < res.getNumOperands(), "Invalid operand {}", opn);
  QBDI_REQUIRE_ABORT(res.getOperand(opn).isImm(), "Unexpected operand type");

  res.getOperand(opn).setImm(offset + execBlock->getDataBlockOffset());
  return res;
}

// DataBlockAbsRel
// ===============

llvm::MCInst DataBlockAbsRel::reloc(ExecBlock *execBlock,
                                    CPUMode cpumode) const {
  llvm::MCInst res = inst;
  QBDI_REQUIRE_ABORT(opn < res.getNumOperands(), "Invalid operand {}", opn);
  QBDI_REQUIRE_ABORT(res.getOperand(opn).isImm(), "Unexpected operand type");

  res.getOperand(opn).setImm(execBlock->getDataBlockBase() + offset);
  return res;
}

} // namespace QBDI
