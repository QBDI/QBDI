.code

PUBLIC qbdipreload_trampoline 
EXTERN qbdipreload_trampoline_impl : proc
EXTERN g_shadowStackTop : QWORD

qbdipreload_trampoline PROC
    mov rsp, g_shadowStackTop
    jmp qbdipreload_trampoline_impl
qbdipreload_trampoline ENDP

END
