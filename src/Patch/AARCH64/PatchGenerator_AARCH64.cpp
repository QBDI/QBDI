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

#include "Target/AArch64/AArch64Subtarget.h"
#include "Target/AArch64/MCTargetDesc/AArch64AddressingModes.h"
#include "llvm/MC/MCInst.h"

#include "Engine/LLVMCPU.h"
#include "Patch/AARCH64/InstInfo_AARCH64.h"
#include "Patch/AARCH64/Layer2_AARCH64.h"
#include "Patch/AARCH64/MemoryAccess_AARCH64.h"
#include "Patch/AARCH64/PatchGenerator_AARCH64.h"
#include "Patch/AARCH64/RelocatableInst_AARCH64.h"
#include "Patch/InstInfo.h"
#include "Patch/Patch.h"
#include "Patch/RelocatableInst.h"
#include "Patch/TempManager.h"
#include "Patch/Types.h"
#include "Utility/LogSys.h"

namespace QBDI {

template <typename T>
RelocatableInst::UniquePtrVec
PureEval<T>::generate(const Patch &patch, TempManager &temp_manager) const {
  return this->genReloc(*patch.llvmcpu);
}

template RelocatableInst::UniquePtrVec
PureEval<AutoClone<PatchGenerator, FullRegisterRestore>>::generate(
    const Patch &patch, TempManager &temp_manager) const;
template RelocatableInst::UniquePtrVec
PureEval<AutoClone<PatchGenerator, FullRegisterReset>>::generate(
    const Patch &patch, TempManager &temp_manager) const;
template RelocatableInst::UniquePtrVec
PureEval<AutoClone<PatchGenerator, GenBTI>>::generate(
    const Patch &patch, TempManager &temp_manager) const;

// Generic PatchGenerator that must be implemented by each target

// TargetPrologue
// ==============

RelocatableInst::UniquePtrVec
TargetPrologue::genReloc(const Patch &patch) const {

  RelocatableInst::UniquePtrVec p = GenBTI().genReloc(*patch.llvmcpu);

  // If the instruction uses X28, restore it
  if (patch.regUsage[28] != 0) {
    append(p, LoadReg(Reg(28), Offset(Reg(28))).genReloc(*patch.llvmcpu));
  }

  return p;
}

// JmpEpilogue
// ===========

RelocatableInst::UniquePtrVec
JmpEpilogue::genReloc(const LLVMCPU &llvmcpu) const {

  return conv_unique<RelocatableInst>(EpilogueAddrRel::unique(branch(0), 0, 0));
}

// Target Specific PatchGenerator

// SimulateLink
// ============

RelocatableInst::UniquePtrVec
SimulateLink::generate(const Patch &patch, TempManager &temp_manager) const {

  Reg tmp = temp_manager.getRegForTemp(temp);
  return conv_unique<RelocatableInst>(
      LoadImm::unique(tmp, Constant(patch.metadata.endAddress())),
      MovReg::unique(Reg(REG_LR), tmp));
}

// GetPCOffset
// ===========

RelocatableInst::UniquePtrVec
GetPCOffset::generate(const Patch &patch, TempManager &temp_manager) const {

  RegLLVM dst(0);
  const llvm::MCInst &inst = patch.metadata.inst;

  switch (type) {
    case TempConstantType:
    case TempOperandType:
      dst = temp_manager.getRegForTemp(temp);
      break;
    case OperandOperandType:
      QBDI_REQUIRE_ABORT_PATCH(opdst < inst.getNumOperands(), patch,
                               "Invalid operand {}", opdst);
      QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(opdst).isReg(), patch,
                               "Unexpected operand type");
      dst = inst.getOperand(opdst).getReg();
      break;
    default:
      QBDI_ABORT_PATCH(patch, "Logical Error");
  }
  switch (type) {
    case TempConstantType: {
      rword value = patch.metadata.address + cst;
      return conv_unique<RelocatableInst>(
          LoadImm::unique(dst, Constant(value)));
    }
    case OperandOperandType:
    case TempOperandType: {
      QBDI_REQUIRE_ABORT_PATCH(opsrc < inst.getNumOperands(), patch,
                               "Invalid operand {}", opsrc);
      if (inst.getOperand(opsrc).isImm()) {
        sword imm = inst.getOperand(opsrc).getImm();
        switch (inst.getOpcode()) {
          case llvm::AArch64::ADRP:
            imm = imm * 0x1000;
            break;
          case llvm::AArch64::B:
          case llvm::AArch64::BL:
          case llvm::AArch64::Bcc:
          case llvm::AArch64::BCcc:
          case llvm::AArch64::CBNZW:
          case llvm::AArch64::CBNZX:
          case llvm::AArch64::CBZW:
          case llvm::AArch64::CBZX:
          case llvm::AArch64::TBNZW:
          case llvm::AArch64::TBNZX:
          case llvm::AArch64::TBZW:
          case llvm::AArch64::TBZX:
          case llvm::AArch64::LDRSl:
          case llvm::AArch64::LDRDl:
          case llvm::AArch64::LDRQl:
          case llvm::AArch64::LDRXl:
          case llvm::AArch64::LDRWl:
          case llvm::AArch64::LDRSWl:
            imm = imm * 4;
            break;
          default:
            break;
        }
        rword real_addr = patch.metadata.address;
        if (inst.getOpcode() == llvm::AArch64::ADRP) {
          // Clear the lower 12-bits
          real_addr = patch.metadata.address & ~(0xFFFllu);
        }
        rword value = real_addr + imm;
        return conv_unique<RelocatableInst>(
            LoadImm::unique(dst, Constant(value)));
      }

      // if (inst.getOperand(opsrc).isReg()) {
      //   RegLLVM reg = inst.getOperand(opsrc).getReg();

      //   return conv_unique<RelocatableInst>(
      //       LoadImm::unique(dst, Constant(patch.metadata.address)),
      //       Add(dst, reg));
      // }
      QBDI_ABORT_PATCH(patch, "Unsupported Operand type");
    }
    default: {
      QBDI_ABORT_PATCH(patch, "Logical Error");
    }
  }
  _QBDI_UNREACHABLE();
}

// SaveX28IfSet
// ============
RelocatableInst::UniquePtrVec
SaveX28IfSet::generate(const Patch &patch, TempManager &temp_manager) const {

  const Patch &p = temp_manager.getPatch();
  if ((p.regUsage[28] & RegisterSet) != 0) {
    return SaveReg(Reg(28), Offset(Reg(28))).genReloc(*patch.llvmcpu);
  }

  return RelocatableInst::UniquePtrVec();
}

// CondExclusifLoad
// ================
RelocatableInst::UniquePtrVec
CondExclusifLoad::generate(const Patch &patch,
                           TempManager &temp_manager) const {

  static constexpr int JUMP_OFFSET = 12;

  RelocatableInst::UniquePtrVec p;
  Reg tmpReg = temp_manager.getRegForTemp(tmp);

  // load enable flag
  p.push_back(
      Ldr(tmpReg, Offset(offsetof(Context, gprState.localMonitor.enable))));

  // if monitor in exclusive mode (if cond == 0 => jump)
  p.push_back(Cbz(tmpReg, Constant(JUMP_OFFSET)));

  // {
  //  do a exclusive load. reuse the tmp register to store the result
  p.push_back(
      Ldr(tmpReg, Offset(offsetof(Context, gprState.localMonitor.addr))));
  p.push_back(Ldxrb(tmpReg, tmpReg));
  // }

  return p;
}

// GetReadAddress
// ==============

RelocatableInst::UniquePtrVec
GetReadAddress::generate(const Patch &patch, TempManager &temp_manager) const {

  Reg tmpRegister = temp_manager.getRegForTemp(temp);
  return generateAddressPatch(patch, false, tmpRegister);
}

// GetWrittenAddress
// =================

RelocatableInst::UniquePtrVec
GetWrittenAddress::generate(const Patch &patch,
                            TempManager &temp_manager) const {

  Reg tmpRegister = temp_manager.getRegForTemp(temp);
  return generateAddressPatch(patch, true, tmpRegister);
}

// GetReadValue
// ============

RelocatableInst::UniquePtrVec
GetReadValue::generate(const Patch &patch, TempManager &temp_manager) const {

  Reg tmpRegister = temp_manager.getRegForTemp(temp);

  if (patch.llvmcpu->hasOptions(Options::OPT_DISABLE_MEMORYACCESS_VALUE)) {
    if (index == 0) {
      return conv_unique<RelocatableInst>(Mov(tmpRegister, Constant(0)));
    } else {
      return {};
    }
  }

  Reg addrRegister = temp_manager.getRegForTemp(addr);
  unsigned readSize = getReadSize(patch.metadata.inst, *patch.llvmcpu);
  switch (readSize) {
    case 1:
      return conv_unique<RelocatableInst>(Ldrb(tmpRegister, addrRegister, 0));
    case 2:
      return conv_unique<RelocatableInst>(Ldrh(tmpRegister, addrRegister, 0));
    case 3: {
      Reg orRegister = temp_manager.getRegForTemp(Temp(0xffff));
      return conv_unique<RelocatableInst>(
          Ldrb(orRegister, addrRegister, 2), Ldrh(tmpRegister, addrRegister, 0),
          Orrs(tmpRegister, tmpRegister, orRegister, 16));
    }
    case 4:
      return conv_unique<RelocatableInst>(Ldrw(tmpRegister, addrRegister, 0));
    case 6: {
      Reg orRegister = temp_manager.getRegForTemp(Temp(0xffff));
      return conv_unique<RelocatableInst>(
          Ldrh(orRegister, addrRegister, 4), Ldrw(tmpRegister, addrRegister, 0),
          Orrs(tmpRegister, tmpRegister, orRegister, 32));
    }
    case 8:
    case 16:
    case 24:
    case 32:
    case 48:
    case 64:
      return conv_unique<RelocatableInst>(
          Ldr(tmpRegister, addrRegister, index * 8));
    case 12:
      if (index == 0) {
        return conv_unique<RelocatableInst>(Ldr(tmpRegister, addrRegister, 0));
      } else {
        return conv_unique<RelocatableInst>(Ldrw(tmpRegister, addrRegister, 8));
      }
    default:
      QBDI_ABORT_PATCH(patch, "Unexpected Read Size {}", readSize);
  }
}

// GetWrittenValue
// ===============

RelocatableInst::UniquePtrVec
GetWrittenValue::generate(const Patch &patch, TempManager &temp_manager) const {

  Reg tmpRegister = temp_manager.getRegForTemp(temp);

  if (patch.llvmcpu->hasOptions(Options::OPT_DISABLE_MEMORYACCESS_VALUE)) {
    if (index == 0) {
      return conv_unique<RelocatableInst>(Mov(tmpRegister, Constant(0)));
    } else {
      return {};
    }
  }

  Reg addrRegister = temp_manager.getRegForTemp(addr);
  unsigned writtenSize = getWriteSize(patch.metadata.inst, *patch.llvmcpu);
  switch (writtenSize) {
    case 1:
      return conv_unique<RelocatableInst>(Ldrb(tmpRegister, addrRegister, 0));
    case 2:
      return conv_unique<RelocatableInst>(Ldrh(tmpRegister, addrRegister, 0));
    case 3: {
      Reg orRegister = temp_manager.getRegForTemp(Temp(0xffff));
      return conv_unique<RelocatableInst>(
          Ldrh(tmpRegister, addrRegister, 0), Ldrb(orRegister, addrRegister, 2),
          Orrs(tmpRegister, tmpRegister, orRegister, 16));
    }
    case 4:
      return conv_unique<RelocatableInst>(Ldrw(tmpRegister, addrRegister, 0));
    case 6: {
      Reg orRegister = temp_manager.getRegForTemp(Temp(0xffff));
      return conv_unique<RelocatableInst>(
          Ldrw(tmpRegister, addrRegister, 0), Ldrh(orRegister, addrRegister, 4),
          Orrs(tmpRegister, tmpRegister, orRegister, 32));
    }
    case 8:
    case 16:
    case 24:
    case 32:
    case 48:
    case 64:
      return conv_unique<RelocatableInst>(
          Ldr(tmpRegister, addrRegister, index * 8));
    case 12:
      if (index == 0) {
        return conv_unique<RelocatableInst>(Ldr(tmpRegister, addrRegister, 0));
      } else {
        return conv_unique<RelocatableInst>(Ldrw(tmpRegister, addrRegister, 8));
      }
    default:
      QBDI_ABORT_PATCH(patch, "Unexpected Written Size {}", writtenSize);
  }
}

// GetReadValueX2
// ==============

RelocatableInst::UniquePtrVec
GetReadValueX2::generate(const Patch &patch, TempManager &temp_manager) const {

  Reg tmpRegister = temp_manager.getRegForTemp(temp);
  Reg tmpRegister2 = temp_manager.getRegForTemp(temp2);

  if (patch.llvmcpu->hasOptions(Options::OPT_DISABLE_MEMORYACCESS_VALUE)) {
    if (index == 0) {
      return conv_unique<RelocatableInst>(Mov(tmpRegister, Constant(0)),
                                          Mov(tmpRegister2, Constant(0)));
    } else {
      return {};
    }
  }

  Reg addrRegister = temp_manager.getRegForTemp(addr);
  unsigned readSize = getReadSize(patch.metadata.inst, *patch.llvmcpu);
  switch (readSize) {
    case 24:
      if (index == 0) {
        return conv_unique<RelocatableInst>(
            Ldp(tmpRegister, tmpRegister2, addrRegister, index * 8));
      } else {
        QBDI_ABORT_PATCH(patch, "Unsupported index {} for read size 24", index);
      }
    case 16:
    case 32:
    case 48:
    case 64:
      return conv_unique<RelocatableInst>(
          Ldp(tmpRegister, tmpRegister2, addrRegister, index * 8));
    case 1:
    case 2:
    case 3:
    case 4:
    case 6:
    case 8:
    case 12:
      QBDI_ABORT_PATCH(patch, "Unsupported Read Size {}", readSize);
    default:
      QBDI_ABORT_PATCH(patch, "Unexpected Read Size {}", readSize);
  }
}

// GetWrittenValueX2
// =================

RelocatableInst::UniquePtrVec
GetWrittenValueX2::generate(const Patch &patch,
                            TempManager &temp_manager) const {

  Reg tmpRegister = temp_manager.getRegForTemp(temp);
  Reg tmpRegister2 = temp_manager.getRegForTemp(temp2);

  if (patch.llvmcpu->hasOptions(Options::OPT_DISABLE_MEMORYACCESS_VALUE)) {
    if (index == 0) {
      return conv_unique<RelocatableInst>(Mov(tmpRegister, Constant(0)),
                                          Mov(tmpRegister2, Constant(0)));
    } else {
      return {};
    }
  }

  Reg addrRegister = temp_manager.getRegForTemp(addr);
  unsigned writtenSize = getWriteSize(patch.metadata.inst, *patch.llvmcpu);
  switch (writtenSize) {
    case 24:
      if (index == 0) {
        return conv_unique<RelocatableInst>(
            Ldp(tmpRegister, tmpRegister2, addrRegister, index * 8));
      } else {
        QBDI_ABORT_PATCH(patch, "Unsupported index {} for written size 24",
                         index);
      }
    case 16:
    case 32:
    case 48:
    case 64:
      return conv_unique<RelocatableInst>(
          Ldp(tmpRegister, tmpRegister2, addrRegister, index * 8));
    case 1:
    case 2:
    case 3:
    case 4:
    case 6:
    case 8:
    case 12:
      QBDI_ABORT_PATCH(patch, "Unsupported Writren Size {}", writtenSize);
    default:
      QBDI_ABORT_PATCH(patch, "Unexpected Written Size {}", writtenSize);
  }
}

// FullRegisterRestore
// ===================

RelocatableInst::UniquePtrVec
FullRegisterRestore::genReloc(const LLVMCPU &llvmcpu) const {

  if (restoreX28) {
    return conv_unique<RelocatableInst>(
        ReadTPIDR(Reg(28)),
        Str(Reg(28), Offset(offsetof(Context, hostState.tpidr))),
        Ldr(Reg(28), Offset(Reg(28))), RestoreScratchRegister::unique());
  } else {
    return conv_unique<RelocatableInst>(RestoreScratchRegister::unique());
  }
}

// FullRegisterReset
// =================

RelocatableInst::UniquePtrVec
FullRegisterReset::genReloc(const LLVMCPU &llvmcpu) const {
  if (restoreX28) {
    return conv_unique<RelocatableInst>(
        WriteSRinTPIDR(), ResetScratchRegister::unique(),
        Str(Reg(28), Offset(Reg(28))), ReadTPIDR(Reg(28)),
        Str(Reg(28), Offset(offsetof(Context, hostState.scratchRegisterValue))),
        Ldr(Reg(28), Offset(offsetof(Context, hostState.tpidr))),
        WriteTPIDR(Reg(28)));
  } else {
    return conv_unique<RelocatableInst>(
        SetScratchRegister::unique(movrr(Reg(28), 0), 2),
        ResetScratchRegister::unique(),
        Str(Reg(28),
            Offset(offsetof(Context, hostState.scratchRegisterValue))));
  }
}

// GetAddrAuth
// ===========

RelocatableInst::UniquePtrVec
GetAddrAuth::generate(const Patch &patch, TempManager &temp_manager) const {

  RegLLVM dst(0);
  const llvm::MCInst &inst = patch.metadata.inst;
  switch (type) {
    case TempType:
      dst = temp_manager.getRegForTemp(temp);
      break;
    case OperandType:
      QBDI_REQUIRE_ABORT_PATCH(op < inst.getNumOperands(), patch,
                               "Invalid operand {}", op);
      QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(op).isReg(), patch,
                               "Unexpected operand type");
      dst = inst.getOperand(op).getReg();
      break;
    default:
      QBDI_ABORT_PATCH(patch, "unexpected type");
  }

  if (bypass) {
    switch (inst.getOpcode()) {
      case llvm::AArch64::LDRAAindexed:
      case llvm::AArch64::LDRABindexed: {
        QBDI_REQUIRE_ABORT_PATCH(2 < inst.getNumOperands(), patch,
                                 "Invalid operand");
        QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(1).isReg(), patch,
                                 "Unexpected operand type");
        QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(2).isImm(), patch,
                                 "Unexpected operand type");
        RegLLVM src = inst.getOperand(1).getReg();
        sword imm = inst.getOperand(2).getImm() * 8;
        if (src != dst)
          return conv_unique<RelocatableInst>(MovReg::unique(dst, src),
                                              Xpaci(dst),
                                              Add(dst, dst, Constant(imm)));
        else
          return conv_unique<RelocatableInst>(Xpaci(dst),
                                              Add(dst, dst, Constant(imm)));
      }
      case llvm::AArch64::LDRAAwriteback:
      case llvm::AArch64::LDRABwriteback: {
        QBDI_REQUIRE_ABORT_PATCH(3 < inst.getNumOperands(), patch,
                                 "Invalid operand");
        QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(2).isReg(), patch,
                                 "Unexpected operand type");
        QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(3).isImm(), patch,
                                 "Unexpected operand type");
        RegLLVM src = inst.getOperand(2).getReg();
        sword imm = inst.getOperand(3).getImm() * 8;
        if (src != dst)
          return conv_unique<RelocatableInst>(MovReg::unique(dst, src),
                                              Xpaci(dst),
                                              Add(dst, dst, Constant(imm)));
        else
          return conv_unique<RelocatableInst>(Xpaci(dst),
                                              Add(dst, dst, Constant(imm)));
      }
      case llvm::AArch64::BRAA:
      case llvm::AArch64::BRAB:
      case llvm::AArch64::BRAAZ:
      case llvm::AArch64::BRABZ:
      case llvm::AArch64::BLRAA:
      case llvm::AArch64::BLRAB:
      case llvm::AArch64::BLRAAZ:
      case llvm::AArch64::BLRABZ: {
        QBDI_REQUIRE_ABORT_PATCH(0 < inst.getNumOperands(), patch,
                                 "Invalid operand");
        QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(0).isReg(), patch,
                                 "Unexpected operand type");
        RegLLVM src = inst.getOperand(0).getReg();
        if (src != dst)
          return conv_unique<RelocatableInst>(MovReg::unique(dst, src),
                                              Xpaci(dst));
        else
          return conv_unique<RelocatableInst>(Xpaci(dst));
      }
      case llvm::AArch64::RETAA:
      case llvm::AArch64::RETAB:
        if (dst != Reg(30))
          return conv_unique<RelocatableInst>(MovReg::unique(dst, Reg(30)),
                                              Xpaci(dst));
        else
          return conv_unique<RelocatableInst>(Xpaci(dst));
      default:
        QBDI_ABORT_PATCH(patch, "Unexpected opcode {}", inst.getOpcode());
    }
  } else {
    switch (inst.getOpcode()) {
      case llvm::AArch64::BRAA:
      case llvm::AArch64::BLRAA: {
        QBDI_REQUIRE_ABORT_PATCH(1 < inst.getNumOperands(), patch,
                                 "Invalid operand");
        QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(0).isReg(), patch,
                                 "Unexpected operand type");
        QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(1).isReg(), patch,
                                 "Unexpected operand type");
        RegLLVM src = inst.getOperand(0).getReg();
        RegLLVM ctx = inst.getOperand(1).getReg();
        if (src != dst)
          return conv_unique<RelocatableInst>(MovReg::unique(dst, src),
                                              Autia(dst, ctx));
        else
          return conv_unique<RelocatableInst>(Autia(dst, ctx));
      }
      case llvm::AArch64::BRAB:
      case llvm::AArch64::BLRAB: {
        QBDI_REQUIRE_ABORT_PATCH(1 < inst.getNumOperands(), patch,
                                 "Invalid operand");
        QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(0).isReg(), patch,
                                 "Unexpected operand type");
        QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(1).isReg(), patch,
                                 "Unexpected operand type");
        RegLLVM src = inst.getOperand(0).getReg();
        RegLLVM ctx = inst.getOperand(1).getReg();
        if (src != dst)
          return conv_unique<RelocatableInst>(MovReg::unique(dst, src),
                                              Autib(dst, ctx));
        else
          return conv_unique<RelocatableInst>(Autib(dst, ctx));
      }
      case llvm::AArch64::BRAAZ:
      case llvm::AArch64::BLRAAZ: {
        QBDI_REQUIRE_ABORT_PATCH(0 < inst.getNumOperands(), patch,
                                 "Invalid operand");
        QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(0).isReg(), patch,
                                 "Unexpected operand type");
        RegLLVM src = inst.getOperand(0).getReg();
        if (src != dst)
          return conv_unique<RelocatableInst>(MovReg::unique(dst, src),
                                              Autiza(dst));
        else
          return conv_unique<RelocatableInst>(Autiza(dst));
      }
      case llvm::AArch64::BRABZ:
      case llvm::AArch64::BLRABZ: {
        QBDI_REQUIRE_ABORT_PATCH(0 < inst.getNumOperands(), patch,
                                 "Invalid operand");
        QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(0).isReg(), patch,
                                 "Unexpected operand type");
        RegLLVM src = inst.getOperand(0).getReg();
        if (src != dst)
          return conv_unique<RelocatableInst>(MovReg::unique(dst, src),
                                              Autizb(dst));
        else
          return conv_unique<RelocatableInst>(Autizb(dst));
      }
      case llvm::AArch64::RETAA:
        if (dst != Reg(30))
          return conv_unique<RelocatableInst>(MovReg::unique(dst, Reg(30)),
                                              Autia(dst, Reg(31)));
        else
          return conv_unique<RelocatableInst>(Autia(dst, Reg(31)));
      case llvm::AArch64::RETAB:
        if (dst != Reg(30))
          return conv_unique<RelocatableInst>(MovReg::unique(dst, Reg(30)),
                                              Autib(dst, Reg(31)));
        else
          return conv_unique<RelocatableInst>(Autib(dst, Reg(31)));

      default:
        QBDI_ABORT_PATCH(patch, "Unexpected opcode {}", inst.getOpcode());
    }
  }
}

// GenBTI
// ======

RelocatableInst::UniquePtrVec GenBTI::genReloc(const LLVMCPU &llvmcpu) const {

  if (llvmcpu.hasOptions(Options::OPT_ENABLE_BTI)) {
    return conv_unique<RelocatableInst>(BTIj());
  }
  return {};
}

} // namespace QBDI
