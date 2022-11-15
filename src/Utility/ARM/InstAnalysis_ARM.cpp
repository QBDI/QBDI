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

#include "Utils/ARMBaseInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrDesc.h"

#include "Patch/ARM/InstInfo_ARM.h"
#include "Patch/Register.h"
#include "Patch/Types.h"
#include "Utility/InstAnalysis_prive.h"
#include "Utility/LogSys.h"

namespace QBDI {
namespace InstructionAnalysis {

ConditionType ConditionLLVM2QBDI(unsigned cond) {
  switch (cond) {
    case llvm::ARMCC::CondCodes::EQ:
      return CONDITION_EQUALS;
    case llvm::ARMCC::CondCodes::NE:
      return CONDITION_NOT_EQUALS;
    case llvm::ARMCC::CondCodes::HS:
      return CONDITION_ABOVE_EQUALS;
    case llvm::ARMCC::CondCodes::LO:
      return CONDITION_BELOW;
    case llvm::ARMCC::CondCodes::MI:
      return CONDITION_SIGN;
    case llvm::ARMCC::CondCodes::PL:
      return CONDITION_NOT_SIGN;
    case llvm::ARMCC::CondCodes::VS:
      return CONDITION_OVERFLOW;
    case llvm::ARMCC::CondCodes::VC:
      return CONDITION_NOT_OVERFLOW;
    case llvm::ARMCC::CondCodes::HI:
      return CONDITION_ABOVE;
    case llvm::ARMCC::CondCodes::LS:
      return CONDITION_BELOW_EQUALS;
    case llvm::ARMCC::CondCodes::GE:
      return CONDITION_GREAT_EQUALS;
    case llvm::ARMCC::CondCodes::LT:
      return CONDITION_LESS;
    case llvm::ARMCC::CondCodes::GT:
      return CONDITION_GREAT;
    case llvm::ARMCC::CondCodes::LE:
      return CONDITION_LESS_EQUALS;
    case llvm::ARMCC::CondCodes::AL:
      return CONDITION_ALWAYS;
    default:
      QBDI_ABORT("Unsupported LLVM condition {}", cond);
  }
}

void analyseCondition(InstAnalysis *instAnalysis, const llvm::MCInst &inst,
                      const llvm::MCInstrDesc &desc, const LLVMCPU &llvmcpu) {

  instAnalysis->condition = ConditionLLVM2QBDI(getCondition(inst, llvmcpu));

  if (instAnalysis->condition == CONDITION_ALWAYS) {
    instAnalysis->condition = CONDITION_NONE;
  } else {
    instAnalysis->flagsAccess |= REGISTER_READ;
  }

  for (unsigned int opn = 0; opn < desc.getNumOperands(); opn++) {
    const llvm::MCOperandInfo *opInfo = desc.opInfo_begin() + opn;
    if (opInfo->RegClass == llvm::ARM::CCRRegClassID) {
      const llvm::MCOperand &op = inst.getOperand(opn);
      if (op.isReg() and op.getReg() == llvm::ARM::CPSR) {
        instAnalysis->flagsAccess |= REGISTER_WRITE;
      }
      break;
    }
  }

  // fix LLVM flag used/set
  switch (inst.getOpcode()) {
    default:
      break;
    case llvm::ARM::MRS:
    case llvm::ARM::SEL:
    case llvm::ARM::t2MRS_AR:
    case llvm::ARM::t2SEL:
      instAnalysis->flagsAccess |= REGISTER_READ;
      break;
    case llvm::ARM::MSR:
    case llvm::ARM::MSRi:
    case llvm::ARM::QADD:
    case llvm::ARM::QDADD:
    case llvm::ARM::QDSUB:
    case llvm::ARM::QSUB:
    case llvm::ARM::SADD16:
    case llvm::ARM::SADD8:
    case llvm::ARM::SASX:
    case llvm::ARM::SMLABB:
    case llvm::ARM::SMLABT:
    case llvm::ARM::SMLAD:
    case llvm::ARM::SMLADX:
    case llvm::ARM::SMLATB:
    case llvm::ARM::SMLATT:
    case llvm::ARM::SMLAWB:
    case llvm::ARM::SMLAWT:
    case llvm::ARM::SMLSD:
    case llvm::ARM::SMLSDX:
    case llvm::ARM::SMUAD:
    case llvm::ARM::SMUADX:
    case llvm::ARM::SSAT16:
    case llvm::ARM::SSAT:
    case llvm::ARM::SSAX:
    case llvm::ARM::SSUB16:
    case llvm::ARM::SSUB8:
    case llvm::ARM::UADD16:
    case llvm::ARM::UADD8:
    case llvm::ARM::UASX:
    case llvm::ARM::USAT16:
    case llvm::ARM::USAT:
    case llvm::ARM::USAX:
    case llvm::ARM::USUB16:
    case llvm::ARM::USUB8:
    case llvm::ARM::t2MSR_AR:
    case llvm::ARM::t2QADD:
    case llvm::ARM::t2QDADD:
    case llvm::ARM::t2QDSUB:
    case llvm::ARM::t2QSUB:
    case llvm::ARM::t2SADD16:
    case llvm::ARM::t2SADD8:
    case llvm::ARM::t2SASX:
    case llvm::ARM::t2SMLABB:
    case llvm::ARM::t2SMLABT:
    case llvm::ARM::t2SMLAD:
    case llvm::ARM::t2SMLADX:
    case llvm::ARM::t2SMLATB:
    case llvm::ARM::t2SMLATT:
    case llvm::ARM::t2SMLAWB:
    case llvm::ARM::t2SMLAWT:
    case llvm::ARM::t2SMLSD:
    case llvm::ARM::t2SMLSDX:
    case llvm::ARM::t2SMUAD:
    case llvm::ARM::t2SMUADX:
    case llvm::ARM::t2SSAT16:
    case llvm::ARM::t2SSAT:
    case llvm::ARM::t2SSAX:
    case llvm::ARM::t2SSUB16:
    case llvm::ARM::t2SSUB8:
    case llvm::ARM::t2UADD16:
    case llvm::ARM::t2UADD8:
    case llvm::ARM::t2UASX:
    case llvm::ARM::t2USAT16:
    case llvm::ARM::t2USAT:
    case llvm::ARM::t2USAX:
    case llvm::ARM::t2USUB16:
    case llvm::ARM::t2USUB8:
      instAnalysis->flagsAccess |= REGISTER_WRITE;
      break;
  }
}

bool isFlagOperand(unsigned opcode, unsigned opNum,
                   const llvm::MCOperandInfo &opdesc) {
  if (opdesc.RegClass == llvm::ARM::CCRRegClassID)
    return true;
  return opdesc.isPredicate();
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

  switch (inst.getOpcode()) {
    case llvm::ARM::BX_RET:
      return 1;
    default:
      return 0;
  }
}

void getAdditionnalOperand(InstAnalysis *instAnalysis, const llvm::MCInst &inst,
                           const llvm::MCInstrDesc &desc,
                           const llvm::MCRegisterInfo &MRI) {
  switch (inst.getOpcode()) {
    default:
      break;
    case llvm::ARM::BX_RET: {
      // add LR
      OperandAnalysis &opa2 = instAnalysis->operands[instAnalysis->numOperands];
      analyseRegister(opa2, GPR_ID[REG_LR], MRI);
      opa2.regAccess = REGISTER_READ;
      opa2.flag |= OPERANDFLAG_IMPLICIT;
      instAnalysis->numOperands++;
      // try to merge with a previous one
      tryMergeCurrentRegister(instAnalysis);
      break;
    }
  }
}

} // namespace InstructionAnalysis
} // namespace QBDI
