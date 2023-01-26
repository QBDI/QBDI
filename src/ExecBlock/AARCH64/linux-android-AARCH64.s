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

.hidden __qbdi_runCodeBlock
.globl  __qbdi_runCodeBlock

.align 8
__qbdi_runCodeBlock:

  # Support BTI
  # ===========
  bti c;

  # Push GPR
  # ========
  # see https://github.com/ARM-software/abi-aa/blob/master/aapcs64/aapcs64.rst#611general-purpose-registers
    # caller-saved register
    #stp x0,  x1,  [sp, #-16]!;
    #stp x2,  x3,  [sp, #-16]!;
    #stp x4,  x5,  [sp, #-16]!;
    #stp x6,  x7,  [sp, #-16]!;
    #stp x8,  x9,  [sp, #-16]!;
    #stp x10, x11, [sp, #-16]!;
    #stp x12, x13, [sp, #-16]!;
    #stp x14, x15, [sp, #-16]!;
    #stp x16, x17, [sp, #-16]!;
  # callee-saved register
  stp x18, x19, [sp, #-16]!;
  stp x20, x21, [sp, #-16]!;
  stp x22, x23, [sp, #-16]!;
  stp x24, x25, [sp, #-16]!;
  stp x26, x27, [sp, #-16]!;
  stp x28, x29, [sp, #-16]!;
  stp x30, x0,  [sp, #-16]!;


  # Save SIMD registers
  # ===================
  # see https://github.com/ARM-software/abi-aa/blob/master/aapcs64/aapcs64.rst#612simd-and-floating-point-registers
  # only need to saved the bottom 64 bits of v8 to v15

  sub sp, sp, #64;
  mov x8, sp;

  st1 {v8.1d-v11.1d},  [x8], #32;
  st1 {v12.1d-v15.1d}, [x8], #32;

  # Call ExecBlock prologue
  # =======================
  blr x0;
  # =======================

  # Load SIMD registers
  # ===================
  ld1     {v8.1d-v11.1d},  [sp], #32;
  ld1     {v12.1d-v15.1d}, [sp], #32;

  # Pop GPR
  # =======
  ldp x30, x0,  [sp], 16;
  ldp x28, x29, [sp], 16;
  ldp x26, x27, [sp], 16;
  ldp x24, x25, [sp], 16;
  ldp x22, x23, [sp], 16;
  ldp x20, x21, [sp], 16;
  ldp x18, x19, [sp], 16;
    # caller-saved register
    #ldp x16, x17, [sp], 16;
    #ldp x14, x15, [sp], 16;
    #ldp x12, x13, [sp], 16;
    #ldp x10, x11, [sp], 16;
    #ldp x8,  x9,  [sp], 16;
    #ldp x6,  x7,  [sp], 16;
    #ldp x4,  x5,  [sp], 16;
    #ldp x2,  x3,  [sp], 16;
    #ldp x0,  x1,  [sp], 16;

  ret;

# mark stack as no-exec
.section	.note.GNU-stack,"",@progbits

