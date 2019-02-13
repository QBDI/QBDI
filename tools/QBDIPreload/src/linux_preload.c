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
#include "QBDIPreload.h"

#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/wait.h>

#include <QBDI.h>

#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
#define SIGBRK SIGTRAP
static const long BRK_MASK = 0xFF;
static const long BRK_INS = 0xCC;
#elif defined(QBDI_ARCH_ARM)
#define SIGBRK SIGILL
static const long BRK_MASK = 0xFFFFFFFF;
static const long BRK_INS = 0xE7FFDEFE;
#endif

static const size_t STACK_SIZE = 8388608;
static bool HAS_EXITED = false;
static bool DEFAULT_HANDLER = false;
GPRState ENTRY_GPR;
FPRState ENTRY_FPR;

static struct {
    void* address;
    long value;
} ENTRY_BRK;


void qbdipreload_threadCtxToGPRState(const void *gprCtx, GPRState* gprState) {
    const ucontext_t* uap = (const ucontext_t*) gprCtx;

#if defined(QBDI_ARCH_X86_64)
    gprState->rax = uap->uc_mcontext.gregs[REG_RAX];
    gprState->rbx = uap->uc_mcontext.gregs[REG_RBX];
    gprState->rcx = uap->uc_mcontext.gregs[REG_RCX];
    gprState->rdx = uap->uc_mcontext.gregs[REG_RDX];
    gprState->rsi = uap->uc_mcontext.gregs[REG_RSI];
    gprState->rdi = uap->uc_mcontext.gregs[REG_RDI];
    gprState->rbp = uap->uc_mcontext.gregs[REG_RBP];
    gprState->rsp = uap->uc_mcontext.gregs[REG_RSP];
    gprState->r8 = uap->uc_mcontext.gregs[REG_R8];
    gprState->r9 = uap->uc_mcontext.gregs[REG_R9];
    gprState->r10 = uap->uc_mcontext.gregs[REG_R10];
    gprState->r11 = uap->uc_mcontext.gregs[REG_R11];
    gprState->r12 = uap->uc_mcontext.gregs[REG_R12];
    gprState->r13 = uap->uc_mcontext.gregs[REG_R13];
    gprState->r14 = uap->uc_mcontext.gregs[REG_R14];
    gprState->r15 = uap->uc_mcontext.gregs[REG_R15];
    gprState->rip = uap->uc_mcontext.gregs[REG_RIP];
    gprState->eflags = uap->uc_mcontext.gregs[REG_EFL];
#elif defined(QBDI_ARCH_X86)
    gprState->eax = uap->uc_mcontext.gregs[REG_EAX];
    gprState->ebx = uap->uc_mcontext.gregs[REG_EBX];
    gprState->ecx = uap->uc_mcontext.gregs[REG_ECX];
    gprState->edx = uap->uc_mcontext.gregs[REG_EDX];
    gprState->esi = uap->uc_mcontext.gregs[REG_ESI];
    gprState->edi = uap->uc_mcontext.gregs[REG_EDI];
    gprState->ebp = uap->uc_mcontext.gregs[REG_EBP];
    gprState->esp = uap->uc_mcontext.gregs[REG_ESP];
    gprState->eip = uap->uc_mcontext.gregs[REG_EIP];
    gprState->eflags = uap->uc_mcontext.gregs[REG_EFL];
#elif defined(QBDI_ARCH_ARM)
    gprState->r0 = uap->uc_mcontext.arm_r0;
    gprState->r1 = uap->uc_mcontext.arm_r1;
    gprState->r2 = uap->uc_mcontext.arm_r2;
    gprState->r3 = uap->uc_mcontext.arm_r3;
    gprState->r4 = uap->uc_mcontext.arm_r4;
    gprState->r5 = uap->uc_mcontext.arm_r5;
    gprState->r6 = uap->uc_mcontext.arm_r6;
    gprState->r7 = uap->uc_mcontext.arm_r7;
    gprState->r8 = uap->uc_mcontext.arm_r8;
    gprState->r9 = uap->uc_mcontext.arm_r9;
    gprState->r10 = uap->uc_mcontext.arm_r10;
    gprState->fp = uap->uc_mcontext.arm_fp;
    gprState->r12 = uap->uc_mcontext.arm_ip;
    gprState->sp = uap->uc_mcontext.arm_sp;
    gprState->lr = uap->uc_mcontext.arm_lr;
    gprState->pc = uap->uc_mcontext.arm_pc;
    gprState->cpsr = uap->uc_mcontext.arm_cpsr;
#endif
}

void qbdipreload_floatCtxToFPRState(const void* fprCtx, FPRState* fprState) {
#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
    const ucontext_t* uap = (const ucontext_t*) fprCtx;

    memcpy(&fprState->stmm0, &uap->uc_mcontext.fpregs->_st[0], 10);
    memcpy(&fprState->stmm1, &uap->uc_mcontext.fpregs->_st[1], 10);
    memcpy(&fprState->stmm2, &uap->uc_mcontext.fpregs->_st[2], 10);
    memcpy(&fprState->stmm3, &uap->uc_mcontext.fpregs->_st[3], 10);
    memcpy(&fprState->stmm4, &uap->uc_mcontext.fpregs->_st[4], 10);
    memcpy(&fprState->stmm5, &uap->uc_mcontext.fpregs->_st[5], 10);
    memcpy(&fprState->stmm6, &uap->uc_mcontext.fpregs->_st[6], 10);
    memcpy(&fprState->stmm7, &uap->uc_mcontext.fpregs->_st[7], 10);
#if defined(QBDI_ARCH_X86_64)
    memcpy(&fprState->xmm0, &uap->uc_mcontext.fpregs->_xmm[0], 16);
    memcpy(&fprState->xmm1, &uap->uc_mcontext.fpregs->_xmm[1], 16);
    memcpy(&fprState->xmm2, &uap->uc_mcontext.fpregs->_xmm[2], 16);
    memcpy(&fprState->xmm3, &uap->uc_mcontext.fpregs->_xmm[3], 16);
    memcpy(&fprState->xmm4, &uap->uc_mcontext.fpregs->_xmm[4], 16);
    memcpy(&fprState->xmm5, &uap->uc_mcontext.fpregs->_xmm[5], 16);
    memcpy(&fprState->xmm6, &uap->uc_mcontext.fpregs->_xmm[6], 16);
    memcpy(&fprState->xmm7, &uap->uc_mcontext.fpregs->_xmm[7], 16);
    memcpy(&fprState->xmm8, &uap->uc_mcontext.fpregs->_xmm[8], 16);
    memcpy(&fprState->xmm9, &uap->uc_mcontext.fpregs->_xmm[9], 16);
    memcpy(&fprState->xmm10, &uap->uc_mcontext.fpregs->_xmm[10], 16);
    memcpy(&fprState->xmm11, &uap->uc_mcontext.fpregs->_xmm[11], 16);
    memcpy(&fprState->xmm12, &uap->uc_mcontext.fpregs->_xmm[12], 16);
    memcpy(&fprState->xmm13, &uap->uc_mcontext.fpregs->_xmm[13], 16);
    memcpy(&fprState->xmm14, &uap->uc_mcontext.fpregs->_xmm[14], 16);
    memcpy(&fprState->xmm15, &uap->uc_mcontext.fpregs->_xmm[15], 16);
    fprState->rfcw = uap->uc_mcontext.fpregs->cwd;
    fprState->rfsw = uap->uc_mcontext.fpregs->swd;
    fprState->ftw = uap->uc_mcontext.fpregs->ftw & 0xFF;
    fprState->rsrv1 = uap->uc_mcontext.fpregs->ftw >> 8;
    fprState->fop = uap->uc_mcontext.fpregs->fop;
    fprState->mxcsr = uap->uc_mcontext.fpregs->mxcsr;
    fprState->mxcsrmask = uap->uc_mcontext.fpregs->mxcr_mask;
#else
    fprState->rfcw = uap->uc_mcontext.fpregs->cw;
    fprState->rfsw = uap->uc_mcontext.fpregs->sw;
    fprState->ftw = uap->uc_mcontext.fpregs->tag & 0xFF;
    fprState->rsrv1 = uap->uc_mcontext.fpregs->tag >> 8;
    fprState->mxcsr = 0x1f80;
    fprState->mxcsrmask = 0xffff;
#endif
#elif defined(QBDI_ARCH_ARM)
    // Somehow not available under ARM
#endif
}

void setEntryBreakpoint(void *address) {
    long pageSize = sysconf(_SC_PAGESIZE);
    uintptr_t base = (uintptr_t) address - ((uintptr_t) address % pageSize);
    
    ENTRY_BRK.address = address;
    mprotect((void*) base, pageSize, PROT_READ | PROT_WRITE);
    ENTRY_BRK.value = *((long*) address);
    *((long*) address) = BRK_INS | (ENTRY_BRK.value & (~BRK_MASK));
    mprotect((void*) base, pageSize, PROT_READ | PROT_EXEC);
}

void unsetEntryBreakpoint() {
    long pageSize = sysconf(_SC_PAGESIZE);
    uintptr_t base = (uintptr_t) ENTRY_BRK.address - ((uintptr_t) ENTRY_BRK.address % pageSize);

    mprotect((void*) base, pageSize, PROT_READ | PROT_WRITE);
    *((long*) ENTRY_BRK.address) = ENTRY_BRK.value;
    mprotect((void*) base, pageSize, PROT_READ | PROT_EXEC);
}

void catchEntrypoint(int argc, char** argv) {
    int status = QBDIPRELOAD_NOT_HANDLED;

    unsetEntryBreakpoint();

    status = qbdipreload_on_main(argc, argv);

    if (DEFAULT_HANDLER && (status == QBDIPRELOAD_NOT_HANDLED)) {
        VMInstanceRef vm;
        qbdi_initVM(&vm, NULL, NULL);
        qbdi_instrumentAllExecutableMaps(vm);

        size_t size = 0, i = 0;
        QBDI_MemoryMap **modules = qbdi_getCurrentProcessMaps(&size);

        // Filter some modules to avoid conflicts
        qbdi_removeInstrumentedModuleFromAddr(vm, (rword) &catchEntrypoint);
        for(i = 0; i < size; i++) {
            if ((modules[i]->permission & QBDI_PF_EXEC) && (
                    strstr(modules[i]->name, "libc-2.") ||
                    strstr(modules[i]->name, "ld-2.") ||
                    strstr(modules[i]->name, "libcofi") ||
                    modules[i]->name[0] == 0)) {
                qbdi_removeInstrumentedRange(vm, modules[i]->start, modules[i]->end);
            }
        }
        for(i = 0; i < size; i++) {
            free(modules[i]->name);
            free(modules[i]);
        }
        free(modules);

        // Set original states
        qbdi_setGPRState(vm, &ENTRY_GPR);
        qbdi_setFPRState(vm, &ENTRY_FPR);

#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
        rword start = QBDI_GPR_GET(qbdi_getGPRState(vm), REG_PC);
        rword stop = *((rword*) QBDI_GPR_GET(qbdi_getGPRState(vm), REG_SP));
#elif defined(QBDI_ARCH_ARM)
        rword start = QBDI_GPR_GET(qbdi_getGPRState(vm), REG_PC);
        rword stop = QBDI_GPR_GET(qbdi_getGPRState(vm), REG_LR);
#endif
        status = qbdipreload_on_run(vm, start, stop);
    }
    exit(status);
}

void redirectExec(int signum, siginfo_t *info, void* data) {
    ucontext_t* uap = (ucontext_t*) data;

    // x86-64 breakpoint quirk
#if defined(QBDI_ARCH_X86_64)
    uap->uc_mcontext.gregs[REG_RIP] -= 1;
#elif defined(QBDI_ARCH_X86)
    uap->uc_mcontext.gregs[REG_EIP] -= 1;
#endif

    int status = qbdipreload_on_premain((void*) uap, (void*) uap);

    // Copying the initial thread state
    qbdipreload_threadCtxToGPRState(uap, &ENTRY_GPR);
    qbdipreload_floatCtxToFPRState(uap, &ENTRY_FPR);

    // if not handled, use default handler
    if (status == QBDIPRELOAD_NOT_HANDLED) {
        DEFAULT_HANDLER = true;
        void* newStack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

#if defined(QBDI_ARCH_X86_64)
        uap->uc_mcontext.gregs[REG_RSP] = (rword) newStack + STACK_SIZE - 8;
        uap->uc_mcontext.gregs[REG_RBP] = (rword) newStack + STACK_SIZE - 8;
#elif defined(QBDI_ARCH_X86)
        uap->uc_mcontext.gregs[REG_ESP] = (rword) newStack + STACK_SIZE - 4;
        uap->uc_mcontext.gregs[REG_EBP] = (rword) newStack + STACK_SIZE - 4;
#elif defined(QBDI_ARCH_ARM)
        uap->uc_mcontext.arm_sp = (rword) newStack + STACK_SIZE - 8;
        uap->uc_mcontext.arm_fp = (rword) newStack + STACK_SIZE - 8;
#endif
    }

#if defined(QBDI_ARCH_X86_64)
    uap->uc_mcontext.gregs[REG_RIP] = (rword) catchEntrypoint;
#elif defined(QBDI_ARCH_X86)
    uap->uc_mcontext.gregs[REG_EIP] = (rword) catchEntrypoint;
#elif defined(QBDI_ARCH_ARM)
    uap->uc_mcontext.arm_pc = (rword) catchEntrypoint;
#endif
}


static void* setupExceptionHandler(void (*action)(int, siginfo_t *, void *)) {
    struct sigaction sa;
    sa.sa_sigaction = action;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    if(sigaction(SIGBRK, &sa, NULL) == -1) {
        fprintf(stderr, "Could not set redirectExec handler ...");
        exit(QBDIPRELOAD_ERR_STARTUP_FAILED);
    }
    return NULL;
}


void* qbdipreload_setup_exception_handler(uint32_t target, uint32_t mask, void* handler) {
    return setupExceptionHandler(handler);
}


int qbdipreload_hook_main(void *main) {
    setEntryBreakpoint(main);
    setupExceptionHandler(redirectExec);
    return QBDIPRELOAD_NO_ERROR;
}


QBDI_EXPORT void exit(int status) {
    if (!HAS_EXITED) {
        HAS_EXITED = true;
        qbdipreload_on_exit(status);
    }
    ((void(*)(int))dlsym(RTLD_NEXT, "exit"))(status);
    __builtin_unreachable();
}

QBDI_EXPORT void _exit(int status) {
    if (!HAS_EXITED) {
        HAS_EXITED = true;
        qbdipreload_on_exit(status);
    }
    ((void(*)(int))dlsym(RTLD_NEXT, "_exit"))(status);
    __builtin_unreachable();
}

typedef int (*start_main_fn)(int(*)(int, char**, char**), int, char**, void(*)(void), void(*)(void), void(*)(void), void*);

QBDI_EXPORT int __libc_start_main(int (*main) (int, char**, char**), int argc, char** ubp_av, void (*init) (void), void (*fini) (void), void (*rtld_fini) (void), void (* stack_end)) {
    start_main_fn o_libc_start_main = NULL;

    o_libc_start_main = (start_main_fn) dlsym(RTLD_NEXT, "__libc_start_main");

    int status = qbdipreload_on_start(main);
    if (status == QBDIPRELOAD_NOT_HANDLED) {
        status = qbdipreload_hook_main(main);
    }
    if (status == QBDIPRELOAD_NO_ERROR) {
        return o_libc_start_main(main, argc, ubp_av, init, fini, rtld_fini, stack_end);
    }
    exit(0);
}

int qbdipreload_hook_init() {
    // not used on linux
    return QBDIPRELOAD_NO_ERROR;
}
