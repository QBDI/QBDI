/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2025 Quarkslab
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
.text

.global __qbdi_runCodeBlock

.align 4
__qbdi_runCodeBlock:
    // Function prologue: Save general-purpose registers and stack frame
    // Save x29 (FP) and x30 (LR) to stack and update stack pointer
    // Save x19-x28, which are callee-saved registers
    stp x29, x30, [sp, -16]!
    stp x27, x28, [sp, -16]!
    stp x25, x26, [sp, -16]!
    stp x23, x24, [sp, -16]!
    stp x21, x22, [sp, -16]!
    stp x19, x20, [sp, -16]!

    // Save floating-point registers (FPRs)
    // Save d8-d15, which are callee-saved registers
    stp d14, d15, [sp, -16]!
    stp d12, d13, [sp, -16]!
    stp d10, d11, [sp, -16]!
    stp d8, d9, [sp, -16]!
    
    // Call function
    // r0 (ARM32) corresponds to x0 (ARM64), with the function address stored in x0
    blr x0;

    // Function epilogue: Restore floating-point registers
    ldp d8, d9, [sp], 16
    ldp d10, d11, [sp], 16
    ldp d12, d13, [sp], 16
    ldp d14, d15, [sp], 16

    // Restore general-purpose registers and stack frames
    ldp x19, x20, [sp], 16
    ldp x21, x22, [sp], 16
    ldp x23, x24, [sp], 16
    ldp x25, x26, [sp], 16
    ldp x27, x28, [sp], 16
    ldp x29, x30, [sp], 16
    
    // return
    ret