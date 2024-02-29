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

.code

; Assembly exported trampoline
PUBLIC qbdipreload_trampoline 

; C trampoline destination import
EXTERN qbdipreload_trampoline_impl : proc

; Original app RSP (stack top) value when
; entering start (PE EntryPoint)
EXTERN g_shadowStackTop : QWORD

; Trampoline stub
; Update RSP and jump to trampoline implementation
; (in C), g_shadowStackTop must be intiialized before
; using the trampoline
; Note that RBP is not used on x64
qbdipreload_trampoline PROC
    mov rsp, g_shadowStackTop
    jmp qbdipreload_trampoline_impl
qbdipreload_trampoline ENDP

END