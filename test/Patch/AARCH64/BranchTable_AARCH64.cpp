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
#include <catch2/catch_test_macros.hpp>
#include <set>
#include <stdio.h>

#include "AArch64InstrInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrDesc.h"

#include "Patch/MemoryAccessTable.h"

#define MAXFAIL 20

namespace {

using namespace llvm::AArch64;

const std::set<unsigned> knownControlFlowInst{
    // clang-format off
    RET, BR,
    BLR,
    BL,
    B,
    Bcc, BCcc,
    TBNZX, TBNZW, TBZX, TBZW,
    CBNZX, CBNZW, CBZX, CBZW,
    BRAA, BRAB, BRAAZ, BRABZ, RETAA, RETAB,
    BLRAA, BLRAB, BLRAAZ, BLRABZ,
    // FEAT_PAuth_LR authenticated returns
    RETAASPPCi, RETABSPPCi, RETAASPPCr, RETABSPPCr,
    // Armv9.4 CMPBR (FEAT_CMPBR) fused compare-and-branch
    CBGTWrr, CBGTXrr, CBGEWrr, CBGEXrr, CBHIWrr, CBHIXrr, CBHSWrr, CBHSXrr,
    CBEQWrr, CBEQXrr, CBNEWrr, CBNEXrr,
    CBGTWri, CBGTXri, CBLTWri, CBLTXri, CBHIWri, CBHIXri, CBLOWri, CBLOXri,
    CBEQWri, CBEQXri, CBNEWri, CBNEXri,
    CBHGTWrr, CBHGEWrr, CBHHIWrr, CBHHSWrr, CBHEQWrr, CBHNEWrr,
    CBBGTWrr, CBBGEWrr, CBBHIWrr, CBBHSWrr, CBBEQWrr, CBBNEWrr,
    // clang-format on
};

const std::set<unsigned> unsupportedOpcode{
    // clang-format off
    // privileged instruction, never reachable in userland (QBDI is a
    // userland-only DBI)
    ERET, ERETAA, ERETAB, DRPS,
    // clang-format on
};

} // namespace

TEST_CASE_METHOD(MemoryAccessTable, "BranchTable-CrossCheck") {

  const QBDI::LLVMCPU &llvmcpu = getCPU(QBDI::CPUMode::DEFAULT);
  const llvm::MCInstrInfo &MCII = llvmcpu.getMCII();
  int nbfail = 0;

  for (unsigned opcode = 0; opcode < llvm::AArch64::INSTRUCTION_LIST_END;
       opcode++) {

    const llvm::MCInstrDesc &desc = MCII.get(opcode);
    const char *mnemonic = MCII.getName(opcode).data();

    if (desc.isPseudo()) {
      continue;
    }

    bool isControlFlow = desc.isBranch() || desc.isCall() || desc.isReturn();
    bool isKnown = knownControlFlowInst.count(opcode) == 1;

    if (unsupportedOpcode.count(opcode) == 1) {
      if (!isControlFlow || isKnown) {
        WARN("Unneeded instruction " << mnemonic << " in unsupportedOpcode");
      }
      continue;
    }

    if (isControlFlow && !isKnown) {
      FAIL_CHECK("Opcode " << mnemonic
                           << " is flagged isBranch/isCall/isReturn by LLVM "
                              "but is not handled by any "
                              "PatchRuleAssembly_AARCH64.cpp rule");
      nbfail++;
    } else if (!isControlFlow && isKnown) {
      FAIL_CHECK("Opcode " << mnemonic
                           << " is listed in knownControlFlowInst but LLVM "
                              "no longer flags it as isBranch/isCall/"
                              "isReturn");
      nbfail++;
    }

    if (nbfail >= MAXFAIL) {
      FAIL("Too many fails, abort BranchTable validation");
    }
  }
}
