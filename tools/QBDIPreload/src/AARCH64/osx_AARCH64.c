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
#include "osx_AARCH64.h"
#include "QBDIPreload.h"

#include <mach/thread_act.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <QBDI.h>

void qbdipreload_threadCtxToGPRState(const void *gprCtx, GPRState *gprState) {
  const arm_thread_state64_t *ts = (arm_thread_state64_t *)gprCtx;

  gprState->x0 = ts->__x[0];
  gprState->x1 = ts->__x[1];
  gprState->x2 = ts->__x[2];
  gprState->x3 = ts->__x[3];
  gprState->x4 = ts->__x[4];
  gprState->x5 = ts->__x[5];
  gprState->x6 = ts->__x[6];
  gprState->x7 = ts->__x[7];
  gprState->x8 = ts->__x[8];
  gprState->x9 = ts->__x[9];
  gprState->x10 = ts->__x[10];
  gprState->x11 = ts->__x[11];
  gprState->x12 = ts->__x[12];
  gprState->x13 = ts->__x[13];
  gprState->x14 = ts->__x[14];
  gprState->x15 = ts->__x[15];
  gprState->x16 = ts->__x[16];
  gprState->x17 = ts->__x[17];
  gprState->x18 = ts->__x[18];
  gprState->x19 = ts->__x[19];
  gprState->x20 = ts->__x[20];
  gprState->x21 = ts->__x[21];
  gprState->x22 = ts->__x[22];
  gprState->x23 = ts->__x[23];
  gprState->x24 = ts->__x[24];
  gprState->x25 = ts->__x[25];
  gprState->x26 = ts->__x[26];
  gprState->x27 = ts->__x[27];
  gprState->x28 = ts->__x[28];
  gprState->x29 = __darwin_arm_thread_state64_get_fp(*ts);
  gprState->lr = __darwin_arm_thread_state64_get_lr(*ts);
  gprState->sp = __darwin_arm_thread_state64_get_sp(*ts);
  gprState->nzcv = ts->__cpsr;
  gprState->pc = __darwin_arm_thread_state64_get_pc(*ts);
}

void qbdipreload_floatCtxToFPRState(const void *fprCtx, FPRState *fprState) {
  const arm_neon_state64_t *fs = (arm_neon_state64_t *)fprCtx;

#if __DARWIN_UNIX03
#define V_POS __v
#define FPSR_POS __fpsr
#define FPCR_POS __fpcr
#else
#define V_POS q
#define FPSR_POS fpsr
#define FPCR_POS fpcr
#endif

  fprState->v0 = fs->V_POS[0];
  fprState->v1 = fs->V_POS[1];
  fprState->v2 = fs->V_POS[2];
  fprState->v3 = fs->V_POS[3];
  fprState->v4 = fs->V_POS[4];
  fprState->v5 = fs->V_POS[5];
  fprState->v6 = fs->V_POS[6];
  fprState->v7 = fs->V_POS[7];
  fprState->v8 = fs->V_POS[8];
  fprState->v9 = fs->V_POS[9];
  fprState->v10 = fs->V_POS[10];
  fprState->v11 = fs->V_POS[11];
  fprState->v12 = fs->V_POS[12];
  fprState->v13 = fs->V_POS[13];
  fprState->v14 = fs->V_POS[14];
  fprState->v15 = fs->V_POS[15];
  fprState->v16 = fs->V_POS[16];
  fprState->v17 = fs->V_POS[17];
  fprState->v18 = fs->V_POS[18];
  fprState->v19 = fs->V_POS[19];
  fprState->v20 = fs->V_POS[20];
  fprState->v21 = fs->V_POS[21];
  fprState->v22 = fs->V_POS[22];
  fprState->v23 = fs->V_POS[23];
  fprState->v24 = fs->V_POS[24];
  fprState->v25 = fs->V_POS[25];
  fprState->v26 = fs->V_POS[26];
  fprState->v27 = fs->V_POS[27];
  fprState->v28 = fs->V_POS[28];
  fprState->v29 = fs->V_POS[29];
  fprState->v30 = fs->V_POS[30];
  fprState->v31 = fs->V_POS[31];
  fprState->fpsr = fs->FPSR_POS;
  fprState->fpcr = fs->FPCR_POS;
}
