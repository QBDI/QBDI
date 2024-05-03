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
#include <stdlib.h>

#include "linux_ARM.h"

void qbdipreload_threadCtxToGPRState(const void *gprCtx, GPRState *gprState) {
  const ucontext_t *uap = (const ucontext_t *)gprCtx;

  gprState->r0 = uap->uc_mcontext.arm_r0;
  gprState->r1 = uap->uc_mcontext.arm_r1;
  gprState->r2 = uap->uc_mcontext.arm_r2;
  gprState->r3 = uap->uc_mcontext.arm_r3;
  gprState->r4 = uap->uc_mcontext.arm_r4;
  gprState->r5 = uap->uc_mcontext.arm_r5;
  gprState->r6 = uap->uc_mcontext.arm_r6;
  gprState->r7 = uap->uc_mcontext.arm_r7;
  gprState->r8 = uap->uc_mcontext.arm_r8;
  gprState->r9 = uap->uc_mcontext.arm_r9;
  gprState->r10 = uap->uc_mcontext.arm_r10;
  gprState->r11 = uap->uc_mcontext.arm_fp;
  gprState->r12 = uap->uc_mcontext.arm_ip;
  gprState->sp = uap->uc_mcontext.arm_sp;
  gprState->lr = uap->uc_mcontext.arm_lr;
  gprState->pc = uap->uc_mcontext.arm_pc;
  if (((uap->uc_mcontext.arm_cpsr >> 5) & 1) == 1) {
    gprState->pc |= 1;
  }
  gprState->cpsr = uap->uc_mcontext.arm_cpsr & 0xf80f001f;
}

void qbdipreload_floatCtxToFPRState(const void *fprCtx, FPRState *fprState) {
  // Somehow not available under ARM
}

void prepareStack(void *newStack, size_t sizeStack, ucontext_t *uap) {
  uap->uc_mcontext.arm_sp = (rword)newStack + sizeStack - 8;
  uap->uc_mcontext.arm_fp = (rword)newStack + sizeStack - 8;
}

void removeConflictModule(VMInstanceRef vm, qbdi_MemoryMap *modules,
                          size_t size) {
  size_t i;
  for (i = 0; i < size; i++) {
    if ((modules[i].permission & QBDI_PF_EXEC) &&
        (strstr(modules[i].name, "libc-2.") ||
         strstr(modules[i].name, "ld-2.") ||
         strstr(modules[i].name, "libpthread-") ||
         strstr(modules[i].name, "libcofi") || modules[i].name[0] == 0)) {
      qbdi_removeInstrumentedRange(vm, modules[i].start, modules[i].end);
    }
  }
}
