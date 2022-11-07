/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2022 Quarkslab
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
#include "darwin_process.h"
#include "instrumented.h"
#include "master.h"
#include "QBDIPreload.h"

#include <mach/mach.h>
#include <mach/task.h>
#include <stdlib.h>
#include <string.h>

#include "QBDI/Memory.hpp"
#include "QBDI/Platform.h"
#include "Utility/LogSys.h"

static const size_t STACK_SIZE = 8388608;
static QBDI::GPRState ENTRY_GPR;
static QBDI::FPRState ENTRY_FPR;
static pid_t DEBUGGED, INSTRUMENTED;
int ctrlfd, datafd, outputDBIfd, outputDBGfd;

enum Role { Master, Instrumented, Debugged } ROLE;

QBDIPRELOAD_INIT;

int QBDI::qbdipreload_on_main(int argc, char **argv) {
  QBDI::setLogPriority(QBDI::LogPriority::DEBUG);

  if (ROLE == Role::Master) {
    DarwinProcess *debuggedProcess = new DarwinProcess(DEBUGGED);
    start_master(debuggedProcess, INSTRUMENTED, ctrlfd, datafd, outputDBGfd,
                 outputDBIfd);
    delete debuggedProcess;
  } else if (ROLE == Role::Instrumented) {
    QBDI::VM *vm = new QBDI::VM();
    vm->instrumentAllExecutableMaps();

    // Skip ourself, the loader, system library (to avoid conflicts) and objc
    // (to avoid awful performances)
    for (const QBDI::MemoryMap &m : QBDI::getCurrentProcessMaps()) {
      if ((m.name.compare(0, 9, "libsystem") == 0 ||
           m.name.compare(0, 4, "dyld") == 0 ||
           m.name.compare(0, 7, "libdyld") == 0 ||
           m.name.compare(0, 7, "libobjc") == 0 ||
           m.name.compare(0, 13, "libvalidator2") == 0)) {
        vm->removeInstrumentedRange(m.range.start(), m.range.end());
      }
    }

    vm->setGPRState(&ENTRY_GPR);
    vm->setFPRState(&ENTRY_FPR);

    QBDI::rword start = QBDI_GPR_GET(vm->getGPRState(), QBDI::REG_PC);
    QBDI::rword stop =
        *((QBDI::rword *)QBDI_GPR_GET(vm->getGPRState(), QBDI::REG_SP));
    start_instrumented(vm, start, stop, ctrlfd, datafd);
  }
  exit(0);
}

int QBDI::qbdipreload_on_premain(void *gprCtx, void *fpuCtx) {
  THREAD_STATE *threadState = (THREAD_STATE *)gprCtx;
  THREAD_STATE_FP *floatState = (THREAD_STATE_FP *)fpuCtx;

  // Perform stack swapping and GPR / FPR init for Instrumented only
  if (ROLE == Role::Instrumented) {
    // Allocating fake stack
    void *newStack = QBDI::alignedAlloc(STACK_SIZE, 16);
    if (newStack == nullptr) {
      fprintf(stderr, "Failed to allocate fake stack\n");
      exit(QBDIPRELOAD_ERR_STARTUP_FAILED);
    }

    // Copying the initial thread state
    threadStateToGPRState(threadState, &ENTRY_GPR);
    floatStateToFPRState(floatState, &ENTRY_FPR);

    // Swapping to fake stack
#if defined(QBDI_ARCH_X86)
    threadState->THREAD_STATE_BP = (rword)newStack + STACK_SIZE - 8;
    threadState->THREAD_STATE_SP = threadState->THREAD_STATE_BP - 44;
    memcpy((void *)threadState->THREAD_STATE_SP, (void *)ENTRY_GPR.esp, 44);
#else
    threadState->THREAD_STATE_BP = (rword)newStack + STACK_SIZE - 8;
    threadState->THREAD_STATE_SP = threadState->THREAD_STATE_BP;
#endif
  } else if (ROLE == Role::Master) {
    // LC_UNIXTHREAD binaries use a different calling convention
    // This allow to call catchEntrypoint, and have not side effect
    // as original execution is never resumed in Master mode
    if ((threadState->THREAD_STATE_SP & 0x8) == 0) {
      threadState->THREAD_STATE_SP -= 8;
    }
  }

  return QBDIPRELOAD_NO_ERROR;
}

int QBDI::qbdipreload_on_run(QBDI::VMInstanceRef vm, QBDI::rword start,
                             QBDI::rword stop) {
  return QBDIPRELOAD_NOT_HANDLED;
}

int QBDI::qbdipreload_on_exit(int status) {
  if (ROLE == Role::Instrumented) {
    cleanup_instrumentation();
  }
  return QBDIPRELOAD_NO_ERROR;
}

int QBDI::qbdipreload_on_start(void *main) {
  int ctrlfds[2];
  int datafds[2];
  int outputDBIfds[2];
  int outputDBGfds[2];
  int dummyfds[2];
  if (pipe(ctrlfds) != 0 or pipe(datafds) != 0 or pipe(outputDBIfds) != 0 or
      pipe(outputDBGfds) != 0 or pipe(dummyfds) != 0) {
    fprintf(stderr,
            "validator: fatal error, fail create pipe for intrumented process "
            "!\n\n");
    exit(0);
  }

  INSTRUMENTED = fork();
  if (INSTRUMENTED == 0) {

    ROLE = Role::Instrumented;
    ctrlfd = ctrlfds[0];
    datafd = datafds[1];
    close(ctrlfds[1]);
    close(datafds[0]);
    if (dup2(outputDBIfds[1], 1) == -1) {
      perror("instrumented: fail to dup2");
    }
    close(outputDBIfds[0]);
    close(outputDBIfds[1]);
    close(outputDBGfds[0]);
    close(outputDBGfds[1]);
    close(dummyfds[0]);
    close(dummyfds[1]);

    QBDI::qbdipreload_hook_main(main);
    return QBDIPRELOAD_NO_ERROR;
  }

  DEBUGGED = fork();
  if (DEBUGGED == 0) {

    ROLE = Role::Debugged;
    if (dup2(dummyfds[0], ctrlfds[0]) == -1 or
        dup2(dummyfds[1], datafds[1]) == -1 or dup2(outputDBGfds[1], 1) == -1) {
      perror("debugged: fail to dup2");
    }
    close(ctrlfds[1]);
    close(datafds[0]);
    close(outputDBIfds[0]);
    close(outputDBIfds[1]);
    close(outputDBGfds[0]);
    close(outputDBGfds[1]);
    close(dummyfds[0]);
    close(dummyfds[1]);

    // We put ourself to sleep, waiting for our charming prince
    task_suspend(mach_task_self());
    return QBDIPRELOAD_NO_ERROR;
  }

  ctrlfd = ctrlfds[1];
  datafd = datafds[0];
  outputDBIfd = outputDBIfds[0];
  outputDBGfd = outputDBGfds[0];
  close(ctrlfds[0]);
  close(datafds[1]);
  close(outputDBIfds[1]);
  close(outputDBGfds[1]);
  close(dummyfds[0]);
  close(dummyfds[1]);

  QBDI::qbdipreload_hook_main(main);
  ROLE = Role::Master;

  return QBDIPRELOAD_NO_ERROR;
}
