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

#include <stdint.h>

#include "X86InstrInfo.h"

#include "Patch/Types.h"
#include "Utility/LogSys.h"

namespace QBDI {
namespace {

uint16_t REGISTER_1BYTE[] = {
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
    //llvm::X86::SIH,
    //llvm::X86::DIH,
    //llvm::X86::BPH,
    //llvm::X86::SPH,
    //llvm::X86::R8BH,
    //llvm::X86::R9BH,
    //llvm::X86::R10BH,
    //llvm::X86::R11BH,
    //llvm::X86::R12BH,
    //llvm::X86::R13BH,
    //llvm::X86::R14BH,
    //llvm::X86::R15BH,
};

size_t REGISTER_1BYTE_SIZE = sizeof(REGISTER_1BYTE)/sizeof(uint16_t);

uint16_t REGISTER_2BYTES[] = {
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
    // artificial
    //llvm::X86::HAX,
    //llvm::X86::HBX,
    //llvm::X86::HCX,
    //llvm::X86::HDX,
    //llvm::X86::HSI,
    //llvm::X86::HDI,
    //llvm::X86::HBP,
    //llvm::X86::HSP,
    //llvm::X86::HIP,
    //llvm::X86::R8WH,
    //llvm::X86::R9WH,
    //llvm::X86::R10WH,
    //llvm::X86::R11WH,
    //llvm::X86::R12WH,
    //llvm::X86::R13WH,
    //llvm::X86::R14WH,
    //llvm::X86::R15WH,
};

size_t REGISTER_2BYTES_SIZE = sizeof(REGISTER_2BYTES)/sizeof(uint16_t);

uint16_t REGISTER_4BYTES[] = {
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

};

size_t REGISTER_4BYTES_SIZE = sizeof(REGISTER_4BYTES)/sizeof(uint16_t);

uint16_t REGISTER_8BYTES[] = {
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
};

size_t REGISTER_8BYTES_SIZE = sizeof(REGISTER_8BYTES)/sizeof(uint16_t);

uint16_t REGISTER_10BYTES[] = {
    llvm::X86::ST0,
    llvm::X86::ST1,
    llvm::X86::ST2,
    llvm::X86::ST3,
    llvm::X86::ST4,
    llvm::X86::ST5,
    llvm::X86::ST6,
    llvm::X86::ST7,
};

size_t REGISTER_10BYTES_SIZE = sizeof(REGISTER_10BYTES)/sizeof(uint16_t);

uint16_t REGISTER_16BYTES[] = {
    llvm::X86::XMM0,
    llvm::X86::XMM1,
    llvm::X86::XMM2,
    llvm::X86::XMM3,
    llvm::X86::XMM4,
    llvm::X86::XMM5,
    llvm::X86::XMM6,
    llvm::X86::XMM7,
    llvm::X86::XMM8,
    llvm::X86::XMM9,
    llvm::X86::XMM10,
    llvm::X86::XMM11,
    llvm::X86::XMM12,
    llvm::X86::XMM13,
    llvm::X86::XMM14,
    llvm::X86::XMM15,
    llvm::X86::XMM16,
    llvm::X86::XMM17,
    llvm::X86::XMM18,
    llvm::X86::XMM19,
    llvm::X86::XMM20,
    llvm::X86::XMM21,
    llvm::X86::XMM22,
    llvm::X86::XMM23,
    llvm::X86::XMM24,
    llvm::X86::XMM25,
    llvm::X86::XMM26,
    llvm::X86::XMM27,
    llvm::X86::XMM28,
    llvm::X86::XMM29,
    llvm::X86::XMM30,
    llvm::X86::XMM31,
};

size_t REGISTER_16BYTES_SIZE = sizeof(REGISTER_16BYTES)/sizeof(uint16_t);

uint16_t REGISTER_32BYTES[] = {
    llvm::X86::YMM0,
    llvm::X86::YMM1,
    llvm::X86::YMM2,
    llvm::X86::YMM3,
    llvm::X86::YMM4,
    llvm::X86::YMM5,
    llvm::X86::YMM6,
    llvm::X86::YMM7,
    llvm::X86::YMM8,
    llvm::X86::YMM9,
    llvm::X86::YMM10,
    llvm::X86::YMM11,
    llvm::X86::YMM12,
    llvm::X86::YMM13,
    llvm::X86::YMM14,
    llvm::X86::YMM15,
    llvm::X86::YMM16,
    llvm::X86::YMM17,
    llvm::X86::YMM18,
    llvm::X86::YMM19,
    llvm::X86::YMM20,
    llvm::X86::YMM21,
    llvm::X86::YMM22,
    llvm::X86::YMM23,
    llvm::X86::YMM24,
    llvm::X86::YMM25,
    llvm::X86::YMM26,
    llvm::X86::YMM27,
    llvm::X86::YMM28,
    llvm::X86::YMM29,
    llvm::X86::YMM30,
    llvm::X86::YMM31,
};

size_t REGISTER_32BYTES_SIZE = sizeof(REGISTER_32BYTES)/sizeof(uint16_t);

uint16_t REGISTER_64BYTES[] = {
    llvm::X86::ZMM0,
    llvm::X86::ZMM1,
    llvm::X86::ZMM2,
    llvm::X86::ZMM3,
    llvm::X86::ZMM4,
    llvm::X86::ZMM5,
    llvm::X86::ZMM6,
    llvm::X86::ZMM7,
    llvm::X86::ZMM8,
    llvm::X86::ZMM9,
    llvm::X86::ZMM10,
    llvm::X86::ZMM11,
    llvm::X86::ZMM12,
    llvm::X86::ZMM13,
    llvm::X86::ZMM14,
    llvm::X86::ZMM15,
    llvm::X86::ZMM16,
    llvm::X86::ZMM17,
    llvm::X86::ZMM18,
    llvm::X86::ZMM19,
    llvm::X86::ZMM20,
    llvm::X86::ZMM21,
    llvm::X86::ZMM22,
    llvm::X86::ZMM23,
    llvm::X86::ZMM24,
    llvm::X86::ZMM25,
    llvm::X86::ZMM26,
    llvm::X86::ZMM27,
    llvm::X86::ZMM28,
    llvm::X86::ZMM29,
    llvm::X86::ZMM30,
    llvm::X86::ZMM31,
};

size_t REGISTER_64BYTES_SIZE = sizeof(REGISTER_64BYTES)/sizeof(uint16_t);

uint16_t REGISTER_SIZE_TABLE[llvm::X86::NUM_TARGET_REGS] = {0};

} // anonymous namespace

void initRegisterSize() {
    for(size_t i = 0; i < REGISTER_1BYTE_SIZE; i++) {
        REGISTER_SIZE_TABLE[REGISTER_1BYTE[i]] = 1;
    }
    for(size_t i = 0; i < REGISTER_2BYTES_SIZE; i++) {
        REGISTER_SIZE_TABLE[REGISTER_2BYTES[i]] = 2;
    }
    for(size_t i = 0; i < REGISTER_4BYTES_SIZE; i++) {
        REGISTER_SIZE_TABLE[REGISTER_4BYTES[i]] = 4;
    }
    for(size_t i = 0; i < REGISTER_8BYTES_SIZE; i++) {
        REGISTER_SIZE_TABLE[REGISTER_8BYTES[i]] = 8;
    }
    for(size_t i = 0; i < REGISTER_10BYTES_SIZE; i++) {
        REGISTER_SIZE_TABLE[REGISTER_10BYTES[i]] = 10;
    }
    for(size_t i = 0; i < REGISTER_16BYTES_SIZE; i++) {
        REGISTER_SIZE_TABLE[REGISTER_16BYTES[i]] = 16;
    }
    for(size_t i = 0; i < REGISTER_32BYTES_SIZE; i++) {
        REGISTER_SIZE_TABLE[REGISTER_32BYTES[i]] = 32;
    }
    for(size_t i = 0; i < REGISTER_64BYTES_SIZE; i++) {
        REGISTER_SIZE_TABLE[REGISTER_64BYTES[i]] = 64;
    }
}

uint16_t getRegisterSize(unsigned reg) {
    if(reg < llvm::X86::NUM_TARGET_REGS)
        return REGISTER_SIZE_TABLE[reg];

    LogError("getRegisterSize", "No register %u", reg);
    return 0;
}

} // namespace QBDI
