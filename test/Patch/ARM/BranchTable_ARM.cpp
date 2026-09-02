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

#include "ARMInstrInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrDesc.h"

#include "Patch/MemoryAccessTable.h"

#define MAXFAIL 20

namespace {

using namespace llvm::ARM;

const std::set<unsigned> knownControlFlowInst{
    // clang-format off
    BX_RET, MOVPCLR,
    BX, BX_pred, BXJ,
    BLX, BLX_pred, BLXi,
    BL, BL_pred,
    Bcc,
    tBcc, t2Bcc, tB, t2B,
    tBL, tBLXi, tBLXr, tBX, t2BXAUT, t2BXJ,
    tCBNZ, tCBZ,
    t2TBB, t2TBH,
    // SVC/tSVC's control transfer (the syscall trap and its return) is
    // entirely managed by the kernel/exception mechanism, not by any
    // PatchRule-computed PC value; their PatchRule only clears the local
    // exclusive-monitor state around the syscall
    SVC, tSVC,
    // clang-format on
};

const std::set<unsigned> unsupportedOpcode{
    // clang-format off
    // M-profile only (mve / lob armv8.1m.main / 8msecext), unreachable on
    // QBDI's A-profile target
    MVE_LETP, MVE_WLSTP_8, MVE_WLSTP_16, MVE_WLSTP_32, MVE_WLSTP_64,
    t2LE, t2LEUpdate, t2WLS,
    tBLXNSr, tBXNS,
    // privileged instruction, never reachable in userland (QBDI is a
    // userland-only DBI)
    t2RFEIA, t2RFEIAW, t2RFEDB, t2RFEDBW,
    ERET, HVC, t2HVC, t2SMC,
    t2SUBS_PC_LR,
    // clang-format on
};

} // namespace

TEST_CASE_METHOD(MemoryAccessTable, "BranchTable-CrossCheck") {

  const QBDI::LLVMCPU &llvmcpu = getCPU(QBDI::CPUMode::DEFAULT);
  const llvm::MCInstrInfo &MCII = llvmcpu.getMCII();
  int nbfail = 0;

  for (unsigned opcode = 0; opcode < llvm::ARM::INSTRUCTION_LIST_END;
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
                              "PatchRuleAssembly_ARM.cpp rule");
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
