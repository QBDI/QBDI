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
#include <set>

#include <catch2/catch_test_macros.hpp>
#include <stdio.h>

#include "ARMInstrInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrDesc.h"

#include "Patch/InstInfo.h"
#include "Patch/MemoryAccessTable.h"

#define MAXFAIL 20

namespace {

using namespace llvm::ARM;

const std::set<unsigned> unsupportedInst{
    // clang-format off
    // PseudoInst
    LDRcp,
    VLD1LNq16Pseudo,
    VLD1LNq16Pseudo_UPD,
    VLD1LNq32Pseudo,
    VLD1LNq32Pseudo_UPD,
    VLD1LNq8Pseudo,
    VLD1LNq8Pseudo_UPD,
    VLD1d16QPseudo,
    VLD1d16QPseudoWB_fixed,
    VLD1d16QPseudoWB_register,
    VLD1d16TPseudo,
    VLD1d16TPseudoWB_fixed,
    VLD1d16TPseudoWB_register,
    VLD1d32QPseudo,
    VLD1d32QPseudoWB_fixed,
    VLD1d32QPseudoWB_register,
    VLD1d32TPseudo,
    VLD1d32TPseudoWB_fixed,
    VLD1d32TPseudoWB_register,
    VLD1d64QPseudo,
    VLD1d64QPseudoWB_fixed,
    VLD1d64QPseudoWB_register,
    VLD1d64TPseudo,
    VLD1d64TPseudoWB_fixed,
    VLD1d64TPseudoWB_register,
    VLD1d8QPseudo,
    VLD1d8QPseudoWB_fixed,
    VLD1d8QPseudoWB_register,
    VLD1d8TPseudo,
    VLD1d8TPseudoWB_fixed,
    VLD1d8TPseudoWB_register,
    VLD1q16HighQPseudo,
    VLD1q16HighQPseudo_UPD,
    VLD1q16HighTPseudo,
    VLD1q16HighTPseudo_UPD,
    VLD1q16LowQPseudo_UPD,
    VLD1q16LowTPseudo_UPD,
    VLD1q32HighQPseudo,
    VLD1q32HighQPseudo_UPD,
    VLD1q32HighTPseudo,
    VLD1q32HighTPseudo_UPD,
    VLD1q32LowQPseudo_UPD,
    VLD1q32LowTPseudo_UPD,
    VLD1q64HighQPseudo,
    VLD1q64HighQPseudo_UPD,
    VLD1q64HighTPseudo,
    VLD1q64HighTPseudo_UPD,
    VLD1q64LowQPseudo_UPD,
    VLD1q64LowTPseudo_UPD,
    VLD1q8HighQPseudo,
    VLD1q8HighQPseudo_UPD,
    VLD1q8HighTPseudo,
    VLD1q8HighTPseudo_UPD,
    VLD1q8LowQPseudo_UPD,
    VLD1q8LowTPseudo_UPD,
    VLD2DUPq16EvenPseudo,
    VLD2DUPq16OddPseudo,
    VLD2DUPq16OddPseudoWB_fixed,
    VLD2DUPq16OddPseudoWB_register,
    VLD2DUPq32EvenPseudo,
    VLD2DUPq32OddPseudo,
    VLD2DUPq32OddPseudoWB_fixed,
    VLD2DUPq32OddPseudoWB_register,
    VLD2DUPq8EvenPseudo,
    VLD2DUPq8OddPseudo,
    VLD2DUPq8OddPseudoWB_fixed,
    VLD2DUPq8OddPseudoWB_register,
    VLD2LNd16Pseudo,
    VLD2LNd16Pseudo_UPD,
    VLD2LNd32Pseudo,
    VLD2LNd32Pseudo_UPD,
    VLD2LNd8Pseudo,
    VLD2LNd8Pseudo_UPD,
    VLD2LNq16Pseudo,
    VLD2LNq16Pseudo_UPD,
    VLD2LNq32Pseudo,
    VLD2LNq32Pseudo_UPD,
    VLD2q16Pseudo,
    VLD2q16PseudoWB_fixed,
    VLD2q16PseudoWB_register,
    VLD2q32Pseudo,
    VLD2q32PseudoWB_fixed,
    VLD2q32PseudoWB_register,
    VLD2q8Pseudo,
    VLD2q8PseudoWB_fixed,
    VLD2q8PseudoWB_register,
    VLD3DUPd16Pseudo,
    VLD3DUPd16Pseudo_UPD,
    VLD3DUPd32Pseudo,
    VLD3DUPd32Pseudo_UPD,
    VLD3DUPd8Pseudo,
    VLD3DUPd8Pseudo_UPD,
    VLD3DUPq16EvenPseudo,
    VLD3DUPq16OddPseudo,
    VLD3DUPq16OddPseudo_UPD,
    VLD3DUPq32EvenPseudo,
    VLD3DUPq32OddPseudo,
    VLD3DUPq32OddPseudo_UPD,
    VLD3DUPq8EvenPseudo,
    VLD3DUPq8OddPseudo,
    VLD3DUPq8OddPseudo_UPD,
    VLD3LNd16Pseudo,
    VLD3LNd16Pseudo_UPD,
    VLD3LNd32Pseudo,
    VLD3LNd32Pseudo_UPD,
    VLD3LNd8Pseudo,
    VLD3LNd8Pseudo_UPD,
    VLD3LNq16Pseudo,
    VLD3LNq16Pseudo_UPD,
    VLD3LNq32Pseudo,
    VLD3LNq32Pseudo_UPD,
    VLD3d16Pseudo,
    VLD3d16Pseudo_UPD,
    VLD3d32Pseudo,
    VLD3d32Pseudo_UPD,
    VLD3d8Pseudo,
    VLD3d8Pseudo_UPD,
    VLD3q16Pseudo_UPD,
    VLD3q16oddPseudo,
    VLD3q16oddPseudo_UPD,
    VLD3q32Pseudo_UPD,
    VLD3q32oddPseudo,
    VLD3q32oddPseudo_UPD,
    VLD3q8Pseudo_UPD,
    VLD3q8oddPseudo,
    VLD3q8oddPseudo_UPD,
    VLD4DUPd16Pseudo,
    VLD4DUPd16Pseudo_UPD,
    VLD4DUPd32Pseudo,
    VLD4DUPd32Pseudo_UPD,
    VLD4DUPd8Pseudo,
    VLD4DUPd8Pseudo_UPD,
    VLD4DUPq16EvenPseudo,
    VLD4DUPq16OddPseudo,
    VLD4DUPq16OddPseudo_UPD,
    VLD4DUPq32EvenPseudo,
    VLD4DUPq32OddPseudo,
    VLD4DUPq32OddPseudo_UPD,
    VLD4DUPq8EvenPseudo,
    VLD4DUPq8OddPseudo,
    VLD4DUPq8OddPseudo_UPD,
    VLD4LNd16Pseudo,
    VLD4LNd16Pseudo_UPD,
    VLD4LNd32Pseudo,
    VLD4LNd32Pseudo_UPD,
    VLD4LNd8Pseudo,
    VLD4LNd8Pseudo_UPD,
    VLD4LNq16Pseudo,
    VLD4LNq16Pseudo_UPD,
    VLD4LNq32Pseudo,
    VLD4LNq32Pseudo_UPD,
    VLD4d16Pseudo,
    VLD4d16Pseudo_UPD,
    VLD4d32Pseudo,
    VLD4d32Pseudo_UPD,
    VLD4d8Pseudo,
    VLD4d8Pseudo_UPD,
    VLD4q16Pseudo_UPD,
    VLD4q16oddPseudo,
    VLD4q16oddPseudo_UPD,
    VLD4q32Pseudo_UPD,
    VLD4q32oddPseudo,
    VLD4q32oddPseudo_UPD,
    VLD4q8Pseudo_UPD,
    VLD4q8oddPseudo,
    VLD4q8oddPseudo_UPD,
    VLDMQIA,
    VST1LNq16Pseudo,
    VST1LNq16Pseudo_UPD,
    VST1LNq32Pseudo,
    VST1LNq32Pseudo_UPD,
    VST1LNq8Pseudo,
    VST1LNq8Pseudo_UPD,
    VST1d16QPseudo,
    VST1d16QPseudoWB_fixed,
    VST1d16QPseudoWB_register,
    VST1d16TPseudo,
    VST1d16TPseudoWB_fixed,
    VST1d16TPseudoWB_register,
    VST1d32QPseudo,
    VST1d32QPseudoWB_fixed,
    VST1d32QPseudoWB_register,
    VST1d32TPseudo,
    VST1d32TPseudoWB_fixed,
    VST1d32TPseudoWB_register,
    VST1d64QPseudo,
    VST1d64QPseudoWB_fixed,
    VST1d64QPseudoWB_register,
    VST1d64TPseudo,
    VST1d64TPseudoWB_fixed,
    VST1d64TPseudoWB_register,
    VST1d8QPseudo,
    VST1d8QPseudoWB_fixed,
    VST1d8QPseudoWB_register,
    VST1d8TPseudo,
    VST1d8TPseudoWB_fixed,
    VST1d8TPseudoWB_register,
    VST1q16HighQPseudo,
    VST1q16HighQPseudo_UPD,
    VST1q16HighTPseudo,
    VST1q16HighTPseudo_UPD,
    VST1q16LowQPseudo_UPD,
    VST1q16LowTPseudo_UPD,
    VST1q32HighQPseudo,
    VST1q32HighQPseudo_UPD,
    VST1q32HighTPseudo,
    VST1q32HighTPseudo_UPD,
    VST1q32LowQPseudo_UPD,
    VST1q32LowTPseudo_UPD,
    VST1q64HighQPseudo,
    VST1q64HighQPseudo_UPD,
    VST1q64HighTPseudo,
    VST1q64HighTPseudo_UPD,
    VST1q64LowQPseudo_UPD,
    VST1q64LowTPseudo_UPD,
    VST1q8HighQPseudo,
    VST1q8HighQPseudo_UPD,
    VST1q8HighTPseudo,
    VST1q8HighTPseudo_UPD,
    VST1q8LowQPseudo_UPD,
    VST1q8LowTPseudo_UPD,
    VST2LNd16Pseudo,
    VST2LNd16Pseudo_UPD,
    VST2LNd32Pseudo,
    VST2LNd32Pseudo_UPD,
    VST2LNd8Pseudo,
    VST2LNd8Pseudo_UPD,
    VST2LNq16Pseudo,
    VST2LNq16Pseudo_UPD,
    VST2LNq32Pseudo,
    VST2LNq32Pseudo_UPD,
    VST2q16Pseudo,
    VST2q16PseudoWB_fixed,
    VST2q16PseudoWB_register,
    VST2q32Pseudo,
    VST2q32PseudoWB_fixed,
    VST2q32PseudoWB_register,
    VST2q8Pseudo,
    VST2q8PseudoWB_fixed,
    VST2q8PseudoWB_register,
    VST3LNd16Pseudo,
    VST3LNd16Pseudo_UPD,
    VST3LNd32Pseudo,
    VST3LNd32Pseudo_UPD,
    VST3LNd8Pseudo,
    VST3LNd8Pseudo_UPD,
    VST3LNq16Pseudo,
    VST3LNq16Pseudo_UPD,
    VST3LNq32Pseudo,
    VST3LNq32Pseudo_UPD,
    VST3d16Pseudo,
    VST3d16Pseudo_UPD,
    VST3d32Pseudo,
    VST3d32Pseudo_UPD,
    VST3d8Pseudo,
    VST3d8Pseudo_UPD,
    VST3q16Pseudo_UPD,
    VST3q16oddPseudo,
    VST3q16oddPseudo_UPD,
    VST3q32Pseudo_UPD,
    VST3q32oddPseudo,
    VST3q32oddPseudo_UPD,
    VST3q8Pseudo_UPD,
    VST3q8oddPseudo,
    VST3q8oddPseudo_UPD,
    VST4LNd16Pseudo,
    VST4LNd16Pseudo_UPD,
    VST4LNd32Pseudo,
    VST4LNd32Pseudo_UPD,
    VST4LNd8Pseudo,
    VST4LNd8Pseudo_UPD,
    VST4LNq16Pseudo,
    VST4LNq16Pseudo_UPD,
    VST4LNq32Pseudo,
    VST4LNq32Pseudo_UPD,
    VST4d16Pseudo,
    VST4d16Pseudo_UPD,
    VST4d32Pseudo,
    VST4d32Pseudo_UPD,
    VST4d8Pseudo,
    VST4d8Pseudo_UPD,
    VST4q16Pseudo_UPD,
    VST4q16oddPseudo,
    VST4q16oddPseudo_UPD,
    VST4q32Pseudo_UPD,
    VST4q32oddPseudo,
    VST4q32oddPseudo_UPD,
    VST4q8Pseudo_UPD,
    VST4q8oddPseudo,
    VST4q8oddPseudo_UPD,
    VSTMQIA,

    // arm-M instruction
    VLLDM,
    VLLDM_T2,
    VLSTM,
    VLSTM_T2,

    // Unsupported instruction
    CDP,
    CDP2,
    t2CDP,
    t2CDP2,
    // system instruction
    MCR,
    MCR2,
    MCRR,
    MCRR2,
    MRC,
    MRC2,
    sysLDMDA,
    sysLDMDA_UPD,
    sysLDMDB,
    sysLDMDB_UPD,
    sysLDMIA,
    sysLDMIA_UPD,
    sysLDMIB,
    sysLDMIB_UPD,
    sysSTMDA,
    sysSTMDA_UPD,
    sysSTMDB,
    sysSTMDB_UPD,
    sysSTMIA,
    sysSTMIA_UPD,
    sysSTMIB,
    sysSTMIB_UPD,
    t2MCR,
    t2MCR2,
    t2MCRR,
    t2MCRR2,
    t2MRC,
    t2MRC2,
    // unknown instruction
    t__brkdiv0,
    // clang-format on
};

// instruction that reads memory but without mayLoad
const std::set<unsigned> fixupRead{
    // clang-format off
    t2LDRBT,
    t2LDRT,
    t2TBB,
    t2TBH,
    // clang-format on
};

// instruction that writes memory but without mayStore
const std::set<unsigned> fixupWrite{
    // clang-format off
    STRBT_POST_IMM,
    STRBT_POST_REG,
    STRH_PRE,
    t2STRBT,
    t2STRT,
    // clang-format on
};

// instruction with mayLoad but don't reads memory
const std::set<unsigned> fixupNoRead{
    // clang-format off
    CLREX,
    DBG,
    DMB,
    DSB,
    HINT,
    ISB,
    PLDWi12,
    PLDWrs,
    PLDi12,
    PLDrs,
    PLIi12,
    PLIrs,
    SADD16,
    SADD8,
    SASX,
    SEL,
    SSAX,
    SSUB16,
    SSUB8,
    STC2L_OFFSET,
    STC2_OFFSET,
    STCL_OFFSET,
    STC_OFFSET,
    STLEX,
    STLEXB,
    STLEXH,
    STREX,
    STREXB,
    STREXH,
    UADD16,
    UADD8,
    UASX,
    UDF,
    USAX,
    USUB16,
    USUB8,
    VMRS,
    VMSR,
    t2CLREX,
    t2DBG,
    t2DMB,
    t2DSB,
    t2HINT,
    t2ISB,
    t2PLDWi12,
    t2PLDWi8,
    t2PLDWs,
    t2PLDi12,
    t2PLDi8,
    t2PLDpci,
    t2PLDs,
    t2PLIi12,
    t2PLIi8,
    t2PLIpci,
    t2PLIs,
    t2SADD16,
    t2SADD8,
    t2SASX,
    t2SEL,
    t2SSAX,
    t2SSUB16,
    t2SSUB8,
    t2STC2L_OFFSET,
    t2STC2_OFFSET,
    t2STCL_OFFSET,
    t2STC_OFFSET,
    t2STLEX,
    t2STLEXB,
    t2STLEXH,
    t2STREX,
    t2STREXB,
    t2STREXH,
    t2UADD16,
    t2UADD8,
    t2UASX,
    t2UDF,
    t2USAX,
    t2USUB16,
    t2USUB8,
    tHINT,
    tUDF,
    // clang-format on
};

// instruction with mayStore but don't writes memory
const std::set<unsigned> fixupNoWrite{
    // clang-format off
    CLREX,
    DBG,
    DMB,
    DSB,
    HINT,
    ISB,
    LDAEX,
    LDAEXB,
    LDAEXH,
    LDC2L_OFFSET,
    LDC2_OFFSET,
    LDCL_OFFSET,
    LDC_OFFSET,
    LDREX,
    LDREXB,
    LDREXH,
    PLDWi12,
    PLDWrs,
    PLDi12,
    PLDrs,
    PLIi12,
    PLIrs,
    SADD16,
    SADD8,
    SASX,
    SSAX,
    SSUB16,
    SSUB8,
    UADD16,
    UADD8,
    UASX,
    UDF,
    USAX,
    USUB16,
    USUB8,
    VMRS,
    VMSR,
    t2CLREX,
    t2DBG,
    t2DMB,
    t2DSB,
    t2HINT,
    t2ISB,
    t2LDAEX,
    t2LDAEXB,
    t2LDAEXH,
    t2LDC2L_OFFSET,
    t2LDC2_OFFSET,
    t2LDCL_OFFSET,
    t2LDC_OFFSET,
    t2LDREX,
    t2LDREXB,
    t2LDREXH,
    t2PLDWi12,
    t2PLDWi8,
    t2PLDWs,
    t2PLDi12,
    t2PLDi8,
    t2PLDpci,
    t2PLDs,
    t2PLIi12,
    t2PLIi8,
    t2PLIpci,
    t2PLIs,
    t2SADD16,
    t2SADD8,
    t2SASX,
    t2SSAX,
    t2SSUB16,
    t2SSUB8,
    t2UADD16,
    t2UADD8,
    t2UASX,
    t2UDF,
    t2USAX,
    t2USUB16,
    t2USUB8,
    tHINT,
    tUDF,
    // clang-format on
};

} // namespace

TEST_CASE_METHOD(MemoryAccessTable, "MemoryAccessTable-CrossCheck") {

  const QBDI::LLVMCPU &llvmcpu = getCPU(QBDI::CPUMode::DEFAULT);
  const llvm::MCInstrInfo &MCII = llvmcpu.getMCII();
  int nbfail = 0;

  for (unsigned opcode = 0; opcode < llvm::ARM::INSTRUCTION_LIST_END;
       opcode++) {
    if (unsupportedInst.count(opcode) == 1)
      continue;

    const llvm::MCInstrDesc &desc = MCII.get(opcode);
    const char *mnemonic = MCII.getName(opcode).data();

    // InstInfo_ARM.cpp only use inst->getOpcode(). The MCInst doesn't need
    // to have his operand
    llvm::MCInst inst;
    inst.setOpcode(opcode);

    bool doRead =
        (QBDI::getReadSize(inst, llvmcpu) != 0) || QBDI::unsupportedRead(inst);
    bool doWrite = (QBDI::getWriteSize(inst, llvmcpu) != 0) ||
                   QBDI::unsupportedWrite(inst);
    bool mayRead = desc.mayLoad();
    bool mayWrite = desc.mayStore();

    bool bypassRead = false;
    bool bypassWrite = false;

    // the opcode is a pseudo instruction used by LLVM internally
    if (desc.isPseudo()) {
      if (doRead or doWrite) {
        WARN("Pseudo instruction " << mnemonic << " in InstInfo");
      }
      continue;
    }

    // llvm mayLoad and mayStore fixup
    if (fixupRead.count(opcode) == 1) {
      if (doRead && !mayRead)
        bypassRead = true;
      else
        WARN("Unneeded instruction " << mnemonic << " in fixupRead");
    }

    if (fixupNoRead.count(opcode) == 1) {
      if (!doRead && mayRead)
        bypassRead = true;
      else
        WARN("Unneeded instruction " << mnemonic << " in fixupNoRead");
    }

    if (fixupWrite.count(opcode) == 1) {
      if (doWrite && !mayWrite)
        bypassWrite = true;
      else
        WARN("Unneeded instruction " << mnemonic << " in fixupWrite");
    }

    if (fixupNoWrite.count(opcode) == 1) {
      if (!doWrite && mayWrite)
        bypassWrite = true;
      else
        WARN("Unneeded instruction " << mnemonic << " in fixupNoWrite");
    }

    if (!bypassRead && doRead != mayRead) {
      if (doRead && !mayRead) {
        FAIL_CHECK("Unexpected read for " << mnemonic);
        nbfail++;
      } else if (!doRead && mayRead) {
        FAIL_CHECK("Missing read for " << mnemonic);
        nbfail++;
      }
    }

    if (!bypassWrite && doWrite != mayWrite) {
      if (doWrite && !mayWrite) {
        FAIL_CHECK("Unexpected write for " << mnemonic);
        nbfail++;
      } else if (!doWrite && mayWrite) {
        FAIL_CHECK("Missing write for " << mnemonic);
        nbfail++;
      }
    }
    if (nbfail >= MAXFAIL) {
      FAIL("Too many fails, abort MemoryAccessTable validation");
    }
  }
}
