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
#ifndef CONTEXT_H
#define CONTEXT_H

#include "State.h"

// ============================================================================
// Anonymous definition to catch unimplemented platform.
// ============================================================================

namespace QBDI {

struct HostState;

struct Context;

}

// ============================================================================
// X86_64 Context
// ============================================================================

#if defined(QBDI_ARCH_X86_64)

#include "X86InstrInfo.h"
#include "X86RegisterInfo.h"

namespace QBDI {

static const unsigned int GPR_ID[] = {
    llvm::X86::RAX, llvm::X86::RBX, llvm::X86::RCX, llvm::X86::RDX, 
    llvm::X86::RSI, llvm::X86::RDI, llvm::X86::R8,  llvm::X86::R9,
    llvm::X86::R10, llvm::X86::R11, llvm::X86::R12, llvm::X86::R13,
    llvm::X86::R14, llvm::X86::R15, llvm::X86::RBP, llvm::X86::RSP, 
    llvm::X86::RIP
};

/*! X86_64 Host context.
 */
struct HostState {
    rword rbp;
    rword rsp;
    rword selector;
    rword callback;
    rword data;
    rword origin;
};

/*! X86_64 Execution context.
 */
struct Context {

public:

    // fprState needs to be first for memory alignement reasons
    FPRState fprState;
    GPRState gprState;
    HostState hostState;

};

}

#endif // QBDI_ARCH_X86_64

// ============================================================================
// ARM Context
// ============================================================================

#if defined(QBDI_ARCH_ARM)

#include "ARMInstrInfo.h"
#include "ARMRegisterInfo.h"

namespace QBDI {

static const unsigned int GPR_ID[] = {
    llvm::ARM::R0, llvm::ARM::R1, llvm::ARM::R2,  llvm::ARM::R3, 
    llvm::ARM::R4, llvm::ARM::R5, llvm::ARM::R6,  llvm::ARM::R7, 
    llvm::ARM::R8, llvm::ARM::R9, llvm::ARM::R10, llvm::ARM::R12, 
    llvm::ARM::R11, llvm::ARM::SP, llvm::ARM::LR, llvm::ARM::PC
};

/*! ARM Host context.
*/
struct HostState {
    rword fp;
    rword sp;
    rword selector;
    rword callback;
    rword data;
    rword origin;
    rword exchange;
};

/*! ARM Execution context.
*/
struct Context {

public:

    // hostState needs to be first for relative addressing range reasons
    HostState hostState;
    FPRState fprState;
    GPRState gprState;
};

}

#endif // QBDI_ARCH_ARM


#endif //CONTEXT_H
