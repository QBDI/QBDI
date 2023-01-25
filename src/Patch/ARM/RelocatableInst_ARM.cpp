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
#include <stdint.h>

#include "Target/ARM/Utils/ARMBaseInfo.h"

#include "Engine/LLVMCPU.h"
#include "ExecBlock/ExecBlock.h"
#include "Patch/ARM/Layer2_ARM.h"
#include "Patch/ARM/RelocatableInst_ARM.h"
#include "Utility/LogSys.h"

namespace QBDI {

// Generic RelocatableInst that must be implemented by each target

// RelocTag
// ========

llvm::MCInst RelocTag::reloc(ExecBlock *execBlock, CPUMode cpumode) const {
  QBDI_ERROR("Internal Error: Relocate a Tag instruction.");
  if (cpumode == CPUMode::Thumb) {
    return tmovr(Reg(8), Reg(8));
  } else {
    return nop();
  }
}

// LoadShadow
// ==========

llvm::MCInst LoadShadow::reloc(ExecBlock *execBlock, CPUMode cpumode) const {
  uint16_t id = execBlock->getLastShadow(tag);
  unsigned int shadowOffset = execBlock->getShadowOffset(id);

  return LoadDataBlockCC(reg, shadowOffset, llvm::ARMCC::AL)
      .reloc(execBlock, cpumode);
}

int LoadShadow::getSize(const LLVMCPU &llvmcpu) const {
  return LoadDataBlockCC(reg, 0xfff, llvm::ARMCC::AL).getSize(llvmcpu);
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
  return StoreDataBlockCC(reg, shadowOffset, llvm::ARMCC::AL)
      .reloc(execBlock, cpumode);
}

int StoreShadow::getSize(const LLVMCPU &llvmcpu) const {
  return StoreDataBlockCC(reg, 0xfff, llvm::ARMCC::AL).getSize(llvmcpu);
}

// LoadDataBlock
// =============

llvm::MCInst LoadDataBlock::reloc(ExecBlock *execBlock, CPUMode cpumode) const {

  return LoadDataBlockCC(reg, offset, llvm::ARMCC::AL)
      .reloc(execBlock, cpumode);
}

int LoadDataBlock::getSize(const LLVMCPU &llvmcpu) const {
  return LoadDataBlockCC(reg, offset, llvm::ARMCC::AL).getSize(llvmcpu);
}

// StoreDataBlock
// ==============

llvm::MCInst StoreDataBlock::reloc(ExecBlock *execBlock,
                                   CPUMode cpumode) const {
  return StoreDataBlockCC(reg, offset, llvm::ARMCC::AL)
      .reloc(execBlock, cpumode);
}

int StoreDataBlock::getSize(const LLVMCPU &llvmcpu) const {
  return StoreDataBlockCC(reg, offset, llvm::ARMCC::AL).getSize(llvmcpu);
}

// MovReg
// ======

llvm::MCInst MovReg::reloc(ExecBlock *execBlock, CPUMode cpumode) const {
  return MovRegCC(dst, src, llvm::ARMCC::AL).reloc(execBlock, cpumode);
}

int MovReg::getSize(const LLVMCPU &llvmcpu) const {
  return MovRegCC(dst, src, llvm::ARMCC::AL).getSize(llvmcpu);
}

// LoadImm
// =======

llvm::MCInst LoadImm::reloc(ExecBlock *execBlock, CPUMode cpumode) const {
  return LoadImmCC(reg, imm, llvm::ARMCC::AL).reloc(execBlock, cpumode);
}

int LoadImm::getSize(const LLVMCPU &llvmcpu) const {
  return LoadImmCC(reg, imm, llvm::ARMCC::AL).getSize(llvmcpu);
}

// InstId
// ======

llvm::MCInst InstId::reloc(ExecBlock *execBlock, CPUMode cpumode) const {
  uint16_t v = execBlock->getNextInstID();
  return LoadImm(reg, v).reloc(execBlock, cpumode);
}

int InstId::getSize(const LLVMCPU &llvmcpu) const {
  return LoadImm(reg, 0xffff).getSize(llvmcpu);
}

// Target Specific RelocatableInst

// LoadShadowCC
// ============

llvm::MCInst LoadShadowCC::reloc(ExecBlock *execBlock, CPUMode cpumode) const {
  uint16_t id = execBlock->getLastShadow(tag);
  unsigned int shadowOffset = execBlock->getShadowOffset(id);

  return LoadDataBlockCC(reg, shadowOffset, cond).reloc(execBlock, cpumode);
}

int LoadShadowCC::getSize(const LLVMCPU &llvmcpu) const {
  return LoadDataBlockCC(reg, 0xfff, cond).getSize(llvmcpu);
}

// StoreShadowCC
// =============

llvm::MCInst StoreShadowCC::reloc(ExecBlock *execBlock, CPUMode cpumode) const {
  uint16_t id;
  if (create) {
    id = execBlock->newShadow(tag);
  } else {
    id = execBlock->getLastShadow(tag);
  }
  unsigned int shadowOffset = execBlock->getShadowOffset(id);
  return StoreDataBlockCC(reg, shadowOffset, cond).reloc(execBlock, cpumode);
}

int StoreShadowCC::getSize(const LLVMCPU &llvmcpu) const {
  return StoreDataBlockCC(reg, 0xfff, cond).getSize(llvmcpu);
}

// LoadDataBlockCC
// ===============

llvm::MCInst LoadDataBlockCC::reloc(ExecBlock *execBlock,
                                    CPUMode cpumode) const {

  if (cpumode == CPUMode::Thumb) {
    RegLLVM sr = execBlock->getScratchRegisterInfo().thumbScratchRegister;
    return t2ldri12(reg, sr, offset, cond);
  } else {
    return ldri12(reg, Reg(REG_PC),
                  execBlock->getDataBlockOffset() + offset - 8, cond);
  }
}

int LoadDataBlockCC::getSize(const LLVMCPU &llvmcpu) const { return 4; }

// StoreDataBlockCC
// ================

llvm::MCInst StoreDataBlockCC::reloc(ExecBlock *execBlock,
                                     CPUMode cpumode) const {

  if (cpumode == CPUMode::Thumb) {
    RegLLVM sr = execBlock->getScratchRegisterInfo().thumbScratchRegister;
    return t2stri12(reg, sr, offset, cond);
  } else {
    return stri12(reg, Reg(REG_PC),
                  execBlock->getDataBlockOffset() + offset - 8, cond);
  }
}

int StoreDataBlockCC::getSize(const LLVMCPU &llvmcpu) const { return 4; }

// MovRegCC
// ========

llvm::MCInst MovRegCC::reloc(ExecBlock *execBlock, CPUMode cpumode) const {
  if (cpumode == CPUMode::Thumb) {
    return tmovr(dst, src, cond);
  } else {
    return movr(dst, src, cond);
  }
}

int MovRegCC::getSize(const LLVMCPU &llvmcpu) const {
  if (llvmcpu.getCPUMode() == CPUMode::Thumb) {
    return 2;
  } else {
    return 4;
  }
}

// LoadImmCC
// =========

llvm::MCInst LoadImmCC::reloc(ExecBlock *execBlock, CPUMode cpumode) const {
  if (cpumode == CPUMode::Thumb) {
    if (not t2moviCompatible(imm)) {
      // ARMv6T2 has a t2MOVi16, however, llvm expects to have the feature v8m
      // to enable this instruction.
      //
      // Avoid this instruction and used a shadow instead
      uint16_t id = execBlock->newShadow();
      execBlock->setShadow(id, imm);
      rword offset = execBlock->getShadowOffset(id);
      RegLLVM sr = execBlock->getScratchRegisterInfo().thumbScratchRegister;

      return t2ldri12(reg, sr, offset, cond);
    } else {
      return t2movi(reg, imm, cond);
    }
  } else {
    if (imm > 0xFFFF) {
      uint16_t id = execBlock->newShadow();
      execBlock->setShadow(id, imm);
      rword offset = execBlock->getShadowOffset(id);

      return ldri12(reg, Reg(REG_PC),
                    execBlock->getDataBlockOffset() + offset - 8, cond);
    } else {
      return movi(reg, imm, cond);
    }
  }
}

int LoadImmCC::getSize(const LLVMCPU &llvmcpu) const { return 4; }

// EpilogueBranch
// ==============

llvm::MCInst EpilogueBranch::reloc(ExecBlock *execBlock,
                                   CPUMode cpumode) const {
  QBDI_REQUIRE_ABORT(cpumode == CPUMode::ARM, "Cannot be used in Thumb mode");

  rword target = execBlock->getEpilogueOffset() - 8;
  QBDI_REQUIRE_ABORT(target % 4 == 0, "Bad alignment!");
  return branch(target);
}

int EpilogueBranch::getSize(const LLVMCPU &llvmcpu) const { return 4; }

// SetSREpilogue
// =============

llvm::MCInst SetSREpilogue::reloc(ExecBlock *execBlock, CPUMode cpumode) const {
  QBDI_REQUIRE_ABORT(cpumode == CPUMode::Thumb, "Cannot be used in ARM mode");
  RegLLVM sr = execBlock->getScratchRegisterInfo().thumbScratchRegister;

  return t2sub(sr, sr, execBlock->getEpilogueSize());
}

int SetSREpilogue::getSize(const LLVMCPU &llvmcpu) const { return 4; }

// SRBranch
// ========

llvm::MCInst SRBranch::reloc(ExecBlock *execBlock, CPUMode cpumode) const {
  QBDI_REQUIRE_ABORT(cpumode == CPUMode::Thumb, "Cannot be used in ARM mode");

  return tbx(execBlock->getScratchRegisterInfo().thumbScratchRegister);
}

int SRBranch::getSize(const LLVMCPU &llvmcpu) const { return 2; }

// DataBlockAddress
// ================

llvm::MCInst DataBlockAddress::reloc(ExecBlock *execBlock,
                                     CPUMode cpumode) const {
  if (cpumode == CPUMode::Thumb) {
    // EpilogueBranch may be used in the execBroker, avoid using sr
    unsigned int offset = execBlock->getDataBlockOffset() - 4;

    // The instruction Adr used Align(PC, 4). If the current instruction is not
    // align, add 2
    if (offset % 4 != 0) {
      offset += 2;
    }
    QBDI_REQUIRE_ABORT(offset % 4 == 0, "Invalid alignement");
    RegLLVM destReg = reg;
    if (setScratchRegister) {
      destReg = execBlock->getScratchRegisterInfo().thumbScratchRegister;
    }
    return t2adr(destReg, offset);
  } else {
    QBDI_REQUIRE_ABORT(not setScratchRegister,
                       "No scratch register in ARM mode");

    unsigned int offset = execBlock->getDataBlockOffset() - 8;
    if (not armExpandCompatible(offset)) {
      offset += 256 - (offset & 0xff);
    }

    return adr(reg, offset);
  }
}

int DataBlockAddress::getSize(const LLVMCPU &llvmcpu) const { return 4; }

// RelativeAddress
// ===============

llvm::MCInst RelativeAddress::reloc(ExecBlock *execBlock,
                                    CPUMode cpumode) const {
  if (cpumode == CPUMode::Thumb) {
    if (execBlock->getCurrentPC() % 4 != 0) {
      return t2adr(reg, offset - 2);
    } else {
      return t2adr(reg, offset - 4);
    }
  } else {
    QBDI_REQUIRE_ABORT(armExpandCompatible(offset - 8),
                       "Offset not compatible with the instruction {}",
                       offset - 8);
    return adr(reg, offset - 8);
  }
}

int RelativeAddress::getSize(const LLVMCPU &llvmcpu) const { return 4; }

// MovToSavedScratchReg
// ====================

llvm::MCInst MovToSavedScratchReg::reloc(ExecBlock *execBlock,
                                         CPUMode cpumode) const {
  if (cpumode == CPUMode::Thumb) {
    if (candidateSC ==
        execBlock->getScratchRegisterInfo().thumbScratchRegister) {
      return StoreDataBlockCC(
                 reg, offsetof(Context, hostState.scratchRegisterValue), cond)
          .reloc(execBlock, cpumode);
    } else {
      // need a 4 bytes instruction as the size of an RelocatableInst should not
      // depend of his position
      return t2add(reg, candidateSC, 0, cond);
    }
  } else {
    // no scratch register in ARM mode
    return MovRegCC(reg, candidateSC, cond).reloc(execBlock, cpumode);
  }
}

int MovToSavedScratchReg::getSize(const LLVMCPU &llvmcpu) const { return 4; }

// MovFromSavedScratchReg
// ======================

llvm::MCInst MovFromSavedScratchReg::reloc(ExecBlock *execBlock,
                                           CPUMode cpumode) const {
  if (cpumode == CPUMode::Thumb) {
    if (candidateSC ==
        execBlock->getScratchRegisterInfo().thumbScratchRegister) {
      return LoadDataBlockCC(
                 reg, offsetof(Context, hostState.scratchRegisterValue), cond)
          .reloc(execBlock, cpumode);
    } else {
      // need a 4 bytes instruction as the size of an RelocatableInst should not
      // depend of his position
      return t2add(candidateSC, reg, 0, cond);
    }
  } else {
    // no scratch register in ARM mode
    return MovRegCC(candidateSC, reg, cond).reloc(execBlock, cpumode);
  }
}

int MovFromSavedScratchReg::getSize(const LLVMCPU &llvmcpu) const { return 4; }

} // namespace QBDI
