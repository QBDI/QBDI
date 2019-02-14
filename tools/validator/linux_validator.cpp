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
static bool INSTRUMENTED = false;
static pid_t debugged, instrumented;

QBDIPRELOAD_INIT;


int QBDI::qbdipreload_on_main(int argc, char** argv) {
    if (INSTRUMENTED) {
        QBDI::LOGSYS.addFilter("*", QBDI::LogPriority::WARNING);
        return QBDIPRELOAD_NOT_HANDLED;

    } else {
        LinuxProcess* debuggedProcess = nullptr;
        debuggedProcess = new LinuxProcess(debugged);
        start_master(debuggedProcess, instrumented);
        delete debuggedProcess;
        return QBDIPRELOAD_NO_ERROR;
    }
}

int QBDI::qbdipreload_on_premain(void *gprCtx, void *fpuCtx) {
    if (INSTRUMENTED)
        return QBDIPRELOAD_NOT_HANDLED;
    return QBDIPRELOAD_NO_ERROR;
}

int QBDI::qbdipreload_on_run(QBDI::VMInstanceRef vm, QBDI::rword start, QBDI::rword stop) {
    start_instrumented(vm, start, stop);
    return QBDIPRELOAD_NOT_HANDLED;
}

int QBDI::qbdipreload_on_exit(int status) {
    if(INSTRUMENTED) {
        cleanup_instrumentation();
    }
    return QBDIPRELOAD_NO_ERROR;
}

int QBDI::qbdipreload_on_start(void *main) {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    instrumented = fork();
    if(instrumented == 0) {
        INSTRUMENTED = true;
        return QBDIPRELOAD_NOT_HANDLED;
    }
    debugged = fork();
    if(debugged == 0) {
        ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        return QBDIPRELOAD_NO_ERROR;
    }
    if (ptrace(PTRACE_ATTACH, debugged, NULL, NULL) == -1) {
        fprintf(stderr, "validator: fatal error, PTRACE_ATTACH failed !\n\n");
        kill(instrumented, SIGKILL);
        kill(debugged, SIGKILL);
        exit(0);
    }

    return QBDIPRELOAD_NOT_HANDLED;
}
