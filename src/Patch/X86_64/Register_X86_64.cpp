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
#include <cstdint>
#include <map>
#include <stddef.h>

#include "X86InstrInfo.h"

#include "Patch/Register.h"
#include "Utility/LogSys.h"

#include "QBDI/Config.h"
#include "QBDI/State.h"

namespace QBDI {

const unsigned int GPR_ID[] = {
#if defined(QBDI_ARCH_X86_64)
    llvm::X86::RAX, llvm::X86::RBX,    llvm::X86::RCX, llvm::X86::RDX,
    llvm::X86::RSI, llvm::X86::RDI,    llvm::X86::R8,  llvm::X86::R9,
    llvm::X86::R10, llvm::X86::R11,    llvm::X86::R12, llvm::X86::R13,
    llvm::X86::R14, llvm::X86::R15,    llvm::X86::RBP, llvm::X86::RSP,
    llvm::X86::RIP, llvm::X86::EFLAGS,
#elif defined(QBDI_ARCH_X86)
    llvm::X86::EAX, llvm::X86::EBX,    llvm::X86::ECX, llvm::X86::EDX,
    llvm::X86::ESI, llvm::X86::EDI,    llvm::X86::EBP, llvm::X86::ESP,
    llvm::X86::EIP, llvm::X86::EFLAGS,
#else
#error "Wrong architecture"
#endif
};

const unsigned int FLAG_ID[] = {
    llvm::X86::DF,
};

const unsigned int SEG_ID[] = {
    llvm::X86::SS, llvm::X86::CS, llvm::X86::DS,  llvm::X86::ES,
    llvm::X86::FS, llvm::X86::GS, llvm::X86::SSP,
};

const std::map<unsigned int, int16_t> FPR_ID = {
    {llvm::X86::FPCW, offsetof(FPRState, rfcw)},
    {llvm::X86::FPSW, offsetof(FPRState, rfsw)},
    {llvm::X86::ST0, offsetof(FPRState, stmm0)},
    {llvm::X86::ST1, offsetof(FPRState, stmm1)},
    {llvm::X86::ST2, offsetof(FPRState, stmm2)},
    {llvm::X86::ST3, offsetof(FPRState, stmm3)},
    {llvm::X86::ST4, offsetof(FPRState, stmm4)},
    {llvm::X86::ST5, offsetof(FPRState, stmm5)},
    {llvm::X86::ST6, offsetof(FPRState, stmm6)},
    {llvm::X86::ST7, offsetof(FPRState, stmm7)},
    {llvm::X86::MM0, offsetof(FPRState, stmm0)},
    {llvm::X86::MM1, offsetof(FPRState, stmm1)},
    {llvm::X86::MM2, offsetof(FPRState, stmm2)},
    {llvm::X86::MM3, offsetof(FPRState, stmm3)},
    {llvm::X86::MM4, offsetof(FPRState, stmm4)},
    {llvm::X86::MM5, offsetof(FPRState, stmm5)},
    {llvm::X86::MM6, offsetof(FPRState, stmm6)},
    {llvm::X86::MM7, offsetof(FPRState, stmm7)},
    {llvm::X86::XMM0, offsetof(FPRState, xmm0)},
    {llvm::X86::XMM1, offsetof(FPRState, xmm1)},
    {llvm::X86::XMM2, offsetof(FPRState, xmm2)},
    {llvm::X86::XMM3, offsetof(FPRState, xmm3)},
    {llvm::X86::XMM4, offsetof(FPRState, xmm4)},
    {llvm::X86::XMM5, offsetof(FPRState, xmm5)},
    {llvm::X86::XMM6, offsetof(FPRState, xmm6)},
    {llvm::X86::XMM7, offsetof(FPRState, xmm7)},
#if defined(QBDI_ARCH_X86_64)
    {llvm::X86::XMM8, offsetof(FPRState, xmm8)},
    {llvm::X86::XMM9, offsetof(FPRState, xmm9)},
    {llvm::X86::XMM10, offsetof(FPRState, xmm10)},
    {llvm::X86::XMM11, offsetof(FPRState, xmm11)},
    {llvm::X86::XMM12, offsetof(FPRState, xmm12)},
    {llvm::X86::XMM13, offsetof(FPRState, xmm13)},
    {llvm::X86::XMM14, offsetof(FPRState, xmm14)},
    {llvm::X86::XMM15, offsetof(FPRState, xmm15)},
#elif defined(QBDI_ARCH_X86)
    {llvm::X86::XMM8, -1},
    {llvm::X86::XMM9, -1},
    {llvm::X86::XMM10, -1},
    {llvm::X86::XMM11, -1},
    {llvm::X86::XMM12, -1},
    {llvm::X86::XMM13, -1},
    {llvm::X86::XMM14, -1},
    {llvm::X86::XMM15, -1},
#endif
    {llvm::X86::XMM16, -1},
    {llvm::X86::XMM17, -1},
    {llvm::X86::XMM18, -1},
    {llvm::X86::XMM19, -1},
    {llvm::X86::XMM20, -1},
    {llvm::X86::XMM21, -1},
    {llvm::X86::XMM22, -1},
    {llvm::X86::XMM23, -1},
    {llvm::X86::XMM24, -1},
    {llvm::X86::XMM25, -1},
    {llvm::X86::XMM26, -1},
    {llvm::X86::XMM27, -1},
    {llvm::X86::XMM28, -1},
    {llvm::X86::XMM29, -1},
    {llvm::X86::XMM30, -1},
    {llvm::X86::XMM31, -1},
    {llvm::X86::YMM0, offsetof(FPRState, ymm0)},
    {llvm::X86::YMM1, offsetof(FPRState, ymm1)},
    {llvm::X86::YMM2, offsetof(FPRState, ymm2)},
    {llvm::X86::YMM3, offsetof(FPRState, ymm3)},
    {llvm::X86::YMM4, offsetof(FPRState, ymm4)},
    {llvm::X86::YMM5, offsetof(FPRState, ymm5)},
    {llvm::X86::YMM6, offsetof(FPRState, ymm6)},
    {llvm::X86::YMM7, offsetof(FPRState, ymm7)},
#if defined(QBDI_ARCH_X86_64)
    {llvm::X86::YMM8, offsetof(FPRState, ymm8)},
    {llvm::X86::YMM9, offsetof(FPRState, ymm9)},
    {llvm::X86::YMM10, offsetof(FPRState, ymm10)},
    {llvm::X86::YMM11, offsetof(FPRState, ymm11)},
    {llvm::X86::YMM12, offsetof(FPRState, ymm12)},
    {llvm::X86::YMM13, offsetof(FPRState, ymm13)},
    {llvm::X86::YMM14, offsetof(FPRState, ymm14)},
    {llvm::X86::YMM15, offsetof(FPRState, ymm15)},
#elif defined(QBDI_ARCH_X86)
    {llvm::X86::YMM8, -1},
    {llvm::X86::YMM9, -1},
    {llvm::X86::YMM10, -1},
    {llvm::X86::YMM11, -1},
    {llvm::X86::YMM12, -1},
    {llvm::X86::YMM13, -1},
    {llvm::X86::YMM14, -1},
    {llvm::X86::YMM15, -1},
#endif
    {llvm::X86::YMM16, -1},
    {llvm::X86::YMM17, -1},
    {llvm::X86::YMM18, -1},
    {llvm::X86::YMM19, -1},
    {llvm::X86::YMM20, -1},
    {llvm::X86::YMM21, -1},
    {llvm::X86::YMM22, -1},
    {llvm::X86::YMM23, -1},
    {llvm::X86::YMM24, -1},
    {llvm::X86::YMM25, -1},
    {llvm::X86::YMM26, -1},
    {llvm::X86::YMM27, -1},
    {llvm::X86::YMM28, -1},
    {llvm::X86::YMM29, -1},
    {llvm::X86::YMM30, -1},
    {llvm::X86::YMM31, -1},
    {llvm::X86::ZMM0, -1},
    {llvm::X86::ZMM1, -1},
    {llvm::X86::ZMM2, -1},
    {llvm::X86::ZMM3, -1},
    {llvm::X86::ZMM4, -1},
    {llvm::X86::ZMM5, -1},
    {llvm::X86::ZMM6, -1},
    {llvm::X86::ZMM7, -1},
    {llvm::X86::ZMM8, -1},
    {llvm::X86::ZMM9, -1},
    {llvm::X86::ZMM10, -1},
    {llvm::X86::ZMM11, -1},
    {llvm::X86::ZMM12, -1},
    {llvm::X86::ZMM13, -1},
    {llvm::X86::ZMM14, -1},
    {llvm::X86::ZMM15, -1},
    {llvm::X86::ZMM16, -1},
    {llvm::X86::ZMM17, -1},
    {llvm::X86::ZMM18, -1},
    {llvm::X86::ZMM19, -1},
    {llvm::X86::ZMM20, -1},
    {llvm::X86::ZMM21, -1},
    {llvm::X86::ZMM22, -1},
    {llvm::X86::ZMM23, -1},
    {llvm::X86::ZMM24, -1},
    {llvm::X86::ZMM25, -1},
    {llvm::X86::ZMM26, -1},
    {llvm::X86::ZMM27, -1},
    {llvm::X86::ZMM28, -1},
    {llvm::X86::ZMM29, -1},
    {llvm::X86::ZMM30, -1},
    {llvm::X86::ZMM31, -1},
};

const unsigned int size_GPR_ID = sizeof(GPR_ID) / sizeof(unsigned int);
const unsigned int size_FLAG_ID = sizeof(FLAG_ID) / sizeof(unsigned int);
const unsigned int size_SEG_ID = sizeof(SEG_ID) / sizeof(unsigned int);

namespace {

constexpr uint16_t REGISTER_1BYTE[] = {
    llvm::X86::AL,
    llvm::X86::BL,
    llvm::X86::CL,
    llvm::X86::DL,
    llvm::X86::AH,
    llvm::X86::BH,
    llvm::X86::CH,
    llvm::X86::DH,
    llvm::X86::SIL,
    llvm::X86::DIL,
    llvm::X86::SPL,
    llvm::X86::BPL,
    llvm::X86::R8B,
    llvm::X86::R9B,
    llvm::X86::R10B,
    llvm::X86::R11B,
    llvm::X86::R12B,
    llvm::X86::R13B,
    llvm::X86::R14B,
    llvm::X86::R15B,
    // artificial
    llvm::X86::SIH,
    llvm::X86::DIH,
    llvm::X86::BPH,
    llvm::X86::SPH,
    llvm::X86::R8BH,
    llvm::X86::R9BH,
    llvm::X86::R10BH,
    llvm::X86::R11BH,
    llvm::X86::R12BH,
    llvm::X86::R13BH,
    llvm::X86::R14BH,
    llvm::X86::R15BH,
};

constexpr size_t REGISTER_1BYTE_SIZE =
    sizeof(REGISTER_1BYTE) / sizeof(uint16_t);

constexpr uint16_t REGISTER_2BYTES[] = {
    llvm::X86::AX,
    llvm::X86::BX,
    llvm::X86::CX,
    llvm::X86::DX,
    llvm::X86::SI,
    llvm::X86::DI,
    llvm::X86::SP,
    llvm::X86::BP,
    llvm::X86::R8W,
    llvm::X86::R9W,
    llvm::X86::R10W,
    llvm::X86::R11W,
    llvm::X86::R12W,
    llvm::X86::R13W,
    llvm::X86::R14W,
    llvm::X86::R15W,
    llvm::X86::IP,
    llvm::X86::FPCW,
    llvm::X86::FPSW,
    // artificial
    llvm::X86::HAX,
    llvm::X86::HBX,
    llvm::X86::HCX,
    llvm::X86::HDX,
    llvm::X86::HSI,
    llvm::X86::HDI,
    llvm::X86::HBP,
    llvm::X86::HSP,
    llvm::X86::HIP,
    llvm::X86::R8WH,
    llvm::X86::R9WH,
    llvm::X86::R10WH,
    llvm::X86::R11WH,
    llvm::X86::R12WH,
    llvm::X86::R13WH,
    llvm::X86::R14WH,
    llvm::X86::R15WH,
    // segment
    llvm::X86::CS,
    llvm::X86::DS,
    llvm::X86::ES,
    llvm::X86::FS,
    llvm::X86::GS,
    llvm::X86::SS,
};

constexpr size_t REGISTER_2BYTES_SIZE =
    sizeof(REGISTER_2BYTES) / sizeof(uint16_t);

constexpr uint16_t REGISTER_4BYTES[] = {
    llvm::X86::EAX,
    llvm::X86::EBX,
    llvm::X86::ECX,
    llvm::X86::EDX,
    llvm::X86::ESI,
    llvm::X86::EDI,
    llvm::X86::ESP,
    llvm::X86::EBP,
    llvm::X86::EIP,
    llvm::X86::R8D,
    llvm::X86::R9D,
    llvm::X86::R10D,
    llvm::X86::R11D,
    llvm::X86::R12D,
    llvm::X86::R13D,
    llvm::X86::R14D,
    llvm::X86::R15D,
    // RFLAGS isn't defined in llvm, the upper 32bits is never used
    llvm::X86::EFLAGS,
#if defined(QBDI_ARCH_X86)
    // shadow stack pointer
    llvm::X86::SSP,
#endif
};

constexpr size_t REGISTER_4BYTES_SIZE =
    sizeof(REGISTER_4BYTES) / sizeof(uint16_t);

constexpr uint16_t REGISTER_8BYTES[] = {
    llvm::X86::RAX,
    llvm::X86::RBX,
    llvm::X86::RCX,
    llvm::X86::RDX,
    llvm::X86::RSI,
    llvm::X86::RDI,
    llvm::X86::RSP,
    llvm::X86::RBP,
    llvm::X86::RIP,
    llvm::X86::R8,
    llvm::X86::R9,
    llvm::X86::R10,
    llvm::X86::R11,
    llvm::X86::R12,
    llvm::X86::R13,
    llvm::X86::R14,
    llvm::X86::R15,
    llvm::X86::MM0,
    llvm::X86::MM1,
    llvm::X86::MM2,
    llvm::X86::MM3,
    llvm::X86::MM4,
    llvm::X86::MM5,
    llvm::X86::MM6,
    llvm::X86::MM7,
#if defined(QBDI_ARCH_X86_64)
    // shadow stack pointer
    llvm::X86::SSP,
#endif
};

constexpr size_t REGISTER_8BYTES_SIZE =
    sizeof(REGISTER_8BYTES) / sizeof(uint16_t);

constexpr uint16_t REGISTER_10BYTES[] = {
    llvm::X86::ST0, llvm::X86::ST1, llvm::X86::ST2, llvm::X86::ST3,
    llvm::X86::ST4, llvm::X86::ST5, llvm::X86::ST6, llvm::X86::ST7,
};

constexpr size_t REGISTER_10BYTES_SIZE =
    sizeof(REGISTER_10BYTES) / sizeof(uint16_t);

constexpr uint16_t REGISTER_16BYTES[] = {
    llvm::X86::XMM0,  llvm::X86::XMM1,  llvm::X86::XMM2,  llvm::X86::XMM3,
    llvm::X86::XMM4,  llvm::X86::XMM5,  llvm::X86::XMM6,  llvm::X86::XMM7,
    llvm::X86::XMM8,  llvm::X86::XMM9,  llvm::X86::XMM10, llvm::X86::XMM11,
    llvm::X86::XMM12, llvm::X86::XMM13, llvm::X86::XMM14, llvm::X86::XMM15,
    llvm::X86::XMM16, llvm::X86::XMM17, llvm::X86::XMM18, llvm::X86::XMM19,
    llvm::X86::XMM20, llvm::X86::XMM21, llvm::X86::XMM22, llvm::X86::XMM23,
    llvm::X86::XMM24, llvm::X86::XMM25, llvm::X86::XMM26, llvm::X86::XMM27,
    llvm::X86::XMM28, llvm::X86::XMM29, llvm::X86::XMM30, llvm::X86::XMM31,
};

constexpr size_t REGISTER_16BYTES_SIZE =
    sizeof(REGISTER_16BYTES) / sizeof(uint16_t);

constexpr uint16_t REGISTER_32BYTES[] = {
    llvm::X86::YMM0,  llvm::X86::YMM1,  llvm::X86::YMM2,  llvm::X86::YMM3,
    llvm::X86::YMM4,  llvm::X86::YMM5,  llvm::X86::YMM6,  llvm::X86::YMM7,
    llvm::X86::YMM8,  llvm::X86::YMM9,  llvm::X86::YMM10, llvm::X86::YMM11,
    llvm::X86::YMM12, llvm::X86::YMM13, llvm::X86::YMM14, llvm::X86::YMM15,
    llvm::X86::YMM16, llvm::X86::YMM17, llvm::X86::YMM18, llvm::X86::YMM19,
    llvm::X86::YMM20, llvm::X86::YMM21, llvm::X86::YMM22, llvm::X86::YMM23,
    llvm::X86::YMM24, llvm::X86::YMM25, llvm::X86::YMM26, llvm::X86::YMM27,
    llvm::X86::YMM28, llvm::X86::YMM29, llvm::X86::YMM30, llvm::X86::YMM31,
};

constexpr size_t REGISTER_32BYTES_SIZE =
    sizeof(REGISTER_32BYTES) / sizeof(uint16_t);

constexpr uint16_t REGISTER_64BYTES[] = {
    llvm::X86::ZMM0,  llvm::X86::ZMM1,  llvm::X86::ZMM2,  llvm::X86::ZMM3,
    llvm::X86::ZMM4,  llvm::X86::ZMM5,  llvm::X86::ZMM6,  llvm::X86::ZMM7,
    llvm::X86::ZMM8,  llvm::X86::ZMM9,  llvm::X86::ZMM10, llvm::X86::ZMM11,
    llvm::X86::ZMM12, llvm::X86::ZMM13, llvm::X86::ZMM14, llvm::X86::ZMM15,
    llvm::X86::ZMM16, llvm::X86::ZMM17, llvm::X86::ZMM18, llvm::X86::ZMM19,
    llvm::X86::ZMM20, llvm::X86::ZMM21, llvm::X86::ZMM22, llvm::X86::ZMM23,
    llvm::X86::ZMM24, llvm::X86::ZMM25, llvm::X86::ZMM26, llvm::X86::ZMM27,
    llvm::X86::ZMM28, llvm::X86::ZMM29, llvm::X86::ZMM30, llvm::X86::ZMM31,
};

constexpr size_t REGISTER_64BYTES_SIZE =
    sizeof(REGISTER_64BYTES) / sizeof(uint16_t);

constexpr int8_t getGPRPositionConst(size_t reg) {
  switch (reg) {
    case llvm::X86::AL:
    case llvm::X86::AH:
    case llvm::X86::AX:
    case llvm::X86::HAX:
    case llvm::X86::EAX:
    case llvm::X86::RAX:
      return 0;
    case llvm::X86::BL:
    case llvm::X86::BH:
    case llvm::X86::BX:
    case llvm::X86::HBX:
    case llvm::X86::EBX:
    case llvm::X86::RBX:
      return 1;
    case llvm::X86::CL:
    case llvm::X86::CH:
    case llvm::X86::CX:
    case llvm::X86::HCX:
    case llvm::X86::ECX:
    case llvm::X86::RCX:
      return 2;
    case llvm::X86::DL:
    case llvm::X86::DH:
    case llvm::X86::DX:
    case llvm::X86::HDX:
    case llvm::X86::EDX:
    case llvm::X86::RDX:
      return 3;
    case llvm::X86::SIL:
    case llvm::X86::SIH:
    case llvm::X86::SI:
    case llvm::X86::HSI:
    case llvm::X86::ESI:
    case llvm::X86::RSI:
      return 4;
    case llvm::X86::DIL:
    case llvm::X86::DIH:
    case llvm::X86::DI:
    case llvm::X86::HDI:
    case llvm::X86::EDI:
    case llvm::X86::RDI:
      return 5;
    case llvm::X86::BPL:
    case llvm::X86::BPH:
    case llvm::X86::BP:
    case llvm::X86::HBP:
    case llvm::X86::EBP:
    case llvm::X86::RBP:
      return REG_BP;
    case llvm::X86::SPL:
    case llvm::X86::SPH:
    case llvm::X86::SP:
    case llvm::X86::HSP:
    case llvm::X86::ESP:
    case llvm::X86::RSP:
      return REG_SP;
    case llvm::X86::IP:
    case llvm::X86::HIP:
    case llvm::X86::EIP:
    case llvm::X86::RIP:
      return REG_PC;
#if defined(QBDI_ARCH_X86_64)
    case llvm::X86::R8:
    case llvm::X86::R8B:
    case llvm::X86::R8BH:
    case llvm::X86::R8D:
    case llvm::X86::R8W:
    case llvm::X86::R8WH:
      return 6;
    case llvm::X86::R9:
    case llvm::X86::R9B:
    case llvm::X86::R9BH:
    case llvm::X86::R9D:
    case llvm::X86::R9W:
    case llvm::X86::R9WH:
      return 7;
    case llvm::X86::R10:
    case llvm::X86::R10B:
    case llvm::X86::R10BH:
    case llvm::X86::R10D:
    case llvm::X86::R10W:
    case llvm::X86::R10WH:
      return 8;
    case llvm::X86::R11:
    case llvm::X86::R11B:
    case llvm::X86::R11BH:
    case llvm::X86::R11D:
    case llvm::X86::R11W:
    case llvm::X86::R11WH:
      return 9;
    case llvm::X86::R12:
    case llvm::X86::R12B:
    case llvm::X86::R12BH:
    case llvm::X86::R12D:
    case llvm::X86::R12W:
    case llvm::X86::R12WH:
      return 10;
    case llvm::X86::R13:
    case llvm::X86::R13B:
    case llvm::X86::R13BH:
    case llvm::X86::R13D:
    case llvm::X86::R13W:
    case llvm::X86::R13WH:
      return 11;
    case llvm::X86::R14:
    case llvm::X86::R14B:
    case llvm::X86::R14BH:
    case llvm::X86::R14D:
    case llvm::X86::R14W:
    case llvm::X86::R14WH:
      return 12;
    case llvm::X86::R15:
    case llvm::X86::R15B:
    case llvm::X86::R15BH:
    case llvm::X86::R15D:
    case llvm::X86::R15W:
    case llvm::X86::R15WH:
      return 13;
#endif // QBDI_ARCH_X86_64
    default:
      return -1;
  }
}

struct RegisterInfoArray {
  uint8_t size[llvm::X86::NUM_TARGET_REGS] = {0};
  int8_t pos[llvm::X86::NUM_TARGET_REGS] = {0};

  constexpr RegisterInfoArray() {
    for (size_t i = 0; i < REGISTER_1BYTE_SIZE; i++) {
      size[REGISTER_1BYTE[i]] = 1;
    }
    for (size_t i = 0; i < REGISTER_2BYTES_SIZE; i++) {
      size[REGISTER_2BYTES[i]] = 2;
    }
    for (size_t i = 0; i < REGISTER_4BYTES_SIZE; i++) {
      size[REGISTER_4BYTES[i]] = 4;
    }
    for (size_t i = 0; i < REGISTER_8BYTES_SIZE; i++) {
      size[REGISTER_8BYTES[i]] = 8;
    }
    for (size_t i = 0; i < REGISTER_10BYTES_SIZE; i++) {
      size[REGISTER_10BYTES[i]] = 10;
    }
    for (size_t i = 0; i < REGISTER_16BYTES_SIZE; i++) {
      size[REGISTER_16BYTES[i]] = 16;
    }
    for (size_t i = 0; i < REGISTER_32BYTES_SIZE; i++) {
      size[REGISTER_32BYTES[i]] = 32;
    }
    for (size_t i = 0; i < REGISTER_64BYTES_SIZE; i++) {
      size[REGISTER_64BYTES[i]] = 64;
    }
    for (size_t i = 0; i < llvm::X86::NUM_TARGET_REGS; i++) {
      pos[i] = getGPRPositionConst(i);
    }
  }

  inline uint8_t getSize(size_t reg) const {
    if (reg < llvm::X86::NUM_TARGET_REGS)
      return size[reg];

    QBDI_ERROR("No register {}", reg);
    return -1;
  }

  inline int8_t getPos(size_t reg) const {
    if (reg < llvm::X86::NUM_TARGET_REGS)
      return pos[reg];

    QBDI_ERROR("No register {}", reg);
    return -1;
  }
};

} // anonymous namespace

static constexpr RegisterInfoArray arrayInfo;

uint8_t getRegisterSize(unsigned reg) { return arrayInfo.getSize(reg); }

uint8_t getRegisterPacked(unsigned reg) { return 1; }

size_t getGPRPosition(unsigned reg) { return arrayInfo.getPos(reg); }

unsigned getUpperRegister(unsigned reg, size_t pos) {
  if (pos != 0) {
    return llvm::X86::NoRegister;
  }
  int8_t p = getGPRPosition(reg);
  if (p == -1) {
    return reg;
  }
  return GPR_ID[p];
}

} // namespace QBDI
