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

#include "linux_AARCH64.h"

// see arch/arm64/include/uapi/asm/sigcontext.h
typedef struct {
  struct {
    uint32_t magic;
    uint32_t size;
  } head;
  uint32_t fpsr;
  uint32_t fpcr;
  __uint128_t vregs[32];
} fpsimd_context;

void qbdipreload_threadCtxToGPRState(const void *gprCtx, GPRState *gprState) {
  const ucontext_t *uap = (const ucontext_t *)gprCtx;

  gprState->x0 = uap->uc_mcontext.regs[0];
  gprState->x1 = uap->uc_mcontext.regs[1];
  gprState->x2 = uap->uc_mcontext.regs[2];
  gprState->x3 = uap->uc_mcontext.regs[3];
  gprState->x4 = uap->uc_mcontext.regs[4];
  gprState->x5 = uap->uc_mcontext.regs[5];
  gprState->x6 = uap->uc_mcontext.regs[6];
  gprState->x7 = uap->uc_mcontext.regs[7];
  gprState->x8 = uap->uc_mcontext.regs[8];
  gprState->x9 = uap->uc_mcontext.regs[9];
  gprState->x10 = uap->uc_mcontext.regs[10];
  gprState->x11 = uap->uc_mcontext.regs[11];
  gprState->x12 = uap->uc_mcontext.regs[12];
  gprState->x13 = uap->uc_mcontext.regs[13];
  gprState->x14 = uap->uc_mcontext.regs[14];
  gprState->x15 = uap->uc_mcontext.regs[15];
  gprState->x16 = uap->uc_mcontext.regs[16];
  gprState->x17 = uap->uc_mcontext.regs[17];
  gprState->x18 = uap->uc_mcontext.regs[18];
  gprState->x19 = uap->uc_mcontext.regs[19];
  gprState->x20 = uap->uc_mcontext.regs[20];
  gprState->x21 = uap->uc_mcontext.regs[21];
  gprState->x22 = uap->uc_mcontext.regs[22];
  gprState->x23 = uap->uc_mcontext.regs[23];
  gprState->x24 = uap->uc_mcontext.regs[24];
  gprState->x25 = uap->uc_mcontext.regs[25];
  gprState->x26 = uap->uc_mcontext.regs[26];
  gprState->x27 = uap->uc_mcontext.regs[27];
  gprState->x28 = uap->uc_mcontext.regs[28];
  gprState->x29 = uap->uc_mcontext.regs[29];
  gprState->lr = uap->uc_mcontext.regs[30];

  gprState->sp = uap->uc_mcontext.sp;
  gprState->pc = uap->uc_mcontext.pc;
  gprState->nzcv = uap->uc_mcontext.pstate & 0xf0000000;
}

void qbdipreload_floatCtxToFPRState(const void *fprCtx, FPRState *fprState) {
  const ucontext_t *uap = (const ucontext_t *)fprCtx;
  const fpsimd_context *fuap =
      (const fpsimd_context *)&(uap->uc_mcontext.__reserved);

  fprState->v0 = fuap->vregs[0];
  fprState->v1 = fuap->vregs[1];
  fprState->v2 = fuap->vregs[2];
  fprState->v3 = fuap->vregs[3];
  fprState->v4 = fuap->vregs[4];
  fprState->v5 = fuap->vregs[5];
  fprState->v6 = fuap->vregs[6];
  fprState->v7 = fuap->vregs[7];
  fprState->v8 = fuap->vregs[8];
  fprState->v9 = fuap->vregs[9];
  fprState->v10 = fuap->vregs[10];
  fprState->v11 = fuap->vregs[11];
  fprState->v12 = fuap->vregs[12];
  fprState->v13 = fuap->vregs[13];
  fprState->v14 = fuap->vregs[14];
  fprState->v15 = fuap->vregs[15];
  fprState->v16 = fuap->vregs[16];
  fprState->v17 = fuap->vregs[17];
  fprState->v18 = fuap->vregs[18];
  fprState->v19 = fuap->vregs[19];
  fprState->v20 = fuap->vregs[20];
  fprState->v21 = fuap->vregs[21];
  fprState->v22 = fuap->vregs[22];
  fprState->v23 = fuap->vregs[23];
  fprState->v24 = fuap->vregs[24];
  fprState->v25 = fuap->vregs[25];
  fprState->v26 = fuap->vregs[26];
  fprState->v27 = fuap->vregs[27];
  fprState->v28 = fuap->vregs[28];
  fprState->v29 = fuap->vregs[29];
  fprState->v30 = fuap->vregs[30];
  fprState->v31 = fuap->vregs[31];
  fprState->fpcr = fuap->fpcr;
  fprState->fpsr = fuap->fpsr;
}

void prepareStack(void *newStack, size_t sizeStack, ucontext_t *uap) {
  uap->uc_mcontext.sp = (rword)newStack + sizeStack - 16;
  uap->uc_mcontext.regs[29] = (rword)newStack + sizeStack - 16;
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
