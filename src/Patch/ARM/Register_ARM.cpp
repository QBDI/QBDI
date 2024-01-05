/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2023 Quarkslab
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

#include "ARMInstrInfo.h"

#include "Engine/LLVMCPU.h"
#include "Patch/ARM/InstInfo_ARM.h"
#include "Patch/Register.h"
#include "Patch/Types.h"
#include "Utility/LogSys.h"

#include "QBDI/State.h"

namespace QBDI {

static_assert(sizeof(FPRStateVReg) == QBDI_NUM_FPR * 8,
              "Wrong size for FPRState");

const RegLLVM GPR_ID[] = {llvm::ARM::R0,  llvm::ARM::R1,  llvm::ARM::R2,
                          llvm::ARM::R3,  llvm::ARM::R4,  llvm::ARM::R5,
                          llvm::ARM::R6,  llvm::ARM::R7,  llvm::ARM::R8,
                          llvm::ARM::R9,  llvm::ARM::R10, llvm::ARM::R11,
                          llvm::ARM::R12, llvm::ARM::SP,  llvm::ARM::LR,
                          llvm::ARM::PC,  llvm::ARM::CPSR};

const RegLLVM FLAG_ID[] = {};

const RegLLVM SEG_ID[] = {llvm::ARM::ITSTATE};

const std::map<RegLLVM, int16_t> FPR_ID = {
    // size 4b
    {llvm::ARM::S0, offsetof(FPRState, vreg.s[0])},
    {llvm::ARM::S1, offsetof(FPRState, vreg.s[1])},
    {llvm::ARM::S2, offsetof(FPRState, vreg.s[2])},
    {llvm::ARM::S3, offsetof(FPRState, vreg.s[3])},
    {llvm::ARM::S4, offsetof(FPRState, vreg.s[4])},
    {llvm::ARM::S5, offsetof(FPRState, vreg.s[5])},
    {llvm::ARM::S6, offsetof(FPRState, vreg.s[6])},
    {llvm::ARM::S7, offsetof(FPRState, vreg.s[7])},
    {llvm::ARM::S8, offsetof(FPRState, vreg.s[8])},
    {llvm::ARM::S9, offsetof(FPRState, vreg.s[9])},
    {llvm::ARM::S10, offsetof(FPRState, vreg.s[10])},
    {llvm::ARM::S11, offsetof(FPRState, vreg.s[11])},
    {llvm::ARM::S12, offsetof(FPRState, vreg.s[12])},
    {llvm::ARM::S13, offsetof(FPRState, vreg.s[13])},
    {llvm::ARM::S14, offsetof(FPRState, vreg.s[14])},
    {llvm::ARM::S15, offsetof(FPRState, vreg.s[15])},
    {llvm::ARM::S16, offsetof(FPRState, vreg.s[16])},
    {llvm::ARM::S17, offsetof(FPRState, vreg.s[17])},
    {llvm::ARM::S18, offsetof(FPRState, vreg.s[18])},
    {llvm::ARM::S19, offsetof(FPRState, vreg.s[19])},
    {llvm::ARM::S20, offsetof(FPRState, vreg.s[20])},
    {llvm::ARM::S21, offsetof(FPRState, vreg.s[21])},
    {llvm::ARM::S22, offsetof(FPRState, vreg.s[22])},
    {llvm::ARM::S23, offsetof(FPRState, vreg.s[23])},
    {llvm::ARM::S24, offsetof(FPRState, vreg.s[24])},
    {llvm::ARM::S25, offsetof(FPRState, vreg.s[25])},
    {llvm::ARM::S26, offsetof(FPRState, vreg.s[26])},
    {llvm::ARM::S27, offsetof(FPRState, vreg.s[27])},
    {llvm::ARM::S28, offsetof(FPRState, vreg.s[28])},
    {llvm::ARM::S29, offsetof(FPRState, vreg.s[29])},
    {llvm::ARM::S30, offsetof(FPRState, vreg.s[30])},
    {llvm::ARM::S31, offsetof(FPRState, vreg.s[31])},

    // size 8b
    {llvm::ARM::D0, offsetof(FPRState, vreg.d[0])},
    {llvm::ARM::D1, offsetof(FPRState, vreg.d[1])},
    {llvm::ARM::D2, offsetof(FPRState, vreg.d[2])},
    {llvm::ARM::D3, offsetof(FPRState, vreg.d[3])},
    {llvm::ARM::D4, offsetof(FPRState, vreg.d[4])},
    {llvm::ARM::D5, offsetof(FPRState, vreg.d[5])},
    {llvm::ARM::D6, offsetof(FPRState, vreg.d[6])},
    {llvm::ARM::D7, offsetof(FPRState, vreg.d[7])},
    {llvm::ARM::D8, offsetof(FPRState, vreg.d[8])},
    {llvm::ARM::D9, offsetof(FPRState, vreg.d[9])},
    {llvm::ARM::D10, offsetof(FPRState, vreg.d[10])},
    {llvm::ARM::D11, offsetof(FPRState, vreg.d[11])},
    {llvm::ARM::D12, offsetof(FPRState, vreg.d[12])},
    {llvm::ARM::D13, offsetof(FPRState, vreg.d[13])},
    {llvm::ARM::D14, offsetof(FPRState, vreg.d[14])},
    {llvm::ARM::D15, offsetof(FPRState, vreg.d[15])},
#if QBDI_NUM_FPR == 32
    {llvm::ARM::D16, offsetof(FPRState, vreg.d[16])},
    {llvm::ARM::D17, offsetof(FPRState, vreg.d[17])},
    {llvm::ARM::D18, offsetof(FPRState, vreg.d[18])},
    {llvm::ARM::D19, offsetof(FPRState, vreg.d[19])},
    {llvm::ARM::D20, offsetof(FPRState, vreg.d[20])},
    {llvm::ARM::D21, offsetof(FPRState, vreg.d[21])},
    {llvm::ARM::D22, offsetof(FPRState, vreg.d[22])},
    {llvm::ARM::D23, offsetof(FPRState, vreg.d[23])},
    {llvm::ARM::D24, offsetof(FPRState, vreg.d[24])},
    {llvm::ARM::D25, offsetof(FPRState, vreg.d[25])},
    {llvm::ARM::D26, offsetof(FPRState, vreg.d[26])},
    {llvm::ARM::D27, offsetof(FPRState, vreg.d[27])},
    {llvm::ARM::D28, offsetof(FPRState, vreg.d[28])},
    {llvm::ARM::D29, offsetof(FPRState, vreg.d[29])},
    {llvm::ARM::D30, offsetof(FPRState, vreg.d[30])},
    {llvm::ARM::D31, offsetof(FPRState, vreg.d[31])},
#else
    {llvm::ARM::D16, -1},
    {llvm::ARM::D17, -1},
    {llvm::ARM::D18, -1},
    {llvm::ARM::D19, -1},
    {llvm::ARM::D20, -1},
    {llvm::ARM::D21, -1},
    {llvm::ARM::D22, -1},
    {llvm::ARM::D23, -1},
    {llvm::ARM::D24, -1},
    {llvm::ARM::D25, -1},
    {llvm::ARM::D26, -1},
    {llvm::ARM::D27, -1},
    {llvm::ARM::D28, -1},
    {llvm::ARM::D29, -1},
    {llvm::ARM::D30, -1},
    {llvm::ARM::D31, -1},
#endif

    // size 16b
    {llvm::ARM::Q0, offsetof(FPRState, vreg.q[0])},
    {llvm::ARM::Q1, offsetof(FPRState, vreg.q[1])},
    {llvm::ARM::Q2, offsetof(FPRState, vreg.q[2])},
    {llvm::ARM::Q3, offsetof(FPRState, vreg.q[3])},
    {llvm::ARM::Q4, offsetof(FPRState, vreg.q[4])},
    {llvm::ARM::Q5, offsetof(FPRState, vreg.q[5])},
    {llvm::ARM::Q6, offsetof(FPRState, vreg.q[6])},
    {llvm::ARM::Q7, offsetof(FPRState, vreg.q[7])},
#if QBDI_NUM_FPR == 32
    {llvm::ARM::Q8, offsetof(FPRState, vreg.q[8])},
    {llvm::ARM::Q9, offsetof(FPRState, vreg.q[9])},
    {llvm::ARM::Q10, offsetof(FPRState, vreg.q[10])},
    {llvm::ARM::Q11, offsetof(FPRState, vreg.q[11])},
    {llvm::ARM::Q12, offsetof(FPRState, vreg.q[12])},
    {llvm::ARM::Q13, offsetof(FPRState, vreg.q[13])},
    {llvm::ARM::Q14, offsetof(FPRState, vreg.q[14])},
    {llvm::ARM::Q15, offsetof(FPRState, vreg.q[15])},
#else
    {llvm::ARM::Q8, -1},
    {llvm::ARM::Q9, -1},
    {llvm::ARM::Q10, -1},
    {llvm::ARM::Q11, -1},
    {llvm::ARM::Q12, -1},
    {llvm::ARM::Q13, -1},
    {llvm::ARM::Q14, -1},
    {llvm::ARM::Q15, -1},
#endif
};

const unsigned int size_GPR_ID = sizeof(GPR_ID) / sizeof(RegLLVM);
const unsigned int size_FLAG_ID = sizeof(FLAG_ID) / sizeof(RegLLVM);
const unsigned int size_SEG_ID = sizeof(SEG_ID) / sizeof(RegLLVM);

namespace {

constexpr uint16_t REGISTER_4BYTES[] = {
    llvm::ARM::R0,  llvm::ARM::R1,  llvm::ARM::R2,  llvm::ARM::R3,
    llvm::ARM::R4,  llvm::ARM::R5,  llvm::ARM::R6,  llvm::ARM::R7,
    llvm::ARM::R8,  llvm::ARM::R9,  llvm::ARM::R10, llvm::ARM::R11,
    llvm::ARM::R12, llvm::ARM::SP,  llvm::ARM::LR,  llvm::ARM::PC,

    llvm::ARM::S0,  llvm::ARM::S1,  llvm::ARM::S2,  llvm::ARM::S3,
    llvm::ARM::S4,  llvm::ARM::S5,  llvm::ARM::S6,  llvm::ARM::S7,
    llvm::ARM::S8,  llvm::ARM::S9,  llvm::ARM::S10, llvm::ARM::S11,
    llvm::ARM::S12, llvm::ARM::S13, llvm::ARM::S14, llvm::ARM::S15,
    llvm::ARM::S16, llvm::ARM::S17, llvm::ARM::S18, llvm::ARM::S19,
    llvm::ARM::S20, llvm::ARM::S21, llvm::ARM::S22, llvm::ARM::S23,
    llvm::ARM::S24, llvm::ARM::S25, llvm::ARM::S26, llvm::ARM::S27,
    llvm::ARM::S28, llvm::ARM::S29, llvm::ARM::S30, llvm::ARM::S31,
};

constexpr size_t REGISTER_4BYTES_SIZE =
    sizeof(REGISTER_4BYTES) / sizeof(uint16_t);

constexpr uint16_t REGISTER_4BYTES_P2[] = {
    llvm::ARM::R0_R1, llvm::ARM::R2_R3,   llvm::ARM::R4_R5,  llvm::ARM::R6_R7,
    llvm::ARM::R8_R9, llvm::ARM::R10_R11, llvm::ARM::R12_SP,
};

constexpr size_t REGISTER_4BYTES_P2_SIZE =
    sizeof(REGISTER_4BYTES_P2) / sizeof(uint16_t);

constexpr uint16_t REGISTER_8BYTES[] = {
    llvm::ARM::D0,  llvm::ARM::D1,  llvm::ARM::D2,  llvm::ARM::D3,
    llvm::ARM::D4,  llvm::ARM::D5,  llvm::ARM::D6,  llvm::ARM::D7,
    llvm::ARM::D8,  llvm::ARM::D9,  llvm::ARM::D10, llvm::ARM::D11,
    llvm::ARM::D12, llvm::ARM::D13, llvm::ARM::D14, llvm::ARM::D15,
    llvm::ARM::D16, llvm::ARM::D17, llvm::ARM::D18, llvm::ARM::D19,
    llvm::ARM::D20, llvm::ARM::D21, llvm::ARM::D22, llvm::ARM::D23,
    llvm::ARM::D24, llvm::ARM::D25, llvm::ARM::D26, llvm::ARM::D27,
    llvm::ARM::D28, llvm::ARM::D29, llvm::ARM::D30, llvm::ARM::D31,
};

constexpr size_t REGISTER_8BYTES_SIZE =
    sizeof(REGISTER_8BYTES) / sizeof(uint16_t);

constexpr uint16_t REGISTER_8BYTES_P2[] = {
    llvm::ARM::D1_D2,   llvm::ARM::D3_D4,   llvm::ARM::D5_D6,
    llvm::ARM::D7_D8,   llvm::ARM::D9_D10,  llvm::ARM::D11_D12,
    llvm::ARM::D13_D14, llvm::ARM::D15_D16, llvm::ARM::D17_D18,
    llvm::ARM::D19_D20, llvm::ARM::D21_D22, llvm::ARM::D23_D24,
    llvm::ARM::D25_D26, llvm::ARM::D27_D28, llvm::ARM::D29_D30,
};

constexpr size_t REGISTER_8BYTES_P2_SIZE =
    sizeof(REGISTER_8BYTES_P2) / sizeof(uint16_t);

constexpr uint16_t REGISTER_8BYTES_P2_S2[] = {
    llvm::ARM::D0_D2,   llvm::ARM::D1_D3,   llvm::ARM::D2_D4,
    llvm::ARM::D3_D5,   llvm::ARM::D4_D6,   llvm::ARM::D5_D7,
    llvm::ARM::D6_D8,   llvm::ARM::D7_D9,   llvm::ARM::D8_D10,
    llvm::ARM::D9_D11,  llvm::ARM::D10_D12, llvm::ARM::D11_D13,
    llvm::ARM::D12_D14, llvm::ARM::D13_D15, llvm::ARM::D14_D16,
    llvm::ARM::D15_D17, llvm::ARM::D16_D18, llvm::ARM::D17_D19,
    llvm::ARM::D18_D20, llvm::ARM::D19_D21, llvm::ARM::D20_D22,
    llvm::ARM::D21_D23, llvm::ARM::D22_D24, llvm::ARM::D23_D25,
    llvm::ARM::D24_D26, llvm::ARM::D25_D27, llvm::ARM::D26_D28,
    llvm::ARM::D27_D29, llvm::ARM::D28_D30, llvm::ARM::D29_D31,
};

constexpr size_t REGISTER_8BYTES_P2_S2_SIZE =
    sizeof(REGISTER_8BYTES_P2_S2) / sizeof(uint16_t);

constexpr uint16_t REGISTER_8BYTES_P3[] = {
    llvm::ARM::D0_D1_D2,    llvm::ARM::D1_D2_D3,    llvm::ARM::D2_D3_D4,
    llvm::ARM::D3_D4_D5,    llvm::ARM::D4_D5_D6,    llvm::ARM::D5_D6_D7,
    llvm::ARM::D6_D7_D8,    llvm::ARM::D7_D8_D9,    llvm::ARM::D8_D9_D10,
    llvm::ARM::D9_D10_D11,  llvm::ARM::D10_D11_D12, llvm::ARM::D11_D12_D13,
    llvm::ARM::D12_D13_D14, llvm::ARM::D13_D14_D15, llvm::ARM::D14_D15_D16,
    llvm::ARM::D15_D16_D17, llvm::ARM::D16_D17_D18, llvm::ARM::D17_D18_D19,
    llvm::ARM::D18_D19_D20, llvm::ARM::D19_D20_D21, llvm::ARM::D20_D21_D22,
    llvm::ARM::D21_D22_D23, llvm::ARM::D22_D23_D24, llvm::ARM::D23_D24_D25,
    llvm::ARM::D24_D25_D26, llvm::ARM::D25_D26_D27, llvm::ARM::D26_D27_D28,
    llvm::ARM::D27_D28_D29, llvm::ARM::D28_D29_D30, llvm::ARM::D29_D30_D31,
};

constexpr size_t REGISTER_8BYTES_P3_SIZE =
    sizeof(REGISTER_8BYTES_P3) / sizeof(uint16_t);

constexpr uint16_t REGISTER_8BYTES_P3_S2[] = {
    llvm::ARM::D0_D2_D4,    llvm::ARM::D1_D3_D5,    llvm::ARM::D2_D4_D6,
    llvm::ARM::D3_D5_D7,    llvm::ARM::D4_D6_D8,    llvm::ARM::D5_D7_D9,
    llvm::ARM::D6_D8_D10,   llvm::ARM::D7_D9_D11,   llvm::ARM::D8_D10_D12,
    llvm::ARM::D9_D11_D13,  llvm::ARM::D10_D12_D14, llvm::ARM::D11_D13_D15,
    llvm::ARM::D12_D14_D16, llvm::ARM::D13_D15_D17, llvm::ARM::D14_D16_D18,
    llvm::ARM::D15_D17_D19, llvm::ARM::D16_D18_D20, llvm::ARM::D17_D19_D21,
    llvm::ARM::D18_D20_D22, llvm::ARM::D19_D21_D23, llvm::ARM::D20_D22_D24,
    llvm::ARM::D21_D23_D25, llvm::ARM::D22_D24_D26, llvm::ARM::D23_D25_D27,
    llvm::ARM::D24_D26_D28, llvm::ARM::D25_D27_D29, llvm::ARM::D26_D28_D30,
    llvm::ARM::D27_D29_D31,
};

constexpr size_t REGISTER_8BYTES_P3_S2_SIZE =
    sizeof(REGISTER_8BYTES_P3_S2) / sizeof(uint16_t);

constexpr uint16_t REGISTER_8BYTES_P4[] = {
    llvm::ARM::D1_D2_D3_D4,     llvm::ARM::D3_D4_D5_D6,
    llvm::ARM::D5_D6_D7_D8,     llvm::ARM::D7_D8_D9_D10,
    llvm::ARM::D9_D10_D11_D12,  llvm::ARM::D11_D12_D13_D14,
    llvm::ARM::D13_D14_D15_D16, llvm::ARM::D15_D16_D17_D18,
    llvm::ARM::D17_D18_D19_D20, llvm::ARM::D19_D20_D21_D22,
    llvm::ARM::D21_D22_D23_D24, llvm::ARM::D23_D24_D25_D26,
    llvm::ARM::D25_D26_D27_D28, llvm::ARM::D27_D28_D29_D30,
};

constexpr size_t REGISTER_8BYTES_P4_SIZE =
    sizeof(REGISTER_8BYTES_P4) / sizeof(uint16_t);

constexpr uint16_t REGISTER_8BYTES_P4_S2[] = {
    llvm::ARM::D0_D2_D4_D6,     llvm::ARM::D1_D3_D5_D7,
    llvm::ARM::D2_D4_D6_D8,     llvm::ARM::D3_D5_D7_D9,
    llvm::ARM::D4_D6_D8_D10,    llvm::ARM::D5_D7_D9_D11,
    llvm::ARM::D6_D8_D10_D12,   llvm::ARM::D7_D9_D11_D13,
    llvm::ARM::D8_D10_D12_D14,  llvm::ARM::D9_D11_D13_D15,
    llvm::ARM::D10_D12_D14_D16, llvm::ARM::D11_D13_D15_D17,
    llvm::ARM::D12_D14_D16_D18, llvm::ARM::D13_D15_D17_D19,
    llvm::ARM::D14_D16_D18_D20, llvm::ARM::D15_D17_D19_D21,
    llvm::ARM::D16_D18_D20_D22, llvm::ARM::D17_D19_D21_D23,
    llvm::ARM::D18_D20_D22_D24, llvm::ARM::D19_D21_D23_D25,
    llvm::ARM::D20_D22_D24_D26, llvm::ARM::D21_D23_D25_D27,
    llvm::ARM::D22_D24_D26_D28, llvm::ARM::D23_D25_D27_D29,
    llvm::ARM::D24_D26_D28_D30, llvm::ARM::D25_D27_D29_D31,
};

constexpr size_t REGISTER_8BYTES_P4_S2_SIZE =
    sizeof(REGISTER_8BYTES_P4_S2) / sizeof(uint16_t);

constexpr uint16_t REGISTER_16BYTES[] = {
    llvm::ARM::Q0,  llvm::ARM::Q1,  llvm::ARM::Q2,  llvm::ARM::Q3,
    llvm::ARM::Q4,  llvm::ARM::Q5,  llvm::ARM::Q6,  llvm::ARM::Q7,
    llvm::ARM::Q8,  llvm::ARM::Q9,  llvm::ARM::Q10, llvm::ARM::Q11,
    llvm::ARM::Q12, llvm::ARM::Q13, llvm::ARM::Q14, llvm::ARM::Q15,
};

constexpr size_t REGISTER_16BYTES_SIZE =
    sizeof(REGISTER_16BYTES) / sizeof(uint16_t);

constexpr uint16_t REGISTER_16BYTES_P2[] = {
    llvm::ARM::Q0_Q1,   llvm::ARM::Q1_Q2,   llvm::ARM::Q2_Q3,
    llvm::ARM::Q3_Q4,   llvm::ARM::Q4_Q5,   llvm::ARM::Q5_Q6,
    llvm::ARM::Q6_Q7,   llvm::ARM::Q7_Q8,   llvm::ARM::Q8_Q9,
    llvm::ARM::Q9_Q10,  llvm::ARM::Q10_Q11, llvm::ARM::Q11_Q12,
    llvm::ARM::Q12_Q13, llvm::ARM::Q13_Q14, llvm::ARM::Q14_Q15,
};

constexpr size_t REGISTER_16BYTES_P2_SIZE =
    sizeof(REGISTER_16BYTES_P2) / sizeof(uint16_t);

constexpr uint16_t REGISTER_16BYTES_P4[] = {
    llvm::ARM::Q0_Q1_Q2_Q3,     llvm::ARM::Q1_Q2_Q3_Q4,
    llvm::ARM::Q2_Q3_Q4_Q5,     llvm::ARM::Q3_Q4_Q5_Q6,
    llvm::ARM::Q4_Q5_Q6_Q7,     llvm::ARM::Q5_Q6_Q7_Q8,
    llvm::ARM::Q6_Q7_Q8_Q9,     llvm::ARM::Q7_Q8_Q9_Q10,
    llvm::ARM::Q8_Q9_Q10_Q11,   llvm::ARM::Q9_Q10_Q11_Q12,
    llvm::ARM::Q10_Q11_Q12_Q13, llvm::ARM::Q11_Q12_Q13_Q14,
    llvm::ARM::Q12_Q13_Q14_Q15,
};

constexpr size_t REGISTER_16BYTES_P4_SIZE =
    sizeof(REGISTER_16BYTES_P4) / sizeof(uint16_t);

/* Encode base register on one byte
 * -1: invalid
 *  0 : R0 ... 12 : R12
 * 13 : SP
 * 14 : LR
 * 15 : PC
 * 16:  Q0 ... 31 : Q15
 * 32 : D0 ... 63 : D31
 * 64 : S0 ... 95 : S31
 */
constexpr int8_t getEncodedBaseReg(unsigned reg) {
  // verify llvm register enum
  static_assert((llvm::ARM::R12 - llvm::ARM::R0) == 12);
  static_assert((llvm::ARM::S31 - llvm::ARM::S0) == 31);
  static_assert((llvm::ARM::D31 - llvm::ARM::D0) == 31);
  static_assert((llvm::ARM::Q15 - llvm::ARM::Q0) == 15);
  static_assert((llvm::ARM::R12_SP - llvm::ARM::R0_R1) == 6);
  static_assert((llvm::ARM::D29_D30 - llvm::ARM::D1_D2) == 14);
  static_assert((llvm::ARM::D29_D31 - llvm::ARM::D0_D2) == 29);
  static_assert((llvm::ARM::D29_D30_D31 - llvm::ARM::D0_D1_D2) == 29);
  static_assert((llvm::ARM::D27_D29_D31 - llvm::ARM::D0_D2_D4) == 27);
  static_assert((llvm::ARM::D27_D28_D29_D30 - llvm::ARM::D1_D2_D3_D4) == 13);
  static_assert((llvm::ARM::D25_D27_D29_D31 - llvm::ARM::D0_D2_D4_D6) == 25);
  static_assert((llvm::ARM::Q14_Q15 - llvm::ARM::Q0_Q1) == 14);
  static_assert((llvm::ARM::Q12_Q13_Q14_Q15 - llvm::ARM::Q0_Q1_Q2_Q3) == 12);

  switch (reg) {
    case llvm::ARM::SP:
      return 13;
    case llvm::ARM::LR:
      return 14;
    case llvm::ARM::PC:
      return 15;
  }
  if (llvm::ARM::R0 <= reg and reg <= llvm::ARM::R12) {
    return reg - llvm::ARM::R0;
  } else if (llvm::ARM::S0 <= reg and reg <= llvm::ARM::S31) {
    return 64 + reg - llvm::ARM::S0;
  } else if (llvm::ARM::D0 <= reg and reg <= llvm::ARM::D31) {
    return 32 + reg - llvm::ARM::D0;
  } else if (llvm::ARM::Q0 <= reg and reg <= llvm::ARM::Q15) {
    return 16 + reg - llvm::ARM::Q0;
  } else if (llvm::ARM::R0_R1 <= reg and reg <= llvm::ARM::R12_SP) {
    return (reg - llvm::ARM::R0_R1) * 2;
  } else if (llvm::ARM::D1_D2 <= reg and reg <= llvm::ARM::D29_D30) {
    return 33 + (reg - llvm::ARM::D1_D2) * 2;
  } else if (llvm::ARM::D0_D2 <= reg and reg <= llvm::ARM::D29_D31) {
    return 32 + (reg - llvm::ARM::D0_D2);
  } else if (llvm::ARM::D0_D1_D2 <= reg and reg <= llvm::ARM::D29_D30_D31) {
    return 32 + (reg - llvm::ARM::D0_D1_D2);
  } else if (llvm::ARM::D0_D2_D4 <= reg and reg <= llvm::ARM::D27_D29_D31) {
    return 32 + (reg - llvm::ARM::D0_D2_D4);
  } else if (llvm::ARM::D1_D2_D3_D4 <= reg and
             reg <= llvm::ARM::D27_D28_D29_D30) {
    return 33 + (reg - llvm::ARM::D1_D2_D3_D4) * 2;
  } else if (llvm::ARM::D0_D2_D4_D6 <= reg and
             reg <= llvm::ARM::D25_D27_D29_D31) {
    return 32 + (reg - llvm::ARM::D0_D2_D4_D6);
  } else if (llvm::ARM::Q0_Q1 <= reg and reg <= llvm::ARM::Q14_Q15) {
    return 16 + (reg - llvm::ARM::Q0_Q1);
  } else if (llvm::ARM::Q0_Q1_Q2_Q3 <= reg and
             reg <= llvm::ARM::Q12_Q13_Q14_Q15) {
    return 16 + (reg - llvm::ARM::Q0_Q1_Q2_Q3);
  }
  return -1;
}

struct RegisterInfoArray {
  uint16_t sizeArr[llvm::ARM::NUM_TARGET_REGS] = {0};

  // Use to find the Higher register
  // -1: invalid
  //  0 : R0 ... 12 : R12
  // 13 : SP
  // 14 : LR
  // 15 : PC
  // 16:  Q0 ... 31 : Q15
  // 32 : D0 ... 63 : D31
  // 64 : S0 ... 95 : S31
  int8_t baseReg[llvm::ARM::NUM_TARGET_REGS] = {0};

  constexpr uint16_t setValue(uint8_t size, uint8_t packed, uint8_t spaced) {
    return (size & 0xff) | ((packed & 0xf) << 8) | ((spaced & 0xf) << 12);
  }

  constexpr RegisterInfoArray() {

    sizeArr[llvm::ARM::ITSTATE] = setValue(1, 1, 1);

    for (size_t i = 0; i < REGISTER_4BYTES_SIZE; i++) {
      sizeArr[REGISTER_4BYTES[i]] = setValue(4, 1, 1);
    }
    for (size_t i = 0; i < REGISTER_4BYTES_P2_SIZE; i++) {
      sizeArr[REGISTER_4BYTES_P2[i]] = setValue(4, 2, 1);
    }
    for (size_t i = 0; i < REGISTER_8BYTES_SIZE; i++) {
      sizeArr[REGISTER_8BYTES[i]] = setValue(8, 1, 1);
    }
    for (size_t i = 0; i < REGISTER_8BYTES_P2_SIZE; i++) {
      sizeArr[REGISTER_8BYTES_P2[i]] = setValue(8, 2, 1);
    }
    for (size_t i = 0; i < REGISTER_8BYTES_P2_S2_SIZE; i++) {
      sizeArr[REGISTER_8BYTES_P2_S2[i]] = setValue(8, 2, 2);
    }
    for (size_t i = 0; i < REGISTER_8BYTES_P3_SIZE; i++) {
      sizeArr[REGISTER_8BYTES_P3[i]] = setValue(8, 3, 1);
    }
    for (size_t i = 0; i < REGISTER_8BYTES_P3_S2_SIZE; i++) {
      sizeArr[REGISTER_8BYTES_P3_S2[i]] = setValue(8, 3, 2);
    }
    for (size_t i = 0; i < REGISTER_8BYTES_P4_SIZE; i++) {
      sizeArr[REGISTER_8BYTES_P4[i]] = setValue(8, 4, 1);
    }
    for (size_t i = 0; i < REGISTER_8BYTES_P4_S2_SIZE; i++) {
      sizeArr[REGISTER_8BYTES_P4_S2[i]] = setValue(8, 4, 2);
    }
    for (size_t i = 0; i < REGISTER_16BYTES_SIZE; i++) {
      sizeArr[REGISTER_16BYTES[i]] = setValue(16, 1, 1);
    }
    for (size_t i = 0; i < REGISTER_16BYTES_P2_SIZE; i++) {
      sizeArr[REGISTER_16BYTES_P2[i]] = setValue(16, 2, 1);
    }
    for (size_t i = 0; i < REGISTER_16BYTES_P4_SIZE; i++) {
      sizeArr[REGISTER_16BYTES_P4[i]] = setValue(16, 4, 1);
    }
    for (size_t i = 0; i < llvm::ARM::NUM_TARGET_REGS; i++) {
      baseReg[i] = getEncodedBaseReg(i);
    }
  }

  inline uint8_t getSize(RegLLVM reg_) const {
    unsigned reg = reg_.getValue();
    if (reg < llvm::ARM::NUM_TARGET_REGS)
      return sizeArr[reg] & 0xff;

    QBDI_ERROR("No register {}", reg);
    return 0;
  }

  inline uint8_t getPacked(RegLLVM reg_) const {
    unsigned reg = reg_.getValue();
    if (reg < llvm::ARM::NUM_TARGET_REGS)
      return (sizeArr[reg] >> 8) & 0xf;

    QBDI_ERROR("No register {}", reg);
    return 0;
  }

  inline uint8_t getSpaced(RegLLVM reg_) const {
    unsigned reg = reg_.getValue();
    if (reg < llvm::ARM::NUM_TARGET_REGS)
      return (sizeArr[reg] >> 12) & 0xf;

    QBDI_ERROR("No register {}", reg);
    return 0;
  }

  inline RegLLVM getUpperReg(RegLLVM reg_) const {
    unsigned reg = reg_.getValue();
    if (reg >= llvm::ARM::NUM_TARGET_REGS) {
      QBDI_ERROR("No register {}", reg);
      return llvm::ARM::NoRegister;
    }
    int8_t v = baseReg[reg];
    if (v < 0) {
      return llvm::ARM::NoRegister;
    } else if (v <= 12) {
      static_assert((llvm::ARM::R12 - llvm::ARM::R0) == 12);
      return llvm::ARM::R0 + v;
    } else if (v < 16) {
      switch (v) {
        case 13:
          return llvm::ARM::SP;
        case 14:
          return llvm::ARM::LR;
        case 15:
          return llvm::ARM::PC;
      }
    } else if (v < 32) {
      static_assert((llvm::ARM::Q15 - llvm::ARM::Q0) == 15);
      return llvm::ARM::Q0 + (v - 16);
    } else if (v < 64) {
      static_assert((llvm::ARM::D31 - llvm::ARM::D0) == 31);
      return llvm::ARM::D0 + (v - 32);
    } else if (v < 96) {
      static_assert((llvm::ARM::S31 - llvm::ARM::S0) == 31);
      return llvm::ARM::S0 + (v - 64);
    }
    // invalid positive value
    QBDI_ERROR("Wrong value {}", v);
    return llvm::ARM::NoRegister;
  }

  inline size_t getGPRPos(RegLLVM reg_) const {
    unsigned reg = reg_.getValue();
    if (reg >= llvm::ARM::NUM_TARGET_REGS) {
      QBDI_ERROR("No register {}", reg);
      return -1;
    }
    int8_t v = baseReg[reg];
    if (v < 0 || v > 15) {
      return -1;
    } else {
      return v;
    }
  }

  inline RegLLVM getUpperBasedRegister(RegLLVM reg) const {
    RegLLVM r = getUpperReg(reg);
    if (r == llvm::ARM::NoRegister) {
      return reg;
    }
    return r;
  }
};

constexpr RegisterInfoArray arrayInfo;

} // namespace

uint8_t getRegisterSize(RegLLVM reg) { return arrayInfo.getSize(reg); }

uint8_t getRegisterPacked(RegLLVM reg) { return arrayInfo.getPacked(reg); }

uint8_t getRegisterSpaced(RegLLVM reg) { return arrayInfo.getSpaced(reg); }

size_t getGPRPosition(RegLLVM reg) { return arrayInfo.getGPRPos(reg); }

RegLLVM getUpperRegister(RegLLVM reg, size_t pos) {
  unsigned r = getPackedRegister(reg, pos).getValue();
  if (llvm::ARM::S0 <= r and r <= llvm::ARM::S31) {
    return llvm::ARM::Q0 + ((r - llvm::ARM::S0) >> 2);
  } else if (llvm::ARM::D0 <= r and r <= llvm::ARM::D31) {
    return llvm::ARM::Q0 + ((r - llvm::ARM::D0) >> 1);
  } else {
    return r;
  }
}

RegLLVM getPackedRegister(RegLLVM reg, size_t pos) {
  if (pos == 0) {
    return arrayInfo.getUpperBasedRegister(reg);
  }
  if (pos >= getRegisterPacked(reg)) {
    return llvm::ARM::NoRegister;
  }
  unsigned r = arrayInfo.getUpperBasedRegister(reg).getValue();
  unsigned s = arrayInfo.getSpaced(reg) * pos;

  if (llvm::ARM::Q0 <= r and r <= llvm::ARM::Q15) {
    QBDI_REQUIRE_ABORT((r - llvm::ARM::Q0) + s <= 15, "Unexpected Qregister {}",
                       (r - llvm::ARM::Q0) + s);
    return r + s;
  } else if (llvm::ARM::D0 <= r and r <= llvm::ARM::D31) {
    QBDI_REQUIRE_ABORT((r - llvm::ARM::D0) + s <= 31, "Unexpected Dregister {}",
                       (r - llvm::ARM::D0) + s);
    return r + s;
  } else if (llvm::ARM::R0 <= r and r <= llvm::ARM::R12) {
    if ((r - llvm::ARM::R0) + s <= 12) {
      return r + s;
    } else if (r == llvm::ARM::R12 and s == 1) {
      // llvm::ARM::R12_SP
      return llvm::ARM::SP;
    } else {
      QBDI_ABORT("Unexpected register {}", reg.getValue());
    }
  } else {
    QBDI_ABORT("Unexpected register {}", reg.getValue());
  }
}

void fixLLVMUsedGPR(const llvm::MCInst &inst, const LLVMCPU &llvmcpu,
                    std::array<RegisterUsage, NUM_GPR> &arr,
                    std::map<RegLLVM, RegisterUsage> &m) {
  switch (inst.getOpcode()) {
    case llvm::ARM::BX_pred:
      arr[REG_PC] |= RegisterSet;
      break;
    case llvm::ARM::BX_RET:
      arr[REG_LR] |= RegisterUsed;
      arr[REG_PC] |= RegisterSet;
      break;
    default:
      break;
  }

  // CSPR register
  if (m.count(Reg(REG_FLAG)) != 0) {
    // when the instruction set the flags, we may mark the operand as a
    // RegisterUsed look to the operandInfo and correct if this the case

    const llvm::MCInstrInfo &MCII = llvmcpu.getMCII();
    const llvm::MCInstrDesc &desc = MCII.get(inst.getOpcode());
    QBDI_REQUIRE_ABORT(desc.getNumOperands() <= inst.getNumOperands(),
                       "Unexpected operands number");

    for (unsigned int opn = 0; opn < desc.getNumOperands(); opn++) {
      const llvm::MCOperandInfo &opInfo = desc.operands()[opn];
      if (opInfo.RegClass == llvm::ARM::CCRRegClassID) {
        // found the position of CSPR set operand
        const llvm::MCOperand &op = inst.getOperand(opn);
        QBDI_REQUIRE_ABORT(op.isReg(), "Unexpected operand type");

        if (op.getReg() != llvm::ARM::NoRegister) {
          // The instruction set the flags, recompute the CSPR flags
          m[Reg(REG_FLAG)] = RegisterSet;
          if (getCondition(inst, llvmcpu) != llvm::ARMCC::AL) {
            // the instruction is conditionnal
            m[Reg(REG_FLAG)] |= RegisterUsed;
          }
        }
        break;
      }
    }
  }

  return;
}

} // namespace QBDI
