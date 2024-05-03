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
#include "AArch64InstrInfo.h"

#include "Engine/LLVMCPU.h"
#include "Patch/AARCH64/ExecBlockPatch_AARCH64.h"
#include "Patch/AARCH64/Layer2_AARCH64.h"
#include "Patch/AARCH64/PatchGenerator_AARCH64.h"
#include "Patch/AARCH64/RelocatableInst_AARCH64.h"
#include "Patch/ExecBlockPatch.h"
#include "Patch/RelocatableInst.h"

#include "Utility/LogSys.h"
#include "Utility/System.h"

#include "QBDI/Options.h"

namespace QBDI {

RelocatableInst::UniquePtrVec getExecBlockPrologue(const LLVMCPU &llvmcpu) {
  Options opts = llvmcpu.getOptions();
  RelocatableInst::UniquePtrVec prologue;

  prologue.push_back(BTIc());

  // X28 is used to address the DataBlock
  prologue.push_back(SetBaseAddress::unique(Reg(28)));

  // Save return address
  // ===================
  prologue.push_back(StrPre(Reg(REG_LR), Reg(REG_SP), Constant(-16)));

  // Save Host SP
  // ============
  prologue.push_back(Mov(Reg(0), Reg(REG_SP)));
  prologue.push_back(
      Str(Reg(0), Reg(28), Offset(offsetof(Context, hostState.sp))));

  if ((opts & Options::OPT_DISABLE_FPR) == 0) {
    // Restore SIMD
    // ============
    prologue.push_back(
        Add(Reg(0), Reg(28), Constant(offsetof(Context, fprState.v0))));
    prologue.push_back(Ld1PostInc(llvm::AArch64::Q0_Q1_Q2_Q3, Reg(0)));
    prologue.push_back(Ld1PostInc(llvm::AArch64::Q4_Q5_Q6_Q7, Reg(0)));
    prologue.push_back(Ld1PostInc(llvm::AArch64::Q8_Q9_Q10_Q11, Reg(0)));
    prologue.push_back(Ld1PostInc(llvm::AArch64::Q12_Q13_Q14_Q15, Reg(0)));
    prologue.push_back(Ld1PostInc(llvm::AArch64::Q16_Q17_Q18_Q19, Reg(0)));
    prologue.push_back(Ld1PostInc(llvm::AArch64::Q20_Q21_Q22_Q23, Reg(0)));
    prologue.push_back(Ld1PostInc(llvm::AArch64::Q24_Q25_Q26_Q27, Reg(0)));
    prologue.push_back(Ld1PostInc(llvm::AArch64::Q28_Q29_Q30_Q31, Reg(0)));

    // Restore FPCR and FPSR
    // =====================
    static_assert(offsetof(FPRState, fpcr) + 8 == offsetof(FPRState, fpsr));
    static_assert(offsetof(Context, fprState.fpcr) ==
                  offsetof(Context, fprState.v31) + sizeof(__uint128_t));
    prologue.push_back(LdpPost(Reg(1), Reg(2), Reg(0), Constant(16)));
    prologue.push_back(WriteFPCR(Reg(1)));
    prologue.push_back(WriteFPSR(Reg(2)));
  }
  prologue.push_back(
      Add(Reg(0), Reg(28), Constant(offsetof(Context, gprState))));

  // Restore Stack and NZCV
  // ======================
  static_assert(offsetof(GPRState, sp) + 8 == offsetof(GPRState, nzcv));
  prologue.push_back(Ldp(Reg(1), Reg(2), Reg(0), offsetof(GPRState, sp)));
  prologue.push_back(WriteNZCV(Reg(2)));
  prologue.push_back(Mov(Reg(REG_SP), Reg(1)));

  // Restore LR and X29
  // ==================
  prologue.push_back(Ldp(Reg(29), Reg(30), Reg(0), offsetof(GPRState, x29)));

  // Skip restore of X28. It will be restore by the instrumented block if
  // needed.

  // Load other registers
  // ====================
  for (int i = 26; i >= 0; i -= 2) {
    if constexpr (is_osx) {
      if (i == 18) {
        // skip x18 that is platform reserved
        prologue.push_back(Ldr(Reg(i + 1), Reg(0), (i + 1) * sizeof(rword)));
      } else {
        prologue.push_back(Ldp(Reg(i), Reg(i + 1), Reg(0), i * sizeof(rword)));
      }
    } else {
      prologue.push_back(Ldp(Reg(i), Reg(i + 1), Reg(0), i * sizeof(rword)));
    }
  }

  // Jump selector
  // =============
  prologue.push_back(
      Ldr(Reg(28), Reg(28), Offset(offsetof(Context, hostState.selector))));

  prologue.push_back(Br(Reg(28)));

  return prologue;
}

RelocatableInst::UniquePtrVec getExecBlockEpilogue(const LLVMCPU &llvmcpu) {
  Options opts = llvmcpu.getOptions();
  RelocatableInst::UniquePtrVec epilogue;

  // X28 is used to address the DataBlock
  // The saved value of X28 is saved by the instrumented block
  // The epilogue is jitted at the end of the block
  epilogue.push_back(SetBaseAddress::unique(Reg(28)));

  // Save GPR from the guest
  // =======================
  for (unsigned i = 0; i < 28; i += 2) {
    if constexpr (is_osx) {
      if (i == 18) {
        // skip x18 that is platform reserved
        epilogue.push_back(
            Str(Reg(i + 1), Reg(28),
                offsetof(Context, gprState) + (i + 1) * sizeof(rword)));
      } else {
        epilogue.push_back(
            Stp(Reg(i), Reg(i + 1), Reg(28),
                offsetof(Context, gprState) + i * sizeof(rword)));
      }
    } else {
      epilogue.push_back(Stp(Reg(i), Reg(i + 1), Reg(28),
                             offsetof(Context, gprState) + i * sizeof(rword)));
    }
  }

  // Save X29 and LR
  // ===============
  epilogue.push_back(
      Stp(Reg(29), Reg(30), Reg(28), offsetof(Context, gprState.x29)));

  // Save stack and NZCV
  // ===================
  static_assert(offsetof(GPRState, sp) + 8 == offsetof(GPRState, nzcv));
  epilogue.push_back(ReadNZCV(Reg(1)));
  epilogue.push_back(Mov(Reg(0), Reg(REG_SP)));
  epilogue.push_back(
      Stp(Reg(0), Reg(1), Reg(28), offsetof(Context, gprState.sp)));

  if ((opts & Options::OPT_DISABLE_FPR) == 0) {
    // Save FPCR and FPSR
    // ==================
    static_assert(offsetof(FPRState, fpcr) + 8 == offsetof(FPRState, fpsr));

    // set X0 at the beginning of the FPRState
    epilogue.push_back(
        Add(Reg(0), Reg(28), Constant(offsetof(Context, fprState.v0))));

    // Get FPCR and FPSR
    // =================
    epilogue.push_back(ReadFPCR(Reg(1)));
    epilogue.push_back(ReadFPSR(Reg(2)));

    // Save FPR
    // ========
    epilogue.push_back(St1PostInc(llvm::AArch64::Q0_Q1_Q2_Q3, Reg(0)));
    epilogue.push_back(St1PostInc(llvm::AArch64::Q4_Q5_Q6_Q7, Reg(0)));
    epilogue.push_back(St1PostInc(llvm::AArch64::Q8_Q9_Q10_Q11, Reg(0)));
    epilogue.push_back(St1PostInc(llvm::AArch64::Q12_Q13_Q14_Q15, Reg(0)));
    epilogue.push_back(St1PostInc(llvm::AArch64::Q16_Q17_Q18_Q19, Reg(0)));
    epilogue.push_back(St1PostInc(llvm::AArch64::Q20_Q21_Q22_Q23, Reg(0)));
    epilogue.push_back(St1PostInc(llvm::AArch64::Q24_Q25_Q26_Q27, Reg(0)));
    epilogue.push_back(St1PostInc(llvm::AArch64::Q28_Q29_Q30_Q31, Reg(0)));

    // Set FPCR and FPSR
    // =================
    static_assert(offsetof(Context, fprState.fpcr) ==
                  offsetof(Context, fprState.v31) + sizeof(__uint128_t));
    epilogue.push_back(Stp(Reg(1), Reg(2), Reg(0), 0));
  }

  // Restore Host SP
  // ===============
  epilogue.push_back(
      Ldr(Reg(0), Reg(28), Offset(offsetof(Context, hostState.sp))));

  epilogue.push_back(Mov(Reg(REG_SP), Reg(0)));

  // Return to host
  // ==============
  epilogue.push_back(LdrPost(Reg(REG_LR), Reg(REG_SP), Constant(16)));

  epilogue.push_back(Ret());
  return epilogue;
}

// Patch allowing to terminate a basic block early by writing address into
// DataBlock[Offset(PC)]
RelocatableInst::UniquePtrVec getTerminator(const LLVMCPU &llvmcpu,
                                            rword address) {
  RelocatableInst::UniquePtrVec terminator;

  // X28 is never restore by the prologue/epilogue.
  // It can be used as a temp register without needed to restore it
  // except when the instruction uses X28.
  // The Terminator is never include in a Patch, X28 is always available

  terminator.push_back(Mov(Reg(28), Constant(address)));
  terminator.push_back(Str(Reg(28), Offset(Reg(REG_PC))));

  return terminator;
}

// Change ScratchRegister
RelocatableInst::UniquePtrVec
changeScratchRegister(const LLVMCPU &llvmcpu, RegLLVM oldSR, RegLLVM nextSR_) {

  QBDI_REQUIRE_ABORT(getGPRPosition(nextSR_) != ((size_t)-1),
                     "Unexpected next ScratchRegister {}",
                     llvmcpu.getRegisterName(nextSR_));

  Reg nextSR = getGPRPosition(nextSR_);

  // use X28
  Reg tmp = 28;
  QBDI_REQUIRE_ABORT(tmp != oldSR,
                     "Unexpected use of X28 as a ScratchRegister");
  QBDI_REQUIRE_ABORT(tmp != nextSR,
                     "Unexpected use of X28 as a ScratchRegister");

  RelocatableInst::UniquePtrVec changeReloc;

  changeReloc.push_back(RelocTag::unique(RelocTagChangeScratchRegister));

  // load the real value of the old SR
  changeReloc.push_back(NoReloc::unique(
      ldr(tmp, oldSR, offsetof(Context, hostState.scratchRegisterValue))));
  // backup the real value of the next SR
  changeReloc.push_back(NoReloc::unique(
      str(nextSR, oldSR, offsetof(Context, hostState.scratchRegisterValue))));
  // change the SR
  changeReloc.push_back(NoReloc::unique(movrr(nextSR, oldSR)));
  // restore the value of the old SR
  changeReloc.push_back(NoReloc::unique(movrr(oldSR, tmp)));
  // change the index of the SRregister
  changeReloc.push_back(NoReloc::unique(movri(tmp, nextSR.getID())));
  changeReloc.push_back(NoReloc::unique(
      str(tmp, nextSR, offsetof(Context, hostState.currentSROffset))));

  return changeReloc;
}

} // namespace QBDI
