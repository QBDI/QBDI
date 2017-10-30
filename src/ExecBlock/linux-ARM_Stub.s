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
.text

.global __qbdi_runCodeBlock

#if defined(__ARM_PCS_VFP)
.fpu vfp
#endif

.align 4
__qbdi_runCodeBlock:
    // save GPRs
    push {r0-r12,lr};
#if defined(__ARM_PCS_VFP)
    // save FPRs
    vpush {s0-s7}
    vpush {s16-s31}
    //vmrs r1, FPSCR
    //push {r1}
    //vmrs r1, FPEXC
    //push {r1}
#endif
    // call block
    blx r0;
#if defined(__ARM_PCS_VFP)
    // restore FPRs
    //pop {r2}
    //vmsr    FPEXC, r2
    //pop {r2}
    //vmsr    FPSCR, r2
    vpop    {s16-s31}
    vpop    {s0-s7}
#endif
    // restore GPR and ret
    pop {r0-r12,pc};
