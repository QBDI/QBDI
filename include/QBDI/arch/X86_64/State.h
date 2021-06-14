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
#ifndef QBDI_STATE_X86_64_H_
#define QBDI_STATE_X86_64_H_

#include <inttypes.h>
#include <stdint.h>

#include "QBDI/Platform.h"

#ifdef __cplusplus
namespace QBDI {
#endif // __cplusplus

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

#define PRIRWORD PRIx64

typedef uint64_t rword;

/*! X86_64 Floating Point Register context.
 */ // SPHINX_X86_64_FPRSTATE_BEGIN
typedef struct {
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
  char xmm8[16];      /* XMM 8  */
  char xmm9[16];      /* XMM 9  */
  char xmm10[16];     /* XMM 10  */
  char xmm11[16];     /* XMM 11  */
  char xmm12[16];     /* XMM 12  */
  char xmm13[16];     /* XMM 13  */
  char xmm14[16];     /* XMM 14  */
  char xmm15[16];     /* XMM 15  */
  char reserved[6 * 16];
  char ymm0[16];  /* YMM0[255:128] */
  char ymm1[16];  /* YMM1[255:128] */
  char ymm2[16];  /* YMM2[255:128] */
  char ymm3[16];  /* YMM3[255:128] */
  char ymm4[16];  /* YMM4[255:128] */
  char ymm5[16];  /* YMM5[255:128] */
  char ymm6[16];  /* YMM6[255:128] */
  char ymm7[16];  /* YMM7[255:128] */
  char ymm8[16];  /* YMM8[255:128] */
  char ymm9[16];  /* YMM9[255:128] */
  char ymm10[16]; /* YMM10[255:128] */
  char ymm11[16]; /* YMM11[255:128] */
  char ymm12[16]; /* YMM12[255:128] */
  char ymm13[16]; /* YMM13[255:128] */
  char ymm14[16]; /* YMM14[255:128] */
  char ymm15[16]; /* YMM15[255:128] */
} FPRState;
// SPHINX_X86_64_FPRSTATE_END
typedef char __compile_check_01__[sizeof(FPRState) == 768 ? 1 : -1];

/*! X86_64 General Purpose Register context.
 */ // SPHINX_X86_64_GPRSTATE_BEGIN
typedef struct {
  rword rax;
  rword rbx;
  rword rcx;
  rword rdx;
  rword rsi;
  rword rdi;
  rword r8;
  rword r9;
  rword r10;
  rword r11;
  rword r12;
  rword r13;
  rword r14;
  rword r15;
  rword rbp;
  rword rsp;
  rword rip;
  rword eflags;
} GPRState;
// SPHINX_X86_64_GPRSTATE_END

static const char *const GPR_NAMES[] = {
    "RAX", "RBX", "RCX", "RDX", "RSI", "RDI", "R8",  "R9",  "R10",
    "R11", "R12", "R13", "R14", "R15", "RBP", "RSP", "RIP", "EFLAGS"};

static const unsigned int NUM_GPR = 17;
static const unsigned int AVAILABLE_GPR = 14;
static const unsigned int REG_RETURN = 0;
static const unsigned int REG_BP = 14;
static const unsigned int REG_SP = 15;
static const unsigned int REG_PC = 16;
static const unsigned int REG_FLAG = 17;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // QBDI_STATE_X86_64_H_
