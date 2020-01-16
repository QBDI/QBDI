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
.intel_syntax noprefix

.text

.globl __qbdi_runCodeBlock

__qbdi_runCodeBlock:
    mov rdx, rsp;
    sub rsp, 16;
    stmxcsr [rsp];
    fnstcw [rsp+8];
    push r15;
    push r14;
    push r13;
    push r12;
    push rsi;
    push rdx;
    push rbx;
    call rdi;
    pop rbx;
    pop rdx;
    pop rsi;
    pop r12;
    pop r13;
    pop r14;
    pop r15;
    fninit;
    fldcw [rsp+8];
    ldmxcsr [rsp];
    emms;
    mov rsp, rdx;
    cld;
    ret;
