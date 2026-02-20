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
.intel_syntax noprefix

.text

.private_extern __qbdi_runCodeBlock

__qbdi_runCodeBlock:
    test rsi, 2;
    jz _skip_save_fpu;
    sub rsp, 8;
    stmxcsr [rsp];
    fnstcw [rsp+4];
_skip_save_fpu:
    push r15;
    push r14;
    push r13;
    push r12;
    push rbp;
    push rsi;
    push rbx;
    call rdi;
    pop rbx;
    pop rsi;
    pop rbp;
    pop r12;
    pop r13;
    pop r14;
    pop r15;
    test rsi, 2;
    jz _skip_restore_fpu;
    fninit;
    fldcw [rsp+4];
    ldmxcsr [rsp];
    add rsp, 8;
_skip_restore_fpu:
    cld;
    ret;
