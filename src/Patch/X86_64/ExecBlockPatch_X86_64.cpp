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
#include <algorithm>
#include <stddef.h>
#include <vector>

#include "X86InstrInfo.h"

#include "QBDI/Config.h"
#include "QBDI/Options.h"
#include "QBDI/State.h"
#include "Engine/LLVMCPU.h"
#include "ExecBlock/Context.h"
#include "Patch/ExecBlockPatch.h"
#include "Patch/PatchGenerator.h"
#include "Patch/PatchUtils.h"
#include "Patch/RelocatableInst.h"
#include "Patch/Types.h"
#include "Patch/X86_64/ExecBlockFlags_X86_64.h"
#include "Patch/X86_64/Layer2_X86_64.h"
#include "Utility/LogSys.h"
#include "Utility/System.h"

namespace QBDI {

RelocatableInst::UniquePtrVec getExecBlockPrologue(const LLVMCPU &llvmcpu) {
  Options opts = llvmcpu.getOptions();
  RelocatableInst::UniquePtrVec prologue;

  // Save host SP
  append(prologue, SaveReg(Reg(REG_SP), Offset(offsetof(Context, hostState.sp)))
                       .genReloc(llvmcpu));
  // Restore FPR
  if ((opts & Options::OPT_DISABLE_FPR) == 0) {
    if ((opts & Options::OPT_DISABLE_OPTIONAL_FPR) == 0) {
      append(prologue,
             LoadReg(Reg(0), Offset(offsetof(Context, hostState.executeFlags)))
                 .genReloc(llvmcpu));
      prologue.push_back(Test(Reg(0), ExecBlockFlags::needFPU));
      prologue.push_back(Je(7 + 4));
    }
    prologue.push_back(Fxrstor(Offset(offsetof(Context, fprState))));
    // target je needFPU
    if (isHostCPUFeaturePresent("avx")) {
      QBDI_DEBUG("AVX support enabled in guest context switches");
      // don't restore if not needed
      if ((opts & Options::OPT_DISABLE_OPTIONAL_FPR) == 0) {
        prologue.push_back(Test(Reg(0), ExecBlockFlags::needAVX));
        if constexpr (is_x86_64)
          prologue.push_back(Je(16 * 10 + 4));
        else
          prologue.push_back(Je(8 * 10 + 4));
      }
      prologue.push_back(Vinsertf128(
          llvm::X86::YMM0,
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm0)), 1));
      prologue.push_back(Vinsertf128(
          llvm::X86::YMM1,
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm1)), 1));
      prologue.push_back(Vinsertf128(
          llvm::X86::YMM2,
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm2)), 1));
      prologue.push_back(Vinsertf128(
          llvm::X86::YMM3,
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm3)), 1));
      prologue.push_back(Vinsertf128(
          llvm::X86::YMM4,
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm4)), 1));
      prologue.push_back(Vinsertf128(
          llvm::X86::YMM5,
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm5)), 1));
      prologue.push_back(Vinsertf128(
          llvm::X86::YMM6,
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm6)), 1));
      prologue.push_back(Vinsertf128(
          llvm::X86::YMM7,
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm7)), 1));
#if defined(QBDI_ARCH_X86_64)
      prologue.push_back(Vinsertf128(
          llvm::X86::YMM8,
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm8)), 1));
      prologue.push_back(Vinsertf128(
          llvm::X86::YMM9,
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm9)), 1));
      prologue.push_back(Vinsertf128(
          llvm::X86::YMM10,
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm10)), 1));
      prologue.push_back(Vinsertf128(
          llvm::X86::YMM11,
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm11)), 1));
      prologue.push_back(Vinsertf128(
          llvm::X86::YMM12,
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm12)), 1));
      prologue.push_back(Vinsertf128(
          llvm::X86::YMM13,
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm13)), 1));
      prologue.push_back(Vinsertf128(
          llvm::X86::YMM14,
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm14)), 1));
      prologue.push_back(Vinsertf128(
          llvm::X86::YMM15,
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm15)), 1));
#endif // QBDI_ARCH_X86_64
       // target je needAVX
    }
  }
#if defined(QBDI_ARCH_X86_64)
  // if enable FS GS
  if ((opts & Options::OPT_ENABLE_FS_GS) == Options::OPT_ENABLE_FS_GS) {
    QBDI_REQUIRE_ABORT(isHostCPUFeaturePresent("fsgsbase"),
                       "Need CPU feature fsgsbase");

    append(prologue,
           LoadReg(Reg(0), Offset(offsetof(Context, hostState.executeFlags)))
               .genReloc(llvmcpu));
    prologue.push_back(Test(Reg(0), ExecBlockFlags::needFSGS));
    prologue.push_back(Je(5 * 4 + 7 * 4 + 4));

    append(prologue, LoadReg(Reg(3), Offset(offsetof(Context, gprState.fs)))
                         .genReloc(llvmcpu));
    append(prologue, LoadReg(Reg(4), Offset(offsetof(Context, gprState.gs)))
                         .genReloc(llvmcpu));
    prologue.push_back(Rdfsbase(Reg(1)));
    prologue.push_back(Rdgsbase(Reg(2)));
    prologue.push_back(Wrfsbase(Reg(3)));
    prologue.push_back(Wrgsbase(Reg(4)));
    append(prologue, SaveReg(Reg(1), Offset(offsetof(Context, hostState.fs)))
                         .genReloc(llvmcpu));
    append(prologue, SaveReg(Reg(2), Offset(offsetof(Context, hostState.gs)))
                         .genReloc(llvmcpu));
  }
#endif // QBDI_ARCH_X86_64
  // Restore EFLAGS
  append(prologue, LoadReg(Reg(0), Offset(offsetof(Context, gprState.eflags)))
                       .genReloc(llvmcpu));
  prologue.push_back(Pushr(Reg(0)));
  prologue.push_back(Popf());
  // Restore GPR
  for (unsigned int i = 0; i < NUM_GPR - 1; i++)
    append(prologue, LoadReg(Reg(i), Offset(Reg(i))).genReloc(llvmcpu));
  // Jump selector
  prologue.push_back(JmpM(Offset(offsetof(Context, hostState.selector))));

  return prologue;
}

RelocatableInst::UniquePtrVec getExecBlockEpilogue(const LLVMCPU &llvmcpu) {
  Options opts = llvmcpu.getOptions();
  RelocatableInst::UniquePtrVec epilogue;

  // Save GPR
  for (unsigned int i = 0; i < NUM_GPR - 1; i++)
    append(epilogue, SaveReg(Reg(i), Offset(Reg(i))).genReloc(llvmcpu));
  // Restore host SP
  append(epilogue, LoadReg(Reg(REG_SP), Offset(offsetof(Context, hostState.sp)))
                       .genReloc(llvmcpu));
  // Save EFLAGS
  epilogue.push_back(Pushf());
  epilogue.push_back(Popr(Reg(0)));
  append(epilogue, SaveReg(Reg(0), Offset(offsetof(Context, gprState.eflags)))
                       .genReloc(llvmcpu));
#if defined(QBDI_ARCH_X86_64)
  // if enable FS GS
  if ((opts & Options::OPT_ENABLE_FS_GS) == Options::OPT_ENABLE_FS_GS) {
    QBDI_REQUIRE_ABORT(isHostCPUFeaturePresent("fsgsbase"),
                       "Need CPU feature fsgsbase");

    append(epilogue,
           LoadReg(Reg(0), Offset(offsetof(Context, hostState.executeFlags)))
               .genReloc(llvmcpu));
    epilogue.push_back(Test(Reg(0), ExecBlockFlags::needFSGS));
    epilogue.push_back(Je(5 * 4 + 7 * 4 + 4));

    append(epilogue, LoadReg(Reg(3), Offset(offsetof(Context, hostState.fs)))
                         .genReloc(llvmcpu));
    append(epilogue, LoadReg(Reg(4), Offset(offsetof(Context, hostState.gs)))
                         .genReloc(llvmcpu));
    epilogue.push_back(Rdfsbase(Reg(1)));
    epilogue.push_back(Rdgsbase(Reg(2)));
    epilogue.push_back(Wrfsbase(Reg(3)));
    epilogue.push_back(Wrgsbase(Reg(4)));
    append(epilogue, SaveReg(Reg(1), Offset(offsetof(Context, gprState.fs)))
                         .genReloc(llvmcpu));
    append(epilogue, SaveReg(Reg(2), Offset(offsetof(Context, gprState.gs)))
                         .genReloc(llvmcpu));
  }
#endif // QBDI_ARCH_X86_64
  // Save FPR
  if ((opts & Options::OPT_DISABLE_FPR) == 0) {
    if ((opts & Options::OPT_DISABLE_OPTIONAL_FPR) == 0) {
      append(epilogue,
             LoadReg(Reg(0), Offset(offsetof(Context, hostState.executeFlags)))
                 .genReloc(llvmcpu));
      epilogue.push_back(Test(Reg(0), ExecBlockFlags::needFPU));
      epilogue.push_back(Je(7 + 4));
    }
    epilogue.push_back(Fxsave(Offset(offsetof(Context, fprState))));
    // target je needFPU
    if (isHostCPUFeaturePresent("avx")) {
      QBDI_DEBUG("AVX support enabled in guest context switches");
      // don't save if not needed
      if ((opts & Options::OPT_DISABLE_OPTIONAL_FPR) == 0) {
        epilogue.push_back(Test(Reg(0), ExecBlockFlags::needAVX));
        if constexpr (is_x86_64)
          epilogue.push_back(Je(16 * 10 + 4));
        else
          epilogue.push_back(Je(8 * 10 + 4));
      }
      epilogue.push_back(Vextractf128(
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm0)),
          llvm::X86::YMM0, 1));
      epilogue.push_back(Vextractf128(
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm1)),
          llvm::X86::YMM1, 1));
      epilogue.push_back(Vextractf128(
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm2)),
          llvm::X86::YMM2, 1));
      epilogue.push_back(Vextractf128(
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm3)),
          llvm::X86::YMM3, 1));
      epilogue.push_back(Vextractf128(
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm4)),
          llvm::X86::YMM4, 1));
      epilogue.push_back(Vextractf128(
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm5)),
          llvm::X86::YMM5, 1));
      epilogue.push_back(Vextractf128(
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm6)),
          llvm::X86::YMM6, 1));
      epilogue.push_back(Vextractf128(
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm7)),
          llvm::X86::YMM7, 1));
#if defined(QBDI_ARCH_X86_64)
      epilogue.push_back(Vextractf128(
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm8)),
          llvm::X86::YMM8, 1));
      epilogue.push_back(Vextractf128(
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm9)),
          llvm::X86::YMM9, 1));
      epilogue.push_back(Vextractf128(
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm10)),
          llvm::X86::YMM10, 1));
      epilogue.push_back(Vextractf128(
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm11)),
          llvm::X86::YMM11, 1));
      epilogue.push_back(Vextractf128(
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm12)),
          llvm::X86::YMM12, 1));
      epilogue.push_back(Vextractf128(
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm13)),
          llvm::X86::YMM13, 1));
      epilogue.push_back(Vextractf128(
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm14)),
          llvm::X86::YMM14, 1));
      epilogue.push_back(Vextractf128(
          Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm15)),
          llvm::X86::YMM15, 1));
#endif // QBDI_ARCH_X86_64
       // target je needAVX
    }
  }
  // return to host
  epilogue.push_back(Ret());

  return epilogue;
}

// Patch allowing to terminate a basic block early by writing address into
// DataBlock[Offset(RIP)]
RelocatableInst::UniquePtrVec getTerminator(const LLVMCPU &llvmcpu,
                                            rword address) {
  RelocatableInst::UniquePtrVec terminator;

  append(terminator, SaveReg(Reg(0), Offset(Reg(0))).genReloc(llvmcpu));
  terminator.push_back(LoadImm::unique(Reg(0), address));
  append(terminator, SaveReg(Reg(0), Offset(Reg(REG_PC))).genReloc(llvmcpu));
  append(terminator, LoadReg(Reg(0), Offset(Reg(0))).genReloc(llvmcpu));

  return terminator;
}

} // namespace QBDI
