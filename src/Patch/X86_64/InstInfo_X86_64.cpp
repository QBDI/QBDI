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

#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrDesc.h"
#include "X86InstrInfo.h"

#include "Patch/InstInfo.h"
#include "Utility/LogSys.h"

#include "Config.h"
#include "State.h"

namespace QBDI {

namespace {

const unsigned READ_8[] = {
    llvm::X86::ADC8mi,
    llvm::X86::ADC8mi8,
    llvm::X86::ADC8mr,
    llvm::X86::ADC8rm,
    llvm::X86::ADD8mi,
    llvm::X86::ADD8mi8,
    llvm::X86::ADD8mr,
    llvm::X86::ADD8rm,
    llvm::X86::AND8mi,
    llvm::X86::AND8mi8,
    llvm::X86::AND8mr,
    llvm::X86::AND8rm,
    llvm::X86::CMP8mi,
    llvm::X86::CMP8mi8,
    llvm::X86::CMP8mr,
    llvm::X86::CMP8rm,
    llvm::X86::CMPXCHG8rm,
    llvm::X86::CRC32r32m8,
    llvm::X86::CRC32r64m8,
    llvm::X86::DEC8m,
    llvm::X86::DIV8m,
    llvm::X86::IDIV8m,
    llvm::X86::IMUL8m,
    llvm::X86::INC8m,
    llvm::X86::LCMPXCHG8,
    llvm::X86::LOCK_ADD8mi,
    llvm::X86::LOCK_ADD8mr,
    llvm::X86::LOCK_AND8mi,
    llvm::X86::LOCK_AND8mr,
    llvm::X86::LOCK_DEC8m,
    llvm::X86::LOCK_INC8m,
    llvm::X86::LOCK_OR8mi,
    llvm::X86::LOCK_OR8mr,
    llvm::X86::LOCK_SUB8mi,
    llvm::X86::LOCK_SUB8mr,
    llvm::X86::LOCK_XOR8mi,
    llvm::X86::LOCK_XOR8mr,
    llvm::X86::LXADD8,
    llvm::X86::MOV8rm,
    llvm::X86::MOVSX16rm8,
    llvm::X86::MOVSX32rm8,
    llvm::X86::MOVSX64rm8,
    llvm::X86::MOVZX16rm8,
    llvm::X86::MOVZX32rm8,
    llvm::X86::MUL8m,
    llvm::X86::NEG8m,
    llvm::X86::NOT8m,
    llvm::X86::OR8mi,
    llvm::X86::OR8mi8,
    llvm::X86::OR8mr,
    llvm::X86::OR8rm,
    llvm::X86::RCL8m1,
    llvm::X86::RCL8mCL,
    llvm::X86::RCL8mi,
    llvm::X86::RCR8m1,
    llvm::X86::RCR8mCL,
    llvm::X86::RCR8mi,
    llvm::X86::ROL8m1,
    llvm::X86::ROL8mCL,
    llvm::X86::ROL8mi,
    llvm::X86::ROR8m1,
    llvm::X86::ROR8mCL,
    llvm::X86::ROR8mi,
    llvm::X86::SAR8mi,
    llvm::X86::SBB8mi,
    llvm::X86::SBB8mr,
    llvm::X86::SBB8rm,
    llvm::X86::SHL8m1,
    llvm::X86::SHL8mCL,
    llvm::X86::SHL8mi,
    llvm::X86::SHR8m1,
    llvm::X86::SHR8mCL,
    llvm::X86::SHR8mi,
    llvm::X86::SUB8mi,
    llvm::X86::SUB8mi8,
    llvm::X86::SUB8mr,
    llvm::X86::SUB8rm,
    llvm::X86::TEST8mi,
    llvm::X86::TEST8mr,
    llvm::X86::XADD8rm,
    llvm::X86::XCHG8rm,
    llvm::X86::XOR8mi,
    llvm::X86::XOR8mi8,
    llvm::X86::XOR8mr,
    llvm::X86::XOR8rm,
};

const size_t READ_8_SIZE = sizeof(READ_8)/sizeof(unsigned);

const unsigned READ_16[] = {
    llvm::X86::ADC16mi,
    llvm::X86::ADC16mi8,
    llvm::X86::ADC16mr,
    llvm::X86::ADC16rm,
    llvm::X86::ADD16mi,
    llvm::X86::ADD16mi8,
    llvm::X86::ADD16mr,
    llvm::X86::ADD16rm,
    llvm::X86::ADD_FI16m,
    llvm::X86::AND16mi,
    llvm::X86::AND16mi8,
    llvm::X86::AND16mr,
    llvm::X86::AND16rm,
    llvm::X86::BOUNDS16rm,
    llvm::X86::BSF16rm,
    llvm::X86::BSR16rm,
    llvm::X86::BT16mi8,
    llvm::X86::BT16mr,
    llvm::X86::BTC16mi8,
    llvm::X86::BTC16mr,
    llvm::X86::BTR16mi8,
    llvm::X86::BTR16mr,
    llvm::X86::BTS16mi8,
    llvm::X86::BTS16mr,
    llvm::X86::CALL16m,
    llvm::X86::CMOV16rm,
    llvm::X86::CMP16mi,
    llvm::X86::CMP16mi8,
    llvm::X86::CMP16mr,
    llvm::X86::CMP16rm,
    llvm::X86::CMPXCHG16rm,
    llvm::X86::CRC32r32m16,
    llvm::X86::DEC16m,
    llvm::X86::DIV16m,
    llvm::X86::DIVR_FI16m,
    llvm::X86::DIV_FI16m,
    llvm::X86::FICOM16m,
    llvm::X86::FICOMP16m,
    llvm::X86::FLDCW16m,
    llvm::X86::FNSTCW16m,
    llvm::X86::IDIV16m,
    llvm::X86::ILD_F16m,
    llvm::X86::IMUL16m,
    llvm::X86::IMUL16rm,
    llvm::X86::IMUL16rmi,
    llvm::X86::IMUL16rmi8,
    llvm::X86::INC16m,
    llvm::X86::JMP16m,
    llvm::X86::LCMPXCHG16,
    llvm::X86::LOCK_ADD16mi,
    llvm::X86::LOCK_ADD16mi8,
    llvm::X86::LOCK_ADD16mr,
    llvm::X86::LOCK_AND16mi,
    llvm::X86::LOCK_AND16mi8,
    llvm::X86::LOCK_AND16mr,
    llvm::X86::LOCK_DEC16m,
    llvm::X86::LOCK_INC16m,
    llvm::X86::LOCK_OR16mi,
    llvm::X86::LOCK_OR16mi8,
    llvm::X86::LOCK_OR16mr,
    llvm::X86::LOCK_SUB16mi,
    llvm::X86::LOCK_SUB16mi8,
    llvm::X86::LOCK_SUB16mr,
    llvm::X86::LOCK_XOR16mi,
    llvm::X86::LOCK_XOR16mi8,
    llvm::X86::LOCK_XOR16mr,
    llvm::X86::LXADD16,
    llvm::X86::LZCNT16rm,
    llvm::X86::MOV16rm,
    llvm::X86::MOVBE16rm,
    llvm::X86::MOVSX32rm16,
    llvm::X86::MOVSX64rm16,
    llvm::X86::MOVZX32rm16,
    llvm::X86::MUL16m,
    llvm::X86::MUL_FI16m,
    llvm::X86::NEG16m,
    llvm::X86::NOT16m,
    llvm::X86::OR16mi,
    llvm::X86::OR16mi8,
    llvm::X86::OR16mr,
    llvm::X86::OR16rm,
    llvm::X86::POPCNT16rm,
    llvm::X86::PUSH16rmm,
    llvm::X86::RCL16m1,
    llvm::X86::RCL16mCL,
    llvm::X86::RCL16mi,
    llvm::X86::RCR16m1,
    llvm::X86::RCR16mCL,
    llvm::X86::RCR16mi,
    llvm::X86::ROL16m1,
    llvm::X86::ROL16mCL,
    llvm::X86::ROR16m1,
    llvm::X86::ROR16mCL,
    llvm::X86::SAR16m1,
    llvm::X86::SAR16mCL,
    llvm::X86::SAR16mi,
    llvm::X86::SBB16mi,
    llvm::X86::SBB16mi8,
    llvm::X86::SBB16mr,
    llvm::X86::SBB16rm,
    llvm::X86::SHL16m1,
    llvm::X86::SHL16mCL,
    llvm::X86::SHL16mi,
    llvm::X86::SHLD16mrCL,
    llvm::X86::SHLD16mri8,
    llvm::X86::SHR16m1,
    llvm::X86::SHR16mCL,
    llvm::X86::SHR16mi,
    llvm::X86::SHRD16mrCL,
    llvm::X86::SHRD16mri8,
    llvm::X86::SUB16mi,
    llvm::X86::SUB16mi8,
    llvm::X86::SUB16mr,
    llvm::X86::SUB16rm,
    llvm::X86::SUBR_FI16m,
    llvm::X86::SUB_FI16m,
    llvm::X86::TEST16mi,
    llvm::X86::TEST16mr,
    llvm::X86::TZCNT16rm,
    llvm::X86::XADD16rm,
    llvm::X86::XCHG16rm,
    llvm::X86::XOR16mi,
    llvm::X86::XOR16mi8,
    llvm::X86::XOR16mr,
    llvm::X86::XOR16rm,
};

const size_t READ_16_SIZE = sizeof(READ_16)/sizeof(unsigned);

const unsigned READ_32[] = {
    llvm::X86::ADC32mi,
    llvm::X86::ADC32mi8,
    llvm::X86::ADC32mr,
    llvm::X86::ADC32rm,
    llvm::X86::ADCX32rm,
    llvm::X86::ADD32mi,
    llvm::X86::ADD32mi8,
    llvm::X86::ADD32mr,
    llvm::X86::ADD32rm,
    llvm::X86::ADD_FI32m,
    llvm::X86::ADOX32rm,
    llvm::X86::AND32mi,
    llvm::X86::AND32mi8,
    llvm::X86::AND32mr,
    llvm::X86::AND32rm,
    llvm::X86::ANDN32rm,
    llvm::X86::BEXTR32rm,
    llvm::X86::BEXTRI32mi,
    llvm::X86::BLCFILL32rm,
    llvm::X86::BLCI32rm,
    llvm::X86::BLCIC32rm,
    llvm::X86::BLCMSK32rm,
    llvm::X86::BLCS32rm,
    llvm::X86::BLSFILL32rm,
    llvm::X86::BLSI32rm,
    llvm::X86::BLSIC32rm,
    llvm::X86::BLSMSK32rm,
    llvm::X86::BLSR32rm,
    llvm::X86::BOUNDS32rm,
    llvm::X86::BSF32rm,
    llvm::X86::BSR32rm,
    llvm::X86::BT32mi8,
    llvm::X86::BT32mr,
    llvm::X86::BTC32mi8,
    llvm::X86::BTC32mr,
    llvm::X86::BTR32mi8,
    llvm::X86::BTR32mr,
    llvm::X86::BTS32mi8,
    llvm::X86::BTS32mr,
    llvm::X86::BZHI32rm,
    llvm::X86::CALL32m,
    llvm::X86::CMOV32rm,
    llvm::X86::CMP32mi,
    llvm::X86::CMP32mi8,
    llvm::X86::CMP32mr,
    llvm::X86::CMP32rm,
    llvm::X86::CMPXCHG32rm,
    llvm::X86::CRC32r32m32,
    llvm::X86::DEC32m,
    llvm::X86::DIV32m,
    llvm::X86::DIVR_FI32m,
    llvm::X86::DIV_FI32m,
    llvm::X86::FICOM32m,
    llvm::X86::FICOMP32m,
    llvm::X86::IDIV32m,
    llvm::X86::ILD_F32m,
    llvm::X86::IMUL32m,
    llvm::X86::IMUL32rm,
    llvm::X86::IMUL32rmi,
    llvm::X86::IMUL32rmi8,
    llvm::X86::INC32m,
    llvm::X86::JMP32m,
    llvm::X86::LCMPXCHG32,
    llvm::X86::LDMXCSR,
    llvm::X86::LOCK_ADD32mi,
    llvm::X86::LOCK_ADD32mi8,
    llvm::X86::LOCK_ADD32mr,
    llvm::X86::LOCK_AND32mi,
    llvm::X86::LOCK_AND32mi8,
    llvm::X86::LOCK_AND32mr,
    llvm::X86::LOCK_DEC32m,
    llvm::X86::LOCK_INC32m,
    llvm::X86::LOCK_OR32mi,
    llvm::X86::LOCK_OR32mi8,
    llvm::X86::LOCK_OR32mr,
    llvm::X86::LOCK_SUB32mi,
    llvm::X86::LOCK_SUB32mi8,
    llvm::X86::LOCK_SUB32mr,
    llvm::X86::LOCK_XOR32mi,
    llvm::X86::LOCK_XOR32mi8,
    llvm::X86::LOCK_XOR32mr,
    llvm::X86::LXADD32,
    llvm::X86::LZCNT32rm,
    llvm::X86::MOV32rm,
    llvm::X86::MOVSX64rm32,
    llvm::X86::MUL32m,
    llvm::X86::MULX32rm,
    llvm::X86::MUL_FI32m,
    llvm::X86::NEG32m,
    llvm::X86::NOT32m,
    llvm::X86::OR32mi,
    llvm::X86::OR32mi8,
    llvm::X86::OR32mi8Locked,
    llvm::X86::OR32mr,
    llvm::X86::OR32rm,
    llvm::X86::PDEP32rm,
    llvm::X86::PEXT32rm,
    llvm::X86::POPCNT32rm,
    llvm::X86::PUSH32rmm,
    llvm::X86::RCL32m1,
    llvm::X86::RCL32mCL,
    llvm::X86::RCL32mi,
    llvm::X86::RCR32m1,
    llvm::X86::RCR32mCL,
    llvm::X86::RCR32mi,
    llvm::X86::ROL32m1,
    llvm::X86::ROL32mCL,
    llvm::X86::ROL32mi,
    llvm::X86::ROR32m1,
    llvm::X86::ROR32mCL,
    llvm::X86::ROR32mi,
    llvm::X86::RORX32mi,
    llvm::X86::SAR32m1,
    llvm::X86::SAR32mCL,
    llvm::X86::SAR32mi,
    llvm::X86::SARX32rm,
    llvm::X86::SBB32mi,
    llvm::X86::SBB32mi8,
    llvm::X86::SBB32mr,
    llvm::X86::SBB32rm,
    llvm::X86::SHL32m1,
    llvm::X86::SHL32mCL,
    llvm::X86::SHL32mi,
    llvm::X86::SHLD32mrCL,
    llvm::X86::SHLD32mri8,
    llvm::X86::SHLX32rm,
    llvm::X86::SHR32m1,
    llvm::X86::SHR32mCL,
    llvm::X86::SHR32mi,
    llvm::X86::SHRD32mrCL,
    llvm::X86::SHRD32mri8,
    llvm::X86::SHRX32rm,
    llvm::X86::SUB32mi,
    llvm::X86::SUB32mi8,
    llvm::X86::SUB32mr,
    llvm::X86::SUB32rm,
    llvm::X86::SUBR_FI32m,
    llvm::X86::SUB_FI32m,
    llvm::X86::T1MSKC32rm,
    llvm::X86::TEST32mi,
    llvm::X86::TEST32mr,
    llvm::X86::TZCNT32rm,
    llvm::X86::TZMSK32rm,
    llvm::X86::VLDMXCSR,
    llvm::X86::XADD32rm,
    llvm::X86::XCHG32rm,
    llvm::X86::XOR32mi,
    llvm::X86::XOR32mi8,
    llvm::X86::XOR32mr,
    llvm::X86::XOR32rm,
};

const size_t READ_32_SIZE = sizeof(READ_32)/sizeof(unsigned);

const unsigned READ_64[] = {
    llvm::X86::ADC64mi32,
    llvm::X86::ADC64mi8,
    llvm::X86::ADC64mr,
    llvm::X86::ADC64rm,
    llvm::X86::ADCX64rm,
    llvm::X86::ADD64mi32,
    llvm::X86::ADD64mi8,
    llvm::X86::ADD64mr,
    llvm::X86::ADD64rm,
    llvm::X86::AND64mi32,
    llvm::X86::AND64mi8,
    llvm::X86::AND64mr,
    llvm::X86::AND64rm,
    llvm::X86::ANDN64rm,
    llvm::X86::BEXTRI64mi,
    llvm::X86::BLCFILL64rm,
    llvm::X86::BLCI64rm,
    llvm::X86::BLCIC64rm,
    llvm::X86::BLCMSK64rm,
    llvm::X86::BLCS64rm,
    llvm::X86::BLSFILL64rm,
    llvm::X86::BLSI64rm,
    llvm::X86::BLSIC64rm,
    llvm::X86::BLSMSK64rm,
    llvm::X86::BLSR64rm,
    llvm::X86::BSF64rm,
    llvm::X86::BSR64rm,
    llvm::X86::BT64mi8,
    llvm::X86::BT64mr,
    llvm::X86::BTC64mi8,
    llvm::X86::BTC64mr,
    llvm::X86::BTR64mi8,
    llvm::X86::BTR64mr,
    llvm::X86::BTS64mi8,
    llvm::X86::BTS64mr,
    llvm::X86::CALL64m,
    llvm::X86::CMOV64rm,
    llvm::X86::CMP64mi32,
    llvm::X86::CMP64mi8,
    llvm::X86::CMP64mr,
    llvm::X86::CMP64rm,
    llvm::X86::CMPXCHG64rm,
    llvm::X86::CMPXCHG8B,
    llvm::X86::CRC32r64m64,
    llvm::X86::DEC64m,
    llvm::X86::DIV64m,
    llvm::X86::IDIV64m,
    llvm::X86::IMUL64m,
    llvm::X86::IMUL64rm,
    llvm::X86::IMUL64rmi32,
    llvm::X86::IMUL64rmi8,
    llvm::X86::INC64m,
    llvm::X86::JMP64m,
    llvm::X86::LCMPXCHG64,
    llvm::X86::LCMPXCHG8B,
    llvm::X86::LOCK_ADD64mi32,
    llvm::X86::LOCK_ADD64mi8,
    llvm::X86::LOCK_ADD64mr,
    llvm::X86::LOCK_AND64mi32,
    llvm::X86::LOCK_AND64mi8,
    llvm::X86::LOCK_AND64mr,
    llvm::X86::LOCK_DEC64m,
    llvm::X86::LOCK_INC64m,
    llvm::X86::LOCK_OR64mi32,
    llvm::X86::LOCK_OR64mi8,
    llvm::X86::LOCK_OR64mr,
    llvm::X86::LOCK_SUB64mi32,
    llvm::X86::LOCK_SUB64mi8,
    llvm::X86::LOCK_SUB64mr,
    llvm::X86::LOCK_XOR64mi32,
    llvm::X86::LOCK_XOR64mi8,
    llvm::X86::LOCK_XOR64mr,
    llvm::X86::LXADD64,
    llvm::X86::LZCNT64rm,
    llvm::X86::MOV64rm,
    llvm::X86::MUL64m,
    llvm::X86::MULX64rm,
    llvm::X86::NEG64m,
    llvm::X86::NOT64m,
    llvm::X86::OR64mi32,
    llvm::X86::OR64mi8,
    llvm::X86::OR64mr,
    llvm::X86::OR64rm,
    llvm::X86::PDEP64rm,
    llvm::X86::PEXT64rm,
    llvm::X86::POPCNT64rm,
    llvm::X86::PUSH64rmm,
    llvm::X86::RCL64m1,
    llvm::X86::RCL64mCL,
    llvm::X86::RCL64mi,
    llvm::X86::RCR64m1,
    llvm::X86::RCR64mCL,
    llvm::X86::RCR64mi,
    llvm::X86::ROL64m1,
    llvm::X86::ROL64mCL,
    llvm::X86::ROL64mi,
    llvm::X86::ROR64m1,
    llvm::X86::ROR64mCL,
    llvm::X86::ROR64mi,
    llvm::X86::RORX64mi,
    llvm::X86::SAR64m1,
    llvm::X86::SAR64mCL,
    llvm::X86::SAR64mi,
    llvm::X86::SBB64mi32,
    llvm::X86::SBB64mi8,
    llvm::X86::SBB64mr,
    llvm::X86::SBB64rm,
    llvm::X86::SHL64m1,
    llvm::X86::SHL64mCL,
    llvm::X86::SHL64mi,
    llvm::X86::SHLD64mrCL,
    llvm::X86::SHLD64mri8,
    llvm::X86::SHR64m1,
    llvm::X86::SHR64mCL,
    llvm::X86::SHR64mi,
    llvm::X86::SHRD64mrCL,
    llvm::X86::SHRD64mri8,
    llvm::X86::SUB64mi32,
    llvm::X86::SUB64mi8,
    llvm::X86::SUB64mr,
    llvm::X86::SUB64rm,
    llvm::X86::T1MSKC64rm,
    llvm::X86::TEST64mi32,
    llvm::X86::TEST64mr,
    llvm::X86::TZCNT64rm,
    llvm::X86::TZMSK64rm,
    llvm::X86::XADD64rm,
    llvm::X86::XCHG64rm,
    llvm::X86::XOR64mi32,
    llvm::X86::XOR64mi8,
    llvm::X86::XOR64mr,
    llvm::X86::XOR64rm,
};

const size_t READ_64_SIZE = sizeof(READ_64)/sizeof(unsigned);

const unsigned WRITE_8[] = {
    llvm::X86::ADC8mi,
    llvm::X86::ADC8mi8,
    llvm::X86::ADC8mr,
    llvm::X86::ADD8mi,
    llvm::X86::ADD8mi8,
    llvm::X86::ADD8mr,
    llvm::X86::AND8mi,
    llvm::X86::AND8mi8,
    llvm::X86::AND8mr,
    llvm::X86::CMPXCHG8rm,
    llvm::X86::DEC8m,
    llvm::X86::INC8m,
    llvm::X86::LCMPXCHG8,
    llvm::X86::LOCK_ADD8mi,
    llvm::X86::LOCK_ADD8mr,
    llvm::X86::LOCK_AND8mi,
    llvm::X86::LOCK_AND8mr,
    llvm::X86::LOCK_DEC8m,
    llvm::X86::LOCK_INC8m,
    llvm::X86::LOCK_OR8mi,
    llvm::X86::LOCK_OR8mr,
    llvm::X86::LOCK_SUB8mi,
    llvm::X86::LOCK_SUB8mr,
    llvm::X86::LOCK_XOR8mi,
    llvm::X86::LOCK_XOR8mr,
    llvm::X86::MOV8mi,
    llvm::X86::MOV8mr,
    llvm::X86::NEG8m,
    llvm::X86::NOT8m,
    llvm::X86::OR8mi,
    llvm::X86::OR8mi8,
    llvm::X86::OR8mr,
    llvm::X86::RCL8m1,
    llvm::X86::RCL8mCL,
    llvm::X86::RCL8mi,
    llvm::X86::RCR8m1,
    llvm::X86::RCR8mCL,
    llvm::X86::RCR8mi,
    llvm::X86::ROL8m1,
    llvm::X86::ROL8mCL,
    llvm::X86::ROL8mi,
    llvm::X86::ROR8m1,
    llvm::X86::ROR8mCL,
    llvm::X86::ROR8mi,
    llvm::X86::SAR8mi,
    llvm::X86::SBB8mi,
    llvm::X86::SBB8mr,
    llvm::X86::SETCCm,
    llvm::X86::SHL8m1,
    llvm::X86::SHL8mCL,
    llvm::X86::SHL8mi,
    llvm::X86::SHR8m1,
    llvm::X86::SHR8mCL,
    llvm::X86::SHR8mi,
    llvm::X86::SUB8mi,
    llvm::X86::SUB8mi8,
    llvm::X86::SUB8mr,
    llvm::X86::XADD8rm,
    llvm::X86::XOR8mi,
    llvm::X86::XOR8mi8,
    llvm::X86::XOR8mr,
};

const size_t WRITE_8_SIZE = sizeof(WRITE_8)/sizeof(unsigned);

const unsigned WRITE_16[] = {
    llvm::X86::ADC16mi,
    llvm::X86::ADC16mi8,
    llvm::X86::ADC16mr,
    llvm::X86::ADD16mi,
    llvm::X86::ADD16mi8,
    llvm::X86::ADD16mr,
    llvm::X86::AND16mi,
    llvm::X86::AND16mi8,
    llvm::X86::AND16mr,
    llvm::X86::BT16mi8,
    llvm::X86::BT16mr,
    llvm::X86::BTC16mi8,
    llvm::X86::BTC16mr,
    llvm::X86::BTR16mi8,
    llvm::X86::BTR16mr,
    llvm::X86::BTS16mi8,
    llvm::X86::BTS16mr,
    llvm::X86::CMPXCHG16rm,
    llvm::X86::DEC16m,
    llvm::X86::INC16m,
    llvm::X86::ISTT_FP16m,
    llvm::X86::IST_F16m,
    llvm::X86::IST_FP16m,
    llvm::X86::LCMPXCHG16,
    llvm::X86::LOCK_ADD16mi,
    llvm::X86::LOCK_ADD16mi8,
    llvm::X86::LOCK_ADD16mr,
    llvm::X86::LOCK_AND16mi,
    llvm::X86::LOCK_AND16mi8,
    llvm::X86::LOCK_AND16mr,
    llvm::X86::LOCK_DEC16m,
    llvm::X86::LOCK_INC16m,
    llvm::X86::LOCK_OR16mi,
    llvm::X86::LOCK_OR16mi8,
    llvm::X86::LOCK_OR16mr,
    llvm::X86::LOCK_SUB16mi,
    llvm::X86::LOCK_SUB16mi8,
    llvm::X86::LOCK_SUB16mr,
    llvm::X86::LOCK_XOR16mi,
    llvm::X86::LOCK_XOR16mi8,
    llvm::X86::LOCK_XOR16mr,
    llvm::X86::MOV16mi,
    llvm::X86::MOV16mr,
    llvm::X86::MOVBE16mr,
    llvm::X86::NEG16m,
    llvm::X86::NOT16m,
    llvm::X86::OR16mi,
    llvm::X86::OR16mi8,
    llvm::X86::OR16mr,
    llvm::X86::POP16rmm,
    llvm::X86::RCL16m1,
    llvm::X86::RCL16mCL,
    llvm::X86::RCL16mi,
    llvm::X86::RCR16m1,
    llvm::X86::RCR16mCL,
    llvm::X86::RCR16mi,
    llvm::X86::ROL16m1,
    llvm::X86::ROL16mCL,
    llvm::X86::ROR16m1,
    llvm::X86::ROR16mCL,
    llvm::X86::SAR16m1,
    llvm::X86::SAR16mCL,
    llvm::X86::SAR16mi,
    llvm::X86::SBB16mi,
    llvm::X86::SBB16mi8,
    llvm::X86::SBB16mr,
    llvm::X86::SHL16m1,
    llvm::X86::SHL16mCL,
    llvm::X86::SHL16mi,
    llvm::X86::SHLD16mrCL,
    llvm::X86::SHLD16mri8,
    llvm::X86::SHR16m1,
    llvm::X86::SHR16mCL,
    llvm::X86::SHR16mi,
    llvm::X86::SHRD16mrCL,
    llvm::X86::SHRD16mri8,
    llvm::X86::SUB16mi,
    llvm::X86::SUB16mi8,
    llvm::X86::SUB16mr,
    llvm::X86::XADD16rm,
    llvm::X86::XOR16mi,
    llvm::X86::XOR16mi8,
    llvm::X86::XOR16mr,
};

const size_t WRITE_16_SIZE = sizeof(WRITE_16)/sizeof(unsigned);

const unsigned WRITE_32[] = {
    llvm::X86::ADC32mi,
    llvm::X86::ADC32mi8,
    llvm::X86::ADC32mr,
    llvm::X86::ADD32mi,
    llvm::X86::ADD32mi8,
    llvm::X86::ADD32mr,
    llvm::X86::AND32mi,
    llvm::X86::AND32mi8,
    llvm::X86::AND32mr,
    llvm::X86::CMPXCHG32rm,
    llvm::X86::DEC32m,
    llvm::X86::INC32m,
    llvm::X86::ISTT_FP32m,
    llvm::X86::IST_F32m,
    llvm::X86::IST_FP32m,
    llvm::X86::LCMPXCHG32,
    llvm::X86::LOCK_ADD32mi,
    llvm::X86::LOCK_ADD32mi8,
    llvm::X86::LOCK_ADD32mr,
    llvm::X86::LOCK_AND32mi,
    llvm::X86::LOCK_AND32mi8,
    llvm::X86::LOCK_AND32mr,
    llvm::X86::LOCK_DEC32m,
    llvm::X86::LOCK_INC32m,
    llvm::X86::LOCK_OR32mi,
    llvm::X86::LOCK_OR32mi8,
    llvm::X86::LOCK_OR32mr,
    llvm::X86::LOCK_SUB32mi,
    llvm::X86::LOCK_SUB32mi8,
    llvm::X86::LOCK_SUB32mr,
    llvm::X86::LOCK_XOR32mi,
    llvm::X86::LOCK_XOR32mi8,
    llvm::X86::LOCK_XOR32mr,
    llvm::X86::MOV32mi,
    llvm::X86::MOV32mr,
    llvm::X86::MOVBE32mr,
    llvm::X86::MOVNTImr,
    llvm::X86::NEG32m,
    llvm::X86::NOT32m,
    llvm::X86::OR32mi,
    llvm::X86::OR32mi8,
    llvm::X86::OR32mi8Locked,
    llvm::X86::OR32mr,
    llvm::X86::POP32rmm,
    llvm::X86::RCL32m1,
    llvm::X86::RCL32mCL,
    llvm::X86::RCL32mi,
    llvm::X86::RCR32m1,
    llvm::X86::RCR32mCL,
    llvm::X86::RCR32mi,
    llvm::X86::ROL32m1,
    llvm::X86::ROL32mCL,
    llvm::X86::ROL32mi,
    llvm::X86::ROR32m1,
    llvm::X86::ROR32mCL,
    llvm::X86::ROR32mi,
    llvm::X86::SAR32m1,
    llvm::X86::SAR32mCL,
    llvm::X86::SAR32mi,
    llvm::X86::SBB32mi,
    llvm::X86::SBB32mi8,
    llvm::X86::SBB32mr,
    llvm::X86::SHL32m1,
    llvm::X86::SHL32mCL,
    llvm::X86::SHL32mi,
    llvm::X86::SHLD32mrCL,
    llvm::X86::SHLD32mri8,
    llvm::X86::SHR32m1,
    llvm::X86::SHR32mCL,
    llvm::X86::SHR32mi,
    llvm::X86::SHRD32mrCL,
    llvm::X86::SHRD32mri8,
    llvm::X86::STMXCSR,
    llvm::X86::SUB32mi,
    llvm::X86::SUB32mi8,
    llvm::X86::SUB32mr,
    llvm::X86::VSTMXCSR,
    llvm::X86::XADD32rm,
    llvm::X86::XOR32mi,
    llvm::X86::XOR32mi8,
    llvm::X86::XOR32mr,
};

const size_t WRITE_32_SIZE = sizeof(WRITE_32)/sizeof(unsigned);

const unsigned WRITE_64[] = {
    llvm::X86::ADC64mi32,
    llvm::X86::ADC64mi8,
    llvm::X86::ADC64mr,
    llvm::X86::ADD64mi32,
    llvm::X86::ADD64mi8,
    llvm::X86::ADD64mr,
    llvm::X86::AND64mi32,
    llvm::X86::AND64mi8,
    llvm::X86::AND64mr,
    llvm::X86::CMPXCHG64rm,
    llvm::X86::CMPXCHG8B,
    llvm::X86::DEC64m,
    llvm::X86::ILD_F64m,
    llvm::X86::INC64m,
    llvm::X86::ISTT_FP64m,
    llvm::X86::IST_FP64m,
    llvm::X86::LCMPXCHG64,
    llvm::X86::LCMPXCHG8B,
    llvm::X86::LOCK_ADD64mi32,
    llvm::X86::LOCK_ADD64mi8,
    llvm::X86::LOCK_ADD64mr,
    llvm::X86::LOCK_AND64mi32,
    llvm::X86::LOCK_AND64mi8,
    llvm::X86::LOCK_AND64mr,
    llvm::X86::LOCK_DEC64m,
    llvm::X86::LOCK_INC64m,
    llvm::X86::LOCK_OR64mi32,
    llvm::X86::LOCK_OR64mi8,
    llvm::X86::LOCK_OR64mr,
    llvm::X86::LOCK_SUB64mi32,
    llvm::X86::LOCK_SUB64mi8,
    llvm::X86::LOCK_SUB64mr,
    llvm::X86::LOCK_XOR64mi32,
    llvm::X86::LOCK_XOR64mi8,
    llvm::X86::LOCK_XOR64mr,
    llvm::X86::MOV64mi32,
    llvm::X86::MOV64mr,
    llvm::X86::NEG64m,
    llvm::X86::NOT64m,
    llvm::X86::OR64mi32,
    llvm::X86::OR64mi8,
    llvm::X86::OR64mr,
    llvm::X86::POP64rmm,
    llvm::X86::RCL64m1,
    llvm::X86::RCL64mCL,
    llvm::X86::RCL64mi,
    llvm::X86::RCR64m1,
    llvm::X86::RCR64mCL,
    llvm::X86::RCR64mi,
    llvm::X86::ROL64m1,
    llvm::X86::ROL64mCL,
    llvm::X86::ROL64mi,
    llvm::X86::ROR64m1,
    llvm::X86::ROR64mCL,
    llvm::X86::ROR64mi,
    llvm::X86::SAR64m1,
    llvm::X86::SAR64mCL,
    llvm::X86::SAR64mi,
    llvm::X86::SBB64mi32,
    llvm::X86::SBB64mi8,
    llvm::X86::SBB64mr,
    llvm::X86::SHL64m1,
    llvm::X86::SHL64mCL,
    llvm::X86::SHL64mi,
    llvm::X86::SHLD64mrCL,
    llvm::X86::SHLD64mri8,
    llvm::X86::SHR64m1,
    llvm::X86::SHR64mCL,
    llvm::X86::SHR64mi,
    llvm::X86::SHRD64mrCL,
    llvm::X86::SHRD64mri8,
    llvm::X86::SUB64mi32,
    llvm::X86::SUB64mi8,
    llvm::X86::SUB64mr,
    llvm::X86::XADD64rm,
    llvm::X86::XOR64mi32,
    llvm::X86::XOR64mi8,
    llvm::X86::XOR64mr,
};

const size_t WRITE_64_SIZE = sizeof(WRITE_64)/sizeof(unsigned);

const unsigned STACK_WRITE_16[] = {
    llvm::X86::PUSH16i8,
    llvm::X86::PUSH16r,
    llvm::X86::PUSH16rmm,
    llvm::X86::PUSH16rmr,
    llvm::X86::PUSHi16,
};

const size_t STACK_WRITE_16_SIZE = sizeof(STACK_WRITE_16)/sizeof(unsigned);

const unsigned STACK_WRITE_32[] = {
#ifdef QBDI_ARCH_X86
    llvm::X86::CALL16m,
    llvm::X86::CALL16r,
    llvm::X86::CALL32m,
    llvm::X86::CALL32r,
    llvm::X86::CALL64m,
    llvm::X86::CALL64pcrel32,
    llvm::X86::CALL64r,
    llvm::X86::CALLpcrel16,
    llvm::X86::CALLpcrel32,
    llvm::X86::ENTER,
#endif
    llvm::X86::PUSH32i8,
    llvm::X86::PUSH32r,
    llvm::X86::PUSH32rmm,
    llvm::X86::PUSH32rmr,
    llvm::X86::PUSHi32,
};

const size_t STACK_WRITE_32_SIZE = sizeof(STACK_WRITE_32)/sizeof(unsigned);

const unsigned STACK_WRITE_64[] = {
#ifdef QBDI_ARCH_X86_64
    llvm::X86::CALL16m,
    llvm::X86::CALL16r,
    llvm::X86::CALL32m,
    llvm::X86::CALL32r,
    llvm::X86::CALL64m,
    llvm::X86::CALL64pcrel32,
    llvm::X86::CALL64r,
    llvm::X86::CALLpcrel16,
    llvm::X86::CALLpcrel32,
    llvm::X86::ENTER,
#endif
    llvm::X86::PUSH64i32,
    llvm::X86::PUSH64i8,
    llvm::X86::PUSH64r,
    llvm::X86::PUSH64rmm,
    llvm::X86::PUSH64rmr,
};

const size_t STACK_WRITE_64_SIZE = sizeof(STACK_WRITE_64)/sizeof(unsigned);

const unsigned STACK_READ_16[] = {
    llvm::X86::POP16r,
    llvm::X86::POP16rmm,
};

const size_t STACK_READ_16_SIZE = sizeof(STACK_READ_16)/sizeof(unsigned);

const unsigned STACK_READ_32[] = {
#ifdef QBDI_ARCH_X86
    llvm::X86::LEAVE,
#endif
    llvm::X86::POP32r,
    llvm::X86::POP32rmm,
#ifdef QBDI_ARCH_X86
    llvm::X86::RETIL,
    llvm::X86::RETIQ,
    llvm::X86::RETIW,
    llvm::X86::RETL,
    llvm::X86::RETQ,
    llvm::X86::RETW,
#endif
};

const size_t STACK_READ_32_SIZE = sizeof(STACK_READ_32)/sizeof(unsigned);

const unsigned STACK_READ_64[] = {
#ifdef QBDI_ARCH_X86_64
    llvm::X86::LEAVE,
#endif
    llvm::X86::LEAVE64,
    llvm::X86::POP64r,
    llvm::X86::POP64rmm,
#ifdef QBDI_ARCH_X86_64
    llvm::X86::RETIL,
    llvm::X86::RETIQ,
    llvm::X86::RETIW,
    llvm::X86::RETL,
    llvm::X86::RETQ,
    llvm::X86::RETW,
#endif
};

const size_t STACK_READ_64_SIZE = sizeof(STACK_READ_64)/sizeof(unsigned);

/* Highest 8 bits are the write access, lowest 8 bits are the read access. For each 8 bits part: the
 * highest bit stores if the access is a stack access or not while the lowest 7 bits store the
 * unsigned access size in bytes (thus up to 127 bytes). A size of 0 means no access.
 *
 * -----------------------------------------------------------------------------------------------------------------
 * |                     WRITE ACCESS                      |                      READ ACCESS                      |
 * -----------------------------------------------------------------------------------------------------------------
 * | 1 bit stack access flag | 7 bits unsigned access size | 1 bit stack access flag | 7 bits unsigned access size |
 * -----------------------------------------------------------------------------------------------------------------
*/

constexpr uint16_t STACK_ACCESS_FLAG = 0x80;
constexpr uint16_t READ(uint16_t s) {return s & 0x7f;}
constexpr uint16_t WRITE(uint16_t s) {return (s & 0x7f)<<8;}
constexpr uint16_t STACK_READ(uint16_t s) {return STACK_ACCESS_FLAG | READ(s);}
constexpr uint16_t STACK_WRITE(uint16_t s) {return ((STACK_ACCESS_FLAG)<<8) | WRITE(s);}
constexpr uint16_t GET_READ_SIZE(uint16_t v) {return v & 0x7f;}
constexpr uint16_t GET_WRITE_SIZE(uint16_t v) {return (v>>8) & 0x7f;}
constexpr uint16_t IS_STACK_READ(uint16_t v) {return (v & STACK_ACCESS_FLAG) == STACK_ACCESS_FLAG;}
constexpr uint16_t IS_STACK_WRITE(uint16_t v) {return ((v>>8) & STACK_ACCESS_FLAG) == STACK_ACCESS_FLAG;}

uint16_t MEMACCESS_INFO_TABLE[llvm::X86::INSTRUCTION_LIST_END] = {0};

inline void _initMemAccessRead(const unsigned buff[], size_t buff_size, uint16_t len) {
    for(size_t i = 0; i < buff_size; i++) {
        #if defined(_QBDI_LOG_DEBUG)
        RequireAction("_initMemAccessRead", GET_READ_SIZE(MEMACCESS_INFO_TABLE[buff[i]]) == 0, {
            LogError("_initMemAccessRead", "%d already have value %d\n", buff[i], GET_READ_SIZE(MEMACCESS_INFO_TABLE[buff[i]]));
            abort();
        });
        #endif
        MEMACCESS_INFO_TABLE[buff[i]] |= READ(len);
    }
}

inline void _initMemAccessWrite(const unsigned buff[], size_t buff_size, uint16_t len) {
    for(size_t i = 0; i < buff_size; i++) {
        #if defined(_QBDI_LOG_DEBUG)
        RequireAction("_initMemAccessWrite", GET_WRITE_SIZE(MEMACCESS_INFO_TABLE[buff[i]]) == 0, {
            LogError("_initMemAccessWrite", "%d already have value %d\n", buff[i], GET_WRITE_SIZE(MEMACCESS_INFO_TABLE[buff[i]]));
            abort();
        });
        #endif
        MEMACCESS_INFO_TABLE[buff[i]] |= WRITE(len);
    }
}

inline void _initMemAccessStackRead(const unsigned buff[], size_t buff_size, uint16_t len) {
    for(size_t i = 0; i < buff_size; i++) {
        #if defined(_QBDI_LOG_DEBUG)
        RequireAction("_initMemAccessStackRead", GET_READ_SIZE(MEMACCESS_INFO_TABLE[buff[i]]) == 0, {
            LogError("_initMemAccessStackRead", "%d already have value %d\n", buff[i], GET_READ_SIZE(MEMACCESS_INFO_TABLE[buff[i]]));
            abort();
        });
        #endif
        MEMACCESS_INFO_TABLE[buff[i]] |= STACK_READ(len);
    }
}

inline void _initMemAccessStackWrite(const unsigned buff[], size_t buff_size, uint16_t len) {
    for(size_t i = 0; i < buff_size; i++) {
        #if defined(_QBDI_LOG_DEBUG)
        RequireAction("_initMemAccessStackWrite", GET_WRITE_SIZE(MEMACCESS_INFO_TABLE[buff[i]]) == 0, {
            LogError("_initMemAccessStackWrite", "%d already have value %d\n", buff[i], GET_WRITE_SIZE(MEMACCESS_INFO_TABLE[buff[i]]));
            abort();
        });
        #endif
        MEMACCESS_INFO_TABLE[buff[i]] |= STACK_WRITE(len);
    }
}

} // anonymous namespace

void initMemAccessInfo() {
    // read
    _initMemAccessRead(READ_8, READ_8_SIZE, 1);
    _initMemAccessRead(READ_16, READ_16_SIZE, 2);
    _initMemAccessRead(READ_32, READ_32_SIZE, 4);
    _initMemAccessRead(READ_64, READ_64_SIZE, 8);
    // write
    _initMemAccessWrite(WRITE_8, WRITE_8_SIZE, 1);
    _initMemAccessWrite(WRITE_16, WRITE_16_SIZE, 2);
    _initMemAccessWrite(WRITE_32, WRITE_32_SIZE, 4);
    _initMemAccessWrite(WRITE_64, WRITE_64_SIZE, 8);
    // read stack
    _initMemAccessStackRead(STACK_READ_16, STACK_READ_16_SIZE, 2);
    _initMemAccessStackRead(STACK_READ_32, STACK_READ_32_SIZE, 4);
    _initMemAccessStackRead(STACK_READ_64, STACK_READ_64_SIZE, 8);
    // write stack
    _initMemAccessStackWrite(STACK_WRITE_16, STACK_WRITE_16_SIZE, 2);
    _initMemAccessStackWrite(STACK_WRITE_32, STACK_WRITE_32_SIZE, 4);
    _initMemAccessStackWrite(STACK_WRITE_64, STACK_WRITE_64_SIZE, 8);
}

unsigned getReadSize(const llvm::MCInst& inst) {
    return GET_READ_SIZE(MEMACCESS_INFO_TABLE[inst.getOpcode()]);
}

unsigned getWriteSize(const llvm::MCInst& inst) {
    return GET_WRITE_SIZE(MEMACCESS_INFO_TABLE[inst.getOpcode()]);
}

bool isStackRead(const llvm::MCInst& inst) {
    return IS_STACK_READ(MEMACCESS_INFO_TABLE[inst.getOpcode()]);
}

bool isStackWrite(const llvm::MCInst& inst) {
    return IS_STACK_WRITE(MEMACCESS_INFO_TABLE[inst.getOpcode()]);
}

unsigned getImmediateSize(const llvm::MCInst& inst, const llvm::MCInstrDesc* desc) {
    return llvm::X86II::getSizeOfImm(desc->TSFlags);
}

bool useAllRegisters(const llvm::MCInst& inst) {
    if constexpr(is_x86) {
        static const unsigned InstAllRegisters[] = {
            llvm::X86::PUSHA16,
            llvm::X86::PUSHA32,
            llvm::X86::POPA16,
            llvm::X86::POPA32
        };
        unsigned opcode = inst.getOpcode();

        for (unsigned op : InstAllRegisters) {
            if (op == opcode)
                return true;
        }
    }

    return false;
}

};
