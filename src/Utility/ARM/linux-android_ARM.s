/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2024 Quarkslab
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

;#if defined(__ARM_PCS_VFP)
;.fpu vfp
.fpu neon

;#endif

.code 32
.section .text
.align 4
.hidden __qbdi_asmStackSwitch
.globl  __qbdi_asmStackSwitch
.type __qbdi_asmStackSwitch, %function

__qbdi_asmStackSwitch:
    push {fp, lr};
    mov fp, sp;

    bic	r1, r1, #15;
    add r1, r1, #12;
    mov sp, r1;

    push {r3};
    mov r1, fp;
    blx r2;

    mov sp, fp;
    pop {fp, pc};

# mark stack as no-exec
.section	.note.GNU-stack
