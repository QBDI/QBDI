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

#include <map>
#include <string>

#include "Utils/AArch64BaseInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrDesc.h"

#include "Patch/AARCH64/InstInfo_AARCH64.h"
#include "Patch/Register.h"
#include "Patch/Types.h"
#include "Utility/InstAnalysis_prive.h"
#include "Utility/LogSys.h"

namespace QBDI {
namespace InstructionAnalysis {

ConditionType ConditionLLVM2QBDI(unsigned cond) {
  switch (cond) {
    case llvm::AArch64CC::CondCode::EQ:
      return CONDITION_EQUALS;
    case llvm::AArch64CC::CondCode::NE:
      return CONDITION_NOT_EQUALS;
    case llvm::AArch64CC::CondCode::HS:
      return CONDITION_ABOVE_EQUALS;
    case llvm::AArch64CC::CondCode::LO:
      return CONDITION_BELOW;
    case llvm::AArch64CC::CondCode::MI:
      return CONDITION_SIGN;
    case llvm::AArch64CC::CondCode::PL:
      return CONDITION_NOT_SIGN;
    case llvm::AArch64CC::CondCode::VS:
      return CONDITION_OVERFLOW;
    case llvm::AArch64CC::CondCode::VC:
      return CONDITION_NOT_OVERFLOW;
    case llvm::AArch64CC::CondCode::HI:
      return CONDITION_ABOVE;
    case llvm::AArch64CC::CondCode::LS:
      return CONDITION_BELOW_EQUALS;
    case llvm::AArch64CC::CondCode::GE:
      return CONDITION_GREAT_EQUALS;
    case llvm::AArch64CC::CondCode::LT:
      return CONDITION_LESS;
    case llvm::AArch64CC::CondCode::GT:
      return CONDITION_GREAT;
    case llvm::AArch64CC::CondCode::LE:
      return CONDITION_LESS_EQUALS;
    case llvm::AArch64CC::CondCode::AL:
    case llvm::AArch64CC::CondCode::NV:
      return CONDITION_ALWAYS;
    default:
      QBDI_ABORT("Unsupported LLVM condition {}", cond);
  }
}

static unsigned getFlagOperand(unsigned opcode) {
  switch (opcode) {
    case llvm::AArch64::Bcc:
      return 0;
    case llvm::AArch64::CCMNWi:
    case llvm::AArch64::CCMNWr:
    case llvm::AArch64::CCMNXi:
    case llvm::AArch64::CCMNXr:
    case llvm::AArch64::CCMPWi:
    case llvm::AArch64::CCMPWr:
    case llvm::AArch64::CCMPXi:
    case llvm::AArch64::CCMPXr:
    case llvm::AArch64::CSELWr:
    case llvm::AArch64::CSELXr:
    case llvm::AArch64::CSINCWr:
    case llvm::AArch64::CSINCXr:
    case llvm::AArch64::CSINVWr:
    case llvm::AArch64::CSINVXr:
    case llvm::AArch64::CSNEGWr:
    case llvm::AArch64::CSNEGXr:
    case llvm::AArch64::F128CSEL:
    case llvm::AArch64::FCCMPDrr:
    case llvm::AArch64::FCCMPEDrr:
    case llvm::AArch64::FCCMPEHrr:
    case llvm::AArch64::FCCMPESrr:
    case llvm::AArch64::FCCMPHrr:
    case llvm::AArch64::FCCMPSrr:
    case llvm::AArch64::FCSELDrrr:
    case llvm::AArch64::FCSELHrrr:
    case llvm::AArch64::FCSELSrrr:
      return 3;
    default:
      return -1;
  }
}

void analyseCondition(InstAnalysis *instAnalysis, const llvm::MCInst &inst,
                      const llvm::MCInstrDesc &desc, const LLVMCPU &llvmcpu) {

  instAnalysis->condition = CONDITION_NONE;

  unsigned n = getFlagOperand(inst.getOpcode());
  if (n == ((unsigned)-1)) {
    return;
  }
  QBDI_REQUIRE_ACTION(n < inst.getNumOperands(), return );
  QBDI_REQUIRE_ACTION(inst.getOperand(n).isImm(), return );

  instAnalysis->condition =
      ConditionLLVM2QBDI(static_cast<unsigned>(inst.getOperand(n).getImm()));
}

bool isFlagOperand(unsigned opcode, unsigned opNum,
                   const llvm::MCOperandInfo &opdesc) {
  unsigned n = getFlagOperand(opcode);
  if (n == ((unsigned)-1)) {
    return false;
  }

  return n == opNum;
}

unsigned getBias(const llvm::MCInstrDesc &desc) {
  unsigned NumDefs = desc.getNumDefs();
  unsigned NumOps = desc.getNumOperands();
  unsigned bias = 0;

  for (unsigned opDef = 0; opDef < NumDefs; opDef++) {
    bool found = false;
    for (unsigned op = opDef + 1; op < NumOps; op++) {
      if (desc.getOperandConstraint(op, llvm::MCOI::TIED_TO) == (int)opDef) {
        bias = opDef + 1;
        found = true;
        break;
      }
    }
    if (not found) {
      break;
    }
  }

  return bias;
}

unsigned getAdditionnalOperandNumber(const llvm::MCInst &inst,
                                     const llvm::MCInstrDesc &desc) {

  return 0;
}

void getAdditionnalOperand(InstAnalysis *instAnalysis, const llvm::MCInst &inst,
                           const llvm::MCInstrDesc &desc,
                           const llvm::MCRegisterInfo &MRI) {}

} // namespace InstructionAnalysis
} // namespace QBDI
