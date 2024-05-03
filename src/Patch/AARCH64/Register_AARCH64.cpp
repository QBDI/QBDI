/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2024 Quarkslab
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
#include <stdint.h>

#include "AArch64InstrInfo.h"

#include "Patch/Register.h"
#include "Patch/Types.h"
#include "Utility/LogSys.h"

#include "QBDI/State.h"

namespace QBDI {

const RegLLVM GPR_ID[] = {
    llvm::AArch64::X0,  llvm::AArch64::X1,  llvm::AArch64::X2,
    llvm::AArch64::X3,  llvm::AArch64::X4,  llvm::AArch64::X5,
    llvm::AArch64::X6,  llvm::AArch64::X7,  llvm::AArch64::X8,
    llvm::AArch64::X9,  llvm::AArch64::X10, llvm::AArch64::X11,
    llvm::AArch64::X12, llvm::AArch64::X13, llvm::AArch64::X14,
    llvm::AArch64::X15, llvm::AArch64::X16, llvm::AArch64::X17,
    llvm::AArch64::X18, llvm::AArch64::X19, llvm::AArch64::X20,
    llvm::AArch64::X21, llvm::AArch64::X22, llvm::AArch64::X23,
    llvm::AArch64::X24, llvm::AArch64::X25, llvm::AArch64::X26,
    llvm::AArch64::X27, llvm::AArch64::X28, llvm::AArch64::FP,
    llvm::AArch64::LR,  llvm::AArch64::SP,  llvm::AArch64::NZCV,
};

const RegLLVM FLAG_ID[] = {};

const RegLLVM SEG_ID[] = {
    // XZR doesn't have an index in GPR or FPR
    // The instAnalysis will use OPERAND_SEG when XZR is used
    llvm::AArch64::XZR, llvm::AArch64::WZR};

const std::map<RegLLVM, int16_t> FPR_ID = {
    // size 1b
    {llvm::AArch64::B0, offsetof(FPRState, v0)},
    {llvm::AArch64::B1, offsetof(FPRState, v1)},
    {llvm::AArch64::B2, offsetof(FPRState, v2)},
    {llvm::AArch64::B3, offsetof(FPRState, v3)},
    {llvm::AArch64::B4, offsetof(FPRState, v4)},
    {llvm::AArch64::B5, offsetof(FPRState, v5)},
    {llvm::AArch64::B6, offsetof(FPRState, v6)},
    {llvm::AArch64::B7, offsetof(FPRState, v7)},
    {llvm::AArch64::B8, offsetof(FPRState, v8)},
    {llvm::AArch64::B9, offsetof(FPRState, v9)},
    {llvm::AArch64::B10, offsetof(FPRState, v10)},
    {llvm::AArch64::B11, offsetof(FPRState, v11)},
    {llvm::AArch64::B12, offsetof(FPRState, v12)},
    {llvm::AArch64::B13, offsetof(FPRState, v13)},
    {llvm::AArch64::B14, offsetof(FPRState, v14)},
    {llvm::AArch64::B15, offsetof(FPRState, v15)},
    {llvm::AArch64::B16, offsetof(FPRState, v16)},
    {llvm::AArch64::B17, offsetof(FPRState, v17)},
    {llvm::AArch64::B18, offsetof(FPRState, v18)},
    {llvm::AArch64::B19, offsetof(FPRState, v19)},
    {llvm::AArch64::B20, offsetof(FPRState, v20)},
    {llvm::AArch64::B21, offsetof(FPRState, v21)},
    {llvm::AArch64::B22, offsetof(FPRState, v22)},
    {llvm::AArch64::B23, offsetof(FPRState, v23)},
    {llvm::AArch64::B24, offsetof(FPRState, v24)},
    {llvm::AArch64::B25, offsetof(FPRState, v25)},
    {llvm::AArch64::B26, offsetof(FPRState, v26)},
    {llvm::AArch64::B27, offsetof(FPRState, v27)},
    {llvm::AArch64::B28, offsetof(FPRState, v28)},
    {llvm::AArch64::B29, offsetof(FPRState, v29)},
    {llvm::AArch64::B30, offsetof(FPRState, v30)},
    {llvm::AArch64::B31, offsetof(FPRState, v31)},

    // size 2b
    {llvm::AArch64::H0, offsetof(FPRState, v0)},
    {llvm::AArch64::H1, offsetof(FPRState, v1)},
    {llvm::AArch64::H2, offsetof(FPRState, v2)},
    {llvm::AArch64::H3, offsetof(FPRState, v3)},
    {llvm::AArch64::H4, offsetof(FPRState, v4)},
    {llvm::AArch64::H5, offsetof(FPRState, v5)},
    {llvm::AArch64::H6, offsetof(FPRState, v6)},
    {llvm::AArch64::H7, offsetof(FPRState, v7)},
    {llvm::AArch64::H8, offsetof(FPRState, v8)},
    {llvm::AArch64::H9, offsetof(FPRState, v9)},
    {llvm::AArch64::H10, offsetof(FPRState, v10)},
    {llvm::AArch64::H11, offsetof(FPRState, v11)},
    {llvm::AArch64::H12, offsetof(FPRState, v12)},
    {llvm::AArch64::H13, offsetof(FPRState, v13)},
    {llvm::AArch64::H14, offsetof(FPRState, v14)},
    {llvm::AArch64::H15, offsetof(FPRState, v15)},
    {llvm::AArch64::H16, offsetof(FPRState, v16)},
    {llvm::AArch64::H17, offsetof(FPRState, v17)},
    {llvm::AArch64::H18, offsetof(FPRState, v18)},
    {llvm::AArch64::H19, offsetof(FPRState, v19)},
    {llvm::AArch64::H20, offsetof(FPRState, v20)},
    {llvm::AArch64::H21, offsetof(FPRState, v21)},
    {llvm::AArch64::H22, offsetof(FPRState, v22)},
    {llvm::AArch64::H23, offsetof(FPRState, v23)},
    {llvm::AArch64::H24, offsetof(FPRState, v24)},
    {llvm::AArch64::H25, offsetof(FPRState, v25)},
    {llvm::AArch64::H26, offsetof(FPRState, v26)},
    {llvm::AArch64::H27, offsetof(FPRState, v27)},
    {llvm::AArch64::H28, offsetof(FPRState, v28)},
    {llvm::AArch64::H29, offsetof(FPRState, v29)},
    {llvm::AArch64::H30, offsetof(FPRState, v30)},
    {llvm::AArch64::H31, offsetof(FPRState, v31)},

    // size 4b
    {llvm::AArch64::S0, offsetof(FPRState, v0)},
    {llvm::AArch64::S1, offsetof(FPRState, v1)},
    {llvm::AArch64::S2, offsetof(FPRState, v2)},
    {llvm::AArch64::S3, offsetof(FPRState, v3)},
    {llvm::AArch64::S4, offsetof(FPRState, v4)},
    {llvm::AArch64::S5, offsetof(FPRState, v5)},
    {llvm::AArch64::S6, offsetof(FPRState, v6)},
    {llvm::AArch64::S7, offsetof(FPRState, v7)},
    {llvm::AArch64::S8, offsetof(FPRState, v8)},
    {llvm::AArch64::S9, offsetof(FPRState, v9)},
    {llvm::AArch64::S10, offsetof(FPRState, v10)},
    {llvm::AArch64::S11, offsetof(FPRState, v11)},
    {llvm::AArch64::S12, offsetof(FPRState, v12)},
    {llvm::AArch64::S13, offsetof(FPRState, v13)},
    {llvm::AArch64::S14, offsetof(FPRState, v14)},
    {llvm::AArch64::S15, offsetof(FPRState, v15)},
    {llvm::AArch64::S16, offsetof(FPRState, v16)},
    {llvm::AArch64::S17, offsetof(FPRState, v17)},
    {llvm::AArch64::S18, offsetof(FPRState, v18)},
    {llvm::AArch64::S19, offsetof(FPRState, v19)},
    {llvm::AArch64::S20, offsetof(FPRState, v20)},
    {llvm::AArch64::S21, offsetof(FPRState, v21)},
    {llvm::AArch64::S22, offsetof(FPRState, v22)},
    {llvm::AArch64::S23, offsetof(FPRState, v23)},
    {llvm::AArch64::S24, offsetof(FPRState, v24)},
    {llvm::AArch64::S25, offsetof(FPRState, v25)},
    {llvm::AArch64::S26, offsetof(FPRState, v26)},
    {llvm::AArch64::S27, offsetof(FPRState, v27)},
    {llvm::AArch64::S28, offsetof(FPRState, v28)},
    {llvm::AArch64::S29, offsetof(FPRState, v29)},
    {llvm::AArch64::S30, offsetof(FPRState, v30)},
    {llvm::AArch64::S31, offsetof(FPRState, v31)},

    // size 8b
    {llvm::AArch64::D0, offsetof(FPRState, v0)},
    {llvm::AArch64::D1, offsetof(FPRState, v1)},
    {llvm::AArch64::D2, offsetof(FPRState, v2)},
    {llvm::AArch64::D3, offsetof(FPRState, v3)},
    {llvm::AArch64::D4, offsetof(FPRState, v4)},
    {llvm::AArch64::D5, offsetof(FPRState, v5)},
    {llvm::AArch64::D6, offsetof(FPRState, v6)},
    {llvm::AArch64::D7, offsetof(FPRState, v7)},
    {llvm::AArch64::D8, offsetof(FPRState, v8)},
    {llvm::AArch64::D9, offsetof(FPRState, v9)},
    {llvm::AArch64::D10, offsetof(FPRState, v10)},
    {llvm::AArch64::D11, offsetof(FPRState, v11)},
    {llvm::AArch64::D12, offsetof(FPRState, v12)},
    {llvm::AArch64::D13, offsetof(FPRState, v13)},
    {llvm::AArch64::D14, offsetof(FPRState, v14)},
    {llvm::AArch64::D15, offsetof(FPRState, v15)},
    {llvm::AArch64::D16, offsetof(FPRState, v16)},
    {llvm::AArch64::D17, offsetof(FPRState, v17)},
    {llvm::AArch64::D18, offsetof(FPRState, v18)},
    {llvm::AArch64::D19, offsetof(FPRState, v19)},
    {llvm::AArch64::D20, offsetof(FPRState, v20)},
    {llvm::AArch64::D21, offsetof(FPRState, v21)},
    {llvm::AArch64::D22, offsetof(FPRState, v22)},
    {llvm::AArch64::D23, offsetof(FPRState, v23)},
    {llvm::AArch64::D24, offsetof(FPRState, v24)},
    {llvm::AArch64::D25, offsetof(FPRState, v25)},
    {llvm::AArch64::D26, offsetof(FPRState, v26)},
    {llvm::AArch64::D27, offsetof(FPRState, v27)},
    {llvm::AArch64::D28, offsetof(FPRState, v28)},
    {llvm::AArch64::D29, offsetof(FPRState, v29)},
    {llvm::AArch64::D30, offsetof(FPRState, v30)},
    {llvm::AArch64::D31, offsetof(FPRState, v31)},
    {llvm::AArch64::FPCR, offsetof(FPRState, fpcr)},

    // size 16b
    {llvm::AArch64::Q0, offsetof(FPRState, v0)},
    {llvm::AArch64::Q1, offsetof(FPRState, v1)},
    {llvm::AArch64::Q2, offsetof(FPRState, v2)},
    {llvm::AArch64::Q3, offsetof(FPRState, v3)},
    {llvm::AArch64::Q4, offsetof(FPRState, v4)},
    {llvm::AArch64::Q5, offsetof(FPRState, v5)},
    {llvm::AArch64::Q6, offsetof(FPRState, v6)},
    {llvm::AArch64::Q7, offsetof(FPRState, v7)},
    {llvm::AArch64::Q8, offsetof(FPRState, v8)},
    {llvm::AArch64::Q9, offsetof(FPRState, v9)},
    {llvm::AArch64::Q10, offsetof(FPRState, v10)},
    {llvm::AArch64::Q11, offsetof(FPRState, v11)},
    {llvm::AArch64::Q12, offsetof(FPRState, v12)},
    {llvm::AArch64::Q13, offsetof(FPRState, v13)},
    {llvm::AArch64::Q14, offsetof(FPRState, v14)},
    {llvm::AArch64::Q15, offsetof(FPRState, v15)},
    {llvm::AArch64::Q16, offsetof(FPRState, v16)},
    {llvm::AArch64::Q17, offsetof(FPRState, v17)},
    {llvm::AArch64::Q18, offsetof(FPRState, v18)},
    {llvm::AArch64::Q19, offsetof(FPRState, v19)},
    {llvm::AArch64::Q20, offsetof(FPRState, v20)},
    {llvm::AArch64::Q21, offsetof(FPRState, v21)},
    {llvm::AArch64::Q22, offsetof(FPRState, v22)},
    {llvm::AArch64::Q23, offsetof(FPRState, v23)},
    {llvm::AArch64::Q24, offsetof(FPRState, v24)},
    {llvm::AArch64::Q25, offsetof(FPRState, v25)},
    {llvm::AArch64::Q26, offsetof(FPRState, v26)},
    {llvm::AArch64::Q27, offsetof(FPRState, v27)},
    {llvm::AArch64::Q28, offsetof(FPRState, v28)},
    {llvm::AArch64::Q29, offsetof(FPRState, v29)},
    {llvm::AArch64::Q30, offsetof(FPRState, v30)},
    {llvm::AArch64::Q31, offsetof(FPRState, v31)},
};

const unsigned int size_GPR_ID = sizeof(GPR_ID) / sizeof(RegLLVM);
const unsigned int size_FLAG_ID = sizeof(FLAG_ID) / sizeof(RegLLVM);
const unsigned int size_SEG_ID = sizeof(SEG_ID) / sizeof(RegLLVM);

namespace {

constexpr uint16_t REGISTER_1BYTE[] = {
    llvm::AArch64::B0,  llvm::AArch64::B1,  llvm::AArch64::B2,
    llvm::AArch64::B3,  llvm::AArch64::B4,  llvm::AArch64::B5,
    llvm::AArch64::B6,  llvm::AArch64::B7,  llvm::AArch64::B8,
    llvm::AArch64::B9,  llvm::AArch64::B10, llvm::AArch64::B11,
    llvm::AArch64::B12, llvm::AArch64::B13, llvm::AArch64::B14,
    llvm::AArch64::B15, llvm::AArch64::B16, llvm::AArch64::B17,
    llvm::AArch64::B18, llvm::AArch64::B19, llvm::AArch64::B20,
    llvm::AArch64::B21, llvm::AArch64::B22, llvm::AArch64::B23,
    llvm::AArch64::B24, llvm::AArch64::B25, llvm::AArch64::B26,
    llvm::AArch64::B27, llvm::AArch64::B28, llvm::AArch64::B29,
    llvm::AArch64::B30, llvm::AArch64::B31,
};

constexpr size_t REGISTER_1BYTE_SIZE =
    sizeof(REGISTER_1BYTE) / sizeof(uint16_t);

constexpr uint16_t REGISTER_2BYTES[] = {
    llvm::AArch64::H0,  llvm::AArch64::H1,  llvm::AArch64::H2,
    llvm::AArch64::H3,  llvm::AArch64::H4,  llvm::AArch64::H5,
    llvm::AArch64::H6,  llvm::AArch64::H7,  llvm::AArch64::H8,
    llvm::AArch64::H9,  llvm::AArch64::H10, llvm::AArch64::H11,
    llvm::AArch64::H12, llvm::AArch64::H13, llvm::AArch64::H14,
    llvm::AArch64::H15, llvm::AArch64::H16, llvm::AArch64::H17,
    llvm::AArch64::H18, llvm::AArch64::H19, llvm::AArch64::H20,
    llvm::AArch64::H21, llvm::AArch64::H22, llvm::AArch64::H23,
    llvm::AArch64::H24, llvm::AArch64::H25, llvm::AArch64::H26,
    llvm::AArch64::H27, llvm::AArch64::H28, llvm::AArch64::H29,
    llvm::AArch64::H30, llvm::AArch64::H31,
};

constexpr size_t REGISTER_2BYTES_SIZE =
    sizeof(REGISTER_2BYTES) / sizeof(uint16_t);

constexpr uint16_t REGISTER_4BYTES[] = {
    llvm::AArch64::W0,  llvm::AArch64::W1,  llvm::AArch64::W2,
    llvm::AArch64::W3,  llvm::AArch64::W4,  llvm::AArch64::W5,
    llvm::AArch64::W6,  llvm::AArch64::W7,  llvm::AArch64::W8,
    llvm::AArch64::W9,  llvm::AArch64::W10, llvm::AArch64::W11,
    llvm::AArch64::W12, llvm::AArch64::W13, llvm::AArch64::W14,
    llvm::AArch64::W15, llvm::AArch64::W16, llvm::AArch64::W17,
    llvm::AArch64::W18, llvm::AArch64::W19, llvm::AArch64::W20,
    llvm::AArch64::W21, llvm::AArch64::W22, llvm::AArch64::W23,
    llvm::AArch64::W24, llvm::AArch64::W25, llvm::AArch64::W26,
    llvm::AArch64::W27, llvm::AArch64::W28, llvm::AArch64::W29,
    llvm::AArch64::W30, llvm::AArch64::WSP, llvm::AArch64::WZR,

    llvm::AArch64::S0,  llvm::AArch64::S1,  llvm::AArch64::S2,
    llvm::AArch64::S3,  llvm::AArch64::S4,  llvm::AArch64::S5,
    llvm::AArch64::S6,  llvm::AArch64::S7,  llvm::AArch64::S8,
    llvm::AArch64::S9,  llvm::AArch64::S10, llvm::AArch64::S11,
    llvm::AArch64::S12, llvm::AArch64::S13, llvm::AArch64::S14,
    llvm::AArch64::S15, llvm::AArch64::S16, llvm::AArch64::S17,
    llvm::AArch64::S18, llvm::AArch64::S19, llvm::AArch64::S20,
    llvm::AArch64::S21, llvm::AArch64::S22, llvm::AArch64::S23,
    llvm::AArch64::S24, llvm::AArch64::S25, llvm::AArch64::S26,
    llvm::AArch64::S27, llvm::AArch64::S28, llvm::AArch64::S29,
    llvm::AArch64::S30, llvm::AArch64::S31,
};

constexpr size_t REGISTER_4BYTES_SIZE =
    sizeof(REGISTER_4BYTES) / sizeof(uint16_t);

constexpr uint16_t REGISTER_4BYTES_P2[] = {
    llvm::AArch64::W0_W1,   llvm::AArch64::W2_W3,   llvm::AArch64::W4_W5,
    llvm::AArch64::W6_W7,   llvm::AArch64::W8_W9,   llvm::AArch64::W10_W11,
    llvm::AArch64::W12_W13, llvm::AArch64::W14_W15, llvm::AArch64::W16_W17,
    llvm::AArch64::W18_W19, llvm::AArch64::W20_W21, llvm::AArch64::W22_W23,
    llvm::AArch64::W24_W25, llvm::AArch64::W26_W27, llvm::AArch64::W28_W29,
    llvm::AArch64::W30_WZR,
};

constexpr size_t REGISTER_4BYTES_P2_SIZE =
    sizeof(REGISTER_4BYTES_P2) / sizeof(uint16_t);

constexpr uint16_t REGISTER_8BYTES[] = {
    llvm::AArch64::X0,  llvm::AArch64::X1,  llvm::AArch64::X2,
    llvm::AArch64::X3,  llvm::AArch64::X4,  llvm::AArch64::X5,
    llvm::AArch64::X6,  llvm::AArch64::X7,  llvm::AArch64::X8,
    llvm::AArch64::X9,  llvm::AArch64::X10, llvm::AArch64::X11,
    llvm::AArch64::X12, llvm::AArch64::X13, llvm::AArch64::X14,
    llvm::AArch64::X15, llvm::AArch64::X16, llvm::AArch64::X17,
    llvm::AArch64::X18, llvm::AArch64::X19, llvm::AArch64::X20,
    llvm::AArch64::X21, llvm::AArch64::X22, llvm::AArch64::X23,
    llvm::AArch64::X24, llvm::AArch64::X25, llvm::AArch64::X26,
    llvm::AArch64::X27, llvm::AArch64::X28, llvm::AArch64::FP,
    llvm::AArch64::LR,  llvm::AArch64::SP,  llvm::AArch64::XZR,

    llvm::AArch64::D0,  llvm::AArch64::D1,  llvm::AArch64::D2,
    llvm::AArch64::D3,  llvm::AArch64::D4,  llvm::AArch64::D5,
    llvm::AArch64::D6,  llvm::AArch64::D7,  llvm::AArch64::D8,
    llvm::AArch64::D9,  llvm::AArch64::D10, llvm::AArch64::D11,
    llvm::AArch64::D12, llvm::AArch64::D13, llvm::AArch64::D14,
    llvm::AArch64::D15, llvm::AArch64::D16, llvm::AArch64::D17,
    llvm::AArch64::D18, llvm::AArch64::D19, llvm::AArch64::D20,
    llvm::AArch64::D21, llvm::AArch64::D22, llvm::AArch64::D23,
    llvm::AArch64::D24, llvm::AArch64::D25, llvm::AArch64::D26,
    llvm::AArch64::D27, llvm::AArch64::D28, llvm::AArch64::D29,
    llvm::AArch64::D30, llvm::AArch64::D31, llvm::AArch64::FPCR,
};

constexpr size_t REGISTER_8BYTES_SIZE =
    sizeof(REGISTER_8BYTES) / sizeof(uint16_t);

constexpr uint16_t REGISTER_8BYTES_P2[] = {
    llvm::AArch64::D0_D1,   llvm::AArch64::D1_D2,   llvm::AArch64::D2_D3,
    llvm::AArch64::D3_D4,   llvm::AArch64::D4_D5,   llvm::AArch64::D5_D6,
    llvm::AArch64::D6_D7,   llvm::AArch64::D7_D8,   llvm::AArch64::D8_D9,
    llvm::AArch64::D9_D10,  llvm::AArch64::D10_D11, llvm::AArch64::D11_D12,
    llvm::AArch64::D12_D13, llvm::AArch64::D13_D14, llvm::AArch64::D14_D15,
    llvm::AArch64::D15_D16, llvm::AArch64::D16_D17, llvm::AArch64::D17_D18,
    llvm::AArch64::D18_D19, llvm::AArch64::D19_D20, llvm::AArch64::D20_D21,
    llvm::AArch64::D21_D22, llvm::AArch64::D22_D23, llvm::AArch64::D23_D24,
    llvm::AArch64::D24_D25, llvm::AArch64::D25_D26, llvm::AArch64::D26_D27,
    llvm::AArch64::D27_D28, llvm::AArch64::D28_D29, llvm::AArch64::D29_D30,
    llvm::AArch64::D30_D31, llvm::AArch64::D31_D0,

    llvm::AArch64::X0_X1,   llvm::AArch64::X2_X3,   llvm::AArch64::X4_X5,
    llvm::AArch64::X6_X7,   llvm::AArch64::X8_X9,   llvm::AArch64::X10_X11,
    llvm::AArch64::X12_X13, llvm::AArch64::X14_X15, llvm::AArch64::X16_X17,
    llvm::AArch64::X18_X19, llvm::AArch64::X20_X21, llvm::AArch64::X22_X23,
    llvm::AArch64::X24_X25, llvm::AArch64::X26_X27, llvm::AArch64::X28_FP,
    llvm::AArch64::LR_XZR,
};

constexpr size_t REGISTER_8BYTES_P2_SIZE =
    sizeof(REGISTER_8BYTES_P2) / sizeof(uint16_t);

constexpr uint16_t REGISTER_8BYTES_P3[] = {
    llvm::AArch64::D0_D1_D2,    llvm::AArch64::D1_D2_D3,
    llvm::AArch64::D2_D3_D4,    llvm::AArch64::D3_D4_D5,
    llvm::AArch64::D4_D5_D6,    llvm::AArch64::D5_D6_D7,
    llvm::AArch64::D6_D7_D8,    llvm::AArch64::D7_D8_D9,
    llvm::AArch64::D8_D9_D10,   llvm::AArch64::D9_D10_D11,
    llvm::AArch64::D10_D11_D12, llvm::AArch64::D11_D12_D13,
    llvm::AArch64::D12_D13_D14, llvm::AArch64::D13_D14_D15,
    llvm::AArch64::D14_D15_D16, llvm::AArch64::D15_D16_D17,
    llvm::AArch64::D16_D17_D18, llvm::AArch64::D17_D18_D19,
    llvm::AArch64::D18_D19_D20, llvm::AArch64::D19_D20_D21,
    llvm::AArch64::D20_D21_D22, llvm::AArch64::D21_D22_D23,
    llvm::AArch64::D22_D23_D24, llvm::AArch64::D23_D24_D25,
    llvm::AArch64::D24_D25_D26, llvm::AArch64::D25_D26_D27,
    llvm::AArch64::D26_D27_D28, llvm::AArch64::D27_D28_D29,
    llvm::AArch64::D28_D29_D30, llvm::AArch64::D29_D30_D31,
    llvm::AArch64::D30_D31_D0,  llvm::AArch64::D31_D0_D1,
};

constexpr size_t REGISTER_8BYTES_P3_SIZE =
    sizeof(REGISTER_8BYTES_P3) / sizeof(uint16_t);

constexpr uint16_t REGISTER_8BYTES_P4[] = {
    llvm::AArch64::D0_D1_D2_D3,     llvm::AArch64::D1_D2_D3_D4,
    llvm::AArch64::D2_D3_D4_D5,     llvm::AArch64::D3_D4_D5_D6,
    llvm::AArch64::D4_D5_D6_D7,     llvm::AArch64::D5_D6_D7_D8,
    llvm::AArch64::D6_D7_D8_D9,     llvm::AArch64::D7_D8_D9_D10,
    llvm::AArch64::D8_D9_D10_D11,   llvm::AArch64::D9_D10_D11_D12,
    llvm::AArch64::D10_D11_D12_D13, llvm::AArch64::D11_D12_D13_D14,
    llvm::AArch64::D12_D13_D14_D15, llvm::AArch64::D13_D14_D15_D16,
    llvm::AArch64::D14_D15_D16_D17, llvm::AArch64::D15_D16_D17_D18,
    llvm::AArch64::D16_D17_D18_D19, llvm::AArch64::D17_D18_D19_D20,
    llvm::AArch64::D18_D19_D20_D21, llvm::AArch64::D19_D20_D21_D22,
    llvm::AArch64::D20_D21_D22_D23, llvm::AArch64::D21_D22_D23_D24,
    llvm::AArch64::D22_D23_D24_D25, llvm::AArch64::D23_D24_D25_D26,
    llvm::AArch64::D24_D25_D26_D27, llvm::AArch64::D25_D26_D27_D28,
    llvm::AArch64::D26_D27_D28_D29, llvm::AArch64::D27_D28_D29_D30,
    llvm::AArch64::D28_D29_D30_D31, llvm::AArch64::D29_D30_D31_D0,
    llvm::AArch64::D30_D31_D0_D1,   llvm::AArch64::D31_D0_D1_D2,
};

constexpr size_t REGISTER_8BYTES_P4_SIZE =
    sizeof(REGISTER_8BYTES_P4) / sizeof(uint16_t);

constexpr uint16_t REGISTER_8BYTES_P8[] = {
    llvm::AArch64::X22_X23_X24_X25_X26_X27_X28_FP,
    llvm::AArch64::X0_X1_X2_X3_X4_X5_X6_X7,
    llvm::AArch64::X2_X3_X4_X5_X6_X7_X8_X9,
    llvm::AArch64::X4_X5_X6_X7_X8_X9_X10_X11,
    llvm::AArch64::X6_X7_X8_X9_X10_X11_X12_X13,
    llvm::AArch64::X8_X9_X10_X11_X12_X13_X14_X15,
    llvm::AArch64::X10_X11_X12_X13_X14_X15_X16_X17,
    llvm::AArch64::X12_X13_X14_X15_X16_X17_X18_X19,
    llvm::AArch64::X14_X15_X16_X17_X18_X19_X20_X21,
    llvm::AArch64::X16_X17_X18_X19_X20_X21_X22_X23,
    llvm::AArch64::X18_X19_X20_X21_X22_X23_X24_X25,
    llvm::AArch64::X20_X21_X22_X23_X24_X25_X26_X27,
};

constexpr size_t REGISTER_8BYTES_P8_SIZE =
    sizeof(REGISTER_8BYTES_P8) / sizeof(uint16_t);

constexpr uint16_t REGISTER_16BYTES[] = {
    llvm::AArch64::Q0,  llvm::AArch64::Q1,  llvm::AArch64::Q2,
    llvm::AArch64::Q3,  llvm::AArch64::Q4,  llvm::AArch64::Q5,
    llvm::AArch64::Q6,  llvm::AArch64::Q7,  llvm::AArch64::Q8,
    llvm::AArch64::Q9,  llvm::AArch64::Q10, llvm::AArch64::Q11,
    llvm::AArch64::Q12, llvm::AArch64::Q13, llvm::AArch64::Q14,
    llvm::AArch64::Q15, llvm::AArch64::Q16, llvm::AArch64::Q17,
    llvm::AArch64::Q18, llvm::AArch64::Q19, llvm::AArch64::Q20,
    llvm::AArch64::Q21, llvm::AArch64::Q22, llvm::AArch64::Q23,
    llvm::AArch64::Q24, llvm::AArch64::Q25, llvm::AArch64::Q26,
    llvm::AArch64::Q27, llvm::AArch64::Q28, llvm::AArch64::Q29,
    llvm::AArch64::Q30, llvm::AArch64::Q31,
};

constexpr size_t REGISTER_16BYTES_SIZE =
    sizeof(REGISTER_16BYTES) / sizeof(uint16_t);

constexpr uint16_t REGISTER_16BYTES_P2[] = {
    llvm::AArch64::Q0_Q1,   llvm::AArch64::Q1_Q2,   llvm::AArch64::Q2_Q3,
    llvm::AArch64::Q3_Q4,   llvm::AArch64::Q4_Q5,   llvm::AArch64::Q5_Q6,
    llvm::AArch64::Q6_Q7,   llvm::AArch64::Q7_Q8,   llvm::AArch64::Q8_Q9,
    llvm::AArch64::Q9_Q10,  llvm::AArch64::Q10_Q11, llvm::AArch64::Q11_Q12,
    llvm::AArch64::Q12_Q13, llvm::AArch64::Q13_Q14, llvm::AArch64::Q14_Q15,
    llvm::AArch64::Q15_Q16, llvm::AArch64::Q16_Q17, llvm::AArch64::Q17_Q18,
    llvm::AArch64::Q18_Q19, llvm::AArch64::Q19_Q20, llvm::AArch64::Q20_Q21,
    llvm::AArch64::Q21_Q22, llvm::AArch64::Q22_Q23, llvm::AArch64::Q23_Q24,
    llvm::AArch64::Q24_Q25, llvm::AArch64::Q25_Q26, llvm::AArch64::Q26_Q27,
    llvm::AArch64::Q27_Q28, llvm::AArch64::Q28_Q29, llvm::AArch64::Q29_Q30,
    llvm::AArch64::Q30_Q31, llvm::AArch64::Q31_Q0,
};

constexpr size_t REGISTER_16BYTES_P2_SIZE =
    sizeof(REGISTER_16BYTES_P2) / sizeof(uint16_t);

constexpr uint16_t REGISTER_16BYTES_P3[] = {
    llvm::AArch64::Q0_Q1_Q2,    llvm::AArch64::Q1_Q2_Q3,
    llvm::AArch64::Q2_Q3_Q4,    llvm::AArch64::Q3_Q4_Q5,
    llvm::AArch64::Q4_Q5_Q6,    llvm::AArch64::Q5_Q6_Q7,
    llvm::AArch64::Q6_Q7_Q8,    llvm::AArch64::Q7_Q8_Q9,
    llvm::AArch64::Q8_Q9_Q10,   llvm::AArch64::Q9_Q10_Q11,
    llvm::AArch64::Q10_Q11_Q12, llvm::AArch64::Q11_Q12_Q13,
    llvm::AArch64::Q12_Q13_Q14, llvm::AArch64::Q13_Q14_Q15,
    llvm::AArch64::Q14_Q15_Q16, llvm::AArch64::Q15_Q16_Q17,
    llvm::AArch64::Q16_Q17_Q18, llvm::AArch64::Q17_Q18_Q19,
    llvm::AArch64::Q18_Q19_Q20, llvm::AArch64::Q19_Q20_Q21,
    llvm::AArch64::Q20_Q21_Q22, llvm::AArch64::Q21_Q22_Q23,
    llvm::AArch64::Q22_Q23_Q24, llvm::AArch64::Q23_Q24_Q25,
    llvm::AArch64::Q24_Q25_Q26, llvm::AArch64::Q25_Q26_Q27,
    llvm::AArch64::Q26_Q27_Q28, llvm::AArch64::Q27_Q28_Q29,
    llvm::AArch64::Q28_Q29_Q30, llvm::AArch64::Q29_Q30_Q31,
    llvm::AArch64::Q30_Q31_Q0,  llvm::AArch64::Q31_Q0_Q1,
};

constexpr size_t REGISTER_16BYTES_P3_SIZE =
    sizeof(REGISTER_16BYTES_P3) / sizeof(uint16_t);

constexpr uint16_t REGISTER_16BYTES_P4[] = {
    llvm::AArch64::Q0_Q1_Q2_Q3,     llvm::AArch64::Q1_Q2_Q3_Q4,
    llvm::AArch64::Q2_Q3_Q4_Q5,     llvm::AArch64::Q3_Q4_Q5_Q6,
    llvm::AArch64::Q4_Q5_Q6_Q7,     llvm::AArch64::Q5_Q6_Q7_Q8,
    llvm::AArch64::Q6_Q7_Q8_Q9,     llvm::AArch64::Q7_Q8_Q9_Q10,
    llvm::AArch64::Q8_Q9_Q10_Q11,   llvm::AArch64::Q9_Q10_Q11_Q12,
    llvm::AArch64::Q10_Q11_Q12_Q13, llvm::AArch64::Q11_Q12_Q13_Q14,
    llvm::AArch64::Q12_Q13_Q14_Q15, llvm::AArch64::Q13_Q14_Q15_Q16,
    llvm::AArch64::Q14_Q15_Q16_Q17, llvm::AArch64::Q15_Q16_Q17_Q18,
    llvm::AArch64::Q16_Q17_Q18_Q19, llvm::AArch64::Q17_Q18_Q19_Q20,
    llvm::AArch64::Q18_Q19_Q20_Q21, llvm::AArch64::Q19_Q20_Q21_Q22,
    llvm::AArch64::Q20_Q21_Q22_Q23, llvm::AArch64::Q21_Q22_Q23_Q24,
    llvm::AArch64::Q22_Q23_Q24_Q25, llvm::AArch64::Q23_Q24_Q25_Q26,
    llvm::AArch64::Q24_Q25_Q26_Q27, llvm::AArch64::Q25_Q26_Q27_Q28,
    llvm::AArch64::Q26_Q27_Q28_Q29, llvm::AArch64::Q27_Q28_Q29_Q30,
    llvm::AArch64::Q28_Q29_Q30_Q31, llvm::AArch64::Q29_Q30_Q31_Q0,
    llvm::AArch64::Q30_Q31_Q0_Q1,   llvm::AArch64::Q31_Q0_Q1_Q2,
};

constexpr size_t REGISTER_16BYTES_P4_SIZE =
    sizeof(REGISTER_16BYTES_P4) / sizeof(uint16_t);

/* Encode base register on one byte
 * -1: invalid
 *  0 : X0 ... 31 : X31/SP  (special 32 : XZR)
 * 33 : Q0 ... 64 : Q31     (special 65 : FPCR)
 */
constexpr int8_t getEncodedBaseReg(unsigned reg) {
  // verify llvm register enum
  static_assert((llvm::AArch64::B31 - llvm::AArch64::B0) == 31);
  static_assert((llvm::AArch64::D31 - llvm::AArch64::D0) == 31);
  static_assert((llvm::AArch64::H31 - llvm::AArch64::H0) == 31);
  static_assert((llvm::AArch64::Q31 - llvm::AArch64::Q0) == 31);
  static_assert((llvm::AArch64::S31 - llvm::AArch64::S0) == 31);
  static_assert((llvm::AArch64::W30 - llvm::AArch64::W0) == 30);
  static_assert((llvm::AArch64::X28 - llvm::AArch64::X0) == 28);
  static_assert((llvm::AArch64::D31_D0 - llvm::AArch64::D0_D1) == 31);
  static_assert((llvm::AArch64::D31_D0_D1 - llvm::AArch64::D0_D1_D2) == 31);
  static_assert((llvm::AArch64::D31_D0_D1_D2 - llvm::AArch64::D0_D1_D2_D3) ==
                31);
  static_assert((llvm::AArch64::Q31_Q0 - llvm::AArch64::Q0_Q1) == 31);
  static_assert((llvm::AArch64::Q31_Q0_Q1 - llvm::AArch64::Q0_Q1_Q2) == 31);
  static_assert((llvm::AArch64::Q31_Q0_Q1_Q2 - llvm::AArch64::Q0_Q1_Q2_Q3) ==
                31);
  static_assert((llvm::AArch64::W28_W29 - llvm::AArch64::W0_W1) == 14);
  static_assert((llvm::AArch64::X26_X27 - llvm::AArch64::X0_X1) == 13);
  static_assert((llvm::AArch64::X20_X21_X22_X23_X24_X25_X26_X27 -
                 llvm::AArch64::X0_X1_X2_X3_X4_X5_X6_X7) == 10);

  if (llvm::AArch64::B0 <= reg and reg <= llvm::AArch64::B31) {
    return 33 + (reg - llvm::AArch64::B0);
  } else if (llvm::AArch64::D0 <= reg and reg <= llvm::AArch64::D31) {
    return 33 + (reg - llvm::AArch64::D0);
  } else if (llvm::AArch64::H0 <= reg and reg <= llvm::AArch64::H31) {
    return 33 + (reg - llvm::AArch64::H0);
  } else if (llvm::AArch64::Q0 <= reg and reg <= llvm::AArch64::Q31) {
    return 33 + (reg - llvm::AArch64::Q0);
  } else if (llvm::AArch64::S0 <= reg and reg <= llvm::AArch64::S31) {
    return 33 + (reg - llvm::AArch64::S0);
  } else if (llvm::AArch64::W0 <= reg and reg <= llvm::AArch64::W30) {
    return 0 + (reg - llvm::AArch64::W0);
  } else if (llvm::AArch64::X0 <= reg and reg <= llvm::AArch64::X28) {
    return 0 + (reg - llvm::AArch64::X0);
  } else if (llvm::AArch64::D0_D1 <= reg and reg <= llvm::AArch64::D31_D0) {
    return 33 + (reg - llvm::AArch64::D0_D1);
  } else if (llvm::AArch64::D0_D1_D2 <= reg and
             reg <= llvm::AArch64::D31_D0_D1) {
    return 33 + (reg - llvm::AArch64::D0_D1_D2);
  } else if (llvm::AArch64::D0_D1_D2_D3 <= reg and
             reg <= llvm::AArch64::D31_D0_D1_D2) {
    return 33 + (reg - llvm::AArch64::D0_D1_D2_D3);
  } else if (llvm::AArch64::Q0_Q1 <= reg and reg <= llvm::AArch64::Q31_Q0) {
    return 33 + (reg - llvm::AArch64::Q0_Q1);
  } else if (llvm::AArch64::Q0_Q1_Q2 <= reg and
             reg <= llvm::AArch64::Q31_Q0_Q1) {
    return 33 + (reg - llvm::AArch64::Q0_Q1_Q2);
  } else if (llvm::AArch64::Q0_Q1_Q2_Q3 <= reg and
             reg <= llvm::AArch64::Q31_Q0_Q1_Q2) {
    return 33 + (reg - llvm::AArch64::Q0_Q1_Q2_Q3);
  } else if (llvm::AArch64::W0_W1 <= reg and reg <= llvm::AArch64::W28_W29) {
    return 0 + (reg - llvm::AArch64::W0_W1) * 2;
  } else if (llvm::AArch64::X0_X1 <= reg and reg <= llvm::AArch64::X26_X27) {
    return 0 + (reg - llvm::AArch64::X0_X1) * 2;
  } else if (llvm::AArch64::X0_X1_X2_X3_X4_X5_X6_X7 <= reg and
             reg <= llvm::AArch64::X20_X21_X22_X23_X24_X25_X26_X27) {
    return 0 + (reg - llvm::AArch64::X0_X1_X2_X3_X4_X5_X6_X7) * 2;
  } else {
    switch (reg) {
      case llvm::AArch64::X22_X23_X24_X25_X26_X27_X28_FP:
        return 22;
      case llvm::AArch64::X28_FP:
        return 28;
      case llvm::AArch64::FP:
        return 29;
      case llvm::AArch64::LR:
      case llvm::AArch64::W30_WZR:
      case llvm::AArch64::LR_XZR:
        return 30;
      case llvm::AArch64::WSP:
      case llvm::AArch64::SP:
        return 31;
      case llvm::AArch64::WZR:
      case llvm::AArch64::XZR:
        return 32;
      case llvm::AArch64::FPCR:
        return 65;
      default:
        return -1;
    }
  }
}

struct RegisterInfoArray {
  uint16_t sizeArr[llvm::AArch64::NUM_TARGET_REGS] = {0};

  // Use to find the Higher register
  // -1: invalid
  //  0 : X0 ... 31 : X31/SP  (special 32 : XZR)
  // 33 : Q0 ... 64 : Q31     (special 65 : FPCR)
  int8_t baseReg[llvm::AArch64::NUM_TARGET_REGS] = {0};

  constexpr uint16_t setValue(uint8_t size, uint8_t packed) {
    return (size & 0xff) | ((packed & 0xff) << 8);
  }

  constexpr RegisterInfoArray() {
    for (size_t i = 0; i < REGISTER_1BYTE_SIZE; i++) {
      sizeArr[REGISTER_1BYTE[i]] = setValue(1, 1);
    }
    for (size_t i = 0; i < REGISTER_2BYTES_SIZE; i++) {
      sizeArr[REGISTER_2BYTES[i]] = setValue(2, 1);
    }
    for (size_t i = 0; i < REGISTER_4BYTES_SIZE; i++) {
      sizeArr[REGISTER_4BYTES[i]] = setValue(4, 1);
    }
    for (size_t i = 0; i < REGISTER_4BYTES_P2_SIZE; i++) {
      sizeArr[REGISTER_4BYTES_P2[i]] = setValue(4, 2);
    }
    for (size_t i = 0; i < REGISTER_8BYTES_SIZE; i++) {
      sizeArr[REGISTER_8BYTES[i]] = setValue(8, 1);
    }
    for (size_t i = 0; i < REGISTER_8BYTES_P2_SIZE; i++) {
      sizeArr[REGISTER_8BYTES_P2[i]] = setValue(8, 2);
    }
    for (size_t i = 0; i < REGISTER_8BYTES_P3_SIZE; i++) {
      sizeArr[REGISTER_8BYTES_P3[i]] = setValue(8, 3);
    }
    for (size_t i = 0; i < REGISTER_8BYTES_P4_SIZE; i++) {
      sizeArr[REGISTER_8BYTES_P4[i]] = setValue(8, 4);
    }
    for (size_t i = 0; i < REGISTER_8BYTES_P8_SIZE; i++) {
      sizeArr[REGISTER_8BYTES_P8[i]] = setValue(8, 8);
    }
    for (size_t i = 0; i < REGISTER_16BYTES_SIZE; i++) {
      sizeArr[REGISTER_16BYTES[i]] = setValue(16, 1);
    }
    for (size_t i = 0; i < REGISTER_16BYTES_P2_SIZE; i++) {
      sizeArr[REGISTER_16BYTES_P2[i]] = setValue(16, 2);
    }
    for (size_t i = 0; i < REGISTER_16BYTES_P3_SIZE; i++) {
      sizeArr[REGISTER_16BYTES_P3[i]] = setValue(16, 3);
    }
    for (size_t i = 0; i < REGISTER_16BYTES_P4_SIZE; i++) {
      sizeArr[REGISTER_16BYTES_P4[i]] = setValue(16, 4);
    }
    for (size_t i = 0; i < llvm::AArch64::NUM_TARGET_REGS; i++) {
      baseReg[i] = getEncodedBaseReg(i);
    }
  }

  inline uint8_t getSize(RegLLVM reg_) const {
    unsigned reg = reg_.getValue();
    if (reg < llvm::AArch64::NUM_TARGET_REGS)
      return sizeArr[reg] & 0xff;

    QBDI_ERROR("No register {}", reg);
    return 0;
  }

  inline uint8_t getPacked(RegLLVM reg_) const {
    unsigned reg = reg_.getValue();
    if (reg < llvm::AArch64::NUM_TARGET_REGS)
      return (sizeArr[reg] >> 8) & 0xff;

    QBDI_ERROR("No register {}", reg);
    return 0;
  }

  inline RegLLVM getUpperReg(RegLLVM reg_) const {
    unsigned reg = reg_.getValue();
    if (reg >= llvm::AArch64::NUM_TARGET_REGS) {
      QBDI_ERROR("No register {}", reg);
      return llvm::AArch64::NoRegister;
    }
    int8_t v = baseReg[reg];
    if (v < 0) {
      return llvm::AArch64::NoRegister;
    } else if (v < 29) {
      static_assert((llvm::AArch64::X28 - llvm::AArch64::X0) == 28);
      return llvm::AArch64::X0 + v;
    } else if (v < 33) {
      switch (v) {
        case 29:
          return llvm::AArch64::FP;
        case 30:
          return llvm::AArch64::LR;
        case 31:
          return llvm::AArch64::SP;
        case 32:
          return llvm::AArch64::XZR;
      }
    } else if (v < 65) {
      static_assert((llvm::AArch64::Q31 - llvm::AArch64::Q0) == 31);
      return llvm::AArch64::Q0 + (v - 33);
    } else if (v == 65) {
      return llvm::AArch64::FPCR;
    }
    // invalid positive value
    QBDI_ERROR("Wrong value {}", v);
    return llvm::AArch64::NoRegister;
  }

  inline size_t getGPRPos(RegLLVM reg_) const {
    unsigned reg = reg_.getValue();
    if (reg >= llvm::AArch64::NUM_TARGET_REGS) {
      QBDI_ERROR("No register {}", reg);
      return -1;
    }
    int8_t v = baseReg[reg];
    if (v < 0 || v > 31) {
      return -1;
    } else {
      return v;
    }
  }

  inline RegLLVM getUpperBasedRegister(RegLLVM reg) const {
    RegLLVM r = getUpperReg(reg);
    if (r == llvm::AArch64::NoRegister) {
      return reg;
    }
    return r;
  }
};

constexpr RegisterInfoArray arrayInfo;

} // anonymous namespace

uint8_t getRegisterSize(RegLLVM reg) { return arrayInfo.getSize(reg); }

uint8_t getRegisterPacked(RegLLVM reg) { return arrayInfo.getPacked(reg); }

uint8_t getRegisterSpaced(RegLLVM reg) { return 1; }

size_t getGPRPosition(RegLLVM reg) { return arrayInfo.getGPRPos(reg); }

RegLLVM getUpperRegister(RegLLVM reg, size_t pos) {
  if (pos == 0) {
    return arrayInfo.getUpperBasedRegister(reg);
  }
  if (pos >= getRegisterPacked(reg)) {
    return llvm::AArch64::NoRegister;
  }
  unsigned r = arrayInfo.getUpperBasedRegister(reg).getValue();
  if (llvm::AArch64::Q0 <= r and r <= llvm::AArch64::Q31) {
    return llvm::AArch64::Q0 + ((pos + (r - llvm::AArch64::Q0)) % 32);
  }

  unsigned p;
  if (llvm::AArch64::X0 <= r and r <= llvm::AArch64::X28) {
    p = r - llvm::AArch64::X0;
  } else {
    switch (r) {
      case llvm::AArch64::FP:
        p = 29;
        break;
      case llvm::AArch64::LR:
        p = 30;
        break;
      case llvm::AArch64::XZR:
        p = 31;
        break;
      case llvm::AArch64::SP:
      default:
        QBDI_ERROR("Unexpected Packed Register {} {}", reg.getValue(), r);
        return llvm::AArch64::NoRegister;
    }
  }
  p = (p + pos) % 32;
  if (p < 29) {
    return llvm::AArch64::X0 + p;
  }
  switch (p) {
    case 29:
      return llvm::AArch64::FP;
    case 30:
      return llvm::AArch64::LR;
    case 31:
      return llvm::AArch64::XZR;
    default:
      QBDI_ERROR("Unexpected Packed Register {} {}", reg.getValue(), r);
      return llvm::AArch64::NoRegister;
  }
}

RegLLVM getPackedRegister(RegLLVM reg, size_t pos) {
  if (pos == 0 and getRegisterPacked(reg) == 1) {
    return reg;
  }
  if (pos >= getRegisterPacked(reg)) {
    return llvm::AArch64::NoRegister;
  }
  unsigned r = getUpperRegister(reg, pos).getValue();
  // for FPR register
  if (llvm::AArch64::Q0 <= r and r <= llvm::AArch64::Q31) {
    switch (getRegisterSize(reg)) {
      case 1:
        return llvm::AArch64::B0 + (r - llvm::AArch64::Q0);
      case 2:
        return llvm::AArch64::H0 + (r - llvm::AArch64::Q0);
      case 4:
        return llvm::AArch64::S0 + (r - llvm::AArch64::Q0);
      case 8:
        return llvm::AArch64::D0 + (r - llvm::AArch64::Q0);
      case 16:
        return llvm::AArch64::Q0 + (r - llvm::AArch64::Q0);
      default:
        QBDI_ERROR("Unexpected size {} for Packed Register {} {}",
                   getRegisterSize(reg), reg.getValue(), r);
        return llvm::AArch64::NoRegister;
    }
  }
  // for GPR register
  if ((llvm::AArch64::X0 <= r and r <= llvm::AArch64::X28) or
      r == llvm::AArch64::FP or r == llvm::AArch64::LR or
      r == llvm::AArch64::XZR) {

    switch (getRegisterSize(reg)) {
      case 4: {
        if (llvm::AArch64::X0 <= r and r <= llvm::AArch64::X28) {
          return llvm::AArch64::W0 + (r - llvm::AArch64::X0);
        }
        switch (r) {
          case llvm::AArch64::FP:
            return llvm::AArch64::W29;
          case llvm::AArch64::LR:
            return llvm::AArch64::W30;
          case llvm::AArch64::XZR:
            return llvm::AArch64::WZR;
          default:
            QBDI_ERROR("Unexpected size {} for Packed Register {} {}",
                       getRegisterSize(reg), reg.getValue(), r);
            return llvm::AArch64::NoRegister;
        }
      }
      case 8:
        return r;
      default:
        QBDI_ERROR("Unexpected size {} for Packed Register {} {}",
                   getRegisterSize(reg), reg.getValue(), r);
        return llvm::AArch64::NoRegister;
    }
  }

  QBDI_ERROR("Unexpected Packed Register {} {}", reg.getValue(), r);
  return llvm::AArch64::NoRegister;
}

void fixLLVMUsedGPR(const llvm::MCInst &inst, const LLVMCPU &llvmcpu,
                    std::array<RegisterUsage, NUM_GPR> &arr,
                    std::map<RegLLVM, RegisterUsage> &m) {}

} // namespace QBDI
