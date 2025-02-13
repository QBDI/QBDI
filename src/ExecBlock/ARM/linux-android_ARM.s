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

;#if defined(__ARM_PCS_VFP)
;.fpu vfp
.fpu neon

;#endif

.code 32
.section .text
.align 4
.hidden __qbdi_runCodeBlock
.globl  __qbdi_runCodeBlock
.type __qbdi_runCodeBlock, %function

__qbdi_runCodeBlock:
    // save GPRs
    push {r0-r12,lr};

;#if defined(__ARM_PCS_VFP)
    // save FPRs
    // https://github.com/ARM-software/abi-aa/blob/main/aapcs32/aapcs32.rst#6121vfp-register-usage-conventions
    // save only s16-s31 (also named d8-d15)
    vpush {s16-s31}
    //vmrs r2, FPSCR
    //push {r2}
    //vmrs r2, FPEXC
    //push {r2}
;#endif

    // call block
    blx r0;

;#if defined(__ARM_PCS_VFP)
    // restore FPRs
    // https://github.com/ARM-software/abi-aa/blob/main/aapcs32/aapcs32.rst#6121vfp-register-usage-conventions
    //pop {r2}
    //vmsr    FPEXC, r2
    //pop {r2}
    //vmsr    FPSCR, r2
    vpop    {s16-s31}
;#endif
    // restore GPR and ret
    pop {r0-r12,pc};

# mark stack as no-exec
.section	.note.GNU-stack
