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
  if ((e = diffGPR(0, gprStateDbg->r0, gprStateInstr->r0)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(1, gprStateDbg->r1, gprStateInstr->r1)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(2, gprStateDbg->r2, gprStateInstr->r2)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(3, gprStateDbg->r3, gprStateInstr->r3)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(4, gprStateDbg->r4, gprStateInstr->r4)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(5, gprStateDbg->r5, gprStateInstr->r5)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(6, gprStateDbg->r6, gprStateInstr->r6)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(7, gprStateDbg->r7, gprStateInstr->r7)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(8, gprStateDbg->r8, gprStateInstr->r8)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(9, gprStateDbg->r9, gprStateInstr->r9)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(9, gprStateDbg->r9, gprStateInstr->r9)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(10, gprStateDbg->r10, gprStateInstr->r10)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(11, gprStateDbg->r11, gprStateInstr->r11)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(12, gprStateDbg->r12, gprStateInstr->r12)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(13, gprStateDbg->sp, gprStateInstr->sp)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(14, gprStateDbg->lr, gprStateInstr->lr)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(16, gprStateDbg->cpsr, gprStateInstr->cpsr)) != -1)
    curLogEntry->errorIDs.push_back(e);

  // FPR
  union {
    double s;
    uint32_t i[2];
  } sreg1, sreg2;
#define DIFF_D(name, dbg, instr)                               \
  sreg1.s = dbg;                                               \
  sreg2.s = instr;                                             \
  if ((e = diffSPR(name "[0]", sreg1.i[0], sreg2.i[0])) != -1) \
    curLogEntry->errorIDs.push_back(e);                        \
  if ((e = diffSPR(name "[1]", sreg1.i[1], sreg2.i[1])) != -1) \
    curLogEntry->errorIDs.push_back(e);

  DIFF_D("d0", fprStateDbg->vreg.d[0], fprStateInstr->vreg.d[0]);
  DIFF_D("d1", fprStateDbg->vreg.d[1], fprStateInstr->vreg.d[1]);
  DIFF_D("d2", fprStateDbg->vreg.d[2], fprStateInstr->vreg.d[2]);
  DIFF_D("d3", fprStateDbg->vreg.d[3], fprStateInstr->vreg.d[3]);
  DIFF_D("d4", fprStateDbg->vreg.d[4], fprStateInstr->vreg.d[4]);
  DIFF_D("d5", fprStateDbg->vreg.d[5], fprStateInstr->vreg.d[5]);
  DIFF_D("d6", fprStateDbg->vreg.d[6], fprStateInstr->vreg.d[6]);
  DIFF_D("d7", fprStateDbg->vreg.d[7], fprStateInstr->vreg.d[7]);
  DIFF_D("d8", fprStateDbg->vreg.d[8], fprStateInstr->vreg.d[8]);
  DIFF_D("d9", fprStateDbg->vreg.d[9], fprStateInstr->vreg.d[9]);
  DIFF_D("d10", fprStateDbg->vreg.d[10], fprStateInstr->vreg.d[10]);
  DIFF_D("d11", fprStateDbg->vreg.d[11], fprStateInstr->vreg.d[11]);
  DIFF_D("d12", fprStateDbg->vreg.d[12], fprStateInstr->vreg.d[12]);
  DIFF_D("d13", fprStateDbg->vreg.d[13], fprStateInstr->vreg.d[13]);
  DIFF_D("d14", fprStateDbg->vreg.d[14], fprStateInstr->vreg.d[14]);
  DIFF_D("d15", fprStateDbg->vreg.d[15], fprStateInstr->vreg.d[15]);
#if QBDI_NUM_FPR == 32
  DIFF_D("d16", fprStateDbg->vreg.d[16], fprStateInstr->vreg.d[16]);
  DIFF_D("d17", fprStateDbg->vreg.d[17], fprStateInstr->vreg.d[17]);
  DIFF_D("d18", fprStateDbg->vreg.d[18], fprStateInstr->vreg.d[18]);
  DIFF_D("d19", fprStateDbg->vreg.d[19], fprStateInstr->vreg.d[19]);
  DIFF_D("d20", fprStateDbg->vreg.d[20], fprStateInstr->vreg.d[20]);
  DIFF_D("d21", fprStateDbg->vreg.d[21], fprStateInstr->vreg.d[21]);
  DIFF_D("d22", fprStateDbg->vreg.d[22], fprStateInstr->vreg.d[22]);
  DIFF_D("d23", fprStateDbg->vreg.d[23], fprStateInstr->vreg.d[23]);
  DIFF_D("d24", fprStateDbg->vreg.d[24], fprStateInstr->vreg.d[24]);
  DIFF_D("d25", fprStateDbg->vreg.d[25], fprStateInstr->vreg.d[25]);
  DIFF_D("d26", fprStateDbg->vreg.d[26], fprStateInstr->vreg.d[26]);
  DIFF_D("d27", fprStateDbg->vreg.d[27], fprStateInstr->vreg.d[27]);
  DIFF_D("d28", fprStateDbg->vreg.d[28], fprStateInstr->vreg.d[28]);
  DIFF_D("d29", fprStateDbg->vreg.d[29], fprStateInstr->vreg.d[29]);
  DIFF_D("d30", fprStateDbg->vreg.d[30], fprStateInstr->vreg.d[30]);
  DIFF_D("d31", fprStateDbg->vreg.d[31], fprStateInstr->vreg.d[31]);
#endif

  if ((e = diffSPR("fpscr", fprStateDbg->fpscr, fprStateInstr->fpscr)) != -1)
    curLogEntry->errorIDs.push_back(e);
}
