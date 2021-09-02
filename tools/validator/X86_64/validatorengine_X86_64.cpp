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
#include <algorithm>
#include <cstring>
#include <inttypes.h>
#include "validatorengine.h"

void ValidatorEngine::compareState(const QBDI::GPRState *gprStateDbg,
                                   const QBDI::FPRState *fprStateDbg,
                                   const QBDI::GPRState *gprStateInstr,
                                   const QBDI::FPRState *fprStateInstr) {
  ssize_t e;

  // GPR
  if ((e = diffGPR(0, gprStateDbg->rax, gprStateInstr->rax)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(1, gprStateDbg->rbx, gprStateInstr->rbx)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(2, gprStateDbg->rcx, gprStateInstr->rcx)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(3, gprStateDbg->rdx, gprStateInstr->rdx)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(4, gprStateDbg->rsi, gprStateInstr->rsi)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(5, gprStateDbg->rdi, gprStateInstr->rdi)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(6, gprStateDbg->r8, gprStateInstr->r8)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(7, gprStateDbg->r9, gprStateInstr->r9)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(8, gprStateDbg->r10, gprStateInstr->r10)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(9, gprStateDbg->r11, gprStateInstr->r11)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(10, gprStateDbg->r12, gprStateInstr->r12)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(11, gprStateDbg->r13, gprStateInstr->r13)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(12, gprStateDbg->r14, gprStateInstr->r14)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(13, gprStateDbg->r15, gprStateInstr->r15)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(14, gprStateDbg->rbp, gprStateInstr->rbp)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(15, gprStateDbg->rsp, gprStateInstr->rsp)) != -1)
    curLogEntry->errorIDs.push_back(e);

  // FPR
  union {
    QBDI::MMSTReg st;
    struct {
      uint32_t m0;
      uint32_t m1;
      uint16_t e;
      uint16_t reserved;
    };
  } streg1, streg2;
#define DIFF_ST(name, dbg, instr)                                  \
  streg1.st = dbg;                                                 \
  streg2.st = instr;                                               \
  if ((e = diffSPR(name ".m[0:32]", streg1.m0, streg2.m0)) != -1)  \
    curLogEntry->errorIDs.push_back(e);                            \
  if ((e = diffSPR(name ".m[32:64]", streg1.m1, streg2.m1)) != -1) \
    curLogEntry->errorIDs.push_back(e);                            \
  if ((e = diffSPR(name ".e", streg1.e, streg2.e)) != -1)          \
    curLogEntry->errorIDs.push_back(e);

  DIFF_ST("st0", fprStateDbg->stmm0, fprStateInstr->stmm0);
  DIFF_ST("st1", fprStateDbg->stmm1, fprStateInstr->stmm1);
  DIFF_ST("st2", fprStateDbg->stmm2, fprStateInstr->stmm2);
  DIFF_ST("st3", fprStateDbg->stmm3, fprStateInstr->stmm3);
  DIFF_ST("st4", fprStateDbg->stmm4, fprStateInstr->stmm4);
  DIFF_ST("st5", fprStateDbg->stmm5, fprStateInstr->stmm5);
  DIFF_ST("st6", fprStateDbg->stmm6, fprStateInstr->stmm6);
  DIFF_ST("st7", fprStateDbg->stmm7, fprStateInstr->stmm7);

  union {
    char xmm[16];
    struct {
      uint32_t m0;
      uint32_t m1;
      uint32_t m2;
      uint32_t m3;
    };
  } xmmreg1, xmmreg2;
#define DIFF_XMM(name, dbg, instr)                                  \
  memcpy(xmmreg1.xmm, dbg, 16);                                     \
  memcpy(xmmreg2.xmm, instr, 16);                                   \
  if ((e = diffSPR(name "[0:32]", xmmreg1.m0, xmmreg2.m0)) != -1)   \
    curLogEntry->errorIDs.push_back(e);                             \
  if ((e = diffSPR(name "[32:64]", xmmreg1.m1, xmmreg2.m1)) != -1)  \
    curLogEntry->errorIDs.push_back(e);                             \
  if ((e = diffSPR(name "[64:96]", xmmreg1.m2, xmmreg2.m2)) != -1)  \
    curLogEntry->errorIDs.push_back(e);                             \
  if ((e = diffSPR(name "[96:128]", xmmreg1.m3, xmmreg2.m3)) != -1) \
    curLogEntry->errorIDs.push_back(e);

  DIFF_XMM("xmm0", fprStateDbg->xmm0, fprStateInstr->xmm0);
  DIFF_XMM("xmm1", fprStateDbg->xmm1, fprStateInstr->xmm1);
  DIFF_XMM("xmm2", fprStateDbg->xmm2, fprStateInstr->xmm2);
  DIFF_XMM("xmm3", fprStateDbg->xmm3, fprStateInstr->xmm3);
  DIFF_XMM("xmm4", fprStateDbg->xmm4, fprStateInstr->xmm4);
  DIFF_XMM("xmm5", fprStateDbg->xmm5, fprStateInstr->xmm5);
  DIFF_XMM("xmm6", fprStateDbg->xmm6, fprStateInstr->xmm6);
  DIFF_XMM("xmm7", fprStateDbg->xmm7, fprStateInstr->xmm7);
  DIFF_XMM("xmm8", fprStateDbg->xmm8, fprStateInstr->xmm8);
  DIFF_XMM("xmm9", fprStateDbg->xmm9, fprStateInstr->xmm9);
  DIFF_XMM("xmm10", fprStateDbg->xmm10, fprStateInstr->xmm10);
  DIFF_XMM("xmm11", fprStateDbg->xmm11, fprStateInstr->xmm11);
  DIFF_XMM("xmm12", fprStateDbg->xmm12, fprStateInstr->xmm12);
  DIFF_XMM("xmm13", fprStateDbg->xmm13, fprStateInstr->xmm13);
  DIFF_XMM("xmm14", fprStateDbg->xmm14, fprStateInstr->xmm14);
  DIFF_XMM("xmm15", fprStateDbg->xmm15, fprStateInstr->xmm15);
// FIXME
#if 0
    DIFF_XMM("ymm0", fprStateDbg->ymm0, fprStateInstr->ymm0);
    DIFF_XMM("ymm1", fprStateDbg->ymm1, fprStateInstr->ymm1);
    DIFF_XMM("ymm2", fprStateDbg->ymm2, fprStateInstr->ymm2);
    DIFF_XMM("ymm3", fprStateDbg->ymm3, fprStateInstr->ymm3);
    DIFF_XMM("ymm4", fprStateDbg->ymm4, fprStateInstr->ymm4);
    DIFF_XMM("ymm5", fprStateDbg->ymm5, fprStateInstr->ymm5);
    DIFF_XMM("ymm6", fprStateDbg->ymm6, fprStateInstr->ymm6);
    DIFF_XMM("ymm7", fprStateDbg->ymm7, fprStateInstr->ymm7);
    DIFF_XMM("ymm8", fprStateDbg->ymm8, fprStateInstr->ymm8);
    DIFF_XMM("ymm9", fprStateDbg->ymm9, fprStateInstr->ymm9);
    DIFF_XMM("ymm10", fprStateDbg->ymm10, fprStateInstr->ymm10);
    DIFF_XMM("ymm11", fprStateDbg->ymm11, fprStateInstr->ymm11);
    DIFF_XMM("ymm12", fprStateDbg->ymm12, fprStateInstr->ymm12);
    DIFF_XMM("ymm13", fprStateDbg->ymm13, fprStateInstr->ymm13);
    DIFF_XMM("ymm14", fprStateDbg->ymm14, fprStateInstr->ymm14);
    DIFF_XMM("ymm15", fprStateDbg->ymm15, fprStateInstr->ymm15);
#endif

  if ((e = diff("fcw", fprStateDbg->rfcw, fprStateInstr->rfcw)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diff("fsw", fprStateDbg->rfsw, fprStateInstr->rfsw)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diff("ftw", fprStateDbg->ftw, fprStateInstr->ftw)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diff("fop", fprStateDbg->fop, fprStateInstr->fop)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diff("mxcsr", fprStateDbg->mxcsr, fprStateInstr->mxcsr)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diff("mxcsrmask", fprStateDbg->mxcsrmask,
                fprStateInstr->mxcsrmask)) != -1)
    curLogEntry->errorIDs.push_back(e);
  // Clear the auxilliary carry flag which generates noisy output
  QBDI::rword eflagsDbg = (gprStateDbg->eflags) & (gprStateDbg->eflags ^ 0x4);
  QBDI::rword eflagsInstr =
      (gprStateInstr->eflags) & (gprStateInstr->eflags ^ 0x4);
  if ((e = diffGPR(17, eflagsDbg, eflagsInstr)) != -1)
    curLogEntry->errorIDs.push_back(e);
}
