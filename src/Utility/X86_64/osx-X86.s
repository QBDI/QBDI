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
.intel_syntax noprefix

.text

.private_extern __qbdi_asmStackSwitch

__qbdi_asmStackSwitch:
    push ebp;
    mov ebp, esp;

    mov esp, [ebp+12];
    and esp, ~0xf;
    sub esp, 8;
    push ebp;
    push [ebp+8];
    call [ebp+16];

    mov esp, ebp;
    pop ebp;
    ret;
