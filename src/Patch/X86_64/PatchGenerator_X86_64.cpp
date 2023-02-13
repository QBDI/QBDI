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
#include <stdlib.h>
#include <utility>

#include "MCTargetDesc/X86BaseInfo.h"
#include "X86InstrInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/MC/MCInstrInfo.h"

#include "QBDI/Config.h"
#include "QBDI/Options.h"
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

// Generic PatchGenerator that must be implemented by each target

// TargetPrologue
// ==============

RelocatableInst::UniquePtrVec
TargetPrologue::genReloc(const Patch &patch) const {
  // no instruction for X86_64
  return {};
}

// JmpEpilogue
// ===========

RelocatableInst::UniquePtrVec
JmpEpilogue::genReloc(const LLVMCPU &llvmcpu) const {

  return conv_unique<RelocatableInst>(EpilogueJump::unique());
}

// Target Specific PatchGenerator

// GetPCOffset
// ===========

RelocatableInst::UniquePtrVec
GetPCOffset::generate(const Patch &patch, TempManager &temp_manager) const {
  if (type == ConstantType) {
    return conv_unique<RelocatableInst>(
        LoadImm::unique(temp_manager.getRegForTemp(temp),
                        Constant(patch.metadata.endAddress() + cst)));
  } else if (type == OperandType) {
    QBDI_REQUIRE_ABORT_PATCH(op < patch.metadata.inst.getNumOperands(), patch,
                             "Invalid operand {}", op);

    // FIXME: Implement for register operand
    QBDI_REQUIRE_ABORT_PATCH(patch.metadata.inst.getOperand(op).isImm(), patch,
                             "Unexpected operand type");
    return conv_unique<RelocatableInst>(
        LoadImm::unique(temp_manager.getRegForTemp(temp),
                        Constant(patch.metadata.endAddress() +
                                 patch.metadata.inst.getOperand(op).getImm())));
  }
  _QBDI_UNREACHABLE();
}

// SimulateCall
// ============

RelocatableInst::UniquePtrVec
SimulateCall::generate(const Patch &patch, TempManager &temp_manager) const {
  RelocatableInst::UniquePtrVec p;

  append(p, WriteTemp(temp, Offset(Reg(REG_PC))).generate(patch, temp_manager));
  append(p, GetPCOffset(temp, Constant(0)).generate(patch, temp_manager));
  p.push_back(Pushr(temp_manager.getRegForTemp(temp)));

  return p;
}

// SimulateRet
// ===========

RelocatableInst::UniquePtrVec
SimulateRet::generate(const Patch &patch, TempManager &temp_manager) const {
  const llvm::MCInst &inst = patch.metadata.inst;
  RelocatableInst::UniquePtrVec p;

  p.push_back(Popr(temp_manager.getRegForTemp(temp)));
  if (inst.getNumOperands() == 1 and inst.getOperand(0).isImm()) {
    p.push_back(
        Add(Reg(REG_SP), Reg(REG_SP), Constant(inst.getOperand(0).getImm())));
  }
  append(p, WriteTemp(temp, Offset(Reg(REG_PC))).generate(patch, temp_manager));

  return p;
}

// GetReadAddress
// ==============

RelocatableInst::UniquePtrVec
GetReadAddress::generate(const Patch &patch, TempManager &temp_manager) const {
  const llvm::MCInst &inst = patch.metadata.inst;
  // Check if this instruction does indeed read something
  unsigned size = getReadSize(inst, *patch.llvmcpu);
  if (size > 0) {
    const llvm::MCInstrDesc &desc =
        patch.llvmcpu->getMCII().get(inst.getOpcode());
    uint64_t TSFlags = desc.TSFlags;
    unsigned FormDesc = TSFlags & llvm::X86II::FormMask;
    int memIndex = llvm::X86II::getMemoryOperandNo(TSFlags);

    Reg dest = temp_manager.getRegForTemp(temp);
    // If it is a stack read, return RSP value
    if (isStackRead(inst)) {
      if (inst.getOpcode() == llvm::X86::LEAVE ||
          inst.getOpcode() == llvm::X86::LEAVE64) {
        return conv_unique<RelocatableInst>(MovReg::unique(dest, Reg(REG_BP)));
      } else {
        return conv_unique<RelocatableInst>(MovReg::unique(dest, Reg(REG_SP)));
      }
    }
    // Implicit RSI or RDI
    else if (implicitDSIAccess(inst, desc)) {
      QBDI_REQUIRE_ABORT_PATCH(index < 2, patch, "Wrong index {}", index);
      RegLLVM reg(0);
      if (FormDesc == llvm::X86II::RawFrmSrc ||
          (FormDesc == llvm::X86II::RawFrmDstSrc and index == 0)) {
        // (R|E)SI
        reg = Reg(4);
        QBDI_REQUIRE(reg == llvm::X86::RSI || reg == llvm::X86::ESI);
      } else {
        // (R|E)DI
        reg = Reg(5);
        QBDI_REQUIRE(reg == llvm::X86::RDI || reg == llvm::X86::EDI);
      }
      return conv_unique<RelocatableInst>(MovReg::unique(dest, reg));
    }
    // Moffs access
    else if (FormDesc == llvm::X86II::RawFrmMemOffs) {
      QBDI_REQUIRE_ABORT_PATCH(1 < inst.getNumOperands(), patch,
                               "Unexpected number of operand");
      QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(0).isImm(), patch,
                               "Unexpected operand type");
      QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(1).isReg(), patch,
                               "Unexpected operand type");
      return conv_unique<RelocatableInst>(Lea(dest, 0, 1, 0,
                                              inst.getOperand(0).getImm(),
                                              inst.getOperand(1).getReg()));
    }
    // XLAT instruction
    else if (inst.getOpcode() == llvm::X86::XLAT) {
      // (R|E)BX
      RegLLVM reg = Reg(1);
      QBDI_REQUIRE(reg == llvm::X86::RBX || reg == llvm::X86::EBX);
      return conv_unique<RelocatableInst>(MovzxrAL(dest),
                                          Lea(dest, reg, 1, dest, 0, 0));
    }
    // Else replace the instruction with a LEA on the same address
    else if (memIndex >= 0) {
      unsigned realMemIndex = memIndex + llvm::X86II::getOperandBias(desc);

      QBDI_REQUIRE_ABORT_PATCH(realMemIndex + 4 < inst.getNumOperands(), patch,
                               "Unexpected number of operand {}",
                               realMemIndex + 4);
      QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(realMemIndex + 0).isReg(), patch,
                               "Unexpected operand type");
      QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(realMemIndex + 1).isImm(), patch,
                               "Unexpected operand type");
      QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(realMemIndex + 2).isReg(), patch,
                               "Unexpected operand type");
      QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(realMemIndex + 3).isImm(), patch,
                               "Unexpected operand type");
      QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(realMemIndex + 4).isReg(), patch,
                               "Unexpected operand type");

      // If it uses PC as a base register, substitute PC
      if (inst.getOperand(realMemIndex + 0).getReg() == GPR_ID[REG_PC]) {
        return conv_unique<RelocatableInst>(
            LoadImm::unique(temp_manager.getRegForTemp(0xFFFFFFFF),
                            Constant(patch.metadata.endAddress())),
            Lea(dest, temp_manager.getRegForTemp(0xFFFFFFFF),
                inst.getOperand(realMemIndex + 1).getImm(),
                inst.getOperand(realMemIndex + 2).getReg(),
                inst.getOperand(realMemIndex + 3).getImm(),
                inst.getOperand(realMemIndex + 4).getReg()));
      } else {
        return conv_unique<RelocatableInst>(
            Lea(dest, inst.getOperand(realMemIndex + 0).getReg(),
                inst.getOperand(realMemIndex + 1).getImm(),
                inst.getOperand(realMemIndex + 2).getReg(),
                inst.getOperand(realMemIndex + 3).getImm(),
                inst.getOperand(realMemIndex + 4).getReg()));
      }
    }
  }
  QBDI_ABORT_PATCH(patch,
                   "Called on an instruction which does not make read access");
}

// GetWriteAddress
// ===============

RelocatableInst::UniquePtrVec
GetWriteAddress::generate(const Patch &patch, TempManager &temp_manager) const {
  const llvm::MCInst &inst = patch.metadata.inst;
  // Check if this instruction does indeed read something
  unsigned size = getWriteSize(inst, *patch.llvmcpu);
  if (size > 0) {
    const llvm::MCInstrDesc &desc =
        patch.llvmcpu->getMCII().get(inst.getOpcode());
    uint64_t TSFlags = desc.TSFlags;
    unsigned FormDesc = TSFlags & llvm::X86II::FormMask;
    int memIndex = llvm::X86II::getMemoryOperandNo(TSFlags);
    unsigned opcode = inst.getOpcode();

    Reg dest = temp_manager.getRegForTemp(temp);
    // If it is a stack read, return RSP value
    if (isStackWrite(inst)) {
      if (inst.getOpcode() == llvm::X86::ENTER) {
        return conv_unique<RelocatableInst>(MovReg::unique(dest, Reg(REG_BP)));
      } else {
        return conv_unique<RelocatableInst>(MovReg::unique(dest, Reg(REG_SP)));
      }
    }
    // Implicit RSI or RDI
    else if (implicitDSIAccess(inst, desc)) {
      RegLLVM reg(0);
      if (FormDesc == llvm::X86II::RawFrmSrc) {
        // (R|E)SI
        reg = Reg(4);
        QBDI_REQUIRE(reg == llvm::X86::RSI || reg == llvm::X86::ESI);
      } else {
        // (R|E)DI
        reg = Reg(5);
        QBDI_REQUIRE(reg == llvm::X86::RDI || reg == llvm::X86::EDI);
      }
      return conv_unique<RelocatableInst>(MovReg::unique(dest, reg));
    }
    // Moffs access
    else if (FormDesc == llvm::X86II::RawFrmMemOffs) {
      QBDI_REQUIRE_ABORT_PATCH(2 <= inst.getNumOperands(), patch,
                               "Unexpected number of operand");
      QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(0).isImm(), patch,
                               "Unexpected operand type");
      QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(1).isReg(), patch,
                               "Unexpected operand type");
      return conv_unique<RelocatableInst>(Lea(dest, 0, 1, 0,
                                              inst.getOperand(0).getImm(),
                                              inst.getOperand(1).getReg()));
    }
    // MOVDIR64B instruction
    else if (opcode == llvm::X86::MOVDIR64B16 ||
             opcode == llvm::X86::MOVDIR64B32 ||
             opcode == llvm::X86::MOVDIR64B64) {
      QBDI_REQUIRE_ABORT_PATCH(0 < inst.getNumOperands(), patch,
                               "Unexpected number of operand");
      return conv_unique<RelocatableInst>(
          MovReg::unique(dest, inst.getOperand(0).getReg()));
    }
    // Else replace the instruction with a LEA on the same address
    else if (memIndex >= 0) {
      unsigned realMemIndex = memIndex + llvm::X86II::getOperandBias(desc);
      QBDI_REQUIRE_ABORT_PATCH(realMemIndex + 4 < inst.getNumOperands(), patch,
                               "Unexpected number of operand {}",
                               realMemIndex + 4);
      QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(realMemIndex + 0).isReg(), patch,
                               "Unexpected operand type");
      QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(realMemIndex + 1).isImm(), patch,
                               "Unexpected operand type");
      QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(realMemIndex + 2).isReg(), patch,
                               "Unexpected operand type");
      QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(realMemIndex + 3).isImm(), patch,
                               "Unexpected operand type");
      QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(realMemIndex + 4).isReg(), patch,
                               "Unexpected operand type");

      // If it uses PC as a base register, substitute PC
      if (inst.getOperand(realMemIndex + 0).getReg() == GPR_ID[REG_PC]) {
        return conv_unique<RelocatableInst>(
            LoadImm::unique(temp_manager.getRegForTemp(0xFFFFFFFF),
                            Constant(patch.metadata.endAddress())),
            Lea(dest, temp_manager.getRegForTemp(0xFFFFFFFF),
                inst.getOperand(realMemIndex + 1).getImm(),
                inst.getOperand(realMemIndex + 2).getReg(),
                inst.getOperand(realMemIndex + 3).getImm(),
                inst.getOperand(realMemIndex + 4).getReg()));
      } else {
        return conv_unique<RelocatableInst>(
            Lea(dest, inst.getOperand(realMemIndex + 0).getReg(),
                inst.getOperand(realMemIndex + 1).getImm(),
                inst.getOperand(realMemIndex + 2).getReg(),
                inst.getOperand(realMemIndex + 3).getImm(),
                inst.getOperand(realMemIndex + 4).getReg()));
      }
    }
  }
  QBDI_ABORT_PATCH(patch,
                   "Called on an instruction which does not make write access");
}

// GetReadValue
// ============

RelocatableInst::UniquePtrVec
GetReadValue::generate(const Patch &patch, TempManager &temp_manager) const {
  const llvm::MCInst &inst = patch.metadata.inst;
  const unsigned size = getReadSize(inst, *patch.llvmcpu);
  QBDI_REQUIRE_ABORT_PATCH(
      size > 0, patch,
      "Called on an instruction which does not make read access");

  RegLLVM dst = temp_manager.getRegForTemp(temp);

  if (patch.llvmcpu->hasOptions(Options::OPT_DISABLE_MEMORYACCESS_VALUE)) {
    return conv_unique<RelocatableInst>(Xorrr(dst, dst));
  } else if (is_bits_64 and size < sizeof(rword)) {
    dst = temp_manager.getSizedSubReg(dst, 4);
  } else if (size > sizeof(rword)) {
    return conv_unique<RelocatableInst>(Xorrr(dst, dst));
  }
  Reg addr = temp_manager.getRegForTemp(address);
  RegLLVM seg;

  const llvm::MCInstrDesc &desc =
      patch.llvmcpu->getMCII().get(inst.getOpcode());
  uint64_t TSFlags = desc.TSFlags;
  unsigned FormDesc = TSFlags & llvm::X86II::FormMask;

  if (isStackRead(inst) or implicitDSIAccess(inst, desc) or
      inst.getOpcode() == llvm::X86::XLAT) {
    seg = 0;
  } else if (FormDesc == llvm::X86II::RawFrmMemOffs) {
    QBDI_REQUIRE_ABORT_PATCH(1 < inst.getNumOperands(), patch,
                             "Unexpected number of operand");
    QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(1).isReg(), patch,
                             "Unexpected operand Type");
    seg = inst.getOperand(1).getReg();
  } else {
    int memIndex = llvm::X86II::getMemoryOperandNo(TSFlags);
    QBDI_REQUIRE_ABORT_PATCH(memIndex >= 0, patch,
                             "Fail to get memory access index");

    unsigned realMemIndex = memIndex + llvm::X86II::getOperandBias(desc);
    QBDI_REQUIRE_ABORT_PATCH(inst.getNumOperands() > realMemIndex + 4, patch,
                             "Invalid memory access index {}",
                             realMemIndex + 4);
    QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(realMemIndex + 4).isReg(), patch,
                             "Unexpected operand Type");
    seg = inst.getOperand(realMemIndex + 4).getReg();
  }

  if (size == 8) {
    return conv_unique<RelocatableInst>(Mov64rm(dst, addr, seg));
  } else if (size == 4) {
    return conv_unique<RelocatableInst>(Mov32rm(dst, addr, seg));
  } else if (size == 2) {
    return conv_unique<RelocatableInst>(Mov32rm16(dst, addr, seg));
  } else if (size == 1) {
    return conv_unique<RelocatableInst>(Mov32rm8(dst, addr, seg));
  } else {
    QBDI_ABORT_PATCH(patch, "Unsupported read size {}", size);
  }
}

// GetWriteValue
// =============

RelocatableInst::UniquePtrVec
GetWriteValue::generate(const Patch &patch, TempManager &temp_manager) const {

  const llvm::MCInst &inst = patch.metadata.inst;
  const unsigned size = getWriteSize(inst, *patch.llvmcpu);
  QBDI_REQUIRE_ABORT_PATCH(
      size > 0, patch,
      "Called on an instruction which does not make write access");

  RegLLVM dst = temp_manager.getRegForTemp(temp);

  if (patch.llvmcpu->hasOptions(Options::OPT_DISABLE_MEMORYACCESS_VALUE)) {
    return conv_unique<RelocatableInst>(Xorrr(dst, dst));
  } else if (is_bits_64 and size < sizeof(rword)) {
    dst = temp_manager.getSizedSubReg(dst, 4);
  } else if (size > sizeof(rword)) {
    return conv_unique<RelocatableInst>(Xorrr(dst, dst));
  }
  Reg addr = temp_manager.getRegForTemp(address);
  unsigned seg = 0;

  const llvm::MCInstrDesc &desc =
      patch.llvmcpu->getMCII().get(inst.getOpcode());
  uint64_t TSFlags = desc.TSFlags;
  unsigned FormDesc = TSFlags & llvm::X86II::FormMask;

  if (isStackWrite(inst) or implicitDSIAccess(inst, desc)) {
    seg = 0;
  } else if (FormDesc == llvm::X86II::RawFrmMemOffs) {
    QBDI_REQUIRE_ABORT_PATCH(1 < inst.getNumOperands(), patch,
                             "Unexpected number of operand");
    QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(1).isReg(), patch,
                             "Unexpected operand Type");
    seg = inst.getOperand(1).getReg();
  } else {
    int memIndex = llvm::X86II::getMemoryOperandNo(TSFlags);
    QBDI_REQUIRE_ABORT_PATCH(memIndex >= 0, patch,
                             "Fail to get memory access index");

    unsigned realMemIndex = memIndex + llvm::X86II::getOperandBias(desc);
    QBDI_REQUIRE_ABORT_PATCH(inst.getNumOperands() > realMemIndex + 4, patch,
                             "Invalid memory access index {}",
                             realMemIndex + 4);
    QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(realMemIndex + 4).isReg(), patch,
                             "Unexpected operand Type");
    seg = inst.getOperand(realMemIndex + 4).getReg();
  }

  if (size == 8) {
    return conv_unique<RelocatableInst>(Mov64rm(dst, addr, seg));
  } else if (size == 4) {
    return conv_unique<RelocatableInst>(Mov32rm(dst, addr, seg));
  } else if (size == 2) {
    return conv_unique<RelocatableInst>(Mov32rm16(dst, addr, seg));
  } else if (size == 1) {
    return conv_unique<RelocatableInst>(Mov32rm8(dst, addr, seg));
  } else {
    QBDI_ABORT_PATCH(patch, "Unsupported written size {}", size);
  }
}

} // namespace QBDI
