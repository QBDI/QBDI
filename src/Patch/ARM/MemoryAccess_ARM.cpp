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
#include <algorithm>
#include <memory>
#include <stddef.h>
#include <stdint.h>
#include <utility>
#include <vector>

#include "Target/ARM/ARMSubtarget.h"
#include "Target/ARM/MCTargetDesc/ARMAddressingModes.h"

#include "devVariable.h"
#include "Engine/LLVMCPU.h"
#include "ExecBlock/ExecBlock.h"
#include "Patch/ARM/Layer2_ARM.h"
#include "Patch/ARM/PatchGenerator_ARM.h"
#include "Patch/ARM/RelocatableInst_ARM.h"
#include "Patch/InstInfo.h"
#include "Patch/InstrRule.h"
#include "Patch/MemoryAccess.h"
#include "Patch/Patch.h"
#include "Patch/PatchCondition.h"
#include "Patch/RelocatableInst.h"
#include "Utility/LogSys.h"

#include "QBDI/Bitmask.h"
#include "QBDI/Callback.h"
#include "QBDI/State.h"

namespace QBDI {

// PatchGenerator MemoryAccess Address/ReadValue/WriteValue Generator
// ==================================================================

namespace {

typedef RelocatableInst::UniquePtrVec(AddressGenFn)(const Patch &patch,
                                                    Reg dest, bool writeAccess);

/* Address in a register
 * =====================
 */

RelocatableInst::UniquePtrVec ADDR_REG_FN(const Patch &patch, Reg dest,
                                          unsigned operandOff) {
  const LLVMCPU &llvmcpu = *patch.llvmcpu;
  const llvm::MCInst &inst = patch.metadata.inst;
  const rword address = patch.metadata.address;

  QBDI_REQUIRE_ABORT_PATCH(operandOff < inst.getNumOperands(), patch,
                           "Invalid operand");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff).isReg(), patch,
                           "Unexpected operand type");
  RegLLVM addrReg = inst.getOperand(operandOff).getReg();
  if (addrReg == llvm::ARM::PC) {
    if (llvmcpu == CPUMode::Thumb) {
      return conv_unique<RelocatableInst>(
          LoadImm::unique(dest, Constant(address + 4)));
    } else {
      return conv_unique<RelocatableInst>(
          LoadImm::unique(dest, Constant(address + 8)));
    }
  } else {
    return conv_unique<RelocatableInst>(MovReg::unique(dest, addrReg));
  }
}

// address base in 1st operand
constexpr unsigned ADDR_REG_1_TABLE[] = {
    llvm::ARM::LDMIA,       llvm::ARM::LDMIA_UPD,   llvm::ARM::STMIA,
    llvm::ARM::STMIA_UPD,   llvm::ARM::VLDMDIA,     llvm::ARM::VLDMDIA_UPD,
    llvm::ARM::VLDMSIA,     llvm::ARM::VLDMSIA_UPD, llvm::ARM::VSTMDIA,
    llvm::ARM::VSTMDIA_UPD, llvm::ARM::VSTMSIA,     llvm::ARM::VSTMSIA_UPD,
    llvm::ARM::VST1LNd16,   llvm::ARM::VST1LNd32,   llvm::ARM::VST1LNd8,
    llvm::ARM::VST1d16,     llvm::ARM::VST1d16Q,    llvm::ARM::VST1d16T,
    llvm::ARM::VST1d32,     llvm::ARM::VST1d32Q,    llvm::ARM::VST1d32T,
    llvm::ARM::VST1d64,     llvm::ARM::VST1d64Q,    llvm::ARM::VST1d64T,
    llvm::ARM::VST1d8,      llvm::ARM::VST1d8Q,     llvm::ARM::VST1d8T,
    llvm::ARM::VST1q16,     llvm::ARM::VST1q32,     llvm::ARM::VST1q64,
    llvm::ARM::VST1q8,      llvm::ARM::VST2LNd16,   llvm::ARM::VST2LNd32,
    llvm::ARM::VST2LNd8,    llvm::ARM::VST2LNq16,   llvm::ARM::VST2LNq32,
    llvm::ARM::VST2b16,     llvm::ARM::VST2b32,     llvm::ARM::VST2b8,
    llvm::ARM::VST2d16,     llvm::ARM::VST2d32,     llvm::ARM::VST2d8,
    llvm::ARM::VST2q16,     llvm::ARM::VST2q32,     llvm::ARM::VST2q8,
    llvm::ARM::VST3LNd16,   llvm::ARM::VST3LNd32,   llvm::ARM::VST3LNd8,
    llvm::ARM::VST3LNq16,   llvm::ARM::VST3LNq32,   llvm::ARM::VST3d16,
    llvm::ARM::VST3d32,     llvm::ARM::VST3d8,      llvm::ARM::VST3q16,
    llvm::ARM::VST3q32,     llvm::ARM::VST3q8,      llvm::ARM::VST4LNd16,
    llvm::ARM::VST4LNd32,   llvm::ARM::VST4LNd8,    llvm::ARM::VST4LNq16,
    llvm::ARM::VST4LNq32,   llvm::ARM::VST4d16,     llvm::ARM::VST4d32,
    llvm::ARM::VST4d8,      llvm::ARM::VST4q16,     llvm::ARM::VST4q32,
    llvm::ARM::VST4q8,      llvm::ARM::t2LDMIA,     llvm::ARM::t2LDMIA_UPD,
    llvm::ARM::t2STMIA,     llvm::ARM::t2STMIA_UPD, llvm::ARM::tLDMIA,
    llvm::ARM::tSTMIA_UPD,
};

constexpr size_t ADDR_REG_1_SIZE = sizeof(ADDR_REG_1_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec ADDR_REG_1_FN(const Patch &patch, Reg dest,
                                            bool writeAccess) {
  return ADDR_REG_FN(patch, dest, 0);
}

// address base in 2nd operand
constexpr unsigned ADDR_REG_2_TABLE[] = {
    llvm::ARM::LDA,
    llvm::ARM::LDAB,
    llvm::ARM::LDAEX,
    llvm::ARM::LDAEXB,
    llvm::ARM::LDAEXD,
    llvm::ARM::LDAEXH,
    llvm::ARM::LDAH,
    llvm::ARM::LDREX,
    llvm::ARM::LDREXB,
    llvm::ARM::LDREXD,
    llvm::ARM::LDREXH,
    llvm::ARM::STL,
    llvm::ARM::STLB,
    llvm::ARM::STLH,
    llvm::ARM::VLD1DUPd16,
    llvm::ARM::VLD1DUPd32,
    llvm::ARM::VLD1DUPd8,
    llvm::ARM::VLD1DUPq16,
    llvm::ARM::VLD1DUPq32,
    llvm::ARM::VLD1DUPq8,
    llvm::ARM::VLD1LNd16,
    llvm::ARM::VLD1LNd16_UPD,
    llvm::ARM::VLD1LNd32,
    llvm::ARM::VLD1LNd32_UPD,
    llvm::ARM::VLD1LNd8,
    llvm::ARM::VLD1LNd8_UPD,
    llvm::ARM::VLD1d16,
    llvm::ARM::VLD1d16Q,
    llvm::ARM::VLD1d16T,
    llvm::ARM::VLD1d32,
    llvm::ARM::VLD1d32Q,
    llvm::ARM::VLD1d32T,
    llvm::ARM::VLD1d64,
    llvm::ARM::VLD1d64Q,
    llvm::ARM::VLD1d64T,
    llvm::ARM::VLD1d8,
    llvm::ARM::VLD1d8Q,
    llvm::ARM::VLD1d8T,
    llvm::ARM::VLD1q16,
    llvm::ARM::VLD1q32,
    llvm::ARM::VLD1q64,
    llvm::ARM::VLD1q8,
    llvm::ARM::VLD2DUPd16,
    llvm::ARM::VLD2DUPd16x2,
    llvm::ARM::VLD2DUPd32,
    llvm::ARM::VLD2DUPd32x2,
    llvm::ARM::VLD2DUPd8,
    llvm::ARM::VLD2DUPd8x2,
    llvm::ARM::VLD2b16,
    llvm::ARM::VLD2b32,
    llvm::ARM::VLD2b8,
    llvm::ARM::VLD2d16,
    llvm::ARM::VLD2d32,
    llvm::ARM::VLD2d8,
    llvm::ARM::VLD2q16,
    llvm::ARM::VLD2q32,
    llvm::ARM::VLD2q8,
    llvm::ARM::VLDR_FPCXTNS_post,
    llvm::ARM::VLDR_FPCXTS_post,
    llvm::ARM::VLDR_FPSCR_NZCVQC_post,
    llvm::ARM::VLDR_FPSCR_post,
    llvm::ARM::VLDR_P0_post,
    llvm::ARM::VLDR_VPR_post,
    llvm::ARM::VST1LNd16_UPD,
    llvm::ARM::VST1LNd32_UPD,
    llvm::ARM::VST1LNd8_UPD,
    llvm::ARM::VST1d16Qwb_fixed,
    llvm::ARM::VST1d16Qwb_register,
    llvm::ARM::VST1d16Twb_fixed,
    llvm::ARM::VST1d16Twb_register,
    llvm::ARM::VST1d16wb_fixed,
    llvm::ARM::VST1d16wb_register,
    llvm::ARM::VST1d32Qwb_fixed,
    llvm::ARM::VST1d32Qwb_register,
    llvm::ARM::VST1d32Twb_fixed,
    llvm::ARM::VST1d32Twb_register,
    llvm::ARM::VST1d32wb_fixed,
    llvm::ARM::VST1d32wb_register,
    llvm::ARM::VST1d64Qwb_fixed,
    llvm::ARM::VST1d64Qwb_register,
    llvm::ARM::VST1d64Twb_fixed,
    llvm::ARM::VST1d64Twb_register,
    llvm::ARM::VST1d64wb_fixed,
    llvm::ARM::VST1d64wb_register,
    llvm::ARM::VST1d8Qwb_fixed,
    llvm::ARM::VST1d8Qwb_register,
    llvm::ARM::VST1d8Twb_fixed,
    llvm::ARM::VST1d8Twb_register,
    llvm::ARM::VST1d8wb_fixed,
    llvm::ARM::VST1d8wb_register,
    llvm::ARM::VST1q16wb_fixed,
    llvm::ARM::VST1q16wb_register,
    llvm::ARM::VST1q32wb_fixed,
    llvm::ARM::VST1q32wb_register,
    llvm::ARM::VST1q64wb_fixed,
    llvm::ARM::VST1q64wb_register,
    llvm::ARM::VST1q8wb_fixed,
    llvm::ARM::VST1q8wb_register,
    llvm::ARM::VST2LNd16_UPD,
    llvm::ARM::VST2LNd32_UPD,
    llvm::ARM::VST2LNd8_UPD,
    llvm::ARM::VST2LNq16_UPD,
    llvm::ARM::VST2LNq32_UPD,
    llvm::ARM::VST2b16wb_fixed,
    llvm::ARM::VST2b16wb_register,
    llvm::ARM::VST2b32wb_fixed,
    llvm::ARM::VST2b32wb_register,
    llvm::ARM::VST2b8wb_fixed,
    llvm::ARM::VST2b8wb_register,
    llvm::ARM::VST2d16wb_fixed,
    llvm::ARM::VST2d16wb_register,
    llvm::ARM::VST2d32wb_fixed,
    llvm::ARM::VST2d32wb_register,
    llvm::ARM::VST2d8wb_fixed,
    llvm::ARM::VST2d8wb_register,
    llvm::ARM::VST2q16wb_fixed,
    llvm::ARM::VST2q16wb_register,
    llvm::ARM::VST2q32wb_fixed,
    llvm::ARM::VST2q32wb_register,
    llvm::ARM::VST2q8wb_fixed,
    llvm::ARM::VST2q8wb_register,
    llvm::ARM::VST3LNd16_UPD,
    llvm::ARM::VST3LNd32_UPD,
    llvm::ARM::VST3LNd8_UPD,
    llvm::ARM::VST3LNq16_UPD,
    llvm::ARM::VST3LNq32_UPD,
    llvm::ARM::VST3d16_UPD,
    llvm::ARM::VST3d32_UPD,
    llvm::ARM::VST3d8_UPD,
    llvm::ARM::VST3q16_UPD,
    llvm::ARM::VST3q32_UPD,
    llvm::ARM::VST3q8_UPD,
    llvm::ARM::VST4LNd16_UPD,
    llvm::ARM::VST4LNd32_UPD,
    llvm::ARM::VST4LNd8_UPD,
    llvm::ARM::VST4LNq16_UPD,
    llvm::ARM::VST4LNq32_UPD,
    llvm::ARM::VST4d16_UPD,
    llvm::ARM::VST4d32_UPD,
    llvm::ARM::VST4d8_UPD,
    llvm::ARM::VST4q16_UPD,
    llvm::ARM::VST4q32_UPD,
    llvm::ARM::VST4q8_UPD,
    llvm::ARM::VSTR_FPCXTNS_pre,
    llvm::ARM::VSTR_FPCXTS_pre,
    llvm::ARM::VSTR_FPSCR_NZCVQC_pre,
    llvm::ARM::VSTR_FPSCR_pre,
    llvm::ARM::VSTR_P0_pre,
    llvm::ARM::VSTR_VPR_pre,
    llvm::ARM::t2LDA,
    llvm::ARM::t2LDAB,
    llvm::ARM::t2LDAEX,
    llvm::ARM::t2LDAEXB,
    llvm::ARM::t2LDAEXH,
    llvm::ARM::t2LDAH,
    llvm::ARM::t2LDREXB,
    llvm::ARM::t2LDREXH,
    llvm::ARM::t2STL,
    llvm::ARM::t2STLB,
    llvm::ARM::t2STLH,
};

constexpr size_t ADDR_REG_2_SIZE = sizeof(ADDR_REG_2_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec ADDR_REG_2_FN(const Patch &patch, Reg dest,
                                            bool writeAccess) {
  return ADDR_REG_FN(patch, dest, 1);
}

// address base in 3rd operand
constexpr unsigned ADDR_REG_3_TABLE[] = {
    llvm::ARM::LDRBT_POST_IMM,
    llvm::ARM::LDRBT_POST_REG,
    llvm::ARM::LDRB_POST_IMM,
    llvm::ARM::LDRB_POST_REG,
    llvm::ARM::LDRHTi,
    llvm::ARM::LDRHTr,
    llvm::ARM::LDRH_POST,
    llvm::ARM::LDRSBTi,
    llvm::ARM::LDRSBTr,
    llvm::ARM::LDRSB_POST,
    llvm::ARM::LDRSHTi,
    llvm::ARM::LDRSHTr,
    llvm::ARM::LDRSH_POST,
    llvm::ARM::LDRT_POST_IMM,
    llvm::ARM::LDRT_POST_REG,
    llvm::ARM::LDR_POST_IMM,
    llvm::ARM::LDR_POST_REG,
    llvm::ARM::STLEX,
    llvm::ARM::STLEXB,
    llvm::ARM::STLEXD,
    llvm::ARM::STLEXH,
    llvm::ARM::STRBT_POST_IMM,
    llvm::ARM::STRBT_POST_REG,
    llvm::ARM::STRB_POST_IMM,
    llvm::ARM::STRB_POST_REG,
    llvm::ARM::STREX,
    llvm::ARM::STREXB,
    llvm::ARM::STREXD,
    llvm::ARM::STREXH,
    llvm::ARM::STRH_POST,
    llvm::ARM::STRT_POST_IMM,
    llvm::ARM::STRT_POST_REG,
    llvm::ARM::STR_POST_IMM,
    llvm::ARM::STR_POST_REG,
    llvm::ARM::SWP,
    llvm::ARM::SWPB,
    llvm::ARM::VLD1DUPd16wb_fixed,
    llvm::ARM::VLD1DUPd16wb_register,
    llvm::ARM::VLD1DUPd32wb_fixed,
    llvm::ARM::VLD1DUPd32wb_register,
    llvm::ARM::VLD1DUPd8wb_fixed,
    llvm::ARM::VLD1DUPd8wb_register,
    llvm::ARM::VLD1DUPq16wb_fixed,
    llvm::ARM::VLD1DUPq16wb_register,
    llvm::ARM::VLD1DUPq32wb_fixed,
    llvm::ARM::VLD1DUPq32wb_register,
    llvm::ARM::VLD1DUPq8wb_fixed,
    llvm::ARM::VLD1DUPq8wb_register,
    llvm::ARM::VLD1d16Qwb_fixed,
    llvm::ARM::VLD1d16Qwb_register,
    llvm::ARM::VLD1d16Twb_fixed,
    llvm::ARM::VLD1d16Twb_register,
    llvm::ARM::VLD1d16wb_fixed,
    llvm::ARM::VLD1d16wb_register,
    llvm::ARM::VLD1d32Qwb_fixed,
    llvm::ARM::VLD1d32Qwb_register,
    llvm::ARM::VLD1d32Twb_fixed,
    llvm::ARM::VLD1d32Twb_register,
    llvm::ARM::VLD1d32wb_fixed,
    llvm::ARM::VLD1d32wb_register,
    llvm::ARM::VLD1d64Qwb_fixed,
    llvm::ARM::VLD1d64Qwb_register,
    llvm::ARM::VLD1d64Twb_fixed,
    llvm::ARM::VLD1d64Twb_register,
    llvm::ARM::VLD1d64wb_fixed,
    llvm::ARM::VLD1d64wb_register,
    llvm::ARM::VLD1d8Qwb_fixed,
    llvm::ARM::VLD1d8Qwb_register,
    llvm::ARM::VLD1d8Twb_fixed,
    llvm::ARM::VLD1d8Twb_register,
    llvm::ARM::VLD1d8wb_fixed,
    llvm::ARM::VLD1d8wb_register,
    llvm::ARM::VLD1q16wb_fixed,
    llvm::ARM::VLD1q16wb_register,
    llvm::ARM::VLD1q32wb_fixed,
    llvm::ARM::VLD1q32wb_register,
    llvm::ARM::VLD1q64wb_fixed,
    llvm::ARM::VLD1q64wb_register,
    llvm::ARM::VLD1q8wb_fixed,
    llvm::ARM::VLD1q8wb_register,
    llvm::ARM::VLD2DUPd16wb_fixed,
    llvm::ARM::VLD2DUPd16wb_register,
    llvm::ARM::VLD2DUPd16x2wb_fixed,
    llvm::ARM::VLD2DUPd16x2wb_register,
    llvm::ARM::VLD2DUPd32wb_fixed,
    llvm::ARM::VLD2DUPd32wb_register,
    llvm::ARM::VLD2DUPd32x2wb_fixed,
    llvm::ARM::VLD2DUPd32x2wb_register,
    llvm::ARM::VLD2DUPd8wb_fixed,
    llvm::ARM::VLD2DUPd8wb_register,
    llvm::ARM::VLD2DUPd8x2wb_fixed,
    llvm::ARM::VLD2DUPd8x2wb_register,
    llvm::ARM::VLD2LNd16,
    llvm::ARM::VLD2LNd32,
    llvm::ARM::VLD2LNd8,
    llvm::ARM::VLD2LNq16,
    llvm::ARM::VLD2LNq32,
    llvm::ARM::VLD2b16wb_fixed,
    llvm::ARM::VLD2b16wb_register,
    llvm::ARM::VLD2b32wb_fixed,
    llvm::ARM::VLD2b32wb_register,
    llvm::ARM::VLD2b8wb_fixed,
    llvm::ARM::VLD2b8wb_register,
    llvm::ARM::VLD2d16wb_fixed,
    llvm::ARM::VLD2d16wb_register,
    llvm::ARM::VLD2d32wb_fixed,
    llvm::ARM::VLD2d32wb_register,
    llvm::ARM::VLD2d8wb_fixed,
    llvm::ARM::VLD2d8wb_register,
    llvm::ARM::VLD2q16wb_fixed,
    llvm::ARM::VLD2q16wb_register,
    llvm::ARM::VLD2q32wb_fixed,
    llvm::ARM::VLD2q32wb_register,
    llvm::ARM::VLD2q8wb_fixed,
    llvm::ARM::VLD2q8wb_register,
    llvm::ARM::t2LDAEXD,
    llvm::ARM::t2LDRB_POST,
    llvm::ARM::t2LDREXD,
    llvm::ARM::t2LDRH_POST,
    llvm::ARM::t2LDRSB_POST,
    llvm::ARM::t2LDRSH_POST,
    llvm::ARM::t2LDR_POST,
    llvm::ARM::t2STLEX,
    llvm::ARM::t2STLEXB,
    llvm::ARM::t2STLEXH,
    llvm::ARM::t2STRB_POST,
    llvm::ARM::t2STREXB,
    llvm::ARM::t2STREXH,
    llvm::ARM::t2STRH_POST,
    llvm::ARM::t2STR_POST,
};

constexpr size_t ADDR_REG_3_SIZE = sizeof(ADDR_REG_3_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec ADDR_REG_3_FN(const Patch &patch, Reg dest,
                                            bool writeAccess) {
  return ADDR_REG_FN(patch, dest, 2);
}

// address base in 4th operand
constexpr unsigned ADDR_REG_4_TABLE[] = {
    llvm::ARM::LDRD_POST,     llvm::ARM::STRD_POST,    llvm::ARM::VLD2LNd16_UPD,
    llvm::ARM::VLD2LNd32_UPD, llvm::ARM::VLD2LNd8_UPD, llvm::ARM::VLD2LNq16_UPD,
    llvm::ARM::VLD2LNq32_UPD, llvm::ARM::VLD3DUPd16,   llvm::ARM::VLD3DUPd32,
    llvm::ARM::VLD3DUPd8,     llvm::ARM::VLD3DUPq16,   llvm::ARM::VLD3DUPq32,
    llvm::ARM::VLD3DUPq8,     llvm::ARM::VLD3LNd16,    llvm::ARM::VLD3LNd32,
    llvm::ARM::VLD3LNd8,      llvm::ARM::VLD3LNq16,    llvm::ARM::VLD3LNq32,
    llvm::ARM::VLD3d16,       llvm::ARM::VLD3d32,      llvm::ARM::VLD3d8,
    llvm::ARM::VLD3q16,       llvm::ARM::VLD3q32,      llvm::ARM::VLD3q8,
    llvm::ARM::t2LDRD_POST,   llvm::ARM::t2STLEXD,     llvm::ARM::t2STRD_POST,
    llvm::ARM::t2STREXD,
};

constexpr size_t ADDR_REG_4_SIZE = sizeof(ADDR_REG_4_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec ADDR_REG_4_FN(const Patch &patch, Reg dest,
                                            bool writeAccess) {
  return ADDR_REG_FN(patch, dest, 3);
}

// address base in 5th operand
constexpr unsigned ADDR_REG_5_TABLE[] = {
    llvm::ARM::VLD3DUPd16_UPD, llvm::ARM::VLD3DUPd32_UPD,
    llvm::ARM::VLD3DUPd8_UPD,  llvm::ARM::VLD3DUPq16_UPD,
    llvm::ARM::VLD3DUPq32_UPD, llvm::ARM::VLD3DUPq8_UPD,
    llvm::ARM::VLD3LNd16_UPD,  llvm::ARM::VLD3LNd32_UPD,
    llvm::ARM::VLD3LNd8_UPD,   llvm::ARM::VLD3LNq16_UPD,
    llvm::ARM::VLD3LNq32_UPD,  llvm::ARM::VLD3d16_UPD,
    llvm::ARM::VLD3d32_UPD,    llvm::ARM::VLD3d8_UPD,
    llvm::ARM::VLD3q16_UPD,    llvm::ARM::VLD3q32_UPD,
    llvm::ARM::VLD3q8_UPD,     llvm::ARM::VLD4DUPd16,
    llvm::ARM::VLD4DUPd32,     llvm::ARM::VLD4DUPd8,
    llvm::ARM::VLD4DUPq16,     llvm::ARM::VLD4DUPq32,
    llvm::ARM::VLD4DUPq8,      llvm::ARM::VLD4LNd16,
    llvm::ARM::VLD4LNd32,      llvm::ARM::VLD4LNd8,
    llvm::ARM::VLD4LNq16,      llvm::ARM::VLD4LNq32,
    llvm::ARM::VLD4d16,        llvm::ARM::VLD4d32,
    llvm::ARM::VLD4d8,         llvm::ARM::VLD4q16,
    llvm::ARM::VLD4q32,        llvm::ARM::VLD4q8,
};

constexpr size_t ADDR_REG_5_SIZE = sizeof(ADDR_REG_5_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec ADDR_REG_5_FN(const Patch &patch, Reg dest,
                                            bool writeAccess) {
  return ADDR_REG_FN(patch, dest, 4);
}

// address base in 6th operand
constexpr unsigned ADDR_REG_6_TABLE[] = {
    llvm::ARM::VLD4DUPd16_UPD, llvm::ARM::VLD4DUPd32_UPD,
    llvm::ARM::VLD4DUPd8_UPD,  llvm::ARM::VLD4DUPq16_UPD,
    llvm::ARM::VLD4DUPq32_UPD, llvm::ARM::VLD4DUPq8_UPD,
    llvm::ARM::VLD4LNd16_UPD,  llvm::ARM::VLD4LNd32_UPD,
    llvm::ARM::VLD4LNd8_UPD,   llvm::ARM::VLD4LNq16_UPD,
    llvm::ARM::VLD4LNq32_UPD,  llvm::ARM::VLD4d16_UPD,
    llvm::ARM::VLD4d32_UPD,    llvm::ARM::VLD4d8_UPD,
    llvm::ARM::VLD4q16_UPD,    llvm::ARM::VLD4q32_UPD,
    llvm::ARM::VLD4q8_UPD,
};

constexpr size_t ADDR_REG_6_SIZE = sizeof(ADDR_REG_6_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec ADDR_REG_6_FN(const Patch &patch, Reg dest,
                                            bool writeAccess) {
  return ADDR_REG_FN(patch, dest, 5);
}

/* Address in a register with fixed offset
 * =======================================
 */

RelocatableInst::UniquePtrVec ADDR_REG_PLUS_FN(const Patch &patch, Reg dest,
                                               unsigned operandOff,
                                               int offset) {
  const LLVMCPU &llvmcpu = *patch.llvmcpu;
  const llvm::MCInst &inst = patch.metadata.inst;

  QBDI_REQUIRE_ABORT_PATCH(operandOff < inst.getNumOperands(), patch,
                           "Invalid operand");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff).isReg(), patch,
                           "Unexpected operand type");
  RegLLVM addrReg = inst.getOperand(operandOff).getReg();
  QBDI_REQUIRE_ABORT_PATCH(addrReg != llvm::ARM::PC, patch,
                           "Unexpected PC register");
  return Addc(llvmcpu, dest, addrReg, offset, dest);
}

// address base in 1st operand + 4 (no PC)
constexpr unsigned ADDR_REG_1_PLUS4_TABLE[] = {
    llvm::ARM::LDMIB,
    llvm::ARM::LDMIB_UPD,
    llvm::ARM::STMIB,
    llvm::ARM::STMIB_UPD,
};

constexpr size_t ADDR_REG_1_PLUS4_SIZE =
    sizeof(ADDR_REG_1_PLUS4_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec ADDR_REG_1_PLUS4_FN(const Patch &patch, Reg dest,
                                                  bool writeAccess) {
  return ADDR_REG_PLUS_FN(patch, dest, 0, sizeof(rword));
}

// address base in 1st operand - dynamique argument size (no PC)
constexpr unsigned ADDR_REG_1_DYN_TABLE[] = {
    llvm::ARM::LDMDB,       llvm::ARM::LDMDB_UPD,   llvm::ARM::STMDB,
    llvm::ARM::STMDB_UPD,   llvm::ARM::VLDMDDB_UPD, llvm::ARM::VSTMDDB_UPD,
    llvm::ARM::VLDMSDB_UPD, llvm::ARM::VSTMSDB_UPD, llvm::ARM::t2LDMDB,
    llvm::ARM::t2LDMDB_UPD, llvm::ARM::t2STMDB,     llvm::ARM::t2STMDB_UPD,
};

constexpr size_t ADDR_REG_1_DYN_SIZE =
    sizeof(ADDR_REG_1_DYN_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec ADDR_REG_1_DYN_FN(const Patch &patch, Reg dest,
                                                bool writeAccess) {
  const LLVMCPU &llvmcpu = *patch.llvmcpu;
  const llvm::MCInst &inst = patch.metadata.inst;

  if (writeAccess) {
    return ADDR_REG_PLUS_FN(patch, dest, 0, -getWriteSize(inst, llvmcpu));
  } else {
    return ADDR_REG_PLUS_FN(patch, dest, 0, -getReadSize(inst, llvmcpu));
  }
}

// address base in 1st operand - dynamique argument size + 4 (no PC)
constexpr unsigned ADDR_REG_1_DYN_PLUS4_TABLE[] = {
    llvm::ARM::LDMDA,
    llvm::ARM::LDMDA_UPD,
    llvm::ARM::STMDA,
    llvm::ARM::STMDA_UPD,
};

constexpr size_t ADDR_REG_1_DYN_PLUS4_SIZE =
    sizeof(ADDR_REG_1_DYN_PLUS4_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec
ADDR_REG_1_DYN_PLUS4_FN(const Patch &patch, Reg dest, bool writeAccess) {
  const LLVMCPU &llvmcpu = *patch.llvmcpu;
  const llvm::MCInst &inst = patch.metadata.inst;

  if (writeAccess) {
    return ADDR_REG_PLUS_FN(patch, dest, 0,
                            sizeof(rword) - getWriteSize(inst, llvmcpu));
  } else {
    return ADDR_REG_PLUS_FN(patch, dest, 0,
                            sizeof(rword) - getReadSize(inst, llvmcpu));
  }
}

/* Address in a register + immediate
 * =================================
 */

RelocatableInst::UniquePtrVec ADDR_REG_SIMM_FN(const Patch &patch, Reg dest,
                                               unsigned operandOff1,
                                               unsigned operandOff2) {
  const LLVMCPU &llvmcpu = *patch.llvmcpu;
  const llvm::MCInst &inst = patch.metadata.inst;
  const rword address = patch.metadata.address;

  QBDI_REQUIRE_ABORT_PATCH(operandOff1 < inst.getNumOperands(), patch,
                           "Invalid operand");
  QBDI_REQUIRE_ABORT_PATCH(operandOff2 < inst.getNumOperands(), patch,
                           "Invalid operand");

  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff1).isReg(), patch,
                           "Unexpected operand type");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff2).isImm(), patch,
                           "Unexpected operand type");
  RegLLVM addrReg = inst.getOperand(operandOff1).getReg();
  sword offset = inst.getOperand(operandOff2).getImm();
  // encoding of #-0x0
  if (offset == INT32_MIN) {
    offset = 0;
  }
  if (addrReg == llvm::ARM::PC) {
    if (llvmcpu == CPUMode::Thumb) {
      return conv_unique<RelocatableInst>(
          LoadImm::unique(dest, Constant(address + 4 + offset)));
    } else {
      return conv_unique<RelocatableInst>(
          LoadImm::unique(dest, Constant(address + 8 + offset)));
    }
  } else {
    return Addc(llvmcpu, dest, addrReg, offset, dest);
  }
}

// address base in 1st operand + signed imm13 offset in the 2nd
constexpr unsigned ADDR_REG_1_SIMM_2_TABLE[] = {
    llvm::ARM::VLDR_FPCXTNS_off,      llvm::ARM::VLDR_FPCXTS_off,
    llvm::ARM::VLDR_FPSCR_NZCVQC_off, llvm::ARM::VLDR_FPSCR_off,
    llvm::ARM::VLDR_P0_off,           llvm::ARM::VLDR_VPR_off,
    llvm::ARM::VSTR_FPCXTNS_off,      llvm::ARM::VSTR_FPCXTS_off,
    llvm::ARM::VSTR_FPSCR_NZCVQC_off, llvm::ARM::VSTR_FPSCR_off,
    llvm::ARM::VSTR_P0_off,           llvm::ARM::VSTR_VPR_off,
};

constexpr size_t ADDR_REG_1_SIMM_2_SIZE =
    sizeof(ADDR_REG_1_SIMM_2_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec ADDR_REG_1_SIMM_2_FN(const Patch &patch, Reg dest,
                                                   bool writeAccess) {
  return ADDR_REG_SIMM_FN(patch, dest, 0, 1);
}

// address base in 2nd operand + signed imm13 offset in the 3rd
constexpr unsigned ADDR_REG_2_SIMM_3_TABLE[] = {
    llvm::ARM::LDRBi12,
    llvm::ARM::LDRi12,
    llvm::ARM::STRBi12,
    llvm::ARM::STRi12,
    llvm::ARM::VLDR_FPCXTNS_pre,
    llvm::ARM::VLDR_FPCXTS_pre,
    llvm::ARM::VLDR_FPSCR_NZCVQC_pre,
    llvm::ARM::VLDR_FPSCR_pre,
    llvm::ARM::VLDR_P0_pre,
    llvm::ARM::VLDR_VPR_pre,
    llvm::ARM::VSTR_FPCXTNS_post,
    llvm::ARM::VSTR_FPCXTS_post,
    llvm::ARM::VSTR_FPSCR_NZCVQC_post,
    llvm::ARM::VSTR_FPSCR_post,
    llvm::ARM::VSTR_P0_post,
    llvm::ARM::VSTR_VPR_post,
    // unsigned imm8
    llvm::ARM::t2LDRBT,
    llvm::ARM::t2LDRT,
    llvm::ARM::t2STRBT,
    llvm::ARM::t2STRT,
    // unsigned imm12
    llvm::ARM::t2LDRBi12,
    llvm::ARM::t2LDRHi12,
    llvm::ARM::t2LDRSBi12,
    llvm::ARM::t2LDRSHi12,
    llvm::ARM::t2LDRi12,
    llvm::ARM::t2STRBi12,
    llvm::ARM::t2STRHi12,
    llvm::ARM::t2STRi12,
    // signed imm8
    llvm::ARM::t2LDRBi8,
    llvm::ARM::t2LDRHi8,
    llvm::ARM::t2LDRSBi8,
    llvm::ARM::t2LDRSHi8,
    llvm::ARM::t2LDRi8,
    llvm::ARM::t2STRBi8,
    llvm::ARM::t2STRHi8,
    llvm::ARM::t2STRi8,
    // unsigned imm5
    llvm::ARM::tLDRBi,
    llvm::ARM::tSTRBi,
};

constexpr size_t ADDR_REG_2_SIMM_3_SIZE =
    sizeof(ADDR_REG_2_SIMM_3_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec ADDR_REG_2_SIMM_3_FN(const Patch &patch, Reg dest,
                                                   bool writeAccess) {
  return ADDR_REG_SIMM_FN(patch, dest, 1, 2);
}

// address base in 3rd operand + signed imm13 offset in the 4th
constexpr unsigned ADDR_REG_3_SIMM_4_TABLE[] = {
    llvm::ARM::LDRB_PRE_IMM,
    llvm::ARM::LDR_PRE_IMM,
    llvm::ARM::STRB_PRE_IMM,
    llvm::ARM::STR_PRE_IMM,
    // signed imm8
    llvm::ARM::t2LDRB_PRE,
    llvm::ARM::t2LDRDi8,
    llvm::ARM::t2LDRH_PRE,
    llvm::ARM::t2LDRSB_PRE,
    llvm::ARM::t2LDRSH_PRE,
    llvm::ARM::t2LDR_PRE,
    llvm::ARM::t2STRB_PRE,
    llvm::ARM::t2STRDi8,
    llvm::ARM::t2STRH_PRE,
    llvm::ARM::t2STR_PRE,
};

constexpr size_t ADDR_REG_3_SIMM_4_SIZE =
    sizeof(ADDR_REG_3_SIMM_4_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec ADDR_REG_3_SIMM_4_FN(const Patch &patch, Reg dest,
                                                   bool writeAccess) {
  return ADDR_REG_SIMM_FN(patch, dest, 2, 3);
}

// address base in 4th operand + signed imm8 offset in the 5th
constexpr unsigned ADDR_REG_4_SIMM_5_TABLE[] = {
    // signed imm8
    llvm::ARM::t2LDRD_PRE,
    llvm::ARM::t2STRD_PRE,
};

constexpr size_t ADDR_REG_4_SIMM_5_SIZE =
    sizeof(ADDR_REG_4_SIMM_5_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec ADDR_REG_4_SIMM_5_FN(const Patch &patch, Reg dest,
                                                   bool writeAccess) {
  return ADDR_REG_SIMM_FN(patch, dest, 3, 4);
}

/* Address in a register + register
 * ================================
 */

RelocatableInst::UniquePtrVec ADDR_REG_REG_FN(const Patch &patch, Reg dest,
                                              unsigned operandOff1,
                                              unsigned operandOff2) {

  const LLVMCPU &llvmcpu = *patch.llvmcpu;
  const llvm::MCInst &inst = patch.metadata.inst;
  const rword address = patch.metadata.address;

  QBDI_REQUIRE_ABORT_PATCH(operandOff1 < inst.getNumOperands(), patch,
                           "Invalid operand");
  QBDI_REQUIRE_ABORT_PATCH(operandOff2 < inst.getNumOperands(), patch,
                           "Invalid operand");

  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff1).isReg(), patch,
                           "Unexpected operand type");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff2).isReg(), patch,
                           "Unexpected operand type");
  RegLLVM addrReg = inst.getOperand(operandOff1).getReg();
  RegLLVM offsetReg = inst.getOperand(operandOff2).getReg();
  QBDI_REQUIRE_ABORT_PATCH(offsetReg != llvm::ARM::NoRegister, patch,
                           "Missing offset register");

  RelocatableInst::UniquePtrVec reloc;

  if (offsetReg == llvm::ARM::PC or addrReg == llvm::ARM::PC) {
    if (llvmcpu == CPUMode::Thumb) {
      reloc.push_back(LoadImm::unique(dest, address + 4));
    } else {
      reloc.push_back(LoadImm::unique(dest, address + 8));
    }
    if (offsetReg == llvm::ARM::PC) {
      offsetReg = dest;
    }
    if (addrReg == llvm::ARM::PC) {
      addrReg = dest;
    }
  }

  reloc.push_back(Addr(llvmcpu, dest, addrReg, offsetReg));
  return reloc;
}

// address base in 1st operand + register in the 2nd
constexpr unsigned ADDR_REG_1_REG_2_TABLE[] = {
    llvm::ARM::t2TBB,
};

constexpr size_t ADDR_REG_1_REG_2_SIZE =
    sizeof(ADDR_REG_1_REG_2_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec ADDR_REG_1_REG_2_FN(const Patch &patch, Reg dest,
                                                  bool writeAccess) {
  return ADDR_REG_REG_FN(patch, dest, 0, 1);
}

// address base in 2nd operand + register in the 3rd
constexpr unsigned ADDR_REG_2_REG_3_TABLE[] = {
    llvm::ARM::tLDRBr, llvm::ARM::tLDRHr, llvm::ARM::tLDRSB, llvm::ARM::tLDRSH,
    llvm::ARM::tLDRr,  llvm::ARM::tSTRBr, llvm::ARM::tSTRHr, llvm::ARM::tSTRr,
};

constexpr size_t ADDR_REG_2_REG_3_SIZE =
    sizeof(ADDR_REG_2_REG_3_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec ADDR_REG_2_REG_3_FN(const Patch &patch, Reg dest,
                                                  bool writeAccess) {
  return ADDR_REG_REG_FN(patch, dest, 1, 2);
}

/* Address in a register + register LSL #1
 * =======================================
 */

constexpr unsigned ADDR_REG_1_REGSHIFT1_2_TABLE[] = {
    llvm::ARM::t2TBH,
};

constexpr size_t ADDR_REG_1_REGSHIFT1_2_SIZE =
    sizeof(ADDR_REG_1_REGSHIFT1_2_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec
ADDR_REG_1_REGSHIFT1_2_FN(const Patch &patch, Reg dest, bool writeAccess) {

  const LLVMCPU &llvmcpu = *patch.llvmcpu;
  const llvm::MCInst &inst = patch.metadata.inst;
  const rword address = patch.metadata.address;

  QBDI_REQUIRE_ABORT_PATCH(2 <= inst.getNumOperands(), patch,
                           "Invalid operand number");

  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(0).isReg(), patch,
                           "Unexpected operand type");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(1).isReg(), patch,
                           "Unexpected operand type");
  RegLLVM addrReg = inst.getOperand(0).getReg();
  RegLLVM offsetReg = inst.getOperand(1).getReg();
  QBDI_REQUIRE_ABORT_PATCH(offsetReg != llvm::ARM::NoRegister, patch,
                           "Missing offset register");

  RelocatableInst::UniquePtrVec reloc;

  if (offsetReg == llvm::ARM::PC or addrReg == llvm::ARM::PC) {
    if (llvmcpu == CPUMode::Thumb) {
      reloc.push_back(LoadImm::unique(dest, address + 4));
    } else {
      reloc.push_back(LoadImm::unique(dest, address + 8));
    }
    if (offsetReg == llvm::ARM::PC) {
      offsetReg = dest;
    }
    if (addrReg == llvm::ARM::PC) {
      addrReg = dest;
    }
  }

  reloc.push_back(
      Addrs(llvmcpu, dest, addrReg, offsetReg, 1, llvm::ARM_AM::lsl));
  return reloc;
}

/* Address in a register + LSL register
 * ====================================
 */

RelocatableInst::UniquePtrVec ADDR_REG_REGLSL_FN(const Patch &patch, Reg dest,
                                                 unsigned operandOff1,
                                                 unsigned operandOff2,
                                                 unsigned operandOff3) {

  const LLVMCPU &llvmcpu = *patch.llvmcpu;
  const llvm::MCInst &inst = patch.metadata.inst;
  const rword address = patch.metadata.address;

  QBDI_REQUIRE_ABORT_PATCH(operandOff1 < inst.getNumOperands(), patch,
                           "Invalid operand");
  QBDI_REQUIRE_ABORT_PATCH(operandOff2 < inst.getNumOperands(), patch,
                           "Invalid operand");
  QBDI_REQUIRE_ABORT_PATCH(operandOff3 < inst.getNumOperands(), patch,
                           "Invalid operand");

  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff1).isReg(), patch,
                           "Unexpected operand type");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff2).isReg(), patch,
                           "Unexpected operand type");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff3).isImm(), patch,
                           "Unexpected operand type");
  RegLLVM addrReg = inst.getOperand(operandOff1).getReg();
  RegLLVM offsetReg = inst.getOperand(operandOff2).getReg();
  unsigned shift = inst.getOperand(operandOff3).getImm();
  QBDI_REQUIRE_ABORT_PATCH(offsetReg != llvm::ARM::NoRegister, patch,
                           "Missing offset register");

  RelocatableInst::UniquePtrVec reloc;

  if (offsetReg == llvm::ARM::PC or addrReg == llvm::ARM::PC) {
    if (llvmcpu == CPUMode::Thumb) {
      reloc.push_back(LoadImm::unique(dest, address + 4));
    } else {
      reloc.push_back(LoadImm::unique(dest, address + 8));
    }
    if (offsetReg == llvm::ARM::PC) {
      offsetReg = dest;
    }
    if (addrReg == llvm::ARM::PC) {
      addrReg = dest;
    }
  }

  if (shift == 0) {
    reloc.push_back(Addr(llvmcpu, dest, addrReg, offsetReg));
  } else {
    reloc.push_back(
        Addrs(llvmcpu, dest, addrReg, offsetReg, shift, llvm::ARM_AM::lsl));
  }
  return reloc;
}

// address base in 2nd operand + shifted register in the 3rd + shift imm in 4th
constexpr unsigned ADDR_REG_2_REGLSL_3_TABLE[] = {
    llvm::ARM::t2LDRBs,  llvm::ARM::t2LDRHs, llvm::ARM::t2LDRSBs,
    llvm::ARM::t2LDRSHs, llvm::ARM::t2LDRs,  llvm::ARM::t2STRBs,
    llvm::ARM::t2STRHs,  llvm::ARM::t2STRs,
};

constexpr size_t ADDR_REG_2_REGLSL_3_SIZE =
    sizeof(ADDR_REG_2_REGLSL_3_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec
ADDR_REG_2_REGLSL_3_FN(const Patch &patch, Reg dest, bool writeAccess) {
  return ADDR_REG_REGLSL_FN(patch, dest, 1, 2, 3);
}

/* Address in a register + shifted register
 * ========================================
 */

RelocatableInst::UniquePtrVec ADDR_REG_REGSHIFT_FN(const Patch &patch, Reg dest,
                                                   unsigned operandOff1,
                                                   unsigned operandOff2,
                                                   unsigned operandOff3) {

  const LLVMCPU &llvmcpu = *patch.llvmcpu;
  const llvm::MCInst &inst = patch.metadata.inst;
  const rword address = patch.metadata.address;

  QBDI_REQUIRE_ABORT_PATCH(operandOff1 < inst.getNumOperands(), patch,
                           "Invalid operand");
  QBDI_REQUIRE_ABORT_PATCH(operandOff2 < inst.getNumOperands(), patch,
                           "Invalid operand");
  QBDI_REQUIRE_ABORT_PATCH(operandOff3 < inst.getNumOperands(), patch,
                           "Invalid operand");

  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff1).isReg(), patch,
                           "Unexpected operand type");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff2).isReg(), patch,
                           "Unexpected operand type");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff3).isImm(), patch,
                           "Unexpected operand type");
  RegLLVM addrReg = inst.getOperand(operandOff1).getReg();
  RegLLVM offsetReg = inst.getOperand(operandOff2).getReg();
  unsigned value = inst.getOperand(operandOff3).getImm();
  QBDI_REQUIRE_ABORT_PATCH(offsetReg != llvm::ARM::NoRegister, patch,
                           "Missing offset register");

  unsigned shift = llvm::ARM_AM::getAM2Offset(value);
  bool offsetIsSub = llvm::ARM_AM::getAM2Op(value) == llvm::ARM_AM::sub;
  llvm::ARM_AM::ShiftOpc shiftType = llvm::ARM_AM::getAM2ShiftOpc(value);

  RelocatableInst::UniquePtrVec reloc;

  if (offsetReg == llvm::ARM::PC or addrReg == llvm::ARM::PC) {
    if (llvmcpu == CPUMode::Thumb) {
      reloc.push_back(LoadImm::unique(dest, address + 4));
    } else {
      reloc.push_back(LoadImm::unique(dest, address + 8));
    }
    if (offsetReg == llvm::ARM::PC) {
      offsetReg = dest;
    }
    if (addrReg == llvm::ARM::PC) {
      addrReg = dest;
    }
  }

  if (shift == 0 and shiftType == llvm::ARM_AM::no_shift) {
    if (offsetIsSub) {
      reloc.push_back(Subr(llvmcpu, dest, addrReg, offsetReg));
    } else {
      reloc.push_back(Addr(llvmcpu, dest, addrReg, offsetReg));
    }
  } else {
    if (offsetIsSub) {
      reloc.push_back(
          Subrs(llvmcpu, dest, addrReg, offsetReg, shift, shiftType));
    } else {
      reloc.push_back(
          Addrs(llvmcpu, dest, addrReg, offsetReg, shift, shiftType));
    }
  }
  return reloc;
}

// address base in 2nd operand + shifted register in the 3rd + shift imm in 4th
constexpr unsigned ADDR_REG_2_REGSHIFT_3_TABLE[] = {
    llvm::ARM::LDRBrs,
    llvm::ARM::LDRrs,
    llvm::ARM::STRBrs,
    llvm::ARM::STRrs,
};

constexpr size_t ADDR_REG_2_REGSHIFT_3_SIZE =
    sizeof(ADDR_REG_2_REGSHIFT_3_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec
ADDR_REG_2_REGSHIFT_3_FN(const Patch &patch, Reg dest, bool writeAccess) {
  return ADDR_REG_REGSHIFT_FN(patch, dest, 1, 2, 3);
}

// address base in 3rd operand + shifted register in the 4th + shift imm in 5th
constexpr unsigned ADDR_REG_3_REGSHIFT_4_TABLE[] = {
    llvm::ARM::LDRB_PRE_REG,
    llvm::ARM::LDR_PRE_REG,
    llvm::ARM::STRB_PRE_REG,
    llvm::ARM::STR_PRE_REG,
};

constexpr size_t ADDR_REG_3_REGSHIFT_4_SIZE =
    sizeof(ADDR_REG_3_REGSHIFT_4_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec
ADDR_REG_3_REGSHIFT_4_FN(const Patch &patch, Reg dest, bool writeAccess) {
  return ADDR_REG_REGSHIFT_FN(patch, dest, 2, 3, 4);
}

/* Address in a register with (+/- register) or (+/- imm8)
 * =======================================================
 */
RelocatableInst::UniquePtrVec ADDR_REG_IMMORREG_FN(const Patch &patch, Reg dest,
                                                   unsigned operandOff1,
                                                   unsigned operandOff2,
                                                   unsigned operandOff3) {

  const LLVMCPU &llvmcpu = *patch.llvmcpu;
  const llvm::MCInst &inst = patch.metadata.inst;
  const rword address = patch.metadata.address;

  QBDI_REQUIRE_ABORT_PATCH(operandOff1 < inst.getNumOperands(), patch,
                           "Invalid operand");
  QBDI_REQUIRE_ABORT_PATCH(operandOff2 < inst.getNumOperands(), patch,
                           "Invalid operand");
  QBDI_REQUIRE_ABORT_PATCH(operandOff3 < inst.getNumOperands(), patch,
                           "Invalid operand");

  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff1).isReg(), patch,
                           "Unexpected operand type");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff2).isReg(), patch,
                           "Unexpected operand type");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff3).isImm(), patch,
                           "Unexpected operand type");
  RegLLVM addrReg = inst.getOperand(operandOff1).getReg();
  RegLLVM offsetReg = inst.getOperand(operandOff2).getReg();
  sword imm = inst.getOperand(operandOff3).getImm();

  bool offsetIsSub = (((imm >> 8) & 1) == 1);
  imm = (imm & 0xff);

  RelocatableInst::UniquePtrVec reloc;

  if (offsetReg == llvm::ARM::PC or addrReg == llvm::ARM::PC) {
    if (llvmcpu == CPUMode::Thumb) {
      reloc.push_back(LoadImm::unique(dest, address + 4));
    } else {
      reloc.push_back(LoadImm::unique(dest, address + 8));
    }
    if (offsetReg == llvm::ARM::PC) {
      offsetReg = dest;
    }
    if (addrReg == llvm::ARM::PC) {
      addrReg = dest;
    }
  }

  if (offsetReg == llvm::ARM::NoRegister) {
    if (offsetIsSub) {
      reloc.push_back(NoReloc::unique(sub(dest, addrReg, imm)));
    } else {
      reloc.push_back(NoReloc::unique(add(dest, addrReg, imm)));
    }
  } else {
    if (offsetIsSub) {
      reloc.push_back(NoReloc::unique(subr(dest, addrReg, offsetReg)));
    } else {
      reloc.push_back(NoReloc::unique(addr(dest, addrReg, offsetReg)));
    }
  }
  return reloc;
}

// address base in 2nd operand + register in the 3rd + imm in 4th
constexpr unsigned ADDR_REG_IMMORREG_2_TABLE[] = {
    llvm::ARM::LDRH,
    llvm::ARM::LDRSB,
    llvm::ARM::LDRSH,
    llvm::ARM::STRH,
};

constexpr size_t ADDR_REG_IMMORREG_2_SIZE =
    sizeof(ADDR_REG_IMMORREG_2_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec
ADDR_REG_IMMORREG_2_FN(const Patch &patch, Reg dest, bool writeAccess) {
  return ADDR_REG_IMMORREG_FN(patch, dest, 1, 2, 3);
}

// address base in 3rd operand + register in the 4th + imm in 5th
constexpr unsigned ADDR_REG_IMMORREG_3_TABLE[] = {
    llvm::ARM::LDRD,      llvm::ARM::LDRH_PRE, llvm::ARM::LDRSB_PRE,
    llvm::ARM::LDRSH_PRE, llvm::ARM::STRD,     llvm::ARM::STRH_PRE,
};

constexpr size_t ADDR_REG_IMMORREG_3_SIZE =
    sizeof(ADDR_REG_IMMORREG_3_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec
ADDR_REG_IMMORREG_3_FN(const Patch &patch, Reg dest, bool writeAccess) {
  return ADDR_REG_IMMORREG_FN(patch, dest, 2, 3, 4);
}

// address base in 4th operand + register in the 5th + imm in 6th
constexpr unsigned ADDR_REG_IMMORREG_4_TABLE[] = {
    llvm::ARM::LDRD_PRE,
    llvm::ARM::STRD_PRE,
};

constexpr size_t ADDR_REG_IMMORREG_4_SIZE =
    sizeof(ADDR_REG_IMMORREG_4_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec
ADDR_REG_IMMORREG_4_FN(const Patch &patch, Reg dest, bool writeAccess) {
  return ADDR_REG_IMMORREG_FN(patch, dest, 3, 4, 5);
}

/* Address in a register with +/- imm8 << offset
 * =============================================
 * note : instruction also in getFixedOperandValue
 */
RelocatableInst::UniquePtrVec ADDR_REG_IMMSHIFT_FN(const Patch &patch, Reg dest,
                                                   unsigned operandOff1,
                                                   unsigned operandOff2,
                                                   unsigned shiftoffset) {

  const LLVMCPU &llvmcpu = *patch.llvmcpu;
  const llvm::MCInst &inst = patch.metadata.inst;
  const rword address = patch.metadata.address;

  QBDI_REQUIRE_ABORT_PATCH(operandOff1 < inst.getNumOperands(), patch,
                           "Invalid operand");
  QBDI_REQUIRE_ABORT_PATCH(operandOff2 < inst.getNumOperands(), patch,
                           "Invalid operand");

  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff1).isReg(), patch,
                           "Unexpected operand type");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff2).isImm(), patch,
                           "Unexpected operand type");
  RegLLVM addrReg = inst.getOperand(operandOff1).getReg();
  sword imm = inst.getOperand(operandOff2).getImm();

  bool offsetIsSub = (((imm >> 8) & 1) == 1);
  imm = (imm & 0xff) << shiftoffset;

  RelocatableInst::UniquePtrVec reloc;

  if (addrReg == llvm::ARM::PC) {
    if (llvmcpu == CPUMode::Thumb) {
      reloc.push_back(LoadImm::unique(dest, address + 4));
    } else {
      reloc.push_back(LoadImm::unique(dest, address + 8));
    }
    addrReg = dest;
  }

  if (offsetIsSub) {
    imm *= -1;
  }
  reloc.push_back(Add(llvmcpu, dest, addrReg, imm));
  return reloc;
}

// address base in 2nd operand + imm in 3rd (shift 1)
constexpr unsigned ADDR_REG_IMMSHIFT_2_SHIFT1_TABLE[] = {
    llvm::ARM::VLDRH,
    llvm::ARM::VSTRH,
    // unsigned imm5
    llvm::ARM::tLDRHi,
    llvm::ARM::tSTRHi,
};

constexpr size_t ADDR_REG_IMMSHIFT_2_SHIFT1_SIZE =
    sizeof(ADDR_REG_IMMSHIFT_2_SHIFT1_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec
ADDR_REG_IMMSHIFT_2_SHIFT1_FN(const Patch &patch, Reg dest, bool writeAccess) {
  return ADDR_REG_IMMSHIFT_FN(patch, dest, 1, 2, 1);
}

// address base in 2nd operand + imm in 3rd (shift 2)
constexpr unsigned ADDR_REG_IMMSHIFT_2_SHIFT2_TABLE[] = {
    llvm::ARM::VLDRD,
    llvm::ARM::VLDRS,
    llvm::ARM::VSTRD,
    llvm::ARM::VSTRS,
    // unsigned imm5
    llvm::ARM::tLDRi,
    llvm::ARM::tSTRi,
    llvm::ARM::tLDRspi,
    llvm::ARM::tSTRspi,
    // unsigned imm8
    llvm::ARM::t2LDREX,
};

constexpr size_t ADDR_REG_IMMSHIFT_2_SHIFT2_SIZE =
    sizeof(ADDR_REG_IMMSHIFT_2_SHIFT2_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec
ADDR_REG_IMMSHIFT_2_SHIFT2_FN(const Patch &patch, Reg dest, bool writeAccess) {
  return ADDR_REG_IMMSHIFT_FN(patch, dest, 1, 2, 2);
}

// address base in 3rd operand + imm in 4th (shift 2)
constexpr unsigned ADDR_REG_IMMSHIFT_3_SHIFT2_TABLE[] = {
    // unsigned imm8
    llvm::ARM::t2STREX,
};

constexpr size_t ADDR_REG_IMMSHIFT_3_SHIFT2_SIZE =
    sizeof(ADDR_REG_IMMSHIFT_3_SHIFT2_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec
ADDR_REG_IMMSHIFT_3_SHIFT2_FN(const Patch &patch, Reg dest, bool writeAccess) {
  return ADDR_REG_IMMSHIFT_FN(patch, dest, 2, 3, 2);
}

/* Address in implicit reg + OFFSET
 * ================================
 */

RelocatableInst::UniquePtrVec
ADDR_REG_IMPLICIT_OFF_FN(const Patch &patch, Reg dest, Reg base, sword offset) {
  const LLVMCPU &llvmcpu = *patch.llvmcpu;

  if (offset == 0) {
    return conv_unique<RelocatableInst>(MovReg::unique(dest, base));
  } else {
    return Addc(llvmcpu, dest, base, offset, dest);
  }
}

// address in SP
constexpr unsigned ADDR_REG_SP_TABLE[] = {
    llvm::ARM::tPOP,
};

constexpr size_t ADDR_REG_SP_SIZE =
    sizeof(ADDR_REG_SP_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec ADDR_REG_SP_FN(const Patch &patch, Reg dest,
                                             bool writeAccess) {
  return ADDR_REG_IMPLICIT_OFF_FN(patch, dest, Reg(REG_SP), 0);
}

// address in SP - dynamic
constexpr unsigned ADDR_REG_SP_DYN_TABLE[] = {
    llvm::ARM::tPUSH,
};

constexpr size_t ADDR_REG_SP_DYN_SIZE =
    sizeof(ADDR_REG_SP_DYN_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec ADDR_REG_SP_DYN_FN(const Patch &patch, Reg dest,
                                                 bool writeAccess) {
  const LLVMCPU &llvmcpu = *patch.llvmcpu;
  const llvm::MCInst &inst = patch.metadata.inst;

  if (writeAccess) {
    return ADDR_REG_IMPLICIT_OFF_FN(patch, dest, Reg(REG_SP),
                                    -getWriteSize(inst, llvmcpu));
  } else {
    return ADDR_REG_IMPLICIT_OFF_FN(patch, dest, Reg(REG_SP),
                                    -getReadSize(inst, llvmcpu));
  }
}

/* Address in implicit PC + OFFSET
 * ===============================
 */

RelocatableInst::UniquePtrVec ADDR_REG_IMPLICIT_PC_OFF_FN(const Patch &patch,
                                                          Reg dest,
                                                          unsigned operandOff,
                                                          bool PCAlign) {
  const LLVMCPU &llvmcpu = *patch.llvmcpu;
  const llvm::MCInst &inst = patch.metadata.inst;
  rword address = patch.metadata.address;

  if (PCAlign) {
    address &= (~3);
    QBDI_REQUIRE_ABORT_PATCH(address % 4 == 0, patch, "Bad align");
  }

  QBDI_REQUIRE_ABORT_PATCH(operandOff < inst.getNumOperands(), patch,
                           "Invalid operand");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff).isImm(), patch,
                           "Unexpected operand type");
  sword offset = inst.getOperand(operandOff).getImm();

  if (llvmcpu == CPUMode::Thumb) {
    return conv_unique<RelocatableInst>(
        LoadImm::unique(dest, Constant(address + 4 + offset)));
  } else {
    return conv_unique<RelocatableInst>(
        LoadImm::unique(dest, Constant(address + 8 + offset)));
  }
}

// address in Align(PC, 4) + 2nd operand (imm)
constexpr unsigned ADDR_REG_ALIGNPC_OFF_2_TABLE[] = {
    llvm::ARM::t2LDRBpci,  llvm::ARM::t2LDRHpci, llvm::ARM::t2LDRSBpci,
    llvm::ARM::t2LDRSHpci, llvm::ARM::t2LDRpci,  llvm::ARM::tLDRpci,
};

constexpr size_t ADDR_REG_ALIGNPC_OFF_2_SIZE =
    sizeof(ADDR_REG_ALIGNPC_OFF_2_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec
ADDR_REG_ALIGNPC_OFF_2_FN(const Patch &patch, Reg dest, bool writeAccess) {
  return ADDR_REG_IMPLICIT_PC_OFF_FN(patch, dest, 1, true);
}

struct MemoryAccessInfoArray {
  AddressGenFn *addrFn[28] = {};
  uint8_t addrArr[llvm::ARM::INSTRUCTION_LIST_END] = {0};

  constexpr void addData(size_t index, const unsigned insts[],
                         const size_t instsSize, AddressGenFn *fn) {
    addrFn[index] = fn;
    for (size_t i = 0; i < instsSize; i++) {
      addrArr[insts[i]] = index;
    }
  }

  constexpr MemoryAccessInfoArray() {
    for (size_t i = 0; i < sizeof(addrArr) / sizeof(uint8_t); i++) {
      addrArr[i] = -1;
    }

    uint8_t index = 0;
    addData(index++, ADDR_REG_1_TABLE, ADDR_REG_1_SIZE, ADDR_REG_1_FN);
    addData(index++, ADDR_REG_2_TABLE, ADDR_REG_2_SIZE, ADDR_REG_2_FN);
    addData(index++, ADDR_REG_3_TABLE, ADDR_REG_3_SIZE, ADDR_REG_3_FN);
    addData(index++, ADDR_REG_4_TABLE, ADDR_REG_4_SIZE, ADDR_REG_4_FN);
    addData(index++, ADDR_REG_5_TABLE, ADDR_REG_5_SIZE, ADDR_REG_5_FN);
    addData(index++, ADDR_REG_6_TABLE, ADDR_REG_6_SIZE, ADDR_REG_6_FN);
    addData(index++, ADDR_REG_1_PLUS4_TABLE, ADDR_REG_1_PLUS4_SIZE,
            ADDR_REG_1_PLUS4_FN);
    addData(index++, ADDR_REG_1_DYN_TABLE, ADDR_REG_1_DYN_SIZE,
            ADDR_REG_1_DYN_FN);
    addData(index++, ADDR_REG_1_DYN_PLUS4_TABLE, ADDR_REG_1_DYN_PLUS4_SIZE,
            ADDR_REG_1_DYN_PLUS4_FN);
    addData(index++, ADDR_REG_1_SIMM_2_TABLE, ADDR_REG_1_SIMM_2_SIZE,
            ADDR_REG_1_SIMM_2_FN);
    addData(index++, ADDR_REG_2_SIMM_3_TABLE, ADDR_REG_2_SIMM_3_SIZE,
            ADDR_REG_2_SIMM_3_FN);
    addData(index++, ADDR_REG_3_SIMM_4_TABLE, ADDR_REG_3_SIMM_4_SIZE,
            ADDR_REG_3_SIMM_4_FN);
    addData(index++, ADDR_REG_4_SIMM_5_TABLE, ADDR_REG_4_SIMM_5_SIZE,
            ADDR_REG_4_SIMM_5_FN);
    addData(index++, ADDR_REG_1_REG_2_TABLE, ADDR_REG_1_REG_2_SIZE,
            ADDR_REG_1_REG_2_FN);
    addData(index++, ADDR_REG_2_REG_3_TABLE, ADDR_REG_2_REG_3_SIZE,
            ADDR_REG_2_REG_3_FN);
    addData(index++, ADDR_REG_1_REGSHIFT1_2_TABLE, ADDR_REG_1_REGSHIFT1_2_SIZE,
            ADDR_REG_1_REGSHIFT1_2_FN);
    addData(index++, ADDR_REG_2_REGLSL_3_TABLE, ADDR_REG_2_REGLSL_3_SIZE,
            ADDR_REG_2_REGLSL_3_FN);
    addData(index++, ADDR_REG_2_REGSHIFT_3_TABLE, ADDR_REG_2_REGSHIFT_3_SIZE,
            ADDR_REG_2_REGSHIFT_3_FN);
    addData(index++, ADDR_REG_3_REGSHIFT_4_TABLE, ADDR_REG_3_REGSHIFT_4_SIZE,
            ADDR_REG_3_REGSHIFT_4_FN);
    addData(index++, ADDR_REG_IMMORREG_2_TABLE, ADDR_REG_IMMORREG_2_SIZE,
            ADDR_REG_IMMORREG_2_FN);
    addData(index++, ADDR_REG_IMMORREG_3_TABLE, ADDR_REG_IMMORREG_3_SIZE,
            ADDR_REG_IMMORREG_3_FN);
    addData(index++, ADDR_REG_IMMORREG_4_TABLE, ADDR_REG_IMMORREG_4_SIZE,
            ADDR_REG_IMMORREG_4_FN);
    addData(index++, ADDR_REG_IMMSHIFT_2_SHIFT1_TABLE,
            ADDR_REG_IMMSHIFT_2_SHIFT1_SIZE, ADDR_REG_IMMSHIFT_2_SHIFT1_FN);
    addData(index++, ADDR_REG_IMMSHIFT_2_SHIFT2_TABLE,
            ADDR_REG_IMMSHIFT_2_SHIFT2_SIZE, ADDR_REG_IMMSHIFT_2_SHIFT2_FN);
    addData(index++, ADDR_REG_IMMSHIFT_3_SHIFT2_TABLE,
            ADDR_REG_IMMSHIFT_3_SHIFT2_SIZE, ADDR_REG_IMMSHIFT_3_SHIFT2_FN);
    addData(index++, ADDR_REG_SP_TABLE, ADDR_REG_SP_SIZE, ADDR_REG_SP_FN);
    addData(index++, ADDR_REG_SP_DYN_TABLE, ADDR_REG_SP_DYN_SIZE,
            ADDR_REG_SP_DYN_FN);
    addData(index++, ADDR_REG_ALIGNPC_OFF_2_TABLE, ADDR_REG_ALIGNPC_OFF_2_SIZE,
            ADDR_REG_ALIGNPC_OFF_2_FN);
  }
};

constexpr MemoryAccessInfoArray memoryAccessInfo;
#if CHECK_MEMORYACCESS_TABLE

struct AddressGenerator {
  const unsigned *insts;
  size_t nbInsts;
  AddressGenFn *fn;
};

int checkTable() {
  const std::vector<AddressGenerator> addrInfo = {
      {ADDR_REG_1_TABLE, ADDR_REG_1_SIZE, ADDR_REG_1_FN},
      {ADDR_REG_2_TABLE, ADDR_REG_2_SIZE, ADDR_REG_2_FN},
      {ADDR_REG_3_TABLE, ADDR_REG_3_SIZE, ADDR_REG_3_FN},
      {ADDR_REG_4_TABLE, ADDR_REG_4_SIZE, ADDR_REG_4_FN},
      {ADDR_REG_5_TABLE, ADDR_REG_5_SIZE, ADDR_REG_5_FN},
      {ADDR_REG_6_TABLE, ADDR_REG_6_SIZE, ADDR_REG_6_FN},
      {ADDR_REG_1_PLUS4_TABLE, ADDR_REG_1_PLUS4_SIZE, ADDR_REG_1_PLUS4_FN},
      {ADDR_REG_1_DYN_TABLE, ADDR_REG_1_DYN_SIZE, ADDR_REG_1_DYN_FN},
      {ADDR_REG_1_DYN_PLUS4_TABLE, ADDR_REG_1_DYN_PLUS4_SIZE,
       ADDR_REG_1_DYN_PLUS4_FN},
      {ADDR_REG_1_SIMM_2_TABLE, ADDR_REG_1_SIMM_2_SIZE, ADDR_REG_1_SIMM_2_FN},
      {ADDR_REG_2_SIMM_3_TABLE, ADDR_REG_2_SIMM_3_SIZE, ADDR_REG_2_SIMM_3_FN},
      {ADDR_REG_3_SIMM_4_TABLE, ADDR_REG_3_SIMM_4_SIZE, ADDR_REG_3_SIMM_4_FN},
      {ADDR_REG_4_SIMM_5_TABLE, ADDR_REG_4_SIMM_5_SIZE, ADDR_REG_4_SIMM_5_FN},
      {ADDR_REG_1_REG_2_TABLE, ADDR_REG_1_REG_2_SIZE, ADDR_REG_1_REG_2_FN},
      {ADDR_REG_2_REG_3_TABLE, ADDR_REG_2_REG_3_SIZE, ADDR_REG_2_REG_3_FN},
      {ADDR_REG_1_REGSHIFT1_2_TABLE, ADDR_REG_1_REGSHIFT1_2_SIZE,
       ADDR_REG_1_REGSHIFT1_2_FN},
      {ADDR_REG_2_REGLSL_3_TABLE, ADDR_REG_2_REGLSL_3_SIZE,
       ADDR_REG_2_REGLSL_3_FN},
      {ADDR_REG_2_REGSHIFT_3_TABLE, ADDR_REG_2_REGSHIFT_3_SIZE,
       ADDR_REG_2_REGSHIFT_3_FN},
      {ADDR_REG_3_REGSHIFT_4_TABLE, ADDR_REG_3_REGSHIFT_4_SIZE,
       ADDR_REG_3_REGSHIFT_4_FN},
      {ADDR_REG_IMMORREG_2_TABLE, ADDR_REG_IMMORREG_2_SIZE,
       ADDR_REG_IMMORREG_2_FN},
      {ADDR_REG_IMMORREG_3_TABLE, ADDR_REG_IMMORREG_3_SIZE,
       ADDR_REG_IMMORREG_3_FN},
      {ADDR_REG_IMMORREG_4_TABLE, ADDR_REG_IMMORREG_4_SIZE,
       ADDR_REG_IMMORREG_4_FN},
      {ADDR_REG_IMMSHIFT_2_SHIFT1_TABLE, ADDR_REG_IMMSHIFT_2_SHIFT1_SIZE,
       ADDR_REG_IMMSHIFT_2_SHIFT1_FN},
      {ADDR_REG_IMMSHIFT_2_SHIFT2_TABLE, ADDR_REG_IMMSHIFT_2_SHIFT2_SIZE,
       ADDR_REG_IMMSHIFT_2_SHIFT2_FN},
      {ADDR_REG_IMMSHIFT_3_SHIFT2_TABLE, ADDR_REG_IMMSHIFT_3_SHIFT2_SIZE,
       ADDR_REG_IMMSHIFT_3_SHIFT2_FN},
      {ADDR_REG_SP_TABLE, ADDR_REG_SP_SIZE, ADDR_REG_SP_FN},
      {ADDR_REG_SP_DYN_TABLE, ADDR_REG_SP_DYN_SIZE, ADDR_REG_SP_DYN_FN},
      {ADDR_REG_ALIGNPC_OFF_2_TABLE, ADDR_REG_ALIGNPC_OFF_2_SIZE,
       ADDR_REG_ALIGNPC_OFF_2_FN},
  };

  const LLVMCPUs llvmcpus{"", {}, Options::NO_OPT};
  const LLVMCPU &llvmcpu = llvmcpus.getCPU(CPUMode::DEFAULT);
  const llvm::MCInstrInfo &MCII = llvmcpu.getMCII();

  for (unsigned op = 0; op < llvm::ARM::INSTRUCTION_LIST_END; op++) {
    llvm::MCInst inst;
    inst.setOpcode(op);
    const char *opcode = MCII.getName(inst.getOpcode()).data();

    if (getReadSize(inst, llvmcpu) != 0 || getWriteSize(inst, llvmcpu) != 0) {
      if (memoryAccessInfo.addrArr[op] == ((uint8_t)-1)) {
        fprintf(stderr,
                "[MemoryAccessInfoArray checkTable] "
                "opcode %s doesn't have an associated lambda\n",
                opcode);
        abort();
      }
      unsigned index = 0;
      for (const auto &e : addrInfo) {
        for (size_t i = 0; i < e.nbInsts; i++) {
          unsigned o = e.insts[i];
          if (o == op && memoryAccessInfo.addrArr[op] != index) {
            fprintf(
                stderr,
                "[MemoryAccessInfoArray checkTable] "
                "opcode %s associated with lambda %d but expected lambda %d\n",
                opcode, memoryAccessInfo.addrArr[op], index);
            abort();
          }
          if (o == op &&
              memoryAccessInfo.addrFn[memoryAccessInfo.addrArr[op]] != e.fn) {
            fprintf(stderr,
                    "[MemoryAccessInfoArray checkTable] "
                    "unexpected lambda association for opcode %s : %p != %p\n",
                    opcode,
                    memoryAccessInfo.addrFn[memoryAccessInfo.addrArr[op]],
                    e.fn);
            abort();
          }
        }
        index++;
      }
    } else {
      if (memoryAccessInfo.addrArr[op] != ((uint8_t)-1)) {
        fprintf(stderr,
                "[MemoryAccessInfoArray checkTable] "
                "opcode %s have an associated lambda but doesn't have an "
                "associated memory access size\n",
                opcode);
        abort();
      }
    }
  }
  return 0;
}

int __check_debug = checkTable();
#endif

} // anonymous namespace

RelocatableInst::UniquePtrVec generateAddressPatch(const Patch &patch,
                                                   bool writeAccess, Reg dest) {
  const LLVMCPU &llvmcpu = *patch.llvmcpu;
  const llvm::MCInst &inst = patch.metadata.inst;

  if (writeAccess) {
    QBDI_REQUIRE(getWriteSize(inst, llvmcpu) != 0);
  } else {
    QBDI_REQUIRE(getReadSize(inst, llvmcpu) != 0);
  }
  const uint8_t index = memoryAccessInfo.addrArr[inst.getOpcode()];
  QBDI_REQUIRE(index != ((uint8_t)-1));

  return memoryAccessInfo.addrFn[index](patch, dest, writeAccess);
}

// Generate dynamic PatchGenerator for instruction
// ===============================================

namespace {

enum MemoryTag : uint16_t {
  MEN_COND_REACH_TAG = MEMORY_TAG_BEGIN + 0,

  MEM_READ_ADDRESS_TAG = MEMORY_TAG_BEGIN + 1,
  MEM_WRITE_ADDRESS_TAG = MEMORY_TAG_BEGIN + 2,

  MEM_READ_VALUE_TAG = MEMORY_TAG_BEGIN + 3,
  MEM_WRITE_VALUE_TAG = MEMORY_TAG_BEGIN + 4,
  MEM_VALUE_EXTENDED_TAG = MEMORY_TAG_BEGIN + 5,
};

const PatchGenerator::UniquePtrVec &
generateReadInstrumentPatch(Patch &patch, const LLVMCPU &llvmcpu) {
  switch (getReadSize(patch.metadata.inst, llvmcpu)) {
    case 1:
    case 2:
    case 4: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
          SetCondReachAndJump::unique(
              Temp(1), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  GetReadValue::unique(Temp(0), Temp(0), 0),
                  WriteTemp::unique(Temp(0), Shadow(MEM_READ_VALUE_TAG)))));
      return r;
    }
    case 3: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
          SetCondReachAndJump::unique(
              Temp(1), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  GetReadValue::unique(Temp(1), Temp(0), 0),
                  WriteTemp::unique(Temp(1), Shadow(MEM_READ_VALUE_TAG)))));
      return r;
    }
    case 6:
    case 8: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
          SetCondReachAndJump::unique(
              Temp(1), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  GetReadValue::unique(Temp(1), Temp(0), 0),
                  WriteTemp::unique(Temp(1), Shadow(MEM_READ_VALUE_TAG)),
                  GetReadValue::unique(Temp(1), Temp(0), 1),
                  WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)))));
      return r;
    }
    case 12: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
          SetCondReachAndJump::unique(
              Temp(1), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_READ_VALUE_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  GetReadValue::unique(Temp(1), Temp(0), 2),
                  WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)))));
      return r;
    }
    case 16: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
          SetCondReachAndJump::unique(
              Temp(1), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_READ_VALUE_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)))));
      return r;
    }
    case 20: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
          SetCondReachAndJump::unique(
              Temp(1), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_READ_VALUE_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  GetReadValue::unique(Temp(1), Temp(0), 4),
                  WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)))));
      return r;
    }
    case 24: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
          SetCondReachAndJump::unique(
              Temp(1), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_READ_VALUE_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)))));
      return r;
    }
    case 28: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
          SetCondReachAndJump::unique(
              Temp(1), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_READ_VALUE_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  GetReadValue::unique(Temp(1), Temp(0), 6),
                  WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)))));
      return r;
    }
    case 32: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
          SetCondReachAndJump::unique(
              Temp(1), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_READ_VALUE_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)))));
      return r;
    }
    case 36: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
          SetCondReachAndJump::unique(
              Temp(1), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_READ_VALUE_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  GetReadValue::unique(Temp(1), Temp(0), 8),
                  WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)))));
      return r;
    }
    case 40: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
          SetCondReachAndJump::unique(
              Temp(1), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_READ_VALUE_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)))));
      return r;
    }
    case 44: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
          SetCondReachAndJump::unique(
              Temp(1), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_READ_VALUE_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  GetReadValue::unique(Temp(1), Temp(0), 10),
                  WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)))));
      return r;
    }
    case 48: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
          SetCondReachAndJump::unique(
              Temp(1), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_READ_VALUE_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)))));
      return r;
    }
    case 52: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
          SetCondReachAndJump::unique(
              Temp(1), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_READ_VALUE_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  GetReadValue::unique(Temp(1), Temp(0), 12),
                  WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)))));
      return r;
    }
    case 56: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
          SetCondReachAndJump::unique(
              Temp(1), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_READ_VALUE_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)))));
      return r;
    }
    case 60: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
          SetCondReachAndJump::unique(
              Temp(1), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_READ_VALUE_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  GetReadValue::unique(Temp(1), Temp(0), 14),
                  WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)))));
      return r;
    }
    case 64: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
          SetCondReachAndJump::unique(
              Temp(1), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_READ_VALUE_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)))));
      return r;
    }
    case 68:
    case 72:
    case 76:
    case 80:
    case 84:
    case 88:
    case 92:
    case 96:
    case 100:
    case 104:
    case 108:
    case 112:
    case 116:
    case 120:
    case 124:
    case 128: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
          SetCondReachAndJump::unique(Temp(0), Shadow(MEN_COND_REACH_TAG),
                                      PatchGenerator::UniquePtrVec()));
      return r;
    }

    default:
      QBDI_ABORT_PATCH(patch, "Unexpected number of memory Access {}",
                       getReadSize(patch.metadata.inst, llvmcpu));
  }
}

const PatchGenerator::UniquePtrVec &
generatePreWriteInstrumentPatch(Patch &patch, const LLVMCPU &llvmcpu) {

  static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
      GetWrittenAddress::unique(Temp(0)),
      WriteTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)));
  return r;
}

const PatchGenerator::UniquePtrVec &
generatePostWriteInstrumentPatch(Patch &patch, const LLVMCPU &llvmcpu) {
  switch (getWriteSize(patch.metadata.inst, llvmcpu)) {
    case 1:
    case 2:
    case 4: {
      static const PatchGenerator::UniquePtrVec r =
          conv_unique<PatchGenerator>(SetCondReachAndJump::unique(
              Temp(0), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  ReadTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
                  GetWrittenValue::unique(Temp(0), Temp(0), 0),
                  WriteTemp::unique(Temp(0), Shadow(MEM_WRITE_VALUE_TAG)))));
      return r;
    }
    case 3: {
      static const PatchGenerator::UniquePtrVec r =
          conv_unique<PatchGenerator>(SetCondReachAndJump::unique(
              Temp(0), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  ReadTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
                  GetWrittenValue::unique(Temp(1), Temp(0), 0),
                  WriteTemp::unique(Temp(1), Shadow(MEM_WRITE_VALUE_TAG)))));
      return r;
    }
    case 6:
    case 8: {
      static const PatchGenerator::UniquePtrVec r =
          conv_unique<PatchGenerator>(SetCondReachAndJump::unique(
              Temp(0), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  ReadTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
                  GetWrittenValue::unique(Temp(1), Temp(0), 0),
                  WriteTemp::unique(Temp(1), Shadow(MEM_WRITE_VALUE_TAG)),
                  GetWrittenValue::unique(Temp(1), Temp(0), 1),
                  WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)))));
      return r;
    }
    case 12: {
      static const PatchGenerator::UniquePtrVec r =
          conv_unique<PatchGenerator>(SetCondReachAndJump::unique(
              Temp(0), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  ReadTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_WRITE_VALUE_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  GetWrittenValue::unique(Temp(1), Temp(0), 2),
                  WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)))));
      return r;
    }
    case 16: {
      static const PatchGenerator::UniquePtrVec r =
          conv_unique<PatchGenerator>(SetCondReachAndJump::unique(
              Temp(0), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  ReadTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_WRITE_VALUE_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)))));
      return r;
    }
    case 20: {
      static const PatchGenerator::UniquePtrVec r =
          conv_unique<PatchGenerator>(SetCondReachAndJump::unique(
              Temp(0), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  ReadTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_WRITE_VALUE_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  GetWrittenValue::unique(Temp(1), Temp(0), 4),
                  WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)))));
      return r;
    }
    case 24: {
      static const PatchGenerator::UniquePtrVec r =
          conv_unique<PatchGenerator>(SetCondReachAndJump::unique(
              Temp(0), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  ReadTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_WRITE_VALUE_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)))));
      return r;
    }
    case 28: {
      static const PatchGenerator::UniquePtrVec r =
          conv_unique<PatchGenerator>(SetCondReachAndJump::unique(
              Temp(0), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  ReadTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_WRITE_VALUE_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  GetWrittenValue::unique(Temp(1), Temp(0), 6),
                  WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)))));
      return r;
    }
    case 32: {
      static const PatchGenerator::UniquePtrVec r =
          conv_unique<PatchGenerator>(SetCondReachAndJump::unique(
              Temp(0), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  ReadTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_WRITE_VALUE_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)))));
      return r;
    }
    case 36: {
      static const PatchGenerator::UniquePtrVec r =
          conv_unique<PatchGenerator>(SetCondReachAndJump::unique(
              Temp(0), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  ReadTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_WRITE_VALUE_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  GetWrittenValue::unique(Temp(1), Temp(0), 8),
                  WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)))));
      return r;
    }
    case 40: {
      static const PatchGenerator::UniquePtrVec r =
          conv_unique<PatchGenerator>(SetCondReachAndJump::unique(
              Temp(0), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  ReadTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_WRITE_VALUE_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)))));
      return r;
    }
    case 44: {
      static const PatchGenerator::UniquePtrVec r =
          conv_unique<PatchGenerator>(SetCondReachAndJump::unique(
              Temp(0), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  ReadTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_WRITE_VALUE_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  GetWrittenValue::unique(Temp(1), Temp(0), 10),
                  WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)))));
      return r;
    }
    case 48: {
      static const PatchGenerator::UniquePtrVec r =
          conv_unique<PatchGenerator>(SetCondReachAndJump::unique(
              Temp(0), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  ReadTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_WRITE_VALUE_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)))));
      return r;
    }
    case 52: {
      static const PatchGenerator::UniquePtrVec r =
          conv_unique<PatchGenerator>(SetCondReachAndJump::unique(
              Temp(0), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  ReadTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_WRITE_VALUE_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  GetWrittenValue::unique(Temp(1), Temp(0), 12),
                  WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)))));
      return r;
    }
    case 56: {
      static const PatchGenerator::UniquePtrVec r =
          conv_unique<PatchGenerator>(SetCondReachAndJump::unique(
              Temp(0), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  ReadTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_WRITE_VALUE_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)))));
      return r;
    }
    case 60: {
      static const PatchGenerator::UniquePtrVec r =
          conv_unique<PatchGenerator>(SetCondReachAndJump::unique(
              Temp(0), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  ReadTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_WRITE_VALUE_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  GetWrittenValue::unique(Temp(1), Temp(0), 14),
                  WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)))));
      return r;
    }
    case 64: {
      static const PatchGenerator::UniquePtrVec r =
          conv_unique<PatchGenerator>(SetCondReachAndJump::unique(
              Temp(0), Shadow(MEN_COND_REACH_TAG),
              conv_unique<PatchGenerator>(
                  ReadTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_WRITE_VALUE_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)),
                  BackupValueX2::unique(Temp(1), Temp(2), Temp(0),
                                        Shadow(MEM_VALUE_EXTENDED_TAG),
                                        Shadow(MEM_VALUE_EXTENDED_TAG)))));
      return r;
    }
    case 68:
    case 72:
    case 76:
    case 80:
    case 84:
    case 88:
    case 92:
    case 96:
    case 100:
    case 104:
    case 108:
    case 112:
    case 116:
    case 120:
    case 124:
    case 128: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          SetCondReachAndJump::unique(Temp(0), Shadow(MEN_COND_REACH_TAG),
                                      PatchGenerator::UniquePtrVec()));
      return r;
    }

    default:
      QBDI_ABORT_PATCH(patch, "Unexpected number of memory Access {}",
                       getWriteSize(patch.metadata.inst, llvmcpu));
  }
}

} // anonymous namespace

std::vector<std::unique_ptr<InstrRule>> getInstrRuleMemAccessRead() {
  return conv_unique<InstrRule>(InstrRuleDynamic::unique(
      DoesReadAccess::unique(), generateReadInstrumentPatch, PREINST, false,
      PRIORITY_MEMACCESS_LIMIT + 1, RelocTagPreInstMemAccess));
}

std::vector<std::unique_ptr<InstrRule>> getInstrRuleMemAccessWrite() {
  return conv_unique<InstrRule>(
      InstrRuleDynamic::unique(
          DoesWriteAccess::unique(), generatePreWriteInstrumentPatch, PREINST,
          false, PRIORITY_MEMACCESS_LIMIT, RelocTagPreInstMemAccess),
      InstrRuleDynamic::unique(
          DoesWriteAccess::unique(), generatePostWriteInstrumentPatch, POSTINST,
          false, PRIORITY_MEMACCESS_LIMIT, RelocTagPostInstMemAccess));
}

// Analyse MemoryAccess from Shadow
// ================================

namespace {

void analyseMemoryAccessAddrValue(const ExecBlock &curExecBlock,
                                  llvm::ArrayRef<ShadowInfo> &shadows,
                                  std::vector<MemoryAccess> &dest,
                                  const LLVMCPU &llvmcpu) {
  if (shadows.size() < 1) {
    return;
  }

  auto access = MemoryAccess();
  access.flags = MEMORY_NO_FLAGS;

  uint16_t expectValueTag;
  const llvm::MCInst &inst = curExecBlock.getOriginalMCInst(shadows[0].instID);

  switch (shadows[0].tag) {
    default:
      return;
    case MEM_READ_ADDRESS_TAG:
      access.type = MEMORY_READ;
      access.size = getReadSize(inst, llvmcpu);
      expectValueTag = MEM_READ_VALUE_TAG;
      break;
    case MEM_WRITE_ADDRESS_TAG:
      access.type = MEMORY_WRITE;
      access.size = getWriteSize(inst, llvmcpu);
      expectValueTag = MEM_WRITE_VALUE_TAG;
      break;
  }

  access.accessAddress = curExecBlock.getShadow(shadows[0].shadowID);
  access.instAddress = curExecBlock.getInstAddress(shadows[0].instID);

  if (access.size > 64) {
    access.value = 0;
    access.flags |= MEMORY_UNKNOWN_VALUE;
    // search if the shadow MEN_COND_REACH_TAG is present
    // drop the access if the condition of the instruction isn't reached.
    for (const ShadowInfo &info : shadows) {
      if (shadows[0].instID != info.instID) {
        break;
      }
      if (info.tag == MEN_COND_REACH_TAG) {
        if (curExecBlock.getShadow(info.shadowID) != 1) {
          return;
        }
        break;
      }
    }
    dest.push_back(access);
    return;
  }

  size_t index = 0;
  // search the index of MEM_x_VALUE_TAG. For most instruction, it's the next
  // shadow.
  do {
    index += 1;
    if (index >= shadows.size()) {
      QBDI_ERROR("Not found shadow tag {:x} for instruction {:x}",
                 expectValueTag, access.instAddress);
      return;
    }
    QBDI_REQUIRE_ACTION(shadows[0].instID == shadows[index].instID, return );

    // if the instruction is conditionnal and the condition hasn't be reach,
    //  drop the shadows.
    if (shadows[index].tag == MEN_COND_REACH_TAG and
        curExecBlock.getShadow(shadows[index].shadowID) != 1)
      return;
  } while (shadows[index].tag != expectValueTag);

  access.value = curExecBlock.getShadow(shadows[index].shadowID);

  if (access.size < sizeof(rword)) {
    rword mask = (1ull << (access.size * 8)) - 1;
    access.value &= mask;
  }

  size_t extendShadow = 0;
  size_t remindSize = access.size;

  if (access.size > sizeof(rword)) {
    extendShadow = (access.size / sizeof(rword));
    if (access.size % sizeof(rword) == 0 && extendShadow > 0) {
      --extendShadow;
    }
    access.size = sizeof(rword);
    ++index;
  }

  dest.push_back(access);

  for (; extendShadow > 0; --extendShadow, ++index) {
    QBDI_REQUIRE_ACTION(index < shadows.size(), return );
    QBDI_REQUIRE_ACTION(shadows[0].instID == shadows[index].instID, return );
    QBDI_REQUIRE_ACTION(shadows[index].tag == MEM_VALUE_EXTENDED_TAG, return );

    access.accessAddress += sizeof(rword);
    access.value = curExecBlock.getShadow(shadows[index].shadowID);
    remindSize -= sizeof(rword);
    if (remindSize < sizeof(rword)) {
      access.size = remindSize;
      rword mask = (1ull << (access.size * 8)) - 1;
      access.value &= mask;
    }
    dest.push_back(access);
  }
}

} // anonymous namespace

void analyseMemoryAccess(const ExecBlock &curExecBlock, uint16_t instID,
                         bool afterInst, std::vector<MemoryAccess> &dest) {

  llvm::ArrayRef<ShadowInfo> shadows = curExecBlock.getShadowByInst(instID);
  const LLVMCPU &llvmcpu = curExecBlock.getLLVMCPUByInst(instID);
  QBDI_DEBUG("Got {} shadows for Instruction {:x}", shadows.size(), instID);

  while (!shadows.empty()) {
    QBDI_REQUIRE_ACTION(shadows[0].instID == instID, return );

    switch (shadows[0].tag) {
      default:
        break;
      case MEN_COND_REACH_TAG:
        // if the instruction is conditionnal and the condition hasn't be reach,
        //  drop the shadows.
        if (curExecBlock.getShadow(shadows[0].shadowID) != 1)
          return;
        break;
      case MEM_READ_ADDRESS_TAG:
        analyseMemoryAccessAddrValue(curExecBlock, shadows, dest, llvmcpu);
        break;
      case MEM_WRITE_ADDRESS_TAG:
        if (afterInst) {
          analyseMemoryAccessAddrValue(curExecBlock, shadows, dest, llvmcpu);
        }
        break;
    }
    shadows = shadows.drop_front();
  }
}

} // namespace QBDI
