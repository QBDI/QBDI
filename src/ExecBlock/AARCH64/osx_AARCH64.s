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

.private_extern __qbdi_runCodeBlock

.align 8
__qbdi_runCodeBlock:

  # Support BTI
  # ===========
  bti c;

  # Push GPR
  # ========
  # see https://github.com/ARM-software/abi-aa/blob/master/aapcs64/aapcs64.rst#611general-purpose-registers
  # OSX specify that x18 should not be used, we don't restore or backup it
  # see https://developer.apple.com/documentation/xcode/writing-arm64-code-for-apple-platforms
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
  stp x19, x20, [sp, #-16]!;
  stp x21, x22, [sp, #-16]!;
  stp x23, x24, [sp, #-16]!;
  stp x25, x26, [sp, #-16]!;
  stp x27, x28, [sp, #-16]!;
  stp x29, x30, [sp, #-16]!;


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
  ldp x29, x30, [sp], 16;
  ldp x27, x28, [sp], 16;
  ldp x25, x26, [sp], 16;
  ldp x23, x24, [sp], 16;
  ldp x21, x22, [sp], 16;
  ldp x19, x20, [sp], 16;
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

