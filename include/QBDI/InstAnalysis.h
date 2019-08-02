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

/*! Operand type
 */
typedef enum {
    _QBDI_EI(OPERAND_INVALID) = 0,  /*!< Invalid operand */
    _QBDI_EI(OPERAND_IMM),          /*!< Immediate operand */
    _QBDI_EI(OPERAND_GPR),          /*!< Register operand */
    _QBDI_EI(OPERAND_PRED),         /*!< Predicate operand */
    _QBDI_EI(OPERAND_FPR),          /*!< Float register operand */
    _QBDI_EI(OPERAND_SEG),          /*!< Segment or unsaved register operand */
} OperandType;

typedef enum {
    _QBDI_EI(OPERANDFLAG_NONE) = 0,                 /*!< No flag */
    _QBDI_EI(OPERANDFLAG_ADDR) = 1<<0,              /*!< The operand is used to compute an address */
    _QBDI_EI(OPERANDFLAG_PCREL) = 1<<1,             /*!< The value of the operand is PC relative */
    _QBDI_EI(OPERANDFLAG_UNDEFINED_EFFECT) = 1<<2,  /*!< The operand role isn't fully defined */
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
    const char* mnemonic;           /*!< LLVM mnemonic (warning: NULL if !ANALYSIS_INSTRUCTION) */
    rword       address;            /*!< Instruction address */
    uint32_t    instSize;           /*!< Instruction size (in bytes) */
    bool        affectControlFlow;  /*!< true if instruction affects control flow */
    bool        isBranch;           /*!< true if instruction acts like a 'jump' */
    bool        isCall;             /*!< true if instruction acts like a 'call' */
    bool        isReturn;           /*!< true if instruction acts like a 'return' */
    bool        isCompare;          /*!< true if instruction is a comparison */
    bool        isPredicable;       /*!< true if instruction contains a predicate (~is conditional) */
    bool        mayLoad;            /*!< true if instruction 'may' load data from memory */
    bool        mayStore;           /*!< true if instruction 'may' store data to memory */
    // ANALYSIS_DISASSEMBLY
    char*       disassembly;        /*!< Instruction disassembly (warning: NULL if !ANALYSIS_DISASSEMBLY) */
    // ANALYSIS_OPERANDS
    RegisterAccessType flagsAccess; /*!< Flag access type (noaccess, r, w, rw)
                                         (warning: REGISTER_UNUSED if !ANALYSIS_OPERANDS) */
    uint8_t     numOperands;        /*!< Number of operands used by the instruction */
    OperandAnalysis* operands;      /*!< Structure containing analysis results of an operand provided by the VM.
                                         (warning: NULL if !ANALYSIS_OPERANDS) */
    // ANALYSIS_SYMBOL
    const char* symbol;             /*!< Instruction symbol (warning: NULL if !ANALYSIS_SYMBOL or not found) */
    uint32_t    symbolOffset;       /*!< Instruction symbol offset */
    const char* module;             /*!< Instruction module name (warning: NULL if !ANALYSIS_SYMBOL or not found) */
    // INTERNAL
    uint32_t    analysisType;       /*!< INTERNAL: Instruction analysis type (this should NOT be used) */
} InstAnalysis;

#ifdef __cplusplus
}
#endif

#endif // _INSTANALYSIS_H_
