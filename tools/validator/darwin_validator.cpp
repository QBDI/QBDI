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
#include "darwin_process.h"
#include "master.h"
#include "instrumented.h"

#include <stdlib.h>
#include <string.h>
#include <mach/mach.h>
#include <mach/task.h>

#include "Memory.hpp"
#include "Platform.h"
#include "Utility/LogSys.h"

static const size_t STACK_SIZE = 8388608;
static QBDI::GPRState ENTRY_GPR;
static QBDI::FPRState ENTRY_FPR;
static pid_t DEBUGGED, INSTRUMENTED;


enum Role {
    Master,
    Instrumented,
    Debugged
} ROLE;


QBDIPRELOAD_INIT;


int QBDI::qbdipreload_on_main(int argc, char** argv) {
    QBDI::LOGSYS.addFilter("*", QBDI::LogPriority::DEBUG);

    if(ROLE == Role::Master) {
        DarwinProcess* debuggedProcess = new DarwinProcess(DEBUGGED);
        start_master(debuggedProcess, INSTRUMENTED);
        delete debuggedProcess;
    }
    else if(ROLE == Role::Instrumented) {
        QBDI::VM* vm = new QBDI::VM();
        vm->instrumentAllExecutableMaps();

        // Skip ourself, system library (to avoid conflicts) and objc (to avoid awful performances)
        for(const QBDI::MemoryMap& m :  QBDI::getCurrentProcessMaps()) {
            if((m.name.compare(0, 9, "libsystem") == 0 ||
                m.name.compare(0, 7, "libobjc") == 0 ||
                m.name.compare(0, 13, "libvalidator2") == 0)) {
                vm->removeInstrumentedRange(m.range.start, m.range.end);
            }
        }

        vm->setGPRState(&ENTRY_GPR);
        vm->setFPRState(&ENTRY_FPR);

        QBDI::rword start = QBDI_GPR_GET(vm->getGPRState(), QBDI::REG_PC);
        QBDI::rword stop = *((QBDI::rword*) QBDI_GPR_GET(vm->getGPRState(), QBDI::REG_SP));
        start_instrumented(vm, start, stop);
    }
    exit(0);
}


int QBDI::qbdipreload_on_premain(void *gprCtx, void *fpuCtx) {
    x86_thread_state64_t* threadState = (x86_thread_state64_t*) gprCtx;
    x86_float_state64_t* floatState = (x86_float_state64_t*) fpuCtx;

    // Perform stack swapping and GPR / FPR init for Instrumented only
    if(ROLE == Role::Instrumented) {
        // Allocating fake stack
        void* newStack = QBDI::alignedAlloc(STACK_SIZE, 16);
        if(newStack == nullptr) {
            fprintf(stderr, "Failed to allocate fake stack\n");
            exit(QBDIPRELOAD_ERR_STARTUP_FAILED);
        }

        // Copying the initial thread state
        threadStateToGPRState(threadState, &ENTRY_GPR);
        floatStateToFPRState(floatState, &ENTRY_FPR);

        // Swapping to fake stack
        threadState->__rbp = (uint64_t) newStack + STACK_SIZE - 8;
        threadState->__rsp = threadState->__rbp;
    } else if(ROLE == Role::Master) {
        // LC_UNIXTHREAD binaries use a different calling convention
        // This allow to call catchEntrypoint, and have not side effect
        // as original execution is never resumed in Master mode
        if ((threadState->__rsp & 0x8) == 0) {
            threadState->__rsp -= 8;
        }
    }

    return QBDIPRELOAD_NO_ERROR;
}


int QBDI::qbdipreload_on_run(QBDI::VMInstanceRef vm, QBDI::rword start, QBDI::rword stop) {
    return QBDIPRELOAD_NOT_HANDLED;
}


int QBDI::qbdipreload_on_exit(int status) {
    if(ROLE == Role::Instrumented) {
        cleanup_instrumentation();
    }
    return QBDIPRELOAD_NO_ERROR;
}


int QBDI::qbdipreload_on_start(void *main) {
    INSTRUMENTED = fork();
    if(INSTRUMENTED == 0) {
        ROLE = Role::Instrumented;
        QBDI::qbdipreload_hook_main(main);
        return QBDIPRELOAD_NO_ERROR;
    }

    DEBUGGED = fork();
    if(DEBUGGED == 0) {
        ROLE = Role::Debugged;
        // We put ourself to sleep, waiting for our charming prince
        task_suspend(mach_task_self());
        return QBDIPRELOAD_NO_ERROR;
    }

    QBDI::qbdipreload_hook_main(main);
    ROLE = Role::Master;

    return QBDIPRELOAD_NO_ERROR;
}
