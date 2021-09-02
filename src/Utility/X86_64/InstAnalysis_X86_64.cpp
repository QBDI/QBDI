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
#include <stdlib.h>

#include "MCTargetDesc/X86BaseInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrDesc.h"

#include "QBDI/Bitmask.h"
#include "QBDI/InstAnalysis.h"
#include "QBDI/State.h"
#include "Patch/Register.h"
#include "Patch/X86_64/InstInfo_X86_64.h"
#include "Utility/InstAnalysis_prive.h"
#include "Utility/LogSys.h"

namespace llvm {
class MCRegisterInfo;
}

namespace QBDI {
namespace InstructionAnalysis {

static ConditionType ConditionLLVM2QBDI(unsigned cond) {
  switch (cond) {
    case llvm::X86::CondCode::COND_E:
      return CONDITION_EQUALS;
    case llvm::X86::CondCode::COND_NE:
      return CONDITION_NOT_EQUALS;
    case llvm::X86::CondCode::COND_A:
      return CONDITION_ABOVE;
    case llvm::X86::CondCode::COND_BE:
      return CONDITION_BELOW_EQUALS;
    case llvm::X86::CondCode::COND_AE:
      return CONDITION_ABOVE_EQUALS;
    case llvm::X86::CondCode::COND_B:
      return CONDITION_BELOW;
    case llvm::X86::CondCode::COND_G:
      return CONDITION_GREAT;
    case llvm::X86::CondCode::COND_LE:
      return CONDITION_LESS_EQUALS;
    case llvm::X86::CondCode::COND_GE:
      return CONDITION_GREAT_EQUALS;
    case llvm::X86::CondCode::COND_L:
      return CONDITION_LESS;
    case llvm::X86::CondCode::COND_P:
      return CONDITION_EVEN;
    case llvm::X86::CondCode::COND_NP:
      return CONDITION_ODD;
    case llvm::X86::CondCode::COND_O:
      return CONDITION_OVERFLOW;
    case llvm::X86::CondCode::COND_NO:
      return CONDITION_NOT_OVERFLOW;
    case llvm::X86::CondCode::COND_S:
      return CONDITION_SIGN;
    case llvm::X86::CondCode::COND_NS:
      return CONDITION_NOT_SIGN;
    default:
      QBDI_ERROR("Unsupported LLVM condition {}", cond);
      abort();
  }
}

void analyseCondition(InstAnalysis *instAnalysis, const llvm::MCInst &inst,
                      const llvm::MCInstrDesc &desc) {
  const unsigned numOperands = inst.getNumOperands();
  for (unsigned i = 0; i < numOperands; i++) {
    const llvm::MCOperandInfo &opdesc = desc.OpInfo[i];
    if (opdesc.OperandType == llvm::X86::OperandType::OPERAND_COND_CODE) {
      instAnalysis->condition = ConditionLLVM2QBDI(
          static_cast<unsigned>(inst.getOperand(i).getImm()));
      return;
    }
  }
  instAnalysis->condition = CONDITION_NONE;
}

bool isFlagOperand(unsigned opcode, unsigned opNum, unsigned operandType) {
  switch (operandType) {
    default:
      return false;
    case llvm::X86::OperandType::OPERAND_COND_CODE:
      return true;
  }
}

unsigned getBias(const llvm::MCInstrDesc &desc) {
  return llvm::X86II::getOperandBias(desc);
}

unsigned getAdditionnalOperandNumber(const llvm::MCInst &inst,
                                     const llvm::MCInstrDesc &desc) {

  if ((desc.isReturn() and isStackRead(inst)) or
      (desc.isCall() and isStackWrite(inst))) {
    return 1;
  } else {
    return 0;
  }
}

void getAdditionnalOperand(InstAnalysis *instAnalysis, const llvm::MCInst &inst,
                           const llvm::MCInstrDesc &desc,
                           const llvm::MCRegisterInfo &MRI) {

  if ((desc.isReturn() and isStackRead(inst)) or
      (desc.isCall() and isStackWrite(inst))) {
    // increment or decrement SP
    OperandAnalysis &opa2 = instAnalysis->operands[instAnalysis->numOperands];
    analyseRegister(opa2, GPR_ID[REG_SP], MRI);
    opa2.regAccess = REGISTER_READ_WRITE;
    opa2.flag |= OPERANDFLAG_IMPLICIT;
    instAnalysis->numOperands++;
    // try to merge with a previous one
    tryMergeCurrentRegister(instAnalysis);
  }
}

} // namespace InstructionAnalysis
} // namespace QBDI
