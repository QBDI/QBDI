; This file is part of QBDI.
;
; Copyright 2017 Quarkslab
;
; Licensed under the Apache License, Version 2.0 (the "License");
; you may not use this file except in compliance with the License.
; You may obtain a copy of the License at
;
;     http://www.apache.org/licenses/LICENSE-2.0
;
; Unless required by applicable law or agreed to in writing, software
; distributed under the License is distributed on an "AS IS" BASIS,
; WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; See the License for the specific language governing permissions and
; limitations under the License.

.386
.xmm
.model flat, C

_TEXT segment

PUBLIC qbdi_runCodeBlockSSE
PUBLIC qbdi_runCodeBlockAVX

.CODE

qbdi_runCodeBlockSSE PROC
    mov eax, [esp+4];
    mov edx, esp;
    sub esp, 512;
    and esp, -512;
    fxsave [esp];
    pushad;
    call eax;
    popad;
    fxrstor [esp];
    mov esp, edx;
    ret;
qbdi_runCodeBlockSSE ENDP

qbdi_runCodeBlockAVX PROC
    mov eax, [esp+4];
    mov edx, esp;
    sub esp, 1024;
    and esp, -1024;
    fxsave [esp];
    vextractf128 xmmword ptr [esp+512], ymm0, 1;
    vextractf128 xmmword ptr [esp+528], ymm1, 1;
    vextractf128 xmmword ptr [esp+544], ymm2, 1;
    vextractf128 xmmword ptr [esp+560], ymm3, 1;
    vextractf128 xmmword ptr [esp+576], ymm4, 1;
    vextractf128 xmmword ptr [esp+592], ymm5, 1;
    vextractf128 xmmword ptr [esp+608], ymm6, 1;
    vextractf128 xmmword ptr [esp+624], ymm7, 1;
    pushad;
    call eax;
    popad;
    fxrstor [esp];
    vinsertf128 ymm0, ymm0, xmmword ptr [esp+512], 1;
    vinsertf128 ymm1, ymm1, xmmword ptr [esp+528], 1;
    vinsertf128 ymm2, ymm2, xmmword ptr [esp+544], 1;
    vinsertf128 ymm3, ymm3, xmmword ptr [esp+560], 1;
    vinsertf128 ymm4, ymm4, xmmword ptr [esp+576], 1;
    vinsertf128 ymm5, ymm5, xmmword ptr [esp+592], 1;
    vinsertf128 ymm6, ymm6, xmmword ptr [esp+608], 1;
    vinsertf128 ymm7, ymm7, xmmword ptr [esp+624], 1;
    mov esp, edx;
    ret;
qbdi_runCodeBlockAVX ENDP

END
