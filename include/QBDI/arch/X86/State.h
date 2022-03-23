/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2022 Quarkslab
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
#ifndef QBDI_STATE_X86_H_
#define QBDI_STATE_X86_H_

#include <inttypes.h>
#include <stdint.h>

#include "QBDI/Platform.h"

// ============================================================================
// X86 Context
// ============================================================================

#define PRIRWORD PRIx32

#ifdef __cplusplus
namespace QBDI {
#endif // __cplusplus

/*! X86 CPU modes.
 */
typedef enum { X86 = 0, DEFAULT = 0, COUNT } CPUMode;

typedef struct {
  uint16_t invalid : 1;
  uint16_t denorm  : 1;
  uint16_t zdiv    : 1;
  uint16_t ovrfl   : 1;
  uint16_t undfl   : 1;
  uint16_t precis  : 1;
  uint16_t         : 2;
  uint16_t pc      : 2;
  uint16_t rc      : 2;
  uint16_t /*inf*/ : 1;
  uint16_t         : 3;
} FPControl;

typedef struct {
  uint16_t invalid : 1;
  uint16_t denorm  : 1;
  uint16_t zdiv    : 1;
  uint16_t ovrfl   : 1;
  uint16_t undfl   : 1;
  uint16_t precis  : 1;
  uint16_t stkflt  : 1;
  uint16_t errsumm : 1;
  uint16_t c0      : 1;
  uint16_t c1      : 1;
  uint16_t c2      : 1;
  uint16_t tos     : 3;
  uint16_t c3      : 1;
  uint16_t busy    : 1;
} FPStatus;

typedef struct {
  char reg[10];
  char rsrv[6];
} MMSTReg;

typedef uint32_t rword;

/*! X86 Floating Point Register context.
 */ // SPHINX_X86_FPRSTATE_BEGIN
typedef struct QBDI_ALIGNED(16) {
  union {
    FPControl fcw; /* x87 FPU control word */
    uint16_t rfcw;
  };
  union {
    FPStatus fsw; /* x87 FPU status word */
    uint16_t rfsw;
  };
  uint8_t ftw;        /* x87 FPU tag word */
  uint8_t rsrv1;      /* reserved */
  uint16_t fop;       /* x87 FPU Opcode */
  uint32_t ip;        /* x87 FPU Instruction Pointer offset */
  uint16_t cs;        /* x87 FPU Instruction Pointer Selector */
  uint16_t rsrv2;     /* reserved */
  uint32_t dp;        /* x87 FPU Instruction Operand(Data) Pointer offset */
  uint16_t ds;        /* x87 FPU Instruction Operand(Data) Pointer Selector */
  uint16_t rsrv3;     /* reserved */
  uint32_t mxcsr;     /* MXCSR Register state */
  uint32_t mxcsrmask; /* MXCSR mask */
  MMSTReg stmm0;      /* ST0/MM0   */
  MMSTReg stmm1;      /* ST1/MM1  */
  MMSTReg stmm2;      /* ST2/MM2  */
  MMSTReg stmm3;      /* ST3/MM3  */
  MMSTReg stmm4;      /* ST4/MM4  */
  MMSTReg stmm5;      /* ST5/MM5  */
  MMSTReg stmm6;      /* ST6/MM6  */
  MMSTReg stmm7;      /* ST7/MM7  */
  char xmm0[16];      /* XMM 0  */
  char xmm1[16];      /* XMM 1  */
  char xmm2[16];      /* XMM 2  */
  char xmm3[16];      /* XMM 3  */
  char xmm4[16];      /* XMM 4  */
  char xmm5[16];      /* XMM 5  */
  char xmm6[16];      /* XMM 6  */
  char xmm7[16];      /* XMM 7  */
  char reserved[14 * 16];
  char ymm0[16]; /* YMM0[255:128] */
  char ymm1[16]; /* YMM1[255:128] */
  char ymm2[16]; /* YMM2[255:128] */
  char ymm3[16]; /* YMM3[255:128] */
  char ymm4[16]; /* YMM4[255:128] */
  char ymm5[16]; /* YMM5[255:128] */
  char ymm6[16]; /* YMM6[255:128] */
  char ymm7[16]; /* YMM7[255:128] */
} FPRState;
// SPHINX_X86_FPRSTATE_END
typedef char __compile_check_01__[sizeof(FPRState) == 640 ? 1 : -1];

/*! X86 General Purpose Register context.
 */ // SPHINX_X86_GPRSTATE_BEGIN
typedef struct QBDI_ALIGNED(4) {
  rword eax;
  rword ebx;
  rword ecx;
  rword edx;
  rword esi;
  rword edi;
  rword ebp;
  rword esp;
  rword eip;
  rword eflags;
} GPRState;
// SPHINX_X86_GPRSTATE_END

static const char *const GPR_NAMES[] = {"EAX", "EBX", "ECX", "EDX", "ESI",
                                        "EDI", "EBP", "ESP", "EIP", "EFLAGS"};

static const unsigned int NUM_GPR = 9;
static const unsigned int AVAILABLE_GPR = 6;
static const unsigned int REG_RETURN = 0;
static const unsigned int REG_BP = 6;
static const unsigned int REG_SP = 7;
static const unsigned int REG_PC = 8;
static const unsigned int REG_FLAG = 9;

#ifdef __cplusplus
#define QBDI_GPR_GET(state, i) (reinterpret_cast<const QBDI::rword *>(state)[i])
#define QBDI_GPR_SET(state, i, v) \
  (reinterpret_cast<QBDI::rword *>(state)[i] = v)
#else
#define QBDI_GPR_GET(state, i) (((rword *)state)[i])
#define QBDI_GPR_SET(state, i, v) (((rword *)state)[i] = v)
#endif

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QBDI_STATE_X86_H_
