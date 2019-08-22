
.intel_syntax noprefix
.text


.globl inc
.globl min


inc:
    pushfq;
    mov     rax, [rax+16];
    add     qword ptr [rax], 1;
    popfq;
    ret;

min:
    pushfq;
    push    rbx;
    /* get asmdata->GPRState->rsp 
     * 15 * 8 = 120
     */
    mov     rbx, [rax];
    mov     rbx, [rbx + 120];
    /* get asmdata->data */
    mov     rax, [rax+16];
    /* compare */
    cmp     [rax], rbx;
    jbe     end_min;

    mov     [rax], rbx;
end_min:
    pop     rbx;
    popfq;
    ret;

