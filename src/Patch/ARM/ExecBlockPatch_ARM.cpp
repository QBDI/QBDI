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

#include "Engine/LLVMCPU.h"

#include "Patch/ARM/ExecBlockPatch_ARM.h"
#include "Patch/ARM/Layer2_ARM.h"
#include "Patch/ARM/PatchGenerator_ARM.h"
#include "Patch/ARM/RelocatableInst_ARM.h"

#include "Utility/LogSys.h"
#include "Utility/System.h"

#include "QBDI/Options.h"
#include "QBDI/State.h"

namespace QBDI {

RelocatableInst::UniquePtrVec getExecBlockPrologue(const LLVMCPU &llvmcpu) {
  Options opts = llvmcpu.getOptions();
  RelocatableInst::UniquePtrVec prologue;

  // prologue.push_back(Bkpt(CPUMode::ARM, 0));

  // Save host LR
  // ============
  prologue.push_back(Pushr1(CPUMode::ARM, Reg(REG_LR)));

  // Save host SP
  // ============
  append(prologue, SaveReg(Reg(REG_SP), Offset(offsetof(Context, hostState.sp)))
                       .genReloc(llvmcpu));

  // set R0 to the address of the dataBlock
  append(prologue, SetDataBlockAddress(Reg(0)).genReloc(llvmcpu));

  if ((opts & Options::OPT_DISABLE_FPR) == 0) {
    // set R1 at the begin of FPRState
    prologue.push_back(Add(CPUMode::ARM, Reg(1), Reg(0),
                           Constant(offsetof(Context, fprState))));
    // load fpscr in R2
    prologue.push_back(
        NoReloc::unique(ldri12(Reg(2), Reg(1), offsetof(FPRState, fpscr))));

    // Restore FPR
    // ===========
    prologue.push_back(VLdmIA(CPUMode::ARM, Reg(1), 0, 16, true));
#if QBDI_NUM_FPR == 32
    if ((opts & Options::OPT_DISABLE_D16_D31) == 0) {
      prologue.push_back(VLdmIA(CPUMode::ARM, Reg(1), 16, 16));
    }
#endif

    // Restore FPSCR
    // =============
    prologue.push_back(Vmsr(CPUMode::ARM, Reg(2)));
  }

  // set R0 at the begin of GPRState
  prologue.push_back(
      Add(CPUMode::ARM, Reg(0), Reg(0), Constant(offsetof(Context, gprState))));

  // Restore CPSR
  // ============
  prologue.push_back(
      NoReloc::unique(ldri12(Reg(1), Reg(0), offsetof(GPRState, cpsr))));
  prologue.push_back(Msr(CPUMode::ARM, Reg(1)));

  // Restore GPR
  // ===========

  prologue.push_back(
      LdmIA(CPUMode::ARM, Reg(0), /* R0-R12 + SP + LR */ 0b0111111111111111));

  // Jump selector
  // =============
  prologue.push_back(LoadDataBlock::unique(
      Reg(REG_PC), Offset(offsetof(Context, hostState.selector))));

  return prologue;
}

RelocatableInst::UniquePtrVec getExecBlockEpilogue(const LLVMCPU &llvmcpu) {
  Options opts = llvmcpu.getOptions();
  RelocatableInst::UniquePtrVec epilogue;

  // Save R0
  // =======
  append(epilogue, SaveReg(Reg(0), Offset(offsetof(Context, gprState.r0)))
                       .genReloc(llvmcpu));

  // set R0 to the address of the dataBlock
  append(epilogue, SetDataBlockAddress(Reg(0)).genReloc(llvmcpu));
  // Set R0 to GPRState.r1
  epilogue.push_back(Add(CPUMode::ARM, Reg(0), Reg(0),
                         Constant(offsetof(Context, gprState.r1))));

  // Save R1-R12 SP LR
  epilogue.push_back(
      StmIA(CPUMode::ARM, Reg(0), /* R1-R12 + SP + LR */ 0b0111111111111110));

  // Save CPSR
  // =========
  epilogue.push_back(Mrs(CPUMode::ARM, Reg(1)));
  epilogue.push_back(
      StoreDataBlock::unique(Reg(1), Offset(offsetof(Context, gprState.cpsr))));

  if ((opts & Options::OPT_DISABLE_FPR) == 0) {
    // set R1 at the begin of FPRState (from the gprState.r1)
    epilogue.push_back(Add(CPUMode::ARM, Reg(1), Reg(0),
                           Constant(offsetof(Context, fprState) -
                                    offsetof(Context, gprState.r1))));
    // Restore FPSCR
    // =============
    epilogue.push_back(Vmrs(CPUMode::ARM, Reg(2)));
    epilogue.push_back(StoreDataBlock::unique(
        Reg(2), Offset(offsetof(Context, fprState.fpscr))));

    // Save FPR
    // ========
    epilogue.push_back(VStmIA(CPUMode::ARM, Reg(1), 0, 16, true));
#if QBDI_NUM_FPR == 32
    if ((opts & Options::OPT_DISABLE_D16_D31) == 0) {
      epilogue.push_back(VStmIA(CPUMode::ARM, Reg(1), 16, 16));
    }
#endif
  }

  // Restore host SP
  // ===============
  append(epilogue, LoadReg(Reg(REG_SP), Offset(offsetof(Context, hostState.sp)))
                       .genReloc(llvmcpu));

  // Return to host
  // ==============
  epilogue.push_back(Popr1(CPUMode::ARM, Reg(REG_PC)));

  return epilogue;
}

// Patch allowing to terminate a basic block early by writing address into
// DataBlock[Offset(PC)]
RelocatableInst::UniquePtrVec getTerminator(const LLVMCPU &llvmcpu,
                                            rword address) {
  RelocatableInst::UniquePtrVec terminator;

  // Set the LSB to the CPUMode
  if (llvmcpu.getCPUMode() == CPUMode::ARM) {
    address &= (~1);
  } else {
    address |= 1;
  }

  // for thumb, any register can be used to be the scratch register
  // except LR. Used LR to saved the next address
  append(terminator,
         SaveReg(Reg(REG_LR), Offset(Reg(REG_LR))).genReloc(llvmcpu));
  terminator.push_back(LoadImm::unique(Reg(REG_LR), address));
  append(terminator,
         SaveReg(Reg(REG_LR), Offset(Reg(REG_PC))).genReloc(llvmcpu));
  append(terminator,
         LoadReg(Reg(REG_LR), Offset(Reg(REG_LR))).genReloc(llvmcpu));

  return terminator;
}

// Change ScratchRegister
RelocatableInst::UniquePtrVec
changeScratchRegister(const LLVMCPU &llvmcpu, RegLLVM oldSR, RegLLVM nextSR_) {

  QBDI_REQUIRE_ABORT(llvmcpu == CPUMode::Thumb,
                     "No scratch Register in ARM mode");

  QBDI_REQUIRE_ABORT(getGPRPosition(nextSR_) != ((size_t)-1),
                     "Unexpected next ScratchRegister {}",
                     llvmcpu.getRegisterName(nextSR_));

  Reg nextSR = getGPRPosition(nextSR_);

  // get another temporary register
  Reg tmp = 0;
  while (tmp == oldSR or tmp == nextSR) {
    tmp = tmp.getID() + 1;
  }

  RelocatableInst::UniquePtrVec changeReloc;

  changeReloc.push_back(RelocTag::unique(RelocTagChangeScratchRegister));

  // save the temporary register
  changeReloc.push_back(NoReloc::unique(t2stri12(tmp, oldSR, tmp.offset())));
  // load the real value of the old SR
  changeReloc.push_back(NoReloc::unique(
      t2ldri12(tmp, oldSR, offsetof(Context, hostState.scratchRegisterValue))));
  // backup the real value of the next SR
  changeReloc.push_back(NoReloc::unique(t2stri12(
      nextSR, oldSR, offsetof(Context, hostState.scratchRegisterValue))));
  // change the SR
  changeReloc.push_back(NoReloc::unique(tmovr(nextSR, oldSR)));
  // restore the value of the old SR
  changeReloc.push_back(NoReloc::unique(tmovr(oldSR, tmp)));
  // change the index of the SRregister
  changeReloc.push_back(NoReloc::unique(t2movi(tmp, nextSR.getID())));
  changeReloc.push_back(NoReloc::unique(
      t2stri12(tmp, nextSR, offsetof(Context, hostState.currentSROffset))));
  // restore the temporary register
  changeReloc.push_back(NoReloc::unique(t2ldri12(tmp, nextSR, tmp.offset())));

  return changeReloc;
}

} // namespace QBDI
