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
#include <stdint.h>
#include <stdlib.h>
#include <utility>

#include "MCTargetDesc/X86BaseInfo.h"
#include "X86InstrInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/MC/MCInstrInfo.h"

#include "QBDI/Config.h"
#include "QBDI/Platform.h"
#include "Engine/LLVMCPU.h"
#include "Patch/InstInfo.h"
#include "Patch/Patch.h"
#include "Patch/RelocatableInst.h"
#include "Patch/TempManager.h"
#include "Patch/X86_64/InstInfo_X86_64.h"
#include "Patch/X86_64/Layer2_X86_64.h"
#include "Patch/X86_64/PatchGenerator_X86_64.h"
#include "Patch/X86_64/RelocatableInst_X86_64.h"
#include "Utility/LogSys.h"

namespace QBDI {
class Patch;

// Generic PatchGenerator that must be implemented by each target

// TargetPrologue
// ==============

RelocatableInst::UniquePtrVec
TargetPrologue::generate(const Patch *patch, TempManager *temp_manager,
                         Patch *toMerge) const {
  // no instruction for X86_64
  return {};
}

// JmpEpilogue
// ===========

RelocatableInst::UniquePtrVec JmpEpilogue::generate(const Patch *patch,
                                                    TempManager *temp_manager,
                                                    Patch *toMerge) const {

  return conv_unique<RelocatableInst>(EpilogueRel::unique(jmp(0), 0, -1));
}

// Target Specific PatchGenerator

// GetPCOffset
// ===========

RelocatableInst::UniquePtrVec GetPCOffset::generate(const Patch *patch,
                                                    TempManager *temp_manager,
                                                    Patch *toMerge) const {
  if (type == ConstantType) {
    return conv_unique<RelocatableInst>(
        Mov(temp_manager->getRegForTemp(temp),
            Constant(patch->metadata.endAddress() + cst)));
  } else if (type == OperandType) {
    // FIXME: Implement for register operand
    return conv_unique<RelocatableInst>(
        Mov(temp_manager->getRegForTemp(temp),
            Constant(patch->metadata.endAddress() +
                     patch->metadata.inst.getOperand(op).getImm())));
  }
  _QBDI_UNREACHABLE();
}

// SimulateCall
// ============

RelocatableInst::UniquePtrVec SimulateCall::generate(const Patch *patch,
                                                     TempManager *temp_manager,
                                                     Patch *toMerge) const {
  RelocatableInst::UniquePtrVec p;

  append(p, WriteTemp(temp, Offset(Reg(REG_PC)))
                .generate(patch, temp_manager, nullptr));
  append(p,
         GetPCOffset(temp, Constant(0)).generate(patch, temp_manager, nullptr));
  p.push_back(Pushr(temp_manager->getRegForTemp(temp)));

  return p;
}

// SimulateRet
// ===========

RelocatableInst::UniquePtrVec SimulateRet::generate(const Patch *patch,
                                                    TempManager *temp_manager,
                                                    Patch *toMerge) const {
  const llvm::MCInst &inst = patch->metadata.inst;
  RelocatableInst::UniquePtrVec p;

  p.push_back(Popr(temp_manager->getRegForTemp(temp)));
  if (inst.getNumOperands() == 1 && inst.getOperand(0).isImm()) {
    p.push_back(Add(Reg(REG_SP), Constant(inst.getOperand(0).getImm())));
  }
  append(p, WriteTemp(temp, Offset(Reg(REG_PC)))
                .generate(patch, temp_manager, nullptr));

  return p;
}

// GetReadAddress
// ==============

RelocatableInst::UniquePtrVec
GetReadAddress::generate(const Patch *patch, TempManager *temp_manager,
                         Patch *toMerge) const {
  const llvm::MCInst &inst = patch->metadata.inst;
  // Check if this instruction does indeed read something
  unsigned size = getReadSize(inst);
  if (size > 0) {
    const llvm::MCInstrDesc &desc =
        patch->llvmcpu->getMCII().get(inst.getOpcode());
    uint64_t TSFlags = desc.TSFlags;
    unsigned FormDesc = TSFlags & llvm::X86II::FormMask;
    int memIndex = llvm::X86II::getMemoryOperandNo(TSFlags);

    Reg dest = temp_manager->getRegForTemp(temp);
    // If it is a stack read, return RSP value
    if (isStackRead(inst)) {
      if (inst.getOpcode() == llvm::X86::LEAVE ||
          inst.getOpcode() == llvm::X86::LEAVE64) {
        return conv_unique<RelocatableInst>(Mov(dest, Reg(REG_BP)));
      } else {
        return conv_unique<RelocatableInst>(Mov(dest, Reg(REG_SP)));
      }
    }
    // Implicit RSI or RDI
    else if (implicitDSIAccess(inst, desc)) {
      QBDI_REQUIRE_ACTION(index < 2 && "Wrong index", abort());
      unsigned int reg;
      if (FormDesc == llvm::X86II::RawFrmSrc ||
          (FormDesc == llvm::X86II::RawFrmDstSrc && index == 0)) {
        // (R|E)SI
        reg = Reg(4);
        QBDI_REQUIRE(reg == llvm::X86::RSI || reg == llvm::X86::ESI);
      } else {
        // (R|E)DI
        reg = Reg(5);
        QBDI_REQUIRE(reg == llvm::X86::RDI || reg == llvm::X86::EDI);
      }
      return conv_unique<RelocatableInst>(NoReloc::unique(movrr(dest, reg)));
    }
    // Moffs access
    else if (FormDesc == llvm::X86II::RawFrmMemOffs) {
      if (inst.getOperand(0).isImm() && inst.getOperand(1).isReg()) {
        return conv_unique<RelocatableInst>(
            NoReloc::unique(lea(dest, 0, 1, 0, inst.getOperand(0).getImm(),
                                inst.getOperand(1).getReg())));
      }
    }
    // XLAT instruction
    else if (inst.getOpcode() == llvm::X86::XLAT) {
      // (R|E)BX
      unsigned int reg = Reg(1);
      QBDI_REQUIRE(reg == llvm::X86::RBX || reg == llvm::X86::EBX);
      return conv_unique<RelocatableInst>(
          NoReloc::unique(movzxrr8(dest, llvm::X86::AL)),
          NoReloc::unique(lea(dest, reg, 1, dest, 0, 0)));
    }
    // Else replace the instruction with a LEA on the same address
    else if (memIndex >= 0) {
      unsigned realMemIndex = memIndex + llvm::X86II::getOperandBias(desc);

      if (inst.getNumOperands() >= realMemIndex + 4 &&
          inst.getOperand(realMemIndex + 0).isReg() &&
          inst.getOperand(realMemIndex + 1).isImm() &&
          inst.getOperand(realMemIndex + 2).isReg() &&
          inst.getOperand(realMemIndex + 3).isImm() &&
          inst.getOperand(realMemIndex + 4).isReg()) {
        // If it uses PC as a base register, substitute PC
        if (inst.getOperand(realMemIndex + 0).getReg() == Reg(REG_PC)) {
          return conv_unique<RelocatableInst>(
              Mov(temp_manager->getRegForTemp(0xFFFFFFFF),
                  Constant(patch->metadata.endAddress())),
              NoReloc::unique(lea(dest, temp_manager->getRegForTemp(0xFFFFFFFF),
                                  inst.getOperand(realMemIndex + 1).getImm(),
                                  inst.getOperand(realMemIndex + 2).getReg(),
                                  inst.getOperand(realMemIndex + 3).getImm(),
                                  inst.getOperand(realMemIndex + 4).getReg())));
        } else {
          return conv_unique<RelocatableInst>(NoReloc::unique(
              lea(dest, inst.getOperand(realMemIndex + 0).getReg(),
                  inst.getOperand(realMemIndex + 1).getImm(),
                  inst.getOperand(realMemIndex + 2).getReg(),
                  inst.getOperand(realMemIndex + 3).getImm(),
                  inst.getOperand(realMemIndex + 4).getReg())));
        }
      }
      QBDI_REQUIRE_ACTION(false && "No memory address found in the instruction",
                          abort());
    }
  }
  QBDI_REQUIRE_ACTION(
      false && "Called on an instruction which does not make read access",
      abort());
}

// GetWriteAddress
// ===============

RelocatableInst::UniquePtrVec
GetWriteAddress::generate(const Patch *patch, TempManager *temp_manager,
                          Patch *toMerge) const {
  const llvm::MCInst &inst = patch->metadata.inst;
  // Check if this instruction does indeed read something
  unsigned size = getWriteSize(inst);
  if (size > 0) {
    const llvm::MCInstrDesc &desc =
        patch->llvmcpu->getMCII().get(inst.getOpcode());
    uint64_t TSFlags = desc.TSFlags;
    unsigned FormDesc = TSFlags & llvm::X86II::FormMask;
    int memIndex = llvm::X86II::getMemoryOperandNo(TSFlags);
    unsigned opcode = inst.getOpcode();

    Reg dest = temp_manager->getRegForTemp(temp);
    // If it is a stack read, return RSP value
    if (isStackWrite(inst)) {
      if (inst.getOpcode() == llvm::X86::ENTER) {
        return conv_unique<RelocatableInst>(Mov(dest, Reg(REG_BP)));
      } else {
        return conv_unique<RelocatableInst>(Mov(dest, Reg(REG_SP)));
      }
    }
    // Implicit RSI or RDI
    else if (implicitDSIAccess(inst, desc)) {
      unsigned int reg;
      if (FormDesc == llvm::X86II::RawFrmSrc) {
        // (R|E)SI
        reg = Reg(4);
        QBDI_REQUIRE(reg == llvm::X86::RSI || reg == llvm::X86::ESI);
      } else {
        // (R|E)DI
        reg = Reg(5);
        QBDI_REQUIRE(reg == llvm::X86::RDI || reg == llvm::X86::EDI);
      }
      return conv_unique<RelocatableInst>(NoReloc::unique(movrr(dest, reg)));
    }
    // Moffs access
    else if (FormDesc == llvm::X86II::RawFrmMemOffs) {
      if (inst.getOperand(0).isImm() && inst.getOperand(1).isReg()) {
        return conv_unique<RelocatableInst>(
            NoReloc::unique(lea(dest, 0, 1, 0, inst.getOperand(0).getImm(),
                                inst.getOperand(1).getReg())));
      }
    }
    // MOVDIR64B instruction
    else if (opcode == llvm::X86::MOVDIR64B16 ||
             opcode == llvm::X86::MOVDIR64B32 ||
             opcode == llvm::X86::MOVDIR64B64) {
      return conv_unique<RelocatableInst>(
          NoReloc::unique(movrr(dest, inst.getOperand(0).getReg())));
    }
    // Else replace the instruction with a LEA on the same address
    else if (memIndex >= 0) {
      unsigned realMemIndex = memIndex + llvm::X86II::getOperandBias(desc);

      if (inst.getNumOperands() >= realMemIndex + 4 &&
          inst.getOperand(realMemIndex + 0).isReg() &&
          inst.getOperand(realMemIndex + 1).isImm() &&
          inst.getOperand(realMemIndex + 2).isReg() &&
          inst.getOperand(realMemIndex + 3).isImm() &&
          inst.getOperand(realMemIndex + 4).isReg()) {
        // If it uses PC as a base register, substitute PC
        if (inst.getOperand(realMemIndex + 0).getReg() == Reg(REG_PC)) {
          return conv_unique<RelocatableInst>(
              Mov(temp_manager->getRegForTemp(0xFFFFFFFF),
                  Constant(patch->metadata.endAddress())),
              NoReloc::unique(lea(dest, temp_manager->getRegForTemp(0xFFFFFFFF),
                                  inst.getOperand(realMemIndex + 1).getImm(),
                                  inst.getOperand(realMemIndex + 2).getReg(),
                                  inst.getOperand(realMemIndex + 3).getImm(),
                                  inst.getOperand(realMemIndex + 4).getReg())));
        } else {
          return conv_unique<RelocatableInst>(NoReloc::unique(
              lea(dest, inst.getOperand(realMemIndex + 0).getReg(),
                  inst.getOperand(realMemIndex + 1).getImm(),
                  inst.getOperand(realMemIndex + 2).getReg(),
                  inst.getOperand(realMemIndex + 3).getImm(),
                  inst.getOperand(realMemIndex + 4).getReg())));
        }
      }
      QBDI_REQUIRE_ACTION(false && "No memory address found in the instruction",
                          abort());
    }
  }
  QBDI_REQUIRE_ACTION(
      false && "Called on an instruction which does not make write access",
      abort());
}

// GetReadValue
// ============

RelocatableInst::UniquePtrVec GetReadValue::generate(const Patch *patch,
                                                     TempManager *temp_manager,
                                                     Patch *toMerge) const {
  const llvm::MCInst &inst = patch->metadata.inst;
  const unsigned size = getReadSize(inst);
  if (size > 0) {
    const llvm::MCInstrDesc &desc =
        patch->llvmcpu->getMCII().get(inst.getOpcode());
    uint64_t TSFlags = desc.TSFlags;
    unsigned FormDesc = TSFlags & llvm::X86II::FormMask;
    int memIndex = llvm::X86II::getMemoryOperandNo(TSFlags);

    unsigned dst = temp_manager->getRegForTemp(temp);
    if (is_bits_64 && size < sizeof(rword)) {
      dst = temp_manager->getSizedSubReg(dst, 4);
    } else if (size > sizeof(rword)) {
      return conv_unique<RelocatableInst>(NoReloc::unique(movri(dst, 0)));
    }

    if (isStackRead(inst)) {
      unsigned int stack_register = REG_SP;
      if (inst.getOpcode() == llvm::X86::LEAVE ||
          inst.getOpcode() == llvm::X86::LEAVE64) {
        stack_register = REG_BP;
      }
      if (size == 8) {
        return conv_unique<RelocatableInst>(
            NoReloc::unique(mov64rm(dst, Reg(stack_register), 1, 0, 0, 0)));
      } else if (size == 4) {
        return conv_unique<RelocatableInst>(
            NoReloc::unique(mov32rm(dst, Reg(stack_register), 1, 0, 0, 0)));
      } else if (size == 2) {
        return conv_unique<RelocatableInst>(
            NoReloc::unique(mov32rm16(dst, Reg(stack_register), 1, 0, 0, 0)));
      } else if (size == 1) {
        return conv_unique<RelocatableInst>(
            NoReloc::unique(mov32rm8(dst, Reg(stack_register), 1, 0, 0, 0)));
      }
    } else if (implicitDSIAccess(inst, desc)) {
      QBDI_REQUIRE_ACTION(index < 2 && "Wrong index", abort());
      unsigned int reg;
      if (FormDesc == llvm::X86II::RawFrmSrc ||
          (FormDesc == llvm::X86II::RawFrmDstSrc && index == 0)) {
        // (R|E)SI
        reg = Reg(4);
        QBDI_REQUIRE(reg == llvm::X86::RSI || reg == llvm::X86::ESI);
      } else {
        // (R|E)DI
        reg = Reg(5);
        QBDI_REQUIRE(reg == llvm::X86::RDI || reg == llvm::X86::EDI);
      }

      if (size == 8) {
        return conv_unique<RelocatableInst>(
            NoReloc::unique(mov64rm(dst, reg, 1, 0, 0, 0)));
      } else if (size == 4) {
        return conv_unique<RelocatableInst>(
            NoReloc::unique(mov32rm(dst, reg, 1, 0, 0, 0)));
      } else if (size == 2) {
        return conv_unique<RelocatableInst>(
            NoReloc::unique(mov32rm16(dst, reg, 1, 0, 0, 0)));
      } else if (size == 1) {
        return conv_unique<RelocatableInst>(
            NoReloc::unique(mov32rm8(dst, reg, 1, 0, 0, 0)));
      }
    } else if (FormDesc == llvm::X86II::RawFrmMemOffs) {
      if (inst.getOperand(0).isImm() && inst.getOperand(1).isReg()) {
        rword displacement = inst.getOperand(0).getImm();
        unsigned seg = inst.getOperand(1).getReg();

        if (size == 8) {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(mov64rm(dst, 0, 1, 0, displacement, seg)));
        } else if (size == 4) {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(mov32rm(dst, 0, 1, 0, displacement, seg)));
        } else if (size == 2) {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(mov32rm16(dst, 0, 1, 0, displacement, seg)));
        } else if (size == 1) {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(mov32rm8(dst, 0, 1, 0, displacement, seg)));
        }
      }
    } else if (inst.getOpcode() == llvm::X86::XLAT) {
      unsigned dest = temp_manager->getRegForTemp(temp);
      // (R|E)BX
      unsigned int reg = Reg(1);
      QBDI_REQUIRE(reg == llvm::X86::RBX || reg == llvm::X86::EBX);
      QBDI_REQUIRE(size == 1);
      return conv_unique<RelocatableInst>(
          NoReloc::unique(movzxrr8(dest, llvm::X86::AL)),
          NoReloc::unique(mov32rm8(dest, reg, 1, dest, 0, 0)));
    } else if (memIndex >= 0) {
      unsigned realMemIndex = memIndex + llvm::X86II::getOperandBias(desc);

      if (inst.getNumOperands() >= realMemIndex + 4 &&
          inst.getOperand(realMemIndex + 0).isReg() &&
          inst.getOperand(realMemIndex + 1).isImm() &&
          inst.getOperand(realMemIndex + 2).isReg() &&
          inst.getOperand(realMemIndex + 3).isImm() &&
          inst.getOperand(realMemIndex + 4).isReg()) {

        llvm::MCInst readinst;

        unsigned base = inst.getOperand(realMemIndex + 0).getReg();
        rword scale = inst.getOperand(realMemIndex + 1).getImm();
        unsigned offset = inst.getOperand(realMemIndex + 2).getReg();
        rword displacement = inst.getOperand(realMemIndex + 3).getImm();
        unsigned seg = inst.getOperand(realMemIndex + 4).getReg();

        if (inst.getOperand(realMemIndex + 0).getReg() == Reg(REG_PC)) {
          base = temp_manager->getRegForTemp(0xFFFFFFFF);
        }

        if (size == 8) {
          readinst = mov64rm(dst, base, scale, offset, displacement, seg);
        } else if (size == 4) {
          readinst = mov32rm(dst, base, scale, offset, displacement, seg);
        } else if (size == 2) {
          readinst = mov32rm16(dst, base, scale, offset, displacement, seg);
        } else if (size == 1) {
          readinst = mov32rm8(dst, base, scale, offset, displacement, seg);
        }

        if (inst.getOperand(realMemIndex + 0).getReg() == Reg(REG_PC)) {
          return conv_unique<RelocatableInst>(
              Mov(temp_manager->getRegForTemp(0xFFFFFFFF),
                  Constant(patch->metadata.endAddress())),
              NoReloc::unique(std::move(readinst)));
        } else {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(std::move(readinst)));
        }
      }
      QBDI_REQUIRE_ACTION(false && "No memory address found in the instruction",
                          abort());
    }
  }
  QBDI_REQUIRE_ACTION(
      false && "Called on an instruction which does not make read access",
      abort());
}

// GetWriteValue
// =============

RelocatableInst::UniquePtrVec GetWriteValue::generate(const Patch *patch,
                                                      TempManager *temp_manager,
                                                      Patch *toMerge) const {
  const llvm::MCInst &inst = patch->metadata.inst;
  const unsigned size = getWriteSize(inst);
  if (size > 0) {
    const llvm::MCInstrDesc &desc =
        patch->llvmcpu->getMCII().get(inst.getOpcode());
    uint64_t TSFlags = desc.TSFlags;
    unsigned FormDesc = TSFlags & llvm::X86II::FormMask;
    int memIndex = llvm::X86II::getMemoryOperandNo(TSFlags);

    unsigned src_addr = temp_manager->getRegForTemp(temp);
    unsigned dst = src_addr;
    if (is_bits_64 && size < sizeof(rword)) {
      dst = temp_manager->getSizedSubReg(dst, 4);
    } else if (size > sizeof(rword)) {
      return conv_unique<RelocatableInst>(NoReloc::unique(movri(dst, 0)));
    }

    if (isStackWrite(inst)) {
      unsigned int stack_register = REG_SP;
      if (inst.getOpcode() == llvm::X86::ENTER) {
        stack_register = REG_BP;
      }
      if (size == 8) {
        return conv_unique<RelocatableInst>(
            NoReloc::unique(mov64rm(dst, Reg(stack_register), 1, 0, 0, 0)));
      } else if (size == 4) {
        return conv_unique<RelocatableInst>(
            NoReloc::unique(mov32rm(dst, Reg(stack_register), 1, 0, 0, 0)));
      } else if (size == 2) {
        return conv_unique<RelocatableInst>(
            NoReloc::unique(mov32rm16(dst, Reg(stack_register), 1, 0, 0, 0)));
      } else if (size == 1) {
        return conv_unique<RelocatableInst>(
            NoReloc::unique(mov32rm8(dst, Reg(stack_register), 1, 0, 0, 0)));
      }
    } else if (implicitDSIAccess(inst, desc)) {
      // As the address is already in temp, just dereference it.
      if (size == 8) {
        return conv_unique<RelocatableInst>(
            NoReloc::unique(mov64rm(dst, src_addr, 1, 0, 0, 0)));
      } else if (size == 4) {
        return conv_unique<RelocatableInst>(
            NoReloc::unique(mov32rm(dst, src_addr, 1, 0, 0, 0)));
      } else if (size == 2) {
        return conv_unique<RelocatableInst>(
            NoReloc::unique(mov32rm16(dst, src_addr, 1, 0, 0, 0)));
      } else if (size == 1) {
        return conv_unique<RelocatableInst>(
            NoReloc::unique(mov32rm8(dst, src_addr, 1, 0, 0, 0)));
      }
    } else if (FormDesc == llvm::X86II::RawFrmMemOffs) {
      if (inst.getOperand(0).isImm() && inst.getOperand(1).isReg()) {
        rword displacement = inst.getOperand(0).getImm();
        unsigned seg = inst.getOperand(1).getReg();

        if (size == 8) {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(mov64rm(dst, 0, 1, 0, displacement, seg)));
        } else if (size == 4) {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(mov32rm(dst, 0, 1, 0, displacement, seg)));
        } else if (size == 2) {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(mov32rm16(dst, 0, 1, 0, displacement, seg)));
        } else if (size == 1) {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(mov32rm8(dst, 0, 1, 0, displacement, seg)));
        }
      }
    } else if (memIndex >= 0) {
      unsigned realMemIndex = memIndex + llvm::X86II::getOperandBias(desc);

      if (inst.getNumOperands() >= realMemIndex + 4 &&
          inst.getOperand(realMemIndex + 0).isReg() &&
          inst.getOperand(realMemIndex + 1).isImm() &&
          inst.getOperand(realMemIndex + 2).isReg() &&
          inst.getOperand(realMemIndex + 3).isImm() &&
          inst.getOperand(realMemIndex + 4).isReg()) {

        unsigned seg = inst.getOperand(realMemIndex + 4).getReg();

        if (size == 8) {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(mov64rm(dst, src_addr, 1, 0, 0, seg)));
        } else if (size == 4) {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(mov32rm(dst, src_addr, 1, 0, 0, seg)));
        } else if (size == 2) {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(mov32rm16(dst, src_addr, 1, 0, 0, seg)));
        } else if (size == 1) {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(mov32rm8(dst, src_addr, 1, 0, 0, seg)));
        }
      }
      QBDI_REQUIRE_ACTION(false && "No memory address found in the instruction",
                          abort());
    }
  }
  QBDI_REQUIRE_ACTION(
      false && "Called on an instruction which does not make write access",
      abort());
}

} // namespace QBDI
