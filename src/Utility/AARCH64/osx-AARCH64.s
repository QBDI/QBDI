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

.private_extern __qbdi_asmStackSwitch

.align 8
__qbdi_asmStackSwitch:
  bti c;
  stp x29, x30, [sp, #-16]!;
  mov x29, sp;

  bic	x1, x1, #15;
  mov sp, x1;
  mov x1, x29;
  blr x2;
  mov sp, x29;
  ldp x29, x30, [sp], 16;
  ret;
