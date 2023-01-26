/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2023 Quarkslab
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

.code 32
.section .text
.align 4
.globl __qbdi_runCodeBlock

.type __qbdi_runCodeBlock, %function
__qbdi_runCodeBlock:
    // save GPRs
    push {r0-r12,lr};
    // save FPRs
    vpush {s0-s7}
    vpush {s16-s31}
    // call block
    blx r0;
    // restore FPRs
    vpop    {s16-s31}
    vpop    {s0-s7}
    // restore GPR and ret
    pop {r0-r12,pc};
