; This file is part of QBDI.
;
; Copyright 2017 - 2023 Quarkslab
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

PUBLIC qbdi_asmStackSwitch

.CODE

qbdi_asmStackSwitch PROC
    push ebp;
    mov ebp, esp;
    mov esp, dword ptr [ebp+12];
    and esp, -16;
    sub esp, 8;
    push ebp;
    push dword ptr [ebp+8];
    call dword ptr [ebp+16];
    mov esp, ebp;
    pop ebp;
    ret;
qbdi_asmStackSwitch ENDP

END
