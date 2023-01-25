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
#ifndef QBDI_STATE_AARCH64_H_
#define QBDI_STATE_AARCH64_H_

#ifdef __cplusplus
#include <type_traits>
#endif
#include <inttypes.h>
#include <stdint.h>
#include "QBDI/Platform.h"

// ============================================================================
// AARCH64 Context
// ============================================================================

#define PRIRWORD PRIx64

#define QBDI_NUM_FPR 32

#ifdef __cplusplus
namespace QBDI {
#endif // __cplusplus

/*! ARM CPU modes.
 */
typedef enum { AARCH64 = 0, DEFAULT = 0, COUNT } CPUMode;

typedef uint64_t rword;
typedef int64_t sword;

// SPHINX_AARCH64_FPRSTATE_BEGIN
/*! ARM Floating Point Register context.
 */
typedef struct QBDI_ALIGNED(8) {
  __uint128_t v0;
  __uint128_t v1;
  __uint128_t v2;
  __uint128_t v3;

  __uint128_t v4;
  __uint128_t v5;
  __uint128_t v6;
  __uint128_t v7;

  __uint128_t v8;
  __uint128_t v9;
  __uint128_t v10;
  __uint128_t v11;

  __uint128_t v12;
  __uint128_t v13;
  __uint128_t v14;
  __uint128_t v15;

  __uint128_t v16;
  __uint128_t v17;
  __uint128_t v18;
  __uint128_t v19;

  __uint128_t v20;
  __uint128_t v21;
  __uint128_t v22;
  __uint128_t v23;

  __uint128_t v24;
  __uint128_t v25;
  __uint128_t v26;
  __uint128_t v27;

  __uint128_t v28;
  __uint128_t v29;
  __uint128_t v30;
  __uint128_t v31;

  rword fpcr;
  rword fpsr;
} FPRState;
// SPHINX_AARCH64_FPRSTATE_END

// SPHINX_AARCH64_GPRSTATE_BEGIN
/*! ARM General Purpose Register context.
 */
typedef struct QBDI_ALIGNED(8) {
  rword x0;
  rword x1;
  rword x2;
  rword x3;
  rword x4;
  rword x5;
  rword x6;
  rword x7;
  rword x8;
  rword x9;
  rword x10;
  rword x11;
  rword x12;
  rword x13;
  rword x14;
  rword x15;
  rword x16;
  rword x17;
  rword x18;
  rword x19;
  rword x20;
  rword x21;
  rword x22;
  rword x23;
  rword x24;
  rword x25;
  rword x26;
  rword x27;
  rword x28;
  rword x29; // FP (x29)
  rword lr;  // LR (x30)

  rword sp;
  rword nzcv;
  rword pc;
  // ? rword daif; ?

  /* Internal CPU state
   * Local monitor state for exclusive load/store instruction
   */
  struct {
    rword addr;
    rword enable; /* 0=>disable, 1=>exclusive state, use a rword to not break
                     align */
  } localMonitor;

} GPRState;
// SPHINX_AARCH64_GPRSTATE_END

static const char *const GPR_NAMES[] = {
    "X0",  "X1",   "X2",  "X3",  "X4",  "X5",  "X6",  "X7",  "X8",  "X9",
    "X10", "X11",  "X12", "X13", "X14", "X15", "X16", "X17", "X18", "X19",
    "X20", "X21",  "X22", "X23", "X24", "X25", "X26", "X27", "X28",
    "X29", // FP
    "LR",

    "SP",  "NZCV", "PC",
};

static const unsigned int NUM_GPR = 32;
static const unsigned int AVAILABLE_GPR = 28;
static const unsigned int REG_RETURN = 0;
static const unsigned int REG_BP = 29;
static const unsigned int REG_LR = 30;
static const unsigned int REG_SP = 31;
static const unsigned int REG_PC = 33;
static const unsigned int REG_FLAG = 32;

#ifdef __cplusplus
#define QBDI_GPR_GET(state, i) (reinterpret_cast<const QBDI::rword *>(state)[i])
#define QBDI_GPR_SET(state, i, v) \
  (reinterpret_cast<QBDI::rword *>(state)[i] = v)
#else
#define QBDI_GPR_GET(state, i) (((rword *)state)[i])
#define QBDI_GPR_SET(state, i, v) (((rword *)state)[i] = v)
#endif

#ifdef __cplusplus
} // namespace QBDI
#endif // __cplusplus

#endif // QBDI_STATE_AARCH64_H_
