/*
 * This file is part of QBDI.
 *
 * Copyright 2017 Quarkslab
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
#include "Patch/PatchRule.h"
#include "Patch/X86_64/PatchRules_X86_64.h"
#include "Patch/X86_64/Layer2_X86_64.h"
#include "Utility/LogSys.h"
#include "Utility/System.h"

namespace QBDI {

RelocatableInst::SharedPtrVec getExecBlockPrologue() {
    RelocatableInst::SharedPtrVec prologue;


    // Save host BP, SP
    append(prologue, SaveReg(Reg(REG_BP), Offset(offsetof(Context, hostState.bp))));
    append(prologue, SaveReg(Reg(REG_SP), Offset(offsetof(Context, hostState.sp))));
    // Restore FPR
#ifndef _QBDI_ASAN_ENABLED_ // Disabled if ASAN is enabled as it breaks context alignment
    prologue.push_back(Fxrstor(Offset(offsetof(Context, fprState))));
    if(isHostCPUFeaturePresent("avx")) {
        LogDebug("getExecBlockPrologue", "AVX support enabled in guest context switches");
        prologue.push_back(Vinsertf128(llvm::X86::YMM0, Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm0)), 1));
        prologue.push_back(Vinsertf128(llvm::X86::YMM1, Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm1)), 1));
        prologue.push_back(Vinsertf128(llvm::X86::YMM2, Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm2)), 1));
        prologue.push_back(Vinsertf128(llvm::X86::YMM3, Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm3)), 1));
        prologue.push_back(Vinsertf128(llvm::X86::YMM4, Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm4)), 1));
        prologue.push_back(Vinsertf128(llvm::X86::YMM5, Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm5)), 1));
        prologue.push_back(Vinsertf128(llvm::X86::YMM6, Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm6)), 1));
        prologue.push_back(Vinsertf128(llvm::X86::YMM7, Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm7)), 1));
#if defined(QBDI_ARCH_X86_64)
        prologue.push_back(Vinsertf128(llvm::X86::YMM8, Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm8)), 1));
        prologue.push_back(Vinsertf128(llvm::X86::YMM9, Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm9)), 1));
        prologue.push_back(Vinsertf128(llvm::X86::YMM10, Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm10)), 1));
        prologue.push_back(Vinsertf128(llvm::X86::YMM11, Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm11)), 1));
        prologue.push_back(Vinsertf128(llvm::X86::YMM12, Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm12)), 1));
        prologue.push_back(Vinsertf128(llvm::X86::YMM13, Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm13)), 1));
        prologue.push_back(Vinsertf128(llvm::X86::YMM14, Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm14)), 1));
        prologue.push_back(Vinsertf128(llvm::X86::YMM15, Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm15)), 1));
#endif // QBDI_ARCH_X86_64
    }
#endif
    // Restore EFLAGS
    append(prologue, LoadReg(Reg(0), Offset(offsetof(Context, gprState.eflags))));
    prologue.push_back(Pushr(Reg(0)));
    prologue.push_back(Popf());
    // Restore GPR
    for(unsigned int i = 0; i < NUM_GPR-1; i++)
        append(prologue, LoadReg(Reg(i), Offset(Reg(i))));
    // Jump selector
    prologue.push_back(JmpM(Offset(offsetof(Context, hostState.selector))));

    return prologue;
}

RelocatableInst::SharedPtrVec getExecBlockEpilogue() {
    RelocatableInst::SharedPtrVec epilogue;

    // Save GPR
    for(unsigned int i = 0; i < NUM_GPR-1; i++)
        append(epilogue, SaveReg(Reg(i), Offset(Reg(i))));
    // Save FPR
#ifndef _QBDI_ASAN_ENABLED_ // Disabled if ASAN is enabled as it breaks context alignment
    epilogue.push_back(Fxsave(Offset(offsetof(Context, fprState))));
    if(isHostCPUFeaturePresent("avx")) {
        LogDebug("getExecBlockEpilogue", "AVX support enabled in guest context switches");
        epilogue.push_back(Vextractf128(Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm0)), llvm::X86::YMM0, 1));
        epilogue.push_back(Vextractf128(Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm1)), llvm::X86::YMM1, 1));
        epilogue.push_back(Vextractf128(Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm2)), llvm::X86::YMM2, 1));
        epilogue.push_back(Vextractf128(Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm3)), llvm::X86::YMM3, 1));
        epilogue.push_back(Vextractf128(Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm4)), llvm::X86::YMM4, 1));
        epilogue.push_back(Vextractf128(Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm5)), llvm::X86::YMM5, 1));
        epilogue.push_back(Vextractf128(Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm6)), llvm::X86::YMM6, 1));
        epilogue.push_back(Vextractf128(Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm7)), llvm::X86::YMM7, 1));
#if defined(QBDI_ARCH_X86_64)
        epilogue.push_back(Vextractf128(Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm8)), llvm::X86::YMM8, 1));
        epilogue.push_back(Vextractf128(Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm9)), llvm::X86::YMM9, 1));
        epilogue.push_back(Vextractf128(Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm10)), llvm::X86::YMM10, 1));
        epilogue.push_back(Vextractf128(Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm11)), llvm::X86::YMM11, 1));
        epilogue.push_back(Vextractf128(Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm12)), llvm::X86::YMM12, 1));
        epilogue.push_back(Vextractf128(Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm13)), llvm::X86::YMM13, 1));
        epilogue.push_back(Vextractf128(Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm14)), llvm::X86::YMM14, 1));
        epilogue.push_back(Vextractf128(Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm15)), llvm::X86::YMM15, 1));
#endif // QBDI_ARCH_X86_64
    }
#endif
    // Restore host BP, SP
    append(epilogue, LoadReg(Reg(REG_BP), Offset(offsetof(Context, hostState.bp))));
    append(epilogue, LoadReg(Reg(REG_SP), Offset(offsetof(Context, hostState.sp))));
    // Save EFLAGS
    epilogue.push_back(Pushf());
    epilogue.push_back(Popr(Reg(0)));
    append(epilogue, SaveReg(Reg(0), Offset(offsetof(Context, gprState.eflags))));
    // return to host
    epilogue.push_back(Ret());

    return epilogue;
}

std::vector<PatchRule> getDefaultPatchRules() {
    std::vector<PatchRule> rules;

    /* Rule #0: Avoid instrumenting instruction prefixes.
     * Target:  X86 prefixes (LOCK, REP and other REX prefixes).
     * Patch:   Output the unmodified MCInst but flag the patch as "do not instrument".
    */
    rules.emplace_back(
        Or({
            OpIs(llvm::X86::LOCK_PREFIX),
            OpIs(llvm::X86::REX64_PREFIX),
            OpIs(llvm::X86::REP_PREFIX),
            OpIs(llvm::X86::REPNE_PREFIX),
            OpIs(llvm::X86::DATA16_PREFIX),
            OpIs(llvm::X86::CS_PREFIX),
            OpIs(llvm::X86::SS_PREFIX),
            OpIs(llvm::X86::DS_PREFIX),
            OpIs(llvm::X86::ES_PREFIX),
            OpIs(llvm::X86::FS_PREFIX),
            OpIs(llvm::X86::GS_PREFIX),
            OpIs(llvm::X86::XACQUIRE_PREFIX),
            OpIs(llvm::X86::XRELEASE_PREFIX)
        }),
        conv_unique<PatchGenerator>(
            DoNotInstrument(),
            ModifyInstruction({})
        )
    );

    /* Rule #1: Simulate jmp to memory value using RIP addressing.
     * Target:  JMP *[RIP + IMM]
     * Patch:   Temp(0) := RIP + Constant(0)
     *          JMP *[RIP + IMM] --> MOV Temp(1), [Temp(0) + IMM]
     *          DataBlock[Offset(RIP)] := Temp(1)
    */
    rules.emplace_back(
        And({
            OpIs(llvm::X86::JMP64m),
            UseReg(Reg(REG_PC))
        }),
        conv_unique<PatchGenerator>(
            GetPCOffset(Temp(0), Constant(0)),
            ModifyInstruction({
                SubstituteWithTemp(Reg(REG_PC), Temp(0)),
                SetOpcode(llvm::X86::MOV64rm),
                AddOperand(Operand(0), Temp(1))
            }),
            WriteTemp(Temp(1), Offset(Reg(REG_PC)))
        )
    );

    /* Rule #2: Simulate call to memory value using RIP addressing.
     * Target:  CALL *[RIP + IMM]
     * Patch:   Temp(0) := RIP + Constant(0)
     *          CALL *[RIP + IMM] --> MOV Temp(1), [Temp(0) + IMM]
     *          SimulateCall(Temp(1))
    */
    rules.emplace_back(
        And({
            OpIs(llvm::X86::CALL64m),
            UseReg(Reg(REG_PC))
        }),
        conv_unique<PatchGenerator>(
            GetPCOffset(Temp(0), Constant(0)),
            ModifyInstruction({
                SubstituteWithTemp(Reg(REG_PC), Temp(0)),
                SetOpcode(llvm::X86::MOV64rm),
                AddOperand(Operand(0), Temp(1))
            }),
            SimulateCall(Temp(1))
        )
    );

    /* Rule #3: Generic RIP patching.
     * Target:  Any instruction with RIP as operand, e.g. LEA RAX, [RIP + 1]
     * Patch:   Temp(0) := rip
     *          LEA RAX, [RIP + IMM] --> LEA RAX, [Temp(0) + IMM]
    */
    rules.emplace_back(
        UseReg(Reg(REG_PC)),
        conv_unique<PatchGenerator>(
            GetPCOffset(Temp(0), Constant(0)),
            ModifyInstruction({
                SubstituteWithTemp(Reg(REG_PC), Temp(0))
            })
        )
    );

    /* Rule #4: Simulate JMP to memory value.
     * Target:  JMP *MEM
     * Patch:   JMP *MEM --> MOV Temp(0), MEM
     *          DataBlock[Offset(RIP)] := Temp(0)
    */
    rules.emplace_back(
        Or({
            OpIs(llvm::X86::JMP32m),
            OpIs(llvm::X86::JMP64m)
        }),
        conv_unique<PatchGenerator>(
            ModifyInstruction({
#if defined(QBDI_ARCH_X86)
                SetOpcode(llvm::X86::MOV32rm),
#else
                SetOpcode(llvm::X86::MOV64rm),
#endif
                AddOperand(Operand(0), Temp(0))
            }),
            WriteTemp(Temp(0), Offset(Reg(REG_PC)))
        )
    );

    /* Rule #5: Simulate CALL to memory value.
     * Target:  CALL MEM
     * Patch:   CALL MEM --> MOV Temp(0), MEM
     *          SimulateCall(Temp(1))
    */
    rules.emplace_back(
        Or({
            OpIs(llvm::X86::CALL32m),
            OpIs(llvm::X86::CALL64m)
        }),
        conv_unique<PatchGenerator>(
            ModifyInstruction({
#if defined(QBDI_ARCH_X86)
                SetOpcode(llvm::X86::MOV32rm),
#else
                SetOpcode(llvm::X86::MOV64rm),
#endif
                AddOperand(Operand(0), Temp(0))
            }),
            SimulateCall(Temp(0))
        )
    );

    /* Rule #6: Simulate JMP to constant value.
     * Target:  JMP IMM
     * Patch:   Temp(0) := RIP + Operand(0)
     *          DataBlock[Offset(RIP)] := Temp(0)
    */
    rules.emplace_back(
        Or({
            OpIs(llvm::X86::JMP_1),
            OpIs(llvm::X86::JMP_2),
            OpIs(llvm::X86::JMP_4)
        }),
        conv_unique<PatchGenerator>(
            GetPCOffset(Temp(0), Operand(0)),
            WriteTemp(Temp(0), Offset(Reg(REG_PC)))
        )
    );

    /* Rule #7: Simulate JMP to register value.
     * Target:  JMP REG
     * Patch:   Temp(0) := Operand(0)
     *          DataBlock[Offset(RIP)] := Temp(0)
    */
    rules.emplace_back(
        Or({
            OpIs(llvm::X86::JMP32r),
            OpIs(llvm::X86::JMP64r)
        }),
        conv_unique<PatchGenerator>(
            GetOperand(Temp(0), Operand(0)),
            WriteTemp(Temp(0), Offset(Reg(REG_PC)))
        )
    );


    /* Rule #8: Simulate CALL to register value.
     * Target:  CALL REG
     * Patch:   Temp(0) := Operand(0)
     *          SimulateCall(Temp(0))
    */
    rules.emplace_back(
        Or({
            OpIs(llvm::X86::CALL32r),
            OpIs(llvm::X86::CALL64r),
        }),
        conv_unique<PatchGenerator>(
            GetOperand(Temp(0), Operand(0)),
            SimulateCall(Temp(0))
        )
    );


    /* Rule #9: Simulate Jcc IMM8.
     * Target:  Jcc IMM8
     * Patch:     Temp(0) := RIP + Operand(0)
     *         ---Jcc IMM8 --> Jcc END
     *         |  Temp(0) := RIP + Constant(0)
     *         -->END: DataBlock[Offset(RIP)] := Temp(0)
    */
    rules.emplace_back(
        OpIs(llvm::X86::JCC_1),
        conv_unique<PatchGenerator>(
            GetPCOffset(Temp(0), Operand(0)),
            ModifyInstruction({
#if defined(QBDI_ARCH_X86)
                SetOperand(Operand(0), Constant(6)) // Offset to jump the next load.
#else
                SetOperand(Operand(0), Constant(11)) // Offset to jump the next load.
#endif
            }),
            GetPCOffset(Temp(0), Constant(0)),
            WriteTemp(Temp(0), Offset(Reg(REG_PC)))
        )
    );

    /* Rule #10: Simulate Jcc IMM16.
     * Target:  Jcc IMM16
     * Patch:     Temp(0) := RIP + Operand(0)
     *         ---Jcc IMM16 --> Jcc END
     *         |  Temp(0) := RIP + Constant(0)
     *         -->END: DataBlock[Offset(RIP)] := Temp(0)
    */
    rules.emplace_back(
        OpIs(llvm::X86::JCC_2),
        conv_unique<PatchGenerator>(
            GetPCOffset(Temp(0), Operand(0)),
            ModifyInstruction({
#if defined(QBDI_ARCH_X86)
                SetOperand(Operand(0), Constant(7))
#else
                SetOperand(Operand(0), Constant(12))
#endif
            }),
            GetPCOffset(Temp(0), Constant(0)),
            WriteTemp(Temp(0), Offset(Reg(REG_PC)))
        )
    );

    /* Rule #11: Simulate Jcc IMM32.
     * Target:  Jcc IMM32
     * Patch:     Temp(0) := RIP + Operand(0)
     *         ---Jcc IMM32 --> Jcc END
     *         |  Temp(0) := RIP + Constant(0)
     *         -->END: DataBlock[Offset(RIP)] := Temp(0)
    */
    rules.emplace_back(
        OpIs(llvm::X86::JCC_4),
        conv_unique<PatchGenerator>(
            GetPCOffset(Temp(0), Operand(0)),
            ModifyInstruction({
#if defined(QBDI_ARCH_X86)
                SetOperand(Operand(0), Constant(9))
#else
                SetOperand(Operand(0), Constant(14))
#endif
            }),
            GetPCOffset(Temp(0), Constant(0)),
            WriteTemp(Temp(0), Offset(Reg(REG_PC)))
        )
    );

    /* Rule #12: Simulate CALL to constant offset.
     * Target:   CALL IMM
     * Patch:    Temp(0) := RIP + Operand(0)
     *           SimulateCall(Temp(0))
    */
    rules.emplace_back(
        Or({
            OpIs(llvm::X86::CALL64pcrel32),
            OpIs(llvm::X86::CALLpcrel16),
            OpIs(llvm::X86::CALLpcrel32),
        }),
        conv_unique<PatchGenerator>(
            GetPCOffset(Temp(0), Operand(0)),
            SimulateCall(Temp(0))
        )
    );

    /* Rule #13: Simulate return.
     * Target:   RET
     * Patch:    SimulateRet(Temp(0))
    */
    rules.emplace_back(
        Or({
            OpIs(llvm::X86::RETL),
            OpIs(llvm::X86::RETQ),
            OpIs(llvm::X86::RETW),
            OpIs(llvm::X86::RETIL),
            OpIs(llvm::X86::RETIQ),
            OpIs(llvm::X86::RETIW)
        }),
        conv_unique<PatchGenerator>(
            SimulateRet(Temp(0))
        )
    );

    /* Rule #14: Default rule for every other instructions.
     * Target:   *
     * Patch:    Output original unmodified instructions.
    */
    rules.emplace_back(True(), conv_unique<PatchGenerator>( ModifyInstruction({}) ));

    return rules;
}

// Patch allowing to terminate a basic block early by writing address into DataBlock[Offset(RIP)]
RelocatableInst::SharedPtrVec getTerminator(rword address) {
    RelocatableInst::SharedPtrVec terminator;

    append(terminator, SaveReg(Reg(0), Offset(Reg(0))));
    terminator.push_back(NoReloc(movri(Reg(0), address)));
    append(terminator, SaveReg(Reg(0), Offset(Reg(REG_PC))));
    append(terminator, LoadReg(Reg(0), Offset(Reg(0))));

    return terminator;
}

}
