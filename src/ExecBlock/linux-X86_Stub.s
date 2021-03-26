/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2021 Quarkslab
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

.globl __qbdi_runCodeBlock

__qbdi_runCodeBlock:
    mov eax, [esp+4]
    mov ecx, [esp+8]
    test ecx, 2;
    jz _skip_save_fpu;
    sub esp, 8;
    stmxcsr [esp];
    fnstcw [esp+4];
_skip_save_fpu:
    pushad;
    call eax;
    popad;
    test ecx, 2;
    jz _skip_restore_fpu;
    fninit;
    fldcw [esp+4];
    ldmxcsr [esp];
    add esp, 8;
_skip_restore_fpu:
    cld;
    ret;
    
