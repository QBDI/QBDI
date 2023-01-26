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
#include <stdlib.h>

#include "linux_X86.h"

void qbdipreload_threadCtxToGPRState(const void *gprCtx, GPRState *gprState) {
  const ucontext_t *uap = (const ucontext_t *)gprCtx;

  gprState->eax = uap->uc_mcontext.gregs[REG_EAX];
  gprState->ebx = uap->uc_mcontext.gregs[REG_EBX];
  gprState->ecx = uap->uc_mcontext.gregs[REG_ECX];
  gprState->edx = uap->uc_mcontext.gregs[REG_EDX];
  gprState->esi = uap->uc_mcontext.gregs[REG_ESI];
  gprState->edi = uap->uc_mcontext.gregs[REG_EDI];
  gprState->ebp = uap->uc_mcontext.gregs[REG_EBP];
  gprState->esp = uap->uc_mcontext.gregs[REG_ESP];
  gprState->eip = uap->uc_mcontext.gregs[REG_EIP];
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
  fprState->rfcw = uap->uc_mcontext.fpregs->cw;
  fprState->rfsw = uap->uc_mcontext.fpregs->sw;
  fprState->ftw = 0;
  int i, j = uap->uc_mcontext.fpregs->tag;
  for (i = 0; i < 8; i++) {
    if (((j >> (i * 2)) & 0x3) != 0x3) {
      fprState->ftw |= 1 << i;
    }
  }
  fprState->mxcsr = 0x1f80;
  fprState->mxcsrmask = 0xffff;
}

void prepareStack(void *newStack, size_t sizeStack, ucontext_t *uap) {
  // need to copy stack arguments
  // main have only three arguments (int argc, char** argv, char** envp) => 0xc
  // align on 0x10
  memcpy(newStack + sizeStack - 0x10,
         (void *)(uap->uc_mcontext.gregs[REG_ESP] + 0x4), 0x10);
  uap->uc_mcontext.gregs[REG_ESP] = (rword)newStack + sizeStack - 0x14;
  uap->uc_mcontext.gregs[REG_EBP] = (rword)newStack + sizeStack - 0x14;
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
