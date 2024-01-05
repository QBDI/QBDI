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
#include <set>
#include <stddef.h>
#include <stdint.h>

#include "ARMInstrInfo.h"
#include "MCTargetDesc/ARMAddressingModes.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrDesc.h"

#include "devVariable.h"
#include "Engine/LLVMCPU.h"
#include "Patch/ARM/InstInfo_ARM.h"
#include "Patch/InstInfo.h"
#include "Patch/Types.h"
#include "Utility/InstAnalysis_prive.h"
#include "Utility/LogSys.h"

#include "QBDI/Config.h"
#include "QBDI/State.h"

namespace QBDI {
namespace {

// Read Instructions
// =================

constexpr unsigned READ_8[] = {
    // clang-format off
    llvm::ARM::LDAB,
    llvm::ARM::LDAEXB,
    llvm::ARM::LDRBT_POST_IMM,
    llvm::ARM::LDRBT_POST_REG,
    llvm::ARM::LDRB_POST_IMM,
    llvm::ARM::LDRB_POST_REG,
    llvm::ARM::LDRB_PRE_IMM,
    llvm::ARM::LDRB_PRE_REG,
    llvm::ARM::LDRBi12,
    llvm::ARM::LDRBrs,
    llvm::ARM::LDREXB,
    llvm::ARM::LDRSB,
    llvm::ARM::LDRSBTi,
    llvm::ARM::LDRSBTr,
    llvm::ARM::LDRSB_POST,
    llvm::ARM::LDRSB_PRE,
    llvm::ARM::SWPB,
    llvm::ARM::VLD1DUPd8,
    llvm::ARM::VLD1DUPd8wb_fixed,
    llvm::ARM::VLD1DUPd8wb_register,
    llvm::ARM::VLD1DUPq8,
    llvm::ARM::VLD1DUPq8wb_fixed,
    llvm::ARM::VLD1DUPq8wb_register,
    llvm::ARM::VLD1LNd8,
    llvm::ARM::VLD1LNd8_UPD,
    llvm::ARM::t2LDAB,
    llvm::ARM::t2LDAEXB,
    llvm::ARM::t2LDRBT,
    llvm::ARM::t2LDRB_POST,
    llvm::ARM::t2LDRB_PRE,
    llvm::ARM::t2LDRBi12,
    llvm::ARM::t2LDRBi8,
    llvm::ARM::t2LDRBpci,
    llvm::ARM::t2LDRBs,
    llvm::ARM::t2LDREXB,
    llvm::ARM::t2LDRSB_POST,
    llvm::ARM::t2LDRSB_PRE,
    llvm::ARM::t2LDRSBi12,
    llvm::ARM::t2LDRSBi8,
    llvm::ARM::t2LDRSBpci,
    llvm::ARM::t2LDRSBs,
    llvm::ARM::t2TBB,
    llvm::ARM::tLDRBi,
    llvm::ARM::tLDRBr,
    llvm::ARM::tLDRSB,
    // clang-format on
};

constexpr size_t READ_8_SIZE = sizeof(READ_8) / sizeof(unsigned);

constexpr unsigned READ_16[] = {
    // clang-format off
    llvm::ARM::LDAEXH,
    llvm::ARM::LDAH,
    llvm::ARM::LDREXH,
    llvm::ARM::LDRH,
    llvm::ARM::LDRHTi,
    llvm::ARM::LDRHTr,
    llvm::ARM::LDRH_POST,
    llvm::ARM::LDRH_PRE,
    llvm::ARM::LDRSH,
    llvm::ARM::LDRSHTi,
    llvm::ARM::LDRSHTr,
    llvm::ARM::LDRSH_POST,
    llvm::ARM::LDRSH_PRE,
    llvm::ARM::VLD1DUPd16,
    llvm::ARM::VLD1DUPd16wb_fixed,
    llvm::ARM::VLD1DUPd16wb_register,
    llvm::ARM::VLD1DUPq16,
    llvm::ARM::VLD1DUPq16wb_fixed,
    llvm::ARM::VLD1DUPq16wb_register,
    llvm::ARM::VLD1LNd16,
    llvm::ARM::VLD1LNd16_UPD,
    llvm::ARM::VLD2DUPd8,
    llvm::ARM::VLD2DUPd8wb_fixed,
    llvm::ARM::VLD2DUPd8wb_register,
    llvm::ARM::VLD2DUPd8x2,
    llvm::ARM::VLD2DUPd8x2wb_fixed,
    llvm::ARM::VLD2DUPd8x2wb_register,
    llvm::ARM::VLD2LNd8,
    llvm::ARM::VLD2LNd8_UPD,
    llvm::ARM::VLDRH,
    llvm::ARM::t2LDAEXH,
    llvm::ARM::t2LDAH,
    llvm::ARM::t2LDREXH,
    llvm::ARM::t2LDRH_POST,
    llvm::ARM::t2LDRH_PRE,
    llvm::ARM::t2LDRHi12,
    llvm::ARM::t2LDRHi8,
    llvm::ARM::t2LDRHpci,
    llvm::ARM::t2LDRHs,
    llvm::ARM::t2LDRSH_POST,
    llvm::ARM::t2LDRSH_PRE,
    llvm::ARM::t2LDRSHi12,
    llvm::ARM::t2LDRSHi8,
    llvm::ARM::t2LDRSHpci,
    llvm::ARM::t2LDRSHs,
    llvm::ARM::t2TBH,
    llvm::ARM::tLDRHi,
    llvm::ARM::tLDRHr,
    llvm::ARM::tLDRSH,
    // clang-format on
};

constexpr size_t READ_16_SIZE = sizeof(READ_16) / sizeof(unsigned);

constexpr unsigned READ_24[] = {
    // clang-format off
    llvm::ARM::VLD3DUPd8,
    llvm::ARM::VLD3DUPd8_UPD,
    llvm::ARM::VLD3DUPq8,
    llvm::ARM::VLD3DUPq8_UPD,
    llvm::ARM::VLD3LNd8,
    llvm::ARM::VLD3LNd8_UPD,
    // clang-format on
};

constexpr size_t READ_24_SIZE = sizeof(READ_24) / sizeof(unsigned);

constexpr unsigned READ_32[] = {
    // clang-format off
    llvm::ARM::LDA,
    llvm::ARM::LDAEX,
    llvm::ARM::LDREX,
    llvm::ARM::LDRT_POST_IMM,
    llvm::ARM::LDRT_POST_REG,
    llvm::ARM::LDR_POST_IMM,
    llvm::ARM::LDR_POST_REG,
    llvm::ARM::LDR_PRE_IMM,
    llvm::ARM::LDR_PRE_REG,
    llvm::ARM::LDRi12,
    llvm::ARM::LDRrs,
    llvm::ARM::SWP,
    llvm::ARM::VLD1DUPd32,
    llvm::ARM::VLD1DUPd32wb_fixed,
    llvm::ARM::VLD1DUPd32wb_register,
    llvm::ARM::VLD1DUPq32,
    llvm::ARM::VLD1DUPq32wb_fixed,
    llvm::ARM::VLD1DUPq32wb_register,
    llvm::ARM::VLD1LNd32,
    llvm::ARM::VLD1LNd32_UPD,
    llvm::ARM::VLD2DUPd16,
    llvm::ARM::VLD2DUPd16wb_fixed,
    llvm::ARM::VLD2DUPd16wb_register,
    llvm::ARM::VLD2DUPd16x2,
    llvm::ARM::VLD2DUPd16x2wb_fixed,
    llvm::ARM::VLD2DUPd16x2wb_register,
    llvm::ARM::VLD2LNd16,
    llvm::ARM::VLD2LNd16_UPD,
    llvm::ARM::VLD2LNq16,
    llvm::ARM::VLD2LNq16_UPD,
    llvm::ARM::VLD4DUPd8,
    llvm::ARM::VLD4DUPd8_UPD,
    llvm::ARM::VLD4DUPq8,
    llvm::ARM::VLD4DUPq8_UPD,
    llvm::ARM::VLD4LNd8,
    llvm::ARM::VLD4LNd8_UPD,
    llvm::ARM::VLDRS,
    llvm::ARM::VLDR_FPCXTNS_off,
    llvm::ARM::VLDR_FPCXTNS_post,
    llvm::ARM::VLDR_FPCXTNS_pre,
    llvm::ARM::VLDR_FPCXTS_off,
    llvm::ARM::VLDR_FPCXTS_post,
    llvm::ARM::VLDR_FPCXTS_pre,
    llvm::ARM::VLDR_FPSCR_NZCVQC_off,
    llvm::ARM::VLDR_FPSCR_NZCVQC_post,
    llvm::ARM::VLDR_FPSCR_NZCVQC_pre,
    llvm::ARM::VLDR_FPSCR_off,
    llvm::ARM::VLDR_FPSCR_post,
    llvm::ARM::VLDR_FPSCR_pre,
    llvm::ARM::VLDR_P0_off,
    llvm::ARM::VLDR_P0_post,
    llvm::ARM::VLDR_P0_pre,
    llvm::ARM::VLDR_VPR_off,
    llvm::ARM::VLDR_VPR_post,
    llvm::ARM::VLDR_VPR_pre,
    llvm::ARM::t2LDA,
    llvm::ARM::t2LDAEX,
    llvm::ARM::t2LDREX,
    llvm::ARM::t2LDRT,
    llvm::ARM::t2LDR_POST,
    llvm::ARM::t2LDR_PRE,
    llvm::ARM::t2LDRi12,
    llvm::ARM::t2LDRi8,
    llvm::ARM::t2LDRpci,
    llvm::ARM::t2LDRs,
    llvm::ARM::tLDRi,
    llvm::ARM::tLDRpci,
    llvm::ARM::tLDRr,
    llvm::ARM::tLDRspi,
    // clang-format on
};

constexpr size_t READ_32_SIZE = sizeof(READ_32) / sizeof(unsigned);

constexpr unsigned READ_48[] = {
    // clang-format off
    llvm::ARM::VLD3DUPd16,
    llvm::ARM::VLD3DUPd16_UPD,
    llvm::ARM::VLD3DUPq16,
    llvm::ARM::VLD3DUPq16_UPD,
    llvm::ARM::VLD3LNd16,
    llvm::ARM::VLD3LNd16_UPD,
    llvm::ARM::VLD3LNq16,
    llvm::ARM::VLD3LNq16_UPD,
    // clang-format on
};

constexpr size_t READ_48_SIZE = sizeof(READ_48) / sizeof(unsigned);

constexpr unsigned READ_64[] = {
    // clang-format off
    llvm::ARM::LDAEXD,
    llvm::ARM::LDRD,
    llvm::ARM::LDRD_POST,
    llvm::ARM::LDRD_PRE,
    llvm::ARM::LDREXD,
    llvm::ARM::VLD1d16,
    llvm::ARM::VLD1d16wb_fixed,
    llvm::ARM::VLD1d16wb_register,
    llvm::ARM::VLD1d32,
    llvm::ARM::VLD1d32wb_fixed,
    llvm::ARM::VLD1d32wb_register,
    llvm::ARM::VLD1d64,
    llvm::ARM::VLD1d64wb_fixed,
    llvm::ARM::VLD1d64wb_register,
    llvm::ARM::VLD1d8,
    llvm::ARM::VLD1d8wb_fixed,
    llvm::ARM::VLD1d8wb_register,
    llvm::ARM::VLD2DUPd32,
    llvm::ARM::VLD2DUPd32wb_fixed,
    llvm::ARM::VLD2DUPd32wb_register,
    llvm::ARM::VLD2DUPd32x2,
    llvm::ARM::VLD2DUPd32x2wb_fixed,
    llvm::ARM::VLD2DUPd32x2wb_register,
    llvm::ARM::VLD2LNd32,
    llvm::ARM::VLD2LNd32_UPD,
    llvm::ARM::VLD2LNq32,
    llvm::ARM::VLD2LNq32_UPD,
    llvm::ARM::VLD4DUPd16,
    llvm::ARM::VLD4DUPd16_UPD,
    llvm::ARM::VLD4DUPq16,
    llvm::ARM::VLD4DUPq16_UPD,
    llvm::ARM::VLD4LNd16,
    llvm::ARM::VLD4LNd16_UPD,
    llvm::ARM::VLD4LNq16,
    llvm::ARM::VLD4LNq16_UPD,
    llvm::ARM::VLDRD,
    llvm::ARM::t2LDAEXD,
    llvm::ARM::t2LDRD_POST,
    llvm::ARM::t2LDRD_PRE,
    llvm::ARM::t2LDRDi8,
    llvm::ARM::t2LDREXD,
    // clang-format on
};

constexpr size_t READ_64_SIZE = sizeof(READ_64) / sizeof(unsigned);

constexpr unsigned READ_96[] = {
    // clang-format off
    llvm::ARM::VLD3DUPd32,
    llvm::ARM::VLD3DUPd32_UPD,
    llvm::ARM::VLD3DUPq32,
    llvm::ARM::VLD3DUPq32_UPD,
    llvm::ARM::VLD3LNd32,
    llvm::ARM::VLD3LNd32_UPD,
    llvm::ARM::VLD3LNq32,
    llvm::ARM::VLD3LNq32_UPD,
    // clang-format on
};

constexpr size_t READ_96_SIZE = sizeof(READ_96) / sizeof(unsigned);

constexpr unsigned READ_128[] = {
    // clang-format off
    llvm::ARM::VLD1q16,
    llvm::ARM::VLD1q16wb_fixed,
    llvm::ARM::VLD1q16wb_register,
    llvm::ARM::VLD1q32,
    llvm::ARM::VLD1q32wb_fixed,
    llvm::ARM::VLD1q32wb_register,
    llvm::ARM::VLD1q64,
    llvm::ARM::VLD1q64wb_fixed,
    llvm::ARM::VLD1q64wb_register,
    llvm::ARM::VLD1q8,
    llvm::ARM::VLD1q8wb_fixed,
    llvm::ARM::VLD1q8wb_register,
    llvm::ARM::VLD2b16,
    llvm::ARM::VLD2b16wb_fixed,
    llvm::ARM::VLD2b16wb_register,
    llvm::ARM::VLD2b32,
    llvm::ARM::VLD2b32wb_fixed,
    llvm::ARM::VLD2b32wb_register,
    llvm::ARM::VLD2b8,
    llvm::ARM::VLD2b8wb_fixed,
    llvm::ARM::VLD2b8wb_register,
    llvm::ARM::VLD2d16,
    llvm::ARM::VLD2d16wb_fixed,
    llvm::ARM::VLD2d16wb_register,
    llvm::ARM::VLD2d32,
    llvm::ARM::VLD2d32wb_fixed,
    llvm::ARM::VLD2d32wb_register,
    llvm::ARM::VLD2d8,
    llvm::ARM::VLD2d8wb_fixed,
    llvm::ARM::VLD2d8wb_register,
    llvm::ARM::VLD4DUPd32,
    llvm::ARM::VLD4DUPd32_UPD,
    llvm::ARM::VLD4DUPq32,
    llvm::ARM::VLD4DUPq32_UPD,
    llvm::ARM::VLD4LNd32,
    llvm::ARM::VLD4LNd32_UPD,
    llvm::ARM::VLD4LNq32,
    llvm::ARM::VLD4LNq32_UPD,
    // clang-format on
};

constexpr size_t READ_128_SIZE = sizeof(READ_128) / sizeof(unsigned);

constexpr unsigned READ_192[] = {
    // clang-format off
    llvm::ARM::VLD1d16T,
    llvm::ARM::VLD1d16Twb_fixed,
    llvm::ARM::VLD1d16Twb_register,
    llvm::ARM::VLD1d32T,
    llvm::ARM::VLD1d32Twb_fixed,
    llvm::ARM::VLD1d32Twb_register,
    llvm::ARM::VLD1d64T,
    llvm::ARM::VLD1d64Twb_fixed,
    llvm::ARM::VLD1d64Twb_register,
    llvm::ARM::VLD1d8T,
    llvm::ARM::VLD1d8Twb_fixed,
    llvm::ARM::VLD1d8Twb_register,
    llvm::ARM::VLD3d16,
    llvm::ARM::VLD3d16_UPD,
    llvm::ARM::VLD3d32,
    llvm::ARM::VLD3d32_UPD,
    llvm::ARM::VLD3d8,
    llvm::ARM::VLD3d8_UPD,
    llvm::ARM::VLD3q16,
    llvm::ARM::VLD3q16_UPD,
    llvm::ARM::VLD3q32,
    llvm::ARM::VLD3q32_UPD,
    llvm::ARM::VLD3q8,
    llvm::ARM::VLD3q8_UPD,
    // clang-format on
};

constexpr size_t READ_192_SIZE = sizeof(READ_192) / sizeof(unsigned);

constexpr unsigned READ_256[] = {
    // clang-format off
    llvm::ARM::VLD1d16Q,
    llvm::ARM::VLD1d16Qwb_fixed,
    llvm::ARM::VLD1d16Qwb_register,
    llvm::ARM::VLD1d32Q,
    llvm::ARM::VLD1d32Qwb_fixed,
    llvm::ARM::VLD1d32Qwb_register,
    llvm::ARM::VLD1d64Q,
    llvm::ARM::VLD1d64Qwb_fixed,
    llvm::ARM::VLD1d64Qwb_register,
    llvm::ARM::VLD1d8Q,
    llvm::ARM::VLD1d8Qwb_fixed,
    llvm::ARM::VLD1d8Qwb_register,
    llvm::ARM::VLD2q16,
    llvm::ARM::VLD2q16wb_fixed,
    llvm::ARM::VLD2q16wb_register,
    llvm::ARM::VLD2q32,
    llvm::ARM::VLD2q32wb_fixed,
    llvm::ARM::VLD2q32wb_register,
    llvm::ARM::VLD2q8,
    llvm::ARM::VLD2q8wb_fixed,
    llvm::ARM::VLD2q8wb_register,
    llvm::ARM::VLD4d16,
    llvm::ARM::VLD4d16_UPD,
    llvm::ARM::VLD4d32,
    llvm::ARM::VLD4d32_UPD,
    llvm::ARM::VLD4d8,
    llvm::ARM::VLD4d8_UPD,
    llvm::ARM::VLD4q16,
    llvm::ARM::VLD4q16_UPD,
    llvm::ARM::VLD4q32,
    llvm::ARM::VLD4q32_UPD,
    llvm::ARM::VLD4q8,
    llvm::ARM::VLD4q8_UPD,
    // clang-format on
};

constexpr size_t READ_256_SIZE = sizeof(READ_256) / sizeof(unsigned);

// instruction with read of a multiple of 4 bytes
constexpr unsigned READ_32_DYN[] = {
    // clang-format off
    llvm::ARM::LDMDA,
    llvm::ARM::LDMDA_UPD,
    llvm::ARM::LDMDB,
    llvm::ARM::LDMDB_UPD,
    llvm::ARM::LDMIA,
    llvm::ARM::LDMIA_UPD,
    llvm::ARM::LDMIB,
    llvm::ARM::LDMIB_UPD,
    llvm::ARM::VLDMSDB_UPD,
    llvm::ARM::VLDMSIA,
    llvm::ARM::VLDMSIA_UPD,
    llvm::ARM::t2LDMDB,
    llvm::ARM::t2LDMDB_UPD,
    llvm::ARM::t2LDMIA,
    llvm::ARM::t2LDMIA_UPD,
    llvm::ARM::tLDMIA,
    llvm::ARM::tPOP,
    // clang-format on
};

constexpr size_t READ_32_DYN_SIZE = sizeof(READ_32_DYN) / sizeof(unsigned);

constexpr unsigned READ_64_DYN[] = {
    // clang-format off
    llvm::ARM::VLDMDDB_UPD,
    llvm::ARM::VLDMDIA,
    llvm::ARM::VLDMDIA_UPD,
    // clang-format on
};

constexpr size_t READ_64_DYN_SIZE = sizeof(READ_64_DYN) / sizeof(unsigned);

constexpr unsigned UNSUPPORTED_READ[] = {
    // clang-format off
    llvm::ARM::LDC2L_OFFSET,
    llvm::ARM::LDC2_OFFSET,
    llvm::ARM::LDCL_OFFSET,
    llvm::ARM::LDC_OFFSET,
    llvm::ARM::MVE_VLD20_16,
    llvm::ARM::MVE_VLD20_16_wb,
    llvm::ARM::MVE_VLD20_32,
    llvm::ARM::MVE_VLD20_32_wb,
    llvm::ARM::MVE_VLD20_8,
    llvm::ARM::MVE_VLD20_8_wb,
    llvm::ARM::MVE_VLD21_16,
    llvm::ARM::MVE_VLD21_16_wb,
    llvm::ARM::MVE_VLD21_32,
    llvm::ARM::MVE_VLD21_32_wb,
    llvm::ARM::MVE_VLD21_8,
    llvm::ARM::MVE_VLD21_8_wb,
    llvm::ARM::MVE_VLD40_16,
    llvm::ARM::MVE_VLD40_16_wb,
    llvm::ARM::MVE_VLD40_32,
    llvm::ARM::MVE_VLD40_32_wb,
    llvm::ARM::MVE_VLD40_8,
    llvm::ARM::MVE_VLD40_8_wb,
    llvm::ARM::MVE_VLD41_16,
    llvm::ARM::MVE_VLD41_16_wb,
    llvm::ARM::MVE_VLD41_32,
    llvm::ARM::MVE_VLD41_32_wb,
    llvm::ARM::MVE_VLD41_8,
    llvm::ARM::MVE_VLD41_8_wb,
    llvm::ARM::MVE_VLD42_16,
    llvm::ARM::MVE_VLD42_16_wb,
    llvm::ARM::MVE_VLD42_32,
    llvm::ARM::MVE_VLD42_32_wb,
    llvm::ARM::MVE_VLD42_8,
    llvm::ARM::MVE_VLD42_8_wb,
    llvm::ARM::MVE_VLD43_16,
    llvm::ARM::MVE_VLD43_16_wb,
    llvm::ARM::MVE_VLD43_32,
    llvm::ARM::MVE_VLD43_32_wb,
    llvm::ARM::MVE_VLD43_8,
    llvm::ARM::MVE_VLD43_8_wb,
    llvm::ARM::MVE_VLDRBS16,
    llvm::ARM::MVE_VLDRBS16_post,
    llvm::ARM::MVE_VLDRBS16_pre,
    llvm::ARM::MVE_VLDRBS16_rq,
    llvm::ARM::MVE_VLDRBS32,
    llvm::ARM::MVE_VLDRBS32_post,
    llvm::ARM::MVE_VLDRBS32_pre,
    llvm::ARM::MVE_VLDRBS32_rq,
    llvm::ARM::MVE_VLDRBU16,
    llvm::ARM::MVE_VLDRBU16_post,
    llvm::ARM::MVE_VLDRBU16_pre,
    llvm::ARM::MVE_VLDRBU16_rq,
    llvm::ARM::MVE_VLDRBU32,
    llvm::ARM::MVE_VLDRBU32_post,
    llvm::ARM::MVE_VLDRBU32_pre,
    llvm::ARM::MVE_VLDRBU32_rq,
    llvm::ARM::MVE_VLDRBU8,
    llvm::ARM::MVE_VLDRBU8_post,
    llvm::ARM::MVE_VLDRBU8_pre,
    llvm::ARM::MVE_VLDRBU8_rq,
    llvm::ARM::MVE_VLDRDU64_qi,
    llvm::ARM::MVE_VLDRDU64_qi_pre,
    llvm::ARM::MVE_VLDRDU64_rq,
    llvm::ARM::MVE_VLDRDU64_rq_u,
    llvm::ARM::MVE_VLDRHS32,
    llvm::ARM::MVE_VLDRHS32_post,
    llvm::ARM::MVE_VLDRHS32_pre,
    llvm::ARM::MVE_VLDRHS32_rq,
    llvm::ARM::MVE_VLDRHS32_rq_u,
    llvm::ARM::MVE_VLDRHU16,
    llvm::ARM::MVE_VLDRHU16_post,
    llvm::ARM::MVE_VLDRHU16_pre,
    llvm::ARM::MVE_VLDRHU16_rq,
    llvm::ARM::MVE_VLDRHU16_rq_u,
    llvm::ARM::MVE_VLDRHU32,
    llvm::ARM::MVE_VLDRHU32_post,
    llvm::ARM::MVE_VLDRHU32_pre,
    llvm::ARM::MVE_VLDRHU32_rq,
    llvm::ARM::MVE_VLDRHU32_rq_u,
    llvm::ARM::MVE_VLDRWU32,
    llvm::ARM::MVE_VLDRWU32_post,
    llvm::ARM::MVE_VLDRWU32_pre,
    llvm::ARM::MVE_VLDRWU32_qi,
    llvm::ARM::MVE_VLDRWU32_qi_pre,
    llvm::ARM::MVE_VLDRWU32_rq,
    llvm::ARM::MVE_VLDRWU32_rq_u,
    llvm::ARM::t2LDC2L_OFFSET,
    llvm::ARM::t2LDC2_OFFSET,
    llvm::ARM::t2LDCL_OFFSET,
    llvm::ARM::t2LDC_OFFSET,
    // clang-format on
};

constexpr size_t UNSUPPORTED_READ_SIZE =
    sizeof(UNSUPPORTED_READ) / sizeof(unsigned);

// Write Instructions
// ==================

constexpr unsigned WRITE_8[] = {
    // clang-format off
    llvm::ARM::STLB,
    llvm::ARM::STLEXB,
    llvm::ARM::STRBT_POST_IMM,
    llvm::ARM::STRBT_POST_REG,
    llvm::ARM::STRB_POST_IMM,
    llvm::ARM::STRB_POST_REG,
    llvm::ARM::STRB_PRE_IMM,
    llvm::ARM::STRB_PRE_REG,
    llvm::ARM::STRBi12,
    llvm::ARM::STRBrs,
    llvm::ARM::STREXB,
    llvm::ARM::SWPB,
    llvm::ARM::VST1LNd8,
    llvm::ARM::VST1LNd8_UPD,
    llvm::ARM::t2STLB,
    llvm::ARM::t2STLEXB,
    llvm::ARM::t2STRBT,
    llvm::ARM::t2STRB_POST,
    llvm::ARM::t2STRB_PRE,
    llvm::ARM::t2STRBi12,
    llvm::ARM::t2STRBi8,
    llvm::ARM::t2STRBs,
    llvm::ARM::t2STREXB,
    llvm::ARM::tSTRBi,
    llvm::ARM::tSTRBr,
    // clang-format on
};

constexpr size_t WRITE_8_SIZE = sizeof(WRITE_8) / sizeof(unsigned);

constexpr unsigned WRITE_16[] = {
    // clang-format off
    llvm::ARM::STLEXH,
    llvm::ARM::STLH,
    llvm::ARM::STREXH,
    llvm::ARM::STRH,
    llvm::ARM::STRH_POST,
    llvm::ARM::STRH_PRE,
    llvm::ARM::VST1LNd16,
    llvm::ARM::VST1LNd16_UPD,
    llvm::ARM::VST2LNd8,
    llvm::ARM::VST2LNd8_UPD,
    llvm::ARM::VSTRH,
    llvm::ARM::t2STLEXH,
    llvm::ARM::t2STLEXH,
    llvm::ARM::t2STLH,
    llvm::ARM::t2STLH,
    llvm::ARM::t2STREXH,
    llvm::ARM::t2STRH_POST,
    llvm::ARM::t2STRH_PRE,
    llvm::ARM::t2STRHi12,
    llvm::ARM::t2STRHi8,
    llvm::ARM::t2STRHs,
    llvm::ARM::tSTRHi,
    llvm::ARM::tSTRHr,
    // clang-format on
};

constexpr size_t WRITE_16_SIZE = sizeof(WRITE_16) / sizeof(unsigned);

constexpr unsigned WRITE_24[] = {
    // clang-format off
    llvm::ARM::VST3LNd8,
    llvm::ARM::VST3LNd8_UPD,
    // clang-format on
};

constexpr size_t WRITE_24_SIZE = sizeof(WRITE_24) / sizeof(unsigned);

constexpr unsigned WRITE_32[] = {
    // clang-format off
    llvm::ARM::STL,
    llvm::ARM::STLEX,
    llvm::ARM::STREX,
    llvm::ARM::STRT_POST_IMM,
    llvm::ARM::STRT_POST_REG,
    llvm::ARM::STR_POST_IMM,
    llvm::ARM::STR_POST_REG,
    llvm::ARM::STR_PRE_IMM,
    llvm::ARM::STR_PRE_REG,
    llvm::ARM::STRi12,
    llvm::ARM::STRrs,
    llvm::ARM::SWP,
    llvm::ARM::VST1LNd32,
    llvm::ARM::VST1LNd32_UPD,
    llvm::ARM::VST2LNd16,
    llvm::ARM::VST2LNd16_UPD,
    llvm::ARM::VST2LNq16,
    llvm::ARM::VST2LNq16_UPD,
    llvm::ARM::VST4LNd8,
    llvm::ARM::VST4LNd8_UPD,
    llvm::ARM::VSTRS,
    llvm::ARM::VSTR_FPCXTNS_off,
    llvm::ARM::VSTR_FPCXTNS_post,
    llvm::ARM::VSTR_FPCXTNS_pre,
    llvm::ARM::VSTR_FPCXTS_off,
    llvm::ARM::VSTR_FPCXTS_post,
    llvm::ARM::VSTR_FPCXTS_pre,
    llvm::ARM::VSTR_FPSCR_NZCVQC_off,
    llvm::ARM::VSTR_FPSCR_NZCVQC_post,
    llvm::ARM::VSTR_FPSCR_NZCVQC_pre,
    llvm::ARM::VSTR_FPSCR_off,
    llvm::ARM::VSTR_FPSCR_post,
    llvm::ARM::VSTR_FPSCR_pre,
    llvm::ARM::VSTR_P0_off,
    llvm::ARM::VSTR_P0_post,
    llvm::ARM::VSTR_P0_pre,
    llvm::ARM::VSTR_VPR_off,
    llvm::ARM::VSTR_VPR_post,
    llvm::ARM::VSTR_VPR_pre,
    llvm::ARM::t2STL,
    llvm::ARM::t2STLEX,
    llvm::ARM::t2STREX,
    llvm::ARM::t2STRT,
    llvm::ARM::t2STR_POST,
    llvm::ARM::t2STR_PRE,
    llvm::ARM::t2STRi12,
    llvm::ARM::t2STRi8,
    llvm::ARM::t2STRs,
    llvm::ARM::tSTRi,
    llvm::ARM::tSTRr,
    llvm::ARM::tSTRspi,
    // clang-format on
};

constexpr size_t WRITE_32_SIZE = sizeof(WRITE_32) / sizeof(unsigned);

constexpr unsigned WRITE_48[] = {
    // clang-format off
    llvm::ARM::VST3LNd16,
    llvm::ARM::VST3LNd16_UPD,
    llvm::ARM::VST3LNq16,
    llvm::ARM::VST3LNq16_UPD,
    // clang-format on
};

constexpr size_t WRITE_48_SIZE = sizeof(WRITE_48) / sizeof(unsigned);

constexpr unsigned WRITE_64[] = {
    // clang-format off
    llvm::ARM::STLEXD,
    llvm::ARM::STRD,
    llvm::ARM::STRD_POST,
    llvm::ARM::STRD_PRE,
    llvm::ARM::STREXD,
    llvm::ARM::VST1d16,
    llvm::ARM::VST1d16wb_fixed,
    llvm::ARM::VST1d16wb_register,
    llvm::ARM::VST1d32,
    llvm::ARM::VST1d32wb_fixed,
    llvm::ARM::VST1d32wb_register,
    llvm::ARM::VST1d64,
    llvm::ARM::VST1d64wb_fixed,
    llvm::ARM::VST1d64wb_register,
    llvm::ARM::VST1d8,
    llvm::ARM::VST1d8wb_fixed,
    llvm::ARM::VST1d8wb_register,
    llvm::ARM::VST2LNd32,
    llvm::ARM::VST2LNd32_UPD,
    llvm::ARM::VST2LNq32,
    llvm::ARM::VST2LNq32_UPD,
    llvm::ARM::VST4LNd16,
    llvm::ARM::VST4LNd16_UPD,
    llvm::ARM::VST4LNq16,
    llvm::ARM::VST4LNq16_UPD,
    llvm::ARM::VSTRD,
    llvm::ARM::t2STLEXD,
    llvm::ARM::t2STRD_POST,
    llvm::ARM::t2STRD_PRE,
    llvm::ARM::t2STRDi8,
    llvm::ARM::t2STREXD,
    // clang-format on
};

constexpr size_t WRITE_64_SIZE = sizeof(WRITE_64) / sizeof(unsigned);

constexpr unsigned WRITE_96[] = {
    // clang-format off
    llvm::ARM::VST3LNd32,
    llvm::ARM::VST3LNd32_UPD,
    llvm::ARM::VST3LNq32,
    llvm::ARM::VST3LNq32_UPD,
    // clang-format on
};

constexpr size_t WRITE_96_SIZE = sizeof(WRITE_96) / sizeof(unsigned);

constexpr unsigned WRITE_128[] = {
    // clang-format off
    llvm::ARM::VST1q16,
    llvm::ARM::VST1q16wb_fixed,
    llvm::ARM::VST1q16wb_register,
    llvm::ARM::VST1q32,
    llvm::ARM::VST1q32wb_fixed,
    llvm::ARM::VST1q32wb_register,
    llvm::ARM::VST1q64,
    llvm::ARM::VST1q64wb_fixed,
    llvm::ARM::VST1q64wb_register,
    llvm::ARM::VST1q8,
    llvm::ARM::VST1q8wb_fixed,
    llvm::ARM::VST1q8wb_register,
    llvm::ARM::VST2b16,
    llvm::ARM::VST2b16wb_fixed,
    llvm::ARM::VST2b16wb_register,
    llvm::ARM::VST2b32,
    llvm::ARM::VST2b32wb_fixed,
    llvm::ARM::VST2b32wb_register,
    llvm::ARM::VST2b8,
    llvm::ARM::VST2b8wb_fixed,
    llvm::ARM::VST2b8wb_register,
    llvm::ARM::VST2d16,
    llvm::ARM::VST2d16wb_fixed,
    llvm::ARM::VST2d16wb_register,
    llvm::ARM::VST2d32,
    llvm::ARM::VST2d32wb_fixed,
    llvm::ARM::VST2d32wb_register,
    llvm::ARM::VST2d8,
    llvm::ARM::VST2d8wb_fixed,
    llvm::ARM::VST2d8wb_register,
    llvm::ARM::VST4LNd32,
    llvm::ARM::VST4LNd32_UPD,
    llvm::ARM::VST4LNq32,
    llvm::ARM::VST4LNq32_UPD,
    // clang-format on
};

constexpr size_t WRITE_128_SIZE = sizeof(WRITE_128) / sizeof(unsigned);

constexpr unsigned WRITE_192[] = {
    // clang-format off
    llvm::ARM::VST1d16T,
    llvm::ARM::VST1d16Twb_fixed,
    llvm::ARM::VST1d16Twb_register,
    llvm::ARM::VST1d32T,
    llvm::ARM::VST1d32Twb_fixed,
    llvm::ARM::VST1d32Twb_register,
    llvm::ARM::VST1d64T,
    llvm::ARM::VST1d64Twb_fixed,
    llvm::ARM::VST1d64Twb_register,
    llvm::ARM::VST1d8T,
    llvm::ARM::VST1d8Twb_fixed,
    llvm::ARM::VST1d8Twb_register,
    llvm::ARM::VST3d16,
    llvm::ARM::VST3d16_UPD,
    llvm::ARM::VST3d32,
    llvm::ARM::VST3d32_UPD,
    llvm::ARM::VST3d8,
    llvm::ARM::VST3d8_UPD,
    llvm::ARM::VST3q16,
    llvm::ARM::VST3q16_UPD,
    llvm::ARM::VST3q32,
    llvm::ARM::VST3q32_UPD,
    llvm::ARM::VST3q8,
    llvm::ARM::VST3q8_UPD,
    // clang-format on
};

constexpr size_t WRITE_192_SIZE = sizeof(WRITE_192) / sizeof(unsigned);

constexpr unsigned WRITE_256[] = {
    // clang-format off
    llvm::ARM::VST1d16Q,
    llvm::ARM::VST1d16Qwb_fixed,
    llvm::ARM::VST1d16Qwb_register,
    llvm::ARM::VST1d32Q,
    llvm::ARM::VST1d32Qwb_fixed,
    llvm::ARM::VST1d32Qwb_register,
    llvm::ARM::VST1d64Q,
    llvm::ARM::VST1d64Qwb_fixed,
    llvm::ARM::VST1d64Qwb_register,
    llvm::ARM::VST1d8Q,
    llvm::ARM::VST1d8Qwb_fixed,
    llvm::ARM::VST1d8Qwb_register,
    llvm::ARM::VST2q16,
    llvm::ARM::VST2q16wb_fixed,
    llvm::ARM::VST2q16wb_register,
    llvm::ARM::VST2q32,
    llvm::ARM::VST2q32wb_fixed,
    llvm::ARM::VST2q32wb_register,
    llvm::ARM::VST2q8,
    llvm::ARM::VST2q8wb_fixed,
    llvm::ARM::VST2q8wb_register,
    llvm::ARM::VST4d16,
    llvm::ARM::VST4d16_UPD,
    llvm::ARM::VST4d32,
    llvm::ARM::VST4d32_UPD,
    llvm::ARM::VST4d8,
    llvm::ARM::VST4d8_UPD,
    llvm::ARM::VST4q16,
    llvm::ARM::VST4q16_UPD,
    llvm::ARM::VST4q32,
    llvm::ARM::VST4q32_UPD,
    llvm::ARM::VST4q8,
    llvm::ARM::VST4q8_UPD,
    // clang-format on
};

constexpr size_t WRITE_256_SIZE = sizeof(WRITE_256) / sizeof(unsigned);

// instruction with write of a multiple of 4 bytes
constexpr unsigned WRITE_32_DYN[] = {
    // clang-format off
    llvm::ARM::STMDA,
    llvm::ARM::STMDA_UPD,
    llvm::ARM::STMDB,
    llvm::ARM::STMDB_UPD,
    llvm::ARM::STMIA,
    llvm::ARM::STMIA_UPD,
    llvm::ARM::STMIB,
    llvm::ARM::STMIB_UPD,
    llvm::ARM::VSTMSDB_UPD,
    llvm::ARM::VSTMSIA,
    llvm::ARM::VSTMSIA_UPD,
    llvm::ARM::t2STMDB,
    llvm::ARM::t2STMDB_UPD,
    llvm::ARM::t2STMIA,
    llvm::ARM::t2STMIA_UPD,
    llvm::ARM::tPUSH,
    llvm::ARM::tSTMIA_UPD,
    // clang-format on
};

constexpr size_t WRITE_32_DYN_SIZE = sizeof(WRITE_32_DYN) / sizeof(unsigned);

constexpr unsigned WRITE_64_DYN[] = {
    // clang-format off
    llvm::ARM::VSTMDDB_UPD,
    llvm::ARM::VSTMDIA,
    llvm::ARM::VSTMDIA_UPD,
    // clang-format on
};

constexpr size_t WRITE_64_DYN_SIZE = sizeof(WRITE_64_DYN) / sizeof(unsigned);

constexpr unsigned UNSUPPORTED_WRITE[] = {
    // clang-format off
    llvm::ARM::MVE_VST20_16,
    llvm::ARM::MVE_VST20_16_wb,
    llvm::ARM::MVE_VST20_32,
    llvm::ARM::MVE_VST20_32_wb,
    llvm::ARM::MVE_VST20_8,
    llvm::ARM::MVE_VST20_8_wb,
    llvm::ARM::MVE_VST21_16,
    llvm::ARM::MVE_VST21_16_wb,
    llvm::ARM::MVE_VST21_32,
    llvm::ARM::MVE_VST21_32_wb,
    llvm::ARM::MVE_VST21_8,
    llvm::ARM::MVE_VST21_8_wb,
    llvm::ARM::MVE_VST40_16,
    llvm::ARM::MVE_VST40_16_wb,
    llvm::ARM::MVE_VST40_32,
    llvm::ARM::MVE_VST40_32_wb,
    llvm::ARM::MVE_VST40_8,
    llvm::ARM::MVE_VST40_8_wb,
    llvm::ARM::MVE_VST41_16,
    llvm::ARM::MVE_VST41_16_wb,
    llvm::ARM::MVE_VST41_32,
    llvm::ARM::MVE_VST41_32_wb,
    llvm::ARM::MVE_VST41_8,
    llvm::ARM::MVE_VST41_8_wb,
    llvm::ARM::MVE_VST42_16,
    llvm::ARM::MVE_VST42_16_wb,
    llvm::ARM::MVE_VST42_32,
    llvm::ARM::MVE_VST42_32_wb,
    llvm::ARM::MVE_VST42_8,
    llvm::ARM::MVE_VST42_8_wb,
    llvm::ARM::MVE_VST43_16,
    llvm::ARM::MVE_VST43_16_wb,
    llvm::ARM::MVE_VST43_32,
    llvm::ARM::MVE_VST43_32_wb,
    llvm::ARM::MVE_VST43_8,
    llvm::ARM::MVE_VST43_8_wb,
    llvm::ARM::MVE_VSTRB16,
    llvm::ARM::MVE_VSTRB16_post,
    llvm::ARM::MVE_VSTRB16_pre,
    llvm::ARM::MVE_VSTRB16_rq,
    llvm::ARM::MVE_VSTRB32,
    llvm::ARM::MVE_VSTRB32_post,
    llvm::ARM::MVE_VSTRB32_pre,
    llvm::ARM::MVE_VSTRB32_rq,
    llvm::ARM::MVE_VSTRB8_rq,
    llvm::ARM::MVE_VSTRBU8,
    llvm::ARM::MVE_VSTRBU8_post,
    llvm::ARM::MVE_VSTRBU8_pre,
    llvm::ARM::MVE_VSTRD64_qi,
    llvm::ARM::MVE_VSTRD64_qi_pre,
    llvm::ARM::MVE_VSTRD64_rq,
    llvm::ARM::MVE_VSTRD64_rq_u,
    llvm::ARM::MVE_VSTRH16_rq,
    llvm::ARM::MVE_VSTRH16_rq_u,
    llvm::ARM::MVE_VSTRH32,
    llvm::ARM::MVE_VSTRH32_post,
    llvm::ARM::MVE_VSTRH32_pre,
    llvm::ARM::MVE_VSTRH32_rq,
    llvm::ARM::MVE_VSTRH32_rq_u,
    llvm::ARM::MVE_VSTRHU16,
    llvm::ARM::MVE_VSTRHU16_post,
    llvm::ARM::MVE_VSTRHU16_pre,
    llvm::ARM::MVE_VSTRW32_qi,
    llvm::ARM::MVE_VSTRW32_qi_pre,
    llvm::ARM::MVE_VSTRW32_rq,
    llvm::ARM::MVE_VSTRW32_rq_u,
    llvm::ARM::MVE_VSTRWU32,
    llvm::ARM::MVE_VSTRWU32_post,
    llvm::ARM::MVE_VSTRWU32_pre,
    llvm::ARM::STC2L_OFFSET,
    llvm::ARM::STC2_OFFSET,
    llvm::ARM::STCL_OFFSET,
    llvm::ARM::STC_OFFSET,
    llvm::ARM::t2STC2L_OFFSET,
    llvm::ARM::t2STC2_OFFSET,
    llvm::ARM::t2STCL_OFFSET,
    llvm::ARM::t2STC_OFFSET,
    // clang-format on
};

constexpr size_t UNSUPPORTED_WRITE_SIZE =
    sizeof(UNSUPPORTED_WRITE) / sizeof(unsigned);

constexpr unsigned TWO_BYTES_ENCODE[] = {
    // clang-format off
    llvm::ARM::t2IT,
    llvm::ARM::t2SETPAN,
    llvm::ARM::tADC,
    llvm::ARM::tADDhirr,
    llvm::ARM::tADDi3,
    llvm::ARM::tADDi8,
    llvm::ARM::tADDrSP,
    llvm::ARM::tADDrSPi,
    llvm::ARM::tADDrr,
    llvm::ARM::tADDspi,
    llvm::ARM::tADDspr,
    llvm::ARM::tADR,
    llvm::ARM::tAND,
    llvm::ARM::tASRri,
    llvm::ARM::tASRrr,
    llvm::ARM::tB,
    llvm::ARM::tBIC,
    llvm::ARM::tBKPT,
    llvm::ARM::tBLXNSr,
    llvm::ARM::tBLXr,
    llvm::ARM::tBX,
    llvm::ARM::tBXNS,
    llvm::ARM::tBcc,
    llvm::ARM::tCBNZ,
    llvm::ARM::tCBZ,
    llvm::ARM::tCMNz,
    llvm::ARM::tCMPhir,
    llvm::ARM::tCMPi8,
    llvm::ARM::tCMPr,
    llvm::ARM::tCPS,
    llvm::ARM::tEOR,
    llvm::ARM::tHINT,
    llvm::ARM::tHLT,
    llvm::ARM::tLDMIA,
    llvm::ARM::tLDRBi,
    llvm::ARM::tLDRBr,
    llvm::ARM::tLDRHi,
    llvm::ARM::tLDRHr,
    llvm::ARM::tLDRSB,
    llvm::ARM::tLDRSH,
    llvm::ARM::tLDRi,
    llvm::ARM::tLDRpci,
    llvm::ARM::tLDRr,
    llvm::ARM::tLDRspi,
    llvm::ARM::tLSLri,
    llvm::ARM::tLSLrr,
    llvm::ARM::tLSRri,
    llvm::ARM::tLSRrr,
    llvm::ARM::tMOVSr,
    llvm::ARM::tMOVi8,
    llvm::ARM::tMOVr,
    llvm::ARM::tMUL,
    llvm::ARM::tMVN,
    llvm::ARM::tORR,
    llvm::ARM::tPICADD,
    llvm::ARM::tPOP,
    llvm::ARM::tPUSH,
    llvm::ARM::tREV,
    llvm::ARM::tREV16,
    llvm::ARM::tREVSH,
    llvm::ARM::tROR,
    llvm::ARM::tRSB,
    llvm::ARM::tSBC,
    llvm::ARM::tSETEND,
    llvm::ARM::tSTMIA_UPD,
    llvm::ARM::tSTRBi,
    llvm::ARM::tSTRBr,
    llvm::ARM::tSTRHi,
    llvm::ARM::tSTRHr,
    llvm::ARM::tSTRi,
    llvm::ARM::tSTRr,
    llvm::ARM::tSTRspi,
    llvm::ARM::tSUBi3,
    llvm::ARM::tSUBi8,
    llvm::ARM::tSUBrr,
    llvm::ARM::tSUBspi,
    llvm::ARM::tSVC,
    llvm::ARM::tSXTB,
    llvm::ARM::tSXTH,
    llvm::ARM::tTRAP,
    llvm::ARM::tTST,
    llvm::ARM::tUDF,
    llvm::ARM::tUXTB,
    llvm::ARM::tUXTH,
    llvm::ARM::t__brkdiv0,
    // clang-format on
};

constexpr size_t TWO_BYTES_ENCODE_SIZE =
    sizeof(TWO_BYTES_ENCODE) / sizeof(unsigned);

/* Highest 16 bits are the write access, lowest 16 bits are the read access.
 *
 * ----------------------------
 * | 0x1f                     |
 * ----------------------------
 * | 1 bit is 16 bit encoding |
 * ----------------------------
 *
 * -----------------------------------------------------------------------------------------
 * | 0x1e                                WRITE ACCESS 0x10 |
 * -----------------------------------------------------------------------------------------
 * | 3 bits unused | 1 bit unsupported | 1 bit dynamic size | 10 bits unsigned
 * access size |
 * -----------------------------------------------------------------------------------------
 *
 * -----------------------------------------------------------------------------------------
 * | 0xf                                 READ ACCESS 0x0 |
 * -----------------------------------------------------------------------------------------
 * | 4 bits unused | 1 bit unsupported | 1 bit dynamic size | 10 bits unsigned
 * access size |
 * -----------------------------------------------------------------------------------------
 */

constexpr uint32_t WRITE_POSITION = 16;
constexpr uint32_t READ(uint32_t s) { return s & 0x3ff; }
constexpr uint32_t WRITE(uint32_t s) { return (s & 0x3ff) << WRITE_POSITION; }
constexpr uint32_t DYN_BIT_READ = 0x400;
constexpr uint32_t DYN_BIT_WRITE = DYN_BIT_READ << WRITE_POSITION;
constexpr uint32_t UNSUPPORTED_BIT_READ = 0x800;
constexpr uint32_t UNSUPPORTED_BIT_WRITE = UNSUPPORTED_BIT_READ
                                           << WRITE_POSITION;
constexpr uint32_t TWO_BYTES_ENCODING = 0x80000000;

constexpr uint32_t GET_READ_SIZE(uint32_t v) { return v & 0x3ff; }
constexpr uint32_t GET_WRITE_SIZE(uint32_t v) {
  return (v >> WRITE_POSITION) & 0x3ff;
}
constexpr bool IS_READ_DYN(uint32_t v) {
  return (v & DYN_BIT_READ) == DYN_BIT_READ;
}
constexpr bool IS_WRITE_DYN(uint32_t v) {
  return (v & DYN_BIT_WRITE) == DYN_BIT_WRITE;
}
constexpr bool IS_UNSUPPORTED_READ(uint32_t v) {
  return (v & UNSUPPORTED_BIT_READ) == UNSUPPORTED_BIT_READ;
}
constexpr bool IS_UNSUPPORTED_WRITE(uint32_t v) {
  return (v & UNSUPPORTED_BIT_WRITE) == UNSUPPORTED_BIT_WRITE;
}
constexpr bool IS_TWO_BYTES_ENCODING(uint32_t v) {
  return (v & TWO_BYTES_ENCODING) == TWO_BYTES_ENCODING;
}

struct MemAccessArray {
  uint32_t arr[llvm::ARM::INSTRUCTION_LIST_END] = {0};

  constexpr inline void _initMemAccessRead(const unsigned buff[],
                                           const size_t buffSize, uint32_t len,
                                           bool dyn = false) {
    for (size_t i = 0; i < buffSize; i++) {
      arr[buff[i]] |= READ(len);
      if (dyn) {
        arr[buff[i]] |= DYN_BIT_READ;
      }
    }
  }

  constexpr inline void _initMemAccessWrite(const unsigned buff[],
                                            const size_t buffSize, uint32_t len,
                                            bool dyn = false) {
    for (size_t i = 0; i < buffSize; i++) {
      arr[buff[i]] |= WRITE(len);
      if (dyn) {
        arr[buff[i]] |= DYN_BIT_WRITE;
      }
    }
  }

  constexpr inline void _initMemUnsupported(const unsigned buff[],
                                            const size_t buffSize, bool read) {
    for (size_t i = 0; i < buffSize; i++) {
      if (read) {
        arr[buff[i]] |= UNSUPPORTED_BIT_READ;
      } else {
        arr[buff[i]] |= UNSUPPORTED_BIT_WRITE;
      }
    }
  }

  constexpr inline void _initTwoBytesEncode(const unsigned buff[],
                                            const size_t buffSize) {
    for (size_t i = 0; i < buffSize; i++) {
      arr[buff[i]] |= TWO_BYTES_ENCODING;
    }
  }

  constexpr MemAccessArray() {
    // read
    _initMemAccessRead(READ_8, READ_8_SIZE, 1);
    _initMemAccessRead(READ_16, READ_16_SIZE, 2);
    _initMemAccessRead(READ_24, READ_24_SIZE, 3);
    _initMemAccessRead(READ_32, READ_32_SIZE, 4);
    _initMemAccessRead(READ_48, READ_48_SIZE, 6);
    _initMemAccessRead(READ_64, READ_64_SIZE, 8);
    _initMemAccessRead(READ_96, READ_96_SIZE, 12);
    _initMemAccessRead(READ_128, READ_128_SIZE, 16);
    _initMemAccessRead(READ_192, READ_192_SIZE, 24);
    _initMemAccessRead(READ_256, READ_256_SIZE, 32);
    _initMemAccessRead(READ_32_DYN, READ_32_DYN_SIZE, 4, true);
    _initMemAccessRead(READ_64_DYN, READ_64_DYN_SIZE, 8, true);
    _initMemUnsupported(UNSUPPORTED_READ, UNSUPPORTED_READ_SIZE, true);
    // write
    _initMemAccessWrite(WRITE_8, WRITE_8_SIZE, 1);
    _initMemAccessWrite(WRITE_16, WRITE_16_SIZE, 2);
    _initMemAccessWrite(WRITE_24, WRITE_24_SIZE, 3);
    _initMemAccessWrite(WRITE_32, WRITE_32_SIZE, 4);
    _initMemAccessWrite(WRITE_48, WRITE_48_SIZE, 6);
    _initMemAccessWrite(WRITE_64, WRITE_64_SIZE, 8);
    _initMemAccessWrite(WRITE_96, WRITE_96_SIZE, 12);
    _initMemAccessWrite(WRITE_128, WRITE_128_SIZE, 16);
    _initMemAccessWrite(WRITE_192, WRITE_192_SIZE, 24);
    _initMemAccessWrite(WRITE_256, WRITE_256_SIZE, 32);
    _initMemAccessWrite(WRITE_32_DYN, WRITE_32_DYN_SIZE, 4, true);
    _initMemAccessWrite(WRITE_64_DYN, WRITE_64_DYN_SIZE, 8, true);
    _initMemUnsupported(UNSUPPORTED_WRITE, UNSUPPORTED_WRITE_SIZE, false);
    // 16 bits encoding
    _initTwoBytesEncode(TWO_BYTES_ENCODE, TWO_BYTES_ENCODE_SIZE);
  }

#if CHECK_INSTINFO_TABLE
  void check_table(const unsigned buff[], const size_t buffSize, uint32_t value,
                   uint32_t mask) const {
    for (size_t i = 0; i < buffSize; i++) {
      unsigned instID = buff[i];
      if ((arr[instID] & mask) != value) {
        fprintf(stderr,
                "[MemAccessArray::check_table], opcode %d, mask %x, expected "
                "%x, found %x\n",
                instID, mask, value, (arr[instID] & mask));
        abort();
      }
    }
  }

  int check() const {
    // read
    check_table(READ_8, READ_8_SIZE, READ(1), 0xfff);
    check_table(READ_16, READ_16_SIZE, READ(2), 0xfff);
    check_table(READ_24, READ_24_SIZE, READ(3), 0xfff);
    check_table(READ_32, READ_32_SIZE, READ(4), 0xfff);
    check_table(READ_48, READ_48_SIZE, READ(6), 0xfff);
    check_table(READ_64, READ_64_SIZE, READ(8), 0xfff);
    check_table(READ_96, READ_96_SIZE, READ(12), 0xfff);
    check_table(READ_128, READ_128_SIZE, READ(16), 0xfff);
    check_table(READ_192, READ_192_SIZE, READ(24), 0xfff);
    check_table(READ_256, READ_256_SIZE, READ(32), 0xfff);
    check_table(READ_32_DYN, READ_32_DYN_SIZE, READ(4) | DYN_BIT_READ, 0xfff);
    check_table(READ_64_DYN, READ_64_DYN_SIZE, READ(8) | DYN_BIT_READ, 0xfff);
    check_table(UNSUPPORTED_READ, UNSUPPORTED_READ_SIZE, UNSUPPORTED_BIT_READ,
                0xfff);
    // write
    check_table(WRITE_8, WRITE_8_SIZE, WRITE(1), 0xfff << WRITE_POSITION);
    check_table(WRITE_16, WRITE_16_SIZE, WRITE(2), 0xfff << WRITE_POSITION);
    check_table(WRITE_24, WRITE_24_SIZE, WRITE(3), 0xfff << WRITE_POSITION);
    check_table(WRITE_32, WRITE_32_SIZE, WRITE(4), 0xfff << WRITE_POSITION);
    check_table(WRITE_48, WRITE_48_SIZE, WRITE(6), 0xfff << WRITE_POSITION);
    check_table(WRITE_64, WRITE_64_SIZE, WRITE(8), 0xfff << WRITE_POSITION);
    check_table(WRITE_96, WRITE_96_SIZE, WRITE(12), 0xfff << WRITE_POSITION);
    check_table(WRITE_128, WRITE_128_SIZE, WRITE(16), 0xfff << WRITE_POSITION);
    check_table(WRITE_192, WRITE_192_SIZE, WRITE(24), 0xfff << WRITE_POSITION);
    check_table(WRITE_256, WRITE_256_SIZE, WRITE(32), 0xfff << WRITE_POSITION);
    check_table(WRITE_32_DYN, WRITE_32_DYN_SIZE, WRITE(4) | DYN_BIT_WRITE,
                0xfff << WRITE_POSITION);
    check_table(WRITE_64_DYN, WRITE_64_DYN_SIZE, WRITE(8) | DYN_BIT_WRITE,
                0xfff << WRITE_POSITION);
    check_table(UNSUPPORTED_WRITE, UNSUPPORTED_WRITE_SIZE,
                UNSUPPORTED_BIT_WRITE, 0xfff << WRITE_POSITION);
    return 0;
  }
#endif

  inline uint32_t get(size_t op) const {
    if (op < llvm::ARM::INSTRUCTION_LIST_END) {
      return arr[op];
    }

    QBDI_ERROR("No opcode {}", op);
    return 0;
  }
};
constexpr MemAccessArray memAccessCache;

#if CHECK_INSTINFO_TABLE
int __check_debug = memAccessCache.check();
#endif

} // anonymous namespace

unsigned getReadSize(const llvm::MCInst &inst, const LLVMCPU &llvmcpu) {
  unsigned readSize = GET_READ_SIZE(memAccessCache.get(inst.getOpcode()));
  if (IS_READ_DYN(memAccessCache.get(inst.getOpcode()))) {
    const llvm::MCInstrDesc &desc = llvmcpu.getMCII().get(inst.getOpcode());
    switch (inst.getOpcode()) {
      default:
        break;
      case llvm::ARM::LDMDA:
      case llvm::ARM::LDMDA_UPD:
      case llvm::ARM::LDMDB:
      case llvm::ARM::LDMDB_UPD:
      case llvm::ARM::LDMIA:
      case llvm::ARM::LDMIA_UPD:
      case llvm::ARM::LDMIB:
      case llvm::ARM::LDMIB_UPD:
      case llvm::ARM::VLDMDDB_UPD:
      case llvm::ARM::VLDMDIA:
      case llvm::ARM::VLDMDIA_UPD:
      case llvm::ARM::VLDMSDB_UPD:
      case llvm::ARM::VLDMSIA:
      case llvm::ARM::VLDMSIA_UPD:
      case llvm::ARM::t2LDMDB:
      case llvm::ARM::t2LDMDB_UPD:
      case llvm::ARM::t2LDMIA:
      case llvm::ARM::t2LDMIA_UPD:
      case llvm::ARM::tLDMIA:
      case llvm::ARM::tPOP: {
        // get number of variadic operand
        unsigned nbVarArgs = desc.getNumOperands();
        unsigned nbArgs = inst.getNumOperands();
        if (nbVarArgs <= nbArgs) {
          readSize = readSize * (1 + nbArgs - nbVarArgs);
        }
        break;
      }
    }
  }
  return readSize;
}

unsigned getWriteSize(const llvm::MCInst &inst, const LLVMCPU &llvmcpu) {
  unsigned writeSize = GET_WRITE_SIZE(memAccessCache.get(inst.getOpcode()));
  if (IS_WRITE_DYN(memAccessCache.get(inst.getOpcode()))) {
    const llvm::MCInstrDesc &desc = llvmcpu.getMCII().get(inst.getOpcode());
    switch (inst.getOpcode()) {
      default:
        break;
      case llvm::ARM::STMDA:
      case llvm::ARM::STMDA_UPD:
      case llvm::ARM::STMDB:
      case llvm::ARM::STMDB_UPD:
      case llvm::ARM::STMIA:
      case llvm::ARM::STMIA_UPD:
      case llvm::ARM::STMIB:
      case llvm::ARM::STMIB_UPD:
      case llvm::ARM::VSTMDDB_UPD:
      case llvm::ARM::VSTMDIA:
      case llvm::ARM::VSTMDIA_UPD:
      case llvm::ARM::VSTMSDB_UPD:
      case llvm::ARM::VSTMSIA:
      case llvm::ARM::VSTMSIA_UPD:
      case llvm::ARM::t2STMDB:
      case llvm::ARM::t2STMDB_UPD:
      case llvm::ARM::t2STMIA:
      case llvm::ARM::t2STMIA_UPD:
      case llvm::ARM::tPUSH:
      case llvm::ARM::tSTMIA_UPD: {
        // get number of variadic operand
        unsigned nbVarArgs = desc.getNumOperands();
        unsigned nbArgs = inst.getNumOperands();
        if (nbVarArgs <= nbArgs) {
          writeSize = writeSize * (1 + nbArgs - nbVarArgs);
        }
        break;
      }
    }
  }
  return writeSize;
}

unsigned getInstSize(const llvm::MCInst &inst, const LLVMCPU &llvmcpu) {

  if (IS_TWO_BYTES_ENCODING(memAccessCache.get(inst.getOpcode()))) {
    return 2;
  } else {
    return 4;
  }
}

unsigned getImmediateSize(const llvm::MCInst &inst, const LLVMCPU &llvmcpu) {
  // TODO check llvm
  return 2;
}

bool unsupportedRead(const llvm::MCInst &inst) {
  return IS_UNSUPPORTED_READ(memAccessCache.get(inst.getOpcode()));
}

bool unsupportedWrite(const llvm::MCInst &inst) {
  return IS_UNSUPPORTED_WRITE(memAccessCache.get(inst.getOpcode()));
}

unsigned getCondition(const llvm::MCInst &inst, const LLVMCPU &llvmcpu) {
  const llvm::MCInstrInfo &MCII = llvmcpu.getMCII();
  const llvm::MCInstrDesc &desc = MCII.get(inst.getOpcode());

  unsigned int predicateFirstOffset = 0;
  bool found = false;
  for (unsigned int opn = 0; opn < desc.getNumOperands(); opn++) {
    const llvm::MCOperandInfo &opInfo = desc.operands()[opn];
    if (opInfo.isPredicate()) {
      found = true;
      predicateFirstOffset = opn;
      break;
    }
  }

  if (not found) {
    return llvm::ARMCC::AL;
  }

  QBDI_REQUIRE_ABORT(predicateFirstOffset + 1 < desc.getNumOperands(),
                     "Invalid operand id {} ({})", predicateFirstOffset + 1,
                     llvmcpu.getInstOpcodeName(inst));

  const llvm::MCOperandInfo &opInfoReg =
      desc.operands()[predicateFirstOffset + 1];

  QBDI_REQUIRE_ABORT(opInfoReg.isPredicate(),
                     "Unexpected operandInfo type {} ({})",
                     predicateFirstOffset + 1, llvmcpu.getInstOpcodeName(inst));
  QBDI_REQUIRE_ABORT(predicateFirstOffset + 1 < inst.getNumOperands(),
                     "Invalid operand id {} ({})", predicateFirstOffset + 1,
                     llvmcpu.getInstOpcodeName(inst));
  QBDI_REQUIRE_ABORT(inst.getOperand(predicateFirstOffset).isImm(),
                     "Unexpected operand type {} ({})", predicateFirstOffset,
                     llvmcpu.getInstOpcodeName(inst));
  QBDI_REQUIRE_ABORT(inst.getOperand(predicateFirstOffset + 1).isReg(),
                     "Unexpected operand type {} ({})",
                     predicateFirstOffset + 1, llvmcpu.getInstOpcodeName(inst));

  RegLLVM r = inst.getOperand(predicateFirstOffset + 1).getReg();
  unsigned cond = inst.getOperand(predicateFirstOffset).getImm();

  if (cond == llvm::ARMCC::AL) {
    QBDI_REQUIRE_ABORT(
        r == llvm::ARM::NoRegister, "Unexpected operand value {} ({})",
        llvmcpu.getRegisterName(r), llvmcpu.getInstOpcodeName(inst));
  } else {
    QBDI_REQUIRE_ABORT(r == llvm::ARM::CPSR, "Unexpected operand value {} ({})",
                       llvmcpu.getRegisterName(r),
                       llvmcpu.getInstOpcodeName(inst));
    QBDI_REQUIRE_ABORT(cond < llvm::ARMCC::AL, "Unexpected condition {} ({})",
                       cond, llvmcpu.getInstOpcodeName(inst));
  }
  return cond;
}

bool variadicOpsIsWrite(const llvm::MCInst &inst) {

  switch (inst.getOpcode()) {
    default:
      return false;
    case llvm::ARM::LDMDA:
    case llvm::ARM::LDMDA_UPD:
    case llvm::ARM::LDMDB:
    case llvm::ARM::LDMDB_UPD:
    case llvm::ARM::LDMIA:
    case llvm::ARM::LDMIA_UPD:
    case llvm::ARM::LDMIB:
    case llvm::ARM::LDMIB_UPD:
    case llvm::ARM::VLDMDDB_UPD:
    case llvm::ARM::VLDMDIA:
    case llvm::ARM::VLDMDIA_UPD:
    case llvm::ARM::VLDMSDB_UPD:
    case llvm::ARM::VLDMSIA:
    case llvm::ARM::VLDMSIA_UPD:
    case llvm::ARM::t2LDMDB:
    case llvm::ARM::t2LDMDB_UPD:
    case llvm::ARM::t2LDMIA:
    case llvm::ARM::t2LDMIA_UPD:
    case llvm::ARM::tLDMIA:
    case llvm::ARM::tPOP:
      return true;
  }
}

sword getFixedOperandValue(const llvm::MCInst &inst, const LLVMCPU &llvmcpu,
                           unsigned index, int64_t value) {
  switch (inst.getOpcode()) {
    default:
      break;
    case llvm::ARM::LDRBT_POST_IMM:
    case llvm::ARM::LDRB_POST_IMM:
    case llvm::ARM::LDRT_POST_IMM:
    case llvm::ARM::LDR_POST_IMM:
    case llvm::ARM::STRBT_POST_IMM:
    case llvm::ARM::STRB_POST_IMM:
    case llvm::ARM::STRT_POST_IMM:
    case llvm::ARM::STR_POST_IMM: {
      if (index != 4)
        break;
      // see lib/Target/ARM/MCTargetDesc/ARMAddressingModes.h getAM2Opc()
      // remove the encoding, as shift and index mode is not needed
      if (llvm::ARM_AM::getAM2Op(value) == llvm::ARM_AM::sub) {
        return -llvm::ARM_AM::getAM2Offset(value);
      } else {
        return llvm::ARM_AM::getAM2Offset(value);
      }
    }
    case llvm::ARM::t2IT:
      if (index != 1)
        return QBDI::InstructionAnalysis::ConditionLLVM2QBDI(value);
      break;
    case llvm::ARM::VLDRH:
    case llvm::ARM::VSTRH:
    case llvm::ARM::tLDRHi:
    case llvm::ARM::tSTRHi:
      if (index == 2)
        return value << 1;
      break;
    case llvm::ARM::VLDRD:
    case llvm::ARM::VLDRS:
    case llvm::ARM::VSTRD:
    case llvm::ARM::VSTRS:
    case llvm::ARM::tLDRi:
    case llvm::ARM::tLDRspi:
    case llvm::ARM::tSTRi:
    case llvm::ARM::tSTRspi:
    case llvm::ARM::t2LDREX:
      if (index == 2)
        return value << 2;
      break;
    case llvm::ARM::t2STREX:
      if (index == 3)
        return value << 2;
      break;
  }
  return static_cast<rword>(value);
}

} // namespace QBDI
