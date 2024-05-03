/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2024 Quarkslab
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

#include "linux_X86.h"
#include <stdlib.h>
#include <sys/ptrace.h>
#include "validator.h"

#include "Utility/LogSys.h"

void userToGPRState(const GPR_STRUCT *user, QBDI::GPRState *gprState) {
  gprState->eax = user->eax;
  gprState->ebx = user->ebx;
  gprState->ecx = user->ecx;
  gprState->edx = user->edx;
  gprState->esi = user->esi;
  gprState->edi = user->edi;
  gprState->ebp = user->ebp;
  gprState->esp = user->esp;
  gprState->eip = user->eip;
  gprState->eflags = user->eflags;
}

void userToFPRState(const FPR_STRUCT *user, QBDI::FPRState *fprState) {
  memcpy(&fprState->stmm0, &user->st_space[0], 10);
  memcpy(&fprState->stmm1, &user->st_space[4], 10);
  memcpy(&fprState->stmm2, &user->st_space[8], 10);
  memcpy(&fprState->stmm3, &user->st_space[12], 10);
  memcpy(&fprState->stmm4, &user->st_space[16], 10);
  memcpy(&fprState->stmm5, &user->st_space[20], 10);
  memcpy(&fprState->stmm6, &user->st_space[24], 10);
  memcpy(&fprState->stmm7, &user->st_space[28], 10);
  memcpy(&fprState->xmm0, &user->xmm_space[0], 16);
  memcpy(&fprState->xmm1, &user->xmm_space[4], 16);
  memcpy(&fprState->xmm2, &user->xmm_space[8], 16);
  memcpy(&fprState->xmm3, &user->xmm_space[12], 16);
  memcpy(&fprState->xmm4, &user->xmm_space[16], 16);
  memcpy(&fprState->xmm5, &user->xmm_space[20], 16);
  memcpy(&fprState->xmm6, &user->xmm_space[24], 16);
  memcpy(&fprState->xmm7, &user->xmm_space[28], 16);
  fprState->ftw = user->twd;
  fprState->mxcsrmask = 0xffff;
  fprState->fop = user->fop;
  fprState->rfcw = user->cwd;
  fprState->rfsw = user->swd;
  fprState->mxcsr = user->mxcsr;
}

void LinuxProcess::getProcessGPR(QBDI::GPRState *gprState) {
  GPR_STRUCT user;
  if (ptrace(PTRACE_GETREGS, this->pid, NULL, &user) == -1) {
    QBDI_ERROR("Failed to get GPR state: {}", strerror(errno));
    exit(VALIDATOR_ERR_UNEXPECTED_API_FAILURE);
  }
  userToGPRState(&user, gprState);
}

void LinuxProcess::getProcessFPR(QBDI::FPRState *fprState) {
  FPR_STRUCT user;
  if (ptrace(PTRACE_GETFPXREGS, this->pid, NULL, &user) == -1) {
    QBDI_ERROR("Failed to get FPR state: {}", strerror(errno));
    exit(VALIDATOR_ERR_UNEXPECTED_API_FAILURE);
  }
  userToFPRState(&user, fprState);
}
