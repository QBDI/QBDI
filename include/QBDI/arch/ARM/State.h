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
#ifndef QBDI_STATE_ARM_H_
#define QBDI_STATE_ARM_H_

#ifdef __cplusplus
#include <type_traits>
#endif
#include <inttypes.h>
#include <stdint.h>
#include "QBDI/Platform.h"

// ============================================================================
// ARM Context
// ============================================================================

#define PRIRWORD PRIx32

#define QBDI_NUM_FPR 32

#ifdef __cplusplus
namespace QBDI {
#endif // __cplusplus

/*! ARM CPU modes.
 */
typedef enum { ARM = 0, DEFAULT = 0, Thumb, COUNT } CPUMode;

typedef uint32_t rword;
typedef int32_t sword;

// SPHINX_ARM_FPRSTATE_BEGIN
/*! ARM Floating Point Register context.
 */
typedef union {
  float QBDI_ALIGNED(8) s[32];
  double QBDI_ALIGNED(8) d[QBDI_NUM_FPR];
  uint8_t QBDI_ALIGNED(8) q[QBDI_NUM_FPR / 2][16];
} FPRStateVReg;

typedef struct QBDI_ALIGNED(8) {
  FPRStateVReg vreg;

  rword fpscr;
} FPRState;
// SPHINX_ARM_FPRSTATE_END

// SPHINX_ARM_GPRSTATE_BEGIN
/*! ARM General Purpose Register context.
 */
typedef struct QBDI_ALIGNED(4) {
  rword r0;
  rword r1;
  rword r2;
  rword r3;
  rword r4;
  rword r5;
  rword r6;
  rword r7;
  rword r8;
  rword r9;
  rword r10;
  rword r11;
  rword r12;
  rword sp;
  rword lr;
  rword pc;
  rword cpsr;

  /* Internal CPU state
   * Local monitor state for exclusive load/store instruction
   */
  struct {
    rword addr;
    rword enable; /* 0=>disable,
                   * 1=>enable by ldrexb,
                   * 2=>enable by ldrexh,
                   * 4=>enable by ldrex,
                   * 8=>enable by ldrexd
                   */
  } localMonitor;

} GPRState;
// SPHINX_ARM_GPRSTATE_END

static const char *const GPR_NAMES[] = {
    "R0", "R1",  "R2",  "R3",  "R4", "R5", "R6", "R7",   "R8",
    "R9", "R10", "R11", "R12", "SP", "LR", "PC", "CPSR",
};

static const unsigned int NUM_GPR = 16;
static const unsigned int AVAILABLE_GPR = 13;
static const unsigned int REG_RETURN = 0;
static const unsigned int REG_BP = 12;
static const unsigned int REG_SP = 13;
static const unsigned int REG_LR = 14;
static const unsigned int REG_PC = 15;
static const unsigned int REG_FLAG = 16;

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

#endif // QBDI_STATE_ARM_H_
