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
    

    // Save host RBP, RSP
    append(prologue, SaveReg(Reg(REG_BP), Offset(offsetof(Context, hostState.rbp))));
    append(prologue, SaveReg(Reg(REG_SP), Offset(offsetof(Context, hostState.rsp))));
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
        prologue.push_back(Vinsertf128(llvm::X86::YMM8, Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm8)), 1));
        prologue.push_back(Vinsertf128(llvm::X86::YMM9, Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm9)), 1));
        prologue.push_back(Vinsertf128(llvm::X86::YMM10, Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm10)), 1));
        prologue.push_back(Vinsertf128(llvm::X86::YMM11, Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm11)), 1));
        prologue.push_back(Vinsertf128(llvm::X86::YMM12, Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm12)), 1));
        prologue.push_back(Vinsertf128(llvm::X86::YMM13, Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm13)), 1));
        prologue.push_back(Vinsertf128(llvm::X86::YMM14, Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm14)), 1));
        prologue.push_back(Vinsertf128(llvm::X86::YMM15, Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm15)), 1));
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
    prologue.push_back(Jmp64m(Offset(offsetof(Context, hostState.selector))));

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
        epilogue.push_back(Vextractf128(Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm8)), llvm::X86::YMM8, 1));
        epilogue.push_back(Vextractf128(Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm9)), llvm::X86::YMM9, 1));
        epilogue.push_back(Vextractf128(Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm10)), llvm::X86::YMM10, 1));
        epilogue.push_back(Vextractf128(Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm11)), llvm::X86::YMM11, 1));
        epilogue.push_back(Vextractf128(Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm12)), llvm::X86::YMM12, 1));
        epilogue.push_back(Vextractf128(Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm13)), llvm::X86::YMM13, 1));
        epilogue.push_back(Vextractf128(Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm14)), llvm::X86::YMM14, 1));
        epilogue.push_back(Vextractf128(Offset(offsetof(Context, fprState) + offsetof(FPRState, ymm15)), llvm::X86::YMM15, 1));
    }
#endif
    // Restore host RBP, RSP
    append(epilogue, LoadReg(Reg(REG_BP), Offset(offsetof(Context, hostState.rbp))));
    append(epilogue, LoadReg(Reg(REG_SP), Offset(offsetof(Context, hostState.rsp))));
    // Save EFLAGS
    epilogue.push_back(Pushf());
    epilogue.push_back(Popr(Reg(0)));
    append(epilogue, SaveReg(Reg(0), Offset(offsetof(Context, gprState.eflags))));
    // return to host
    epilogue.push_back(Ret());

    return epilogue;
}

PatchRule::SharedPtrVec getDefaultPatchRules() {
    PatchRule::SharedPtrVec rules;

    /* Rule #0: Avoid instrumenting instruction prefixes.
     * Target:  X86 prefixes (LOCK, REP and other REX prefixes).
     * Patch:   Output the unmodified MCInst but flag the patch as "do not instrument".
    */
    rules.push_back(
        PatchRule(
            Or({
                OpIs(llvm::X86::LOCK_PREFIX),
                OpIs(llvm::X86::REX64_PREFIX),
                OpIs(llvm::X86::REP_PREFIX),
                OpIs(llvm::X86::REPNE_PREFIX),
                OpIs(llvm::X86::DATA16_PREFIX),
                OpIs(llvm::X86::DATA32_PREFIX),
                OpIs(llvm::X86::CS_PREFIX),
                OpIs(llvm::X86::SS_PREFIX),
                OpIs(llvm::X86::DS_PREFIX),
                OpIs(llvm::X86::ES_PREFIX),
                OpIs(llvm::X86::FS_PREFIX),
                OpIs(llvm::X86::GS_PREFIX),
                OpIs(llvm::X86::XACQUIRE_PREFIX),
                OpIs(llvm::X86::XRELEASE_PREFIX)
            }),
            {
                DoNotInstrument(),
                ModifyInstruction({})
            }
        )
    );

    /* Rule #1: Simulate jmp to memory value using RIP addressing.
     * Target:  JMP *[RIP + IMM]
     * Patch:   Temp(0) := RIP + Constant(0)
     *          JMP *[RIP + IMM] --> MOV Temp(1), [Temp(0) + IMM]
     *          DataBlock[Offset(RIP)] := Temp(1)
    */
    rules.push_back(
        PatchRule(
            And({
                OpIs(llvm::X86::JMP64m),
                UseReg(Reg(REG_PC))
            }),
            {
                GetPCOffset(Temp(0), Constant(0)),
                ModifyInstruction({
                    SubstituteWithTemp(Reg(REG_PC), Temp(0)),
                    SetOpcode(llvm::X86::MOV64rm),
                    AddOperand(Operand(0), Temp(1))
                }),
                WriteTemp(Temp(1), Offset(Reg(REG_PC)))
            }
        )
    );

    /* Rule #2: Simulate call to memory value using RIP addressing.
     * Target:  CALL *[RIP + IMM]
     * Patch:   Temp(0) := RIP + Constant(0)
     *          CALL *[RIP + IMM] --> MOV Temp(1), [Temp(0) + IMM]
     *          SimulateCall(Temp(1))
    */
    rules.push_back(
        PatchRule(
            And({
                OpIs(llvm::X86::CALL64m),
                UseReg(Reg(REG_PC))
            }),
            {
                GetPCOffset(Temp(0), Constant(0)),
                ModifyInstruction({
                    SubstituteWithTemp(Reg(REG_PC), Temp(0)),
                    SetOpcode(llvm::X86::MOV64rm),
                    AddOperand(Operand(0), Temp(1))
                }),
                SimulateCall(Temp(1))
            }
        )
    );

    /* Rule #3: Generic RIP patching.
     * Target:  Any instruction with RIP as operand, e.g. LEA RAX, [RIP + 1]
     * Patch:   Temp(0) := rip
     *          LEA RAX, [RIP + IMM] --> LEA RAX, [Temp(0) + IMM]
    */
    rules.push_back(
        PatchRule(
            UseReg(Reg(REG_PC)),
            {
                GetPCOffset(Temp(0), Constant(0)),
                ModifyInstruction({
                    SubstituteWithTemp(Reg(REG_PC), Temp(0))
                })
            }
        )
    );

    /* Rule #4: Simulate JMP to memory value.
     * Target:  JMP *MEM
     * Patch:   JMP *MEM --> MOV Temp(0), MEM
     *          DataBlock[Offset(RIP)] := Temp(0)
    */
    rules.push_back(
        PatchRule(
            OpIs(llvm::X86::JMP64m),
            {
                ModifyInstruction({
                    SetOpcode(llvm::X86::MOV64rm),
                    AddOperand(Operand(0), Temp(0))
                }),
                WriteTemp(Temp(0), Offset(Reg(REG_PC)))
            }
        )
    );

    /* Rule #5: Simulate CALL to memory value.
     * Target:  CALL MEM
     * Patch:   CALL MEM --> MOV Temp(0), MEM
     *          SimulateCall(Temp(1))
    */
    rules.push_back(
        PatchRule(
            OpIs(llvm::X86::CALL64m),
            {
                ModifyInstruction({
                    SetOpcode(llvm::X86::MOV64rm),
                    AddOperand(Operand(0), Temp(0))
                }),
                SimulateCall(Temp(0))
            }
        )
    );

    /* Rule #6: Simulate JMP to constant value.
     * Target:  JMP IMM
     * Patch:   Temp(0) := RIP + Operand(0)
     *          DataBlock[Offset(RIP)] := Temp(0)
    */
    rules.push_back(
        PatchRule(
            Or({
                OpIs(llvm::X86::JMP_1),
                OpIs(llvm::X86::JMP_2),
                OpIs(llvm::X86::JMP_4)
            }),
            {
                GetPCOffset(Temp(0), Operand(0)),
                WriteTemp(Temp(0), Offset(Reg(REG_PC)))
            }
        )
    );

    /* Rule #7: Simulate JMP to register value.
     * Target:  JMP REG
     * Patch:   Temp(0) := Operand(0)
     *          DataBlock[Offset(RIP)] := Temp(0)
    */
    rules.push_back(
        PatchRule(
            OpIs(llvm::X86::JMP64r),
            {
                GetOperand(Temp(0), Operand(0)),
                WriteTemp(Temp(0), Offset(Reg(REG_PC)))
            }
        )
    );


    /* Rule #8: Simulate CALL to register value.
     * Target:  CALL REG
     * Patch:   Temp(0) := Operand(0)
     *          SimulateCall(Temp(0))
    */
    rules.push_back(
        PatchRule(
            OpIs(llvm::X86::CALL64r),
            {
                GetOperand(Temp(0), Operand(0)),
                SimulateCall(Temp(0))
            }
        )
    );


    /* Rule #9: Simulate Jcc IMM8.
     * Target:  Jcc IMM8
     * Patch:     Temp(0) := RIP + Operand(0)
     *         ---Jcc IMM8 --> Jcc END
     *         |  Temp(0) := RIP + Constant(0)
     *         -->END: DataBlock[Offset(RIP)] := Temp(0)
    */
    rules.push_back(
        PatchRule(
            Or({
                OpIs(llvm::X86::JNE_1),
                OpIs(llvm::X86::JE_1),
                OpIs(llvm::X86::JG_1),
                OpIs(llvm::X86::JGE_1),
                OpIs(llvm::X86::JA_1),
                OpIs(llvm::X86::JAE_1),
                OpIs(llvm::X86::JL_1),
                OpIs(llvm::X86::JLE_1),
                OpIs(llvm::X86::JB_1),
                OpIs(llvm::X86::JBE_1),
                OpIs(llvm::X86::JP_1),
                OpIs(llvm::X86::JNP_1),
                OpIs(llvm::X86::JO_1),
                OpIs(llvm::X86::JNO_1),
                OpIs(llvm::X86::JS_1),
                OpIs(llvm::X86::JNS_1)
            }),
            {
                GetPCOffset(Temp(0), Operand(0)),
                ModifyInstruction({
                    SetOperand(Operand(0), Constant(11)) // Offset to jump the next load.
                }),
                GetPCOffset(Temp(0), Constant(0)),
                WriteTemp(Temp(0), Offset(Reg(REG_PC)))
            }
        )
    );

    /* Rule #10: Simulate Jcc IMM16.
     * Target:  Jcc IMM16
     * Patch:     Temp(0) := RIP + Operand(0)
     *         ---Jcc IMM16 --> Jcc END
     *         |  Temp(0) := RIP + Constant(0)
     *         -->END: DataBlock[Offset(RIP)] := Temp(0)
    */
    rules.push_back(
        PatchRule(
            Or({
                OpIs(llvm::X86::JNE_2),
                OpIs(llvm::X86::JE_2),
                OpIs(llvm::X86::JG_2),
                OpIs(llvm::X86::JGE_2),
                OpIs(llvm::X86::JA_2),
                OpIs(llvm::X86::JAE_2),
                OpIs(llvm::X86::JL_2),
                OpIs(llvm::X86::JLE_2),
                OpIs(llvm::X86::JB_2),
                OpIs(llvm::X86::JBE_2),
                OpIs(llvm::X86::JP_2),
                OpIs(llvm::X86::JNP_2),
                OpIs(llvm::X86::JO_2),
                OpIs(llvm::X86::JNO_2),
                OpIs(llvm::X86::JS_2),
                OpIs(llvm::X86::JNS_2)
            }),
            {
                GetPCOffset(Temp(0), Operand(0)),
                ModifyInstruction({
                    SetOperand(Operand(0), Constant(12))
                }),
                GetPCOffset(Temp(0), Constant(0)),
                WriteTemp(Temp(0), Offset(Reg(REG_PC)))
            }
        )
    );

    /* Rule #11: Simulate Jcc IMM32.
     * Target:  Jcc IMM32
     * Patch:     Temp(0) := RIP + Operand(0)
     *         ---Jcc IMM32 --> Jcc END
     *         |  Temp(0) := RIP + Constant(0)
     *         -->END: DataBlock[Offset(RIP)] := Temp(0)
    */
    rules.push_back(
        PatchRule(
            Or({
                OpIs(llvm::X86::JNE_4),
                OpIs(llvm::X86::JE_4),
                OpIs(llvm::X86::JG_4),
                OpIs(llvm::X86::JGE_4),
                OpIs(llvm::X86::JA_4),
                OpIs(llvm::X86::JAE_4),
                OpIs(llvm::X86::JL_4),
                OpIs(llvm::X86::JLE_4),
                OpIs(llvm::X86::JB_4),
                OpIs(llvm::X86::JBE_4),
                OpIs(llvm::X86::JP_4),
                OpIs(llvm::X86::JNP_4),
                OpIs(llvm::X86::JO_4),
                OpIs(llvm::X86::JNO_4),
                OpIs(llvm::X86::JS_4),
                OpIs(llvm::X86::JNS_4)
            }),
            {
                GetPCOffset(Temp(0), Operand(0)),
                ModifyInstruction({
                    SetOperand(Operand(0), Constant(14))
                }),
                GetPCOffset(Temp(0), Constant(0)),
                WriteTemp(Temp(0), Offset(Reg(REG_PC)))
            }
        )
    );

    /* Rule #12: Simulate CALL to constant offset.
     * Target:   CALL IMM
     * Patch:    Temp(0) := RIP + Operand(0)
     *           SimulateCall(Temp(0))
    */
    rules.push_back(
        PatchRule(
            Or({
                OpIs(llvm::X86::CALL64pcrel32),
                OpIs(llvm::X86::CALLpcrel16),
                OpIs(llvm::X86::CALLpcrel32),
            }),
            {
                GetPCOffset(Temp(0), Operand(0)),
                SimulateCall(Temp(0))
            }
        )
    );

    /* Rule #13: Simulate return.
     * Target:   RET
     * Patch:    SimulateRet(Temp(0))
    */
    rules.push_back(
        PatchRule(
            Or({
                OpIs(llvm::X86::RETQ),
                OpIs(llvm::X86::RETIQ),
                OpIs(llvm::X86::RETW),
                OpIs(llvm::X86::RETIW)
            }),
            {
                SimulateRet(Temp(0))
            }
        )
    );

    /* Rule #14: Default rule for every other instructions.
     * Target:   *
     * Patch:    Output original unmodified instructions.
    */
    rules.push_back(PatchRule(True(), {ModifyInstruction({})}));

    return rules;
}

// Patch allowing to terminate a basic block early by writing address into DataBlock[Offset(RIP)]
RelocatableInst::SharedPtrVec getTerminator(rword address) {
    RelocatableInst::SharedPtrVec terminator;

    append(terminator, SaveReg(Reg(0), Offset(Reg(0))));
    terminator.push_back(NoReloc(mov64ri(Reg(0), address)));
    append(terminator, SaveReg(Reg(0), Offset(Reg(REG_PC))));
    append(terminator, LoadReg(Reg(0), Offset(Reg(0))));

    return terminator;
}

}
