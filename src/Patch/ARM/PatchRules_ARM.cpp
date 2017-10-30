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
#include "Patch/ARM/PatchRules_ARM.h"
#include "Patch/ARM/RelocatableInst_ARM.h"
#include "Patch/ARM/Layer2_ARM.h"

namespace QBDI {

RelocatableInst::SharedPtrVec getExecBlockPrologue() {
    RelocatableInst::SharedPtrVec prologue;

    // save return address on host stack
    prologue.push_back(Pushr(Reg(REG_LR)));
    // save host fp and sp
    append(prologue, SaveReg(Reg(REG_SP), Offset(offsetof(Context, hostState.sp))));
    // Move sp at the beginning of the data block to solve memory addressing range problems
    // This instruction needs to be EXACTLY HERE for relative addressing alignement reasons
    prologue.push_back(Adr(Reg(REG_SP), 4080));
    append(prologue, SaveReg(Reg(REG_BP), Offset(offsetof(Context, hostState.fp))));
    // Restore FPR
    for(unsigned int i = 0; i < QBDI_NUM_FPR; i++)
        prologue.push_back(
            Vldrs(llvm::ARM::S0 + i, Reg(REG_SP), offsetof(Context, fprState.s) + i*sizeof(float))
        );
    // Restore CPSR
    prologue.push_back(Ldr(Reg(0), Reg(REG_SP), offsetof(Context, gprState.cpsr)));
    prologue.push_back(Msr(Reg(0)));
    // Restore GPR
    for(unsigned int i = 0; i < NUM_GPR-1; i++)
        append(prologue, LoadReg(Reg(i), Offset(Reg(i))));
    // Jump selector
    prologue.push_back(
        Ldr(Reg(REG_PC), Offset(offsetof(Context, hostState.selector)))
    );

    return prologue;
}

RelocatableInst::SharedPtrVec getExecBlockEpilogue() {
    RelocatableInst::SharedPtrVec epilogue;

    // Save guest state
    for(unsigned int i = 0; i < NUM_GPR-1; i++)
        append(epilogue, SaveReg(Reg(i), Offset(Reg(i))));
    // Move sp at the beginning of the data block to solve memory addressing range problems
    epilogue.push_back(Adr(Reg(REG_SP), Offset(0)));
    // Save FPR
    for(unsigned int i = 0; i < QBDI_NUM_FPR; i++)
        epilogue.push_back(
            Vstrs(llvm::ARM::S0 + i, Reg(REG_SP), offsetof(Context, fprState.s) + i*sizeof(float))
        );
    // Save CPSR
    epilogue.push_back(Mrs(Reg(0)));
    epilogue.push_back(Str(Reg(0), Offset(offsetof(Context, gprState.cpsr))));
    // Restore host RBP, RSP
    append(epilogue, LoadReg(Reg(REG_BP), Offset(offsetof(Context, hostState.fp))));
    append(epilogue, LoadReg(Reg(REG_SP), Offset(offsetof(Context, hostState.sp))));
    // return to host
    epilogue.push_back(Popr(Reg(REG_PC)));

    return epilogue;
}

PatchRule::SharedPtrVec getDefaultPatchRules() {
    PatchRule::SharedPtrVec rules;

    /* Rule #0: Simulating BX instructions.
     * Target:  BX REG
     * Patch:   Temp(0) := Operand(0)
     *          DataOffset[Offset(PC)] := Temp(0)
    */
    rules.push_back(
        PatchRule(
            Or({
                OpIs(llvm::ARM::BX),
                OpIs(llvm::ARM::BX_pred)
            }),
            {
                GetOperand(Temp(0), Operand(0)),
                WriteTemp(Temp(0), Offset(Reg(REG_PC)))
            }
        )
    );

    /* Rule #1: Simulating BLX instructions.
     * Target:  BLX REG
     * Patch:   Temp(0) := Operand(0)
     *          DataOffset[Offset(PC)] := Temp(0)
     *          SimulateLink(Temp(0))
    */
    rules.push_back(
        PatchRule(
            Or({
                OpIs(llvm::ARM::BLX),
                OpIs(llvm::ARM::BLX_pred),
            }),
            {
                GetOperand(Temp(0), Operand(0)),
                WriteTemp(Temp(0), Offset(Reg(REG_PC))),
                SimulateLink(Temp(0))
            }
        )
    );

    /* Rule #2: Simulating BL and BLX immediate instructions.
     * Target:  BL(X) IMM
     * Patch:   Temp(0) := PC + Operand(0)
     *          DataOffset[Offset(PC)] := Temp(0)
     *          SimulateLink(Temp(0))
    */
    rules.push_back(
        PatchRule(
            Or({
                OpIs(llvm::ARM::BL),
                OpIs(llvm::ARM::BL_pred),
                OpIs(llvm::ARM::BLXi)
            }),
            {
                GetPCOffset(Temp(0), Operand(0)),
                WriteTemp(Temp(0), Offset(Reg(REG_PC))),
                SimulateLink(Temp(0))
            }
        )
    );

    /* Rule #3: Simulating BX_RET and MOVPCLR with conditional flag.
     * Target:  BXcc LR | MOVcc PC, LR
     * Patch:   Temp(0) := PC + Constant(-4) # next instruction address
     *          (BXcc LR | MOVcc PC, LR) --> MOVcc Temp(0), LR
     *          DataOffset[Offset(PC)] := Temp(0)
    */
    rules.push_back(
        PatchRule(
            Or({
                OpIs(llvm::ARM::MOVPCLR),
                OpIs(llvm::ARM::BX_RET)
            }),
            {
                GetPCOffset(Temp(0), Constant(-4)),
                ModifyInstruction({
                    SetOpcode(llvm::ARM::MOVr),
                    AddOperand(Operand(0), Temp(0)),
                    AddOperand(Operand(1), Reg(REG_LR)),
                    AddOperand(Operand(4), Constant(0)),
                }),
                WriteTemp(Temp(0), Offset(Reg(REG_PC)))
            }
        )
    );

    /* Rule #4: Simulating B with conditional flag.
     * Target:  Bcc IMM
     * Patch:     Temp(0) := PC + Operand(0)
     *         ---Bcc IMM --> Bcc END
     *         |  Temp(0) := PC + Constant(-4) # next instruction
     *         -->END: DataOffset[Offset(PC)] := Temp(0)
    */
    rules.push_back(
        PatchRule(
            OpIs(llvm::ARM::Bcc),
            {
                GetPCOffset(Temp(0), Operand(0)),
                ModifyInstruction({
                    SetOperand(Operand(0), Constant(0))
                }),
                GetPCOffset(Temp(0), Constant(-4)),
                WriteTemp(Temp(0), Offset(Reg(REG_PC)))
            }
        )
    );

    /* Rule #5: Simulating LDMIA using pc in the reg list.
     * Target:  LDMIA {REG1, ..., REGN, pc}
     * Patch:   LDMIA {REG1, ..., REGN, pc} --> LDMIA {REG1, ..., REGN}
     *          SimulateRet(Temp(0))
    */
    rules.push_back(
        PatchRule(
            And({
                OpIs(llvm::ARM::LDMIA_UPD),
                UseReg(Reg(REG_PC)),
            }),
            {
                ModifyInstruction({
                    RemoveOperand(Reg(REG_PC))
                }),
                SimulatePopPC(Temp(0))
            }
        )
    );

    /* Rule #6: Generic PC modification patch with potential conditional code.
     * Target:  Anything that has PC as destination operand. E.g. ADDcc PC, PC, R1
     * Patch:   Temp(0) := PC + Constant(0)  # to substitute read values of PC
     *          Temp(1) := PC + Constant(-4) # to substitute written values of PC
     *          ADDcc PC, PC, R1 --> ADDcc Temp(1), Temp(0), R1
     *          DataOffset[Offset(PC)] := Temp(1)
    */
    rules.push_back(
        PatchRule(
            RegIs(Operand(0), Reg(REG_PC)),
            {
                GetPCOffset(Temp(0), Constant(0)),
                GetPCOffset(Temp(1), Constant(-4)),
                ModifyInstruction({
                    SubstituteWithTemp(Reg(REG_PC), Temp(0)),
                    SetOperand(Operand(0), Temp(1))
                }),
                WriteTemp(Temp(1), Offset(Reg(REG_PC)))
            }
        )
    );

    /* Rule #7: Generic PC utilization patch
     * Target:  Anything that uses PC. E.g. ADDcc R2, PC, R1
     * Patch:   Temp(0) := PC + Constant(0)  # to substitute read values of PC
     *          ADDcc R2, PC, R1 --> ADDcc R2, Temp(0), R1
    */
    rules.push_back(
        PatchRule(
            UseReg(Reg(REG_PC)),
            {
                GetPCOffset(Temp(0), Constant(0)),
                ModifyInstruction({
                    SubstituteWithTemp(Reg(REG_PC), Temp(0))
                }),
            }
        )
    );

    /* Rule #8: Default rule for every other instructions.
     * Target:   *
     * Patch:    Output original unmodified instructions.
    */
    rules.push_back(PatchRule(True(), {ModifyInstruction({})}));

    return rules;
}

// Patch allowing to terminate a basic block early by writing address into DataBlock[Offset(PC)]
RelocatableInst::SharedPtrVec getTerminator(rword address) {
    RelocatableInst::SharedPtrVec terminator;
    
    append(terminator, SaveReg(Reg(2), Offset(Reg(2))));
    terminator.push_back(Ldr(Reg(2), Constant(address)));
    append(terminator, SaveReg(Reg(2), Offset(Reg(REG_PC))));
    append(terminator, LoadReg(Reg(2), Offset(Reg(2))));

    return terminator;
}

}
