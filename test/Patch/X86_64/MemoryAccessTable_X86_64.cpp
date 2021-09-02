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
#include <catch2/catch.hpp>
#include <stdio.h>

#include "X86InstrInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrDesc.h"

#include "Patch/InstInfo.h"
#include "Patch/MemoryAccessTable.h"

namespace {

using namespace llvm::X86;

const std::set<unsigned> unsupportedInst{
    // codeGenOnly
    // alias for other instruction, will never be dissassemble by LLVM
    LXADD16,
    LXADD32,
    LXADD64,
    LXADD8,
    MAXCPDrm,
    MAXCPSrm,
    MAXCSDrm,
    MAXCSSrm,
    MINCPDrm,
    MINCPSrm,
    MINCSDrm,
    MINCSSrm,
    MMX_MOVD64from64rm,
    MMX_MOVD64to64rm,
    MOV64toPQIrm,
    MOVPQIto64mr,
    MOVSX16rm16,
    MOVSX16rm32,
    MOVSX32rm32,
    MOVZX16rm16,
    RDSSPD,
    RDSSPQ,
    REP_MOVSB_32,
    REP_MOVSD_32,
    REP_MOVSQ_32,
    REP_MOVSW_32,
    REP_MOVSB_64,
    REP_MOVSD_64,
    REP_MOVSQ_64,
    REP_MOVSW_64,
    REP_STOSB_32,
    REP_STOSB_64,
    REP_STOSD_32,
    REP_STOSD_64,
    REP_STOSQ_32,
    REP_STOSQ_64,
    REP_STOSW_32,
    REP_STOSW_64,
    SBB8mi8,
    VMAXCPDYrm,
    VMAXCPDrm,
    VMAXCPSYrm,
    VMAXCPSrm,
    VMAXCSDrm,
    VMAXCSSrm,
    VMINCPDYrm,
    VMINCPDrm,
    VMINCPSYrm,
    VMINCPSrm,
    VMINCSDrm,
    VMINCSSrm,
    VMOV64toPQIrm,
    VMOVPQIto64mr,
    // priviledged instruction
    INVPCID32,
    VMREAD32mr,
    VMREAD64mr,
    VMWRITE32rm,
    VMWRITE64rm,
    WBINVD,
    WBNOINVD,
    // CET feature (shadow stack)
    CLRSSBSY,
    INCSSPD,
    INCSSPQ,
    RSTORSSP,
    SAVEPREVSSP,
    SETSSBSY,
    WRSSD,
    WRSSQ,
    WRUSSD,
    WRUSSQ,
    // RTM feature unsupported
    XABORT,
    XBEGIN,
    XEND,
    // AVX512 unsupported
    KMOVBkm,
    KMOVBmk,
    KMOVDkm,
    KMOVDmk,
    KMOVQkm,
    KMOVQmk,
    KMOVWkm,
    KMOVWmk,
    // MPX feature
    BNDLDXrm,
    BNDSTXmr,
    // complex & conditionnal memory access
    VGATHERDPDYrm,
    VGATHERDPDrm,
    VGATHERDPSYrm,
    VGATHERDPSrm,
    VGATHERQPDYrm,
    VGATHERQPDrm,
    VGATHERQPSYrm,
    VGATHERQPSrm,
    VPGATHERDDYrm,
    VPGATHERDDrm,
    VPGATHERDQYrm,
    VPGATHERDQrm,
    VPGATHERQDYrm,
    VPGATHERQDrm,
    VPGATHERQQYrm,
    VPGATHERQQrm,
};

// instruction that reads memory/stack but without mayLoad
const std::set<unsigned> fixupRead{
    // clang-format off
    ARPL16mr,
    BOUNDS16rm,
    BOUNDS32rm,
    CMPSB,
    CMPSL,
    CMPSQ,
    CMPSW,
    FBLDm,
    FCOM32m,
    FCOM64m,
    FCOMP32m,
    FCOMP64m,
    FICOM16m,
    FICOM32m,
    FICOMP16m,
    FICOMP32m,
    FLDENVm,
    FRSTORm,
    LODSB,
    LODSL,
    LODSQ,
    LODSW,
    LRETIL,
    LRETIQ,
    LRETIW,
    LRETL,
    LRETQ,
    LRETW,
    MOVDIR64B16,
    MOVSB,
    MOVSL,
    MOVSQ,
    MOVSW,
    OR32mi8Locked,
    RCL16m1,
    RCL16mCL,
    RCL16mi,
    RCL32m1,
    RCL32mCL,
    RCL32mi,
    RCL64m1,
    RCL64mCL,
    RCL64mi,
    RCL8m1,
    RCL8mCL,
    RCL8mi,
    RCR16m1,
    RCR16mCL,
    RCR16mi,
    RCR32m1,
    RCR32mCL,
    RCR32mi,
    RCR64m1,
    RCR64mCL,
    RCR64mi,
    RCR8m1,
    RCR8mCL,
    RCR8mi,
    RETIL,
    RETIQ,
    RETIW,
    RETL,
    RETQ,
    RETW,
    SCASB,
    SCASL,
    SCASQ,
    SCASW,
    // clang-format on
};
// instruction that writes memory/stack but without mayStore
const std::set<unsigned> fixupWrite{
    // clang-format off
    CALL16m,
    CALL16m_NT,
    CALL16r,
    CALL16r_NT,
    CALL32m,
    CALL32m_NT,
    CALL32r,
    CALL32r_NT,
    CALL64m,
    CALL64m_NT,
    CALL64pcrel32,
    CALL64r,
    CALL64r_NT,
    CALLpcrel16,
    CALLpcrel32,
    ENTER,
    FBSTPm,
    FNSTSWm,
    FSAVEm,
    FSTENVm,
    MOVDIR64B16,
    MOVSB,
    MOVSL,
    MOVSQ,
    MOVSW,
    OR32mi8Locked,
    STOSB,
    STOSL,
    STOSQ,
    STOSW,
    // clang-format on
};
// instruction with mayLoad but don't reads memory/stack
const std::set<unsigned> fixupNoRead{
    // clang-format off
    CLDEMOTE,
    CLFLUSH,
    CLFLUSHOPT,
    CLWB,
    FEMMS,
    FXSAVE,
    FXSAVE64,
    INT,
    INT3,
    LFENCE,
    MFENCE,
    MMX_EMMS,
    MMX_MOVNTQmr,
    MOVDIRI32,
    MOVDIRI64,
    MWAITXrrr,
    MWAITrr,
    PAUSE,
    PREFETCH,
    PREFETCHNTA,
    PREFETCHT0,
    PREFETCHT1,
    PREFETCHT2,
    PREFETCHW,
    PREFETCHWT1,
    PTWRITE64r,
    PTWRITEr,
    RDFSBASE,
    RDFSBASE64,
    RDGSBASE,
    RDGSBASE64,
    RDPID32,
    SFENCE,
    TRAP,
    UD2B,
    UMONITOR16,
    UMONITOR32,
    UMONITOR64,
    VZEROALL,
    VZEROUPPER,
    WRFSBASE,
    WRFSBASE64,
    WRGSBASE,
    WRGSBASE64,
    XSETBV,
    // clang-format on
};
// instruction with mayStore but don't writes memory/stack
const std::set<unsigned> fixupNoWrite{
    // clang-format off
    CLDEMOTE,
    CLFLUSH,
    CLFLUSHOPT,
    CLWB,
    FEMMS,
    FXRSTOR,
    FXRSTOR64,
    INT,
    INT3,
    LFENCE,
    MFENCE,
    MMX_EMMS,
    MWAITXrrr,
    MWAITrr,
    PAUSE,
    PREFETCH,
    PREFETCHNTA,
    PREFETCHT0,
    PREFETCHT1,
    PREFETCHT2,
    PREFETCHW,
    PREFETCHWT1,
    PTWRITE64m,
    PTWRITE64r,
    PTWRITEm,
    PTWRITEr,
    RDFSBASE,
    RDFSBASE64,
    RDGSBASE,
    RDGSBASE64,
    RDPID32,
    SFENCE,
    UMONITOR16,
    UMONITOR32,
    UMONITOR64,
    VZEROALL,
    VZEROUPPER,
    WRFSBASE,
    WRFSBASE64,
    WRGSBASE,
    WRGSBASE64,
    XRSTOR,
    XRSTOR64,
    XRSTORS,
    XRSTORS64,
    XSETBV,
    // clang-format on
};

} // namespace

TEST_CASE_METHOD(MemoryAccessTable, "MemoryAccessTable-CrossCheck") {

  const llvm::MCInstrInfo &MCII = getCPU(QBDI::CPUMode::DEFAULT).getMCII();

  for (unsigned opcode = 0; opcode < llvm::X86::INSTRUCTION_LIST_END;
       opcode++) {
    if (unsupportedInst.count(opcode) == 1)
      continue;

    const llvm::MCInstrDesc &desc = MCII.get(opcode);
    const char *mnemonic = MCII.getName(opcode).data();

    // the opcode is a pseudo instruction used by LLVM internally
    if (desc.isPseudo())
      continue;

    // some no pseudo instructions are also pseudo ...
    if ((desc.TSFlags & llvm::X86II::FormMask) == llvm::X86II::Pseudo)
      continue;

    // not support AVX512. discard all instruction with the encodage EVEX
    // introduce with AVX512
    if ((desc.TSFlags & llvm::X86II::EncodingMask) == llvm::X86II::EVEX)
      continue;

    // not support XOP. (AMD eXtended Operations)
    if ((desc.TSFlags & llvm::X86II::EncodingMask) == llvm::X86II::XOP)
      continue;

    // InstInfo_X86_64.cpp only use inst->getOpcode(). The MCInst doesn't need
    // to have his operand
    llvm::MCInst inst;
    inst.setOpcode(opcode);

    bool doRead = (QBDI::getReadSize(inst) != 0);
    bool doWrite = (QBDI::getWriteSize(inst) != 0);
    bool mayRead = desc.mayLoad();
    bool mayWrite = desc.mayStore();

    bool bypassRead = false;
    bool bypassWrite = false;

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
      } else if (!doRead && mayRead) {
        FAIL_CHECK("Missing read for "
                   << mnemonic << " type "
                   << (desc.TSFlags & llvm::X86II::FormMask));
      }
    }

    if (!bypassWrite && doWrite != mayWrite) {
      if (doWrite && !mayWrite) {
        FAIL_CHECK("Unexpected write for " << mnemonic);
      } else if (!doWrite && mayWrite) {
        FAIL_CHECK("Missing write for "
                   << mnemonic << " type "
                   << (desc.TSFlags & llvm::X86II::FormMask));
      }
    }
  }
}
