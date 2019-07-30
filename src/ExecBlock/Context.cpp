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

#include "Platform.h"

#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
#include "X86InstrInfo.h"
#include "X86RegisterInfo.h"
#elif defined(QBDI_ARCH_ARM)
#include "ARMInstrInfo.h"
#include "ARMRegisterInfo.h"
#endif

#include "ExecBlock/Context.h"

namespace QBDI {

#if defined(QBDI_ARCH_X86_64)
const unsigned int GPR_ID[] = {
    llvm::X86::RAX, llvm::X86::RBX, llvm::X86::RCX, llvm::X86::RDX,
    llvm::X86::RSI, llvm::X86::RDI, llvm::X86::R8,  llvm::X86::R9,
    llvm::X86::R10, llvm::X86::R11, llvm::X86::R12, llvm::X86::R13,
    llvm::X86::R14, llvm::X86::R15, llvm::X86::RBP, llvm::X86::RSP,
    llvm::X86::RIP, llvm::X86::EFLAGS
};

const unsigned int FLAG_ID[] = {
    llvm::X86::DF,
};

#elif defined(QBDI_ARCH_X86)
const unsigned int GPR_ID[] = {
    llvm::X86::EAX, llvm::X86::EBX, llvm::X86::ECX, llvm::X86::EDX,
    llvm::X86::ESI, llvm::X86::EDI, llvm::X86::EBP, llvm::X86::ESP,
    llvm::X86::EIP, llvm::X86::EFLAGS
};

const unsigned int FLAG_ID[] = {
    llvm::X86::DF,
};

#elif defined(QBDI_ARCH_ARM)
const unsigned int GPR_ID[] = {
    llvm::ARM::R0, llvm::ARM::R1, llvm::ARM::R2,  llvm::ARM::R3,
    llvm::ARM::R4, llvm::ARM::R5, llvm::ARM::R6,  llvm::ARM::R7,
    llvm::ARM::R8, llvm::ARM::R9, llvm::ARM::R10, llvm::ARM::R12,
    llvm::ARM::R11, llvm::ARM::SP, llvm::ARM::LR, llvm::ARM::PC,
    llvm::ARM::CPSR
};

const unsigned int FLAG_ID[] = {};
#endif

const unsigned int size_GPR_ID = sizeof(GPR_ID) / sizeof(unsigned int);
const unsigned int size_FLAG_ID = sizeof(FLAG_ID) / sizeof(unsigned int);

}
