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
#include "QBDIPreload.h"
#include "darwin_exceptd.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <mach/task.h>
#include <mach/mach_error.h>
#include <mach/mach_init.h>
#include <mach/mach_port.h>
#include <mach/mach_traps.h>
#include <mach/mach_vm.h>
#include <mach/thread_act.h>
#include <mach-o/dyld.h>
#include <mach-o/loader.h>

#include <QBDI.h>

#define DYLD_INTERPOSE(_replacment,_replacee) \
    __attribute__((used)) static struct{ const void* replacment; const void* replacee; } _interpose_##_replacee \
    __attribute__ ((section ("__DATA,__interpose"))) = { (const void*)(unsigned long)&_replacment, (const void*)(unsigned long)&_replacee };

#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
static const uint8_t BRK_INS = 0xCC;
#endif
static const size_t STACK_SIZE = 8388608;

static bool HAS_EXITED = false;
static bool HAS_PRELOAD = false;
static bool DEFAULT_HANDLER = false;
static GPRState ENTRY_GPR;
static FPRState ENTRY_FPR;

static struct ExceptionHandler* MAIN_EXCEPTION_HANDLER = NULL;

static struct {
    rword address;
    uint8_t value;
} ENTRY_BRK;

#if defined(QBDI_ARCH_X86)
#define MACH_MAGIC MH_MAGIC
#define MACH_HEADER mach_header
#define MACH_SEG_CMD LC_SEGMENT
#define MACH_SEG segment_command
#define THREAD_STATE_ID x86_THREAD_STATE32
#define THREAD_STATE_COUNT x86_THREAD_STATE32_COUNT
#define THREAD_STATE_FP_ID x86_FLOAT_STATE32
#define THREAD_STATE_FP_COUNT x86_FLOAT_STATE32_COUNT
#define THREAD_STATE x86_thread_state32_t
#define THREAD_STATE_FP x86_float_state32_t
#define THREAD_STATE_BP __ebp
#define THREAD_STATE_SP __esp
#define THREAD_STATE_PC __eip
#elif defined(QBDI_ARCH_X86_64)
#define MACH_MAGIC MH_MAGIC_64
#define MACH_HEADER mach_header_64
#define MACH_SEG segment_command_64
#define MACH_SEG_CMD LC_SEGMENT_64
#define THREAD_STATE_ID x86_THREAD_STATE64
#define THREAD_STATE_COUNT x86_THREAD_STATE64_COUNT
#define THREAD_STATE_FP_ID x86_FLOAT_STATE64
#define THREAD_STATE_FP_COUNT x86_FLOAT_STATE64_COUNT
#define THREAD_STATE x86_thread_state64_t
#define THREAD_STATE_FP x86_float_state64_t
#define THREAD_STATE_BP __rbp
#define THREAD_STATE_SP __rsp
#define THREAD_STATE_PC __rip
#endif

void qbdipreload_threadCtxToGPRState(const void *gprCtx, GPRState* gprState) {
#if defined(QBDI_ARCH_X86)
    const x86_thread_state32_t* ts = (x86_thread_state32_t*) gprCtx;

    gprState->eax    = ts->__eax;
    gprState->ebx    = ts->__ebx;
    gprState->ecx    = ts->__ecx;
    gprState->edx    = ts->__edx;
    gprState->esi    = ts->__esi;
    gprState->edi    = ts->__edi;
    gprState->ebp    = ts->__ebp;
    gprState->esp    = ts->__esp;
    gprState->eip    = ts->__eip;
    gprState->eflags = ts->__eflags;
#else
    const x86_thread_state64_t* ts = (x86_thread_state64_t*) gprCtx;

    gprState->rax    = ts->__rax;
    gprState->rbx    = ts->__rbx;
    gprState->rcx    = ts->__rcx;
    gprState->rdx    = ts->__rdx;
    gprState->rsi    = ts->__rsi;
    gprState->rdi    = ts->__rdi;
    gprState->rbp    = ts->__rbp;
    gprState->rsp    = ts->__rsp;
    gprState->r8     = ts->__r8;
    gprState->r9     = ts->__r9;
    gprState->r10    = ts->__r10;
    gprState->r11    = ts->__r11;
    gprState->r12    = ts->__r12;
    gprState->r13    = ts->__r13;
    gprState->r14    = ts->__r14;
    gprState->r15    = ts->__r15;
    gprState->rip    = ts->__rip;
    gprState->eflags = ts->__rflags;
#endif
}

void qbdipreload_floatCtxToFPRState(const void *fprCtx, FPRState* fprState) {
    const x86_float_state64_t* fs = (x86_float_state64_t*) fprCtx;

#define SYNC_STMM(ID) memcpy(&fprState->stmm##ID, &fs->__fpu_stmm##ID, 10);
    SYNC_STMM(0);
    SYNC_STMM(1);
    SYNC_STMM(2);
    SYNC_STMM(3);
    SYNC_STMM(4);
    SYNC_STMM(5);
    SYNC_STMM(6);
    SYNC_STMM(7);
#undef SYNC_STMM
#define SYNC_XMM(ID) memcpy(&fprState->xmm##ID, &fs->__fpu_xmm##ID, 16);
    SYNC_XMM(0);
    SYNC_XMM(1);
    SYNC_XMM(2);
    SYNC_XMM(3);
    SYNC_XMM(4);
    SYNC_XMM(5);
    SYNC_XMM(6);
    SYNC_XMM(7);
#if defined(QBDI_ARCH_X86_64)
    SYNC_XMM(8);
    SYNC_XMM(9);
    SYNC_XMM(10);
    SYNC_XMM(11);
    SYNC_XMM(12);
    SYNC_XMM(13);
    SYNC_XMM(14);
    SYNC_XMM(15);
#endif
#undef SYNC_XMM
    memcpy(&fprState->rfcw, &fs->__fpu_fcw, 2);
    memcpy(&fprState->rfsw, &fs->__fpu_fsw, 2);
    memcpy(&fprState->ftw, &fs->__fpu_ftw, 1);
    memcpy(&fprState->rsrv1, &fs->__fpu_rsrv1, 1);
    memcpy(&fprState->fop, &fs->__fpu_fop, 2);
    memcpy(&fprState->mxcsr, &fs->__fpu_mxcsr, 4);
    memcpy(&fprState->mxcsrmask, &fs->__fpu_mxcsrmask, 4);
}

void setEntryBreakpoint(rword address) {
    kern_return_t kr;
    task_t self = mach_task_self();

    kr = mach_vm_protect(self, address, 1, false, VM_PROT_READ | VM_PROT_WRITE);
    if(kr != KERN_SUCCESS) {
        fprintf(stderr, "Failed to change memory protection to RW for setting a breakpoint: %s\n", mach_error_string(kr));
        exit(QBDIPRELOAD_ERR_STARTUP_FAILED);
    }
    ENTRY_BRK.address = address;
    ENTRY_BRK.value = *((uint8_t*) address);
    *((uint8_t*) address) = BRK_INS;
    kr = mach_vm_protect(self, address, 1, false, VM_PROT_READ | VM_PROT_EXECUTE);
    if(kr != KERN_SUCCESS) {
        fprintf(stderr, "Failed to change memory protection to RX after setting a breakpoint: %s\n", mach_error_string(kr));
        exit(QBDIPRELOAD_ERR_STARTUP_FAILED);
    }
}

void unsetEntryBreakpoint() {
    kern_return_t kr;
    task_t self = mach_task_self();

    kr = mach_vm_protect(self, ENTRY_BRK.address, 1, false, VM_PROT_READ | VM_PROT_WRITE);
    if(kr != KERN_SUCCESS) {
        fprintf(stderr, "Failed to change memory protection to RW for unsetting a breakpoint: %s\n", mach_error_string(kr));
        exit(QBDIPRELOAD_ERR_STARTUP_FAILED);
    }
    *((uint8_t*) ENTRY_BRK.address) = ENTRY_BRK.value;
    kr = mach_vm_protect(self, ENTRY_BRK.address, 1, false, VM_PROT_READ | VM_PROT_EXECUTE);
    if(kr != KERN_SUCCESS) {
        fprintf(stderr, "Failed to change memory protection to RX after unsetting a breakpoint: %s\n", mach_error_string(kr));
        exit(QBDIPRELOAD_ERR_STARTUP_FAILED);
    }
}

rword getEntrypointAddress() {
    unsigned i = 0, j = 0;
    rword segaddr = 0;
    rword entryoff = 0;
    // 0 is the process base binary image index
    rword slide = _dyld_get_image_vmaddr_slide(0);
    const struct MACH_HEADER* header = (struct MACH_HEADER*) _dyld_get_image_header(0);

    // Checking that it is indeed a mach binary
    if(header->magic != MACH_MAGIC) {
        fprintf(stderr, "Process is not a mach binary\n");
        exit(QBDIPRELOAD_ERR_STARTUP_FAILED);
    }

    // Find entrypoint and _TEXT segment command - Logic copied from libmacho/getsecbyname.c
    struct load_command* cmd = (struct load_command*) ((char*) header + sizeof(struct MACH_HEADER));
    for(i = 0; i < header->ncmds; i++) {
        if (cmd->cmd == LC_UNIXTHREAD) {
            uint32_t flavor = *((uint32_t*) cmd + 2);
            switch (flavor) {
                // TODO: support more arch
                case THREAD_STATE_ID:
                    {
                        THREAD_STATE* state = (THREAD_STATE*)((uint32_t*) cmd + 4);
                        entryoff = state->THREAD_STATE_PC;
                        return entryoff + slide;
                    }
            }
        }
#ifdef LC_MAIN
        else if(cmd->cmd == LC_MAIN) {
            entryoff = ((struct entry_point_command*) cmd)->entryoff;
            j |= 1;
        }
#endif
        else if(cmd->cmd == MACH_SEG_CMD && strcmp("__TEXT", ((struct MACH_SEG*) cmd)->segname) == 0) {
            segaddr = ((struct MACH_SEG*) cmd)->vmaddr;
            j |= 2;
        }
        cmd = (struct load_command*) ((char*) cmd + cmd->cmdsize);
    }

    if(j != 3) {
        fprintf(stderr, "Could not find process entry point");
        exit(QBDIPRELOAD_ERR_STARTUP_FAILED);
    }

    return segaddr + slide + entryoff;
}

void catchEntrypoint(int argc, char **argv) {
    int status = QBDIPRELOAD_NOT_HANDLED;

    unsetEntryBreakpoint();
    stopExceptionHandler(MAIN_EXCEPTION_HANDLER);

#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
    // detect legacy UNIXTHREAD start (push 0)
    uint16_t *insn = (uint16_t*) QBDI_GPR_GET(&ENTRY_GPR, REG_PC);
    if (*insn == 0x6a) {
        argc = *(int*) QBDI_GPR_GET(&ENTRY_GPR, REG_SP);
        argv = (char**) ((rword)QBDI_GPR_GET(&ENTRY_GPR, REG_SP) + sizeof(rword));
    }
#endif
    status = qbdipreload_on_main(argc, argv);

    if (DEFAULT_HANDLER && (status == QBDIPRELOAD_NOT_HANDLED)) {
        VMInstanceRef vm;
        qbdi_initVM(&vm, NULL, NULL, 0);
        qbdi_instrumentAllExecutableMaps(vm);

        // Skip system library (to avoid conflicts)
        size_t size = 0, i = 0;
        char **modules = qbdi_getModuleNames(&size);

        // Filter some modules to avoid conflicts
        qbdi_removeInstrumentedModuleFromAddr(vm, (rword) &catchEntrypoint);
        for(i = 0; i < size; i++) {
            if (strstr(modules[i], "libsystem")) {
                qbdi_removeInstrumentedModule(vm, modules[i]);
            }
        }
        for(i = 0; i < size; i++) {
            free(modules[i]);
        }
        free(modules);

        // Setup VM states
        qbdi_setGPRState(vm, &ENTRY_GPR);
        qbdi_setFPRState(vm, &ENTRY_FPR);

        rword start = QBDI_GPR_GET(qbdi_getGPRState(vm), REG_PC);
        rword stop = *((rword*) QBDI_GPR_GET(qbdi_getGPRState(vm), REG_SP));

        status = qbdipreload_on_run(vm, start, stop);
    }
    exit(status);
}

kern_return_t redirectExec(
    mach_port_t exception_port,
    mach_port_t thread,
    mach_port_t task,
    exception_type_t exception,
    mach_exception_data_t code,
    mach_msg_type_number_t codeCnt
) {
    kern_return_t kr;
    THREAD_STATE threadState;
    THREAD_STATE_FP floatState;
    mach_msg_type_number_t count;

    // Reading thread state
    count = THREAD_STATE_COUNT;
    kr = thread_get_state(thread, THREAD_STATE_ID, (thread_state_t) &threadState, &count);
    if(kr != KERN_SUCCESS) {
        fprintf(stderr, "Failed to get GPR thread state: %s\n", mach_error_string(kr));
        exit(QBDIPRELOAD_ERR_STARTUP_FAILED);
    }
    count = THREAD_STATE_FP_COUNT;
    kr = thread_get_state(thread, THREAD_STATE_FP_ID, (thread_state_t) &floatState, &count);
    if(kr != KERN_SUCCESS) {
        fprintf(stderr, "Failed to get FPR thread state: %s\n", mach_error_string(kr));
        exit(QBDIPRELOAD_ERR_STARTUP_FAILED);
    }

    // x86 breakpoint quirk
    threadState.THREAD_STATE_PC -= 1;

    int status = qbdipreload_on_premain((void*) &threadState, (void*) &floatState);

    // Copying the initial thread state
    qbdipreload_threadCtxToGPRState(&threadState, &ENTRY_GPR);
    qbdipreload_floatCtxToFPRState(&floatState, &ENTRY_FPR);

    // if not handled, use default handler
    if (status == QBDIPRELOAD_NOT_HANDLED) {
        DEFAULT_HANDLER = true;

        // Allocating fake stack
        void* newStack = NULL;
        kr = mach_vm_map(task, (mach_vm_address_t*) &newStack, STACK_SIZE, 0, VM_FLAGS_ANYWHERE,
                         MEMORY_OBJECT_NULL, 0, false, VM_PROT_READ|VM_PROT_WRITE, VM_PROT_ALL, VM_INHERIT_COPY);
        if(kr != KERN_SUCCESS) {
            fprintf(stderr, "Failed to allocate fake stack: %s\n", mach_error_string(kr));
            exit(QBDIPRELOAD_ERR_STARTUP_FAILED);
        }

        // Swapping to fake stack
        threadState.THREAD_STATE_BP = (rword) newStack + STACK_SIZE - 8;
        threadState.THREAD_STATE_SP = threadState.THREAD_STATE_BP;
    }

    // Execution redirection
    threadState.THREAD_STATE_PC = (rword) catchEntrypoint;
    count = THREAD_STATE_COUNT;
    kr = thread_set_state(thread, THREAD_STATE_ID, (thread_state_t) &threadState, count);
    if(kr != KERN_SUCCESS) {
        fprintf(stderr, "Failed to set GPR thread state for redirection: %s\n", mach_error_string(kr));
        exit(QBDIPRELOAD_ERR_STARTUP_FAILED);
    }

    return KERN_SUCCESS;
}


void* qbdipreload_setup_exception_handler(uint32_t target, uint32_t mask, void* handler) {
    task_t target_ = (task_t) target;
    exception_mask_t mask_ = (exception_mask_t) mask;
    exception_handler_func handler_ = (exception_handler_func) handler;
    return setupExceptionHandler(target_, mask_, handler_);
}


int qbdipreload_hook_main(void *main) {
    setEntryBreakpoint((rword) main);
    MAIN_EXCEPTION_HANDLER = setupExceptionHandler(mach_task_self(), EXC_MASK_BREAKPOINT, redirectExec);
    return QBDIPRELOAD_NO_ERROR;
}

QBDI_EXPORT void intercept_exit(int status) {
    if (!HAS_EXITED && HAS_PRELOAD) {
        HAS_EXITED = true;
        qbdipreload_on_exit(status);
    }
    exit(status);
}
DYLD_INTERPOSE(intercept_exit, exit)

QBDI_EXPORT void intercept__exit(int status) {
    if (!HAS_EXITED && HAS_PRELOAD) {
        HAS_EXITED = true;
        qbdipreload_on_exit(status);
    }
    _exit(status);
}
DYLD_INTERPOSE(intercept__exit, _exit)

int qbdipreload_hook_init() {
    // do nothing if the library isn't preload
    if (getenv("DYLD_INSERT_LIBRARIES") == NULL)
        return QBDIPRELOAD_NO_ERROR;

    HAS_PRELOAD = true;
    rword entrypoint = getEntrypointAddress();

    int status = qbdipreload_on_start((void*)entrypoint);
    if (status == QBDIPRELOAD_NOT_HANDLED) {
        status = qbdipreload_hook_main((void*)entrypoint);
    }
    return status;
}
