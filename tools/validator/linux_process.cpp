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
#include "linux_process.h"
#include "validator.h"

#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include "Utility/LogSys.h"

#ifndef QBDI_ARCH_ARM
void LinuxProcess::setBreakpoint(void *address) {
  this->brk_address = address;
  this->brk_value = ptrace(PTRACE_PEEKDATA, this->pid, address, NULL);
  if (ptrace(PTRACE_POKEDATA, this->pid, address,
             BRK_INS | (this->brk_value & (~BRK_MASK))) == -1) {
    QBDI_ERROR("Failed to set breakpoint: {}", strerror(errno));
    exit(VALIDATOR_ERR_UNEXPECTED_API_FAILURE);
  }
}
#endif

void LinuxProcess::unsetBreakpoint() {
  if (ptrace(PTRACE_POKEDATA, this->pid, this->brk_address, this->brk_value) ==
      -1) {
    QBDI_ERROR("Failed to unset breakpoint: {}", strerror(errno));
    exit(VALIDATOR_ERR_UNEXPECTED_API_FAILURE);
  }
}

void LinuxProcess::continueExecution() {
  ptrace(PTRACE_CONT, this->pid, NULL, NULL);
}

int LinuxProcess::waitForStatus() {
  int status = 0;
  waitpid(this->pid, &status, 0);
#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
  if (WSTOPSIG(status) == SIGBRK) {
    GPR_STRUCT user;
    ptrace(PTRACE_GETREGS, this->pid, NULL, &user);
    fix_GPR_STRUCT(&user);
    ptrace(PTRACE_SETREGS, this->pid, NULL, &user);
  }
#endif
  return status;
}

bool hasExited(int status) { return WIFEXITED(status) > 0; }

bool hasStopped(int status) {
  return WSTOPSIG(status) == SIGBRK || WSTOPSIG(status) == SIGSTOP;
}

bool hasCrashed(int status) {
  return hasExited(status) == false && hasStopped(status) == false;
}
