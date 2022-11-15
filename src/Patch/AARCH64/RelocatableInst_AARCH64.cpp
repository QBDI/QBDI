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
#include "Patch/AARCH64/Layer2_AARCH64.h"
#include "Patch/AARCH64/RelocatableInst_AARCH64.h"
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

  RegLLVM sr = execBlock->getScratchRegisterInfo().writeScratchRegister;
  return ldr(reg, sr, shadowOffset);
}

int LoadShadow::getSize(const LLVMCPU &llvmcpu) const { return 4; }

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

  RegLLVM sr = execBlock->getScratchRegisterInfo().writeScratchRegister;
  return str(reg, sr, shadowOffset);
}

int StoreShadow::getSize(const LLVMCPU &llvmcpu) const { return 4; }

// LoadDataBlock
// =============

llvm::MCInst LoadDataBlock::reloc(ExecBlock *execBlock, CPUMode cpumode) const {
  RegLLVM sr = execBlock->getScratchRegisterInfo().writeScratchRegister;
  return ldr(reg, sr, offset);
}

int LoadDataBlock::getSize(const LLVMCPU &llvmcpu) const { return 4; }

// StoreDataBlock
// ==============

llvm::MCInst StoreDataBlock::reloc(ExecBlock *execBlock,
                                   CPUMode cpumode) const {
  RegLLVM sr = execBlock->getScratchRegisterInfo().writeScratchRegister;
  return str(reg, sr, offset);
}

int StoreDataBlock::getSize(const LLVMCPU &llvmcpu) const { return 4; }

// MovReg
// ======

llvm::MCInst MovReg::reloc(ExecBlock *execBlock, CPUMode cpumode) const {
  return movrr(dst, src);
}

int MovReg::getSize(const LLVMCPU &llvmcpu) const { return 4; }

// LoadImm
// =======

llvm::MCInst LoadImm::reloc(ExecBlock *execBlock, CPUMode cpumode) const {
  if (imm > 0xFFFF) {
    uint16_t id = execBlock->newShadow();
    execBlock->setShadow(id, imm);
    rword offset = execBlock->getShadowOffset(id);
    RegLLVM sr = execBlock->getScratchRegisterInfo().writeScratchRegister;

    return ldr(reg, sr, offset);
  }
  return movri(reg, imm);
}

int LoadImm::getSize(const LLVMCPU &llvmcpu) const { return 4; }

// InstId
// ======

llvm::MCInst InstId::reloc(ExecBlock *execBlock, CPUMode cpumode) const {
  uint16_t v = execBlock->getNextInstID();
  QBDI_REQUIRE_ABORT(v <= 0xFFFF, "Unexpected InstID {}", v);

  return movri(reg, v);
}

int InstId::getSize(const LLVMCPU &llvmcpu) const { return 4; }

// Target Specific RelocatableInst

// SetScratchRegister
// ================
llvm::MCInst SetScratchRegister::reloc(ExecBlock *execBlock,
                                       CPUMode cpumode) const {
  llvm::MCInst res = inst;
  QBDI_REQUIRE_ABORT(opn < res.getNumOperands(), "Invalid operand {}", opn);
  res.getOperand(opn).setReg(
      execBlock->getScratchRegisterInfo().writeScratchRegister.getValue());
  return res;
}

// EpilogueAddrRel
// ===============
llvm::MCInst EpilogueAddrRel::reloc(ExecBlock *execBlock,
                                    CPUMode cpumode) const {
  rword target = execBlock->getEpilogueOffset() + offset;
  if (target % 4 != 0) {
    QBDI_CRITICAL("Bad alignment!");
  }
  llvm::MCInst res = inst;
  QBDI_REQUIRE_ABORT(opn < res.getNumOperands(), "Invalid operand {}", opn);
  res.getOperand(opn).setImm(target / 4);
  return res;
}

// RestoreScratchRegister
// ======================

llvm::MCInst RestoreScratchRegister::reloc(ExecBlock *execBlock,
                                           CPUMode cpumode) const {
  RegLLVM sr = execBlock->getScratchRegisterInfo().writeScratchRegister;
  return ldr(sr, sr, offsetof(Context, hostState.scratchRegisterValue));
}

// ResetScratchRegister
// ====================

llvm::MCInst ResetScratchRegister::reloc(ExecBlock *execBlock,
                                         CPUMode cpumode) const {
  RegLLVM sr = execBlock->getScratchRegisterInfo().writeScratchRegister;
  unsigned offset =
      execBlock->getDataBlockBase() - (execBlock->getCurrentPC() & ~0xfff);
  return adrp(sr, offset);
}

// SetBaseAddress
// ==============
llvm::MCInst SetBaseAddress::reloc(ExecBlock *execBlock,
                                   CPUMode cpumode) const {
  unsigned offset =
      execBlock->getDataBlockBase() - (execBlock->getCurrentPC() & ~0xfff);
  return adrp(reg, offset);
}

// LoadDataBlockX2
// ===============

llvm::MCInst LoadDataBlockX2::reloc(ExecBlock *execBlock,
                                    CPUMode cpumode) const {
  RegLLVM sr = execBlock->getScratchRegisterInfo().writeScratchRegister;
  return ldp(reg, reg2, sr, offset);
}

// StoreDataBlockX2
// ================

llvm::MCInst StoreDataBlockX2::reloc(ExecBlock *execBlock,
                                     CPUMode cpumode) const {
  RegLLVM sr = execBlock->getScratchRegisterInfo().writeScratchRegister;
  return stp(reg, reg2, sr, offset);
}

} // namespace QBDI
