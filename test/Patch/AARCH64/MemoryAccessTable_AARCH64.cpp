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

#include "AArch64InstrInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrDesc.h"

#include "Patch/InstInfo.h"
#include "Patch/MemoryAccessTable.h"

#define MAXFAIL 20

namespace {

using namespace llvm::AArch64;

const std::set<unsigned> unsupportedInst{
    // PseudoInst
    CMP_SWAP_128,
    CMP_SWAP_16,
    CMP_SWAP_32,
    CMP_SWAP_64,
    CMP_SWAP_8,
    HWASAN_CHECK_MEMACCESS,
    HWASAN_CHECK_MEMACCESS_SHORTGRANULES,
    SPACE,
    STGloop,
    STZGloop,
    // ARMv8 SVE
    LD1_MXIPXX_H_B,
    LD1_MXIPXX_H_D,
    LD1_MXIPXX_H_H,
    LD1_MXIPXX_H_Q,
    LD1_MXIPXX_H_S,
    LD1_MXIPXX_V_B,
    LD1_MXIPXX_V_D,
    LD1_MXIPXX_V_H,
    LD1_MXIPXX_V_Q,
    LD1_MXIPXX_V_S,
    LDR_ZA,
    ST1_MXIPXX_H_B,
    ST1_MXIPXX_H_D,
    ST1_MXIPXX_H_H,
    ST1_MXIPXX_H_Q,
    ST1_MXIPXX_H_S,
    ST1_MXIPXX_V_B,
    ST1_MXIPXX_V_D,
    ST1_MXIPXX_V_H,
    ST1_MXIPXX_V_Q,
    ST1_MXIPXX_V_S,
    STR_ZA,
    // ARMv8 SVE
    GLD1B_D_IMM_REAL,
    GLD1B_D_REAL,
    GLD1B_D_SXTW_REAL,
    GLD1B_D_UXTW_REAL,
    GLD1B_S_IMM_REAL,
    GLD1B_S_SXTW_REAL,
    GLD1B_S_UXTW_REAL,
    GLD1D_IMM_REAL,
    GLD1D_REAL,
    GLD1D_SCALED_REAL,
    GLD1D_SXTW_REAL,
    GLD1D_SXTW_SCALED_REAL,
    GLD1D_UXTW_REAL,
    GLD1D_UXTW_SCALED_REAL,
    GLD1H_D_IMM_REAL,
    GLD1H_D_REAL,
    GLD1H_D_SCALED_REAL,
    GLD1H_D_SXTW_REAL,
    GLD1H_D_SXTW_SCALED_REAL,
    GLD1H_D_UXTW_REAL,
    GLD1H_D_UXTW_SCALED_REAL,
    GLD1H_S_IMM_REAL,
    GLD1H_S_SXTW_REAL,
    GLD1H_S_SXTW_SCALED_REAL,
    GLD1H_S_UXTW_REAL,
    GLD1H_S_UXTW_SCALED_REAL,
    GLD1SB_D_IMM_REAL,
    GLD1SB_D_REAL,
    GLD1SB_D_SXTW_REAL,
    GLD1SB_D_UXTW_REAL,
    GLD1SB_S_IMM_REAL,
    GLD1SB_S_SXTW_REAL,
    GLD1SB_S_UXTW_REAL,
    GLD1SH_D_IMM_REAL,
    GLD1SH_D_REAL,
    GLD1SH_D_SCALED_REAL,
    GLD1SH_D_SXTW_REAL,
    GLD1SH_D_SXTW_SCALED_REAL,
    GLD1SH_D_UXTW_REAL,
    GLD1SH_D_UXTW_SCALED_REAL,
    GLD1SH_S_IMM_REAL,
    GLD1SH_S_SXTW_REAL,
    GLD1SH_S_SXTW_SCALED_REAL,
    GLD1SH_S_UXTW_REAL,
    GLD1SH_S_UXTW_SCALED_REAL,
    GLD1SW_D_IMM_REAL,
    GLD1SW_D_REAL,
    GLD1SW_D_SCALED_REAL,
    GLD1SW_D_SXTW_REAL,
    GLD1SW_D_SXTW_SCALED_REAL,
    GLD1SW_D_UXTW_REAL,
    GLD1SW_D_UXTW_SCALED_REAL,
    GLD1W_D_IMM_REAL,
    GLD1W_D_REAL,
    GLD1W_D_SCALED_REAL,
    GLD1W_D_SXTW_REAL,
    GLD1W_D_SXTW_SCALED_REAL,
    GLD1W_D_UXTW_REAL,
    GLD1W_D_UXTW_SCALED_REAL,
    GLD1W_IMM_REAL,
    GLD1W_SXTW_REAL,
    GLD1W_SXTW_SCALED_REAL,
    GLD1W_UXTW_REAL,
    GLD1W_UXTW_SCALED_REAL,
    GLDFF1B_D_IMM_REAL,
    GLDFF1B_D_REAL,
    GLDFF1B_D_SXTW_REAL,
    GLDFF1B_D_UXTW_REAL,
    GLDFF1B_S_IMM_REAL,
    GLDFF1B_S_SXTW_REAL,
    GLDFF1B_S_UXTW_REAL,
    GLDFF1D_IMM_REAL,
    GLDFF1D_REAL,
    GLDFF1D_SCALED_REAL,
    GLDFF1D_SXTW_REAL,
    GLDFF1D_SXTW_SCALED_REAL,
    GLDFF1D_UXTW_REAL,
    GLDFF1D_UXTW_SCALED_REAL,
    GLDFF1H_D_IMM_REAL,
    GLDFF1H_D_REAL,
    GLDFF1H_D_SCALED_REAL,
    GLDFF1H_D_SXTW_REAL,
    GLDFF1H_D_SXTW_SCALED_REAL,
    GLDFF1H_D_UXTW_REAL,
    GLDFF1H_D_UXTW_SCALED_REAL,
    GLDFF1H_S_IMM_REAL,
    GLDFF1H_S_SXTW_REAL,
    GLDFF1H_S_SXTW_SCALED_REAL,
    GLDFF1H_S_UXTW_REAL,
    GLDFF1H_S_UXTW_SCALED_REAL,
    GLDFF1SB_D_IMM_REAL,
    GLDFF1SB_D_REAL,
    GLDFF1SB_D_SXTW_REAL,
    GLDFF1SB_D_UXTW_REAL,
    GLDFF1SB_S_IMM_REAL,
    GLDFF1SB_S_SXTW_REAL,
    GLDFF1SB_S_UXTW_REAL,
    GLDFF1SH_D_IMM_REAL,
    GLDFF1SH_D_REAL,
    GLDFF1SH_D_SCALED_REAL,
    GLDFF1SH_D_SXTW_REAL,
    GLDFF1SH_D_SXTW_SCALED_REAL,
    GLDFF1SH_D_UXTW_REAL,
    GLDFF1SH_D_UXTW_SCALED_REAL,
    GLDFF1SH_S_IMM_REAL,
    GLDFF1SH_S_SXTW_REAL,
    GLDFF1SH_S_SXTW_SCALED_REAL,
    GLDFF1SH_S_UXTW_REAL,
    GLDFF1SH_S_UXTW_SCALED_REAL,
    GLDFF1SW_D_IMM_REAL,
    GLDFF1SW_D_REAL,
    GLDFF1SW_D_SCALED_REAL,
    GLDFF1SW_D_SXTW_REAL,
    GLDFF1SW_D_SXTW_SCALED_REAL,
    GLDFF1SW_D_UXTW_REAL,
    GLDFF1SW_D_UXTW_SCALED_REAL,
    GLDFF1W_D_IMM_REAL,
    GLDFF1W_D_REAL,
    GLDFF1W_D_SCALED_REAL,
    GLDFF1W_D_SXTW_REAL,
    GLDFF1W_D_SXTW_SCALED_REAL,
    GLDFF1W_D_UXTW_REAL,
    GLDFF1W_D_UXTW_SCALED_REAL,
    GLDFF1W_IMM_REAL,
    GLDFF1W_SXTW_REAL,
    GLDFF1W_SXTW_SCALED_REAL,
    GLDFF1W_UXTW_REAL,
    GLDFF1W_UXTW_SCALED_REAL,
    LD1B,
    LD1B_D,
    LD1B_D_IMM,
    LD1B_D_IMM_REAL,
    LD1B_H,
    LD1B_H_IMM,
    LD1B_H_IMM_REAL,
    LD1B_IMM,
    LD1B_IMM_REAL,
    LD1B_S,
    LD1B_S_IMM,
    LD1B_S_IMM_REAL,
    LD1D,
    LD1D_IMM,
    LD1D_IMM_REAL,
    LD1H,
    LD1H,
    LD1H_D,
    LD1H_D,
    LD1H_D_IMM,
    LD1H_D_IMM,
    LD1H_D_IMM_REAL,
    LD1H_IMM,
    LD1H_IMM,
    LD1H_IMM_REAL,
    LD1H_S,
    LD1H_S,
    LD1H_S_IMM,
    LD1H_S_IMM,
    LD1H_S_IMM_REAL,
    LD1RB_D_IMM,
    LD1RB_H_IMM,
    LD1RB_IMM,
    LD1RB_S_IMM,
    LD1RD_IMM,
    LD1RH_D_IMM,
    LD1RH_IMM,
    LD1RH_IMM,
    LD1RH_S_IMM,
    LD1RH_S_IMM,
    LD1RO_B,
    LD1RO_B_IMM,
    LD1RO_D,
    LD1RO_D_IMM,
    LD1RO_H,
    LD1RO_H_IMM,
    LD1RO_W,
    LD1RO_W_IMM,
    LD1RQ_B,
    LD1RQ_B_IMM,
    LD1RQ_D,
    LD1RQ_D_IMM,
    LD1RQ_H,
    LD1RQ_H_IMM,
    LD1RQ_W,
    LD1RQ_W_IMM,
    LD1RSB_D_IMM,
    LD1RSB_H_IMM,
    LD1RSB_S_IMM,
    LD1RSH_D_IMM,
    LD1RSH_S_IMM,
    LD1RSW_IMM,
    LD1RW_D_IMM,
    LD1RW_IMM,
    LD1SB_D,
    LD1SB_D_IMM,
    LD1SB_D_IMM_REAL,
    LD1SB_H,
    LD1SB_H_IMM,
    LD1SB_H_IMM_REAL,
    LD1SB_S,
    LD1SB_S_IMM,
    LD1SB_S_IMM_REAL,
    LD1SH_D,
    LD1SH_D_IMM,
    LD1SH_D_IMM_REAL,
    LD1SH_S,
    LD1SH_S_IMM,
    LD1SH_S_IMM_REAL,
    LD1SW_D,
    LD1SW_D_IMM,
    LD1SW_D_IMM_REAL,
    LD1W,
    LD1W_D,
    LD1W_D_IMM,
    LD1W_D_IMM_REAL,
    LD1W_IMM,
    LD1W_IMM_REAL,
    LD2B,
    LD2B_IMM,
    LD2D,
    LD2D_IMM,
    LD2H,
    LD2H_IMM,
    LD2W,
    LD2W_IMM,
    LD3B,
    LD3B_IMM,
    LD3D,
    LD3D_IMM,
    LD3H,
    LD3H_IMM,
    LD3W,
    LD3W_IMM,
    LD4B,
    LD4B_IMM,
    LD4D,
    LD4D_IMM,
    LD4H,
    LD4H_IMM,
    LD4W,
    LD4W_IMM,
    LDFF1B_D_REAL,
    LDFF1B_H_REAL,
    LDFF1B_REAL,
    LDFF1B_S_REAL,
    LDFF1D_REAL,
    LDFF1H_D_REAL,
    LDFF1H_REAL,
    LDFF1H_S_REAL,
    LDFF1SB_D_REAL,
    LDFF1SB_H_REAL,
    LDFF1SB_S_REAL,
    LDFF1SH_D_REAL,
    LDFF1SH_S_REAL,
    LDFF1SW_D_REAL,
    LDFF1W_D_REAL,
    LDFF1W_REAL,
    LDNF1B_D_IMM,
    LDNF1B_D_IMM_REAL,
    LDNF1B_H_IMM,
    LDNF1B_H_IMM_REAL,
    LDNF1B_IMM,
    LDNF1B_IMM_REAL,
    LDNF1B_S_IMM,
    LDNF1B_S_IMM_REAL,
    LDNF1D_IMM,
    LDNF1D_IMM_REAL,
    LDNF1H_D_IMM,
    LDNF1H_D_IMM_REAL,
    LDNF1H_IMM,
    LDNF1H_IMM_REAL,
    LDNF1H_S_IMM,
    LDNF1H_S_IMM_REAL,
    LDNF1SB_D_IMM,
    LDNF1SB_D_IMM_REAL,
    LDNF1SB_H_IMM,
    LDNF1SB_H_IMM_REAL,
    LDNF1SB_S_IMM,
    LDNF1SB_S_IMM_REAL,
    LDNF1SH_D_IMM,
    LDNF1SH_D_IMM_REAL,
    LDNF1SH_S_IMM,
    LDNF1SH_S_IMM_REAL,
    LDNF1SW_D_IMM,
    LDNF1SW_D_IMM_REAL,
    LDNF1W_D_IMM,
    LDNF1W_D_IMM_REAL,
    LDNF1W_IMM,
    LDNF1W_IMM_REAL,
    LDNT1B_ZRI,
    LDNT1B_ZRR,
    LDNT1B_ZZR_D_REAL,
    LDNT1B_ZZR_S_REAL,
    LDNT1D_ZRI,
    LDNT1D_ZRR,
    LDNT1D_ZZR_D_REAL,
    LDNT1H_ZRI,
    LDNT1H_ZRR,
    LDNT1H_ZZR_D_REAL,
    LDNT1H_ZZR_S_REAL,
    LDNT1SB_ZZR_D_REAL,
    LDNT1SB_ZZR_S_REAL,
    LDNT1SH_ZZR_D_REAL,
    LDNT1SH_ZZR_S_REAL,
    LDNT1SW_ZZR_D_REAL,
    LDNT1W_ZRI,
    LDNT1W_ZRR,
    LDNT1W_ZZR_D_REAL,
    LDNT1W_ZZR_S_REAL,
    LDR_PXI,
    LDR_ZXI,
    PRFB_D_PZI,
    PRFB_D_SCALED,
    PRFB_D_SXTW_SCALED,
    PRFB_D_UXTW_SCALED,
    PRFB_PRI,
    PRFB_PRR,
    PRFB_S_PZI,
    PRFB_S_SXTW_SCALED,
    PRFB_S_UXTW_SCALED,
    PRFD_D_PZI,
    PRFD_D_SCALED,
    PRFD_D_SXTW_SCALED,
    PRFD_D_UXTW_SCALED,
    PRFD_PRI,
    PRFD_PRR,
    PRFD_S_PZI,
    PRFD_S_SXTW_SCALED,
    PRFD_S_UXTW_SCALED,
    PRFH_D_PZI,
    PRFH_D_SCALED,
    PRFH_D_SXTW_SCALED,
    PRFH_D_UXTW_SCALED,
    PRFH_PRI,
    PRFH_PRR,
    PRFH_S_PZI,
    PRFH_S_SXTW_SCALED,
    PRFH_S_UXTW_SCALED,
    PRFS_PRR,
    PRFW_D_PZI,
    PRFW_D_SCALED,
    PRFW_D_SXTW_SCALED,
    PRFW_D_UXTW_SCALED,
    PRFW_PRI,
    PRFW_S_PZI,
    PRFW_S_SXTW_SCALED,
    PRFW_S_UXTW_SCALED,
    SETFFR,
    SST1B_D_IMM,
    SST1B_D_REAL,
    SST1B_D_SXTW,
    SST1B_D_UXTW,
    SST1B_S_IMM,
    SST1B_S_SXTW,
    SST1B_S_UXTW,
    SST1D_IMM,
    SST1D_REAL,
    SST1D_SCALED_SCALED_REAL,
    SST1D_SXTW,
    SST1D_SXTW_SCALED,
    SST1D_UXTW,
    SST1D_UXTW_SCALED,
    SST1H_D_IMM,
    SST1H_D_REAL,
    SST1H_D_SCALED_SCALED_REAL,
    SST1H_D_SXTW,
    SST1H_D_SXTW_SCALED,
    SST1H_D_UXTW,
    SST1H_D_UXTW_SCALED,
    SST1H_S_IMM,
    SST1H_S_SXTW,
    SST1H_S_SXTW_SCALED,
    SST1H_S_UXTW,
    SST1H_S_UXTW_SCALED,
    SST1W_D_IMM,
    SST1W_D_REAL,
    SST1W_D_SCALED_SCALED_REAL,
    SST1W_D_SXTW,
    SST1W_D_SXTW_SCALED,
    SST1W_D_UXTW,
    SST1W_D_UXTW_SCALED,
    SST1W_IMM,
    SST1W_SXTW,
    SST1W_SXTW_SCALED,
    SST1W_UXTW,
    SST1W_UXTW_SCALED,
    ST1B,
    ST1B_D,
    ST1B_D_IMM,
    ST1B_H,
    ST1B_H_IMM,
    ST1B_IMM,
    ST1B_S,
    ST1B_S_IMM,
    ST1D,
    ST1D_IMM,
    ST1H,
    ST1H_D,
    ST1H_D_IMM,
    ST1H_IMM,
    ST1H_S,
    ST1H_S_IMM,
    ST1W,
    ST1W_D,
    ST1W_D_IMM,
    ST1W_IMM,
    ST2B,
    ST2B_IMM,
    ST2D,
    ST2D_IMM,
    ST2H,
    ST2H_IMM,
    ST2W,
    ST2W_IMM,
    ST3B,
    ST3B_IMM,
    ST3D,
    ST3D_IMM,
    ST3H,
    ST3H_IMM,
    ST3W,
    ST3W_IMM,
    ST4B,
    ST4B_IMM,
    ST4D,
    ST4D_IMM,
    ST4H,
    ST4H_IMM,
    ST4W,
    ST4W_IMM,
    STNT1B_ZRI,
    STNT1B_ZRR,
    STNT1B_ZZR_D_REAL,
    STNT1B_ZZR_S_REAL,
    STNT1D_ZRI,
    STNT1D_ZRR,
    STNT1D_ZZR_D_REAL,
    STNT1H_ZRI,
    STNT1H_ZRR,
    STNT1H_ZZR_D_REAL,
    STNT1H_ZZR_S_REAL,
    STNT1W_ZRI,
    STNT1W_ZRR,
    STNT1W_ZZR_D_REAL,
    STNT1W_ZZR_S_REAL,
    STR_PXI,
    STR_ZXI,
    WHILERW_PXX_B,
    WHILERW_PXX_D,
    WHILERW_PXX_H,
    WHILERW_PXX_S,
    WHILEWR_PXX_B,
    WHILEWR_PXX_D,
    WHILEWR_PXX_H,
    WHILEWR_PXX_S,
    WRFFR,

    // ARMv8 MTE
    LDG,
    LDGM,
    ST2GOffset,
    ST2GPostIndex,
    ST2GPreIndex,
    STGOffset,
    STGPi,
    STGPostIndex,
    STGPpost,
    STGPpre,
    STGPreIndex,
    STZ2GOffset,
    STZ2GPostIndex,
    STZ2GPreIndex,
    STZGOffset,
    STZGPostIndex,
    STZGPreIndex,

    // ARMv8 Pauth
    LDRAAindexed,
    LDRAAwriteback,
    LDRABindexed,
    LDRABwriteback,

    // ARMv9 TME
    TCANCEL,
    TCOMMIT,
    TSTART,
};

// instruction that reads memory but without mayLoad
const std::set<unsigned> fixupRead{
    LD64B,
};

// instruction that writes memory but without mayStore
const std::set<unsigned> fixupWrite{
    ST64B,
    ST64BV,
    ST64BV0,
};

// instruction with mayLoad but don't reads memory
const std::set<unsigned> fixupNoRead{
    // exclusive mecanism
    CLREX,
    STLXPW,
    STLXPX,
    STLXRB,
    STLXRH,
    STLXRW,
    STLXRX,
    STXPW,
    STXPX,
    STXRB,
    STXRH,
    STXRW,
    STXRX,
    // barrier
    DMB,
    DSB,
    HINT,
    ISB,
};
// instruction with mayStore but don't writes memory
const std::set<unsigned> fixupNoWrite{
    // exclusive mecanism
    LDXRB,
    LDXRH,
    LDXRW,
    LDXRX,
    LDAXRB,
    LDAXRH,
    LDAXRW,
    LDAXRX,
    LDXPW,
    LDXPX,
    LDAXPW,
    LDAXPX,
    CLREX,
    // barrier
    DMB,
    DSB,
    HINT,
    ISB,
};

} // namespace

TEST_CASE_METHOD(MemoryAccessTable, "MemoryAccessTable-CrossCheck") {

  const QBDI::LLVMCPU &llvmcpu = getCPU(QBDI::CPUMode::DEFAULT);
  const llvm::MCInstrInfo &MCII = llvmcpu.getMCII();
  int nbfail = 0;

  for (unsigned opcode = 0; opcode < llvm::AArch64::INSTRUCTION_LIST_END;
       opcode++) {
    if (unsupportedInst.count(opcode) == 1)
      continue;

    const llvm::MCInstrDesc &desc = MCII.get(opcode);
    const char *mnemonic = MCII.getName(opcode).data();

    // InstInfo_AARCH64.cpp only use inst->getOpcode(). The MCInst doesn't need
    // to have his operand
    llvm::MCInst inst;
    inst.setOpcode(opcode);

    bool doRead = (QBDI::getReadSize(inst, llvmcpu) != 0);
    bool doWrite = (QBDI::getWriteSize(inst, llvmcpu) != 0);
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
