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

#include "X86InstrInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrDesc.h"

#include "Patch/MemoryAccessTable.h"

#define MAXFAIL 20

namespace {

using namespace llvm::X86;

const std::set<unsigned> knownControlFlowInst{
    // clang-format off
    JMP64m, CALL64m, JMP32m, JMP16m, CALL32m, CALL16m,
    JMP_1, JMP_2, JMP_4,
    JMP32r, JMP64r, JMP16r,
    CALL32r, CALL64r, CALL16r,
    JCC_1, JCC_2, JCC_4,
    LOOP, LOOPE, LOOPNE, JRCXZ, JECXZ, JCXZ,
    CALL64pcrel32, CALLpcrel16, CALLpcrel32,
    RET32, RET64, RET16, RETI32, RETI64, RETI16,
    LRET32, LRET64, LRET16, LRETI32, LRETI64, LRETI16,
    // clang-format on
};

const std::set<unsigned> unsupportedOpcode{
    // clang-format off
    // CET no-track / REX-tail-call-lowering opcode variants and phantom
    // size-less RET/IRET enum entries, not supported by QBDI (see
    // PatchRuleAssembly_X86_64.cpp's isSupportedInstruction)
    CALL16m_NT, CALL16r_NT, CALL32m_NT, CALL32r_NT, CALL64m_NT, CALL64r_NT,
    JMP16m_NT, JMP16r_NT, JMP32m_NT, JMP32r_NT, JMP64m_NT, JMP64r_NT,
    JMP64m_REX, JMP64r_REX,
    RET, IRET,
    // SelectionDAG-only pseudo instructions, lowered away before MC emission
    CATCHRET, CLEANUPRET, EH_RETURN, EH_RETURN64, EH_SjLj_Setup,
    TAILJMPd64_CC, TAILJMPd64, TAILJMPd_CC, TAILJMPd,
    TAILJMPm64, TAILJMPm64_REX, TAILJMPm,
    TAILJMPr64, TAILJMPr64_REX, TAILJMPr,
    TCRETURNdi64cc, TCRETURNdi64, TCRETURNdicc, TCRETURNdi,
    TCRETURN_HIPE32ri, TCRETURNmi64, TCRETURNmi,
    TCRETURNri64_ImpCall, TCRETURNri64, TCRETURNri,
    TCRETURN_WIN64ri, TCRETURN_WINmi64,
    // 16-bit real mode target, not supported by QBDI (see
    // PatchRuleAssembly_X86_64.cpp's isSupportedInstruction)
    FARCALL16i, FARJMP16i,
    // Intel TSX, not supported by QBDI (see PatchRuleAssembly_X86_64.cpp's
    // isSupportedInstruction)
    XBEGIN_2, XBEGIN_4,
    // far call/jump (segment:offset target), not supported by QBDI (see
    // PatchRuleAssembly_X86_64.cpp's isSupportedInstruction)
    FARCALL16m, FARCALL32m, FARCALL64m, FARJMP16m, FARJMP32m, FARJMP64m,
    FARCALL32i, FARJMP32i,
    // IRET pops CS/EIP/EFLAGS from the stack, not supported by QBDI (see
    // PatchRuleAssembly_X86_64.cpp's isSupportedInstruction)
    IRET16, IRET32, IRET64,
    // clang-format on
};

} // namespace

TEST_CASE_METHOD(MemoryAccessTable, "BranchTable-CrossCheck") {

  const QBDI::LLVMCPU &llvmcpu = getCPU(QBDI::CPUMode::DEFAULT);
  const llvm::MCInstrInfo &MCII = llvmcpu.getMCII();
  int nbfail = 0;

  for (unsigned opcode = 0; opcode < llvm::X86::INSTRUCTION_LIST_END;
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
                              "PatchRuleAssembly_X86_64.cpp rule");
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
