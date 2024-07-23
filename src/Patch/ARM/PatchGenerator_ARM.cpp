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

#include "Target/ARM/MCTargetDesc/ARMAddressingModes.h"
#include "Target/ARM/Utils/ARMBaseInfo.h"
#include "llvm/MC/MCInst.h"

#include "Engine/LLVMCPU.h"
#include "Patch/ARM/InstInfo_ARM.h"
#include "Patch/ARM/Layer2_ARM.h"
#include "Patch/ARM/MemoryAccess_ARM.h"
#include "Patch/ARM/PatchGenerator_ARM.h"
#include "Patch/ARM/RelocatableInst_ARM.h"
#include "Patch/ARM/TempManager_ARM.h"
#include "Patch/InstInfo.h"
#include "Patch/InstTransform.h"
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
PureEval<AutoClone<PatchGenerator, SetDataBlockAddress>>::generate(
    const Patch &patch, TempManager &temp_manager) const;

// Generic PatchGenerator that must be implemented by each target

// TargetPrologue
// ==============

RelocatableInst::UniquePtrVec
TargetPrologue::genReloc(const Patch &patch) const {

  return {};
}

// JmpEpilogue
// ===========

RelocatableInst::UniquePtrVec
JmpEpilogue::genReloc(const LLVMCPU &llvmcpu) const {

  if (llvmcpu == CPUMode::ARM) {
    return conv_unique<RelocatableInst>(EpilogueBranch::unique());
  } else {
    return conv_unique<RelocatableInst>(SetSREpilogue::unique(),
                                        SRBranch::unique());
  }
}

// Target Specific PatchGenerator

// SetDataBlockAddress
// ===================

RelocatableInst::UniquePtrVec
SetDataBlockAddress::genReloc(const LLVMCPU &llvmcpu) const {
  CPUMode cpumode = llvmcpu.getCPUMode();

  if (cpumode == CPUMode::Thumb) {
    if (setScratchRegister) {
      return conv_unique<RelocatableInst>(DataBlockAddress::unique());
    } else {
      return conv_unique<RelocatableInst>(DataBlockAddress::unique(reg));
    }
  } else {
    QBDI_REQUIRE_ABORT(not setScratchRegister,
                       "ARM mode doesn't have a scratch register");
    return conv_unique<RelocatableInst>(DataBlockAddress::unique(reg),
                                        NoReloc::unique(bic(reg, reg, 0xff)));
  }
}

// WritePC
// =======

RelocatableInst::UniquePtrVec
WritePC::generate(const Patch &patch, TempManager &temp_manager) const {

  CPUMode cpumode = patch.llvmcpu->getCPUMode();

  bool forceThumb = false;
  bool forceARM = false;

  switch (patch.metadata.inst.getOpcode()) {
    // never (see BranchWritePC)
    //  B, BL, CBNZ, CBZ, TBB, TBH
    case llvm::ARM::BL:
    case llvm::ARM::BL_pred:
    case llvm::ARM::Bcc:
    case llvm::ARM::t2B:
    case llvm::ARM::t2BXAUT:
    case llvm::ARM::t2Bcc:
    case llvm::ARM::t2TBB:
    case llvm::ARM::t2TBH:
    case llvm::ARM::tB:
    case llvm::ARM::tBL:
    case llvm::ARM::tBcc:
    case llvm::ARM::tCBNZ:
    case llvm::ARM::tCBZ:
      // the instrumentation process manages the address
      // nothings to add here
      break;
    // since 5 (see LoadWritePC)
    //  LDM<IA|DA|DB|IB>, LDR, POP
    //
    // The new address is loaded from memory:
    //   need to patch it if < ARMv5
    case llvm::ARM::LDMDA:
    case llvm::ARM::LDMDA_UPD:
    case llvm::ARM::LDMDB:
    case llvm::ARM::LDMDB_UPD:
    case llvm::ARM::LDMIA:
    case llvm::ARM::LDMIA_UPD:
    case llvm::ARM::LDMIB:
    case llvm::ARM::LDMIB_UPD:
    case llvm::ARM::LDR_POST_IMM:
    case llvm::ARM::LDR_PRE_IMM:
    case llvm::ARM::LDRi12:
    case llvm::ARM::LDRrs:
    case llvm::ARM::t2LDMDB:
    case llvm::ARM::t2LDMDB_UPD:
    case llvm::ARM::t2LDMIA:
    case llvm::ARM::t2LDMIA_UPD:
    case llvm::ARM::t2LDR_POST:
    case llvm::ARM::t2LDR_PRE:
    case llvm::ARM::t2LDRi12:
    case llvm::ARM::t2LDRi8:
    case llvm::ARM::t2LDRpci:
    case llvm::ARM::t2LDRs:
    case llvm::ARM::tPOP:
      if (patch.llvmcpu->hasOptions(Options::OPT_ARMv4_bit)) {
        if (cpumode == CPUMode::Thumb) {
          forceThumb = true;
        } else {
          forceARM = true;
        }
      }
      break;
    // since 7 and current mode == ARM (see ALUWritePC)
    //  ADC, ADD, AND, ASR, BIC, EOR, LSL, LSR, MOV, MVN, ORR, ROR, RRX, RSB,
    //  RSC, SBC, SUB
    case llvm::ARM::ADCri:
    case llvm::ARM::ADCrr:
    case llvm::ARM::ADCrsi:
    case llvm::ARM::ADDri:
    case llvm::ARM::ADDrr:
    case llvm::ARM::ADDrsi:
    case llvm::ARM::ANDri:
    case llvm::ARM::ANDrr:
    case llvm::ARM::ANDrsi:
    case llvm::ARM::BICri:
    case llvm::ARM::BICrr:
    case llvm::ARM::BICrsi:
    case llvm::ARM::EORri:
    case llvm::ARM::EORrr:
    case llvm::ARM::EORrsi:
    case llvm::ARM::MOVi16:
    case llvm::ARM::MOVi:
    case llvm::ARM::MOVr:
    case llvm::ARM::MOVPCLR:
    case llvm::ARM::MOVsi:
    case llvm::ARM::MVNi:
    case llvm::ARM::MVNr:
    case llvm::ARM::ORRri:
    case llvm::ARM::ORRrr:
    case llvm::ARM::ORRrsi:
    case llvm::ARM::RSBri:
    case llvm::ARM::RSBrr:
    case llvm::ARM::RSBrsi:
    case llvm::ARM::RSCri:
    case llvm::ARM::RSCrr:
    case llvm::ARM::RSCrsi:
    case llvm::ARM::SBCri:
    case llvm::ARM::SBCrr:
    case llvm::ARM::SBCrsi:
    case llvm::ARM::SUBri:
    case llvm::ARM::SUBrr:
    case llvm::ARM::SUBrsi:
    case llvm::ARM::tADDhirr:
    case llvm::ARM::tADDrSP:
    case llvm::ARM::tMOVr:
      if (cpumode == CPUMode::Thumb) {
        forceThumb = true;
      } else if (patch.llvmcpu->hasOptions(Options::OPT_ARMv5T_6)) {
        forceARM = true;
      }
      break;
    // always (see BXWritePC)
    //  BLX, BX, BXJ
    case llvm::ARM::BLX:
    case llvm::ARM::BLX_pred:
    case llvm::ARM::BX:
    case llvm::ARM::BX_RET:
    case llvm::ARM::BX_pred:
    case llvm::ARM::tBLXr:
      // register operand, depend on the value of the register
      // do nothings
      break;
    case llvm::ARM::BLXi:
    case llvm::ARM::tBLXi:
      // managed by GetPCOffset
      break;
    case llvm::ARM::tBX:
      // if BX pc, switch to ARM (but PC is always aligned, nothing to do)
      // else, depend on the value of the register
      break;
    default:
      QBDI_ABORT_PATCH(patch, "SetExchange doesn't support this instruction:");
  }

  RelocatableInst::UniquePtrVec relocInstList;

  QBDI_REQUIRE_ABORT_PATCH(
      (not forceThumb) or (not forceARM), patch,
      "Cannot force both ARM and Thumb mode at the same time");

  if (forceThumb or forceARM) {
    unsigned cond =
        dropCond ? llvm::ARMCC::AL : patch.metadata.archMetadata.cond;

    Reg tempReg = temp_manager.getRegForTemp(temp);

    if (cpumode == CPUMode::Thumb) {
      // we need to keep the condition, as we must not force the new address
      // mode if the instruction isn't executed
      if (cond != llvm::ARMCC::AL) {
        append(relocInstList, ItPatch(false).generate(patch, temp_manager));
      }
      if (forceThumb) {
        relocInstList.push_back(
            NoReloc::unique(t2orri(tempReg, tempReg, 1, cond)));
      } else if (forceARM) {
        relocInstList.push_back(
            NoReloc::unique(t2bic(tempReg, tempReg, 1, cond)));
      }
    } else {
      if (forceThumb) {
        relocInstList.push_back(
            NoReloc::unique(orri(tempReg, tempReg, 1, cond)));
      } else if (forceARM) {
        relocInstList.push_back(
            NoReloc::unique(bic(tempReg, tempReg, 1, cond)));
      }
    }
  }

  append(relocInstList,
         WriteTemp(temp, Offset(Reg(REG_PC))).generate(patch, temp_manager));

  return relocInstList;
}

// SetExchange
// ===========

RelocatableInst::UniquePtrVec
SetExchange::generate(const Patch &patch, TempManager &temp_manager) const {

  CPUMode cpumode = patch.llvmcpu->getCPUMode();

  switch (patch.metadata.inst.getOpcode()) {
    // never (see BranchWritePC)
    //  B, BL, CBNZ, CBZ, TBB, TBH
    case llvm::ARM::BL:
    case llvm::ARM::BL_pred:
    case llvm::ARM::Bcc:
    case llvm::ARM::t2B:
    case llvm::ARM::t2Bcc:
    case llvm::ARM::t2TBB:
    case llvm::ARM::t2TBH:
    case llvm::ARM::tB:
    case llvm::ARM::tBL:
    case llvm::ARM::tBcc:
    case llvm::ARM::tCBNZ:
    case llvm::ARM::tCBZ:
      return {};
    // since 5 (see LoadWritePC)
    //  LDM<IA|DA|DB|IB>, LDR, POP
    case llvm::ARM::LDMDA:
    case llvm::ARM::LDMDA_UPD:
    case llvm::ARM::LDMDB:
    case llvm::ARM::LDMDB_UPD:
    case llvm::ARM::LDMIA:
    case llvm::ARM::LDMIA_UPD:
    case llvm::ARM::LDMIB:
    case llvm::ARM::LDMIB_UPD:
    case llvm::ARM::LDR_POST_IMM:
    case llvm::ARM::LDR_PRE_IMM:
    case llvm::ARM::LDRi12:
    case llvm::ARM::LDRrs:
    case llvm::ARM::t2LDMDB:
    case llvm::ARM::t2LDMDB_UPD:
    case llvm::ARM::t2LDMIA:
    case llvm::ARM::t2LDMIA_UPD:
    case llvm::ARM::t2LDR_POST:
    case llvm::ARM::t2LDR_PRE:
    case llvm::ARM::t2LDRi12:
    case llvm::ARM::t2LDRi8:
    case llvm::ARM::t2LDRpci:
    case llvm::ARM::t2LDRs:
    case llvm::ARM::tPOP:
      if (patch.llvmcpu->hasOptions(Options::OPT_ARMv4_bit)) {
        return {};
      }
      break;
    // since 7 and current mode == ARM (see ALUWritePC)
    //  ADC, ADD, AND, ASR, BIC, EOR, LSL, LSR, MOV, MVN, ORR, ROR, RRX, RSB,
    //  RSC, SBC, SUB
    case llvm::ARM::ADCri:
    case llvm::ARM::ADCrr:
    case llvm::ARM::ADCrsi:
    case llvm::ARM::ADDri:
    case llvm::ARM::ADDrr:
    case llvm::ARM::ADDrsi:
    case llvm::ARM::ANDri:
    case llvm::ARM::ANDrr:
    case llvm::ARM::ANDrsi:
    case llvm::ARM::BICri:
    case llvm::ARM::BICrr:
    case llvm::ARM::BICrsi:
    case llvm::ARM::EORri:
    case llvm::ARM::EORrr:
    case llvm::ARM::EORrsi:
    case llvm::ARM::MOVi16:
    case llvm::ARM::MOVi:
    case llvm::ARM::MOVr:
    case llvm::ARM::MOVPCLR:
    case llvm::ARM::MOVsi:
    case llvm::ARM::MVNi:
    case llvm::ARM::MVNr:
    case llvm::ARM::ORRri:
    case llvm::ARM::ORRrr:
    case llvm::ARM::ORRrsi:
    case llvm::ARM::RSBri:
    case llvm::ARM::RSBrr:
    case llvm::ARM::RSBrsi:
    case llvm::ARM::RSCri:
    case llvm::ARM::RSCrr:
    case llvm::ARM::RSCrsi:
    case llvm::ARM::SBCri:
    case llvm::ARM::SBCrr:
    case llvm::ARM::SBCrsi:
    case llvm::ARM::SUBri:
    case llvm::ARM::SUBrr:
    case llvm::ARM::SUBrsi:
      if (patch.llvmcpu->hasOptions(Options::OPT_ARMv5T_6) or
          cpumode != CPUMode::ARM) {
        return {};
      }
      break;
    case llvm::ARM::tADDrSP:
    case llvm::ARM::tADDhirr:
    case llvm::ARM::tMOVr:
      // Thumb inst
      return {};
    // always (see BXWritePC)
    //  BLX, BX, BXJ
    case llvm::ARM::BLX:
    case llvm::ARM::BLX_pred:
    case llvm::ARM::BLXi:
    case llvm::ARM::BX:
    case llvm::ARM::BX_RET:
    case llvm::ARM::BX_pred:
    case llvm::ARM::t2BXAUT:
    case llvm::ARM::tBLXi:
    case llvm::ARM::tBLXr:
    case llvm::ARM::tBX:
      break;
    default:
      QBDI_ABORT_PATCH(patch, "SetExchange doesn't support this instruction:");
  }

  Reg destReg = temp_manager.getRegForTemp(temp);

  if (patch.metadata.archMetadata.cond != llvm::ARMCC::AL and
      cpumode == CPUMode::Thumb) {
    return conv_unique<RelocatableInst>(
        T2it(cpumode, patch.metadata.archMetadata.cond,
             (unsigned)llvm::ARM::PredBlockMask::TT),
        LoadImmCC::unique(destReg, Constant(1),
                          patch.metadata.archMetadata.cond),
        StoreDataBlockCC::unique(destReg,
                                 Offset(offsetof(Context, hostState.exchange)),
                                 patch.metadata.archMetadata.cond));
  } else {
    return conv_unique<RelocatableInst>(
        LoadImmCC::unique(destReg, Constant(1),
                          patch.metadata.archMetadata.cond),
        StoreDataBlockCC::unique(destReg,
                                 Offset(offsetof(Context, hostState.exchange)),
                                 patch.metadata.archMetadata.cond));
  }
}

// GetPCOffset
// ===========

RelocatableInst::UniquePtrVec
GetPCOffset::generate(const Patch &patch, TempManager &temp_manager) const {

  CPUMode cpumode = patch.llvmcpu->getCPUMode();
  RegLLVM destReg;
  rword imm;

  if (type == OpOperandType) {
    QBDI_REQUIRE_ABORT_PATCH(opDest < patch.metadata.inst.getNumOperands(),
                             patch, "Invalid operand");
    QBDI_REQUIRE_ABORT_PATCH(patch.metadata.inst.getOperand(opDest).isReg(),
                             patch, "Unexpected operand type");
    destReg = patch.metadata.inst.getOperand(opDest).getReg();
  } else {
    destReg = temp_manager.getRegForTemp(temp);
  }

  if (type == TmpConstantType) {
    imm = cst;
  } else {
    QBDI_REQUIRE_ABORT_PATCH(type == TmpOperandType or type == OpOperandType,
                             patch, "Unexepcted type");
    QBDI_REQUIRE_ABORT_PATCH(op < patch.metadata.inst.getNumOperands(), patch,
                             "Invalid operand");
    QBDI_REQUIRE_ABORT_PATCH(patch.metadata.inst.getOperand(op).isImm(), patch,
                             "Unexpected operand type");
    imm = patch.metadata.inst.getOperand(op).getImm();
    switch (patch.metadata.inst.getOpcode()) {
      case llvm::ARM::BLXi:
      case llvm::ARM::t2B:
      case llvm::ARM::t2Bcc:
      case llvm::ARM::tB:
      case llvm::ARM::tBL:
      case llvm::ARM::tBcc:
      case llvm::ARM::tCBNZ:
      case llvm::ARM::tCBZ:
        // switch (or keep) the CPU to Thumb: set LSB to 1
        imm |= 1;
        break;
      case llvm::ARM::tBLXi:
        // switch to ARM mode : remove LSB if any
        imm ^= (imm & 1);
        break;
      case llvm::ARM::tADR:
        imm = imm << 2;
        break;
      default:
        break;
    }
  }
  if (cpumode == CPUMode::Thumb) {
    imm = imm + patch.metadata.address + 4;
    // instruction isn't align, need to detect instruction that used PC or
    // Align(PC, 4)
    if (patch.metadata.address % 4 != 0) {
      QBDI_REQUIRE_ABORT_PATCH(patch.metadata.address % 4 == 2, patch,
                               "Not aligned instruction");
      switch (patch.metadata.inst.getOpcode()) {
        case llvm::ARM::VLDRD:
        case llvm::ARM::VLDRH:
        case llvm::ARM::VLDRS:
        case llvm::ARM::t2ADR:
        case llvm::ARM::t2LDC2L_OFFSET:
        case llvm::ARM::t2LDC2_OFFSET:
        case llvm::ARM::t2LDCL_OFFSET:
        case llvm::ARM::t2LDC_OFFSET:
        case llvm::ARM::t2LDRBpci:
        case llvm::ARM::t2LDRDi8:
        case llvm::ARM::t2LDRHpci:
        case llvm::ARM::t2LDRSBpci:
        case llvm::ARM::t2LDRSHpci:
        case llvm::ARM::t2LDRpci:
        case llvm::ARM::tADR:
        case llvm::ARM::tBLXi:
        case llvm::ARM::tLDRpci:
          imm -= 2;
          break;
        case llvm::ARM::t2B:
        case llvm::ARM::t2Bcc:
        case llvm::ARM::t2TBB:
        case llvm::ARM::t2TBH:
        case llvm::ARM::tADDhirr:
        case llvm::ARM::tADDrSP:
        case llvm::ARM::tADDspr:
        case llvm::ARM::tB:
        case llvm::ARM::tBL:
        case llvm::ARM::tBLXr:
        case llvm::ARM::tBcc:
        case llvm::ARM::tCBZ:
        case llvm::ARM::tCBNZ:
        case llvm::ARM::tMOVr:
          break;
        case llvm::ARM::tBX:
          QBDI_ABORT_PATCH(patch, "BX pc with PC not aligned:");
        default:
          QBDI_ABORT_PATCH(patch, "Missing PC align behavior for:");
      }
    }
  } else {
    imm = imm + patch.metadata.address + 8;
  }
  if (keepCond) {
    if (patch.metadata.archMetadata.cond != llvm::ARMCC::AL and
        cpumode == CPUMode::Thumb) {
      return conv_unique<RelocatableInst>(
          T2it(cpumode, patch.metadata.archMetadata.cond,
               (unsigned)llvm::ARM::PredBlockMask::T),
          LoadImmCC::unique(destReg, Constant(imm),
                            patch.metadata.archMetadata.cond));
    } else {
      return conv_unique<RelocatableInst>(LoadImmCC::unique(
          destReg, Constant(imm), patch.metadata.archMetadata.cond));
    }
  } else {
    return conv_unique<RelocatableInst>(
        LoadImm::unique(destReg, Constant(imm)));
  }

  _QBDI_UNREACHABLE();
}

// GetNextInstAddr
// ===============

RelocatableInst::UniquePtrVec
GetNextInstAddr::generate(const Patch &patch, TempManager &temp_manager) const {

  CPUMode cpumode = patch.llvmcpu->getCPUMode();
  Reg destReg(0);
  if (type == TmpType) {
    destReg = temp_manager.getRegForTemp(temp);
  } else {
    destReg = reg;
  }
  rword imm = patch.metadata.endAddress();

  if (cpumode == CPUMode::Thumb) {
    imm |= 1;
  }

  if (keepCond) {
    uint8_t cond = patch.metadata.archMetadata.cond;
    if (invCond) {
      if (cond == llvm::ARMCC::AL) {
        // inv(AL) == not True == False
        // In this case, do not generate an instruction
        return {};
      }
      cond = llvm::ARMCC::getOppositeCondition((llvm::ARMCC::CondCodes)cond);
    }
    if (cond != llvm::ARMCC::AL and cpumode == CPUMode::Thumb) {
      return conv_unique<RelocatableInst>(
          T2it(cpumode, cond, (unsigned)llvm::ARM::PredBlockMask::T),
          LoadImmCC::unique(destReg, Constant(imm), cond));
    } else {
      return conv_unique<RelocatableInst>(
          LoadImmCC::unique(destReg, Constant(imm), cond));
    }
  } else {
    return conv_unique<RelocatableInst>(
        LoadImm::unique(destReg, Constant(imm)));
  }

  _QBDI_UNREACHABLE();
}

// GetOperandCC
// ============

RelocatableInst::UniquePtrVec
GetOperandCC::generate(const Patch &patch, TempManager &temp_manager) const {
  CPUMode cpumode = patch.llvmcpu->getCPUMode();
  const llvm::MCInst &inst = patch.metadata.inst;
  Reg destReg = reg;
  if (type == TmpType) {
    destReg = temp_manager.getRegForTemp(temp);
  }
  QBDI_REQUIRE_ABORT_PATCH(op < inst.getNumOperands(), patch,
                           "Invalid operand");
  if (inst.getOperand(op).isReg()) {
    if (patch.metadata.archMetadata.cond != llvm::ARMCC::AL and
        cpumode == CPUMode::Thumb) {
      return conv_unique<RelocatableInst>(
          T2it(cpumode, patch.metadata.archMetadata.cond,
               (unsigned)llvm::ARM::PredBlockMask::T),
          MovRegCC::unique(destReg, inst.getOperand(op).getReg(),
                           patch.metadata.archMetadata.cond));
    } else {
      return conv_unique<RelocatableInst>(
          MovRegCC::unique(destReg, inst.getOperand(op).getReg(),
                           patch.metadata.archMetadata.cond));
    }
  } else if (inst.getOperand(op).isImm()) {
    if (patch.metadata.archMetadata.cond != llvm::ARMCC::AL and
        cpumode == CPUMode::Thumb) {
      return conv_unique<RelocatableInst>(
          T2it(cpumode, patch.metadata.archMetadata.cond,
               (unsigned)llvm::ARM::PredBlockMask::T),
          LoadImmCC::unique(destReg, Constant(inst.getOperand(op).getImm()),
                            patch.metadata.archMetadata.cond));
    } else {
      return conv_unique<RelocatableInst>(
          LoadImmCC::unique(destReg, Constant(inst.getOperand(op).getImm()),
                            patch.metadata.archMetadata.cond));
    }
  } else {
    QBDI_ERROR("Invalid operand type for GetOperand()");
    return {};
  }
}

// CopyRegCC
// =========

RelocatableInst::UniquePtrVec
CopyRegCC::generate(const Patch &patch, TempManager &temp_manager) const {

  CPUMode cpumode = patch.llvmcpu->getCPUMode();
  Reg dest = destReg;
  if (type == Reg2Temp) {
    dest = temp_manager.getRegForTemp(destTemp);
  }

  if (patch.metadata.archMetadata.cond != llvm::ARMCC::AL and
      cpumode == CPUMode::Thumb) {
    return conv_unique<RelocatableInst>(
        T2it(cpumode, patch.metadata.archMetadata.cond,
             (unsigned)llvm::ARM::PredBlockMask::T),
        MovRegCC::unique(dest, src, patch.metadata.archMetadata.cond));
  } else {
    return conv_unique<RelocatableInst>(
        MovRegCC::unique(dest, src, patch.metadata.archMetadata.cond));
  }
}

// WriteTempCC
// ===========

RelocatableInst::UniquePtrVec
WriteTempCC::generate(const Patch &patch, TempManager &temp_manager) const {

  CPUMode cpumode = patch.llvmcpu->getCPUMode();

  if (patch.metadata.archMetadata.cond != llvm::ARMCC::AL and
      cpumode == CPUMode::Thumb) {
    return conv_unique<RelocatableInst>(
        T2it(cpumode, patch.metadata.archMetadata.cond,
             (unsigned)llvm::ARM::PredBlockMask::T),
        StoreDataBlockCC::unique(temp_manager.getRegForTemp(temp), offset,
                                 patch.metadata.archMetadata.cond));
  } else {
    return conv_unique<RelocatableInst>(
        StoreDataBlockCC::unique(temp_manager.getRegForTemp(temp), offset,
                                 patch.metadata.archMetadata.cond));
  }
}

// WriteOperandCC
// ==============

RelocatableInst::UniquePtrVec
WriteOperandCC::generate(const Patch &patch, TempManager &temp_manager) const {
  const llvm::MCInst &inst = patch.metadata.inst;
  CPUMode cpumode = patch.llvmcpu->getCPUMode();

  QBDI_REQUIRE_ABORT_PATCH(op < inst.getNumOperands(), patch,
                           "Invalid operand {}", op);
  if (inst.getOperand(op).isReg()) {
    if (patch.metadata.archMetadata.cond != llvm::ARMCC::AL and
        cpumode == CPUMode::Thumb) {
      return conv_unique<RelocatableInst>(
          T2it(cpumode, patch.metadata.archMetadata.cond,
               (unsigned)llvm::ARM::PredBlockMask::T),
          StoreDataBlockCC::unique(inst.getOperand(op).getReg(), offset,
                                   patch.metadata.archMetadata.cond));
    } else {
      return conv_unique<RelocatableInst>(
          StoreDataBlockCC::unique(inst.getOperand(op).getReg(), offset,
                                   patch.metadata.archMetadata.cond));
    }
  } else {
    QBDI_ERROR("Invalid operand type for WriteOperand()");
    return {};
  }
}

// CopyTempCC
// ==========

RelocatableInst::UniquePtrVec
CopyTempCC::generate(const Patch &patch, TempManager &temp_manager) const {

  CPUMode cpumode = patch.llvmcpu->getCPUMode();
  Reg dest = destReg;
  if (type == Temp2Temp) {
    dest = temp_manager.getRegForTemp(destTemp);
  }

  if (patch.metadata.archMetadata.cond != llvm::ARMCC::AL and
      cpumode == CPUMode::Thumb) {
    return conv_unique<RelocatableInst>(
        T2it(cpumode, patch.metadata.archMetadata.cond,
             (unsigned)llvm::ARM::PredBlockMask::T),
        MovRegCC::unique(dest, temp_manager.getRegForTemp(src),
                         patch.metadata.archMetadata.cond));
  } else {
    return conv_unique<RelocatableInst>(
        MovRegCC::unique(dest, temp_manager.getRegForTemp(src),
                         patch.metadata.archMetadata.cond));
  }
}

// AddOperandToTemp
// ================

RelocatableInst::UniquePtrVec
AddOperandToTemp::generate(const Patch &patch,
                           TempManager &temp_manager) const {

  QBDI_REQUIRE_ABORT_PATCH(*patch.llvmcpu == CPUMode::Thumb, patch,
                           "Unimplemented in ARM mode");

  const llvm::MCInst &inst = patch.metadata.inst;
  Reg dest = temp_manager.getRegForTemp(temp);

  QBDI_REQUIRE_ABORT_PATCH(op < inst.getNumOperands(), patch,
                           "Invalid operand");
  QBDI_REQUIRE_ABORT_PATCH(op2 < inst.getNumOperands(), patch,
                           "Invalid operand");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(op).isReg(), patch,
                           "Unexpected operand type");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(op2).isImm(), patch,
                           "Unexpected operand type");

  RegLLVM addrReg = inst.getOperand(op).getReg();
  rword imm = inst.getOperand(op2).getImm();

  switch (inst.getOpcode()) {
    case llvm::ARM::t2LDREX:
      imm = imm << 2;
      break;
    default:
      break;
  }

  if (imm == 0) {
    return conv_unique<RelocatableInst>(MovReg::unique(dest, addrReg));
  } else {
    return conv_unique<RelocatableInst>(
        NoReloc::unique(t2add(dest, addrReg, imm)));
  }
}

// LDMPatchGen
// ===========

RelocatableInst::UniquePtrVec
LDMPatchGen::generate(const Patch &patch, TempManager &temp_manager) const {

  const llvm::MCInst &inst = patch.metadata.inst;
  CPUMode cpumode = patch.llvmcpu->getCPUMode();
  QBDI_REQUIRE_ABORT_PATCH(cpumode == CPUMode::ARM, patch,
                           "Only available in ARM mode");

  // lock tempRegister. Only 1 register may be a temp register
  Reg tempReg = temp_manager.getRegForTemp(temp);
  temp_manager.lockTempManager();
  QBDI_REQUIRE_ABORT_PATCH(temp_manager.getUsedRegisters().size() == 1, patch,
                           "Unexpected TempManager state");

  // verify the tempReg isn't the address register (should never happend)
  QBDI_REQUIRE_ABORT_PATCH(0 < inst.getNumOperands(), patch,
                           "Invalid instruction");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(0).isReg(), patch,
                           "Unexpected operand type");
  RegLLVM addrReg = inst.getOperand(0).getReg();
  QBDI_REQUIRE_ABORT_PATCH(tempReg != addrReg, patch,
                           "tempRegister allocation error");

  // verify PC is on the register list (always the last register of the list)
  QBDI_REQUIRE_ABORT_PATCH(
      inst.getOperand(inst.getNumOperands() - 1).isReg() and
          inst.getOperand(inst.getNumOperands() - 1).getReg() == GPR_ID[REG_PC],
      patch, "LDM without PC doesn't need this patch");

  // verify if the tempReg is a register on the register list
  // (may happend if all register are present in the register list)
  bool tempIsNeeded =
      (patch.regUsage[tempReg.getID()] & RegisterUsage::RegisterBoth) != 0;

  // copy of the original MCInst but without PC (that is the last operand)
  llvm::MCInst instWithoutPC = inst;
  instWithoutPC.erase(instWithoutPC.begin() +
                      (instWithoutPC.getNumOperands() - 1));

  // copy of the original MCInst but without any register
  // Only used with writeback instruction
  llvm::MCInst instNoReg = inst;
  // Verify if the address register is within the register list
  bool addrIsSet = false;
  while (instNoReg.getNumOperands() > 0 and
         instNoReg.getOperand(instNoReg.getNumOperands() - 1).isReg() and
         instNoReg.getOperand(instNoReg.getNumOperands() - 1).getReg() !=
             llvm::ARM::CPSR and
         instNoReg.getOperand(instNoReg.getNumOperands() - 1).getReg() !=
             llvm::ARM::NoRegister) {
    addrIsSet |=
        (instNoReg.getOperand(instNoReg.getNumOperands() - 1).getReg() ==
         instNoReg.getOperand(0).getReg());
    instNoReg.erase(instNoReg.begin() + (instNoReg.getNumOperands() - 1));
  }

  // verify if the instruction is conditionnal
  bool hasCond = patch.metadata.archMetadata.cond != llvm::ARMCC::AL;

  RelocatableInst::UniquePtrVec res;

  switch (inst.getOpcode()) {
    case llvm::ARM::LDMIA: {
      unsigned nbReg = inst.getNumOperands() - /* source + 2 * cond + PC*/ 4;

      // if the instruction is conditionnal, load PC+4
      if (hasCond) {
        append(res, GetNextInstAddr(temp, /* keepCond */ false)
                        .generate(patch, temp_manager));
      }
      // load PC (with cond if needed)
      res.push_back(NoReloc::unique(ldri12(tempReg, addrReg, 4 * nbReg,
                                           patch.metadata.archMetadata.cond)));

      // fix and store PC (without cond)
      append(res, WritePC(temp).generate(patch, temp_manager));

      if (nbReg > 0) {
        // apply the instruction without PC
        res.push_back(NoReloc::unique(std::move(instWithoutPC)));

        // if needed, replace the value of the tempRegister in the Datablock
        if (tempIsNeeded) {
          res.push_back(StoreDataBlockCC::unique(
              tempReg, Offset(tempReg), patch.metadata.archMetadata.cond));
        }
      }

      return res;
    }
    case llvm::ARM::LDMIB: {
      unsigned nbReg = inst.getNumOperands() - /* source + 2 * cond + PC*/ 4;

      // if the instruction is conditionnal, load PC+4
      if (hasCond) {
        append(res, GetNextInstAddr(temp, /* keepCond */ false)
                        .generate(patch, temp_manager));
      }
      // load PC (with cond if needed)
      res.push_back(NoReloc::unique(ldri12(tempReg, addrReg, 4 * nbReg + 4,
                                           patch.metadata.archMetadata.cond)));

      // fix and store PC (without cond)
      append(res, WritePC(temp).generate(patch, temp_manager));

      if (nbReg > 0) {
        // apply the instruction without PC
        res.push_back(NoReloc::unique(std::move(instWithoutPC)));

        // if needed, replace the value of the tempRegister in the Datablock
        if (tempIsNeeded) {
          res.push_back(StoreDataBlockCC::unique(
              tempReg, Offset(tempReg), patch.metadata.archMetadata.cond));
        }
      }

      return res;
    }
    case llvm::ARM::LDMDA: {
      unsigned nbReg = inst.getNumOperands() - /* source + 2 * cond + PC*/ 4;

      // if the instruction is conditionnal, load PC+4
      if (hasCond) {
        append(res, GetNextInstAddr(temp, /* keepCond */ false)
                        .generate(patch, temp_manager));
      }
      // load PC (with cond if needed)
      res.push_back(NoReloc::unique(
          ldri12(tempReg, addrReg, 0, patch.metadata.archMetadata.cond)));

      // fix and store PC (without cond)
      append(res, WritePC(temp).generate(patch, temp_manager));

      if (nbReg > 0) {

        // apply the instruction without PC
        // Replace opcode by LDMDB to avoid PC position
        instWithoutPC.setOpcode(llvm::ARM::LDMDB);
        res.push_back(NoReloc::unique(std::move(instWithoutPC)));

        // if needed, replace the value of the tempRegister in the Datablock
        if (tempIsNeeded) {
          res.push_back(StoreDataBlockCC::unique(
              tempReg, Offset(tempReg), patch.metadata.archMetadata.cond));
        }
      }
      return res;
    }
    case llvm::ARM::LDMDB: {
      unsigned nbReg = inst.getNumOperands() - /* source + 2 * cond + PC*/ 4;

      // if the instruction is conditionnal, load PC+4
      if (hasCond) {
        append(res, GetNextInstAddr(temp, /* keepCond */ false)
                        .generate(patch, temp_manager));
      }
      // load PC (with cond if needed
      res.push_back(NoReloc::unique(
          ldri12(tempReg, addrReg, -4, patch.metadata.archMetadata.cond)));

      // fix and store PC (without cond)
      append(res, WritePC(temp).generate(patch, temp_manager));

      if (nbReg > 0) {
        // sub 4 to addrReg to avoid PC
        res.push_back(NoReloc::unique(
            sub(addrReg, addrReg, 4, patch.metadata.archMetadata.cond)));

        // apply the instruction without PC
        res.push_back(NoReloc::unique(std::move(instWithoutPC)));

        if (not addrIsSet) {
          // add 4 to addrReg
          res.push_back(NoReloc::unique(
              add(addrReg, addrReg, 4, patch.metadata.archMetadata.cond)));
        }

        // if needed, replace the value of the tempRegister in the Datablock
        if (tempIsNeeded) {
          res.push_back(StoreDataBlockCC::unique(
              tempReg, Offset(tempReg), patch.metadata.archMetadata.cond));
        }
      }
      return res;
    }
    case llvm::ARM::LDMIA_UPD:
    case llvm::ARM::LDMIB_UPD: {
      QBDI_REQUIRE_ABORT_PATCH(
          not addrIsSet, patch,
          "invalid instruction (wback && registers<n> == '1')");

      unsigned nbReg =
          inst.getNumOperands() - /* source + wback + 2 * cond + PC*/ 5;

      if (nbReg > 0) {
        // apply the instruction without PC
        res.push_back(NoReloc::unique(std::move(instWithoutPC)));

        // if needed, replace the value of the tempRegister in the Datablock
        if (tempIsNeeded) {
          res.push_back(StoreDataBlockCC::unique(
              tempReg, Offset(tempReg), patch.metadata.archMetadata.cond));
        }
      }

      // if the instruction is conditionnal, load PC+4
      if (hasCond) {
        append(res, GetNextInstAddr(temp, /* keepCond */ false)
                        .generate(patch, temp_manager));
      }

      // load PC (with instNoReg + tempReg) with the same instruction to wback
      // the address
      QBDI_REQUIRE_ABORT_PATCH(instNoReg.getNumOperands() == 4, patch,
                               "Unexpected state");
      instNoReg.insert(instNoReg.end(),
                       llvm::MCOperand::createReg(tempReg.getValue()));
      res.push_back(NoReloc::unique(std::move(instNoReg)));

      // fix and store PC (without cond)
      append(res, WritePC(temp).generate(patch, temp_manager));

      return res;
    }
    case llvm::ARM::LDMDA_UPD:
    case llvm::ARM::LDMDB_UPD: {
      QBDI_REQUIRE_ABORT_PATCH(
          not addrIsSet, patch,
          "invalid instruction (wback && registers<n> == '1')");
      unsigned nbReg = inst.getNumOperands() - /* source + 2 * cond + PC*/ 5;

      // if the instruction is conditionnal, load PC+4
      if (hasCond) {
        append(res, GetNextInstAddr(temp, /* keepCond */ false)
                        .generate(patch, temp_manager));
      }

      // load PC (with instNoReg + tempReg) with the same instruction to wback
      // the address
      QBDI_REQUIRE_ABORT_PATCH(instNoReg.getNumOperands() == 4, patch,
                               "Unexpected state");
      instNoReg.insert(instNoReg.end(),
                       llvm::MCOperand::createReg(tempReg.getValue()));
      res.push_back(NoReloc::unique(std::move(instNoReg)));

      // fix and store PC (without cond)
      append(res, WritePC(temp).generate(patch, temp_manager));

      if (nbReg > 0) {
        // apply the instruction without PC
        res.push_back(NoReloc::unique(std::move(instWithoutPC)));

        // if needed, replace the value of the tempRegister in the Datablock
        if (tempIsNeeded) {
          res.push_back(StoreDataBlockCC::unique(
              tempReg, Offset(tempReg), patch.metadata.archMetadata.cond));
        }
      }
      return res;
    }
    default:
      QBDI_ABORT_PATCH(patch,
                       "LDMPatchGen should not be used for this instruction:");
  }
}

// STMPatchGen
// ===========

RelocatableInst::UniquePtrVec
STMPatchGen::generate(const Patch &patch, TempManager &temp_manager) const {

  const llvm::MCInst &inst = patch.metadata.inst;
  CPUMode cpumode = patch.llvmcpu->getCPUMode();
  QBDI_REQUIRE_ABORT_PATCH(cpumode == CPUMode::ARM, patch,
                           "Only available in ARM mode");

  // lock tempRegister. Only 1 register may be a temp register
  Reg tempReg = temp_manager.getRegForTemp(temp);
  temp_manager.lockTempManager();
  QBDI_REQUIRE_ABORT_PATCH(temp_manager.getUsedRegisters().size() == 1, patch,
                           "Unexpected TempManager state");

  // verify the tempReg isn't the address register (should never happend)
  QBDI_REQUIRE_ABORT_PATCH(0 < inst.getNumOperands(), patch,
                           "Invalid instruction");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(0).isReg(), patch,
                           "Unexpected operand type");
  RegLLVM addrReg = inst.getOperand(0).getReg();
  QBDI_REQUIRE_ABORT_PATCH(tempReg != addrReg, patch,
                           "tempRegister allocation error");

  // verify PC is on the register list (always the last register of the list)
  QBDI_REQUIRE_ABORT_PATCH(
      inst.getOperand(inst.getNumOperands() - 1).isReg() and
          inst.getOperand(inst.getNumOperands() - 1).getReg() == GPR_ID[REG_PC],
      patch, "STM without PC doesn't need this patch");

  // verify if the tempReg is a register on the register list
  // (may happend if all register are present in the register list)
  bool tempIsNeeded =
      (patch.regUsage[tempReg.getID()] & RegisterUsage::RegisterBoth) != 0;

  unsigned nbReg;
  unsigned fixPCOffset;

  switch (inst.getOpcode()) {
    case llvm::ARM::STMIA:
      nbReg = inst.getNumOperands() - /* source + 2 * cond + PC*/ 4;
      fixPCOffset = 4 * nbReg;
      break;
    case llvm::ARM::STMIB:
      nbReg = inst.getNumOperands() - /* source + 2 * cond + PC*/ 4;
      fixPCOffset = 4 * nbReg + 4;
      break;
    case llvm::ARM::STMDA:
      nbReg = inst.getNumOperands() - /* source + 2 * cond + PC*/ 4;
      fixPCOffset = 0;
      break;
    case llvm::ARM::STMDB:
      nbReg = inst.getNumOperands() - /* source + 2 * cond + PC*/ 4;
      fixPCOffset = -4;
      break;
    case llvm::ARM::STMIA_UPD:
      nbReg = inst.getNumOperands() - /* source + wback + 2 * cond + PC*/ 5;
      fixPCOffset = -4;
      break;
    case llvm::ARM::STMIB_UPD:
      nbReg = inst.getNumOperands() - /* source + wback + 2 * cond + PC*/ 5;
      fixPCOffset = 0;
      break;
    case llvm::ARM::STMDA_UPD:
      nbReg = inst.getNumOperands() - /* source + wback + 2 * cond + PC*/ 5;
      fixPCOffset = 4 * nbReg + 4;
      break;
    case llvm::ARM::STMDB_UPD:
      nbReg = inst.getNumOperands() - /* source + wback + 2 * cond + PC*/ 5;
      fixPCOffset = 4 * nbReg;
      break;
    default:
      QBDI_ABORT_PATCH(patch,
                       "STMPatchGen should not be used for this instruction:");
  }

  RelocatableInst::UniquePtrVec res;

  // if needed, load the value of the tempRegister from the Datablock
  if (tempIsNeeded) {
    res.push_back(LoadDataBlockCC::unique(tempReg, Offset(tempReg),
                                          patch.metadata.archMetadata.cond));
  }

  // apply the instruction (the value of PC will be fixed after)
  llvm::MCInst instCpy = inst;
  res.push_back(NoReloc::unique(std::move(instCpy)));

  // Get current PC
  append(res, GetPCOffset(temp, Constant(0), /* keepCond */ false)
                  .generate(patch, temp_manager));

  // store PC (with cond if needed)
  res.push_back(NoReloc::unique(
      stri12(tempReg, addrReg, fixPCOffset, patch.metadata.archMetadata.cond)));

  return res;
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
      return conv_unique<RelocatableInst>(LoadImm::unique(tmpRegister, 0));
    } else {
      return {};
    }
  }

  Reg addrRegister = temp_manager.getRegForTemp(addr);

  bool isARM = (patch.llvmcpu->getCPUMode() == CPUMode::ARM);
  bool wback = (tmpRegister != addrRegister);

  unsigned readSize = getReadSize(patch.metadata.inst, *patch.llvmcpu);
  switch (readSize) {
    case 6:
      if (index != 1) {
        readSize = 4;
      } else {
        readSize = 2;
      }
    default:
      break;
  }
  switch (readSize) {
    case 1: {
      if (isARM) {
        if (wback) {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(ldrbPost(tmpRegister, addrRegister)));
        } else {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(ldrb(tmpRegister, addrRegister, 0)));
        }
      } else {
        if (wback) {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(t2ldrbPost(tmpRegister, addrRegister)));
        } else {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(t2ldrb(tmpRegister, addrRegister, 0)));
        }
      }
    }
    case 2: {
      if (isARM) {
        if (wback) {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(ldrhPost(tmpRegister, addrRegister)));
        } else {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(ldrh(tmpRegister, addrRegister, 0)));
        }
      } else {
        if (wback) {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(t2ldrhPost(tmpRegister, addrRegister)));
        } else {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(t2ldrh(tmpRegister, addrRegister, 0)));
        }
      }
    }
    case 3: {
      QBDI_REQUIRE_ABORT_PATCH(wback, patch,
                               "Two tempReg are needed with readSize==3");
      if (isARM) {
        return conv_unique<RelocatableInst>(
            NoReloc::unique(ldrb(tmpRegister, addrRegister, 0)),
            NoReloc::unique(ldrb(addrRegister, addrRegister, 2)),
            NoReloc::unique(
                orrshift(tmpRegister, tmpRegister, addrRegister, 2)));
      } else {
        return conv_unique<RelocatableInst>(
            NoReloc::unique(t2ldrb(tmpRegister, addrRegister, 0)),
            NoReloc::unique(t2ldrb(addrRegister, addrRegister, 2)),
            NoReloc::unique(
                t2orrshift(tmpRegister, tmpRegister, addrRegister, 2)));
      }
    }
    case 4:
    case 8:
    case 12:
    case 20:
    case 28:
    case 36:
    case 44:
    case 52:
    case 60: {
      if (isARM) {
        if (wback) {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(ldrPost(tmpRegister, addrRegister, 4)));
        } else {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(ldri12(tmpRegister, addrRegister, 0)));
        }
      } else {
        if (wback) {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(t2ldrPost(tmpRegister, addrRegister, 4)));
        } else {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(t2ldri12(tmpRegister, addrRegister, 0)));
        }
      }
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
      return conv_unique<RelocatableInst>(LoadImm::unique(tmpRegister, 0));
    } else {
      return {};
    }
  }

  Reg addrRegister = temp_manager.getRegForTemp(addr);

  bool isARM = (patch.llvmcpu->getCPUMode() == CPUMode::ARM);
  bool wback = (tmpRegister != addrRegister);

  unsigned writeSize = getWriteSize(patch.metadata.inst, *patch.llvmcpu);
  switch (writeSize) {
    case 6:
      if (index != 1) {
        writeSize = 4;
      } else {
        writeSize = 2;
      }
    default:
      break;
  }
  switch (writeSize) {
    case 1: {
      if (isARM) {
        if (wback) {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(ldrbPost(tmpRegister, addrRegister)));
        } else {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(ldrb(tmpRegister, addrRegister, 0)));
        }
      } else {
        if (wback) {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(t2ldrbPost(tmpRegister, addrRegister)));
        } else {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(t2ldrb(tmpRegister, addrRegister, 0)));
        }
      }
    }
    case 2: {
      if (isARM) {
        if (wback) {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(ldrhPost(tmpRegister, addrRegister)));
        } else {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(ldrh(tmpRegister, addrRegister, 0)));
        }
      } else {
        if (wback) {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(t2ldrhPost(tmpRegister, addrRegister)));
        } else {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(t2ldrh(tmpRegister, addrRegister, 0)));
        }
      }
    }
    case 3: {
      QBDI_REQUIRE_ABORT_PATCH(wback, patch,
                               "Two tempReg are needed with readSize==3");
      if (isARM) {
        return conv_unique<RelocatableInst>(
            NoReloc::unique(ldrb(tmpRegister, addrRegister, 0)),
            NoReloc::unique(ldrb(addrRegister, addrRegister, 2)),
            NoReloc::unique(
                orrshift(tmpRegister, tmpRegister, addrRegister, 2)));
      } else {
        return conv_unique<RelocatableInst>(
            NoReloc::unique(t2ldrb(tmpRegister, addrRegister, 0)),
            NoReloc::unique(t2ldrb(addrRegister, addrRegister, 2)),
            NoReloc::unique(
                t2orrshift(tmpRegister, tmpRegister, addrRegister, 2)));
      }
    }
    case 4:
    case 8:
    case 12:
    case 20:
    case 28:
    case 36:
    case 44:
    case 52:
    case 60: {
      if (isARM) {
        if (wback) {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(ldrPost(tmpRegister, addrRegister, 4)));
        } else {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(ldri12(tmpRegister, addrRegister, 0)));
        }
      } else {
        if (wback) {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(t2ldrPost(tmpRegister, addrRegister, 4)));
        } else {
          return conv_unique<RelocatableInst>(
              NoReloc::unique(t2ldri12(tmpRegister, addrRegister, 0)));
        }
      }
    }
    default:
      QBDI_ABORT_PATCH(patch, "Unexpected Write Size", writeSize);
  }
}

// BackupValueX2
// =============

RelocatableInst::UniquePtrVec
BackupValueX2::generate(const Patch &patch, TempManager &temp_manager) const {

  Reg tmpRegister = temp_manager.getRegForTemp(temp);
  Reg tmp2Register = temp_manager.getRegForTemp(temp2);

  if (patch.llvmcpu->hasOptions(Options::OPT_DISABLE_MEMORYACCESS_VALUE)) {
    // only set to zero for the first BackupValueX2
    if (shadow.getTag() != shadow2.getTag()) {
      return conv_unique<RelocatableInst>(LoadImm::unique(tmpRegister, 0),
                                          LoadImm::unique(tmp2Register, 0));
    } else {
      return {};
    }
  }

  Reg addrRegister = temp_manager.getRegForTemp(addr);

  QBDI_REQUIRE_ABORT_PATCH(tmpRegister != tmp2Register, patch,
                           "Need different TempRegister");
  QBDI_REQUIRE_ABORT_PATCH(tmpRegister != addrRegister, patch,
                           "Need different TempRegister");
  QBDI_REQUIRE_ABORT_PATCH(tmp2Register != addrRegister, patch,
                           "Need different TempRegister");

  unsigned mask = (1 << tmpRegister.getID()) | (1 << tmp2Register.getID());

  if (patch.llvmcpu->getCPUMode() == CPUMode::ARM) {
    if (tmpRegister.getID() < tmp2Register.getID()) {
      return conv_unique<RelocatableInst>(
          NoReloc::unique(ldmia(addrRegister, mask, true)),
          StoreShadow::unique(tmpRegister, shadow, true),
          StoreShadow::unique(tmp2Register, shadow2, true));
    } else {
      return conv_unique<RelocatableInst>(
          NoReloc::unique(ldmia(addrRegister, mask, true)),
          StoreShadow::unique(tmp2Register, shadow, true),
          StoreShadow::unique(tmpRegister, shadow2, true));
    }
  } else {
    if (tmpRegister.getID() < tmp2Register.getID()) {
      return conv_unique<RelocatableInst>(
          NoReloc::unique(t2ldmia(addrRegister, mask, true)),
          StoreShadow::unique(tmpRegister, shadow, true),
          StoreShadow::unique(tmp2Register, shadow2, true));
    } else {
      return conv_unique<RelocatableInst>(
          NoReloc::unique(t2ldmia(addrRegister, mask, true)),
          StoreShadow::unique(tmp2Register, shadow, true),
          StoreShadow::unique(tmpRegister, shadow2, true));
    }
  }
}

// CondExclusifLoad
// ================

RelocatableInst::UniquePtrVec
CondExclusifLoad::generate(const Patch &patch,
                           TempManager &temp_manager) const {
  CPUMode cpumode = patch.llvmcpu->getCPUMode();

  if (cpumode == CPUMode::ARM) {
    allocateConsecutiveTempRegister(temp_manager, temp, temp2);
  }

  Reg tmpReg = temp_manager.getRegForTemp(temp);
  Reg cpyFlags = temp_manager.getRegForTemp(temp2);
  bool hasCond = (patch.metadata.archMetadata.cond != llvm::ARMCC::AL);

  // ==== generate the load code ====
  RelocatableInst::UniquePtrVec loadPatch;

  // default case : restore flags and continue
  loadPatch.push_back(Msr(cpumode, cpyFlags));

  // case 8 (LDREXD)
  RelocatableInst::UniquePtrVec tmpPatch8_block;

  tmpPatch8_block.push_back(Msr(cpumode, cpyFlags));
  tmpPatch8_block.push_back(LoadDataBlock::unique(
      tmpReg, offsetof(Context, gprState.localMonitor.addr)));
  if (cpumode == CPUMode::Thumb) {
    if (hasCond) {
      tmpPatch8_block.push_back(T2it(cpumode, patch.metadata.archMetadata.cond,
                                     (unsigned)llvm::ARM::PredBlockMask::T));
    }
    tmpPatch8_block.push_back(NoReloc::unique(
        t2ldrexd(tmpReg, cpyFlags, tmpReg, patch.metadata.archMetadata.cond)));
  } else {
    tmpPatch8_block.push_back(NoReloc::unique(
        ldrexd(tmpReg, cpyFlags, tmpReg, patch.metadata.archMetadata.cond)));
  }
  tmpPatch8_block.push_back(
      Branch(cpumode, getUniquePtrVecSize(loadPatch, *patch.llvmcpu),
             /* addBranchLen */ true));

  RelocatableInst::UniquePtrVec tmpPatch8_cond;

  tmpPatch8_cond.push_back(Cmp(cpumode, tmpReg, 8));
  tmpPatch8_cond.push_back(
      BranchCC(cpumode, getUniquePtrVecSize(tmpPatch8_block, *patch.llvmcpu),
               llvm::ARMCC::CondCodes::NE,
               /* withinITBlock */ false, /* addBranchLen */ true));

  prepend(tmpPatch8_block, std::move(tmpPatch8_cond));
  prepend(loadPatch, std::move(tmpPatch8_block));

  // case 4 (LDREX)
  RelocatableInst::UniquePtrVec tmpPatch4_block;

  tmpPatch4_block.push_back(Msr(cpumode, cpyFlags));
  tmpPatch4_block.push_back(LoadDataBlock::unique(
      tmpReg, offsetof(Context, gprState.localMonitor.addr)));
  if (cpumode == CPUMode::Thumb) {
    if (hasCond) {
      tmpPatch4_block.push_back(T2it(cpumode, patch.metadata.archMetadata.cond,
                                     (unsigned)llvm::ARM::PredBlockMask::T));
    }
    tmpPatch4_block.push_back(NoReloc::unique(
        t2ldrex(tmpReg, tmpReg, patch.metadata.archMetadata.cond)));
  } else {
    tmpPatch4_block.push_back(NoReloc::unique(
        ldrex(tmpReg, tmpReg, patch.metadata.archMetadata.cond)));
  }
  tmpPatch4_block.push_back(
      Branch(cpumode, getUniquePtrVecSize(loadPatch, *patch.llvmcpu),
             /* addBranchLen */ true));

  RelocatableInst::UniquePtrVec tmpPatch4_cond;

  tmpPatch4_cond.push_back(Cmp(cpumode, tmpReg, 4));
  tmpPatch4_cond.push_back(
      BranchCC(cpumode, getUniquePtrVecSize(tmpPatch4_block, *patch.llvmcpu),
               llvm::ARMCC::CondCodes::NE,
               /* withinITBlock */ false, /* addBranchLen */ true));

  prepend(tmpPatch4_block, std::move(tmpPatch4_cond));
  prepend(loadPatch, std::move(tmpPatch4_block));

  // case 2 (LDREXH)
  RelocatableInst::UniquePtrVec tmpPatch2_block;

  tmpPatch2_block.push_back(Msr(cpumode, cpyFlags));
  tmpPatch2_block.push_back(LoadDataBlock::unique(
      tmpReg, offsetof(Context, gprState.localMonitor.addr)));
  if (cpumode == CPUMode::Thumb) {
    if (hasCond) {
      tmpPatch2_block.push_back(T2it(cpumode, patch.metadata.archMetadata.cond,
                                     (unsigned)llvm::ARM::PredBlockMask::T));
    }
    tmpPatch2_block.push_back(NoReloc::unique(
        t2ldrexh(tmpReg, tmpReg, patch.metadata.archMetadata.cond)));
  } else {
    tmpPatch2_block.push_back(NoReloc::unique(
        ldrexh(tmpReg, tmpReg, patch.metadata.archMetadata.cond)));
  }
  tmpPatch2_block.push_back(
      Branch(cpumode, getUniquePtrVecSize(loadPatch, *patch.llvmcpu),
             /* addBranchLen */ true));

  RelocatableInst::UniquePtrVec tmpPatch2_cond;

  tmpPatch2_cond.push_back(Cmp(cpumode, tmpReg, 2));
  tmpPatch2_cond.push_back(
      BranchCC(cpumode, getUniquePtrVecSize(tmpPatch2_block, *patch.llvmcpu),
               llvm::ARMCC::CondCodes::NE,
               /* withinITBlock */ false, /* addBranchLen */ true));

  prepend(tmpPatch2_block, std::move(tmpPatch2_cond));
  prepend(loadPatch, std::move(tmpPatch2_block));

  // case 1 (LDREXB)
  RelocatableInst::UniquePtrVec tmpPatch1_block;

  tmpPatch1_block.push_back(Msr(cpumode, cpyFlags));
  tmpPatch1_block.push_back(LoadDataBlock::unique(
      tmpReg, offsetof(Context, gprState.localMonitor.addr)));
  if (cpumode == CPUMode::Thumb) {
    if (hasCond) {
      tmpPatch1_block.push_back(T2it(cpumode, patch.metadata.archMetadata.cond,
                                     (unsigned)llvm::ARM::PredBlockMask::T));
    }
    tmpPatch1_block.push_back(NoReloc::unique(
        t2ldrexb(tmpReg, tmpReg, patch.metadata.archMetadata.cond)));
  } else {
    tmpPatch1_block.push_back(NoReloc::unique(
        ldrexb(tmpReg, tmpReg, patch.metadata.archMetadata.cond)));
  }
  tmpPatch1_block.push_back(
      Branch(cpumode, getUniquePtrVecSize(loadPatch, *patch.llvmcpu),
             /* addBranchLen */ true));

  RelocatableInst::UniquePtrVec finalPatch;

  finalPatch.push_back(Mrs(cpumode, cpyFlags));
  finalPatch.push_back(LoadDataBlock::unique(
      tmpReg, offsetof(Context, gprState.localMonitor.enable)));
  finalPatch.push_back(Cmp(cpumode, tmpReg, 1));
  finalPatch.push_back(
      BranchCC(cpumode, getUniquePtrVecSize(tmpPatch1_block, *patch.llvmcpu),
               llvm::ARMCC::CondCodes::NE,
               /* withinITBlock */ false, /* addBranchLen */ true));

  append(finalPatch, std::move(tmpPatch1_block));
  append(finalPatch, std::move(loadPatch));
  return finalPatch;
}

// SetCondReachAndJump
// ===================

PatchGenerator::UniquePtr SetCondReachAndJump::clone() const {
  PatchGenerator::UniquePtrVec newVec;

  for (const PatchGenerator::UniquePtr &g : patchVec) {
    newVec.push_back(g->clone());
  }

  return SetCondReachAndJump::unique(temp, tag, std::move(newVec));
}

RelocatableInst::UniquePtrVec
SetCondReachAndJump::generate(const Patch &patch,
                              TempManager &temp_manager) const {

  CPUMode cpumode = patch.llvmcpu->getCPUMode();

  if (patch.metadata.archMetadata.cond != llvm::ARMCC::AL) {

    RelocatableInst::UniquePtrVec instru;

    if (tag.getTag() != ShadowReservedTag::Untagged) {
      Reg tmpRegister = temp_manager.getRegForTemp(temp);

      instru.push_back(LoadImm::unique(tmpRegister, 0));
      if (cpumode == CPUMode::Thumb) {
        instru.push_back(T2it(cpumode, patch.metadata.archMetadata.cond,
                              (unsigned)llvm::ARM::PredBlockMask::T));
      }
      instru.push_back(
          LoadImmCC::unique(tmpRegister, 1, patch.metadata.archMetadata.cond));
      instru.push_back(StoreShadow::unique(tmpRegister, tag, true));
    }

    RelocatableInst::UniquePtrVec tmpInstru;
    for (const PatchGenerator::UniquePtr &g : patchVec) {
      append(tmpInstru, g->generate(patch, temp_manager));
    }

    rword patchSize = getUniquePtrVecSize(tmpInstru, *patch.llvmcpu);
    if (patchSize > 0) {
      instru.push_back(BranchCC(
          patch.llvmcpu->getCPUMode(), patchSize,
          llvm::ARMCC::getOppositeCondition(
              (llvm::ARMCC::CondCodes)patch.metadata.archMetadata.cond),
          /* withinITBlock */ false, /* addBranchLen */ true));

      append(instru, std::move(tmpInstru));
    }
    return instru;
  } else {
    RelocatableInst::UniquePtrVec instru;
    for (const PatchGenerator::UniquePtr &g : patchVec) {
      append(instru, g->generate(patch, temp_manager));
    }
    return instru;
  }
}

// ItPatch
// =======

RelocatableInst::UniquePtrVec
ItPatch::generate(const Patch &patch, TempManager &temp_manager) const {
  CPUMode cpumode = patch.llvmcpu->getCPUMode();
  QBDI_REQUIRE_ABORT_PATCH(cpumode == CPUMode::Thumb, patch,
                           "Only available in Thumb mode");

  // we return a null patch if the instruction isn't in a ITblock
  // If the instruction is in a ITblock with the condition AL, we must preserve
  // the block to keep the flags behavior
  if (patch.metadata.archMetadata.posITblock == 0) {
    for (unsigned i = 0; i < nbcond; i++) {
      QBDI_REQUIRE_ABORT_PATCH(
          cond[i] == false, patch,
          "Use ItPatch with invCond ({}) on a instruction outside of ITBlock",
          i);
    }
    return RelocatableInst::UniquePtrVec();
  }

  llvm::ARM::PredBlockMask mask = llvm::ARM::PredBlockMask::T;

  if (patch.metadata.archMetadata.cond == llvm::ARMCC::AL) {
    for (unsigned i = 0; i < nbcond; i++) {
      QBDI_REQUIRE_ABORT_PATCH(cond[i] == false, patch,
                               "Use ItPatch with invCond ({}) on AL cond", i);
    }
  }

  for (unsigned i = 1; i < nbcond; i++) {
    if (cond[0] ^ cond[i]) {
      mask = llvm::expandPredBlockMask(mask, llvm::ARMVCC::VPTCodes::Else);
    } else {
      mask = llvm::expandPredBlockMask(mask, llvm::ARMVCC::VPTCodes::Then);
    }
  }
  if (cond[0]) {
    return conv_unique<RelocatableInst>(
        T2it(cpumode,
             llvm::ARMCC::getOppositeCondition(
                 (llvm::ARMCC::CondCodes)patch.metadata.archMetadata.cond),
             (unsigned)mask));
  } else {
    return conv_unique<RelocatableInst>(
        T2it(cpumode, patch.metadata.archMetadata.cond, (unsigned)mask));
  }
}

// TPopPatchGen
// ============

RelocatableInst::UniquePtrVec
TPopPatchGen::generate(const Patch &patch, TempManager &temp_manager) const {

  const llvm::MCInst &inst = patch.metadata.inst;
  const CPUMode cpumode = patch.llvmcpu->getCPUMode();
  QBDI_REQUIRE_ABORT_PATCH(cpumode == CPUMode::Thumb, patch,
                           "Only available in Thumb mode");

  const unsigned numOperands = inst.getNumOperands();

  // verify if the instruction need PC
  QBDI_REQUIRE_ABORT_PATCH(0 < numOperands, patch, "Invalid instruction");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(numOperands - 1).isReg(), patch,
                           "Unexpected operand type");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(numOperands - 1).getReg() ==
                               GPR_ID[REG_PC],
                           patch, "Unexpected PC behavior");

  // number of register to pop (included pc)
  unsigned listRegsNum = numOperands - 2;

  // get tempReg
  Reg tempReg = temp_manager.getRegForTemp(temp);

  // begin the patch
  RelocatableInst::UniquePtrVec relocInstList;

  if (patch.metadata.archMetadata.cond != llvm::ARMCC::AL) {
    const unsigned invCond = llvm::ARMCC::getOppositeCondition(
        (llvm::ARMCC::CondCodes)patch.metadata.archMetadata.cond);
    if (listRegsNum > 1) {
      relocInstList.push_back(
          T2it(cpumode, invCond, (unsigned)llvm::ARM::PredBlockMask::TEE));
    } else {
      relocInstList.push_back(
          T2it(cpumode, invCond, (unsigned)llvm::ARM::PredBlockMask::TE));
    }
    relocInstList.push_back(LoadImmCC::unique(
        tempReg, Constant(patch.metadata.endAddress() | 1), invCond));
  }
  if (listRegsNum > 1) {
    // generate the pop instruction without PC
    llvm::MCInst instNoPC = inst;
    instNoPC.erase(instNoPC.begin() + (numOperands - 1));

    relocInstList.push_back(NoReloc::unique(std::move(instNoPC)));
  }

  relocInstList.push_back(NoReloc::unique(
      t2ldrPost(tempReg, GPR_ID[REG_SP], 4, patch.metadata.archMetadata.cond)));

  append(relocInstList, WritePC(temp).generate(patch, temp_manager));

  return relocInstList;
}

// T2LDMPatchGen
// =============

static void gent2LDMsubPatch(RelocatableInst::UniquePtrVec &vec,
                             const Patch &patch, Reg tempReg, RegLLVM addrReg,
                             unsigned startOp, unsigned endOp, bool pendingTmp,
                             bool IA) {

  const llvm::MCInst &inst = patch.metadata.inst;

  if (endOp <= startOp) {
    return;
  } else if (startOp + 1 == endOp) {
    QBDI_REQUIRE_ABORT_PATCH(startOp < inst.getNumOperands(), patch,
                             "Invalid operand");
    QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(startOp).isReg(), patch,
                             "Unexpected operand type");
    RegLLVM destReg = inst.getOperand(startOp).getReg();
    if (IA) {
      vec.push_back(NoReloc::unique(t2ldrPost(destReg, addrReg, 4)));
    } else {
      vec.push_back(NoReloc::unique(t2ldrPre(destReg, addrReg, -4)));
    }
    if (pendingTmp) {
      QBDI_REQUIRE_ABORT_PATCH(
          destReg == tempReg, patch,
          "The loaded register is expected to be the TempRegister");
      vec.push_back(StoreDataBlock::unique(tempReg, Offset(tempReg)));
    } else {
      QBDI_REQUIRE_ABORT_PATCH(
          destReg != tempReg, patch,
          "The loaded register isn't expected to be the TempRegister");
    }
  } else {
    unsigned mask = 0;
    bool foundTmpReg = false;
    QBDI_REQUIRE_ABORT_PATCH(endOp <= inst.getNumOperands(), patch,
                             "Invalid operand");
    for (unsigned i = startOp; i < endOp; i++) {
      QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(i).isReg(), patch,
                               "Unexpected operand {} type", i);
      RegLLVM r = inst.getOperand(i).getReg();
      switch (r.getValue()) {
        case llvm::ARM::R0:
        case llvm::ARM::R1:
        case llvm::ARM::R2:
        case llvm::ARM::R3:
        case llvm::ARM::R4:
        case llvm::ARM::R5:
        case llvm::ARM::R6:
        case llvm::ARM::R7:
        case llvm::ARM::R8:
        case llvm::ARM::R9:
        case llvm::ARM::R10:
        case llvm::ARM::R11:
        case llvm::ARM::R12:
          mask |= (1 << (r.getValue() - llvm::ARM::R0));
          break;
        case llvm::ARM::LR:
          mask |= (1 << 14);
          break;
        // SP cannot be set in Thumb
        case llvm::ARM::SP:
        // PC is handle by the caller and should be in the list
        case llvm::ARM::PC:
        default:
          QBDI_ABORT_PATCH(patch, "Unexpected register {}", r.getValue());
      }
      if (r == tempReg) {
        QBDI_REQUIRE_ABORT_PATCH(not foundTmpReg, patch,
                                 "TempReg already found");
        QBDI_REQUIRE_ABORT_PATCH(pendingTmp, patch,
                                 "Unexpected TempReg in the register list");
        foundTmpReg = true;
      }
    }
    if (IA) {
      vec.push_back(NoReloc::unique(t2ldmia(addrReg, mask, true)));
    } else {
      vec.push_back(NoReloc::unique(t2ldmdb(addrReg, mask, true)));
    }
    if (pendingTmp) {
      QBDI_REQUIRE_ABORT_PATCH(foundTmpReg, patch,
                               "TempReg not found in the register list");
      vec.push_back(StoreDataBlock::unique(tempReg, Offset(tempReg)));
    }
  }
}

RelocatableInst::UniquePtrVec
T2LDMPatchGen::generate(const Patch &patch, TempManager &temp_manager) const {

  const llvm::MCInst &inst = patch.metadata.inst;
  const CPUMode cpumode = patch.llvmcpu->getCPUMode();
  QBDI_REQUIRE_ABORT_PATCH(cpumode == CPUMode::Thumb, patch,
                           "Only available in Thumb mode");

  const unsigned opcode = inst.getOpcode();
  const unsigned numOperands = inst.getNumOperands();

  // verify if the instruction need PC
  QBDI_REQUIRE_ABORT_PATCH(0 < numOperands, patch, "Invalid instruction");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(numOperands - 1).isReg(), patch,
                           "Unexpected operand type");
  bool needPC = (inst.getOperand(numOperands - 1).getReg() == GPR_ID[REG_PC]);
  QBDI_REQUIRE_ABORT_PATCH(writePC == needPC, patch, "Unexpected PC behavior");

  // get baseAddr
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(0).isReg(), patch,
                           "Unexpected operand type");
  RegLLVM addrReg = inst.getOperand(0).getReg();

  // get the number of register in the register list
  unsigned listRegsNum;
  bool wback;

  switch (opcode) {
    case llvm::ARM::t2LDMIA:
    case llvm::ARM::t2LDMDB:
      listRegsNum = numOperands - 3;
      wback = false;
      break;
    case llvm::ARM::t2LDMIA_UPD:
    case llvm::ARM::t2LDMDB_UPD:
      listRegsNum = numOperands - 4;
      wback = true;
      break;
    default:
      QBDI_ABORT_PATCH(patch, "Unexpected instruction");
  }

  // verify if the flag RegisterUsage::RegisterSavedScratch is set
  Reg reservedSavedScratch = 0;
  bool unusedSavedScratch = true;
  bool foundSavedScratch = false;

  for (unsigned i = 0; i < AVAILABLE_GPR; i++) {
    if (patch.regUsage[i] != 0 and
        (patch.regUsage[i] & RegisterUsage::RegisterSavedScratch) != 0) {
      QBDI_REQUIRE_ABORT_PATCH(not foundSavedScratch, patch,
                               "Maximum one reservedSavedScratch");
      QBDI_REQUIRE_ABORT_PATCH(Reg(i) != addrReg, patch,
                               "baseReg must not be the scratchRegister");
      unusedSavedScratch =
          ((patch.regUsage[i] & RegisterUsage::RegisterBoth) == 0);
      reservedSavedScratch = Reg(i);
      foundSavedScratch = true;
    }
  }

  // for t2LDM, we can used the original instruction if:
  // - PC isn't in the register list AND
  // - reservedSavedScratch isn't set or used by the instruction
  if (unusedSavedScratch and not needPC) {
    RelocatableInst::UniquePtrVec relocInstList;
    append(relocInstList, ItPatch(false).generate(patch, temp_manager));
    append(relocInstList, ModifyInstruction(InstTransform::UniquePtrVec())
                              .generate(patch, temp_manager));
    // force the temp_manager to not allocate tempRegister in this case
    // Otherwith, we are not sure that the tempRegister isn't in the list of
    // restored registers.
    temp_manager.lockTempManager();
    QBDI_REQUIRE_ABORT_PATCH(temp_manager.getUsedRegisters().size() == 0, patch,
                             "Unexpected TempManager state");
    return relocInstList;
  }

  // lock tempRegister. Only 1 register may be a temp register
  Reg tempReg = temp_manager.getRegForTemp(temp);
  temp_manager.lockTempManager();
  QBDI_REQUIRE_ABORT_PATCH(temp_manager.getUsedRegisters().size() == 1, patch,
                           "Unexpected TempManager state");
  QBDI_REQUIRE_ABORT_PATCH(tempReg != addrReg, patch,
                           "tempRegister allocation error");
  QBDI_REQUIRE_ABORT_PATCH(tempReg != reservedSavedScratch or
                               not foundSavedScratch,
                           patch, "tempRegister allocation error");

  RelocatableInst::UniquePtrVec relocInstList;

  switch (opcode) {
    case llvm::ARM::t2LDMIA:
    case llvm::ARM::t2LDMIA_UPD: {
      // create a range of register to add
      unsigned firstPendingReg = numOperands - listRegsNum;
      // does the tmp register is in the range of loaded register
      bool pendingTmp = false;
      // manage the base register
      // If the base register is in the list, keep his position to load it at
      // the end
      bool addrRegInList = false;
      unsigned offsetAddrReg = 0;
      for (unsigned i = firstPendingReg; i < numOperands; i++) {
        QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(i).isReg(), patch,
                                 "Unexpected operand type");
        RegLLVM r = inst.getOperand(i).getReg();
        if (r == tempReg) {
          pendingTmp = true;
        } else if (r == reservedSavedScratch and foundSavedScratch) {
          gent2LDMsubPatch(relocInstList, patch, tempReg, addrReg,
                           firstPendingReg, i, pendingTmp, true);
          relocInstList.push_back(
              NoReloc::unique(t2ldrPost(tempReg, addrReg, 4)));
          relocInstList.push_back(MovToSavedScratchReg::unique(
              tempReg, reservedSavedScratch, llvm::ARMCC::AL));
          pendingTmp = false;
          firstPendingReg = i + 1;
        } else if (r == addrReg) {
          QBDI_REQUIRE_ABORT_PATCH(
              not wback, patch,
              "Writeback when the address register is loaded is Undefined");
          gent2LDMsubPatch(relocInstList, patch, tempReg, addrReg,
                           firstPendingReg, i, pendingTmp, true);
          // skip the value for now
          append(relocInstList, Addc(cpumode, addrReg, addrReg, 4, tempReg));
          pendingTmp = false;
          firstPendingReg = i + 1;
          addrRegInList = true;
          offsetAddrReg = i - (numOperands - listRegsNum);
        } else if (r == Reg(REG_PC)) {
          gent2LDMsubPatch(relocInstList, patch, tempReg, addrReg,
                           firstPendingReg, i, pendingTmp, true);
          relocInstList.push_back(
              NoReloc::unique(t2ldrPost(tempReg, addrReg, 4)));
          append(
              relocInstList,
              WritePC(temp, /* dropCond */ true).generate(patch, temp_manager));
          pendingTmp = false;
          firstPendingReg = i + 1;
        }
      }
      gent2LDMsubPatch(relocInstList, patch, tempReg, addrReg, firstPendingReg,
                       numOperands, pendingTmp, true);
      if (addrRegInList) {
        relocInstList.push_back(NoReloc::unique(
            t2ldri8(addrReg, addrReg, -4 * (listRegsNum - offsetAddrReg))));
      } else if (not wback) {
        append(relocInstList,
               Addc(cpumode, addrReg, addrReg, -4 * listRegsNum, tempReg));
      }
      break;
    }
    case llvm::ARM::t2LDMDB:
    case llvm::ARM::t2LDMDB_UPD: {
      unsigned lastPendingReg = numOperands;
      bool pendingTmp = false;
      bool addrRegInList = false;
      unsigned offsetAddrReg = 0;
      for (unsigned i = numOperands - 1; i >= (numOperands - listRegsNum);
           i--) {
        QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(i).isReg(), patch,
                                 "Unexpected operand type");
        RegLLVM r = inst.getOperand(i).getReg();
        if (r == tempReg) {
          pendingTmp = true;
        } else if (r == reservedSavedScratch and foundSavedScratch) {
          gent2LDMsubPatch(relocInstList, patch, tempReg, addrReg, i + 1,
                           lastPendingReg, pendingTmp, false);
          relocInstList.push_back(
              NoReloc::unique(t2ldrPre(tempReg, addrReg, -4)));
          relocInstList.push_back(MovToSavedScratchReg::unique(
              tempReg, reservedSavedScratch, llvm::ARMCC::AL));
          pendingTmp = false;
          lastPendingReg = i;
        } else if (r == addrReg) {
          QBDI_REQUIRE_ABORT_PATCH(
              not wback, patch,
              "Writeback when the address register is loaded is Undefined");
          gent2LDMsubPatch(relocInstList, patch, tempReg, addrReg, i + 1,
                           lastPendingReg, pendingTmp, false);
          // skip the value for now
          append(relocInstList, Addc(cpumode, addrReg, addrReg, -4, tempReg));
          pendingTmp = false;
          lastPendingReg = i;
          addrRegInList = true;
          offsetAddrReg = i - (numOperands - listRegsNum);
        } else if (r == Reg(REG_PC)) {
          gent2LDMsubPatch(relocInstList, patch, tempReg, addrReg, i + 1,
                           lastPendingReg, pendingTmp, false);
          relocInstList.push_back(
              NoReloc::unique(t2ldrPre(tempReg, addrReg, -4)));
          append(
              relocInstList,
              WritePC(temp, /* dropCond */ true).generate(patch, temp_manager));
          pendingTmp = false;
          lastPendingReg = i;
        }
      }
      gent2LDMsubPatch(relocInstList, patch, tempReg, addrReg,
                       numOperands - listRegsNum, lastPendingReg, pendingTmp,
                       false);
      if (addrRegInList) {
        relocInstList.push_back(
            NoReloc::unique(t2ldri12(addrReg, addrReg, 4 * offsetAddrReg)));
      } else if (not wback) {
        append(relocInstList,
               Addc(cpumode, addrReg, addrReg, 4 * listRegsNum, tempReg));
      }
      break;
    }
    default:
      QBDI_ABORT_PATCH(patch, "Unexpected instruction");
  }
  if (patch.metadata.archMetadata.cond != llvm::ARMCC::AL) {
    const unsigned invCond = llvm::ARMCC::getOppositeCondition(
        (llvm::ARMCC::CondCodes)patch.metadata.archMetadata.cond);
    rword patchSize = getUniquePtrVecSize(relocInstList, *patch.llvmcpu);

    if (needPC) {
      // if need pc, create a it block to set PC and jump over if the condition
      // isn't reach
      RelocatableInst::UniquePtrVec relocInstListPre;

      relocInstListPre.push_back(
          T2it(cpumode, invCond, (unsigned)llvm::ARM::PredBlockMask::TTT));
      relocInstListPre.push_back(LoadImmCC::unique(
          tempReg, Constant(patch.metadata.endAddress() | 1), invCond));
      // no WritePC here, because we generate the case where the condition isn't
      // reach. We also need to have a fine control of the number of instruction
      // for the current ITblock
      relocInstListPre.push_back(
          StoreDataBlockCC::unique(tempReg, Offset(Reg(REG_PC)), invCond));
      relocInstListPre.push_back(BranchCC(cpumode, patchSize, invCond,
                                          /* withinITBlock */ true,
                                          /* addBranchLen */ true));

      prepend(relocInstList, std::move(relocInstListPre));
    } else {
      // if no need of PC, just jump over the whole patch
      relocInstList.insert(relocInstList.begin(),
                           BranchCC(cpumode, patchSize, invCond,
                                    /* withinITBlock */ false,
                                    /* addBranchLen */ true));
    }
  }
  return relocInstList;
}

// T2STMPatchGen
// =============

RelocatableInst::UniquePtrVec
T2STMPatchGen::generate(const Patch &patch, TempManager &temp_manager) const {

  const llvm::MCInst &inst = patch.metadata.inst;
  const CPUMode cpumode = patch.llvmcpu->getCPUMode();
  QBDI_REQUIRE_ABORT_PATCH(cpumode == CPUMode::Thumb, patch,
                           "Only available in Thumb mode");

  const unsigned opcode = inst.getOpcode();
  const unsigned numOperands = inst.getNumOperands();

  // verify if the instruction need PC (PC not supported)
  QBDI_REQUIRE_ABORT_PATCH(0 < numOperands, patch, "Invalid instruction");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(numOperands - 1).isReg(), patch,
                           "Unexpected operand type");
  bool needPC = (inst.getOperand(numOperands - 1).getReg() == GPR_ID[REG_PC]);
  QBDI_REQUIRE_ABORT_PATCH(not needPC, patch, "T2STM with PC is undefined");

  // get baseAddr
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(0).isReg(), patch,
                           "Unexpected operand type");
  RegLLVM addrReg = inst.getOperand(0).getReg();

  // get the number of register in the register list
  unsigned listRegsNum;

  switch (opcode) {
    case llvm::ARM::t2STMIA:
    case llvm::ARM::t2STMDB:
      listRegsNum = numOperands - 3;
      break;
    case llvm::ARM::t2STMIA_UPD:
    case llvm::ARM::t2STMDB_UPD:
      listRegsNum = numOperands - 4;
      break;
    default:
      QBDI_ABORT_PATCH(patch, "Unexpected instruction");
  }

  // verify if the flag RegisterUsage::RegisterSavedScratch is set
  Reg reservedSavedScratch = 0;
  bool unusedSavedScratch = true;
  bool foundSavedScratch = false;

  for (unsigned i = 0; i < AVAILABLE_GPR; i++) {
    if (patch.regUsage[i] != 0 and
        (patch.regUsage[i] & RegisterUsage::RegisterSavedScratch) != 0) {
      QBDI_REQUIRE_ABORT_PATCH(not foundSavedScratch, patch,
                               "Maximum one reservedSavedScratch");
      QBDI_REQUIRE_ABORT_PATCH(Reg(i) != addrReg, patch,
                               "baseReg must not be the scratchRegister");
      unusedSavedScratch =
          ((patch.regUsage[i] & RegisterUsage::RegisterBoth) == 0);
      reservedSavedScratch = Reg(i);
      foundSavedScratch = true;
    }
  }

  // for t2STM, we can used the original instruction if:
  // - reservedSavedScratch isn't set or used by the instruction
  if (unusedSavedScratch) {
    RelocatableInst::UniquePtrVec relocInstList;
    append(relocInstList, ItPatch(false).generate(patch, temp_manager));
    append(relocInstList, ModifyInstruction(InstTransform::UniquePtrVec())
                              .generate(patch, temp_manager));
    // force the temp_manager to not allocate tempRegister in this case
    // Otherwith, we are not sure that the tempRegister isn't in the list of
    // restored registers.
    temp_manager.lockTempManager();
    QBDI_REQUIRE_ABORT_PATCH(temp_manager.getUsedRegisters().size() == 0, patch,
                             "Unexpected TempManager state");
    return relocInstList;
  }

  // lock tempRegister. Only 1 register may be a temp register
  Reg tempReg = temp_manager.getRegForTemp(temp);
  temp_manager.lockTempManager();
  QBDI_REQUIRE_ABORT_PATCH(temp_manager.getUsedRegisters().size() == 1, patch,
                           "Unexpected TempManager state");
  QBDI_REQUIRE_ABORT_PATCH(tempReg != addrReg, patch,
                           "tempRegister allocation error");
  QBDI_REQUIRE_ABORT_PATCH(tempReg != reservedSavedScratch, patch,
                           "tempRegister allocation error");

  // verify if the tempReg is a register on the register list
  // This must be the case, otherwise, the scratch register should be reserved
  // on a free register
  bool tempIsNeeded =
      (patch.regUsage[tempReg.getID()] & RegisterUsage::RegisterBoth) != 0;
  QBDI_REQUIRE_ABORT_PATCH(tempIsNeeded, patch,
                           "Unexpected TempRegister state");

  // search position of scratch register in the list
  bool foundSavedScratchPosition = false;
  unsigned savedScratchPosition = 0;

  for (unsigned int i = (numOperands - listRegsNum); i < numOperands; i++) {
    QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(i).isReg(), patch,
                             "Unexpected operand type");
    RegLLVM r = inst.getOperand(i).getReg();
    if (r == reservedSavedScratch) {
      foundSavedScratchPosition = true;
      savedScratchPosition = i - (numOperands - listRegsNum);
      break;
    }
  }
  QBDI_REQUIRE_ABORT_PATCH(foundSavedScratchPosition, patch,
                           "Unexpected usage of the Scratch Register");

  RelocatableInst::UniquePtrVec relocInstList;

  if (patch.metadata.archMetadata.cond != llvm::ARMCC::AL) {
    append(relocInstList,
           ItPatch(false, false, false, false).generate(patch, temp_manager));
  }

  // load the value of the tempRegister from the Datablock
  relocInstList.push_back(LoadDataBlockCC::unique(
      tempReg, Offset(tempReg), patch.metadata.archMetadata.cond));

  // apply the instruction (the value of the scratch register will be fixed
  // after) appling the same instruction allow us to have the same behaviour if
  // the base register is stored.
  append(relocInstList, ModifyInstruction(InstTransform::UniquePtrVec())
                            .generate(patch, temp_manager));

  // Get ScratchRegister
  relocInstList.push_back(MovFromSavedScratchReg::unique(
      reservedSavedScratch, tempReg, patch.metadata.archMetadata.cond));

  unsigned fixOffset;

  switch (inst.getOpcode()) {
    case llvm::ARM::t2STMIA:
    case llvm::ARM::t2STMDB_UPD:
      fixOffset = 4 * savedScratchPosition;
      break;
    case llvm::ARM::t2STMDB:
    case llvm::ARM::t2STMIA_UPD:
      fixOffset = -4 * (listRegsNum - savedScratchPosition);
      break;
    default:
      QBDI_ABORT_PATCH(patch,
                       "STMPatchGen should not be used for this instruction:");
  }

  // store ScratchRegister
  relocInstList.push_back(NoReloc::unique(
      t2stri8(tempReg, addrReg, fixOffset, patch.metadata.archMetadata.cond)));

  if (patch.metadata.archMetadata.cond != llvm::ARMCC::AL) {
    QBDI_REQUIRE_ABORT_PATCH(relocInstList.size() == 5, patch,
                             "Unexpected patch size {}", relocInstList.size());
  }

  return relocInstList;
}

// T2TBBTBHPatchGen
// ================

RelocatableInst::UniquePtrVec
T2TBBTBHPatchGen::generate(const Patch &patch,
                           TempManager &temp_manager) const {

  const llvm::MCInst &inst = patch.metadata.inst;
  const CPUMode cpumode = patch.llvmcpu->getCPUMode();
  QBDI_REQUIRE_ABORT_PATCH(cpumode == CPUMode::Thumb, patch,
                           "Only available in Thumb mode");

  const unsigned opcode = inst.getOpcode();

  // get instruction operand
  QBDI_REQUIRE_ABORT_PATCH(2 <= inst.getNumOperands(), patch,
                           "Invalid instruction");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(0).isReg(), patch,
                           "Unexpected operand type");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(1).isReg(), patch,
                           "Unexpected operand type");
  RegLLVM baseReg = inst.getOperand(0).getReg();
  RegLLVM indexReg = inst.getOperand(1).getReg();

  QBDI_REQUIRE_ABORT_PATCH(baseReg != GPR_ID[REG_SP], patch,
                           "Unsupported SP in baseRegister");
  QBDI_REQUIRE_ABORT_PATCH(indexReg != GPR_ID[REG_SP], patch,
                           "Unsupported SP in indexRegister");
  QBDI_REQUIRE_ABORT_PATCH(indexReg != GPR_ID[REG_PC], patch,
                           "Unexpected PC in indexRegister");

  // get our tempRegister
  Reg tempReg1 = temp_manager.getRegForTemp(temp1);
  Reg tempReg2 = temp_manager.getRegForTemp(temp2);

  // begin patch
  RelocatableInst::UniquePtrVec relocInstList;

  // 1. get base address in tempRegister (if baseReg == PC, get PC value)
  if (baseReg == GPR_ID[REG_PC]) {
    append(relocInstList, GetPCOffset(temp1, Constant(0), /* keepCond */ false)
                              .generate(patch, temp_manager));
    baseReg = tempReg1;
  }

  // 2. get next PC address
  // note: TBB and TBH use BranchWritePC => the next instruction mode is always
  // Thumb
  //       The value returns by GetNextInstAddr is always odd.
  append(relocInstList, GetNextInstAddr(temp2, /* keepCond */ false)
                            .generate(patch, temp_manager));

  // 3. load offset in tempReg1 and add the offset with the next address
  append(relocInstList, ItPatch(false, false).generate(patch, temp_manager));
  switch (opcode) {
    case llvm::ARM::t2TBB:
      relocInstList.push_back(NoReloc::unique(t2ldrbr(
          tempReg1, baseReg, indexReg, patch.metadata.archMetadata.cond)));
      break;
    case llvm::ARM::t2TBH:
      relocInstList.push_back(NoReloc::unique(t2ldrhrs(
          tempReg1, baseReg, indexReg, 1, patch.metadata.archMetadata.cond)));
      break;
    default:
      QBDI_ABORT_PATCH(patch, "Unexpected instruction");
  }
  relocInstList.push_back(NoReloc::unique(
      t2addrsi(tempReg2, tempReg2, tempReg1, 1, llvm::ARM_AM::lsl,
               patch.metadata.archMetadata.cond)));

  // 4. save next PC
  append(relocInstList, WritePC(temp2).generate(patch, temp_manager));

  return relocInstList;
}

// T2BXAUTPatchGen
// ===============

RelocatableInst::UniquePtrVec
T2BXAUTPatchGen::generate(const Patch &patch, TempManager &temp_manager) const {

  const llvm::MCInst &inst = patch.metadata.inst;
  const CPUMode cpumode = patch.llvmcpu->getCPUMode();
  QBDI_REQUIRE_ABORT_PATCH(cpumode == CPUMode::Thumb, patch,
                           "Only available in Thumb mode");

  // get instruction operand
  QBDI_REQUIRE_ABORT_PATCH(5 <= inst.getNumOperands(), patch,
                           "Invalid instruction");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(2).isReg(), patch,
                           "Unexpected operand type");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(3).isReg(), patch,
                           "Unexpected operand type");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(4).isReg(), patch,
                           "Unexpected operand type");
  RegLLVM targetReg = inst.getOperand(2).getReg();
  RegLLVM contextReg = inst.getOperand(3).getReg();
  RegLLVM encryptedReg = inst.getOperand(4).getReg();

  // begin patch
  RelocatableInst::UniquePtrVec relocInstList;

  if (patch.metadata.archMetadata.cond != llvm::ARMCC::AL) {
    append(relocInstList, ItPatch(false).generate(patch, temp_manager));
  }
  relocInstList.push_back(NoReloc::unique(t2autg(
      targetReg, contextReg, encryptedReg, patch.metadata.archMetadata.cond)));

  return relocInstList;
}

} // namespace QBDI
