; This file is part of QBDI.
;
; Copyright 2019 Quarkslab
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

.model flat
.code

; Assembly exported trampoline
PUBLIC _qbdipreload_trampoline 

; C trampoline destination import
EXTERN _qbdipreload_trampoline_impl : proc

; Original app RSP (stack top) value when
; entering start (PE EntryPoint)
EXTERN _g_shadowStackTop : DWORD
EXTERN _g_shadowStackBase : DWORD

; Trampoline stub
; Update ESP/EBP and jump to trampoline implementation
; (in C), _g_shadowStackTop & _g_shadowStackBase must be 
; intiialized before using the trampoline
_qbdipreload_trampoline PROC
    mov esp, _g_shadowStackTop
    mov ebp, _g_shadowStackBase
    jmp _qbdipreload_trampoline_impl
_qbdipreload_trampoline ENDP

END