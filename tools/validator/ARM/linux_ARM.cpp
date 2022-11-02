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

#include "linux_ARM.h"
#include "validator.h"

#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>

#include "Utility/LogSys.h"

void LinuxProcess::setBreakpoint(void *address) {
  this->brk_address = (void *)(((QBDI::rword)address) & (~3));
  this->brk_value = ptrace(PTRACE_PEEKDATA, this->pid, this->brk_address, NULL);

  long bytecode, mask;
  if ((((QBDI::rword)address) & 1) == 0) {
    bytecode = 0xE7F001F0;
    mask = 0xFFFFFFFF;
  } else if ((((QBDI::rword)address) & 2) == 0) {
    // thumb 4 bytes aligned
    bytecode = 0xDE01;
    mask = 0xFFFF;
  } else {
    // thumb not 4 bytes aligned
    bytecode = 0xDE010000;
    mask = 0xFFFF0000;
  }
  if (ptrace(PTRACE_POKEDATA, this->pid, this->brk_address,
             bytecode | (this->brk_value & (~mask))) == -1) {
    QBDI_ERROR("Failed to set breakpoint: {}", strerror(errno));
    exit(VALIDATOR_ERR_UNEXPECTED_API_FAILURE);
  }
}

void userToGPRState(const GPR_STRUCT *user, QBDI::GPRState *gprState) {
  gprState->r0 = user->uregs[0];
  gprState->r1 = user->uregs[1];
  gprState->r2 = user->uregs[2];
  gprState->r3 = user->uregs[3];
  gprState->r4 = user->uregs[4];
  gprState->r5 = user->uregs[5];
  gprState->r6 = user->uregs[6];
  gprState->r7 = user->uregs[7];
  gprState->r8 = user->uregs[8];
  gprState->r9 = user->uregs[9];
  gprState->r10 = user->uregs[10];
  gprState->r11 = user->uregs[11];
  gprState->r12 = user->uregs[12];
  gprState->sp = user->uregs[13];
  gprState->lr = user->uregs[14];
  gprState->pc = user->uregs[15];
  if (((user->uregs[16] >> 5) & 1) == 1) {
    gprState->pc |= 1;
  }
  gprState->cpsr = user->uregs[16] & 0xf80f001f;
}

void userToFPRState(const FPR_STRUCT *user, QBDI::FPRState *fprState) {
  for (size_t i = 0; i < QBDI_NUM_FPR; i++) {
    *((uint64_t *)&fprState->vreg.d[i]) = user->d[i];
  }
  fprState->fpscr = user->FPSCR;
}

void LinuxProcess::getProcessGPR(QBDI::GPRState *gprState) {
  GPR_STRUCT user;
  int ret = ptrace(PTRACE_GETREGS, this->pid, NULL, &user);
  if (ret < 0) {
    perror("Fail to get GPR");
  }
  userToGPRState(&user, gprState);
}

void LinuxProcess::getProcessFPR(QBDI::FPRState *fprState) {
  FPR_STRUCT user;
  int ret = ptrace(PTRACE_GETVFPREGS, this->pid, NULL, &user);
  if (ret < 0) {
    perror("Fail to get FPR");
  }
  userToFPRState(&user, fprState);
}
