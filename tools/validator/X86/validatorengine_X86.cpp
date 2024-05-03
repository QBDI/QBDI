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
  if ((e = diffGPR(0, gprStateDbg->eax, gprStateInstr->eax)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(1, gprStateDbg->ebx, gprStateInstr->ebx)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(2, gprStateDbg->ecx, gprStateInstr->ecx)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(3, gprStateDbg->edx, gprStateInstr->edx)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(4, gprStateDbg->esi, gprStateInstr->esi)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(5, gprStateDbg->edi, gprStateInstr->edi)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(6, gprStateDbg->ebp, gprStateInstr->ebp)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(7, gprStateDbg->esp, gprStateInstr->esp)) != -1)
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
#if 0
    DIFF_XMM("ymm0", fprStateDbg->ymm0, fprStateInstr->ymm0);
    DIFF_XMM("ymm1", fprStateDbg->ymm1, fprStateInstr->ymm1);
    DIFF_XMM("ymm2", fprStateDbg->ymm2, fprStateInstr->ymm2);
    DIFF_XMM("ymm3", fprStateDbg->ymm3, fprStateInstr->ymm3);
    DIFF_XMM("ymm4", fprStateDbg->ymm4, fprStateInstr->ymm4);
    DIFF_XMM("ymm5", fprStateDbg->ymm5, fprStateInstr->ymm5);
    DIFF_XMM("ymm6", fprStateDbg->ymm6, fprStateInstr->ymm6);
    DIFF_XMM("ymm7", fprStateDbg->ymm7, fprStateInstr->ymm7);
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
