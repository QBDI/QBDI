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

.globl __qbdi_runCodeBlockSSE
.globl __qbdi_runCodeBlockAVX

__qbdi_runCodeBlockSSE:
    mov rdx, rsp;
    sub rsp, 512;
    and rsp, -512;
    fxsave [rsp];
    push r15;
    push r14;
    push r13;
    push r12;
    push r11;
    push r10;
    push r9;
    push r8;
    push rdi;
    push rsi;
    push rdx;
    push rcx;
    push rbx;
    push rax;
    call rdi;
    pop rax;
    pop rbx;
    pop rcx;
    pop rdx;
    pop rsi;
    pop rdi;
    pop r8;
    pop r9;
    pop r10;
    pop r11;
    pop r12;
    pop r13;
    pop r14;
    pop r15;
    fxrstor [rsp];
    mov rsp, rdx;
    ret;
    
__qbdi_runCodeBlockAVX:
    mov rdx, rsp;
    sub rsp, 1024;
    and rsp, -1024;
    fxsave [rsp];
    vextractf128 [rsp+512], ymm0, 1;
    vextractf128 [rsp+528], ymm1, 1;
    vextractf128 [rsp+544], ymm2, 1;
    vextractf128 [rsp+560], ymm3, 1;
    vextractf128 [rsp+576], ymm4, 1;
    vextractf128 [rsp+592], ymm5, 1;
    vextractf128 [rsp+608], ymm6, 1;
    vextractf128 [rsp+624], ymm7, 1;
    vextractf128 [rsp+640], ymm8, 1;
    vextractf128 [rsp+656], ymm9, 1;
    vextractf128 [rsp+672], ymm10, 1;
    vextractf128 [rsp+688], ymm11, 1;
    vextractf128 [rsp+704], ymm12, 1;
    vextractf128 [rsp+720], ymm13, 1;
    vextractf128 [rsp+736], ymm14, 1;
    vextractf128 [rsp+752], ymm15, 1;
    push r15;
    push r14;
    push r13;
    push r12;
    push r11;
    push r10;
    push r9;
    push r8;
    push rdi;
    push rsi;
    push rdx;
    push rcx;
    push rbx;
    push rax;
    call rdi;
    pop rax;
    pop rbx;
    pop rcx;
    pop rdx;
    pop rsi;
    pop rdi;
    pop r8;
    pop r9;
    pop r10;
    pop r11;
    pop r12;
    pop r13;
    pop r14;
    pop r15;
    fxrstor [rsp];
    vinsertf128 ymm0, ymm0, [rsp+512], 1;
    vinsertf128 ymm1, ymm1, [rsp+528], 1;
    vinsertf128 ymm2, ymm2, [rsp+544], 1;
    vinsertf128 ymm3, ymm3, [rsp+560], 1;
    vinsertf128 ymm4, ymm4, [rsp+576], 1;
    vinsertf128 ymm5, ymm5, [rsp+592], 1;
    vinsertf128 ymm6, ymm6, [rsp+608], 1;
    vinsertf128 ymm7, ymm7, [rsp+624], 1;
    vinsertf128 ymm8, ymm8, [rsp+640], 1;
    vinsertf128 ymm9, ymm9, [rsp+656], 1;
    vinsertf128 ymm10, ymm10, [rsp+672], 1;
    vinsertf128 ymm11, ymm11, [rsp+688], 1;
    vinsertf128 ymm12, ymm12, [rsp+704], 1;
    vinsertf128 ymm13, ymm13, [rsp+720], 1;
    vinsertf128 ymm14, ymm14, [rsp+736], 1;
    vinsertf128 ymm15, ymm15, [rsp+752], 1;
    mov rsp, rdx;
    ret;
