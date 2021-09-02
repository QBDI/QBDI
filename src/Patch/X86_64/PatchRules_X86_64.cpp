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
#include <algorithm>
#include <stddef.h>
#include <vector>

#include "X86InstrInfo.h"

#include "QBDI/Config.h"
#include "QBDI/Options.h"
#include "QBDI/State.h"
#include "ExecBlock/Context.h"
#include "Patch/InstTransform.h"
#include "Patch/PatchCondition.h"
#include "Patch/PatchGenerator.h"
#include "Patch/PatchRule.h"
#include "Patch/PatchRules.h"
#include "Patch/PatchUtils.h"
#include "Patch/RelocatableInst.h"
#include "Patch/Types.h"
#include "Patch/X86_64/ExecBlockFlags_X86_64.h"
#include "Patch/X86_64/Layer2_X86_64.h"
#include "Patch/X86_64/PatchGenerator_X86_64.h"
#include "Patch/X86_64/PatchRules_X86_64.h"
#include "Utility/LogSys.h"
#include "Utility/System.h"

namespace QBDI {

RelocatableInst::UniquePtrVec getExecBlockPrologue(Options opts) {
  RelocatableInst::UniquePtrVec prologue;

  // Save host SP
  append(prologue,
         SaveReg(Reg(REG_SP), Offset(offsetof(Context, hostState.sp))));
  // Restore FPR
  if ((opts & Options::OPT_DISABLE_FPR) == 0) {
    if ((opts & Options::OPT_DISABLE_OPTIONAL_FPR) == 0) {
      append(
          prologue,
          LoadReg(Reg(0), Offset(offsetof(Context, hostState.executeFlags))));
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
  // Restore EFLAGS
  append(prologue, LoadReg(Reg(0), Offset(offsetof(Context, gprState.eflags))));
  prologue.push_back(Pushr(Reg(0)));
  prologue.push_back(Popf());
  // Restore GPR
  for (unsigned int i = 0; i < NUM_GPR - 1; i++)
    append(prologue, LoadReg(Reg(i), Offset(Reg(i))));
  // Jump selector
  prologue.push_back(JmpM(Offset(offsetof(Context, hostState.selector))));

  return prologue;
}

RelocatableInst::UniquePtrVec getExecBlockEpilogue(Options opts) {
  RelocatableInst::UniquePtrVec epilogue;

  // Save GPR
  for (unsigned int i = 0; i < NUM_GPR - 1; i++)
    append(epilogue, SaveReg(Reg(i), Offset(Reg(i))));
  // Restore host SP
  append(epilogue,
         LoadReg(Reg(REG_SP), Offset(offsetof(Context, hostState.sp))));
  // Save EFLAGS
  epilogue.push_back(Pushf());
  epilogue.push_back(Popr(Reg(0)));
  append(epilogue, SaveReg(Reg(0), Offset(offsetof(Context, gprState.eflags))));
  // Save FPR
  if ((opts & Options::OPT_DISABLE_FPR) == 0) {
    if ((opts & Options::OPT_DISABLE_OPTIONAL_FPR) == 0) {
      append(
          epilogue,
          LoadReg(Reg(0), Offset(offsetof(Context, hostState.executeFlags))));
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

std::vector<PatchRule> getDefaultPatchRules(Options opts) {
  std::vector<PatchRule> rules;

  /* Rule #0: Avoid instrumenting instruction prefixes.
   * Target:  X86 prefixes (LOCK, REP and other REX prefixes).
   * Patch:   Output the unmodified MCInst but flag the patch as "do not
   * instrument".
   */
  rules.emplace_back(
      Or::unique(conv_unique<PatchCondition>(
          OpIs::unique(llvm::X86::LOCK_PREFIX),
          OpIs::unique(llvm::X86::REX64_PREFIX),
          OpIs::unique(llvm::X86::REP_PREFIX),
          OpIs::unique(llvm::X86::REPNE_PREFIX),
          OpIs::unique(llvm::X86::DATA16_PREFIX),
          OpIs::unique(llvm::X86::CS_PREFIX),
          OpIs::unique(llvm::X86::SS_PREFIX),
          OpIs::unique(llvm::X86::DS_PREFIX),
          OpIs::unique(llvm::X86::ES_PREFIX),
          OpIs::unique(llvm::X86::FS_PREFIX),
          OpIs::unique(llvm::X86::GS_PREFIX),
          OpIs::unique(llvm::X86::XACQUIRE_PREFIX),
          OpIs::unique(llvm::X86::XRELEASE_PREFIX))),
      conv_unique<PatchGenerator>(
          DoNotInstrument::unique(),
          ModifyInstruction::unique(InstTransform::UniquePtrVec())));

  /* Rule #1: Simulate jmp to memory value using RIP addressing.
   * Target:  JMP *[RIP + IMM]
   * Patch:   Temp(0) := RIP + Constant(0)
   *          JMP *[RIP + IMM] --> MOV Temp(1), [Temp(0) + IMM]
   *          DataBlock[Offset(RIP)] := Temp(1)
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(OpIs::unique(llvm::X86::JMP64m),
                                              UseReg::unique(Reg(REG_PC)))),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Constant(0)),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SubstituteWithTemp::unique(Reg(REG_PC), Temp(0)),
              SetOpcode::unique(llvm::X86::MOV64rm),
              AddOperand::unique(Operand(0), Temp(1)))),
          WriteTemp::unique(Temp(1), Offset(Reg(REG_PC)))));

  /* Rule #2: Simulate call to memory value using RIP addressing.
   * Target:  CALL *[RIP + IMM]
   * Patch:   Temp(0) := RIP + Constant(0)
   *          CALL *[RIP + IMM] --> MOV Temp(1), [Temp(0) + IMM]
   *          SimulateCall(Temp(1))
   */
  rules.emplace_back(
      And::unique(conv_unique<PatchCondition>(OpIs::unique(llvm::X86::CALL64m),
                                              UseReg::unique(Reg(REG_PC)))),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Constant(0)),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SubstituteWithTemp::unique(Reg(REG_PC), Temp(0)),
              SetOpcode::unique(llvm::X86::MOV64rm),
              AddOperand::unique(Operand(0), Temp(1)))),
          SimulateCall::unique(Temp(1))));

  /* Rule #3: Generic RIP patching.
   * Target:  Any instruction with RIP as operand, e.g. LEA RAX, [RIP + 1]
   * Patch:   Temp(0) := rip
   *          LEA RAX, [RIP + IMM] --> LEA RAX, [Temp(0) + IMM]
   */
  rules.emplace_back(
      UseReg::unique(Reg(REG_PC)),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Constant(0)),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SubstituteWithTemp::unique(Reg(REG_PC), Temp(0))))));

  /* Rule #4: Simulate JMP to memory value.
   * Target:  JMP *MEM
   * Patch:   JMP *MEM --> MOV Temp(0), MEM
   *          DataBlock[Offset(RIP)] := Temp(0)
   */
  rules.emplace_back(
      Or::unique(conv_unique<PatchCondition>(OpIs::unique(llvm::X86::JMP32m),
                                             OpIs::unique(llvm::X86::JMP64m))),
      conv_unique<PatchGenerator>(
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SetOpcode::unique(is_x86 ? llvm::X86::MOV32rm
                                       : llvm::X86::MOV64rm),
              AddOperand::unique(Operand(0), Temp(0)))),
          WriteTemp::unique(Temp(0), Offset(Reg(REG_PC)))));

  /* Rule #5: Simulate CALL to memory value.
   * Target:  CALL MEM
   * Patch:   CALL MEM --> MOV Temp(0), MEM
   *          SimulateCall(Temp(1))
   */
  rules.emplace_back(
      Or::unique(conv_unique<PatchCondition>(OpIs::unique(llvm::X86::CALL32m),
                                             OpIs::unique(llvm::X86::CALL64m))),
      conv_unique<PatchGenerator>(
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SetOpcode::unique(is_x86 ? llvm::X86::MOV32rm
                                       : llvm::X86::MOV64rm),
              AddOperand::unique(Operand(0), Temp(0)))),
          SimulateCall::unique(Temp(0))));

  /* Rule #6: Simulate JMP to constant value.
   * Target:  JMP IMM
   * Patch:   Temp(0) := RIP + Operand(0)
   *          DataBlock[Offset(RIP)] := Temp(0)
   */
  rules.emplace_back(
      Or::unique(conv_unique<PatchCondition>(OpIs::unique(llvm::X86::JMP_1),
                                             OpIs::unique(llvm::X86::JMP_2),
                                             OpIs::unique(llvm::X86::JMP_4))),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Operand(0)),
          WriteTemp::unique(Temp(0), Offset(Reg(REG_PC)))));

  /* Rule #7: Simulate JMP to register value.
   * Target:  JMP REG
   * Patch:   Temp(0) := Operand(0)
   *          DataBlock[Offset(RIP)] := Temp(0)
   */
  rules.emplace_back(
      Or::unique(conv_unique<PatchCondition>(OpIs::unique(llvm::X86::JMP32r),
                                             OpIs::unique(llvm::X86::JMP64r))),
      conv_unique<PatchGenerator>(
          GetOperand::unique(Temp(0), Operand(0)),
          WriteTemp::unique(Temp(0), Offset(Reg(REG_PC)))));

  /* Rule #8: Simulate CALL to register value.
   * Target:  CALL REG
   * Patch:   Temp(0) := Operand(0)
   *          SimulateCall(Temp(0))
   */
  rules.emplace_back(
      Or::unique(conv_unique<PatchCondition>(OpIs::unique(llvm::X86::CALL32r),
                                             OpIs::unique(llvm::X86::CALL64r))),
      conv_unique<PatchGenerator>(GetOperand::unique(Temp(0), Operand(0)),
                                  SimulateCall::unique(Temp(0))));

  /* Rule #9: Simulate Jcc IMM8.
   * Target:  Jcc IMM8
   * Patch:     Temp(0) := RIP + Operand(0)
   *         ---Jcc IMM8 --> Jcc END
   *         |  Temp(0) := RIP + Constant(0)
   *         -->END: DataBlock[Offset(RIP)] := Temp(0)
   */
  rules.emplace_back(
      OpIs::unique(llvm::X86::JCC_1),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Operand(0)),
          ModifyInstruction::unique(
              conv_unique<InstTransform>(SetOperand::unique(
                  Operand(0),
                  Constant(is_x86 ? 6 : 11)) // Offset to jump the next load.
                                         )),
          GetPCOffset::unique(Temp(0), Constant(0)),
          WriteTemp::unique(Temp(0), Offset(Reg(REG_PC)))));

  /* Rule #10: Simulate Jcc IMM16.
   * Target:  Jcc IMM16
   * Patch:     Temp(0) := RIP + Operand(0)
   *         ---Jcc IMM16 --> Jcc END
   *         |  Temp(0) := RIP + Constant(0)
   *         -->END: DataBlock[Offset(RIP)] := Temp(0)
   */
  rules.emplace_back(
      OpIs::unique(llvm::X86::JCC_2),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Operand(0)),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SetOperand::unique(Operand(0), Constant(is_x86 ? 7 : 12)))),
          GetPCOffset::unique(Temp(0), Constant(0)),
          WriteTemp::unique(Temp(0), Offset(Reg(REG_PC)))));

  /* Rule #11: Simulate Jcc IMM32.
   * Target:  Jcc IMM32
   * Patch:     Temp(0) := RIP + Operand(0)
   *         ---Jcc IMM32 --> Jcc END
   *         |  Temp(0) := RIP + Constant(0)
   *         -->END: DataBlock[Offset(RIP)] := Temp(0)
   */
  rules.emplace_back(
      OpIs::unique(llvm::X86::JCC_4),
      conv_unique<PatchGenerator>(
          GetPCOffset::unique(Temp(0), Operand(0)),
          ModifyInstruction::unique(conv_unique<InstTransform>(
              SetOperand::unique(Operand(0), Constant(is_x86 ? 9 : 14)))),
          GetPCOffset::unique(Temp(0), Constant(0)),
          WriteTemp::unique(Temp(0), Offset(Reg(REG_PC)))));

  /* Rule #12: Simulate CALL to constant offset.
   * Target:   CALL IMM
   * Patch:    Temp(0) := RIP + Operand(0)
   *           SimulateCall(Temp(0))
   */
  rules.emplace_back(
      Or::unique(
          conv_unique<PatchCondition>(OpIs::unique(llvm::X86::CALL64pcrel32),
                                      OpIs::unique(llvm::X86::CALLpcrel16),
                                      OpIs::unique(llvm::X86::CALLpcrel32))),
      conv_unique<PatchGenerator>(GetPCOffset::unique(Temp(0), Operand(0)),
                                  SimulateCall::unique(Temp(0))));

  /* Rule #13: Simulate return.
   * Target:   RET
   * Patch:    SimulateRet(Temp(0))
   */
  rules.emplace_back(
      Or::unique(conv_unique<PatchCondition>(
          OpIs::unique(llvm::X86::RETL), OpIs::unique(llvm::X86::RETQ),
          OpIs::unique(llvm::X86::RETW), OpIs::unique(llvm::X86::RETIL),
          OpIs::unique(llvm::X86::RETIQ), OpIs::unique(llvm::X86::RETIW))),
      conv_unique<PatchGenerator>(SimulateRet::unique(Temp(0))));

  /* Rule #14: Default rule for every other instructions.
   * Target:   *
   * Patch:    Output original unmodified instructions.
   */
  rules.emplace_back(True::unique(),
                     conv_unique<PatchGenerator>(ModifyInstruction::unique(
                         InstTransform::UniquePtrVec())));

  return rules;
}

// Patch allowing to terminate a basic block early by writing address into
// DataBlock[Offset(RIP)]
RelocatableInst::UniquePtrVec getTerminator(rword address) {
  RelocatableInst::UniquePtrVec terminator;

  append(terminator, SaveReg(Reg(0), Offset(Reg(0))));
  terminator.push_back(NoReloc::unique(movri(Reg(0), address)));
  append(terminator, SaveReg(Reg(0), Offset(Reg(REG_PC))));
  append(terminator, LoadReg(Reg(0), Offset(Reg(0))));

  return terminator;
}

} // namespace QBDI
