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
#include "darwin_exceptd.h"
#include "QBDIPreload.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <QBDI.h>

void qbdipreload_threadCtxToGPRState(const void *gprCtx, GPRState *gprState) {
  const x86_thread_state32_t *ts = (x86_thread_state32_t *)gprCtx;

  gprState->eax = ts->__eax;
  gprState->ebx = ts->__ebx;
  gprState->ecx = ts->__ecx;
  gprState->edx = ts->__edx;
  gprState->esi = ts->__esi;
  gprState->edi = ts->__edi;
  gprState->ebp = ts->__ebp;
  gprState->esp = ts->__esp;
  gprState->eip = ts->__eip;
  gprState->eflags = ts->__eflags;
}

void qbdipreload_floatCtxToFPRState(const void *fprCtx, FPRState *fprState) {
  const x86_float_state32_t *fs = (x86_float_state32_t *)fprCtx;

#define SYNC_STMM(ID) memcpy(&fprState->stmm##ID, &fs->__fpu_stmm##ID, 10);
  SYNC_STMM(0);
  SYNC_STMM(1);
  SYNC_STMM(2);
  SYNC_STMM(3);
  SYNC_STMM(4);
  SYNC_STMM(5);
  SYNC_STMM(6);
  SYNC_STMM(7);
#undef SYNC_STMM
#define SYNC_XMM(ID) memcpy(&fprState->xmm##ID, &fs->__fpu_xmm##ID, 16);
  SYNC_XMM(0);
  SYNC_XMM(1);
  SYNC_XMM(2);
  SYNC_XMM(3);
  SYNC_XMM(4);
  SYNC_XMM(5);
  SYNC_XMM(6);
  SYNC_XMM(7);
#undef SYNC_XMM
  memcpy(&fprState->rfcw, &fs->__fpu_fcw, 2);
  memcpy(&fprState->rfsw, &fs->__fpu_fsw, 2);
  memcpy(&fprState->ftw, &fs->__fpu_ftw, 1);
  memcpy(&fprState->rsrv1, &fs->__fpu_rsrv1, 1);
  memcpy(&fprState->fop, &fs->__fpu_fop, 2);
  memcpy(&fprState->mxcsr, &fs->__fpu_mxcsr, 4);
  memcpy(&fprState->mxcsrmask, &fs->__fpu_mxcsrmask, 4);
}
