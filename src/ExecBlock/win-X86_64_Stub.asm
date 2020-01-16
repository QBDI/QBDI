; This file is part of QBDI.
;
; Copyright 2017 Quarkslab
;
; Licensed under the Apache License, Version 2.0 (the "License");
; you may not use this file except in compliance with the License.
; You may obtain a copy of the License at
;
;     http://www.apache.org/licenses/LICENSE-2.0
;
; Unless required by applicable law or agreed to in writing, software
; distributed under the License is distributed on an "AS IS" BASIS,
; WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; See the License for the specific language governing permissions and
; limitations under the License.

; "The registers RBX, RBP, RDI, RSI, RSP, R12, R13, R14, R15, and XMM6-15 are considered nonvolatile"
; https://docs.microsoft.com/en-us/cpp/build/x64-calling-convention?view=vs-2019#callercallee-saved-registers

PUBLIC qbdi_runCodeBlock

.CODE

qbdi_runCodeBlock PROC
    mov rdx, rsp;
    sub rsp, 256;
    and rsp, -256;
    movaps [rsp], xmm6;
    movaps [rsp+16], xmm7;
    movaps [rsp+32], xmm8;
    movaps [rsp+48], xmm9;
    movaps [rsp+64], xmm10;
    movaps [rsp+80], xmm11;
    movaps [rsp+96], xmm12;
    movaps [rsp+112], xmm13;
    movaps [rsp+128], xmm14;
    movaps [rsp+144], xmm15;
    stmxcsr [rsp+160];
    fnstcw [rsp+168];
    push r15;
    push r14;
    push r13;
    push r12;
    push rdi;
    push rsi;
    push rdx;
    push rbx;
    call rcx;
    pop rbx;
    pop rdx;
    pop rsi;
    pop rdi;
    pop r12;
    pop r13;
    pop r14;
    pop r15;
    fninit;
    fldcw [rsp+168];
    ldmxcsr [rsp+160];
    emms;
    movaps xmm6, [rsp];
    movaps xmm7, [rsp+16];
    movaps xmm8, [rsp+32];
    movaps xmm9, [rsp+48];
    movaps xmm10, [rsp+64];
    movaps xmm11, [rsp+80];
    movaps xmm12, [rsp+96];
    movaps xmm13, [rsp+112];
    movaps xmm14, [rsp+128];
    movaps xmm15, [rsp+144];
    cld;
    mov rsp, rdx;
    ret;
qbdi_runCodeBlock ENDP

END
