/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2023 Quarkslab
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

#include "linux_AARCH64.h"
#include <stdlib.h>
#include "validator.h"

#include "Utility/LogSys.h"

void userToGPRState(const GPR_STRUCT *user, QBDI::GPRState *gprState) {
  gprState->x0 = user->regs[0];
  gprState->x1 = user->regs[1];
  gprState->x2 = user->regs[2];
  gprState->x3 = user->regs[3];
  gprState->x4 = user->regs[4];
  gprState->x5 = user->regs[5];
  gprState->x6 = user->regs[6];
  gprState->x7 = user->regs[7];
  gprState->x8 = user->regs[8];
  gprState->x9 = user->regs[9];
  gprState->x10 = user->regs[10];
  gprState->x11 = user->regs[11];
  gprState->x12 = user->regs[12];
  gprState->x13 = user->regs[13];
  gprState->x14 = user->regs[14];
  gprState->x15 = user->regs[15];
  gprState->x16 = user->regs[16];
  gprState->x17 = user->regs[17];
  gprState->x18 = user->regs[18];
  gprState->x19 = user->regs[19];
  gprState->x20 = user->regs[20];
  gprState->x21 = user->regs[21];
  gprState->x22 = user->regs[22];
  gprState->x23 = user->regs[23];
  gprState->x24 = user->regs[24];
  gprState->x25 = user->regs[25];
  gprState->x26 = user->regs[26];
  gprState->x27 = user->regs[27];
  gprState->x28 = user->regs[28];
  gprState->x29 = user->regs[29];
  gprState->lr = user->regs[30];
  gprState->sp = user->sp;
  gprState->pc = user->pc;
  gprState->nzcv = user->pstate & 0xf0000000;
}

void userToFPRState(const FPR_STRUCT *user, QBDI::FPRState *fprState) {
  fprState->v0 = user->vregs[0];
  fprState->v1 = user->vregs[1];
  fprState->v2 = user->vregs[2];
  fprState->v3 = user->vregs[3];
  fprState->v4 = user->vregs[4];
  fprState->v5 = user->vregs[5];
  fprState->v6 = user->vregs[6];
  fprState->v7 = user->vregs[7];
  fprState->v8 = user->vregs[8];
  fprState->v9 = user->vregs[9];
  fprState->v10 = user->vregs[10];
  fprState->v11 = user->vregs[11];
  fprState->v12 = user->vregs[12];
  fprState->v13 = user->vregs[13];
  fprState->v14 = user->vregs[14];
  fprState->v15 = user->vregs[15];
  fprState->v16 = user->vregs[16];
  fprState->v17 = user->vregs[17];
  fprState->v18 = user->vregs[18];
  fprState->v19 = user->vregs[19];
  fprState->v20 = user->vregs[20];
  fprState->v21 = user->vregs[21];
  fprState->v22 = user->vregs[22];
  fprState->v23 = user->vregs[23];
  fprState->v24 = user->vregs[24];
  fprState->v25 = user->vregs[25];
  fprState->v26 = user->vregs[26];
  fprState->v27 = user->vregs[27];
  fprState->v28 = user->vregs[28];
  fprState->v29 = user->vregs[29];
  fprState->v30 = user->vregs[30];
  fprState->v31 = user->vregs[31];
  fprState->fpsr = user->fpsr & 0xf800009f;
  fprState->fpcr = user->fpcr & 0x07f79f00;
}

void LinuxProcess::getProcessGPR(QBDI::GPRState *gprState) {
  GPR_STRUCT user;
  struct iovec iovec = {&user, sizeof(user)};

  if (ptrace(PTRACE_GETREGSET, this->pid, ((void *)NT_PRSTATUS), &iovec) ==
      -1) {
    QBDI_ERROR("Failed to get GPR state: {}", strerror(errno));
    exit(VALIDATOR_ERR_UNEXPECTED_API_FAILURE);
  }
  userToGPRState(&user, gprState);
}

void LinuxProcess::getProcessFPR(QBDI::FPRState *fprState) {
  FPR_STRUCT user;
  struct iovec iovec = {&user, sizeof(user)};

  if (ptrace(PTRACE_GETREGSET, this->pid, ((void *)NT_FPREGSET), &iovec) ==
      -1) {
    QBDI_ERROR("Failed to get FPR state: {}", strerror(errno));
    exit(VALIDATOR_ERR_UNEXPECTED_API_FAILURE);
  }
  userToFPRState(&user, fprState);
}
