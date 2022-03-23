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
#include "instrumented.h"
#include "linux_process.h"
#include "master.h"
#include "validator.h"
#include "QBDIPreload.h"

#include <dlfcn.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>

#include "QBDI/Memory.hpp"
#include "QBDI/Platform.h"
#include "Utility/LogSys.h"

static bool INSTRUMENTED = false;
static bool MASTER = false;
static pid_t debugged, instrumented;
int ctrlfd, datafd;

QBDIPRELOAD_INIT;

int QBDI::qbdipreload_on_main(int argc, char **argv) {
  if (INSTRUMENTED) {
    if (getenv("QBDI_DEBUG") != NULL) {
      QBDI::setLogPriority(QBDI::LogPriority::DEBUG);
    } else {
      QBDI::setLogPriority(QBDI::LogPriority::WARNING);
    }
    return QBDIPRELOAD_NOT_HANDLED;

  } else {
    LinuxProcess *debuggedProcess = nullptr;
    debuggedProcess = new LinuxProcess(debugged);
    start_master(debuggedProcess, instrumented, ctrlfd, datafd);
    delete debuggedProcess;
    return QBDIPRELOAD_NO_ERROR;
  }
}

int QBDI::qbdipreload_on_premain(void *gprCtx, void *fpuCtx) {
  if (INSTRUMENTED)
    return QBDIPRELOAD_NOT_HANDLED;
  return QBDIPRELOAD_NO_ERROR;
}

int QBDI::qbdipreload_on_run(QBDI::VMInstanceRef vm, QBDI::rword start,
                             QBDI::rword stop) {
  start_instrumented(vm, start, stop, ctrlfd, datafd);
  return QBDIPRELOAD_NOT_HANDLED;
}

void killChild() {
  kill(debugged, SIGKILL);
  kill(instrumented, SIGKILL);
}

int QBDI::qbdipreload_on_exit(int status) {
  if (INSTRUMENTED) {
    cleanup_instrumentation();
  } else if (MASTER) {
    killChild();
  }
  return QBDIPRELOAD_NO_ERROR;
}

int QBDI::qbdipreload_on_start(void *main) {
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);

  int ctrlfds[2];
  int datafds[2];
  if (pipe(ctrlfds) != 0 || pipe(datafds) != 0) {
    fprintf(stderr,
            "validator: fatal error, fail create pipe for intrumented process "
            "!\n\n");
    exit(0);
  }

  instrumented = fork();
  if (instrumented == 0) {

    ctrlfd = ctrlfds[0];
    datafd = datafds[1];
    close(ctrlfds[1]);
    close(datafds[0]);

    signal(SIGCHLD, SIG_IGN);
    INSTRUMENTED = true;
    return QBDIPRELOAD_NOT_HANDLED;
  }

  debugged = fork();
  if (debugged == 0) {
    // don't close all pipe to have the same filedescriptor in the
    // debugged and the instrumented process
    close(ctrlfds[1]);
    close(datafds[0]);
    signal(SIGCHLD, SIG_IGN);

    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    raise(SIGSTOP);
    return QBDIPRELOAD_NO_ERROR;
  }

  if (ptrace(PTRACE_ATTACH, debugged, NULL, NULL) == -1) {
    fprintf(stderr, "validator: fatal error, PTRACE_ATTACH failed !\n\n");
    kill(instrumented, SIGKILL);
    kill(debugged, SIGKILL);
    exit(0);
  }

  ctrlfd = ctrlfds[1];
  datafd = datafds[0];
  close(ctrlfds[0]);
  close(datafds[1]);

  int wstatus;
  do {
    waitpid(debugged, &wstatus, WUNTRACED | WCONTINUED);
  } while (!WIFSTOPPED(wstatus));

  signal(SIGCHLD, SIG_IGN);
  MASTER = true;
  atexit(killChild);
  return QBDIPRELOAD_NOT_HANDLED;
}
