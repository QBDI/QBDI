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
.intel_syntax noprefix

.text

.globl __qbdi_runCodeBlockSSE
.globl __qbdi_runCodeBlockAVX

__qbdi_runCodeBlockSSE:
    mov eax, [esp+4]
    mov edx, esp;
    sub esp, 512;
    and esp, -512;
    fxsave [esp];
    pusha;
    call eax;
    popa;
    fxrstor [esp];
    mov esp, edx;
    ret;
    
__qbdi_runCodeBlockAVX:
    mov eax, [esp+4]
    mov edx, esp;
    sub esp, 1024;
    and esp, -1024;
    fxsave [esp];
    vextractf128 [esp+512], ymm0, 1;
    vextractf128 [esp+528], ymm1, 1;
    vextractf128 [esp+544], ymm2, 1;
    vextractf128 [esp+560], ymm3, 1;
    vextractf128 [esp+576], ymm4, 1;
    vextractf128 [esp+592], ymm5, 1;
    vextractf128 [esp+608], ymm6, 1;
    vextractf128 [esp+624], ymm7, 1;
    pusha;
    call eax;
    popa;
    fxrstor [esp];
    vinsertf128 ymm0, ymm0, [esp+512], 1;
    vinsertf128 ymm1, ymm1, [esp+528], 1;
    vinsertf128 ymm2, ymm2, [esp+544], 1;
    vinsertf128 ymm3, ymm3, [esp+560], 1;
    vinsertf128 ymm4, ymm4, [esp+576], 1;
    vinsertf128 ymm5, ymm5, [esp+592], 1;
    vinsertf128 ymm6, ymm6, [esp+608], 1;
    vinsertf128 ymm7, ymm7, [esp+624], 1;
    mov esp, edx;
    ret;
