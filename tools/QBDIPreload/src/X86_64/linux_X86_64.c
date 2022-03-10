/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2021 Quarkslab
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
#include <stdlib.h>

#include "linux_X86_64.h"

void qbdipreload_threadCtxToGPRState(const void *gprCtx, GPRState *gprState) {
  const ucontext_t *uap = (const ucontext_t *)gprCtx;

  gprState->rax = uap->uc_mcontext.gregs[REG_RAX];
  gprState->rbx = uap->uc_mcontext.gregs[REG_RBX];
  gprState->rcx = uap->uc_mcontext.gregs[REG_RCX];
  gprState->rdx = uap->uc_mcontext.gregs[REG_RDX];
  gprState->rsi = uap->uc_mcontext.gregs[REG_RSI];
  gprState->rdi = uap->uc_mcontext.gregs[REG_RDI];
  gprState->rbp = uap->uc_mcontext.gregs[REG_RBP];
  gprState->rsp = uap->uc_mcontext.gregs[REG_RSP];
  gprState->r8 = uap->uc_mcontext.gregs[REG_R8];
  gprState->r9 = uap->uc_mcontext.gregs[REG_R9];
  gprState->r10 = uap->uc_mcontext.gregs[REG_R10];
  gprState->r11 = uap->uc_mcontext.gregs[REG_R11];
  gprState->r12 = uap->uc_mcontext.gregs[REG_R12];
  gprState->r13 = uap->uc_mcontext.gregs[REG_R13];
  gprState->r14 = uap->uc_mcontext.gregs[REG_R14];
  gprState->r15 = uap->uc_mcontext.gregs[REG_R15];
  gprState->rip = uap->uc_mcontext.gregs[REG_RIP];
  gprState->eflags = uap->uc_mcontext.gregs[REG_EFL];
}

void qbdipreload_floatCtxToFPRState(const void *fprCtx, FPRState *fprState) {
  const ucontext_t *uap = (const ucontext_t *)fprCtx;

  memcpy(&fprState->stmm0, &uap->uc_mcontext.fpregs->_st[0], 10);
  memcpy(&fprState->stmm1, &uap->uc_mcontext.fpregs->_st[1], 10);
  memcpy(&fprState->stmm2, &uap->uc_mcontext.fpregs->_st[2], 10);
  memcpy(&fprState->stmm3, &uap->uc_mcontext.fpregs->_st[3], 10);
  memcpy(&fprState->stmm4, &uap->uc_mcontext.fpregs->_st[4], 10);
  memcpy(&fprState->stmm5, &uap->uc_mcontext.fpregs->_st[5], 10);
  memcpy(&fprState->stmm6, &uap->uc_mcontext.fpregs->_st[6], 10);
  memcpy(&fprState->stmm7, &uap->uc_mcontext.fpregs->_st[7], 10);
  memcpy(&fprState->xmm0, &uap->uc_mcontext.fpregs->_xmm[0], 16);
  memcpy(&fprState->xmm1, &uap->uc_mcontext.fpregs->_xmm[1], 16);
  memcpy(&fprState->xmm2, &uap->uc_mcontext.fpregs->_xmm[2], 16);
  memcpy(&fprState->xmm3, &uap->uc_mcontext.fpregs->_xmm[3], 16);
  memcpy(&fprState->xmm4, &uap->uc_mcontext.fpregs->_xmm[4], 16);
  memcpy(&fprState->xmm5, &uap->uc_mcontext.fpregs->_xmm[5], 16);
  memcpy(&fprState->xmm6, &uap->uc_mcontext.fpregs->_xmm[6], 16);
  memcpy(&fprState->xmm7, &uap->uc_mcontext.fpregs->_xmm[7], 16);
  memcpy(&fprState->xmm8, &uap->uc_mcontext.fpregs->_xmm[8], 16);
  memcpy(&fprState->xmm9, &uap->uc_mcontext.fpregs->_xmm[9], 16);
  memcpy(&fprState->xmm10, &uap->uc_mcontext.fpregs->_xmm[10], 16);
  memcpy(&fprState->xmm11, &uap->uc_mcontext.fpregs->_xmm[11], 16);
  memcpy(&fprState->xmm12, &uap->uc_mcontext.fpregs->_xmm[12], 16);
  memcpy(&fprState->xmm13, &uap->uc_mcontext.fpregs->_xmm[13], 16);
  memcpy(&fprState->xmm14, &uap->uc_mcontext.fpregs->_xmm[14], 16);
  memcpy(&fprState->xmm15, &uap->uc_mcontext.fpregs->_xmm[15], 16);
  fprState->rfcw = uap->uc_mcontext.fpregs->cwd;
  fprState->rfsw = uap->uc_mcontext.fpregs->swd;
  fprState->ftw = uap->uc_mcontext.fpregs->ftw & 0xFF;
  fprState->rsrv1 = uap->uc_mcontext.fpregs->ftw >> 8;
  fprState->fop = uap->uc_mcontext.fpregs->fop;
  fprState->mxcsr = uap->uc_mcontext.fpregs->mxcsr;
  fprState->mxcsrmask = uap->uc_mcontext.fpregs->mxcr_mask;
}

void prepareStack(void *newStack, size_t sizeStack, ucontext_t *uap) {
  uap->uc_mcontext.gregs[REG_RSP] = (rword)newStack + sizeStack - 8;
  uap->uc_mcontext.gregs[REG_RBP] = (rword)newStack + sizeStack - 8;
}

void removeConflictModule(VMInstanceRef vm, qbdi_MemoryMap *modules,
                          size_t size) {
  size_t i;
  for (i = 0; i < size; i++) {
    if ((modules[i].permission & QBDI_PF_EXEC) &&
        (strstr(modules[i].name, "libc-2.") ||
         strstr(modules[i].name, "libc.so.") ||
         strstr(modules[i].name, "ld-2.") ||
         strstr(modules[i].name, "ld-linux-") ||
         strstr(modules[i].name, "libpthread-") ||
         strstr(modules[i].name, "libcofi") || modules[i].name[0] == 0)) {
      qbdi_removeInstrumentedRange(vm, modules[i].start, modules[i].end);
    }
  }
}
