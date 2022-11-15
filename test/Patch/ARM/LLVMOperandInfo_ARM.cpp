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
#include <set>

#include <catch2/catch.hpp>
#include <stdio.h>

#include "ARMInstrInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrDesc.h"

#include "TestSetup/LLVMTestEnv.h"
#include "Patch/InstInfo.h"

class LLVMOperandInfoCheck : public LLVMTestEnv {
protected:
  void checkPredicate(QBDI::CPUMode cpuMode);
  void checkTIEDOperand(QBDI::CPUMode cpuMode);
};

// LLVM MCInstrDesc doesn't have a specific OperandInfo for the condition. both
// the condition flags and the condition register are predicate.
//
// This test verify that QBDI can extract the condition of an instruction based
// on the followed properties:
// - If the instruction doesn't not support condition, the instruction doesn't
//    have predicate operand info
// - The condition flags is the first predicate (real type: immediate) but
//    marked as MCOI::Predicate with MCOI::OPERAND_UNKNOWN
// - The register flags is the second predicate (real type: reg 0 or reg CPSR)
// but
//    marked as MCOI::Predicate with MCOI::OPERAND_UNKNOWN. The second
//    predicate is just after the first one.

void LLVMOperandInfoCheck::checkPredicate(QBDI::CPUMode cpuMode) {
  const llvm::MCInstrInfo &MCII = getCPU(cpuMode).getMCII();

  for (unsigned opcode = 0; opcode < llvm::ARM::INSTRUCTION_LIST_END;
       opcode++) {

    const llvm::MCInstrDesc &desc = MCII.get(opcode);
    const char *mnemonic = MCII.getName(opcode).data();

    // the opcode is a pseudo instruction used by LLVM internally
    if (desc.isPseudo())
      continue;

    unsigned int predicateFirstOffset = 0;
    unsigned int predicateNumber = 0;
    unsigned int numCCRRegClassID = 0;
    bool consecutivePredicate = true;

    for (unsigned int opn = 0; opn < desc.getNumOperands(); opn++) {
      const llvm::MCOperandInfo *opInfo = desc.opInfo_begin() + opn;
      if (opInfo->isPredicate()) {
        CHECK(opInfo->OperandType == llvm::MCOI::OPERAND_UNKNOWN);
        if (predicateNumber == 0) {
          predicateFirstOffset = opn;
          predicateNumber++;
        } else {
          consecutivePredicate &=
              (opn == predicateFirstOffset + predicateNumber);
          predicateNumber++;
        }
      }
      if (opInfo->RegClass == llvm::ARM::CCRRegClassID) {
        CHECK(opInfo->isOptionalDef());
        numCCRRegClassID++;
      }
    }

    if (predicateNumber == 0) {
      continue;
    }

    if (predicateNumber != 2) {
      FAIL_CHECK("Instruction " << mnemonic << " has " << predicateNumber
                                << " predicate operands.");
      continue;
    }

    if (not consecutivePredicate) {
      FAIL_CHECK("Instruction "
                 << mnemonic
                 << " doesn't have consecutive predicate operands.");
    }

    if (numCCRRegClassID > 1) {
      FAIL_CHECK("Instruction " << mnemonic << " has " << numCCRRegClassID
                                << " llvm::ARM::CCRRegClassID operand.");
    }
  }
}

// LLVM MCInstrDesc specify some operand are TIED_TO another one. We want to
// verify the followed properties:
//
// - If the operation is variadic, the last operand isn't TIED_TO another one.
// - Two operands can't be TIED_TO to the same operand
// - An operand is TIED_TO an previous operand
//
// An instruction with TIED_TO operand must be in one of theses two cases:
// - The operand that is tied are the first in the list, and no futher operand.
//    -> In this case, the instAnalysis skip the tieded operand
// - Each tied operand is tied to the previous operand.
//    -> In this case, the instAnalysis merge the operand with the previous one.

void LLVMOperandInfoCheck::checkTIEDOperand(QBDI::CPUMode cpuMode) {

  const llvm::MCInstrInfo &MCII = getCPU(cpuMode).getMCII();

  for (unsigned opcode = 0; opcode < llvm::ARM::INSTRUCTION_LIST_END;
       opcode++) {

    const llvm::MCInstrDesc &desc = MCII.get(opcode);
    const char *mnemonic = MCII.getName(opcode).data();
    const int numOperand = desc.getNumOperands();

    // the opcode is a pseudo instruction used by LLVM internally
    if (desc.isPseudo())
      continue;

    if (desc.isVariadic()) {
      if (numOperand == 0) {
        FAIL_CHECK("Instruction "
                   << mnemonic << " is variadic but doesn't have any operand.");
      }
      if (desc.getOperandConstraint(numOperand - 1, llvm::MCOI::TIED_TO) !=
          -1) {
        FAIL_CHECK(
            "Instruction "
            << mnemonic
            << " is variadic but the last operand is tied to another one.");
      }
      for (int opn = 0; opn < numOperand; opn++) {
        if (desc.getOperandConstraint(opn, llvm::MCOI::TIED_TO) ==
            numOperand - 1) {
          FAIL_CHECK("Instruction " << mnemonic
                                    << " is variadic but the operand " << opn
                                    << "is tied to the variadic operand.");
        }
      }
    }

    bool tiedtoPreviousOperand = true;
    int numTied = 0;

    for (int opn = 0; opn < numOperand; opn++) {
      int opnTied = desc.getOperandConstraint(opn, llvm::MCOI::TIED_TO);
      if (opnTied != -1) {
        numTied++;
        if (opnTied >= opn) {
          FAIL_CHECK("Instruction " << mnemonic << " has the operand " << opn
                                    << " tied to the next operand " << opnTied
                                    << ".");
        }
        if (opnTied != opn - 1) {
          tiedtoPreviousOperand = false;
        }

        for (int opn2 = opn + 1; opn2 < numOperand; opn2++) {
          int opnTied2 = desc.getOperandConstraint(opn2, llvm::MCOI::TIED_TO);
          if (opnTied == opnTied2) {
            FAIL_CHECK("Instruction " << mnemonic << " has operands " << opn
                                      << " and " << opn2
                                      << " both tied to the same operand");
          }
        }
      }
    }

    int sequencTied = 0;
    for (int opn = 0; opn < numTied; opn++) {
      bool found = false;
      for (int op = opn + 1; op < numOperand; op++) {
        if (desc.getOperandConstraint(op, llvm::MCOI::TIED_TO) == opn) {
          sequencTied++;
          found = true;
          break;
        }
      }
      if (not found) {
        break;
      }
    }

    if ((sequencTied != numTied) and (not tiedtoPreviousOperand)) {
      FAIL_CHECK("Instruction "
                 << mnemonic
                 << " fail to verify Bias or Previous Operand property.");
    }
  }
}

TEST_CASE_METHOD(LLVMOperandInfoCheck, "LLVMOperandInfo-CrossCheck") {
  checkPredicate(QBDI::CPUMode::ARM);
  checkPredicate(QBDI::CPUMode::Thumb);
  checkTIEDOperand(QBDI::CPUMode::ARM);
  checkTIEDOperand(QBDI::CPUMode::Thumb);
}
