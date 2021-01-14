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
#ifndef _INSTANALYSIS_H_
#define _INSTANALYSIS_H_

#include <stdbool.h>

#include "Platform.h"
#include "Bitmask.h"
#include "State.h"

#ifdef __cplusplus
namespace QBDI {
#endif

/*! Access type (R/W/RW) of a register operand
 */
typedef enum {
    _QBDI_EI(REGISTER_UNUSED)     = 0,    /*!< Unused register */
    _QBDI_EI(REGISTER_READ)       = 1,    /*!< Register read access */
    _QBDI_EI(REGISTER_WRITE)      = 1<<1, /*!< Register write access */
    _QBDI_EI(REGISTER_READ_WRITE) = 3     /*!< Register read/write access */
} RegisterAccessType;

_QBDI_ENABLE_BITMASK_OPERATORS(RegisterAccessType)

/*! Instruction Condition
 */
typedef enum {
    _QBDI_EI(CONDITION_NONE)           = 0x0,    /*!< The instruction is unconditionnal */
    // ConditionType ^ 0x1 reverse the condition, except for CONDITION_NONE
    _QBDI_EI(CONDITION_ALWAYS)         = 0x2,    /*!< The instruction is always true */
    _QBDI_EI(CONDITION_NEVER)          = 0x3,    /*!< The instruction is always false */
    _QBDI_EI(CONDITION_EQUALS)         = 0x4,    /*!< Equals ( '==' ) */
    _QBDI_EI(CONDITION_NOT_EQUALS)     = 0x5,    /*!< Not Equals ( '!=' ) */
    _QBDI_EI(CONDITION_ABOVE)          = 0x6,    /*!< Above ( '>' unsigned ) */
    _QBDI_EI(CONDITION_BELOW_EQUALS)   = 0x7,    /*!< Below or Equals ( '<=' unsigned ) */
    _QBDI_EI(CONDITION_ABOVE_EQUALS)   = 0x8,    /*!< Above or Equals ( '>=' unsigned ) */
    _QBDI_EI(CONDITION_BELOW)          = 0x9,    /*!< Below ( '<' unsigned ) */
    _QBDI_EI(CONDITION_GREAT)          = 0xa,    /*!< Great ( '>' signed ) */
    _QBDI_EI(CONDITION_LESS_EQUALS)    = 0xb,    /*!< Less or Equals ( '<=' signed ) */
    _QBDI_EI(CONDITION_GREAT_EQUALS)   = 0xc,    /*!< Great or Equals ( '>=' signed ) */
    _QBDI_EI(CONDITION_LESS)           = 0xd,    /*!< Less ( '<' signed ) */
    _QBDI_EI(CONDITION_EVEN)           = 0xe,    /*!< Even */
    _QBDI_EI(CONDITION_ODD)            = 0xf,    /*!< Odd */
    _QBDI_EI(CONDITION_OVERFLOW)       = 0x10,   /*!< Overflow */
    _QBDI_EI(CONDITION_NOT_OVERFLOW)   = 0x11,   /*!< Not Overflow */
    _QBDI_EI(CONDITION_SIGN)           = 0x12,   /*!< Sign */
    _QBDI_EI(CONDITION_NOT_SIGN)       = 0x13,   /*!< Not Sign */
} ConditionType;

/*! Operand type
 */
typedef enum {
    _QBDI_EI(OPERAND_INVALID) = 0,  /*!< Invalid operand */
    _QBDI_EI(OPERAND_IMM),          /*!< Immediate operand */
    _QBDI_EI(OPERAND_GPR),          /*!< Register operand */
    _QBDI_EI(OPERAND_PRED),         /*!< Predicate operand */
    _QBDI_EI(OPERAND_FPR),          /*!< Float register operand */
    _QBDI_EI(OPERAND_SEG),          /*!< Segment or unsupported register operand */
} OperandType;

typedef enum {
    _QBDI_EI(OPERANDFLAG_NONE) = 0,                 /*!< No flag */
    _QBDI_EI(OPERANDFLAG_ADDR) = 1<<0,              /*!< The operand is used to compute an address */
    _QBDI_EI(OPERANDFLAG_PCREL) = 1<<1,             /*!< The value of the operand is PC relative */
    _QBDI_EI(OPERANDFLAG_UNDEFINED_EFFECT) = 1<<2,  /*!< The operand role isn't fully defined */
    _QBDI_EI(OPERANDFLAG_IMPLICIT) = 1<<3,          /*!< The operand is implicit */
} OperandFlag;

_QBDI_ENABLE_BITMASK_OPERATORS(OperandFlag)

/*! Structure containing analysis results of an operand provided by the VM.
 */
typedef struct {
    // Common fields
    OperandType        type;       /*!< Operand type */
    OperandFlag        flag;       /*!< Operand flag */
    rword              value;      /*!< Operand value (if immediate), or register Id */
    uint8_t            size;       /*!< Operand size (in bytes) */
    // Register specific fields
    uint8_t            regOff;     /*!< Sub-register offset in register (in bits) */
    int16_t            regCtxIdx;  /*!< Register index in VM state (< 0 if not know) */
    const char*        regName;    /*!< Register name */
    RegisterAccessType regAccess;  /*!< Register access type (r, w, rw) */
} OperandAnalysis;

/*! Instruction analysis type
 */
typedef enum {
    _QBDI_EI(ANALYSIS_INSTRUCTION) = 1,     /*!< Instruction analysis (address, mnemonic, ...) */
    _QBDI_EI(ANALYSIS_DISASSEMBLY) = 1<<1,  /*!< Instruction disassembly */
    _QBDI_EI(ANALYSIS_OPERANDS)    = 1<<2,  /*!< Instruction operands analysis */
    _QBDI_EI(ANALYSIS_SYMBOL)      = 1<<3,  /*!< Instruction symbol */
} AnalysisType;

_QBDI_ENABLE_BITMASK_OPERATORS(AnalysisType)

/*! Structure containing analysis results of an instruction provided by the VM.
 */
typedef struct {
    // ANALYSIS_INSTRUCTION
    const char*    mnemonic;           /*!< LLVM mnemonic (warning: NULL if !ANALYSIS_INSTRUCTION) */
    rword          address;            /*!< Instruction address */
    uint32_t       instSize;           /*!< Instruction size (in bytes) */
    bool           affectControlFlow;  /*!< true if instruction affects control flow */
    bool           isBranch;           /*!< true if instruction acts like a 'jump' */
    bool           isCall;             /*!< true if instruction acts like a 'call' */
    bool           isReturn;           /*!< true if instruction acts like a 'return' */
    bool           isCompare;          /*!< true if instruction is a comparison */
    bool           isPredicable;       /*!< true if instruction contains a predicate (~is conditional) */
    bool           mayLoad;            /*!< true if QBDI detects a load for this instruction */
    bool           mayStore;           /*!< true if QBDI detects a store for this instruction */
    uint32_t       loadSize;           /*!< size of the expected read access, may be 0 with mayLoad if the size isn't determined */
    uint32_t       storeSize;          /*!< size of the expected write access, may be 0 with mayStore if the size isn't determined */
    ConditionType  condition;          /*!< Condition associated with the instruction */
    bool           mayLoad_LLVM;       // mayLoad of 0.7.1
    bool           mayStore_LLVM;      // mayStore of 0.7.1
    // ANALYSIS_DISASSEMBLY
    char*          disassembly;        /*!< Instruction disassembly (warning: NULL if !ANALYSIS_DISASSEMBLY) */
    // ANALYSIS_OPERANDS
    RegisterAccessType flagsAccess;    /*!< Flag access type (noaccess, r, w, rw)
                                         (warning: REGISTER_UNUSED if !ANALYSIS_OPERANDS) */
    uint8_t        numOperands;        /*!< Number of operands used by the instruction */
    OperandAnalysis* operands;         /*!< Structure containing analysis results of an operand provided by the VM.
                                            (warning: NULL if !ANALYSIS_OPERANDS) */
    // ANALYSIS_SYMBOL
    const char*    symbol;             /*!< Instruction symbol (warning: NULL if !ANALYSIS_SYMBOL or not found) */
    uint32_t       symbolOffset;       /*!< Instruction symbol offset */
    const char*    module;             /*!< Instruction module name (warning: NULL if !ANALYSIS_SYMBOL or not found) */
    // INTERNAL
    uint32_t       analysisType;       /*!< INTERNAL: Instruction analysis type (this should NOT be used) */
} InstAnalysis;

#ifdef __cplusplus
}
#endif

#endif // _INSTANALYSIS_H_
