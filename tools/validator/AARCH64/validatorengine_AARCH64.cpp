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
#define DIFF_X(Xn)                                                       \
  if ((e = diffGPR(Xn, gprStateDbg->x##Xn, gprStateInstr->x##Xn)) != -1) \
    curLogEntry->errorIDs.push_back(e);

  DIFF_X(0);
  DIFF_X(1);
  DIFF_X(2);
  DIFF_X(3);
  DIFF_X(4);
  DIFF_X(5);
  DIFF_X(6);
  DIFF_X(7);
  DIFF_X(8);
  DIFF_X(9);
  DIFF_X(9);
  DIFF_X(10);
  DIFF_X(11);
  DIFF_X(12);
  DIFF_X(13);
  DIFF_X(14);
  DIFF_X(15);
  DIFF_X(16);
  DIFF_X(17);
  DIFF_X(18);
  DIFF_X(19);
  DIFF_X(19);
  DIFF_X(20);
  DIFF_X(21);
  DIFF_X(22);
  DIFF_X(23);
  DIFF_X(24);
  DIFF_X(25);
  DIFF_X(26);
  DIFF_X(27);
  DIFF_X(28);
  DIFF_X(29);
  DIFF_X(29);
  if ((e = diffGPR(30, gprStateDbg->lr, gprStateInstr->lr)) != -1)
    curLogEntry->errorIDs.push_back(e);
  if ((e = diffGPR(31, gprStateDbg->sp, gprStateInstr->sp)) != -1)
    curLogEntry->errorIDs.push_back(e);

  // get only revelant bits
  if ((e = diffGPR(32, gprStateDbg->nzcv & 0xf0000000,
                   gprStateInstr->nzcv & 0xf0000000)) != -1)
    curLogEntry->errorIDs.push_back(e);

  const __uint128_t first64 = ((static_cast<__uint128_t>(1) << 64) - 1);
// FPR
#define DIFF_V(v)                                         \
  if ((e = diffSPR(#v "[0:64]", fprStateDbg->v & first64, \
                   fprStateInstr->v & first64)) != -1)    \
    curLogEntry->errorIDs.push_back(e);                   \
  if ((e = diffSPR(#v "[64:128]", fprStateDbg->v >> 64,   \
                   fprStateInstr->v >> 64)) != -1)        \
    curLogEntry->errorIDs.push_back(e);

  DIFF_V(v0);
  DIFF_V(v1);
  DIFF_V(v2);
  DIFF_V(v3);
  DIFF_V(v4);
  DIFF_V(v5);
  DIFF_V(v6);
  DIFF_V(v7);
  DIFF_V(v8);
  DIFF_V(v9);
  DIFF_V(v10);
  DIFF_V(v11);
  DIFF_V(v12);
  DIFF_V(v13);
  DIFF_V(v14);
  DIFF_V(v15);
  DIFF_V(v16);
  DIFF_V(v17);
  DIFF_V(v18);
  DIFF_V(v19);
  DIFF_V(v20);
  DIFF_V(v21);
  DIFF_V(v22);
  DIFF_V(v23);
  DIFF_V(v24);
  DIFF_V(v25);
  DIFF_V(v26);
  DIFF_V(v27);
  DIFF_V(v28);
  DIFF_V(v29);
  DIFF_V(v30);
  DIFF_V(v31);

  // nzcv, fpcr and fpsr can be a single register. used mask to distinct the
  // effect

  if ((e = diff("fpcr", fprStateDbg->fpcr, fprStateInstr->fpcr & 0x07f79f00)) !=
      -1)
    curLogEntry->errorIDs.push_back(e);

  // fpsr can have the NZCV bit
  if (((fprStateDbg->fpsr & 0xf0000000) == 0) &&
      ((fprStateInstr->fpsr & 0xf0000000) ==
       (gprStateInstr->nzcv & 0xf0000000))) {
    e = diff("fpsr", fprStateDbg->fpsr, fprStateInstr->fpsr & 0x0800009f);
  } else {
    e = diff("fpsr", fprStateDbg->fpsr, fprStateInstr->fpsr & 0xf800009f);
  }
  if (e != -1)
    curLogEntry->errorIDs.push_back(e);
}
