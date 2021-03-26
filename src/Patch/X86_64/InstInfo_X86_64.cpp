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
#include <stdint.h>

#include "X86InstrInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrDesc.h"

#include "Patch/X86_64/InstInfo_X86_64.h"
#include "Utility/LogSys.h"

#include "QBDI/Config.h"
#include "QBDI/State.h"

#define CHECK_TABLE 0

namespace QBDI {
/* TODO instruction (no yet supported)
 * See test/Patch/MemoryAccessTable_X86_64.cpp
 */
namespace {

constexpr unsigned READ_8[] = {
    // clang-format off
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
    llvm::X86::CMPSB,
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
    llvm::X86::LODSB,
    llvm::X86::LXADD8,
    llvm::X86::MOV8ao16,
    llvm::X86::MOV8ao32,
    llvm::X86::MOV8ao64,
    llvm::X86::MOV8rm,
    llvm::X86::MOV8rm_NOREX,
    llvm::X86::MOVSB,
    llvm::X86::MOVSX16rm8,
    llvm::X86::MOVSX32rm8,
    llvm::X86::MOVSX32rm8_NOREX,
    llvm::X86::MOVSX64rm8,
    llvm::X86::MOVZX16rm8,
    llvm::X86::MOVZX32rm8,
    llvm::X86::MOVZX32rm8_NOREX,
    llvm::X86::MOVZX64rm8,
    llvm::X86::MUL8m,
    llvm::X86::NEG8m,
    llvm::X86::NOT8m,
    llvm::X86::OR8mi,
    llvm::X86::OR8mi8,
    llvm::X86::OR8mr,
    llvm::X86::OR8rm,
    llvm::X86::PINSRBrm,
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
    llvm::X86::SAR8m1,
    llvm::X86::SAR8mCL,
    llvm::X86::SAR8mi,
    llvm::X86::SBB8mi,
    llvm::X86::SBB8mr,
    llvm::X86::SBB8rm,
    llvm::X86::SCASB,
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
    llvm::X86::VPBROADCASTBYrm,
    llvm::X86::VPBROADCASTBrm,
    llvm::X86::VPINSRBrm,
    llvm::X86::XADD8rm,
    llvm::X86::XCHG8rm,
    llvm::X86::XLAT,
    llvm::X86::XOR8mi,
    llvm::X86::XOR8mi8,
    llvm::X86::XOR8mr,
    llvm::X86::XOR8rm,
    // clang-format on
};

constexpr size_t READ_8_SIZE = sizeof(READ_8) / sizeof(unsigned);

constexpr unsigned READ_16[] = {
    // clang-format off
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
    llvm::X86::ARPL16mr,
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
    llvm::X86::CALL16m_NT,
    llvm::X86::CMOV16rm,
    llvm::X86::CMP16mi,
    llvm::X86::CMP16mi8,
    llvm::X86::CMP16mr,
    llvm::X86::CMP16rm,
    llvm::X86::CMPSW,
    llvm::X86::CMPXCHG16rm,
    llvm::X86::CRC32r32m16,
    llvm::X86::DEC16m,
    llvm::X86::DIV16m,
    llvm::X86::DIVR_FI16m,
    llvm::X86::DIV_FI16m,
    llvm::X86::FICOM16m,
    llvm::X86::FICOMP16m,
    llvm::X86::FLDCW16m,
    llvm::X86::IDIV16m,
    llvm::X86::ILD_F16m,
    llvm::X86::IMUL16m,
    llvm::X86::IMUL16rm,
    llvm::X86::IMUL16rmi,
    llvm::X86::IMUL16rmi8,
    llvm::X86::INC16m,
    llvm::X86::JMP16m,
    llvm::X86::JMP16m_NT,
    llvm::X86::LAR16rm,
    llvm::X86::LAR32rm,
    llvm::X86::LAR64rm,
    llvm::X86::LCMPXCHG16,
    llvm::X86::LLDT16m,
    llvm::X86::LMSW16m,
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
    llvm::X86::LODSW,
    llvm::X86::LSL16rm,
    llvm::X86::LSL32rm,
    llvm::X86::LSL64rm,
    llvm::X86::LTRm,
    llvm::X86::LXADD16,
    llvm::X86::LZCNT16rm,
    llvm::X86::MMX_PINSRWrm,
    llvm::X86::MOV16ao16,
    llvm::X86::MOV16ao32,
    llvm::X86::MOV16ao64,
    llvm::X86::MOV16rm,
    llvm::X86::MOV16sm,
    llvm::X86::MOVBE16rm,
    llvm::X86::MOVSW,
    llvm::X86::MOVSX32rm16,
    llvm::X86::MOVSX64rm16,
    llvm::X86::MOVZX32rm16,
    llvm::X86::MOVZX64rm16,
    llvm::X86::MUL16m,
    llvm::X86::MUL_FI16m,
    llvm::X86::NEG16m,
    llvm::X86::NOT16m,
    llvm::X86::OR16mi,
    llvm::X86::OR16mi8,
    llvm::X86::OR16mr,
    llvm::X86::OR16rm,
    llvm::X86::PINSRWrm,
    llvm::X86::PMOVSXBQrm,
    llvm::X86::PMOVZXBQrm,
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
    llvm::X86::ROL16mi,
    llvm::X86::ROR16m1,
    llvm::X86::ROR16mCL,
    llvm::X86::ROR16mi,
    llvm::X86::SAR16m1,
    llvm::X86::SAR16mCL,
    llvm::X86::SAR16mi,
    llvm::X86::SBB16mi,
    llvm::X86::SBB16mi8,
    llvm::X86::SBB16mr,
    llvm::X86::SBB16rm,
    llvm::X86::SCASW,
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
    llvm::X86::VERRm,
    llvm::X86::VERWm,
    llvm::X86::VPBROADCASTWYrm,
    llvm::X86::VPBROADCASTWrm,
    llvm::X86::VPINSRWrm,
    llvm::X86::VPMOVSXBQrm,
    llvm::X86::VPMOVZXBQrm,
    llvm::X86::XADD16rm,
    llvm::X86::XCHG16rm,
    llvm::X86::XOR16mi,
    llvm::X86::XOR16mi8,
    llvm::X86::XOR16mr,
    llvm::X86::XOR16rm,
    // clang-format on
};

constexpr size_t READ_16_SIZE = sizeof(READ_16) / sizeof(unsigned);

constexpr unsigned READ_32[] = {
    // clang-format off
    llvm::X86::ADC32mi,
    llvm::X86::ADC32mi8,
    llvm::X86::ADC32mr,
    llvm::X86::ADC32rm,
    llvm::X86::ADCX32rm,
    llvm::X86::ADD32mi,
    llvm::X86::ADD32mi8,
    llvm::X86::ADD32mr,
    llvm::X86::ADD32rm,
    llvm::X86::ADDSSrm,
    llvm::X86::ADDSSrm_Int,
    llvm::X86::ADD_F32m,
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
    llvm::X86::BOUNDS16rm,
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
    llvm::X86::CALL32m_NT,
    llvm::X86::CMOV32rm,
    llvm::X86::CMP32mi,
    llvm::X86::CMP32mi8,
    llvm::X86::CMP32mr,
    llvm::X86::CMP32rm,
    llvm::X86::CMPSL,
    llvm::X86::CMPSSrm,
    llvm::X86::CMPSSrm_Int,
    llvm::X86::CMPXCHG32rm,
    llvm::X86::COMISSrm,
    llvm::X86::COMISSrm_Int,
    llvm::X86::CRC32r32m32,
    llvm::X86::CVTSI2SDrm,
    llvm::X86::CVTSI2SDrm_Int,
    llvm::X86::CVTSI2SSrm,
    llvm::X86::CVTSI2SSrm_Int,
    llvm::X86::CVTSS2SDrm,
    llvm::X86::CVTSS2SDrm_Int,
    llvm::X86::CVTSS2SI64rm_Int,
    llvm::X86::CVTSS2SIrm_Int,
    llvm::X86::CVTTSS2SI64rm,
    llvm::X86::CVTTSS2SI64rm_Int,
    llvm::X86::CVTTSS2SIrm,
    llvm::X86::CVTTSS2SIrm_Int,
    llvm::X86::DEC32m,
    llvm::X86::DIV32m,
    llvm::X86::DIVR_F32m,
    llvm::X86::DIVR_FI32m,
    llvm::X86::DIVSSrm,
    llvm::X86::DIVSSrm_Int,
    llvm::X86::DIV_F32m,
    llvm::X86::DIV_FI32m,
    llvm::X86::FCOM32m,
    llvm::X86::FCOMP32m,
    llvm::X86::FICOM32m,
    llvm::X86::FICOMP32m,
    llvm::X86::IDIV32m,
    llvm::X86::ILD_F32m,
    llvm::X86::IMUL32m,
    llvm::X86::IMUL32rm,
    llvm::X86::IMUL32rmi,
    llvm::X86::IMUL32rmi8,
    llvm::X86::INC32m,
    llvm::X86::INSERTPSrm,
    llvm::X86::JMP32m,
    llvm::X86::JMP32m_NT,
    llvm::X86::LCMPXCHG32,
    llvm::X86::LDMXCSR,
    llvm::X86::LD_F32m,
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
    llvm::X86::LODSL,
    llvm::X86::LXADD32,
    llvm::X86::LZCNT32rm,
    llvm::X86::MAXSSrm,
    llvm::X86::MAXSSrm_Int,
    llvm::X86::MINSSrm,
    llvm::X86::MINSSrm_Int,
    llvm::X86::MMX_MOVD64rm,
    llvm::X86::MMX_PUNPCKLBWirm,
    llvm::X86::MMX_PUNPCKLDQirm,
    llvm::X86::MMX_PUNPCKLWDirm,
    llvm::X86::MOV32ao16,
    llvm::X86::MOV32ao32,
    llvm::X86::MOV32ao64,
    llvm::X86::MOV32rm,
    llvm::X86::MOVBE32rm,
    llvm::X86::MOVDI2PDIrm,
    llvm::X86::MOVSL,
    llvm::X86::MOVSSrm,
    llvm::X86::MOVSSrm_alt,
    llvm::X86::MOVSX64rm32,
    llvm::X86::MUL32m,
    llvm::X86::MULSSrm,
    llvm::X86::MULSSrm_Int,
    llvm::X86::MULX32rm,
    llvm::X86::MUL_F32m,
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
    llvm::X86::PINSRDrm,
    llvm::X86::PMOVSXBDrm,
    llvm::X86::PMOVSXWQrm,
    llvm::X86::PMOVZXBDrm,
    llvm::X86::PMOVZXWQrm,
    llvm::X86::POPCNT32rm,
    llvm::X86::PTWRITEm,
    llvm::X86::PUSH32rmm,
    llvm::X86::RCL32m1,
    llvm::X86::RCL32mCL,
    llvm::X86::RCL32mi,
    llvm::X86::RCPSSm,
    llvm::X86::RCPSSm_Int,
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
    llvm::X86::ROUNDSSm,
    llvm::X86::ROUNDSSm_Int,
    llvm::X86::RSQRTSSm,
    llvm::X86::RSQRTSSm_Int,
    llvm::X86::SAR32m1,
    llvm::X86::SAR32mCL,
    llvm::X86::SAR32mi,
    llvm::X86::SARX32rm,
    llvm::X86::SBB32mi,
    llvm::X86::SBB32mi8,
    llvm::X86::SBB32mr,
    llvm::X86::SBB32rm,
    llvm::X86::SCASL,
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
    llvm::X86::SQRTSSm,
    llvm::X86::SQRTSSm_Int,
    llvm::X86::SUB32mi,
    llvm::X86::SUB32mi8,
    llvm::X86::SUB32mr,
    llvm::X86::SUB32rm,
    llvm::X86::SUBR_F32m,
    llvm::X86::SUBR_FI32m,
    llvm::X86::SUBSSrm,
    llvm::X86::SUBSSrm_Int,
    llvm::X86::SUB_F32m,
    llvm::X86::SUB_FI32m,
    llvm::X86::T1MSKC32rm,
    llvm::X86::TEST32mi,
    llvm::X86::TEST32mr,
    llvm::X86::TZCNT32rm,
    llvm::X86::TZMSK32rm,
    llvm::X86::UCOMISSrm,
    llvm::X86::UCOMISSrm_Int,
    llvm::X86::VADDSSrm,
    llvm::X86::VADDSSrm_Int,
    llvm::X86::VBROADCASTSSYrm,
    llvm::X86::VBROADCASTSSrm,
    llvm::X86::VCMPSSrm,
    llvm::X86::VCMPSSrm_Int,
    llvm::X86::VCOMISSrm,
    llvm::X86::VCOMISSrm_Int,
    llvm::X86::VCVTSI2SDrm,
    llvm::X86::VCVTSI2SDrm_Int,
    llvm::X86::VCVTSI2SSrm,
    llvm::X86::VCVTSI2SSrm_Int,
    llvm::X86::VCVTSS2SDrm,
    llvm::X86::VCVTSS2SDrm_Int,
    llvm::X86::VCVTSS2SI64rm_Int,
    llvm::X86::VCVTSS2SIrm_Int,
    llvm::X86::VCVTTSS2SI64rm,
    llvm::X86::VCVTTSS2SI64rm_Int,
    llvm::X86::VCVTTSS2SIrm,
    llvm::X86::VCVTTSS2SIrm_Int,
    llvm::X86::VDIVSSrm,
    llvm::X86::VDIVSSrm_Int,
    llvm::X86::VFMADD132SSm,
    llvm::X86::VFMADD132SSm_Int,
    llvm::X86::VFMADD213SSm,
    llvm::X86::VFMADD213SSm_Int,
    llvm::X86::VFMADD231SSm,
    llvm::X86::VFMADD231SSm_Int,
    llvm::X86::VFMADDSS4mr,
    llvm::X86::VFMADDSS4mr_Int,
    llvm::X86::VFMADDSS4rm,
    llvm::X86::VFMADDSS4rm_Int,
    llvm::X86::VFMSUB132SSm,
    llvm::X86::VFMSUB132SSm_Int,
    llvm::X86::VFMSUB213SSm,
    llvm::X86::VFMSUB213SSm_Int,
    llvm::X86::VFMSUB231SSm,
    llvm::X86::VFMSUB231SSm_Int,
    llvm::X86::VFMSUBSS4mr,
    llvm::X86::VFMSUBSS4mr_Int,
    llvm::X86::VFMSUBSS4rm,
    llvm::X86::VFMSUBSS4rm_Int,
    llvm::X86::VFNMADD132SSm,
    llvm::X86::VFNMADD132SSm_Int,
    llvm::X86::VFNMADD213SSm,
    llvm::X86::VFNMADD213SSm_Int,
    llvm::X86::VFNMADD231SSm,
    llvm::X86::VFNMADD231SSm_Int,
    llvm::X86::VFNMADDSS4mr,
    llvm::X86::VFNMADDSS4mr_Int,
    llvm::X86::VFNMADDSS4rm,
    llvm::X86::VFNMADDSS4rm_Int,
    llvm::X86::VFNMSUB132SSm,
    llvm::X86::VFNMSUB132SSm_Int,
    llvm::X86::VFNMSUB213SSm,
    llvm::X86::VFNMSUB213SSm_Int,
    llvm::X86::VFNMSUB231SSm,
    llvm::X86::VFNMSUB231SSm_Int,
    llvm::X86::VFNMSUBSS4mr,
    llvm::X86::VFNMSUBSS4mr_Int,
    llvm::X86::VFNMSUBSS4rm,
    llvm::X86::VFNMSUBSS4rm_Int,
    llvm::X86::VINSERTPSrm,
    llvm::X86::VLDMXCSR,
    llvm::X86::VMAXSSrm,
    llvm::X86::VMAXSSrm_Int,
    llvm::X86::VMINSSrm,
    llvm::X86::VMINSSrm_Int,
    llvm::X86::VMOVDI2PDIrm,
    llvm::X86::VMOVSSrm,
    llvm::X86::VMOVSSrm_alt,
    llvm::X86::VMULSSrm,
    llvm::X86::VMULSSrm_Int,
    llvm::X86::VPBROADCASTDYrm,
    llvm::X86::VPBROADCASTDrm,
    llvm::X86::VPINSRDrm,
    llvm::X86::VPMOVSXBDrm,
    llvm::X86::VPMOVSXBQYrm,
    llvm::X86::VPMOVSXWQrm,
    llvm::X86::VPMOVZXBDrm,
    llvm::X86::VPMOVZXBQYrm,
    llvm::X86::VPMOVZXWQrm,
    llvm::X86::VRCPSSm,
    llvm::X86::VRCPSSm_Int,
    llvm::X86::VROUNDSSm,
    llvm::X86::VROUNDSSm_Int,
    llvm::X86::VRSQRTSSm,
    llvm::X86::VRSQRTSSm_Int,
    llvm::X86::VSQRTSSm,
    llvm::X86::VSQRTSSm_Int,
    llvm::X86::VSUBSSrm,
    llvm::X86::VSUBSSrm_Int,
    llvm::X86::VUCOMISSrm,
    llvm::X86::VUCOMISSrm_Int,
    llvm::X86::XADD32rm,
    llvm::X86::XCHG32rm,
    llvm::X86::XOR32mi,
    llvm::X86::XOR32mi8,
    llvm::X86::XOR32mr,
    llvm::X86::XOR32rm,
    // clang-format on
};

constexpr size_t READ_32_SIZE = sizeof(READ_32) / sizeof(unsigned);

constexpr unsigned READ_64[] = {
    // clang-format off
    llvm::X86::ADC64mi32,
    llvm::X86::ADC64mi8,
    llvm::X86::ADC64mr,
    llvm::X86::ADC64rm,
    llvm::X86::ADCX64rm,
    llvm::X86::ADD64mi32,
    llvm::X86::ADD64mi8,
    llvm::X86::ADD64mr,
    llvm::X86::ADD64rm,
    llvm::X86::ADDSDrm,
    llvm::X86::ADDSDrm_Int,
    llvm::X86::ADD_F64m,
    llvm::X86::ADOX64rm,
    llvm::X86::AND64mi32,
    llvm::X86::AND64mi8,
    llvm::X86::AND64mr,
    llvm::X86::AND64rm,
    llvm::X86::ANDN64rm,
    llvm::X86::BEXTR64rm,
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
    llvm::X86::BNDMOV32rm,
    llvm::X86::BOUNDS32rm,
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
    llvm::X86::BZHI64rm,
    llvm::X86::CALL64m,
    llvm::X86::CALL64m_NT,
    llvm::X86::CMOV64rm,
    llvm::X86::CMP64mi32,
    llvm::X86::CMP64mi8,
    llvm::X86::CMP64mr,
    llvm::X86::CMP64rm,
    llvm::X86::CMPSDrm,
    llvm::X86::CMPSDrm_Int,
    llvm::X86::CMPSQ,
    llvm::X86::CMPXCHG64rm,
    llvm::X86::CMPXCHG8B,
    llvm::X86::COMISDrm,
    llvm::X86::COMISDrm_Int,
    llvm::X86::CRC32r64m64,
    llvm::X86::CVTDQ2PDrm,
    llvm::X86::CVTPS2PDrm,
    llvm::X86::CVTSD2SI64rm_Int,
    llvm::X86::CVTSD2SIrm_Int,
    llvm::X86::CVTSD2SSrm,
    llvm::X86::CVTSD2SSrm_Int,
    llvm::X86::CVTSI642SDrm,
    llvm::X86::CVTSI642SDrm_Int,
    llvm::X86::CVTSI642SSrm,
    llvm::X86::CVTSI642SSrm_Int,
    llvm::X86::CVTTSD2SI64rm,
    llvm::X86::CVTTSD2SI64rm_Int,
    llvm::X86::CVTTSD2SIrm,
    llvm::X86::CVTTSD2SIrm_Int,
    llvm::X86::DEC64m,
    llvm::X86::DIV64m,
    llvm::X86::DIVR_F64m,
    llvm::X86::DIVSDrm,
    llvm::X86::DIVSDrm_Int,
    llvm::X86::DIV_F64m,
    llvm::X86::FCOM64m,
    llvm::X86::FCOMP64m,
    llvm::X86::IDIV64m,
    llvm::X86::ILD_F64m,
    llvm::X86::IMUL64m,
    llvm::X86::IMUL64rm,
    llvm::X86::IMUL64rmi32,
    llvm::X86::IMUL64rmi8,
    llvm::X86::INC64m,
    llvm::X86::JMP64m,
    llvm::X86::JMP64m_NT,
    llvm::X86::JMP64m_REX,
    llvm::X86::LCMPXCHG64,
    llvm::X86::LCMPXCHG8B,
    llvm::X86::LD_F64m,
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
    llvm::X86::LODSQ,
    llvm::X86::LXADD64,
    llvm::X86::LZCNT64rm,
    llvm::X86::MAXSDrm,
    llvm::X86::MAXSDrm_Int,
    llvm::X86::MINSDrm,
    llvm::X86::MINSDrm_Int,
    llvm::X86::MMX_CVTPI2PDirm,
    llvm::X86::MMX_CVTPI2PSirm,
    llvm::X86::MMX_CVTPS2PIirm,
    llvm::X86::MMX_CVTTPS2PIirm,
    llvm::X86::MMX_MASKMOVQ,
    llvm::X86::MMX_MASKMOVQ64,
    llvm::X86::MMX_MOVQ64rm,
    llvm::X86::MMX_PABSBrm,
    llvm::X86::MMX_PABSDrm,
    llvm::X86::MMX_PABSWrm,
    llvm::X86::MMX_PACKSSDWirm,
    llvm::X86::MMX_PACKSSWBirm,
    llvm::X86::MMX_PACKUSWBirm,
    llvm::X86::MMX_PADDBirm,
    llvm::X86::MMX_PADDDirm,
    llvm::X86::MMX_PADDQirm,
    llvm::X86::MMX_PADDSBirm,
    llvm::X86::MMX_PADDSWirm,
    llvm::X86::MMX_PADDUSBirm,
    llvm::X86::MMX_PADDUSWirm,
    llvm::X86::MMX_PADDWirm,
    llvm::X86::MMX_PALIGNRrmi,
    llvm::X86::MMX_PANDNirm,
    llvm::X86::MMX_PANDirm,
    llvm::X86::MMX_PAVGBirm,
    llvm::X86::MMX_PAVGWirm,
    llvm::X86::MMX_PCMPEQBirm,
    llvm::X86::MMX_PCMPEQDirm,
    llvm::X86::MMX_PCMPEQWirm,
    llvm::X86::MMX_PCMPGTBirm,
    llvm::X86::MMX_PCMPGTDirm,
    llvm::X86::MMX_PCMPGTWirm,
    llvm::X86::MMX_PHADDDrm,
    llvm::X86::MMX_PHADDSWrm,
    llvm::X86::MMX_PHADDWrm,
    llvm::X86::MMX_PHSUBDrm,
    llvm::X86::MMX_PHSUBSWrm,
    llvm::X86::MMX_PHSUBWrm,
    llvm::X86::MMX_PMADDUBSWrm,
    llvm::X86::MMX_PMADDWDirm,
    llvm::X86::MMX_PMAXSWirm,
    llvm::X86::MMX_PMAXUBirm,
    llvm::X86::MMX_PMINSWirm,
    llvm::X86::MMX_PMINUBirm,
    llvm::X86::MMX_PMULHRSWrm,
    llvm::X86::MMX_PMULHUWirm,
    llvm::X86::MMX_PMULHWirm,
    llvm::X86::MMX_PMULLWirm,
    llvm::X86::MMX_PMULUDQirm,
    llvm::X86::MMX_PORirm,
    llvm::X86::MMX_PSADBWirm,
    llvm::X86::MMX_PSHUFBrm,
    llvm::X86::MMX_PSHUFWmi,
    llvm::X86::MMX_PSIGNBrm,
    llvm::X86::MMX_PSIGNDrm,
    llvm::X86::MMX_PSIGNWrm,
    llvm::X86::MMX_PSLLDrm,
    llvm::X86::MMX_PSLLQrm,
    llvm::X86::MMX_PSLLWrm,
    llvm::X86::MMX_PSRADrm,
    llvm::X86::MMX_PSRAWrm,
    llvm::X86::MMX_PSRLDrm,
    llvm::X86::MMX_PSRLQrm,
    llvm::X86::MMX_PSRLWrm,
    llvm::X86::MMX_PSUBBirm,
    llvm::X86::MMX_PSUBDirm,
    llvm::X86::MMX_PSUBQirm,
    llvm::X86::MMX_PSUBSBirm,
    llvm::X86::MMX_PSUBSWirm,
    llvm::X86::MMX_PSUBUSBirm,
    llvm::X86::MMX_PSUBUSWirm,
    llvm::X86::MMX_PSUBWirm,
    llvm::X86::MMX_PUNPCKHBWirm,
    llvm::X86::MMX_PUNPCKHDQirm,
    llvm::X86::MMX_PUNPCKHWDirm,
    llvm::X86::MMX_PXORirm,
    llvm::X86::MOV64ao32,
    llvm::X86::MOV64ao64,
    llvm::X86::MOV64rm,
    llvm::X86::MOVBE64rm,
    llvm::X86::MOVDDUPrm,
    llvm::X86::MOVHPDrm,
    llvm::X86::MOVHPSrm,
    llvm::X86::MOVLPDrm,
    llvm::X86::MOVLPSrm,
    llvm::X86::MOVQI2PQIrm,
    llvm::X86::MOVSDrm,
    llvm::X86::MOVSDrm_alt,
    llvm::X86::MOVSQ,
    llvm::X86::MUL64m,
    llvm::X86::MULSDrm,
    llvm::X86::MULSDrm_Int,
    llvm::X86::MULX64rm,
    llvm::X86::MUL_F64m,
    llvm::X86::NEG64m,
    llvm::X86::NOT64m,
    llvm::X86::OR64mi32,
    llvm::X86::OR64mi8,
    llvm::X86::OR64mr,
    llvm::X86::OR64rm,
    llvm::X86::PAVGUSBrm,
    llvm::X86::PDEP64rm,
    llvm::X86::PEXT64rm,
    llvm::X86::PF2IDrm,
    llvm::X86::PF2IWrm,
    llvm::X86::PFACCrm,
    llvm::X86::PFADDrm,
    llvm::X86::PFCMPEQrm,
    llvm::X86::PFCMPGErm,
    llvm::X86::PFCMPGTrm,
    llvm::X86::PFMAXrm,
    llvm::X86::PFMINrm,
    llvm::X86::PFMULrm,
    llvm::X86::PFNACCrm,
    llvm::X86::PFPNACCrm,
    llvm::X86::PFRCPIT1rm,
    llvm::X86::PFRCPIT2rm,
    llvm::X86::PFRCPrm,
    llvm::X86::PFRSQIT1rm,
    llvm::X86::PFRSQRTrm,
    llvm::X86::PFSUBRrm,
    llvm::X86::PFSUBrm,
    llvm::X86::PI2FDrm,
    llvm::X86::PI2FWrm,
    llvm::X86::PINSRQrm,
    llvm::X86::PMOVSXBWrm,
    llvm::X86::PMOVSXDQrm,
    llvm::X86::PMOVSXWDrm,
    llvm::X86::PMOVZXBWrm,
    llvm::X86::PMOVZXDQrm,
    llvm::X86::PMOVZXWDrm,
    llvm::X86::PMULHRWrm,
    llvm::X86::POPCNT64rm,
    llvm::X86::PSWAPDrm,
    llvm::X86::PTWRITE64m,
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
    llvm::X86::ROUNDSDm,
    llvm::X86::ROUNDSDm_Int,
    llvm::X86::SAR64m1,
    llvm::X86::SAR64mCL,
    llvm::X86::SAR64mi,
    llvm::X86::SARX64rm,
    llvm::X86::SBB64mi32,
    llvm::X86::SBB64mi8,
    llvm::X86::SBB64mr,
    llvm::X86::SBB64rm,
    llvm::X86::SCASQ,
    llvm::X86::SHL64m1,
    llvm::X86::SHL64mCL,
    llvm::X86::SHL64mi,
    llvm::X86::SHLD64mrCL,
    llvm::X86::SHLD64mri8,
    llvm::X86::SHLX64rm,
    llvm::X86::SHR64m1,
    llvm::X86::SHR64mCL,
    llvm::X86::SHR64mi,
    llvm::X86::SHRD64mrCL,
    llvm::X86::SHRD64mri8,
    llvm::X86::SHRX64rm,
    llvm::X86::SQRTSDm,
    llvm::X86::SQRTSDm_Int,
    llvm::X86::SUB64mi32,
    llvm::X86::SUB64mi8,
    llvm::X86::SUB64mr,
    llvm::X86::SUB64rm,
    llvm::X86::SUBR_F64m,
    llvm::X86::SUBSDrm,
    llvm::X86::SUBSDrm_Int,
    llvm::X86::SUB_F64m,
    llvm::X86::T1MSKC64rm,
    llvm::X86::TEST64mi32,
    llvm::X86::TEST64mr,
    llvm::X86::TZCNT64rm,
    llvm::X86::TZMSK64rm,
    llvm::X86::UCOMISDrm,
    llvm::X86::UCOMISDrm_Int,
    llvm::X86::VADDSDrm,
    llvm::X86::VADDSDrm_Int,
    llvm::X86::VBROADCASTSDYrm,
    llvm::X86::VCMPSDrm,
    llvm::X86::VCMPSDrm_Int,
    llvm::X86::VCOMISDrm,
    llvm::X86::VCOMISDrm_Int,
    llvm::X86::VCVTDQ2PDrm,
    llvm::X86::VCVTPH2PSrm,
    llvm::X86::VCVTPS2PDrm,
    llvm::X86::VCVTSD2SI64rm_Int,
    llvm::X86::VCVTSD2SIrm_Int,
    llvm::X86::VCVTSD2SSrm,
    llvm::X86::VCVTSD2SSrm_Int,
    llvm::X86::VCVTSI642SDrm,
    llvm::X86::VCVTSI642SDrm_Int,
    llvm::X86::VCVTSI642SSrm,
    llvm::X86::VCVTSI642SSrm_Int,
    llvm::X86::VCVTTSD2SI64rm,
    llvm::X86::VCVTTSD2SI64rm_Int,
    llvm::X86::VCVTTSD2SIrm,
    llvm::X86::VCVTTSD2SIrm_Int,
    llvm::X86::VDIVSDrm,
    llvm::X86::VDIVSDrm_Int,
    llvm::X86::VFMADD132SDm,
    llvm::X86::VFMADD132SDm_Int,
    llvm::X86::VFMADD213SDm,
    llvm::X86::VFMADD213SDm_Int,
    llvm::X86::VFMADD231SDm,
    llvm::X86::VFMADD231SDm_Int,
    llvm::X86::VFMADDSD4mr,
    llvm::X86::VFMADDSD4mr_Int,
    llvm::X86::VFMADDSD4rm,
    llvm::X86::VFMADDSD4rm_Int,
    llvm::X86::VFMSUB132SDm,
    llvm::X86::VFMSUB132SDm_Int,
    llvm::X86::VFMSUB213SDm,
    llvm::X86::VFMSUB213SDm_Int,
    llvm::X86::VFMSUB231SDm,
    llvm::X86::VFMSUB231SDm_Int,
    llvm::X86::VFMSUBSD4mr,
    llvm::X86::VFMSUBSD4mr_Int,
    llvm::X86::VFMSUBSD4rm,
    llvm::X86::VFMSUBSD4rm_Int,
    llvm::X86::VFNMADD132SDm,
    llvm::X86::VFNMADD132SDm_Int,
    llvm::X86::VFNMADD213SDm,
    llvm::X86::VFNMADD213SDm_Int,
    llvm::X86::VFNMADD231SDm,
    llvm::X86::VFNMADD231SDm_Int,
    llvm::X86::VFNMADDSD4mr,
    llvm::X86::VFNMADDSD4mr_Int,
    llvm::X86::VFNMADDSD4rm,
    llvm::X86::VFNMADDSD4rm_Int,
    llvm::X86::VFNMSUB132SDm,
    llvm::X86::VFNMSUB132SDm_Int,
    llvm::X86::VFNMSUB213SDm,
    llvm::X86::VFNMSUB213SDm_Int,
    llvm::X86::VFNMSUB231SDm,
    llvm::X86::VFNMSUB231SDm_Int,
    llvm::X86::VFNMSUBSD4mr,
    llvm::X86::VFNMSUBSD4mr_Int,
    llvm::X86::VFNMSUBSD4rm,
    llvm::X86::VFNMSUBSD4rm_Int,
    llvm::X86::VMAXSDrm,
    llvm::X86::VMAXSDrm_Int,
    llvm::X86::VMINSDrm,
    llvm::X86::VMINSDrm_Int,
    llvm::X86::VMOVDDUPrm,
    llvm::X86::VMOVHPDrm,
    llvm::X86::VMOVHPSrm,
    llvm::X86::VMOVLPDrm,
    llvm::X86::VMOVLPSrm,
    llvm::X86::VMOVQI2PQIrm,
    llvm::X86::VMOVSDrm,
    llvm::X86::VMOVSDrm_alt,
    llvm::X86::VMULSDrm,
    llvm::X86::VMULSDrm_Int,
    llvm::X86::VPBROADCASTQYrm,
    llvm::X86::VPBROADCASTQrm,
    llvm::X86::VPINSRQrm,
    llvm::X86::VPMOVSXBDYrm,
    llvm::X86::VPMOVSXBWrm,
    llvm::X86::VPMOVSXDQrm,
    llvm::X86::VPMOVSXWDrm,
    llvm::X86::VPMOVSXWQYrm,
    llvm::X86::VPMOVZXBDYrm,
    llvm::X86::VPMOVZXBWrm,
    llvm::X86::VPMOVZXDQrm,
    llvm::X86::VPMOVZXWDrm,
    llvm::X86::VPMOVZXWQYrm,
    llvm::X86::VROUNDSDm,
    llvm::X86::VROUNDSDm_Int,
    llvm::X86::VSQRTSDm,
    llvm::X86::VSQRTSDm_Int,
    llvm::X86::VSUBSDrm,
    llvm::X86::VSUBSDrm_Int,
    llvm::X86::VUCOMISDrm,
    llvm::X86::VUCOMISDrm_Int,
    llvm::X86::XADD64rm,
    llvm::X86::XCHG64rm,
    llvm::X86::XOR64mi32,
    llvm::X86::XOR64mi8,
    llvm::X86::XOR64mr,
    llvm::X86::XOR64rm,
    // clang-format on
};

constexpr size_t READ_64_SIZE = sizeof(READ_64) / sizeof(unsigned);

constexpr unsigned READ_80[] = {
    // clang-format off
    llvm::X86::FBLDm,
    llvm::X86::LD_F80m,
    // clang-format on
};

constexpr size_t READ_80_SIZE = sizeof(READ_80) / sizeof(unsigned);

constexpr unsigned READ_128[] = {
    // clang-format off
    llvm::X86::ADDPDrm,
    llvm::X86::ADDPSrm,
    llvm::X86::ADDSUBPDrm,
    llvm::X86::ADDSUBPSrm,
    llvm::X86::AESDECLASTrm,
    llvm::X86::AESDECrm,
    llvm::X86::AESENCLASTrm,
    llvm::X86::AESENCrm,
    llvm::X86::AESIMCrm,
    llvm::X86::AESKEYGENASSIST128rm,
    llvm::X86::ANDNPDrm,
    llvm::X86::ANDNPSrm,
    llvm::X86::ANDPDrm,
    llvm::X86::ANDPSrm,
    llvm::X86::BLENDPDrmi,
    llvm::X86::BLENDPSrmi,
    llvm::X86::BLENDVPDrm0,
    llvm::X86::BLENDVPSrm0,
    llvm::X86::BNDMOV64rm,
    llvm::X86::CMPPDrmi,
    llvm::X86::CMPPSrmi,
    llvm::X86::CMPXCHG16B,
    llvm::X86::CVTDQ2PSrm,
    llvm::X86::CVTPD2DQrm,
    llvm::X86::CVTPD2PSrm,
    llvm::X86::CVTPS2DQrm,
    llvm::X86::CVTTPD2DQrm,
    llvm::X86::CVTTPS2DQrm,
    llvm::X86::DIVPDrm,
    llvm::X86::DIVPSrm,
    llvm::X86::DPPDrmi,
    llvm::X86::DPPSrmi,
    llvm::X86::GF2P8AFFINEINVQBrmi,
    llvm::X86::GF2P8AFFINEQBrmi,
    llvm::X86::GF2P8MULBrm,
    llvm::X86::HADDPDrm,
    llvm::X86::HADDPSrm,
    llvm::X86::HSUBPDrm,
    llvm::X86::HSUBPSrm,
    llvm::X86::LCMPXCHG16B,
    llvm::X86::LDDQUrm,
    llvm::X86::MASKMOVDQU,
    llvm::X86::MASKMOVDQU64,
    llvm::X86::MAXPDrm,
    llvm::X86::MAXPSrm,
    llvm::X86::MINPDrm,
    llvm::X86::MINPSrm,
    llvm::X86::MMX_CVTPD2PIirm,
    llvm::X86::MMX_CVTTPD2PIirm,
    llvm::X86::MOVAPDrm,
    llvm::X86::MOVAPSrm,
    llvm::X86::MOVDQArm,
    llvm::X86::MOVDQUrm,
    llvm::X86::MOVNTDQArm,
    llvm::X86::MOVSHDUPrm,
    llvm::X86::MOVSLDUPrm,
    llvm::X86::MOVUPDrm,
    llvm::X86::MOVUPSrm,
    llvm::X86::MPSADBWrmi,
    llvm::X86::MULPDrm,
    llvm::X86::MULPSrm,
    llvm::X86::ORPDrm,
    llvm::X86::ORPSrm,
    llvm::X86::PABSBrm,
    llvm::X86::PABSDrm,
    llvm::X86::PABSWrm,
    llvm::X86::PACKSSDWrm,
    llvm::X86::PACKSSWBrm,
    llvm::X86::PACKUSDWrm,
    llvm::X86::PACKUSWBrm,
    llvm::X86::PADDBrm,
    llvm::X86::PADDDrm,
    llvm::X86::PADDQrm,
    llvm::X86::PADDSBrm,
    llvm::X86::PADDSWrm,
    llvm::X86::PADDUSBrm,
    llvm::X86::PADDUSWrm,
    llvm::X86::PADDWrm,
    llvm::X86::PALIGNRrmi,
    llvm::X86::PANDNrm,
    llvm::X86::PANDrm,
    llvm::X86::PAVGBrm,
    llvm::X86::PAVGWrm,
    llvm::X86::PBLENDVBrm0,
    llvm::X86::PBLENDWrmi,
    llvm::X86::PCLMULQDQrm,
    llvm::X86::PCMPEQBrm,
    llvm::X86::PCMPEQDrm,
    llvm::X86::PCMPEQQrm,
    llvm::X86::PCMPEQWrm,
    llvm::X86::PCMPESTRIrm,
    llvm::X86::PCMPESTRMrm,
    llvm::X86::PCMPGTBrm,
    llvm::X86::PCMPGTDrm,
    llvm::X86::PCMPGTQrm,
    llvm::X86::PCMPGTWrm,
    llvm::X86::PCMPISTRIrm,
    llvm::X86::PCMPISTRMrm,
    llvm::X86::PHADDDrm,
    llvm::X86::PHADDSWrm,
    llvm::X86::PHADDWrm,
    llvm::X86::PHMINPOSUWrm,
    llvm::X86::PHSUBDrm,
    llvm::X86::PHSUBSWrm,
    llvm::X86::PHSUBWrm,
    llvm::X86::PMADDUBSWrm,
    llvm::X86::PMADDWDrm,
    llvm::X86::PMAXSBrm,
    llvm::X86::PMAXSDrm,
    llvm::X86::PMAXSWrm,
    llvm::X86::PMAXUBrm,
    llvm::X86::PMAXUDrm,
    llvm::X86::PMAXUWrm,
    llvm::X86::PMINSBrm,
    llvm::X86::PMINSDrm,
    llvm::X86::PMINSWrm,
    llvm::X86::PMINUBrm,
    llvm::X86::PMINUDrm,
    llvm::X86::PMINUWrm,
    llvm::X86::PMULDQrm,
    llvm::X86::PMULHRSWrm,
    llvm::X86::PMULHUWrm,
    llvm::X86::PMULHWrm,
    llvm::X86::PMULLDrm,
    llvm::X86::PMULLWrm,
    llvm::X86::PMULUDQrm,
    llvm::X86::PORrm,
    llvm::X86::PSADBWrm,
    llvm::X86::PSHUFBrm,
    llvm::X86::PSHUFDmi,
    llvm::X86::PSHUFHWmi,
    llvm::X86::PSHUFLWmi,
    llvm::X86::PSIGNBrm,
    llvm::X86::PSIGNDrm,
    llvm::X86::PSIGNWrm,
    llvm::X86::PSLLDrm,
    llvm::X86::PSLLQrm,
    llvm::X86::PSLLWrm,
    llvm::X86::PSRADrm,
    llvm::X86::PSRAWrm,
    llvm::X86::PSRLDrm,
    llvm::X86::PSRLQrm,
    llvm::X86::PSRLWrm,
    llvm::X86::PSUBBrm,
    llvm::X86::PSUBDrm,
    llvm::X86::PSUBQrm,
    llvm::X86::PSUBSBrm,
    llvm::X86::PSUBSWrm,
    llvm::X86::PSUBUSBrm,
    llvm::X86::PSUBUSWrm,
    llvm::X86::PSUBWrm,
    llvm::X86::PTESTrm,
    llvm::X86::PUNPCKHBWrm,
    llvm::X86::PUNPCKHDQrm,
    llvm::X86::PUNPCKHQDQrm,
    llvm::X86::PUNPCKHWDrm,
    llvm::X86::PUNPCKLBWrm,
    llvm::X86::PUNPCKLDQrm,
    llvm::X86::PUNPCKLQDQrm,
    llvm::X86::PUNPCKLWDrm,
    llvm::X86::PXORrm,
    llvm::X86::RCPPSm,
    llvm::X86::ROUNDPDm,
    llvm::X86::ROUNDPSm,
    llvm::X86::RSQRTPSm,
    llvm::X86::SHA1MSG1rm,
    llvm::X86::SHA1MSG2rm,
    llvm::X86::SHA1NEXTErm,
    llvm::X86::SHA1RNDS4rmi,
    llvm::X86::SHA256MSG1rm,
    llvm::X86::SHA256MSG2rm,
    llvm::X86::SHA256RNDS2rm,
    llvm::X86::SHUFPDrmi,
    llvm::X86::SHUFPSrmi,
    llvm::X86::SQRTPDm,
    llvm::X86::SQRTPSm,
    llvm::X86::SUBPDrm,
    llvm::X86::SUBPSrm,
    llvm::X86::UNPCKHPDrm,
    llvm::X86::UNPCKHPSrm,
    llvm::X86::UNPCKLPDrm,
    llvm::X86::UNPCKLPSrm,
    llvm::X86::VADDPDrm,
    llvm::X86::VADDPSrm,
    llvm::X86::VADDSUBPDrm,
    llvm::X86::VADDSUBPSrm,
    llvm::X86::VAESDECLASTrm,
    llvm::X86::VAESDECrm,
    llvm::X86::VAESENCLASTrm,
    llvm::X86::VAESENCrm,
    llvm::X86::VAESIMCrm,
    llvm::X86::VAESKEYGENASSIST128rm,
    llvm::X86::VANDNPDrm,
    llvm::X86::VANDNPSrm,
    llvm::X86::VANDPDrm,
    llvm::X86::VANDPSrm,
    llvm::X86::VBLENDPDrmi,
    llvm::X86::VBLENDPSrmi,
    llvm::X86::VBLENDVPDrm,
    llvm::X86::VBLENDVPSrm,
    llvm::X86::VBROADCASTF128,
    llvm::X86::VBROADCASTI128,
    llvm::X86::VCMPPDrmi,
    llvm::X86::VCMPPSrmi,
    llvm::X86::VCVTDQ2PDYrm,
    llvm::X86::VCVTDQ2PSrm,
    llvm::X86::VCVTPD2DQrm,
    llvm::X86::VCVTPD2PSrm,
    llvm::X86::VCVTPH2PSYrm,
    llvm::X86::VCVTPS2DQrm,
    llvm::X86::VCVTPS2PDYrm,
    llvm::X86::VCVTTPD2DQrm,
    llvm::X86::VCVTTPS2DQrm,
    llvm::X86::VDIVPDrm,
    llvm::X86::VDIVPSrm,
    llvm::X86::VDPPDrmi,
    llvm::X86::VDPPSrmi,
    llvm::X86::VFMADD132PDm,
    llvm::X86::VFMADD132PSm,
    llvm::X86::VFMADD213PDm,
    llvm::X86::VFMADD213PSm,
    llvm::X86::VFMADD231PDm,
    llvm::X86::VFMADD231PSm,
    llvm::X86::VFMADDPD4mr,
    llvm::X86::VFMADDPD4rm,
    llvm::X86::VFMADDPS4mr,
    llvm::X86::VFMADDPS4rm,
    llvm::X86::VFMADDSUB132PDm,
    llvm::X86::VFMADDSUB132PSm,
    llvm::X86::VFMADDSUB213PDm,
    llvm::X86::VFMADDSUB213PSm,
    llvm::X86::VFMADDSUB231PDm,
    llvm::X86::VFMADDSUB231PSm,
    llvm::X86::VFMADDSUBPD4mr,
    llvm::X86::VFMADDSUBPD4rm,
    llvm::X86::VFMADDSUBPS4mr,
    llvm::X86::VFMADDSUBPS4rm,
    llvm::X86::VFMSUB132PDm,
    llvm::X86::VFMSUB132PSm,
    llvm::X86::VFMSUB213PDm,
    llvm::X86::VFMSUB213PSm,
    llvm::X86::VFMSUB231PDm,
    llvm::X86::VFMSUB231PSm,
    llvm::X86::VFMSUBADD132PDm,
    llvm::X86::VFMSUBADD132PSm,
    llvm::X86::VFMSUBADD213PDm,
    llvm::X86::VFMSUBADD213PSm,
    llvm::X86::VFMSUBADD231PDm,
    llvm::X86::VFMSUBADD231PSm,
    llvm::X86::VFMSUBADDPD4mr,
    llvm::X86::VFMSUBADDPD4rm,
    llvm::X86::VFMSUBADDPS4mr,
    llvm::X86::VFMSUBADDPS4rm,
    llvm::X86::VFMSUBPD4mr,
    llvm::X86::VFMSUBPD4rm,
    llvm::X86::VFMSUBPS4mr,
    llvm::X86::VFMSUBPS4rm,
    llvm::X86::VFNMADD132PDm,
    llvm::X86::VFNMADD132PSm,
    llvm::X86::VFNMADD213PDm,
    llvm::X86::VFNMADD213PSm,
    llvm::X86::VFNMADD231PDm,
    llvm::X86::VFNMADD231PSm,
    llvm::X86::VFNMADDPD4mr,
    llvm::X86::VFNMADDPD4rm,
    llvm::X86::VFNMADDPS4mr,
    llvm::X86::VFNMADDPS4rm,
    llvm::X86::VFNMSUB132PDm,
    llvm::X86::VFNMSUB132PSm,
    llvm::X86::VFNMSUB213PDm,
    llvm::X86::VFNMSUB213PSm,
    llvm::X86::VFNMSUB231PDm,
    llvm::X86::VFNMSUB231PSm,
    llvm::X86::VFNMSUBPD4mr,
    llvm::X86::VFNMSUBPD4rm,
    llvm::X86::VFNMSUBPS4mr,
    llvm::X86::VFNMSUBPS4rm,
    llvm::X86::VGF2P8AFFINEINVQBrmi,
    llvm::X86::VGF2P8AFFINEQBrmi,
    llvm::X86::VGF2P8MULBrm,
    llvm::X86::VHADDPDrm,
    llvm::X86::VHADDPSrm,
    llvm::X86::VHSUBPDrm,
    llvm::X86::VHSUBPSrm,
    llvm::X86::VINSERTF128rm,
    llvm::X86::VINSERTI128rm,
    llvm::X86::VLDDQUrm,
    llvm::X86::VMASKMOVDQU,
    llvm::X86::VMASKMOVDQU64,
    llvm::X86::VMASKMOVPDmr,
    llvm::X86::VMASKMOVPDrm,
    llvm::X86::VMASKMOVPSmr,
    llvm::X86::VMASKMOVPSrm,
    llvm::X86::VMAXPDrm,
    llvm::X86::VMAXPSrm,
    llvm::X86::VMINPDrm,
    llvm::X86::VMINPSrm,
    llvm::X86::VMOVAPDrm,
    llvm::X86::VMOVAPSrm,
    llvm::X86::VMOVDQArm,
    llvm::X86::VMOVDQUrm,
    llvm::X86::VMOVNTDQArm,
    llvm::X86::VMOVSHDUPrm,
    llvm::X86::VMOVSLDUPrm,
    llvm::X86::VMOVUPDrm,
    llvm::X86::VMOVUPSrm,
    llvm::X86::VMPSADBWrmi,
    llvm::X86::VMULPDrm,
    llvm::X86::VMULPSrm,
    llvm::X86::VORPDrm,
    llvm::X86::VORPSrm,
    llvm::X86::VPABSBrm,
    llvm::X86::VPABSDrm,
    llvm::X86::VPABSWrm,
    llvm::X86::VPACKSSDWrm,
    llvm::X86::VPACKSSWBrm,
    llvm::X86::VPACKUSDWrm,
    llvm::X86::VPACKUSWBrm,
    llvm::X86::VPADDBrm,
    llvm::X86::VPADDDrm,
    llvm::X86::VPADDQrm,
    llvm::X86::VPADDSBrm,
    llvm::X86::VPADDSWrm,
    llvm::X86::VPADDUSBrm,
    llvm::X86::VPADDUSWrm,
    llvm::X86::VPADDWrm,
    llvm::X86::VPALIGNRrmi,
    llvm::X86::VPANDNrm,
    llvm::X86::VPANDrm,
    llvm::X86::VPAVGBrm,
    llvm::X86::VPAVGWrm,
    llvm::X86::VPBLENDDrmi,
    llvm::X86::VPBLENDVBrm,
    llvm::X86::VPBLENDWrmi,
    llvm::X86::VPCLMULQDQrm,
    llvm::X86::VPCMPEQBrm,
    llvm::X86::VPCMPEQDrm,
    llvm::X86::VPCMPEQQrm,
    llvm::X86::VPCMPEQWrm,
    llvm::X86::VPCMPESTRIrm,
    llvm::X86::VPCMPESTRMrm,
    llvm::X86::VPCMPGTBrm,
    llvm::X86::VPCMPGTDrm,
    llvm::X86::VPCMPGTQrm,
    llvm::X86::VPCMPGTWrm,
    llvm::X86::VPCMPISTRIrm,
    llvm::X86::VPCMPISTRMrm,
    llvm::X86::VPERMIL2PDmr,
    llvm::X86::VPERMIL2PDrm,
    llvm::X86::VPERMIL2PSmr,
    llvm::X86::VPERMIL2PSrm,
    llvm::X86::VPERMILPDmi,
    llvm::X86::VPERMILPDrm,
    llvm::X86::VPERMILPSmi,
    llvm::X86::VPERMILPSrm,
    llvm::X86::VPHADDDrm,
    llvm::X86::VPHADDSWrm,
    llvm::X86::VPHADDWrm,
    llvm::X86::VPHMINPOSUWrm,
    llvm::X86::VPHSUBDrm,
    llvm::X86::VPHSUBSWrm,
    llvm::X86::VPHSUBWrm,
    llvm::X86::VPMADDUBSWrm,
    llvm::X86::VPMADDWDrm,
    llvm::X86::VPMASKMOVDmr,
    llvm::X86::VPMASKMOVDrm,
    llvm::X86::VPMASKMOVQmr,
    llvm::X86::VPMASKMOVQrm,
    llvm::X86::VPMAXSBrm,
    llvm::X86::VPMAXSDrm,
    llvm::X86::VPMAXSWrm,
    llvm::X86::VPMAXUBrm,
    llvm::X86::VPMAXUDrm,
    llvm::X86::VPMAXUWrm,
    llvm::X86::VPMINSBrm,
    llvm::X86::VPMINSDrm,
    llvm::X86::VPMINSWrm,
    llvm::X86::VPMINUBrm,
    llvm::X86::VPMINUDrm,
    llvm::X86::VPMINUWrm,
    llvm::X86::VPMOVSXBWYrm,
    llvm::X86::VPMOVSXDQYrm,
    llvm::X86::VPMOVSXWDYrm,
    llvm::X86::VPMOVZXBWYrm,
    llvm::X86::VPMOVZXDQYrm,
    llvm::X86::VPMOVZXWDYrm,
    llvm::X86::VPMULDQrm,
    llvm::X86::VPMULHRSWrm,
    llvm::X86::VPMULHUWrm,
    llvm::X86::VPMULHWrm,
    llvm::X86::VPMULLDrm,
    llvm::X86::VPMULLWrm,
    llvm::X86::VPMULUDQrm,
    llvm::X86::VPORrm,
    llvm::X86::VPSADBWrm,
    llvm::X86::VPSHUFBrm,
    llvm::X86::VPSHUFDmi,
    llvm::X86::VPSHUFHWmi,
    llvm::X86::VPSHUFLWmi,
    llvm::X86::VPSIGNBrm,
    llvm::X86::VPSIGNDrm,
    llvm::X86::VPSIGNWrm,
    llvm::X86::VPSLLDYrm,
    llvm::X86::VPSLLDrm,
    llvm::X86::VPSLLQYrm,
    llvm::X86::VPSLLQrm,
    llvm::X86::VPSLLVDrm,
    llvm::X86::VPSLLVQrm,
    llvm::X86::VPSLLWYrm,
    llvm::X86::VPSLLWrm,
    llvm::X86::VPSRADYrm,
    llvm::X86::VPSRADrm,
    llvm::X86::VPSRAVDrm,
    llvm::X86::VPSRAWYrm,
    llvm::X86::VPSRAWrm,
    llvm::X86::VPSRLDYrm,
    llvm::X86::VPSRLDrm,
    llvm::X86::VPSRLQYrm,
    llvm::X86::VPSRLQrm,
    llvm::X86::VPSRLVDrm,
    llvm::X86::VPSRLVQrm,
    llvm::X86::VPSRLWYrm,
    llvm::X86::VPSRLWrm,
    llvm::X86::VPSUBBrm,
    llvm::X86::VPSUBDrm,
    llvm::X86::VPSUBQrm,
    llvm::X86::VPSUBSBrm,
    llvm::X86::VPSUBSWrm,
    llvm::X86::VPSUBUSBrm,
    llvm::X86::VPSUBUSWrm,
    llvm::X86::VPSUBWrm,
    llvm::X86::VPTESTrm,
    llvm::X86::VPUNPCKHBWrm,
    llvm::X86::VPUNPCKHDQrm,
    llvm::X86::VPUNPCKHQDQrm,
    llvm::X86::VPUNPCKHWDrm,
    llvm::X86::VPUNPCKLBWrm,
    llvm::X86::VPUNPCKLDQrm,
    llvm::X86::VPUNPCKLQDQrm,
    llvm::X86::VPUNPCKLWDrm,
    llvm::X86::VPXORrm,
    llvm::X86::VRCPPSm,
    llvm::X86::VROUNDPDm,
    llvm::X86::VROUNDPSm,
    llvm::X86::VRSQRTPSm,
    llvm::X86::VSHUFPDrmi,
    llvm::X86::VSHUFPSrmi,
    llvm::X86::VSQRTPDm,
    llvm::X86::VSQRTPSm,
    llvm::X86::VSUBPDrm,
    llvm::X86::VSUBPSrm,
    llvm::X86::VTESTPDrm,
    llvm::X86::VTESTPSrm,
    llvm::X86::VUNPCKHPDrm,
    llvm::X86::VUNPCKHPSrm,
    llvm::X86::VUNPCKLPDrm,
    llvm::X86::VUNPCKLPSrm,
    llvm::X86::VXORPDrm,
    llvm::X86::VXORPSrm,
    llvm::X86::XORPDrm,
    llvm::X86::XORPSrm,
    // clang-format on
};

constexpr size_t READ_128_SIZE = sizeof(READ_128) / sizeof(unsigned);

constexpr unsigned READ_224[] = {
    // clang-format off
    llvm::X86::FLDENVm,
    // clang-format on
};

constexpr size_t READ_224_SIZE = sizeof(READ_224) / sizeof(unsigned);

constexpr unsigned READ_256[] = {
    // clang-format off
    llvm::X86::VADDPDYrm,
    llvm::X86::VADDPSYrm,
    llvm::X86::VADDSUBPDYrm,
    llvm::X86::VADDSUBPSYrm,
    llvm::X86::VAESDECLASTYrm,
    llvm::X86::VAESDECYrm,
    llvm::X86::VAESENCLASTYrm,
    llvm::X86::VAESENCYrm,
    llvm::X86::VANDNPDYrm,
    llvm::X86::VANDNPSYrm,
    llvm::X86::VANDPDYrm,
    llvm::X86::VANDPSYrm,
    llvm::X86::VBLENDPDYrmi,
    llvm::X86::VBLENDPSYrmi,
    llvm::X86::VBLENDVPDYrm,
    llvm::X86::VBLENDVPSYrm,
    llvm::X86::VCMPPDYrmi,
    llvm::X86::VCMPPSYrmi,
    llvm::X86::VCVTDQ2PSYrm,
    llvm::X86::VCVTPD2DQYrm,
    llvm::X86::VCVTPD2PSYrm,
    llvm::X86::VCVTPS2DQYrm,
    llvm::X86::VCVTTPD2DQYrm,
    llvm::X86::VCVTTPS2DQYrm,
    llvm::X86::VDIVPDYrm,
    llvm::X86::VDIVPSYrm,
    llvm::X86::VDPPSYrmi,
    llvm::X86::VFMADD132PDYm,
    llvm::X86::VFMADD132PSYm,
    llvm::X86::VFMADD213PDYm,
    llvm::X86::VFMADD213PSYm,
    llvm::X86::VFMADD231PDYm,
    llvm::X86::VFMADD231PSYm,
    llvm::X86::VFMADDPD4Ymr,
    llvm::X86::VFMADDPD4Yrm,
    llvm::X86::VFMADDPS4Ymr,
    llvm::X86::VFMADDPS4Yrm,
    llvm::X86::VFMADDSUB132PDYm,
    llvm::X86::VFMADDSUB132PSYm,
    llvm::X86::VFMADDSUB213PDYm,
    llvm::X86::VFMADDSUB213PSYm,
    llvm::X86::VFMADDSUB231PDYm,
    llvm::X86::VFMADDSUB231PSYm,
    llvm::X86::VFMADDSUBPD4Ymr,
    llvm::X86::VFMADDSUBPD4Yrm,
    llvm::X86::VFMADDSUBPS4Ymr,
    llvm::X86::VFMADDSUBPS4Yrm,
    llvm::X86::VFMSUB132PDYm,
    llvm::X86::VFMSUB132PSYm,
    llvm::X86::VFMSUB213PDYm,
    llvm::X86::VFMSUB213PSYm,
    llvm::X86::VFMSUB231PDYm,
    llvm::X86::VFMSUB231PSYm,
    llvm::X86::VFMSUBADD132PDYm,
    llvm::X86::VFMSUBADD132PSYm,
    llvm::X86::VFMSUBADD213PDYm,
    llvm::X86::VFMSUBADD213PSYm,
    llvm::X86::VFMSUBADD231PDYm,
    llvm::X86::VFMSUBADD231PSYm,
    llvm::X86::VFMSUBADDPD4Ymr,
    llvm::X86::VFMSUBADDPD4Yrm,
    llvm::X86::VFMSUBADDPS4Ymr,
    llvm::X86::VFMSUBADDPS4Yrm,
    llvm::X86::VFMSUBPD4Ymr,
    llvm::X86::VFMSUBPD4Yrm,
    llvm::X86::VFMSUBPS4Ymr,
    llvm::X86::VFMSUBPS4Yrm,
    llvm::X86::VFNMADD132PDYm,
    llvm::X86::VFNMADD132PSYm,
    llvm::X86::VFNMADD213PDYm,
    llvm::X86::VFNMADD213PSYm,
    llvm::X86::VFNMADD231PDYm,
    llvm::X86::VFNMADD231PSYm,
    llvm::X86::VFNMADDPD4Ymr,
    llvm::X86::VFNMADDPD4Yrm,
    llvm::X86::VFNMADDPS4Ymr,
    llvm::X86::VFNMADDPS4Yrm,
    llvm::X86::VFNMSUB132PDYm,
    llvm::X86::VFNMSUB132PSYm,
    llvm::X86::VFNMSUB213PDYm,
    llvm::X86::VFNMSUB213PSYm,
    llvm::X86::VFNMSUB231PDYm,
    llvm::X86::VFNMSUB231PSYm,
    llvm::X86::VFNMSUBPD4Ymr,
    llvm::X86::VFNMSUBPD4Yrm,
    llvm::X86::VFNMSUBPS4Ymr,
    llvm::X86::VFNMSUBPS4Yrm,
    llvm::X86::VGF2P8AFFINEINVQBYrmi,
    llvm::X86::VGF2P8AFFINEQBYrmi,
    llvm::X86::VGF2P8MULBYrm,
    llvm::X86::VHADDPDYrm,
    llvm::X86::VHADDPSYrm,
    llvm::X86::VHSUBPDYrm,
    llvm::X86::VHSUBPSYrm,
    llvm::X86::VLDDQUYrm,
    llvm::X86::VMASKMOVPDYmr,
    llvm::X86::VMASKMOVPDYrm,
    llvm::X86::VMASKMOVPSYmr,
    llvm::X86::VMASKMOVPSYrm,
    llvm::X86::VMAXPDYrm,
    llvm::X86::VMAXPSYrm,
    llvm::X86::VMINPDYrm,
    llvm::X86::VMINPSYrm,
    llvm::X86::VMOVAPDYrm,
    llvm::X86::VMOVAPSYrm,
    llvm::X86::VMOVDDUPYrm,
    llvm::X86::VMOVDQAYrm,
    llvm::X86::VMOVDQUYrm,
    llvm::X86::VMOVNTDQAYrm,
    llvm::X86::VMOVSHDUPYrm,
    llvm::X86::VMOVSLDUPYrm,
    llvm::X86::VMOVUPDYrm,
    llvm::X86::VMOVUPSYrm,
    llvm::X86::VMPSADBWYrmi,
    llvm::X86::VMULPDYrm,
    llvm::X86::VMULPSYrm,
    llvm::X86::VORPDYrm,
    llvm::X86::VORPSYrm,
    llvm::X86::VPABSBYrm,
    llvm::X86::VPABSDYrm,
    llvm::X86::VPABSWYrm,
    llvm::X86::VPACKSSDWYrm,
    llvm::X86::VPACKSSWBYrm,
    llvm::X86::VPACKUSDWYrm,
    llvm::X86::VPACKUSWBYrm,
    llvm::X86::VPADDBYrm,
    llvm::X86::VPADDDYrm,
    llvm::X86::VPADDQYrm,
    llvm::X86::VPADDSBYrm,
    llvm::X86::VPADDSWYrm,
    llvm::X86::VPADDUSBYrm,
    llvm::X86::VPADDUSWYrm,
    llvm::X86::VPADDWYrm,
    llvm::X86::VPALIGNRYrmi,
    llvm::X86::VPANDNYrm,
    llvm::X86::VPANDYrm,
    llvm::X86::VPAVGBYrm,
    llvm::X86::VPAVGWYrm,
    llvm::X86::VPBLENDDYrmi,
    llvm::X86::VPBLENDVBYrm,
    llvm::X86::VPBLENDWYrmi,
    llvm::X86::VPCLMULQDQYrm,
    llvm::X86::VPCMPEQBYrm,
    llvm::X86::VPCMPEQDYrm,
    llvm::X86::VPCMPEQQYrm,
    llvm::X86::VPCMPEQWYrm,
    llvm::X86::VPCMPGTBYrm,
    llvm::X86::VPCMPGTDYrm,
    llvm::X86::VPCMPGTQYrm,
    llvm::X86::VPCMPGTWYrm,
    llvm::X86::VPERM2F128rm,
    llvm::X86::VPERM2I128rm,
    llvm::X86::VPERMDYrm,
    llvm::X86::VPERMIL2PDYmr,
    llvm::X86::VPERMIL2PDYrm,
    llvm::X86::VPERMIL2PSYmr,
    llvm::X86::VPERMIL2PSYrm,
    llvm::X86::VPERMILPDYmi,
    llvm::X86::VPERMILPDYrm,
    llvm::X86::VPERMILPSYmi,
    llvm::X86::VPERMILPSYrm,
    llvm::X86::VPERMPDYmi,
    llvm::X86::VPERMPSYrm,
    llvm::X86::VPERMQYmi,
    llvm::X86::VPHADDDYrm,
    llvm::X86::VPHADDSWYrm,
    llvm::X86::VPHADDWYrm,
    llvm::X86::VPHSUBDYrm,
    llvm::X86::VPHSUBSWYrm,
    llvm::X86::VPHSUBWYrm,
    llvm::X86::VPMADDUBSWYrm,
    llvm::X86::VPMADDWDYrm,
    llvm::X86::VPMASKMOVDYmr,
    llvm::X86::VPMASKMOVDYrm,
    llvm::X86::VPMASKMOVQYmr,
    llvm::X86::VPMASKMOVQYrm,
    llvm::X86::VPMAXSBYrm,
    llvm::X86::VPMAXSDYrm,
    llvm::X86::VPMAXSWYrm,
    llvm::X86::VPMAXUBYrm,
    llvm::X86::VPMAXUDYrm,
    llvm::X86::VPMAXUWYrm,
    llvm::X86::VPMINSBYrm,
    llvm::X86::VPMINSDYrm,
    llvm::X86::VPMINSWYrm,
    llvm::X86::VPMINUBYrm,
    llvm::X86::VPMINUDYrm,
    llvm::X86::VPMINUWYrm,
    llvm::X86::VPMULDQYrm,
    llvm::X86::VPMULHRSWYrm,
    llvm::X86::VPMULHUWYrm,
    llvm::X86::VPMULHWYrm,
    llvm::X86::VPMULLDYrm,
    llvm::X86::VPMULLWYrm,
    llvm::X86::VPMULUDQYrm,
    llvm::X86::VPORYrm,
    llvm::X86::VPSADBWYrm,
    llvm::X86::VPSHUFBYrm,
    llvm::X86::VPSHUFDYmi,
    llvm::X86::VPSHUFHWYmi,
    llvm::X86::VPSHUFLWYmi,
    llvm::X86::VPSIGNBYrm,
    llvm::X86::VPSIGNDYrm,
    llvm::X86::VPSIGNWYrm,
    llvm::X86::VPSLLVDYrm,
    llvm::X86::VPSLLVQYrm,
    llvm::X86::VPSRAVDYrm,
    llvm::X86::VPSRLVDYrm,
    llvm::X86::VPSRLVQYrm,
    llvm::X86::VPSUBBYrm,
    llvm::X86::VPSUBDYrm,
    llvm::X86::VPSUBQYrm,
    llvm::X86::VPSUBSBYrm,
    llvm::X86::VPSUBSWYrm,
    llvm::X86::VPSUBUSBYrm,
    llvm::X86::VPSUBUSWYrm,
    llvm::X86::VPSUBWYrm,
    llvm::X86::VPTESTYrm,
    llvm::X86::VPUNPCKHBWYrm,
    llvm::X86::VPUNPCKHDQYrm,
    llvm::X86::VPUNPCKHQDQYrm,
    llvm::X86::VPUNPCKHWDYrm,
    llvm::X86::VPUNPCKLBWYrm,
    llvm::X86::VPUNPCKLDQYrm,
    llvm::X86::VPUNPCKLQDQYrm,
    llvm::X86::VPUNPCKLWDYrm,
    llvm::X86::VPXORYrm,
    llvm::X86::VRCPPSYm,
    llvm::X86::VROUNDPDYm,
    llvm::X86::VROUNDPSYm,
    llvm::X86::VRSQRTPSYm,
    llvm::X86::VSHUFPDYrmi,
    llvm::X86::VSHUFPSYrmi,
    llvm::X86::VSQRTPDYm,
    llvm::X86::VSQRTPSYm,
    llvm::X86::VSUBPDYrm,
    llvm::X86::VSUBPSYrm,
    llvm::X86::VTESTPDYrm,
    llvm::X86::VTESTPSYrm,
    llvm::X86::VUNPCKHPDYrm,
    llvm::X86::VUNPCKHPSYrm,
    llvm::X86::VUNPCKLPDYrm,
    llvm::X86::VUNPCKLPSYrm,
    llvm::X86::VXORPDYrm,
    llvm::X86::VXORPSYrm,
    // clang-format on
};

constexpr size_t READ_256_SIZE = sizeof(READ_256) / sizeof(unsigned);

constexpr unsigned READ_864[] = {
    // clang-format off
    llvm::X86::FRSTORm,
    // clang-format on
};

constexpr size_t READ_864_SIZE = sizeof(READ_864) / sizeof(unsigned);

constexpr unsigned READ_4096[] = {
    // clang-format off
    llvm::X86::FXRSTOR,
    llvm::X86::FXRSTOR64,
    llvm::X86::MOVDIR64B16,
    llvm::X86::MOVDIR64B32,
    llvm::X86::MOVDIR64B64,
    // clang-format on
};

constexpr size_t READ_4096_SIZE = sizeof(READ_4096) / sizeof(unsigned);

constexpr unsigned READ_4608[] = {
    // clang-format off
    llvm::X86::XRSTOR,
    llvm::X86::XRSTOR64,
    llvm::X86::XRSTORS,
    llvm::X86::XRSTORS64,
    llvm::X86::XSAVE,
    llvm::X86::XSAVE64,
    llvm::X86::XSAVEC,
    llvm::X86::XSAVEC64,
    llvm::X86::XSAVEOPT,
    llvm::X86::XSAVEOPT64,
    llvm::X86::XSAVES,
    llvm::X86::XSAVES64,
    // clang-format on
};

constexpr size_t READ_4608_SIZE = sizeof(READ_4608) / sizeof(unsigned);

constexpr unsigned WRITE_8[] = {
    // clang-format off
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
    llvm::X86::MOV8mr_NOREX,
    llvm::X86::MOV8o16a,
    llvm::X86::MOV8o32a,
    llvm::X86::MOV8o64a,
    llvm::X86::MOVSB,
    llvm::X86::NEG8m,
    llvm::X86::NOT8m,
    llvm::X86::OR8mi,
    llvm::X86::OR8mi8,
    llvm::X86::OR8mr,
    llvm::X86::PEXTRBmr,
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
    llvm::X86::SAR8m1,
    llvm::X86::SAR8mCL,
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
    llvm::X86::STOSB,
    llvm::X86::SUB8mi,
    llvm::X86::SUB8mi8,
    llvm::X86::SUB8mr,
    llvm::X86::VPEXTRBmr,
    llvm::X86::XADD8rm,
    llvm::X86::XCHG8rm,
    llvm::X86::XOR8mi,
    llvm::X86::XOR8mi8,
    llvm::X86::XOR8mr,
    // clang-format on
};

constexpr size_t WRITE_8_SIZE = sizeof(WRITE_8) / sizeof(unsigned);

constexpr unsigned WRITE_16[] = {
    // clang-format off
    llvm::X86::ADC16mi,
    llvm::X86::ADC16mi8,
    llvm::X86::ADC16mr,
    llvm::X86::ADD16mi,
    llvm::X86::ADD16mi8,
    llvm::X86::ADD16mr,
    llvm::X86::AND16mi,
    llvm::X86::AND16mi8,
    llvm::X86::AND16mr,
    llvm::X86::ARPL16mr,
    llvm::X86::BTC16mi8,
    llvm::X86::BTC16mr,
    llvm::X86::BTR16mi8,
    llvm::X86::BTR16mr,
    llvm::X86::BTS16mi8,
    llvm::X86::BTS16mr,
    llvm::X86::CMPXCHG16rm,
    llvm::X86::DEC16m,
    llvm::X86::FNSTCW16m,
    llvm::X86::FNSTSWm,
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
    llvm::X86::MOV16ms,
    llvm::X86::MOV16o16a,
    llvm::X86::MOV16o32a,
    llvm::X86::MOV16o64a,
    llvm::X86::MOVBE16mr,
    llvm::X86::MOVSW,
    llvm::X86::NEG16m,
    llvm::X86::NOT16m,
    llvm::X86::OR16mi,
    llvm::X86::OR16mi8,
    llvm::X86::OR16mr,
    llvm::X86::PEXTRWmr,
    llvm::X86::POP16rmm,
    llvm::X86::RCL16m1,
    llvm::X86::RCL16mCL,
    llvm::X86::RCL16mi,
    llvm::X86::RCR16m1,
    llvm::X86::RCR16mCL,
    llvm::X86::RCR16mi,
    llvm::X86::ROL16m1,
    llvm::X86::ROL16mCL,
    llvm::X86::ROL16mi,
    llvm::X86::ROR16m1,
    llvm::X86::ROR16mCL,
    llvm::X86::ROR16mi,
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
    llvm::X86::SLDT16m,
    llvm::X86::STOSW,
    llvm::X86::STRm,
    llvm::X86::SUB16mi,
    llvm::X86::SUB16mi8,
    llvm::X86::SUB16mr,
    llvm::X86::VPEXTRWmr,
    llvm::X86::XADD16rm,
    llvm::X86::XCHG16rm,
    llvm::X86::XOR16mi,
    llvm::X86::XOR16mi8,
    llvm::X86::XOR16mr,
    // clang-format on
};

constexpr size_t WRITE_16_SIZE = sizeof(WRITE_16) / sizeof(unsigned);

constexpr unsigned WRITE_32[] = {
    // clang-format off
    llvm::X86::ADC32mi,
    llvm::X86::ADC32mi8,
    llvm::X86::ADC32mr,
    llvm::X86::ADD32mi,
    llvm::X86::ADD32mi8,
    llvm::X86::ADD32mr,
    llvm::X86::AND32mi,
    llvm::X86::AND32mi8,
    llvm::X86::AND32mr,
    llvm::X86::BTC32mi8,
    llvm::X86::BTC32mr,
    llvm::X86::BTR32mi8,
    llvm::X86::BTR32mr,
    llvm::X86::BTS32mi8,
    llvm::X86::BTS32mr,
    llvm::X86::CMPXCHG32rm,
    llvm::X86::DEC32m,
    llvm::X86::EXTRACTPSmr,
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
    llvm::X86::MMX_MOVD64mr,
    llvm::X86::MOV32mi,
    llvm::X86::MOV32mr,
    llvm::X86::MOV32o16a,
    llvm::X86::MOV32o32a,
    llvm::X86::MOV32o64a,
    llvm::X86::MOVBE32mr,
    llvm::X86::MOVDIRI32,
    llvm::X86::MOVNTImr,
    llvm::X86::MOVNTSS,
    llvm::X86::MOVPDI2DImr,
    llvm::X86::MOVSL,
    llvm::X86::MOVSSmr,
    llvm::X86::NEG32m,
    llvm::X86::NOT32m,
    llvm::X86::OR32mi,
    llvm::X86::OR32mi8,
    llvm::X86::OR32mi8Locked,
    llvm::X86::OR32mr,
    llvm::X86::PEXTRDmr,
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
    llvm::X86::STOSL,
    llvm::X86::ST_F32m,
    llvm::X86::ST_FP32m,
    llvm::X86::SUB32mi,
    llvm::X86::SUB32mi8,
    llvm::X86::SUB32mr,
    llvm::X86::VEXTRACTPSmr,
    llvm::X86::VMOVPDI2DImr,
    llvm::X86::VMOVSSmr,
    llvm::X86::VPEXTRDmr,
    llvm::X86::VSTMXCSR,
    llvm::X86::XADD32rm,
    llvm::X86::XCHG32rm,
    llvm::X86::XOR32mi,
    llvm::X86::XOR32mi8,
    llvm::X86::XOR32mr,
    // clang-format on
};

constexpr size_t WRITE_32_SIZE = sizeof(WRITE_32) / sizeof(unsigned);

constexpr unsigned WRITE_64[] = {
    // clang-format off
    llvm::X86::ADC64mi32,
    llvm::X86::ADC64mi8,
    llvm::X86::ADC64mr,
    llvm::X86::ADD64mi32,
    llvm::X86::ADD64mi8,
    llvm::X86::ADD64mr,
    llvm::X86::AND64mi32,
    llvm::X86::AND64mi8,
    llvm::X86::AND64mr,
    llvm::X86::BNDMOV32mr,
    llvm::X86::BTC64mi8,
    llvm::X86::BTC64mr,
    llvm::X86::BTR64mi8,
    llvm::X86::BTR64mr,
    llvm::X86::BTS64mi8,
    llvm::X86::BTS64mr,
    llvm::X86::CMPXCHG64rm,
    llvm::X86::CMPXCHG8B,
    llvm::X86::DEC64m,
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
    llvm::X86::MMX_MASKMOVQ,
    llvm::X86::MMX_MASKMOVQ64,
    llvm::X86::MMX_MOVNTQmr,
    llvm::X86::MMX_MOVQ64mr,
    llvm::X86::MOV64mi32,
    llvm::X86::MOV64mr,
    llvm::X86::MOV64o32a,
    llvm::X86::MOV64o64a,
    llvm::X86::MOVBE64mr,
    llvm::X86::MOVDIRI64,
    llvm::X86::MOVHPDmr,
    llvm::X86::MOVHPSmr,
    llvm::X86::MOVLPDmr,
    llvm::X86::MOVLPSmr,
    llvm::X86::MOVNTI_64mr,
    llvm::X86::MOVNTSD,
    llvm::X86::MOVPQI2QImr,
    llvm::X86::MOVSDmr,
    llvm::X86::MOVSQ,
    llvm::X86::NEG64m,
    llvm::X86::NOT64m,
    llvm::X86::OR64mi32,
    llvm::X86::OR64mi8,
    llvm::X86::OR64mr,
    llvm::X86::PEXTRQmr,
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
    llvm::X86::STOSQ,
    llvm::X86::ST_F64m,
    llvm::X86::ST_FP64m,
    llvm::X86::SUB64mi32,
    llvm::X86::SUB64mi8,
    llvm::X86::SUB64mr,
    llvm::X86::VCVTPS2PHmr,
    llvm::X86::VMOVHPDmr,
    llvm::X86::VMOVHPSmr,
    llvm::X86::VMOVLPDmr,
    llvm::X86::VMOVLPSmr,
    llvm::X86::VMOVPQI2QImr,
    llvm::X86::VMOVSDmr,
    llvm::X86::VPEXTRQmr,
    llvm::X86::XADD64rm,
    llvm::X86::XCHG64rm,
    llvm::X86::XOR64mi32,
    llvm::X86::XOR64mi8,
    llvm::X86::XOR64mr,
    // clang-format on
};

constexpr size_t WRITE_64_SIZE = sizeof(WRITE_64) / sizeof(unsigned);

constexpr unsigned WRITE_80[] = {
    // clang-format off
    llvm::X86::FBSTPm,
    llvm::X86::ST_FP80m,
    // clang-format on
};

constexpr size_t WRITE_80_SIZE = sizeof(WRITE_80) / sizeof(unsigned);

constexpr unsigned WRITE_128[] = {
    // clang-format off
    llvm::X86::BNDMOV64mr,
    llvm::X86::CMPXCHG16B,
    llvm::X86::LCMPXCHG16B,
    llvm::X86::MASKMOVDQU,
    llvm::X86::MASKMOVDQU64,
    llvm::X86::MOVAPDmr,
    llvm::X86::MOVAPSmr,
    llvm::X86::MOVDQAmr,
    llvm::X86::MOVDQUmr,
    llvm::X86::MOVNTDQmr,
    llvm::X86::MOVNTPDmr,
    llvm::X86::MOVNTPSmr,
    llvm::X86::MOVUPDmr,
    llvm::X86::MOVUPSmr,
    llvm::X86::VCVTPS2PHYmr,
    llvm::X86::VEXTRACTF128mr,
    llvm::X86::VEXTRACTI128mr,
    llvm::X86::VMASKMOVDQU,
    llvm::X86::VMASKMOVDQU64,
    llvm::X86::VMASKMOVPDmr,
    llvm::X86::VMASKMOVPSmr,
    llvm::X86::VMOVAPDmr,
    llvm::X86::VMOVAPSmr,
    llvm::X86::VMOVDQAmr,
    llvm::X86::VMOVDQUmr,
    llvm::X86::VMOVNTDQmr,
    llvm::X86::VMOVNTPDmr,
    llvm::X86::VMOVNTPSmr,
    llvm::X86::VMOVUPDmr,
    llvm::X86::VMOVUPSmr,
    llvm::X86::VPMASKMOVDmr,
    llvm::X86::VPMASKMOVQmr,
    // clang-format on
};

constexpr size_t WRITE_128_SIZE = sizeof(WRITE_128) / sizeof(unsigned);

constexpr unsigned WRITE_224[] = {
    // clang-format off
    llvm::X86::FSTENVm,
    // clang-format on
};

constexpr size_t WRITE_224_SIZE = sizeof(WRITE_224) / sizeof(unsigned);

constexpr unsigned WRITE_256[] = {
    // clang-format off
    llvm::X86::VMASKMOVPDYmr,
    llvm::X86::VMASKMOVPSYmr,
    llvm::X86::VMOVAPDYmr,
    llvm::X86::VMOVAPSYmr,
    llvm::X86::VMOVDQAYmr,
    llvm::X86::VMOVDQUYmr,
    llvm::X86::VMOVNTDQYmr,
    llvm::X86::VMOVNTPDYmr,
    llvm::X86::VMOVNTPSYmr,
    llvm::X86::VMOVUPDYmr,
    llvm::X86::VMOVUPSYmr,
    llvm::X86::VPMASKMOVDYmr,
    llvm::X86::VPMASKMOVQYmr,
    // clang-format on
};

constexpr size_t WRITE_256_SIZE = sizeof(WRITE_256) / sizeof(unsigned);

constexpr unsigned WRITE_864[] = {
    // clang-format off
    llvm::X86::FSAVEm,
    // clang-format on
};

constexpr size_t WRITE_864_SIZE = sizeof(WRITE_864) / sizeof(unsigned);

constexpr unsigned WRITE_4096[] = {
    // clang-format off
    llvm::X86::FXSAVE,
    llvm::X86::FXSAVE64,
    llvm::X86::MOVDIR64B16,
    llvm::X86::MOVDIR64B32,
    llvm::X86::MOVDIR64B64,
    // clang-format on
};

constexpr size_t WRITE_4096_SIZE = sizeof(WRITE_4096) / sizeof(unsigned);

constexpr unsigned WRITE_4608[] = {
    // clang-format off
    llvm::X86::XSAVE,
    llvm::X86::XSAVE64,
    llvm::X86::XSAVEC,
    llvm::X86::XSAVEC64,
    llvm::X86::XSAVEOPT,
    llvm::X86::XSAVEOPT64,
    llvm::X86::XSAVES,
    llvm::X86::XSAVES64,
    // clang-format on
};

constexpr size_t WRITE_4608_SIZE = sizeof(WRITE_4608) / sizeof(unsigned);

constexpr unsigned STACK_WRITE_16[] = {
    // clang-format off
    llvm::X86::PUSH16i8,
    llvm::X86::PUSH16r,
    llvm::X86::PUSH16rmm,
    llvm::X86::PUSH16rmr,
    llvm::X86::PUSHF16,
    llvm::X86::PUSHi16,
    // clang-format on
};

constexpr size_t STACK_WRITE_16_SIZE =
    sizeof(STACK_WRITE_16) / sizeof(unsigned);

constexpr unsigned STACK_WRITE_32[] = {
// clang-format off
#ifdef QBDI_ARCH_X86
    llvm::X86::CALL16m,
    llvm::X86::CALL16m_NT,
    llvm::X86::CALL16r,
    llvm::X86::CALL16r_NT,
    llvm::X86::CALL32m,
    llvm::X86::CALL32m_NT,
    llvm::X86::CALL32r,
    llvm::X86::CALL32r_NT,
    llvm::X86::CALL64m,
    llvm::X86::CALL64m_NT,
    llvm::X86::CALL64pcrel32,
    llvm::X86::CALL64r,
    llvm::X86::CALL64r_NT,
    llvm::X86::CALLpcrel16,
    llvm::X86::CALLpcrel32,
    llvm::X86::ENTER,
#endif
    llvm::X86::PUSH32i8,
    llvm::X86::PUSH32r,
    llvm::X86::PUSH32rmm,
    llvm::X86::PUSH32rmr,
    llvm::X86::PUSHF32,
    llvm::X86::PUSHi32,
    // clang-format on
};

constexpr size_t STACK_WRITE_32_SIZE =
    sizeof(STACK_WRITE_32) / sizeof(unsigned);

constexpr unsigned STACK_WRITE_64[] = {
// clang-format off
#ifdef QBDI_ARCH_X86_64
    llvm::X86::CALL16m,
    llvm::X86::CALL16m_NT,
    llvm::X86::CALL16r,
    llvm::X86::CALL16r_NT,
    llvm::X86::CALL32m,
    llvm::X86::CALL32m_NT,
    llvm::X86::CALL32r,
    llvm::X86::CALL32r_NT,
    llvm::X86::CALL64m,
    llvm::X86::CALL64m_NT,
    llvm::X86::CALL64pcrel32,
    llvm::X86::CALL64r,
    llvm::X86::CALL64r_NT,
    llvm::X86::CALLpcrel16,
    llvm::X86::CALLpcrel32,
    llvm::X86::ENTER,
#endif
    llvm::X86::PUSH64i32,
    llvm::X86::PUSH64i8,
    llvm::X86::PUSH64r,
    llvm::X86::PUSH64rmm,
    llvm::X86::PUSH64rmr,
    llvm::X86::PUSHF64,
    // clang-format on
};

constexpr size_t STACK_WRITE_64_SIZE =
    sizeof(STACK_WRITE_64) / sizeof(unsigned);

constexpr unsigned STACK_WRITE_128[] = {
    // clang-format off
    llvm::X86::PUSHA16,
    // clang-format on
};

constexpr size_t STACK_WRITE_128_SIZE =
    sizeof(STACK_WRITE_128) / sizeof(unsigned);

constexpr unsigned STACK_WRITE_256[] = {
    // clang-format off
    llvm::X86::PUSHA32,
    // clang-format on
};

constexpr size_t STACK_WRITE_256_SIZE =
    sizeof(STACK_WRITE_256) / sizeof(unsigned);

constexpr unsigned STACK_READ_16[] = {
    // clang-format off
    llvm::X86::POP16r,
    llvm::X86::POP16rmm,
    llvm::X86::POP16rmr,
    llvm::X86::POPF16,
    // clang-format on
};

constexpr size_t STACK_READ_16_SIZE = sizeof(STACK_READ_16) / sizeof(unsigned);

constexpr unsigned STACK_READ_32[] = {
// clang-format off
#ifdef QBDI_ARCH_X86
    llvm::X86::LEAVE,
    llvm::X86::LRETIL,
    llvm::X86::LRETIQ,
    llvm::X86::LRETIW,
    llvm::X86::LRETL,
    llvm::X86::LRETQ,
    llvm::X86::LRETW,
#endif
    llvm::X86::POP32r,
    llvm::X86::POP32rmm,
    llvm::X86::POP32rmr,
    llvm::X86::POPF32,
#ifdef QBDI_ARCH_X86
    llvm::X86::RETIL,
    llvm::X86::RETIQ,
    llvm::X86::RETIW,
    llvm::X86::RETL,
    llvm::X86::RETQ,
    llvm::X86::RETW,
#endif
    // clang-format on
};

constexpr size_t STACK_READ_32_SIZE = sizeof(STACK_READ_32) / sizeof(unsigned);

constexpr unsigned STACK_READ_64[] = {
// clang-format off
#ifdef QBDI_ARCH_X86_64
    llvm::X86::LEAVE,
#endif
    llvm::X86::LEAVE64,
#ifdef QBDI_ARCH_X86_64
    llvm::X86::LRETIL,
    llvm::X86::LRETIQ,
    llvm::X86::LRETIW,
    llvm::X86::LRETL,
    llvm::X86::LRETQ,
    llvm::X86::LRETW,
#endif
    llvm::X86::POP64r,
    llvm::X86::POP64rmm,
    llvm::X86::POP64rmr,
    llvm::X86::POPF64,
#ifdef QBDI_ARCH_X86_64
    llvm::X86::RETIL,
    llvm::X86::RETIQ,
    llvm::X86::RETIW,
    llvm::X86::RETL,
    llvm::X86::RETQ,
    llvm::X86::RETW,
#endif
    // clang-format on
};

constexpr size_t STACK_READ_64_SIZE = sizeof(STACK_READ_64) / sizeof(unsigned);

constexpr unsigned STACK_READ_128[] = {
    llvm::X86::POPA16,
};

constexpr size_t STACK_READ_128_SIZE =
    sizeof(STACK_READ_128) / sizeof(unsigned);

constexpr unsigned STACK_READ_256[] = {
    llvm::X86::POPA32,
};

constexpr size_t STACK_READ_256_SIZE =
    sizeof(STACK_READ_256) / sizeof(unsigned);

constexpr unsigned MIN_SIZE_READ[] = {
    // clang-format off
    llvm::X86::XRSTOR,
    llvm::X86::XRSTOR64,
    llvm::X86::XRSTORS,
    llvm::X86::XRSTORS64,
    llvm::X86::XSAVE,
    llvm::X86::XSAVE64,
    llvm::X86::XSAVEC,
    llvm::X86::XSAVEC64,
    llvm::X86::XSAVEOPT,
    llvm::X86::XSAVEOPT64,
    llvm::X86::XSAVES,
    llvm::X86::XSAVES64,
    // clang-format on
};

constexpr size_t MIN_SIZE_READ_SIZE = sizeof(MIN_SIZE_READ) / sizeof(unsigned);

constexpr unsigned MIN_SIZE_WRITE[] = {
    // clang-format off
    llvm::X86::XSAVE,
    llvm::X86::XSAVE64,
    llvm::X86::XSAVEC,
    llvm::X86::XSAVEC64,
    llvm::X86::XSAVEOPT,
    llvm::X86::XSAVEOPT64,
    llvm::X86::XSAVES,
    llvm::X86::XSAVES64,
    // clang-format on
};

constexpr size_t MIN_SIZE_WRITE_SIZE =
    sizeof(MIN_SIZE_WRITE) / sizeof(unsigned);

/* Highest 16 bits are the write access, lowest 16 bits are the read access. For
 * each 16 bits part: the highest bit stores if the access is a stack access or
 * not while the lowest 12 bits store the unsigned access size in bytes (thus up
 * to 4095 bytes). A size of 0 means no access.
 *
 * ------------------------------------------------------------------
 * | Ox1f                        WRITE ACCESS                  0x1c |
 * ------------------------------------------------------------------
 * | 1 bit stack access flag | 1 bit minimum size | 2 bits reserved |
 * ------------------------------------------------------------------
 *
 * --------------------------------
 * | 0x1b   WRITE ACCESS     0x10 |
 * --------------------------------
 * | 12 bits unsigned access size |
 * --------------------------------
 *
 *
 * ------------------------------------------------------------------
 * | 0xf                           READ ACCESS                  0xc |
 * ------------------------------------------------------------------
 * | 1 bit stack access flag | 1 bit minimum size | 2 bits reserved |
 * ------------------------------------------------------------------
 *
 * --------------------------------
 * | 0xb     READ ACCESS     0x10 |
 * --------------------------------
 * | 12 bits unsigned access size |
 * --------------------------------
 */

constexpr uint32_t WRITE_POSITION = 16;
constexpr uint32_t STACK_ACCESS_FLAG = 0x8000;
constexpr uint32_t ACCESS_MIN_SIZE_FLAG = 0x4000;
constexpr uint32_t READ(uint32_t s) { return s & 0xfff; }
constexpr uint32_t WRITE(uint32_t s) { return (s & 0xfff) << WRITE_POSITION; }
constexpr uint32_t STACK_READ(uint32_t s) {
  return STACK_ACCESS_FLAG | READ(s);
}
constexpr uint32_t STACK_WRITE(uint32_t s) {
  return ((STACK_ACCESS_FLAG) << WRITE_POSITION) | WRITE(s);
}
constexpr uint32_t GET_READ_SIZE(uint32_t v) { return v & 0xfff; }
constexpr uint32_t GET_WRITE_SIZE(uint32_t v) {
  return (v >> WRITE_POSITION) & 0xfff;
}
constexpr uint32_t IS_STACK_READ(uint32_t v) {
  return (v & STACK_ACCESS_FLAG) == STACK_ACCESS_FLAG;
}
constexpr uint32_t IS_STACK_WRITE(uint32_t v) {
  return ((v >> WRITE_POSITION) & STACK_ACCESS_FLAG) == STACK_ACCESS_FLAG;
}
constexpr uint32_t IS_MIN_SIZE_READ(uint32_t v) {
  return (v & ACCESS_MIN_SIZE_FLAG) == ACCESS_MIN_SIZE_FLAG;
}
constexpr uint32_t IS_MIN_SIZE_WRITE(uint32_t v) {
  return ((v >> WRITE_POSITION) & ACCESS_MIN_SIZE_FLAG) == ACCESS_MIN_SIZE_FLAG;
}

struct MemAccessArray {
  uint32_t arr[llvm::X86::INSTRUCTION_LIST_END] = {0};

  constexpr inline void _initMemAccessRead(const unsigned buff[],
                                           size_t buff_size, uint32_t len) {
    for (size_t i = 0; i < buff_size; i++) {
      arr[buff[i]] |= READ(len);
    }
  }

  constexpr inline void _initMemAccessWrite(const unsigned buff[],
                                            size_t buff_size, uint32_t len) {
    for (size_t i = 0; i < buff_size; i++) {
      arr[buff[i]] |= WRITE(len);
    }
  }

  constexpr inline void _initMemAccessStackRead(const unsigned buff[],
                                                size_t buff_size,
                                                uint32_t len) {
    for (size_t i = 0; i < buff_size; i++) {
      arr[buff[i]] |= STACK_READ(len);
    }
  }

  constexpr inline void _initMemAccessStackWrite(const unsigned buff[],
                                                 size_t buff_size,
                                                 uint32_t len) {
    for (size_t i = 0; i < buff_size; i++) {
      arr[buff[i]] |= STACK_WRITE(len);
    }
  }

  constexpr MemAccessArray() {
    // read
    _initMemAccessRead(READ_8, READ_8_SIZE, 1);
    _initMemAccessRead(READ_16, READ_16_SIZE, 2);
    _initMemAccessRead(READ_32, READ_32_SIZE, 4);
    _initMemAccessRead(READ_64, READ_64_SIZE, 8);
    _initMemAccessRead(READ_80, READ_80_SIZE, 10);
    _initMemAccessRead(READ_128, READ_128_SIZE, 16);
    _initMemAccessRead(READ_224, READ_224_SIZE, 28);
    _initMemAccessRead(READ_256, READ_256_SIZE, 32);
    _initMemAccessRead(READ_864, READ_864_SIZE, 108);
    _initMemAccessRead(READ_4096, READ_4096_SIZE, 512);
    _initMemAccessRead(READ_4608, READ_4608_SIZE, 576);
    // write
    _initMemAccessWrite(WRITE_8, WRITE_8_SIZE, 1);
    _initMemAccessWrite(WRITE_16, WRITE_16_SIZE, 2);
    _initMemAccessWrite(WRITE_32, WRITE_32_SIZE, 4);
    _initMemAccessWrite(WRITE_64, WRITE_64_SIZE, 8);
    _initMemAccessWrite(WRITE_80, WRITE_80_SIZE, 10);
    _initMemAccessWrite(WRITE_128, WRITE_128_SIZE, 16);
    _initMemAccessWrite(WRITE_224, WRITE_224_SIZE, 28);
    _initMemAccessWrite(WRITE_256, WRITE_256_SIZE, 32);
    _initMemAccessWrite(WRITE_864, WRITE_864_SIZE, 108);
    _initMemAccessWrite(WRITE_4096, WRITE_4096_SIZE, 512);
    _initMemAccessWrite(WRITE_4608, WRITE_4608_SIZE, 576);
    // read stack
    _initMemAccessStackRead(STACK_READ_16, STACK_READ_16_SIZE, 2);
    _initMemAccessStackRead(STACK_READ_32, STACK_READ_32_SIZE, 4);
    _initMemAccessStackRead(STACK_READ_64, STACK_READ_64_SIZE, 8);
    _initMemAccessStackRead(STACK_READ_128, STACK_READ_128_SIZE, 16);
    _initMemAccessStackRead(STACK_READ_256, STACK_READ_256_SIZE, 32);
    // write stack
    _initMemAccessStackWrite(STACK_WRITE_16, STACK_WRITE_16_SIZE, 2);
    _initMemAccessStackWrite(STACK_WRITE_32, STACK_WRITE_32_SIZE, 4);
    _initMemAccessStackWrite(STACK_WRITE_64, STACK_WRITE_64_SIZE, 8);
    _initMemAccessStackWrite(STACK_WRITE_128, STACK_WRITE_128_SIZE, 16);
    _initMemAccessStackWrite(STACK_WRITE_256, STACK_WRITE_256_SIZE, 32);
    // min size read
    for (size_t i = 0; i < MIN_SIZE_READ_SIZE; i++) {
      arr[MIN_SIZE_READ[i]] |= ACCESS_MIN_SIZE_FLAG;
    }
    // min size write
    for (size_t i = 0; i < MIN_SIZE_WRITE_SIZE; i++) {
      arr[MIN_SIZE_WRITE[i]] |= (ACCESS_MIN_SIZE_FLAG << WRITE_POSITION);
    }
  }

#if CHECK_TABLE
  void check_table(const unsigned buff[], size_t buff_size, uint32_t value,
                   uint32_t mask) const {
    for (size_t i = 0; i < buff_size; i++) {
      if ((arr[buff[i]] & mask) != value) {
        fprintf(stderr,
                "[MemAccessArray::check_table], opcode %d, mask %x, expected "
                "%x, found %x\n",
                buff[i], mask, value, (arr[buff[i]] & mask));
        abort();
      }
    }
  }

  int check() const {
    // read
    check_table(READ_8, READ_8_SIZE, READ(1), 0xfff);
    check_table(READ_16, READ_16_SIZE, READ(2), 0xfff);
    check_table(READ_32, READ_32_SIZE, READ(4), 0xfff);
    check_table(READ_64, READ_64_SIZE, READ(8), 0xfff);
    check_table(READ_80, READ_80_SIZE, READ(10), 0xfff);
    check_table(READ_128, READ_128_SIZE, READ(16), 0xfff);
    check_table(READ_224, READ_224_SIZE, READ(28), 0xfff);
    check_table(READ_256, READ_256_SIZE, READ(32), 0xfff);
    check_table(READ_864, READ_864_SIZE, READ(108), 0xfff);
    check_table(READ_4096, READ_4096_SIZE, READ(512), 0xfff);
    check_table(READ_4608, READ_4608_SIZE, READ(576), 0xfff);
    // write
    check_table(WRITE_8, WRITE_8_SIZE, WRITE(1), 0xfff << WRITE_POSITION);
    check_table(WRITE_16, WRITE_16_SIZE, WRITE(2), 0xfff << WRITE_POSITION);
    check_table(WRITE_32, WRITE_32_SIZE, WRITE(4), 0xfff << WRITE_POSITION);
    check_table(WRITE_64, WRITE_64_SIZE, WRITE(8), 0xfff << WRITE_POSITION);
    check_table(WRITE_80, WRITE_80_SIZE, WRITE(10), 0xfff << WRITE_POSITION);
    check_table(WRITE_128, WRITE_128_SIZE, WRITE(16), 0xfff << WRITE_POSITION);
    check_table(WRITE_224, WRITE_224_SIZE, WRITE(28), 0xfff << WRITE_POSITION);
    check_table(WRITE_256, WRITE_256_SIZE, WRITE(32), 0xfff << WRITE_POSITION);
    check_table(WRITE_864, WRITE_864_SIZE, WRITE(108), 0xfff << WRITE_POSITION);
    check_table(WRITE_4096, WRITE_4096_SIZE, WRITE(512),
                0xfff << WRITE_POSITION);
    check_table(WRITE_4608, WRITE_4608_SIZE, WRITE(576),
                0xfff << WRITE_POSITION);
    // read stack
    check_table(STACK_READ_16, STACK_READ_16_SIZE, STACK_READ(2), 0x8fff);
    check_table(STACK_READ_32, STACK_READ_32_SIZE, STACK_READ(4), 0x8fff);
    check_table(STACK_READ_64, STACK_READ_64_SIZE, STACK_READ(8), 0x8fff);
    check_table(STACK_READ_128, STACK_READ_128_SIZE, STACK_READ(16), 0x8fff);
    check_table(STACK_READ_256, STACK_READ_256_SIZE, STACK_READ(32), 0x8fff);
    // write stack
    check_table(STACK_WRITE_16, STACK_WRITE_16_SIZE, STACK_WRITE(2),
                0x8fff << WRITE_POSITION);
    check_table(STACK_WRITE_32, STACK_WRITE_32_SIZE, STACK_WRITE(4),
                0x8fff << WRITE_POSITION);
    check_table(STACK_WRITE_64, STACK_WRITE_64_SIZE, STACK_WRITE(8),
                0x8fff << WRITE_POSITION);
    check_table(STACK_WRITE_128, STACK_WRITE_128_SIZE, STACK_WRITE(16),
                0x8fff << WRITE_POSITION);
    check_table(STACK_WRITE_256, STACK_WRITE_256_SIZE, STACK_WRITE(32),
                0x8fff << WRITE_POSITION);
    // min size read
    check_table(MIN_SIZE_READ, MIN_SIZE_READ_SIZE, ACCESS_MIN_SIZE_FLAG,
                ACCESS_MIN_SIZE_FLAG);
    // min size write
    check_table(MIN_SIZE_WRITE, MIN_SIZE_WRITE_SIZE,
                ACCESS_MIN_SIZE_FLAG << WRITE_POSITION,
                ACCESS_MIN_SIZE_FLAG << WRITE_POSITION);
    return 0;
  }
#endif

  inline uint32_t get(size_t op) const {
    if (op < llvm::X86::INSTRUCTION_LIST_END) {
      return arr[op];
    }

    QBDI_ERROR("No opcode {}", op);
    return 0;
  }
};

static constexpr MemAccessArray memAccessCache;

#if CHECK_TABLE
static int __check_debug = memAccessCache.check();
#endif

} // anonymous namespace

unsigned getReadSize(const llvm::MCInst &inst) {
  return GET_READ_SIZE(memAccessCache.get(inst.getOpcode()));
}

unsigned getWriteSize(const llvm::MCInst &inst) {
  return GET_WRITE_SIZE(memAccessCache.get(inst.getOpcode()));
}

bool isStackRead(const llvm::MCInst &inst) {
  return IS_STACK_READ(memAccessCache.get(inst.getOpcode()));
}

bool isStackWrite(const llvm::MCInst &inst) {
  return IS_STACK_WRITE(memAccessCache.get(inst.getOpcode()));
}

bool isMinSizeRead(const llvm::MCInst &inst) {
  return IS_MIN_SIZE_READ(memAccessCache.get(inst.getOpcode()));
}

bool isMinSizeWrite(const llvm::MCInst &inst) {
  return IS_MIN_SIZE_WRITE(memAccessCache.get(inst.getOpcode()));
}

unsigned getImmediateSize(const llvm::MCInst &inst,
                          const llvm::MCInstrDesc &desc) {
  return llvm::X86II::getSizeOfImm(desc.TSFlags);
}

bool useAllRegisters(const llvm::MCInst &inst) {
  if constexpr (is_x86) {
    switch (inst.getOpcode()) {
      case llvm::X86::PUSHA16:
      case llvm::X86::PUSHA32:
      case llvm::X86::POPA16:
      case llvm::X86::POPA32:
        return true;
      default:
        break;
    }
  }

  return false;
}

bool isDoubleRead(const llvm::MCInst &inst) {
  switch (inst.getOpcode()) {
    case llvm::X86::CMPSB:
    case llvm::X86::CMPSL:
    case llvm::X86::CMPSQ:
    case llvm::X86::CMPSW:
      return true;
    default:
      return false;
  }
}

bool mayChangeWriteAddr(const llvm::MCInst &inst,
                        const llvm::MCInstrDesc &desc) {

  switch (desc.TSFlags & llvm::X86II::FormMask) {
    case llvm::X86II::RawFrmDstSrc:
    case llvm::X86II::RawFrmDst:
    case llvm::X86II::RawFrmSrc:
      return true;
    default:
      break;
  }

  switch (inst.getOpcode()) {
    case llvm::X86::XCHG8rm:
    case llvm::X86::XCHG16rm:
    case llvm::X86::XCHG32rm:
    case llvm::X86::XCHG64rm:
    case llvm::X86::CMPXCHG8rm:
    case llvm::X86::CMPXCHG16rm:
    case llvm::X86::CMPXCHG32rm:
    case llvm::X86::CMPXCHG64rm:
    case llvm::X86::CMPXCHG8B:
    case llvm::X86::CMPXCHG16B:
    case llvm::X86::LCMPXCHG8:
    case llvm::X86::LCMPXCHG16:
    case llvm::X86::LCMPXCHG32:
    case llvm::X86::LCMPXCHG64:
    case llvm::X86::LCMPXCHG8B:
    case llvm::X86::LCMPXCHG16B:
      return true;
    default:
      return false;
  }
}

bool hasREPPrefix(const llvm::MCInst &instr) {
  return (instr.getFlags() &
          (llvm::X86::IP_HAS_REPEAT_NE | llvm::X86::IP_HAS_REPEAT)) !=
         llvm::X86::IP_NO_PREFIX;
}

bool implicitDSIAccess(const llvm::MCInst &inst,
                       const llvm::MCInstrDesc &desc) {

  switch (desc.TSFlags & llvm::X86II::FormMask) {
    case llvm::X86II::RawFrmDstSrc:
    case llvm::X86II::RawFrmDst:
    case llvm::X86II::RawFrmSrc:
      return true;
    default:
      break;
  }

  switch (inst.getOpcode()) {
    case llvm::X86::MASKMOVDQU:
    case llvm::X86::MASKMOVDQU64:
    case llvm::X86::MMX_MASKMOVQ:
    case llvm::X86::MMX_MASKMOVQ64:
    case llvm::X86::VMASKMOVDQU:
    case llvm::X86::VMASKMOVDQU64:
      return true;
    default:
      return false;
  }
}

bool unsupportedRead(const llvm::MCInst &inst) {

  switch (inst.getOpcode()) {
    case llvm::X86::VGATHERDPDYrm:
    case llvm::X86::VGATHERDPDrm:
    case llvm::X86::VGATHERDPSYrm:
    case llvm::X86::VGATHERDPSrm:
    case llvm::X86::VGATHERQPDYrm:
    case llvm::X86::VGATHERQPDrm:
    case llvm::X86::VGATHERQPSYrm:
    case llvm::X86::VGATHERQPSrm:
    case llvm::X86::VPGATHERDDYrm:
    case llvm::X86::VPGATHERDDrm:
    case llvm::X86::VPGATHERDQYrm:
    case llvm::X86::VPGATHERDQrm:
    case llvm::X86::VPGATHERQDYrm:
    case llvm::X86::VPGATHERQDrm:
    case llvm::X86::VPGATHERQQYrm:
    case llvm::X86::VPGATHERQQrm:
      return true;
    default:
      return false;
  }
}

}; // namespace QBDI
