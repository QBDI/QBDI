/*
 * This file is part of QBDI.
 *
 * Copyright 2017 Quarkslab
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
#ifndef _STATE_H_
#define _STATE_H_

#include <stdint.h>
#include <inttypes.h>

#include "Platform.h"

#ifdef __cplusplus
namespace QBDI {
#endif // __cplusplus

// ============================================================================
// X86 Common
// ============================================================================
#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
typedef struct {
    uint16_t invalid :1,
             denorm  :1,
             zdiv    :1,
             ovrfl   :1,
             undfl   :1,
             precis  :1,
                     :2,
             pc      :2,
             rc      :2,
             /*inf*/ :1,
                     :3;
} FPControl;

typedef struct {
    uint16_t invalid :1,
             denorm  :1,
             zdiv    :1,
             ovrfl   :1,
             undfl   :1,
             precis  :1,
             stkflt  :1,
             errsumm :1,
             c0      :1,
             c1      :1,
             c2      :1,
             tos     :3,
             c3      :1,
             busy    :1;
} FPStatus;

typedef struct {
    char    reg[10];
    char    rsrv[6];
} MMSTReg;
#endif

// ============================================================================
// X86 Context
// ============================================================================

#if defined(QBDI_ARCH_X86)

#define PRIRWORD PRIx32

typedef uint32_t rword;

// SPHINX_X86_FPRSTATE_BEGIN
/*! X86 Floating Point Register context.
 */
typedef struct {
    union {
        FPControl     fcw;      /* x87 FPU control word */
        uint16_t      rfcw;
    };
    union {
        FPStatus      fsw;      /* x87 FPU status word */
        uint16_t      rfsw;
    };
    uint8_t           ftw;          /* x87 FPU tag word */
    uint8_t           rsrv1;        /* reserved */
    uint16_t          fop;          /* x87 FPU Opcode */
    uint32_t          ip;           /* x87 FPU Instruction Pointer offset */
    uint16_t          cs;           /* x87 FPU Instruction Pointer Selector */
    uint16_t          rsrv2;        /* reserved */
    uint32_t          dp;           /* x87 FPU Instruction Operand(Data) Pointer offset */
    uint16_t          ds;           /* x87 FPU Instruction Operand(Data) Pointer Selector */
    uint16_t          rsrv3;        /* reserved */
    uint32_t          mxcsr;        /* MXCSR Register state */
    uint32_t          mxcsrmask;    /* MXCSR mask */
    MMSTReg           stmm0;        /* ST0/MM0   */
    MMSTReg           stmm1;        /* ST1/MM1  */
    MMSTReg           stmm2;        /* ST2/MM2  */
    MMSTReg           stmm3;        /* ST3/MM3  */
    MMSTReg           stmm4;        /* ST4/MM4  */
    MMSTReg           stmm5;        /* ST5/MM5  */
    MMSTReg           stmm6;        /* ST6/MM6  */
    MMSTReg           stmm7;        /* ST7/MM7  */
    char              xmm0[16];     /* XMM 0  */
    char              xmm1[16];     /* XMM 1  */
    char              xmm2[16];     /* XMM 2  */
    char              xmm3[16];     /* XMM 3  */
    char              xmm4[16];     /* XMM 4  */
    char              xmm5[16];     /* XMM 5  */
    char              xmm6[16];     /* XMM 6  */
    char              xmm7[16];     /* XMM 7  */
    char              reserved[14*16];
    char              ymm0[16];     /* YMM0[255:128] */
    char              ymm1[16];     /* YMM1[255:128] */
    char              ymm2[16];     /* YMM2[255:128] */
    char              ymm3[16];     /* YMM3[255:128] */
    char              ymm4[16];     /* YMM4[255:128] */
    char              ymm5[16];     /* YMM5[255:128] */
    char              ymm6[16];     /* YMM6[255:128] */
    char              ymm7[16];     /* YMM7[255:128] */
} FPRState;
// SPHINX_X86_FPRSTATE_END
typedef char __compile_check_01__[sizeof(FPRState) == 640 ? 1 : -1];

// SPHINX_X86_GPRSTATE_BEGIN
/*! X86 General Purpose Register context.
 */
typedef struct {
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

static const char* const GPR_NAMES[] = {
    "EAX",
    "EBX",
    "ECX",
    "EDX",
    "ESI",
    "EDI",
    "EBP",
    "ESP",
    "EIP",
    "EFLAGS"
};

static const unsigned int NUM_GPR = 9;
static const unsigned int AVAILABLE_GPR = 6;
static const unsigned int REG_RETURN = 0;
static const unsigned int REG_BP = 6;
static const unsigned int REG_SP = 7;
static const unsigned int REG_PC = 8;
static const unsigned int REG_FLAG = 9;

#endif // QBDI_ARCH_X86


// ============================================================================
// X86_64 Context
// ============================================================================

#if defined(QBDI_ARCH_X86_64)

#define PRIRWORD PRIx64

typedef uint64_t rword;

// SPHINX_X86_64_FPRSTATE_BEGIN
/*! X86_64 Floating Point Register context.
 */
typedef struct {
    union {
        FPControl     fcw;      /* x87 FPU control word */
        uint16_t      rfcw;
    };
    union {
        FPStatus      fsw;      /* x87 FPU status word */
        uint16_t      rfsw;
    };
    uint8_t           ftw;          /* x87 FPU tag word */
    uint8_t           rsrv1;        /* reserved */
    uint16_t          fop;          /* x87 FPU Opcode */
    uint32_t          ip;           /* x87 FPU Instruction Pointer offset */
    uint16_t          cs;           /* x87 FPU Instruction Pointer Selector */
    uint16_t          rsrv2;        /* reserved */
    uint32_t          dp;           /* x87 FPU Instruction Operand(Data) Pointer offset */
    uint16_t          ds;           /* x87 FPU Instruction Operand(Data) Pointer Selector */
    uint16_t          rsrv3;        /* reserved */
    uint32_t          mxcsr;        /* MXCSR Register state */
    uint32_t          mxcsrmask;    /* MXCSR mask */
    MMSTReg           stmm0;        /* ST0/MM0   */
    MMSTReg           stmm1;        /* ST1/MM1  */
    MMSTReg           stmm2;        /* ST2/MM2  */
    MMSTReg           stmm3;        /* ST3/MM3  */
    MMSTReg           stmm4;        /* ST4/MM4  */
    MMSTReg           stmm5;        /* ST5/MM5  */
    MMSTReg           stmm6;        /* ST6/MM6  */
    MMSTReg           stmm7;        /* ST7/MM7  */
    char              xmm0[16];     /* XMM 0  */
    char              xmm1[16];     /* XMM 1  */
    char              xmm2[16];     /* XMM 2  */
    char              xmm3[16];     /* XMM 3  */
    char              xmm4[16];     /* XMM 4  */
    char              xmm5[16];     /* XMM 5  */
    char              xmm6[16];     /* XMM 6  */
    char              xmm7[16];     /* XMM 7  */
    char              xmm8[16];     /* XMM 8  */
    char              xmm9[16];     /* XMM 9  */
    char              xmm10[16];    /* XMM 10  */
    char              xmm11[16];    /* XMM 11  */
    char              xmm12[16];    /* XMM 12  */
    char              xmm13[16];    /* XMM 13  */
    char              xmm14[16];    /* XMM 14  */
    char              xmm15[16];    /* XMM 15  */
    char              reserved[6*16];
    char              ymm0[16];     /* YMM0[255:128] */
    char              ymm1[16];     /* YMM1[255:128] */
    char              ymm2[16];     /* YMM2[255:128] */
    char              ymm3[16];     /* YMM3[255:128] */
    char              ymm4[16];     /* YMM4[255:128] */
    char              ymm5[16];     /* YMM5[255:128] */
    char              ymm6[16];     /* YMM6[255:128] */
    char              ymm7[16];     /* YMM7[255:128] */
    char              ymm8[16];     /* YMM8[255:128] */
    char              ymm9[16];     /* YMM9[255:128] */
    char              ymm10[16];    /* YMM10[255:128] */
    char              ymm11[16];    /* YMM11[255:128] */
    char              ymm12[16];    /* YMM12[255:128] */
    char              ymm13[16];    /* YMM13[255:128] */
    char              ymm14[16];    /* YMM14[255:128] */
    char              ymm15[16];    /* YMM15[255:128] */
} FPRState;
// SPHINX_X86_64_FPRSTATE_END
typedef char __compile_check_01__[sizeof(FPRState) == 768 ? 1 : -1];

// SPHINX_X86_64_GPRSTATE_BEGIN
/*! X86_64 General Purpose Register context.
 */
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

static const char* const GPR_NAMES[] = {
    "RAX",
    "RBX",
    "RCX",
    "RDX",
    "RSI",
    "RDI",
    "R8",
    "R9",
    "R10",
    "R11",
    "R12",
    "R13",
    "R14",
    "R15",
    "RBP",
    "RSP",
    "RIP",
    "EFLAGS"
};

static const unsigned int NUM_GPR = 17;
static const unsigned int AVAILABLE_GPR = 14;
static const unsigned int REG_RETURN = 0;
static const unsigned int REG_BP = 14;
static const unsigned int REG_SP = 15;
static const unsigned int REG_PC = 16;
static const unsigned int REG_FLAG = 17;

#endif // QBDI_ARCH_X86_64

// ============================================================================
// ARM Context
// ============================================================================

#if defined(QBDI_ARCH_ARM)

#define PRIRWORD "x"

#define QBDI_NUM_FPR 32

typedef uint32_t rword;

// SPHINX_ARM_FPRSTATE_BEGIN
/*! ARM Floating Point Register context.
 */
typedef struct {
    float s[QBDI_NUM_FPR];
} FPRState;
// SPHINX_ARM_FPRSTATE_END

// SPHINX_ARM_GPRSTATE_BEGIN
/*! ARM General Purpose Register context.
 */
typedef struct {
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
    rword r12;
    rword fp;
    rword sp;
    rword lr;
    rword pc;
    rword cpsr;

} GPRState;
// SPHINX_ARM_GPRSTATE_END

static const char* const GPR_NAMES[] = {
    "R0",
    "R1",
    "R2",
    "R3",
    "R4",
    "R5",
    "R6",
    "R7",
    "R8",
    "R9",
    "R10",
    "R12",
    "FP",
    "SP",
    "LR",
    "PC",
    "CPSR",
};

static const unsigned int NUM_GPR = 16;
static const unsigned int AVAILABLE_GPR = 12;
static const unsigned int REG_RETURN = 0;
static const unsigned int REG_BP = 12;
static const unsigned int REG_SP = 13;
static const unsigned int REG_LR = 14;
static const unsigned int REG_PC = 15;
static const unsigned int REG_FLAG = 16;

#endif // QBDI_ARCH_ARM

#ifdef __cplusplus
}
#endif // __cplusplus


#ifdef __cplusplus

#define QBDI_GPR_GET(state, i) (reinterpret_cast<const QBDI::rword*>(state)[i])

#define QBDI_GPR_SET(state, i, v) (reinterpret_cast<QBDI::rword*>(state)[i] = v)

#else

#define QBDI_GPR_GET(state, i) (((rword*)state)[i])

#define QBDI_GPR_SET(state, i, v) (((rword*)state)[i] = v)

#endif

#endif // _STATE_H_
