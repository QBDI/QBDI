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
#include "validator.h"
#include "QBDIPreload.h"
#include "linux_process.h"
#include "master.h"
#include "instrumented.h"

#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

#include "Memory.h"
#include "Platform.h"
#include "Utility/LogSys.h"

static const size_t STACK_SIZE = 8388608;
static QBDI::GPRState ENTRY_GPR;
static QBDI::FPRState ENTRY_FPR;
static bool INSTRUMENTED = false;

QBDIPRELOAD_INIT;


int QBDI::qbdipreload_on_main() {
    QBDI::LOGSYS.addFilter("*", QBDI::LogPriority::DEBUG);

    QBDI::VM* vm = new QBDI::VM();
    vm->instrumentAllExecutableMaps();
    for(const QBDI::MemoryMap& m :  QBDI::getCurrentProcessMaps()) {
        if((m.name.compare(0, 7, "libc-2.") == 0) ||
           (m.name.compare(0, 5, "ld-2.") == 0)   ||
           (m.name.compare(0, 7, "libcofi") == 0)) {
            vm->removeInstrumentedRange(m.range.start, m.range.end);
        }
    }

    vm->setGPRState(&ENTRY_GPR);
    vm->setFPRState(&ENTRY_FPR);

#if defined(__x86_64__)
    QBDI::rword start = QBDI_GPR_GET(vm->getGPRState(), QBDI::REG_PC);
    QBDI::rword stop = *((QBDI::rword*) QBDI_GPR_GET(vm->getGPRState(), QBDI::REG_SP));
#elif defined(__arm__)
    QBDI::rword start = QBDI_GPR_GET(vm->getGPRState(), QBDI::REG_PC);
    QBDI::rword stop = QBDI_GPR_GET(vm->getGPRState(), QBDI::REG_LR);
#endif
    start_instrumented(vm, start, stop);

    return QBDIPRELOAD_NO_ERROR;
}

int QBDI::qbdipreload_on_premain(void *gprCtx, void *fpuCtx) {
    void* newStack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    ucontext_t* uap = (ucontext_t*) gprCtx;

    qbdipreload_threadCtxToGPRState(uap, &ENTRY_GPR);
    qbdipreload_floatCtxToFPRState(uap, &ENTRY_FPR);

#if defined(QBDI_ARCH_X86_64)
    uap->uc_mcontext.gregs[REG_RSP] = (uint64_t) newStack + STACK_SIZE - 8;
    uap->uc_mcontext.gregs[REG_RBP] = (uint64_t) newStack + STACK_SIZE - 8;
#elif defined(QBDI_ARCH_ARM)
    uap->uc_mcontext.arm_sp = (uint32_t) newStack + STACK_SIZE - 8;
    uap->uc_mcontext.arm_fp = (uint32_t) newStack + STACK_SIZE - 8;
#endif
    return QBDIPRELOAD_NO_ERROR;
}

int QBDI::qbdipreload_on_run(QBDI::VMInstanceRef vm, QBDI::rword start, QBDI::rword stop) {
    return QBDIPRELOAD_NOT_HANDLED;
}

int QBDI::qbdipreload_on_exit(int status) {
    if(INSTRUMENTED) {
        cleanup_instrumentation();
    }
    return QBDIPRELOAD_NO_ERROR;
}

int QBDI::qbdipreload_on_start(void *main, int argc, char **argv) {
    pid_t debugged, instrumented;
    LinuxProcess* debuggedProcess = nullptr;

    instrumented = fork();
    if(instrumented == 0) {
        INSTRUMENTED = true;
		QBDI::qbdipreload_hook_main(main);
        return QBDIPRELOAD_NO_ERROR;
    }
    debugged = fork();
    if(debugged == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        return QBDIPRELOAD_NO_ERROR;
    }

    debuggedProcess = new LinuxProcess(debugged);
    if (ptrace(PTRACE_ATTACH, debugged, NULL, NULL) == -1) {
        fprintf(stderr, "validator: fatal error, PTRACE_ATTACH failed !\n\n");
    }
    start_master(debuggedProcess, instrumented);
    delete debuggedProcess;

    return QBDIPRELOAD_NOT_HANDLED;
}
