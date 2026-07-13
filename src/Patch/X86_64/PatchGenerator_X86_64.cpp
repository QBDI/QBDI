/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2026 Quarkslab
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
    QBDI_REQUIRE_ABORT(op < patch.metadata.inst.getNumOperands(),
                       "Invalid operand {} {}", op, patch);

    // FIXME: Implement for register operand
    QBDI_REQUIRE_ABORT(patch.metadata.inst.getOperand(op).isImm(),
                       "Unexpected operand type {}", patch);
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
  const unsigned opcode = patch.metadata.inst.getOpcode();
  RelocatableInst::UniquePtrVec p;

  if constexpr (is_x86_64) {
    QBDI_REQUIRE_ABORT(
        opcode != llvm::X86::CALL32r and opcode != llvm::X86::CALL32m and
            opcode != llvm::X86::CALLpcrel32,
        "CALL32r/CALL32m/CALLpcrel32 is not reachable when compiled for "
        "X86_64 {}",
        patch);
  } else {
    QBDI_REQUIRE_ABORT(
        opcode != llvm::X86::CALL64r and opcode != llvm::X86::CALL64m and
            opcode != llvm::X86::CALL64pcrel32,
        "CALL64r/CALL64m/CALL64pcrel32 is not reachable when compiled for "
        "X86 (32 bits) {}",
        patch);
  }

  append(p, WriteTemp(temp, Offset(Reg(REG_PC))).generate(patch, temp_manager));
  append(p, GetPCOffset(temp, Constant(0)).generate(patch, temp_manager));

  if (opcode == llvm::X86::CALLpcrel16 or opcode == llvm::X86::CALL16r or
      opcode == llvm::X86::CALL16m) {
    RegLLVM dst = temp_manager.getRegForTemp(temp);
    p.push_back(Push16r(temp_manager.getSizedSubReg(dst, 2)));
  } else {
    p.push_back(Pushr(temp_manager.getRegForTemp(temp)));
  }

  return p;
}

// SimulateRet
// ===========

RelocatableInst::UniquePtrVec
SimulateRet::generate(const Patch &patch, TempManager &temp_manager) const {
  const llvm::MCInst &inst = patch.metadata.inst;
  const unsigned opcode = inst.getOpcode();
  RelocatableInst::UniquePtrVec p;

  if constexpr (!is_x86_64) {
    QBDI_REQUIRE_ABORT(
        opcode != llvm::X86::RET64 and opcode != llvm::X86::RETI64,
        "RET64/RETI64 is not reachable when compiled for X86 (32 bits) {}",
        patch);
  } else {
    QBDI_REQUIRE_ABORT(
        opcode != llvm::X86::RET32 and opcode != llvm::X86::RETI32,
        "RET32/RETI32 is not reachable when compiled for X86_64 {}", patch);
  }

  const bool isNativeOperandSize =
      is_x86_64 ? (opcode == llvm::X86::RET64 or opcode == llvm::X86::RETI64)
                : (opcode == llvm::X86::RET32 or opcode == llvm::X86::RETI32);

  if (isNativeOperandSize) {
    p.push_back(Popr(temp_manager.getRegForTemp(temp)));
  } else {
    RegLLVM dst = temp_manager.getRegForTemp(temp);
    p.push_back(Xorrr(dst, dst));
    p.push_back(Pop16r(temp_manager.getSizedSubReg(dst, 2)));
  }

  if (inst.getNumOperands() == 1 and inst.getOperand(0).isImm()) {
    p.push_back(
        Add(Reg(REG_SP), Reg(REG_SP), Constant(inst.getOperand(0).getImm())));
  }
  append(p, WriteTemp(temp, Offset(Reg(REG_PC))).generate(patch, temp_manager));

  return p;
}

// SimulateLret
// ============

RelocatableInst::UniquePtrVec
SimulateLret::generate(const Patch &patch, TempManager &temp_manager) const {
  const llvm::MCInst &inst = patch.metadata.inst;
  const unsigned opcode = inst.getOpcode();
  RelocatableInst::UniquePtrVec p;

  if constexpr (!is_x86_64) {
    QBDI_REQUIRE_ABORT(
        opcode != llvm::X86::LRET64 and opcode != llvm::X86::LRETI64,
        "LRET64/LRETI64 is not reachable when compiled for X86 (32 bits) {}",
        patch);
  }

  const unsigned operandSize =
      (opcode == llvm::X86::LRET64 or opcode == llvm::X86::LRETI64)   ? 8
      : (opcode == llvm::X86::LRET32 or opcode == llvm::X86::LRETI32) ? 4
                                                                      : 2;

  RegLLVM dst = temp_manager.getRegForTemp(temp);
  if (operandSize == 8) {
    p.push_back(Mov64rm(dst, Reg(REG_SP), 0));
  } else if (operandSize == 4) {
    if constexpr (is_x86_64) {
      dst = temp_manager.getSizedSubReg(dst, 4);
    }
    p.push_back(Mov32rm(dst, Reg(REG_SP), 0));
  } else {
    if constexpr (is_x86_64) {
      dst = temp_manager.getSizedSubReg(dst, 4);
    }
    p.push_back(Mov32rm16(dst, Reg(REG_SP), 0));
  }

  rword stackDelta = 2 * operandSize;
  if (inst.getNumOperands() == 1 and inst.getOperand(0).isImm()) {
    stackDelta += inst.getOperand(0).getImm();
  }
  p.push_back(Add(Reg(REG_SP), Reg(REG_SP), Constant(stackDelta)));

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
      QBDI_REQUIRE_ABORT(index < 2, "Wrong index {} {}", index, patch);
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
      QBDI_REQUIRE_ABORT(1 < inst.getNumOperands(),
                         "Unexpected number of operand {}", patch);
      QBDI_REQUIRE_ABORT(inst.getOperand(0).isImm(),
                         "Unexpected operand type {}", patch);
      QBDI_REQUIRE_ABORT(inst.getOperand(1).isReg(),
                         "Unexpected operand type {}", patch);
      return conv_unique<RelocatableInst>(
          LoadImm::unique(dest, Constant(inst.getOperand(0).getImm())));
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

      QBDI_REQUIRE_ABORT(realMemIndex + 4 < inst.getNumOperands(),
                         "Unexpected number of operand {} {}", realMemIndex + 4,
                         patch);
      QBDI_REQUIRE_ABORT(inst.getOperand(realMemIndex + 0).isReg(),
                         "Unexpected operand type {}", patch);
      QBDI_REQUIRE_ABORT(inst.getOperand(realMemIndex + 1).isImm(),
                         "Unexpected operand type {}", patch);
      QBDI_REQUIRE_ABORT(inst.getOperand(realMemIndex + 2).isReg(),
                         "Unexpected operand type {}", patch);
      QBDI_REQUIRE_ABORT(inst.getOperand(realMemIndex + 3).isImm(),
                         "Unexpected operand type {}", patch);
      QBDI_REQUIRE_ABORT(inst.getOperand(realMemIndex + 4).isReg(),
                         "Unexpected operand type {}", patch);

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
  QBDI_ABORT("Called on an instruction which does not make read access {}",
             patch);
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
      QBDI_REQUIRE_ABORT(2 <= inst.getNumOperands(),
                         "Unexpected number of operand {}", patch);
      QBDI_REQUIRE_ABORT(inst.getOperand(0).isImm(),
                         "Unexpected operand type {}", patch);
      QBDI_REQUIRE_ABORT(inst.getOperand(1).isReg(),
                         "Unexpected operand type {}", patch);
      return conv_unique<RelocatableInst>(
          LoadImm::unique(dest, Constant(inst.getOperand(0).getImm())));
    }
    // MOVDIR64B instruction
    else if (opcode == llvm::X86::MOVDIR64B16 ||
             opcode == llvm::X86::MOVDIR64B32 ||
             opcode == llvm::X86::MOVDIR64B64 ||
             opcode == llvm::X86::MOVDIR64B32_EVEX ||
             opcode == llvm::X86::MOVDIR64B64_EVEX) {
      QBDI_REQUIRE_ABORT(0 < inst.getNumOperands(),
                         "Unexpected number of operand {}", patch);
      return conv_unique<RelocatableInst>(
          MovReg::unique(dest, inst.getOperand(0).getReg()));
    }
    // Else replace the instruction with a LEA on the same address
    else if (memIndex >= 0) {
      unsigned realMemIndex = memIndex + llvm::X86II::getOperandBias(desc);
      QBDI_REQUIRE_ABORT(realMemIndex + 4 < inst.getNumOperands(),
                         "Unexpected number of operand {} {}", realMemIndex + 4,
                         patch);
      QBDI_REQUIRE_ABORT(inst.getOperand(realMemIndex + 0).isReg(),
                         "Unexpected operand type {}", patch);
      QBDI_REQUIRE_ABORT(inst.getOperand(realMemIndex + 1).isImm(),
                         "Unexpected operand type {}", patch);
      QBDI_REQUIRE_ABORT(inst.getOperand(realMemIndex + 2).isReg(),
                         "Unexpected operand type {}", patch);
      QBDI_REQUIRE_ABORT(inst.getOperand(realMemIndex + 3).isImm(),
                         "Unexpected operand type {}", patch);
      QBDI_REQUIRE_ABORT(inst.getOperand(realMemIndex + 4).isReg(),
                         "Unexpected operand type {}", patch);

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
  QBDI_ABORT("Called on an instruction which does not make write access {}",
             patch);
}

// GetReadValue
// ============

RelocatableInst::UniquePtrVec
GetReadValue::generate(const Patch &patch, TempManager &temp_manager) const {
  const llvm::MCInst &inst = patch.metadata.inst;
  const unsigned size = getReadSize(inst, *patch.llvmcpu);
  QBDI_REQUIRE_ABORT(
      size > 0, "Called on an instruction which does not make read access {}",
      patch);

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
    QBDI_REQUIRE_ABORT(1 < inst.getNumOperands(),
                       "Unexpected number of operand {}", patch);
    QBDI_REQUIRE_ABORT(inst.getOperand(1).isReg(), "Unexpected operand Type {}",
                       patch);
    seg = inst.getOperand(1).getReg();
  } else {
    int memIndex = llvm::X86II::getMemoryOperandNo(TSFlags);
    QBDI_REQUIRE_ABORT(memIndex >= 0, "Fail to get memory access index {}",
                       patch);

    unsigned realMemIndex = memIndex + llvm::X86II::getOperandBias(desc);
    QBDI_REQUIRE_ABORT(inst.getNumOperands() > realMemIndex + 4,
                       "Invalid memory access index {} {}", realMemIndex + 4,
                       patch);
    QBDI_REQUIRE_ABORT(inst.getOperand(realMemIndex + 4).isReg(),
                       "Unexpected operand Type {}", patch);
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
    QBDI_ABORT("Unsupported read size {} {}", size, patch);
  }
}

// GetWriteValue
// =============

RelocatableInst::UniquePtrVec
GetWriteValue::generate(const Patch &patch, TempManager &temp_manager) const {

  const llvm::MCInst &inst = patch.metadata.inst;
  const unsigned size = getWriteSize(inst, *patch.llvmcpu);
  QBDI_REQUIRE_ABORT(
      size > 0, "Called on an instruction which does not make write access {}",
      patch);

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
    QBDI_REQUIRE_ABORT(1 < inst.getNumOperands(),
                       "Unexpected number of operand {}", patch);
    QBDI_REQUIRE_ABORT(inst.getOperand(1).isReg(), "Unexpected operand Type {}",
                       patch);
    seg = inst.getOperand(1).getReg();
  } else {
    int memIndex = llvm::X86II::getMemoryOperandNo(TSFlags);
    QBDI_REQUIRE_ABORT(memIndex >= 0, "Fail to get memory access index {}",
                       patch);

    unsigned realMemIndex = memIndex + llvm::X86II::getOperandBias(desc);
    QBDI_REQUIRE_ABORT(inst.getNumOperands() > realMemIndex + 4,
                       "Invalid memory access index {} {}", realMemIndex + 4,
                       patch);
    QBDI_REQUIRE_ABORT(inst.getOperand(realMemIndex + 4).isReg(),
                       "Unexpected operand Type {}", patch);
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
    QBDI_ABORT("Unsupported written size {} {}", size, patch);
  }
}

} // namespace QBDI
