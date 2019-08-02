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

#include "Context.h"

namespace QBDI {

// ============================================================================
// X86_64 Context
// ============================================================================

#if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)


#if defined(QBDI_ARCH_X86_64)
const unsigned int GPR_ID[] = {
    llvm::X86::RAX, llvm::X86::RBX, llvm::X86::RCX, llvm::X86::RDX,
    llvm::X86::RSI, llvm::X86::RDI, llvm::X86::R8,  llvm::X86::R9,
    llvm::X86::R10, llvm::X86::R11, llvm::X86::R12, llvm::X86::R13,
    llvm::X86::R14, llvm::X86::R15, llvm::X86::RBP, llvm::X86::RSP,
    llvm::X86::RIP, llvm::X86::EFLAGS
};
const unsigned int size_GPR_ID = sizeof(GPR_ID)/sizeof(unsigned int);

const std::map<unsigned int, int16_t> FPR_ID = {
    {llvm::X86::FPCW, offsetof(FPRState, rfcw)},
    {llvm::X86::FPSW, offsetof(FPRState, rfsw)},
    {llvm::X86::ST0, offsetof(FPRState, stmm0)}, {llvm::X86::ST1, offsetof(FPRState, stmm1)},
    {llvm::X86::ST2, offsetof(FPRState, stmm2)}, {llvm::X86::ST3, offsetof(FPRState, stmm3)},
    {llvm::X86::ST4, offsetof(FPRState, stmm4)}, {llvm::X86::ST5, offsetof(FPRState, stmm5)},
    {llvm::X86::ST6, offsetof(FPRState, stmm6)}, {llvm::X86::ST7, offsetof(FPRState, stmm7)},
    {llvm::X86::MM0, offsetof(FPRState, stmm0)}, {llvm::X86::MM1, offsetof(FPRState, stmm1)},
    {llvm::X86::MM2, offsetof(FPRState, stmm2)}, {llvm::X86::MM3, offsetof(FPRState, stmm3)},
    {llvm::X86::MM4, offsetof(FPRState, stmm4)}, {llvm::X86::MM5, offsetof(FPRState, stmm5)},
    {llvm::X86::MM6, offsetof(FPRState, stmm6)}, {llvm::X86::MM7, offsetof(FPRState, stmm7)},
    {llvm::X86::XMM0, offsetof(FPRState, xmm0)}, {llvm::X86::XMM1, offsetof(FPRState, xmm1)},
    {llvm::X86::XMM2, offsetof(FPRState, xmm2)}, {llvm::X86::XMM3, offsetof(FPRState, xmm3)},
    {llvm::X86::XMM4, offsetof(FPRState, xmm4)}, {llvm::X86::XMM5, offsetof(FPRState, xmm5)},
    {llvm::X86::XMM6, offsetof(FPRState, xmm6)}, {llvm::X86::XMM7, offsetof(FPRState, xmm7)},
    {llvm::X86::XMM8, offsetof(FPRState, xmm8)}, {llvm::X86::XMM9, offsetof(FPRState, xmm9)},
    {llvm::X86::XMM10, offsetof(FPRState, xmm10)}, {llvm::X86::XMM11, offsetof(FPRState, xmm11)},
    {llvm::X86::XMM12, offsetof(FPRState, xmm12)}, {llvm::X86::XMM13, offsetof(FPRState, xmm13)},
    {llvm::X86::XMM14, offsetof(FPRState, xmm14)}, {llvm::X86::XMM15, offsetof(FPRState, xmm15)},
    {llvm::X86::XMM16, -1}, {llvm::X86::XMM17, -1}, {llvm::X86::XMM18, -1}, {llvm::X86::XMM19, -1},
    {llvm::X86::XMM20, -1}, {llvm::X86::XMM21, -1}, {llvm::X86::XMM22, -1}, {llvm::X86::XMM23, -1},
    {llvm::X86::XMM24, -1}, {llvm::X86::XMM25, -1}, {llvm::X86::XMM26, -1}, {llvm::X86::XMM27, -1},
    {llvm::X86::XMM28, -1}, {llvm::X86::XMM29, -1}, {llvm::X86::XMM30, -1}, {llvm::X86::XMM31, -1},
    {llvm::X86::YMM0, offsetof(FPRState, ymm0)}, {llvm::X86::YMM1, offsetof(FPRState, ymm1)},
    {llvm::X86::YMM2, offsetof(FPRState, ymm2)}, {llvm::X86::YMM3, offsetof(FPRState, ymm3)},
    {llvm::X86::YMM4, offsetof(FPRState, ymm4)}, {llvm::X86::YMM5, offsetof(FPRState, ymm5)},
    {llvm::X86::YMM6, offsetof(FPRState, ymm6)}, {llvm::X86::YMM7, offsetof(FPRState, ymm7)},
    {llvm::X86::YMM8, offsetof(FPRState, ymm8)}, {llvm::X86::YMM9, offsetof(FPRState, ymm9)},
    {llvm::X86::YMM10, offsetof(FPRState, ymm10)}, {llvm::X86::YMM11, offsetof(FPRState, ymm11)},
    {llvm::X86::YMM12, offsetof(FPRState, ymm12)}, {llvm::X86::YMM13, offsetof(FPRState, ymm13)},
    {llvm::X86::YMM14, offsetof(FPRState, ymm14)}, {llvm::X86::YMM15, offsetof(FPRState, ymm15)},
    {llvm::X86::YMM16, -1}, {llvm::X86::YMM17, -1}, {llvm::X86::YMM18, -1}, {llvm::X86::YMM19, -1},
    {llvm::X86::YMM20, -1}, {llvm::X86::YMM21, -1}, {llvm::X86::YMM22, -1}, {llvm::X86::YMM23, -1},
    {llvm::X86::YMM24, -1}, {llvm::X86::YMM25, -1}, {llvm::X86::YMM26, -1}, {llvm::X86::YMM27, -1},
    {llvm::X86::YMM28, -1}, {llvm::X86::YMM29, -1}, {llvm::X86::YMM30, -1}, {llvm::X86::YMM31, -1},
    {llvm::X86::ZMM0, -1}, {llvm::X86::ZMM1, -1}, {llvm::X86::ZMM2, -1}, {llvm::X86::ZMM3, -1},
    {llvm::X86::ZMM4, -1}, {llvm::X86::ZMM5, -1}, {llvm::X86::ZMM6, -1}, {llvm::X86::ZMM7, -1},
    {llvm::X86::ZMM8, -1}, {llvm::X86::ZMM9, -1}, {llvm::X86::ZMM10, -1}, {llvm::X86::ZMM11, -1},
    {llvm::X86::ZMM12, -1}, {llvm::X86::ZMM13, -1}, {llvm::X86::ZMM14, -1}, {llvm::X86::ZMM15, -1},
    {llvm::X86::ZMM16, -1}, {llvm::X86::ZMM17, -1}, {llvm::X86::ZMM18, -1}, {llvm::X86::ZMM19, -1},
    {llvm::X86::ZMM20, -1}, {llvm::X86::ZMM21, -1}, {llvm::X86::ZMM22, -1}, {llvm::X86::ZMM23, -1},
    {llvm::X86::ZMM24, -1}, {llvm::X86::ZMM25, -1}, {llvm::X86::ZMM26, -1}, {llvm::X86::ZMM27, -1},
    {llvm::X86::ZMM28, -1}, {llvm::X86::ZMM29, -1}, {llvm::X86::ZMM30, -1}, {llvm::X86::ZMM31, -1},
};

const unsigned int SEG_ID[] = {
    llvm::X86::SS, llvm::X86::CS, llvm::X86::DS, llvm::X86::ES,
    llvm::X86::FS, llvm::X86::GS, llvm::X86::SSP,
};
const unsigned int size_SEG_ID = sizeof(SEG_ID)/sizeof(unsigned int);


#elif defined(QBDI_ARCH_X86)
const unsigned int GPR_ID[] = {
    llvm::X86::EAX, llvm::X86::EBX, llvm::X86::ECX, llvm::X86::EDX,
    llvm::X86::ESI, llvm::X86::EDI, llvm::X86::EBP, llvm::X86::ESP,
    llvm::X86::EIP, llvm::X86::EFLAGS
};
const unsigned int size_GPR_ID = sizeof(GPR_ID)/sizeof(unsigned int);

const std::map<unsigned int, int16_t> FPR_ID = {
    {llvm::X86::FPCW, offsetof(FPRState, rfcw)},
    {llvm::X86::FPSW, offsetof(FPRState, rfsw)},
    {llvm::X86::ST0, offsetof(FPRState, stmm0)}, {llvm::X86::ST1, offsetof(FPRState, stmm1)},
    {llvm::X86::ST2, offsetof(FPRState, stmm2)}, {llvm::X86::ST3, offsetof(FPRState, stmm3)},
    {llvm::X86::ST4, offsetof(FPRState, stmm4)}, {llvm::X86::ST5, offsetof(FPRState, stmm5)},
    {llvm::X86::ST6, offsetof(FPRState, stmm6)}, {llvm::X86::ST7, offsetof(FPRState, stmm7)},
    {llvm::X86::MM0, offsetof(FPRState, stmm0)}, {llvm::X86::MM1, offsetof(FPRState, stmm1)},
    {llvm::X86::MM2, offsetof(FPRState, stmm2)}, {llvm::X86::MM3, offsetof(FPRState, stmm3)},
    {llvm::X86::MM4, offsetof(FPRState, stmm4)}, {llvm::X86::MM5, offsetof(FPRState, stmm5)},
    {llvm::X86::MM6, offsetof(FPRState, stmm6)}, {llvm::X86::MM7, offsetof(FPRState, stmm7)},
    {llvm::X86::XMM0, offsetof(FPRState, xmm0)}, {llvm::X86::XMM1, offsetof(FPRState, xmm1)},
    {llvm::X86::XMM2, offsetof(FPRState, xmm2)}, {llvm::X86::XMM3, offsetof(FPRState, xmm3)},
    {llvm::X86::XMM4, offsetof(FPRState, xmm4)}, {llvm::X86::XMM5, offsetof(FPRState, xmm5)},
    {llvm::X86::XMM6, offsetof(FPRState, xmm6)}, {llvm::X86::XMM7, offsetof(FPRState, xmm7)},
    {llvm::X86::XMM8, -1}, {llvm::X86::XMM9, -1}, {llvm::X86::XMM10, -1}, {llvm::X86::XMM11, -1},
    {llvm::X86::XMM12, -1}, {llvm::X86::XMM13, -1}, {llvm::X86::XMM14, -1}, {llvm::X86::XMM15, -1},
    {llvm::X86::XMM16, -1}, {llvm::X86::XMM17, -1}, {llvm::X86::XMM18, -1}, {llvm::X86::XMM19, -1},
    {llvm::X86::XMM20, -1}, {llvm::X86::XMM21, -1}, {llvm::X86::XMM22, -1}, {llvm::X86::XMM23, -1},
    {llvm::X86::XMM24, -1}, {llvm::X86::XMM25, -1}, {llvm::X86::XMM26, -1}, {llvm::X86::XMM27, -1},
    {llvm::X86::XMM28, -1}, {llvm::X86::XMM29, -1}, {llvm::X86::XMM30, -1}, {llvm::X86::XMM31, -1},
    {llvm::X86::YMM0, offsetof(FPRState, ymm0)}, {llvm::X86::YMM1, offsetof(FPRState, ymm1)},
    {llvm::X86::YMM2, offsetof(FPRState, ymm2)}, {llvm::X86::YMM3, offsetof(FPRState, ymm3)},
    {llvm::X86::YMM4, offsetof(FPRState, ymm4)}, {llvm::X86::YMM5, offsetof(FPRState, ymm5)},
    {llvm::X86::YMM6, offsetof(FPRState, ymm6)}, {llvm::X86::YMM7, offsetof(FPRState, ymm7)},
    {llvm::X86::YMM8, -1}, {llvm::X86::YMM9, -1}, {llvm::X86::YMM10, -1}, {llvm::X86::YMM11, -1},
    {llvm::X86::YMM12, -1}, {llvm::X86::YMM13, -1}, {llvm::X86::YMM14, -1}, {llvm::X86::YMM15, -1},
    {llvm::X86::YMM16, -1}, {llvm::X86::YMM17, -1}, {llvm::X86::YMM18, -1}, {llvm::X86::YMM19, -1},
    {llvm::X86::YMM20, -1}, {llvm::X86::YMM21, -1}, {llvm::X86::YMM22, -1}, {llvm::X86::YMM23, -1},
    {llvm::X86::YMM24, -1}, {llvm::X86::YMM25, -1}, {llvm::X86::YMM26, -1}, {llvm::X86::YMM27, -1},
    {llvm::X86::YMM28, -1}, {llvm::X86::YMM29, -1}, {llvm::X86::YMM30, -1}, {llvm::X86::YMM31, -1},
    {llvm::X86::ZMM0, -1}, {llvm::X86::ZMM1, -1}, {llvm::X86::ZMM2, -1}, {llvm::X86::ZMM3, -1},
    {llvm::X86::ZMM4, -1}, {llvm::X86::ZMM5, -1}, {llvm::X86::ZMM6, -1}, {llvm::X86::ZMM7, -1},
    {llvm::X86::ZMM8, -1}, {llvm::X86::ZMM9, -1}, {llvm::X86::ZMM10, -1}, {llvm::X86::ZMM11, -1},
    {llvm::X86::ZMM12, -1}, {llvm::X86::ZMM13, -1}, {llvm::X86::ZMM14, -1}, {llvm::X86::ZMM15, -1},
    {llvm::X86::ZMM16, -1}, {llvm::X86::ZMM17, -1}, {llvm::X86::ZMM18, -1}, {llvm::X86::ZMM19, -1},
    {llvm::X86::ZMM20, -1}, {llvm::X86::ZMM21, -1}, {llvm::X86::ZMM22, -1}, {llvm::X86::ZMM23, -1},
    {llvm::X86::ZMM24, -1}, {llvm::X86::ZMM25, -1}, {llvm::X86::ZMM26, -1}, {llvm::X86::ZMM27, -1},
    {llvm::X86::ZMM28, -1}, {llvm::X86::ZMM29, -1}, {llvm::X86::ZMM30, -1}, {llvm::X86::ZMM31, -1},
};

const unsigned int SEG_ID[] = {
    llvm::X86::SS, llvm::X86::CS, llvm::X86::DS, llvm::X86::ES,
    llvm::X86::FS, llvm::X86::GS, llvm::X86::SSP,
};
const unsigned int size_SEG_ID = sizeof(SEG_ID)/sizeof(unsigned int);

#endif

// only DF is defined in llvm
const unsigned int FLAG_ID[] = {
    llvm::X86::NoRegister, llvm::X86::NoRegister, llvm::X86::NoRegister, llvm::X86::NoRegister,
    llvm::X86::NoRegister, llvm::X86::NoRegister, llvm::X86::NoRegister, llvm::X86::NoRegister,
    llvm::X86::NoRegister, llvm::X86::NoRegister, llvm::X86::NoRegister, llvm::X86::NoRegister,
    llvm::X86::NoRegister, llvm::X86::NoRegister, llvm::X86::DF, llvm::X86::NoRegister,
    llvm::X86::NoRegister, llvm::X86::NoRegister, llvm::X86::NoRegister, llvm::X86::NoRegister,
    llvm::X86::NoRegister, llvm::X86::NoRegister, llvm::X86::NoRegister, llvm::X86::NoRegister,
    llvm::X86::NoRegister, llvm::X86::NoRegister, llvm::X86::NoRegister, llvm::X86::NoRegister,
    llvm::X86::NoRegister, llvm::X86::NoRegister, llvm::X86::NoRegister, llvm::X86::NoRegister,
};
const unsigned int size_FLAG_ID = sizeof(FLAG_ID)/sizeof(unsigned int);



#endif // QBDI_ARCH_X86_64

// ============================================================================
// ARM Context
// ============================================================================

#if defined(QBDI_ARCH_ARM)

const unsigned int GPR_ID[] = {
    llvm::ARM::R0, llvm::ARM::R1, llvm::ARM::R2,  llvm::ARM::R3,
    llvm::ARM::R4, llvm::ARM::R5, llvm::ARM::R6,  llvm::ARM::R7,
    llvm::ARM::R8, llvm::ARM::R9, llvm::ARM::R10, llvm::ARM::R12,
    llvm::ARM::R11, llvm::ARM::SP, llvm::ARM::LR, llvm::ARM::PC,
    llvm::ARM::CPSR
};
const unsigned int size_GPR_ID = sizeof(GPR_ID)/sizeof(unsigned int);


#endif // QBDI_ARCH_ARM

}
