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

#include "Memory.h"
#include "Platform.h"
#include "Logs.h"

#define DYLD_INTERPOSE(_replacment,_replacee) \
    __attribute__((used)) static struct{ const void* replacment; const void* replacee; } _interpose_##_replacee \
    __attribute__ ((section ("__DATA,__interpose"))) = { (const void*)(unsigned long)&_replacment, (const void*)(unsigned long)&_replacee };

static const uint8_t BRK_INS = 0xCC;
static const size_t STACK_SIZE = 8388608;

static bool DEFAULT_HANDLER = false;
static GPRState ENTRY_GPR;
static FPRState ENTRY_FPR;

static struct ExceptionHandler* MAIN_EXCEPTION_HANDLER = NULL;

static struct {
    rword address;
    uint8_t value;
} ENTRY_BRK;


void qbdipreload_threadCtxToGPRState(const void *gprCtx, GPRState* gprState) {
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
    SYNC_XMM(8);
    SYNC_XMM(9);
    SYNC_XMM(10);
    SYNC_XMM(11);
    SYNC_XMM(12);
    SYNC_XMM(13);
    SYNC_XMM(14);
    SYNC_XMM(15);
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
    const struct mach_header_64* header = (struct mach_header_64*) _dyld_get_image_header(0);

    // Checking that it is indeed a 64 bit mach binary
    if(header->magic != MH_MAGIC_64) {
        fprintf(stderr, "Process is not a 64 bit mach binary\n");
        exit(QBDIPRELOAD_ERR_STARTUP_FAILED);
    }

    // Find entrypoint and _TEXT segment command - Logic copied from libmacho/getsecbyname.c
    struct load_command* cmd = (struct load_command*) ((char*) header + sizeof(struct mach_header_64));
    for(i = 0; i < header->ncmds; i++) {
        if (cmd->cmd == LC_UNIXTHREAD) {
            uint32_t flavor = *((uint32_t*) cmd + 2);
            switch (flavor) {
                // TODO: support more arch
                case x86_THREAD_STATE64:
                    {
                        x86_thread_state64_t* state = (x86_thread_state64_t*)((uint32_t*) cmd + 4);
                        entryoff = state->__rip;
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
        else if(cmd->cmd == LC_SEGMENT_64 && strcmp("__TEXT", ((struct segment_command_64*) cmd)->segname) == 0) {
            segaddr = ((struct segment_command_64*) cmd)->vmaddr;
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

void catchEntrypoint() {
    int status = QBDIPRELOAD_NOT_HANDLED;

    unsetEntryBreakpoint();
    stopExceptionHandler(MAIN_EXCEPTION_HANDLER);
   
    if (!DEFAULT_HANDLER) {
        status = qbdipreload_on_main();
    }

    if (status == QBDIPRELOAD_NOT_HANDLED) {
#if defined(_QBDI_DEBUG)
        qbdi_addLogFilter("*", QBDI_DEBUG);
#endif
        VMInstanceRef vm;
        qbdi_initVM(&vm, NULL, NULL);
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
    x86_thread_state64_t threadState;
    x86_float_state64_t floatState;
    mach_msg_type_number_t count;

    // Reading thread state
    count = x86_THREAD_STATE64_COUNT;
    kr = thread_get_state(thread, x86_THREAD_STATE64, (thread_state_t) &threadState, &count);
    if(kr != KERN_SUCCESS) {
        fprintf(stderr, "Failed to get GPR thread state: %s\n", mach_error_string(kr));
        exit(QBDIPRELOAD_ERR_STARTUP_FAILED);
    }
    count = x86_FLOAT_STATE64_COUNT;
    kr = thread_get_state(thread, x86_FLOAT_STATE64, (thread_state_t) &floatState, &count);
    if(kr != KERN_SUCCESS) {
        fprintf(stderr, "Failed to get FPR thread state: %s\n", mach_error_string(kr));
        exit(QBDIPRELOAD_ERR_STARTUP_FAILED);
    }

    // x86 breakpoint quirk
    threadState.__rip -= 1;

    int status = qbdipreload_on_premain((void*) &threadState, (void*) &floatState);

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

        // Copying the initial thread state
        qbdipreload_threadCtxToGPRState(&threadState, &ENTRY_GPR);
        qbdipreload_floatCtxToFPRState(&floatState, &ENTRY_FPR);

        // Swapping to fake stack
        threadState.__rbp = (uint64_t) newStack + STACK_SIZE - 8;
        threadState.__rsp = threadState.__rbp;
    }

    // Execution redirection
    threadState.__rip = (uint64_t) catchEntrypoint;
    count = x86_THREAD_STATE64_COUNT;
    kr = thread_set_state(thread, x86_THREAD_STATE64, (thread_state_t) &threadState, count);
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
    qbdipreload_on_exit(status);
    exit(status);
}
DYLD_INTERPOSE(intercept_exit, exit)

QBDI_EXPORT void intercept__exit(int status) {
    qbdipreload_on_exit(status);
    _exit(status);
}
DYLD_INTERPOSE(intercept__exit, _exit)

int qbdipreload_hook_init() {
    rword entrypoint = getEntrypointAddress();
    
    int status = qbdipreload_on_start((void*)entrypoint);
    if (status == QBDIPRELOAD_NOT_HANDLED) {
        status = qbdipreload_hook_main((void*)entrypoint);
    }
    return status;
}
