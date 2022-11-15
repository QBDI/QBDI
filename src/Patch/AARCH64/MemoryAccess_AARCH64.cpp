/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2022 Quarkslab
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

#include "llvm/ADT/ArrayRef.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"

#include "AArch64InstrInfo.h"
#include "llvm/MC/MCInstrInfo.h"

#include "devVariable.h"
#include "Engine/LLVMCPU.h"
#include "ExecBlock/ExecBlock.h"
#include "Patch/AARCH64/InstInfo_AARCH64.h"
#include "Patch/AARCH64/Layer2_AARCH64.h"
#include "Patch/AARCH64/MemoryAccess_AARCH64.h"
#include "Patch/AARCH64/PatchCondition_AARCH64.h"
#include "Patch/AARCH64/PatchGenerator_AARCH64.h"
#include "Patch/AARCH64/RelocatableInst_AARCH64.h"
#include "Patch/InstInfo.h"
#include "Patch/MemoryAccess.h"
#include "Patch/Patch.h"
#include "Patch/PatchCondition.h"
#include "Patch/PatchGenerator.h"
#include "Patch/PatchUtils.h"
#include "Patch/Register.h"
#include "Patch/RelocatableInst.h"
#include "Patch/Types.h"
#include "Utility/LogSys.h"

#include "QBDI/Bitmask.h"
#include "QBDI/Callback.h"
#include "QBDI/State.h"

namespace QBDI {

// PatchGenerator MemoryAccess Address/ReadValue/WriteValue Generator
// ==================================================================

namespace {

typedef RelocatableInst::UniquePtrVec(AddressGenFn)(const Patch &patch,
                                                    bool writeAccess, Reg dest);

/* Address in a register
 * =====================
 */
RelocatableInst::UniquePtrVec ADDR_REGISTER_FN(const Patch &patch,
                                               bool writeAccess, Reg dest,
                                               unsigned operandOff) {
  const llvm::MCInst &inst = patch.metadata.inst;

  QBDI_REQUIRE_ABORT_PATCH(operandOff < inst.getNumOperands(), patch,
                           "Invalid operand {}", operandOff);
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff).isReg(), patch,
                           "Unexpected operand type");
  RegLLVM addrReg = inst.getOperand(operandOff).getReg();
  return conv_unique<RelocatableInst>(MovReg::unique(dest, addrReg));
}

// address on the 2nd operand
constexpr unsigned ADDR_REGISTER_2_TABLE[] = {
    // ld1	{ v28.4s }, [x0]
    llvm::AArch64::LD1Fourv16b,  llvm::AArch64::LD1Fourv1d,
    llvm::AArch64::LD1Fourv2d,   llvm::AArch64::LD1Fourv2s,
    llvm::AArch64::LD1Fourv4h,   llvm::AArch64::LD1Fourv4s,
    llvm::AArch64::LD1Fourv8b,   llvm::AArch64::LD1Fourv8h,
    llvm::AArch64::LD1Onev16b,   llvm::AArch64::LD1Onev1d,
    llvm::AArch64::LD1Onev2d,    llvm::AArch64::LD1Onev2s,
    llvm::AArch64::LD1Onev4h,    llvm::AArch64::LD1Onev4s,
    llvm::AArch64::LD1Onev8b,    llvm::AArch64::LD1Onev8h,
    llvm::AArch64::LD1Rv16b,     llvm::AArch64::LD1Rv1d,
    llvm::AArch64::LD1Rv2d,      llvm::AArch64::LD1Rv2s,
    llvm::AArch64::LD1Rv4h,      llvm::AArch64::LD1Rv4s,
    llvm::AArch64::LD1Rv8b,      llvm::AArch64::LD1Rv8h,
    llvm::AArch64::LD1Threev16b, llvm::AArch64::LD1Threev1d,
    llvm::AArch64::LD1Threev2d,  llvm::AArch64::LD1Threev2s,
    llvm::AArch64::LD1Threev4h,  llvm::AArch64::LD1Threev4s,
    llvm::AArch64::LD1Threev8b,  llvm::AArch64::LD1Threev8h,
    llvm::AArch64::LD1Twov16b,   llvm::AArch64::LD1Twov1d,
    llvm::AArch64::LD1Twov2d,    llvm::AArch64::LD1Twov2s,
    llvm::AArch64::LD1Twov4h,    llvm::AArch64::LD1Twov4s,
    llvm::AArch64::LD1Twov8b,    llvm::AArch64::LD1Twov8h,
    llvm::AArch64::LD2Rv16b,     llvm::AArch64::LD2Rv1d,
    llvm::AArch64::LD2Rv2d,      llvm::AArch64::LD2Rv2s,
    llvm::AArch64::LD2Rv4h,      llvm::AArch64::LD2Rv4s,
    llvm::AArch64::LD2Rv8b,      llvm::AArch64::LD2Rv8h,
    llvm::AArch64::LD2Twov16b,   llvm::AArch64::LD2Twov2d,
    llvm::AArch64::LD2Twov2s,    llvm::AArch64::LD2Twov4h,
    llvm::AArch64::LD2Twov4s,    llvm::AArch64::LD2Twov8b,
    llvm::AArch64::LD2Twov8h,    llvm::AArch64::LD3Rv16b,
    llvm::AArch64::LD3Rv1d,      llvm::AArch64::LD3Rv2d,
    llvm::AArch64::LD3Rv2s,      llvm::AArch64::LD3Rv4h,
    llvm::AArch64::LD3Rv4s,      llvm::AArch64::LD3Rv8b,
    llvm::AArch64::LD3Rv8h,      llvm::AArch64::LD3Threev16b,
    llvm::AArch64::LD3Threev2d,  llvm::AArch64::LD3Threev2s,
    llvm::AArch64::LD3Threev4h,  llvm::AArch64::LD3Threev4s,
    llvm::AArch64::LD3Threev8b,  llvm::AArch64::LD3Threev8h,
    llvm::AArch64::LD4Fourv16b,  llvm::AArch64::LD4Fourv2d,
    llvm::AArch64::LD4Fourv2s,   llvm::AArch64::LD4Fourv4h,
    llvm::AArch64::LD4Fourv4s,   llvm::AArch64::LD4Fourv8b,
    llvm::AArch64::LD4Fourv8h,   llvm::AArch64::LD4Rv16b,
    llvm::AArch64::LD4Rv1d,      llvm::AArch64::LD4Rv2d,
    llvm::AArch64::LD4Rv2s,      llvm::AArch64::LD4Rv4h,
    llvm::AArch64::LD4Rv4s,      llvm::AArch64::LD4Rv8b,
    llvm::AArch64::LD4Rv8h,      llvm::AArch64::LD64B,
    llvm::AArch64::LDAPRB,       llvm::AArch64::LDAPRH,
    llvm::AArch64::LDAPRW,       llvm::AArch64::LDAPRX,
    llvm::AArch64::LDARB,        llvm::AArch64::LDARH,
    llvm::AArch64::LDARW,        llvm::AArch64::LDARX,
    llvm::AArch64::LDAXRB,       llvm::AArch64::LDAXRH,
    llvm::AArch64::LDAXRW,       llvm::AArch64::LDAXRX,
    llvm::AArch64::LDLARB,       llvm::AArch64::LDLARH,
    llvm::AArch64::LDLARW,       llvm::AArch64::LDLARX,
    llvm::AArch64::LDXRB,        llvm::AArch64::LDXRH,
    llvm::AArch64::LDXRW,        llvm::AArch64::LDXRX,
    llvm::AArch64::ST1Fourv16b,  llvm::AArch64::ST1Fourv1d,
    llvm::AArch64::ST1Fourv2d,   llvm::AArch64::ST1Fourv2s,
    llvm::AArch64::ST1Fourv4h,   llvm::AArch64::ST1Fourv4s,
    llvm::AArch64::ST1Fourv8b,   llvm::AArch64::ST1Fourv8h,
    llvm::AArch64::ST1Onev16b,   llvm::AArch64::ST1Onev1d,
    llvm::AArch64::ST1Onev2d,    llvm::AArch64::ST1Onev2s,
    llvm::AArch64::ST1Onev4h,    llvm::AArch64::ST1Onev4s,
    llvm::AArch64::ST1Onev8b,    llvm::AArch64::ST1Onev8h,
    llvm::AArch64::ST1Threev16b, llvm::AArch64::ST1Threev1d,
    llvm::AArch64::ST1Threev2d,  llvm::AArch64::ST1Threev2s,
    llvm::AArch64::ST1Threev4h,  llvm::AArch64::ST1Threev4s,
    llvm::AArch64::ST1Threev8b,  llvm::AArch64::ST1Threev8h,
    llvm::AArch64::ST1Twov16b,   llvm::AArch64::ST1Twov1d,
    llvm::AArch64::ST1Twov2d,    llvm::AArch64::ST1Twov2s,
    llvm::AArch64::ST1Twov4h,    llvm::AArch64::ST1Twov4s,
    llvm::AArch64::ST1Twov8b,    llvm::AArch64::ST1Twov8h,
    llvm::AArch64::ST2Twov16b,   llvm::AArch64::ST2Twov2d,
    llvm::AArch64::ST2Twov2s,    llvm::AArch64::ST2Twov4h,
    llvm::AArch64::ST2Twov4s,    llvm::AArch64::ST2Twov8b,
    llvm::AArch64::ST2Twov8h,    llvm::AArch64::ST3Threev16b,
    llvm::AArch64::ST3Threev2d,  llvm::AArch64::ST3Threev2s,
    llvm::AArch64::ST3Threev4h,  llvm::AArch64::ST3Threev4s,
    llvm::AArch64::ST3Threev8b,  llvm::AArch64::ST3Threev8h,
    llvm::AArch64::ST4Fourv16b,  llvm::AArch64::ST4Fourv2d,
    llvm::AArch64::ST4Fourv2s,   llvm::AArch64::ST4Fourv4h,
    llvm::AArch64::ST4Fourv4s,   llvm::AArch64::ST4Fourv8b,
    llvm::AArch64::ST4Fourv8h,   llvm::AArch64::ST64B,
    llvm::AArch64::STLLRB,       llvm::AArch64::STLLRH,
    llvm::AArch64::STLLRW,       llvm::AArch64::STLLRX,
    llvm::AArch64::STLRB,        llvm::AArch64::STLRH,
    llvm::AArch64::STLRW,        llvm::AArch64::STLRX,
};

constexpr size_t ADDR_REGISTER_2_SIZE =
    sizeof(ADDR_REGISTER_2_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec ADDR_REGISTER_2_FN(const Patch &patch,
                                                 bool writeAccess, Reg dest) {
  return ADDR_REGISTER_FN(patch, writeAccess, dest, 1);
}

// address on the 3rd operand
constexpr unsigned ADDR_REGISTER_3_TABLE[] = {
    // swpb w0, w1, [x2]
    llvm::AArch64::LD1Fourv16b_POST,
    llvm::AArch64::LD1Fourv1d_POST,
    llvm::AArch64::LD1Fourv2d_POST,
    llvm::AArch64::LD1Fourv2s_POST,
    llvm::AArch64::LD1Fourv4h_POST,
    llvm::AArch64::LD1Fourv4s_POST,
    llvm::AArch64::LD1Fourv8b_POST,
    llvm::AArch64::LD1Fourv8h_POST,
    llvm::AArch64::LD1Onev16b_POST,
    llvm::AArch64::LD1Onev1d_POST,
    llvm::AArch64::LD1Onev2d_POST,
    llvm::AArch64::LD1Onev2s_POST,
    llvm::AArch64::LD1Onev4h_POST,
    llvm::AArch64::LD1Onev4s_POST,
    llvm::AArch64::LD1Onev8b_POST,
    llvm::AArch64::LD1Onev8h_POST,
    llvm::AArch64::LD1Rv16b_POST,
    llvm::AArch64::LD1Rv1d_POST,
    llvm::AArch64::LD1Rv2d_POST,
    llvm::AArch64::LD1Rv2s_POST,
    llvm::AArch64::LD1Rv4h_POST,
    llvm::AArch64::LD1Rv4s_POST,
    llvm::AArch64::LD1Rv8b_POST,
    llvm::AArch64::LD1Rv8h_POST,
    llvm::AArch64::LD1Threev16b_POST,
    llvm::AArch64::LD1Threev1d_POST,
    llvm::AArch64::LD1Threev2d_POST,
    llvm::AArch64::LD1Threev2s_POST,
    llvm::AArch64::LD1Threev4h_POST,
    llvm::AArch64::LD1Threev4s_POST,
    llvm::AArch64::LD1Threev8b_POST,
    llvm::AArch64::LD1Threev8h_POST,
    llvm::AArch64::LD1Twov16b_POST,
    llvm::AArch64::LD1Twov1d_POST,
    llvm::AArch64::LD1Twov2d_POST,
    llvm::AArch64::LD1Twov2s_POST,
    llvm::AArch64::LD1Twov4h_POST,
    llvm::AArch64::LD1Twov4s_POST,
    llvm::AArch64::LD1Twov8b_POST,
    llvm::AArch64::LD1Twov8h_POST,
    llvm::AArch64::LD2Rv16b_POST,
    llvm::AArch64::LD2Rv1d_POST,
    llvm::AArch64::LD2Rv2d_POST,
    llvm::AArch64::LD2Rv2s_POST,
    llvm::AArch64::LD2Rv4h_POST,
    llvm::AArch64::LD2Rv4s_POST,
    llvm::AArch64::LD2Rv8b_POST,
    llvm::AArch64::LD2Rv8h_POST,
    llvm::AArch64::LD2Twov16b_POST,
    llvm::AArch64::LD2Twov2d_POST,
    llvm::AArch64::LD2Twov2s_POST,
    llvm::AArch64::LD2Twov4h_POST,
    llvm::AArch64::LD2Twov4s_POST,
    llvm::AArch64::LD2Twov8b_POST,
    llvm::AArch64::LD2Twov8h_POST,
    llvm::AArch64::LD3Rv16b_POST,
    llvm::AArch64::LD3Rv1d_POST,
    llvm::AArch64::LD3Rv2d_POST,
    llvm::AArch64::LD3Rv2s_POST,
    llvm::AArch64::LD3Rv4h_POST,
    llvm::AArch64::LD3Rv4s_POST,
    llvm::AArch64::LD3Rv8b_POST,
    llvm::AArch64::LD3Rv8h_POST,
    llvm::AArch64::LD3Threev16b_POST,
    llvm::AArch64::LD3Threev2d_POST,
    llvm::AArch64::LD3Threev2s_POST,
    llvm::AArch64::LD3Threev4h_POST,
    llvm::AArch64::LD3Threev4s_POST,
    llvm::AArch64::LD3Threev8b_POST,
    llvm::AArch64::LD3Threev8h_POST,
    llvm::AArch64::LD4Fourv16b_POST,
    llvm::AArch64::LD4Fourv2d_POST,
    llvm::AArch64::LD4Fourv2s_POST,
    llvm::AArch64::LD4Fourv4h_POST,
    llvm::AArch64::LD4Fourv4s_POST,
    llvm::AArch64::LD4Fourv8b_POST,
    llvm::AArch64::LD4Fourv8h_POST,
    llvm::AArch64::LD4Rv16b_POST,
    llvm::AArch64::LD4Rv1d_POST,
    llvm::AArch64::LD4Rv2d_POST,
    llvm::AArch64::LD4Rv2s_POST,
    llvm::AArch64::LD4Rv4h_POST,
    llvm::AArch64::LD4Rv4s_POST,
    llvm::AArch64::LD4Rv8b_POST,
    llvm::AArch64::LD4Rv8h_POST,
    llvm::AArch64::LDADDAB,
    llvm::AArch64::LDADDAH,
    llvm::AArch64::LDADDALB,
    llvm::AArch64::LDADDALH,
    llvm::AArch64::LDADDALW,
    llvm::AArch64::LDADDALX,
    llvm::AArch64::LDADDAW,
    llvm::AArch64::LDADDAX,
    llvm::AArch64::LDADDB,
    llvm::AArch64::LDADDH,
    llvm::AArch64::LDADDLB,
    llvm::AArch64::LDADDLH,
    llvm::AArch64::LDADDLW,
    llvm::AArch64::LDADDLX,
    llvm::AArch64::LDADDW,
    llvm::AArch64::LDADDX,
    llvm::AArch64::LDAXPW,
    llvm::AArch64::LDAXPX,
    llvm::AArch64::LDCLRAB,
    llvm::AArch64::LDCLRAH,
    llvm::AArch64::LDCLRALB,
    llvm::AArch64::LDCLRALH,
    llvm::AArch64::LDCLRALW,
    llvm::AArch64::LDCLRALX,
    llvm::AArch64::LDCLRAW,
    llvm::AArch64::LDCLRAX,
    llvm::AArch64::LDCLRB,
    llvm::AArch64::LDCLRH,
    llvm::AArch64::LDCLRLB,
    llvm::AArch64::LDCLRLH,
    llvm::AArch64::LDCLRLW,
    llvm::AArch64::LDCLRLX,
    llvm::AArch64::LDCLRW,
    llvm::AArch64::LDCLRX,
    llvm::AArch64::LDEORAB,
    llvm::AArch64::LDEORAH,
    llvm::AArch64::LDEORALB,
    llvm::AArch64::LDEORALH,
    llvm::AArch64::LDEORALW,
    llvm::AArch64::LDEORALX,
    llvm::AArch64::LDEORAW,
    llvm::AArch64::LDEORAX,
    llvm::AArch64::LDEORB,
    llvm::AArch64::LDEORH,
    llvm::AArch64::LDEORLB,
    llvm::AArch64::LDEORLH,
    llvm::AArch64::LDEORLW,
    llvm::AArch64::LDEORLX,
    llvm::AArch64::LDEORW,
    llvm::AArch64::LDEORX,
    llvm::AArch64::LDRBBpost,
    llvm::AArch64::LDRBpost,
    llvm::AArch64::LDRDpost,
    llvm::AArch64::LDRHHpost,
    llvm::AArch64::LDRHpost,
    llvm::AArch64::LDRQpost,
    llvm::AArch64::LDRSBWpost,
    llvm::AArch64::LDRSBXpost,
    llvm::AArch64::LDRSHWpost,
    llvm::AArch64::LDRSHXpost,
    llvm::AArch64::LDRSWpost,
    llvm::AArch64::LDRSpost,
    llvm::AArch64::LDRWpost,
    llvm::AArch64::LDRXpost,
    llvm::AArch64::LDSETAB,
    llvm::AArch64::LDSETAH,
    llvm::AArch64::LDSETALB,
    llvm::AArch64::LDSETALH,
    llvm::AArch64::LDSETALW,
    llvm::AArch64::LDSETALX,
    llvm::AArch64::LDSETAW,
    llvm::AArch64::LDSETAX,
    llvm::AArch64::LDSETB,
    llvm::AArch64::LDSETH,
    llvm::AArch64::LDSETLB,
    llvm::AArch64::LDSETLH,
    llvm::AArch64::LDSETLW,
    llvm::AArch64::LDSETLX,
    llvm::AArch64::LDSETW,
    llvm::AArch64::LDSETX,
    llvm::AArch64::LDSMAXAB,
    llvm::AArch64::LDSMAXAH,
    llvm::AArch64::LDSMAXALB,
    llvm::AArch64::LDSMAXALH,
    llvm::AArch64::LDSMAXALW,
    llvm::AArch64::LDSMAXALX,
    llvm::AArch64::LDSMAXAW,
    llvm::AArch64::LDSMAXAX,
    llvm::AArch64::LDSMAXB,
    llvm::AArch64::LDSMAXH,
    llvm::AArch64::LDSMAXLB,
    llvm::AArch64::LDSMAXLH,
    llvm::AArch64::LDSMAXLW,
    llvm::AArch64::LDSMAXLX,
    llvm::AArch64::LDSMAXW,
    llvm::AArch64::LDSMAXX,
    llvm::AArch64::LDSMINAB,
    llvm::AArch64::LDSMINAH,
    llvm::AArch64::LDSMINALB,
    llvm::AArch64::LDSMINALH,
    llvm::AArch64::LDSMINALW,
    llvm::AArch64::LDSMINALX,
    llvm::AArch64::LDSMINAW,
    llvm::AArch64::LDSMINAX,
    llvm::AArch64::LDSMINB,
    llvm::AArch64::LDSMINH,
    llvm::AArch64::LDSMINLB,
    llvm::AArch64::LDSMINLH,
    llvm::AArch64::LDSMINLW,
    llvm::AArch64::LDSMINLX,
    llvm::AArch64::LDSMINW,
    llvm::AArch64::LDSMINX,
    llvm::AArch64::LDUMAXAB,
    llvm::AArch64::LDUMAXAH,
    llvm::AArch64::LDUMAXALB,
    llvm::AArch64::LDUMAXALH,
    llvm::AArch64::LDUMAXALW,
    llvm::AArch64::LDUMAXALX,
    llvm::AArch64::LDUMAXAW,
    llvm::AArch64::LDUMAXAX,
    llvm::AArch64::LDUMAXB,
    llvm::AArch64::LDUMAXH,
    llvm::AArch64::LDUMAXLB,
    llvm::AArch64::LDUMAXLH,
    llvm::AArch64::LDUMAXLW,
    llvm::AArch64::LDUMAXLX,
    llvm::AArch64::LDUMAXW,
    llvm::AArch64::LDUMAXX,
    llvm::AArch64::LDUMINAB,
    llvm::AArch64::LDUMINAH,
    llvm::AArch64::LDUMINALB,
    llvm::AArch64::LDUMINALH,
    llvm::AArch64::LDUMINALW,
    llvm::AArch64::LDUMINALX,
    llvm::AArch64::LDUMINAW,
    llvm::AArch64::LDUMINAX,
    llvm::AArch64::LDUMINB,
    llvm::AArch64::LDUMINH,
    llvm::AArch64::LDUMINLB,
    llvm::AArch64::LDUMINLH,
    llvm::AArch64::LDUMINLW,
    llvm::AArch64::LDUMINLX,
    llvm::AArch64::LDUMINW,
    llvm::AArch64::LDUMINX,
    llvm::AArch64::LDXPW,
    llvm::AArch64::LDXPX,
    llvm::AArch64::ST1Fourv16b_POST,
    llvm::AArch64::ST1Fourv1d_POST,
    llvm::AArch64::ST1Fourv2d_POST,
    llvm::AArch64::ST1Fourv2s_POST,
    llvm::AArch64::ST1Fourv4h_POST,
    llvm::AArch64::ST1Fourv4s_POST,
    llvm::AArch64::ST1Fourv8b_POST,
    llvm::AArch64::ST1Fourv8h_POST,
    llvm::AArch64::ST1Onev16b_POST,
    llvm::AArch64::ST1Onev1d_POST,
    llvm::AArch64::ST1Onev2d_POST,
    llvm::AArch64::ST1Onev2s_POST,
    llvm::AArch64::ST1Onev4h_POST,
    llvm::AArch64::ST1Onev4s_POST,
    llvm::AArch64::ST1Onev8b_POST,
    llvm::AArch64::ST1Onev8h_POST,
    llvm::AArch64::ST1Threev16b_POST,
    llvm::AArch64::ST1Threev1d_POST,
    llvm::AArch64::ST1Threev2d_POST,
    llvm::AArch64::ST1Threev2s_POST,
    llvm::AArch64::ST1Threev4h_POST,
    llvm::AArch64::ST1Threev4s_POST,
    llvm::AArch64::ST1Threev8b_POST,
    llvm::AArch64::ST1Threev8h_POST,
    llvm::AArch64::ST1Twov16b_POST,
    llvm::AArch64::ST1Twov1d_POST,
    llvm::AArch64::ST1Twov2d_POST,
    llvm::AArch64::ST1Twov2s_POST,
    llvm::AArch64::ST1Twov4h_POST,
    llvm::AArch64::ST1Twov4s_POST,
    llvm::AArch64::ST1Twov8b_POST,
    llvm::AArch64::ST1Twov8h_POST,
    llvm::AArch64::ST1i16,
    llvm::AArch64::ST1i32,
    llvm::AArch64::ST1i64,
    llvm::AArch64::ST1i8,
    llvm::AArch64::ST2Twov16b_POST,
    llvm::AArch64::ST2Twov2d_POST,
    llvm::AArch64::ST2Twov2s_POST,
    llvm::AArch64::ST2Twov4h_POST,
    llvm::AArch64::ST2Twov4s_POST,
    llvm::AArch64::ST2Twov8b_POST,
    llvm::AArch64::ST2Twov8h_POST,
    llvm::AArch64::ST2i16,
    llvm::AArch64::ST2i32,
    llvm::AArch64::ST2i64,
    llvm::AArch64::ST2i8,
    llvm::AArch64::ST3Threev16b_POST,
    llvm::AArch64::ST3Threev2d_POST,
    llvm::AArch64::ST3Threev2s_POST,
    llvm::AArch64::ST3Threev4h_POST,
    llvm::AArch64::ST3Threev4s_POST,
    llvm::AArch64::ST3Threev8b_POST,
    llvm::AArch64::ST3Threev8h_POST,
    llvm::AArch64::ST3i16,
    llvm::AArch64::ST3i32,
    llvm::AArch64::ST3i64,
    llvm::AArch64::ST3i8,
    llvm::AArch64::ST4Fourv16b_POST,
    llvm::AArch64::ST4Fourv2d_POST,
    llvm::AArch64::ST4Fourv2s_POST,
    llvm::AArch64::ST4Fourv4h_POST,
    llvm::AArch64::ST4Fourv4s_POST,
    llvm::AArch64::ST4Fourv8b_POST,
    llvm::AArch64::ST4Fourv8h_POST,
    llvm::AArch64::ST4i16,
    llvm::AArch64::ST4i32,
    llvm::AArch64::ST4i64,
    llvm::AArch64::ST4i8,
    llvm::AArch64::ST64BV,
    llvm::AArch64::ST64BV0,
    llvm::AArch64::STLXRB,
    llvm::AArch64::STLXRH,
    llvm::AArch64::STLXRW,
    llvm::AArch64::STLXRX,
    llvm::AArch64::STRBBpost,
    llvm::AArch64::STRBpost,
    llvm::AArch64::STRDpost,
    llvm::AArch64::STRHHpost,
    llvm::AArch64::STRHpost,
    llvm::AArch64::STRQpost,
    llvm::AArch64::STRSpost,
    llvm::AArch64::STRWpost,
    llvm::AArch64::STRXpost,
    llvm::AArch64::STXRB,
    llvm::AArch64::STXRH,
    llvm::AArch64::STXRW,
    llvm::AArch64::STXRX,
    llvm::AArch64::SWPAB,
    llvm::AArch64::SWPAH,
    llvm::AArch64::SWPALB,
    llvm::AArch64::SWPALH,
    llvm::AArch64::SWPALW,
    llvm::AArch64::SWPALX,
    llvm::AArch64::SWPAW,
    llvm::AArch64::SWPAX,
    llvm::AArch64::SWPB,
    llvm::AArch64::SWPH,
    llvm::AArch64::SWPLB,
    llvm::AArch64::SWPLH,
    llvm::AArch64::SWPLW,
    llvm::AArch64::SWPLX,
    llvm::AArch64::SWPW,
    llvm::AArch64::SWPX,
};

constexpr size_t ADDR_REGISTER_3_SIZE =
    sizeof(ADDR_REGISTER_3_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec ADDR_REGISTER_3_FN(const Patch &patch,
                                                 bool writeAccess, Reg dest) {
  return ADDR_REGISTER_FN(patch, writeAccess, dest, 2);
}

// address on the 4th operand
constexpr unsigned ADDR_REGISTER_4_TABLE[] = {
    // casb w0, w1, [x2]
    llvm::AArch64::CASAB,       llvm::AArch64::CASAH,
    llvm::AArch64::CASALB,      llvm::AArch64::CASALH,
    llvm::AArch64::CASALW,      llvm::AArch64::CASALX,
    llvm::AArch64::CASAW,       llvm::AArch64::CASAX,
    llvm::AArch64::CASB,        llvm::AArch64::CASH,
    llvm::AArch64::CASLB,       llvm::AArch64::CASLH,
    llvm::AArch64::CASLW,       llvm::AArch64::CASLX,
    llvm::AArch64::CASPALW,     llvm::AArch64::CASPALX,
    llvm::AArch64::CASPAW,      llvm::AArch64::CASPAX,
    llvm::AArch64::CASPLW,      llvm::AArch64::CASPLX,
    llvm::AArch64::CASPW,       llvm::AArch64::CASPX,
    llvm::AArch64::CASW,        llvm::AArch64::CASX,
    llvm::AArch64::LD1i16,      llvm::AArch64::LD1i32,
    llvm::AArch64::LD1i64,      llvm::AArch64::LD1i8,
    llvm::AArch64::LD2i16,      llvm::AArch64::LD2i32,
    llvm::AArch64::LD2i64,      llvm::AArch64::LD2i8,
    llvm::AArch64::LD3i16,      llvm::AArch64::LD3i32,
    llvm::AArch64::LD3i64,      llvm::AArch64::LD3i8,
    llvm::AArch64::LD4i16,      llvm::AArch64::LD4i32,
    llvm::AArch64::LD4i64,      llvm::AArch64::LD4i8,
    llvm::AArch64::LDPDpost,    llvm::AArch64::LDPQpost,
    llvm::AArch64::LDPSWpost,   llvm::AArch64::LDPSpost,
    llvm::AArch64::LDPWpost,    llvm::AArch64::LDPXpost,
    llvm::AArch64::ST1i16_POST, llvm::AArch64::ST1i32_POST,
    llvm::AArch64::ST1i64_POST, llvm::AArch64::ST1i8_POST,
    llvm::AArch64::ST2i16_POST, llvm::AArch64::ST2i32_POST,
    llvm::AArch64::ST2i64_POST, llvm::AArch64::ST2i8_POST,
    llvm::AArch64::ST3i16_POST, llvm::AArch64::ST3i32_POST,
    llvm::AArch64::ST3i64_POST, llvm::AArch64::ST3i8_POST,
    llvm::AArch64::ST4i16_POST, llvm::AArch64::ST4i32_POST,
    llvm::AArch64::ST4i64_POST, llvm::AArch64::ST4i8_POST,
    llvm::AArch64::STLXPW,      llvm::AArch64::STLXPX,
    llvm::AArch64::STPDpost,    llvm::AArch64::STPQpost,
    llvm::AArch64::STPSpost,    llvm::AArch64::STPWpost,
    llvm::AArch64::STPXpost,    llvm::AArch64::STXPW,
    llvm::AArch64::STXPX,
};

constexpr size_t ADDR_REGISTER_4_SIZE =
    sizeof(ADDR_REGISTER_4_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec ADDR_REGISTER_4_FN(const Patch &patch,
                                                 bool writeAccess, Reg dest) {
  return ADDR_REGISTER_FN(patch, writeAccess, dest, 3);
}

// address on the 5th operand
constexpr unsigned ADDR_REGISTER_5_TABLE[] = {
    // ld1	{ v0.d }[0], [x28], #8
    llvm::AArch64::LD1i16_POST, llvm::AArch64::LD1i32_POST,
    llvm::AArch64::LD1i64_POST, llvm::AArch64::LD1i8_POST,
    llvm::AArch64::LD2i16_POST, llvm::AArch64::LD2i32_POST,
    llvm::AArch64::LD2i64_POST, llvm::AArch64::LD2i8_POST,
    llvm::AArch64::LD3i16_POST, llvm::AArch64::LD3i32_POST,
    llvm::AArch64::LD3i64_POST, llvm::AArch64::LD3i8_POST,
    llvm::AArch64::LD4i16_POST, llvm::AArch64::LD4i32_POST,
    llvm::AArch64::LD4i64_POST, llvm::AArch64::LD4i8_POST,
};

constexpr size_t ADDR_REGISTER_5_SIZE =
    sizeof(ADDR_REGISTER_5_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec ADDR_REGISTER_5_FN(const Patch &patch,
                                                 bool writeAccess, Reg dest) {
  return ADDR_REGISTER_FN(patch, writeAccess, dest, 4);
}

/* Address in a register + immediate
 * =================================
 */

RelocatableInst::UniquePtrVec ADDR_REGISTER_IMM_FN(const Patch &patch,
                                                   bool writeAccess, Reg dest,
                                                   unsigned operandOff1,
                                                   unsigned operandOff2) {
  const llvm::MCInst &inst = patch.metadata.inst;

  QBDI_REQUIRE_ABORT_PATCH(operandOff1 < inst.getNumOperands(), patch,
                           "Invalid operand {}", operandOff1);
  QBDI_REQUIRE_ABORT_PATCH(operandOff2 < inst.getNumOperands(), patch,
                           "Invalid operand {}", operandOff2);

  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff1).isReg(), patch,
                           "Unexpected operand type");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff2).isImm(), patch,
                           "Unexpected operand type");
  RegLLVM addrReg = inst.getOperand(operandOff1).getReg();
  sword imm = inst.getOperand(operandOff2).getImm();
  return Addc(dest, addrReg, Constant(imm), dest);
}

// register addr = 2nd operand
// immediate = 3rd operand
constexpr unsigned ADDR_REGISTER_IMM_2_TABLE[] = {
    // ldtr	x1, [x17, #0x8]
    llvm::AArch64::LDTRBi,   llvm::AArch64::LDTRHi,   llvm::AArch64::LDTRSBWi,
    llvm::AArch64::LDTRSBXi, llvm::AArch64::LDTRSHWi, llvm::AArch64::LDTRSHXi,
    llvm::AArch64::LDTRSWi,  llvm::AArch64::LDTRWi,   llvm::AArch64::LDTRXi,
    llvm::AArch64::LDURBBi,  llvm::AArch64::LDURBi,   llvm::AArch64::LDURDi,
    llvm::AArch64::LDURHHi,  llvm::AArch64::LDURHi,   llvm::AArch64::LDURQi,
    llvm::AArch64::LDURSBWi, llvm::AArch64::LDURSBXi, llvm::AArch64::LDURSHWi,
    llvm::AArch64::LDURSHXi, llvm::AArch64::LDURSWi,  llvm::AArch64::LDURSi,
    llvm::AArch64::LDURWi,   llvm::AArch64::LDURXi,   llvm::AArch64::STTRBi,
    llvm::AArch64::STTRHi,   llvm::AArch64::STTRWi,   llvm::AArch64::STTRXi,
    llvm::AArch64::STURBBi,  llvm::AArch64::STURBi,   llvm::AArch64::STURDi,
    llvm::AArch64::STURHHi,  llvm::AArch64::STURHi,   llvm::AArch64::STURQi,
    llvm::AArch64::STURSi,   llvm::AArch64::STURWi,   llvm::AArch64::STURXi,
};

constexpr size_t ADDR_REGISTER_IMM_2_SIZE =
    sizeof(ADDR_REGISTER_IMM_2_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec
ADDR_REGISTER_IMM_2_FN(const Patch &patch, bool writeAccess, Reg dest) {
  return ADDR_REGISTER_IMM_FN(patch, writeAccess, dest, 1, 2);
}

// register addr = 3rd operand
// immediate = 4th operand
constexpr unsigned ADDR_REGISTER_IMM_3_TABLE[] = {
    // ldrb	w1, [x17, #0x8]!
    llvm::AArch64::LDRBBpre,  llvm::AArch64::LDRBpre,
    llvm::AArch64::LDRDpre,   llvm::AArch64::LDRHHpre,
    llvm::AArch64::LDRHpre,   llvm::AArch64::LDRQpre,
    llvm::AArch64::LDRSBWpre, llvm::AArch64::LDRSBXpre,
    llvm::AArch64::LDRSHWpre, llvm::AArch64::LDRSHXpre,
    llvm::AArch64::LDRSWpre,  llvm::AArch64::LDRSpre,
    llvm::AArch64::LDRWpre,   llvm::AArch64::LDRXpre,
    llvm::AArch64::STRBBpre,  llvm::AArch64::STRBpre,
    llvm::AArch64::STRDpre,   llvm::AArch64::STRHHpre,
    llvm::AArch64::STRHpre,   llvm::AArch64::STRQpre,
    llvm::AArch64::STRSpre,   llvm::AArch64::STRWpre,
    llvm::AArch64::STRXpre,
};

constexpr size_t ADDR_REGISTER_IMM_3_SIZE =
    sizeof(ADDR_REGISTER_IMM_3_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec
ADDR_REGISTER_IMM_3_FN(const Patch &patch, bool writeAccess, Reg dest) {
  return ADDR_REGISTER_IMM_FN(patch, writeAccess, dest, 2, 3);
}

/* Address in a register + immediate LSL log2(accessSize)
 * ======================================================
 */

RelocatableInst::UniquePtrVec
ADDR_REGISTER_IMM_LSL_FN(const Patch &patch, bool writeAccess, Reg dest,
                         unsigned operandOff1, unsigned operandOff2) {
  const LLVMCPU &llvmcpu = *patch.llvmcpu;
  const llvm::MCInst &inst = patch.metadata.inst;

  QBDI_REQUIRE_ABORT_PATCH(operandOff1 < inst.getNumOperands(), patch,
                           "Invalid operand {}", operandOff1);
  QBDI_REQUIRE_ABORT_PATCH(operandOff2 < inst.getNumOperands(), patch,
                           "Invalid operand {}", operandOff2);

  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff1).isReg(), patch,
                           "Unexpected operand type");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff2).isImm(), patch,
                           "Unexpected operand type");
  RegLLVM addrReg = inst.getOperand(operandOff1).getReg();
  sword imm = inst.getOperand(operandOff2).getImm();
  size_t accessSize;
  if (writeAccess) {
    accessSize = getWriteSize(inst, llvmcpu);
  } else {
    accessSize = getReadSize(inst, llvmcpu);
  }

  return Addc(dest, addrReg, Constant(imm * accessSize), dest);
}

// register addr = 2nd operand
// immediate = 3rd operand
// immediate shift = LSL log2(accessSize)
constexpr unsigned ADDR_REGISTER_IMM_LSL_2_TABLE[] = {
    // ldr	h0, [x26, #0x2]
    llvm::AArch64::LDRBBui,  llvm::AArch64::LDRBui,   llvm::AArch64::LDRDui,
    llvm::AArch64::LDRHHui,  llvm::AArch64::LDRHui,   llvm::AArch64::LDRQui,
    llvm::AArch64::LDRSBWui, llvm::AArch64::LDRSBXui, llvm::AArch64::LDRSHWui,
    llvm::AArch64::LDRSHXui, llvm::AArch64::LDRSWui,  llvm::AArch64::LDRSui,
    llvm::AArch64::LDRWui,   llvm::AArch64::LDRXui,   llvm::AArch64::STRBBui,
    llvm::AArch64::STRBui,   llvm::AArch64::STRDui,   llvm::AArch64::STRHHui,
    llvm::AArch64::STRHui,   llvm::AArch64::STRQui,   llvm::AArch64::STRSui,
    llvm::AArch64::STRWui,   llvm::AArch64::STRXui,
};

constexpr size_t ADDR_REGISTER_IMM_LSL_2_SIZE =
    sizeof(ADDR_REGISTER_IMM_LSL_2_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec
ADDR_REGISTER_IMM_LSL_2_FN(const Patch &patch, bool writeAccess, Reg dest) {
  return ADDR_REGISTER_IMM_LSL_FN(patch, writeAccess, dest, 1, 2);
}

/* Authanticated Address in a register + immediate LSL log2(accessSize)
 * ====================================================================
 */

RelocatableInst::UniquePtrVec
ADDR_PAC_REGISTER_IMM_LSL_FN(const Patch &patch, bool writeAccess, Reg dest,
                             unsigned operandOff1, unsigned operandOff2) {
  const LLVMCPU &llvmcpu = *patch.llvmcpu;
  const llvm::MCInst &inst = patch.metadata.inst;

  QBDI_REQUIRE_ABORT_PATCH(operandOff1 < inst.getNumOperands(), patch,
                           "Invalid operand {}", operandOff1);
  QBDI_REQUIRE_ABORT_PATCH(operandOff2 < inst.getNumOperands(), patch,
                           "Invalid operand {}", operandOff2);

  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff1).isReg(), patch,
                           "Unexpected operand type");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff2).isImm(), patch,
                           "Unexpected operand type");
  RegLLVM addrReg = inst.getOperand(operandOff1).getReg();
  sword imm = inst.getOperand(operandOff2).getImm();
  size_t accessSize;
  if (writeAccess) {
    accessSize = getWriteSize(inst, llvmcpu);
  } else {
    accessSize = getReadSize(inst, llvmcpu);
  }

  return conv_unique<RelocatableInst>(
      MovReg::unique(dest, addrReg), Xpacd(dest),
      // Special usage of ADD
      // ADD can only support value between -4095 and 4095
      //      or -4095 and 4095 with a shift of 12
      // LDRAA can have a immediate between -4096 and 4088
      // -> [-4095, 4088] is supported with no shift
      // -> -4096 is supported as -1<<12 (shift = 1)
      Add(dest, dest, Constant(imm * accessSize)));
}

// register addr = 2nd operand
// immediate = 3rd operand
// immediate shift = LSL log2(accessSize)
constexpr unsigned ADDR_PAC_REGISTER_IMM_LSL_2_TABLE[] = {
    // ldraa	x0, [x27]
    llvm::AArch64::LDRAAindexed,
    llvm::AArch64::LDRABindexed,
};

constexpr size_t ADDR_PAC_REGISTER_IMM_LSL_2_SIZE =
    sizeof(ADDR_PAC_REGISTER_IMM_LSL_2_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec
ADDR_PAC_REGISTER_IMM_LSL_2_FN(const Patch &patch, bool writeAccess, Reg dest) {
  return ADDR_PAC_REGISTER_IMM_LSL_FN(patch, writeAccess, dest, 1, 2);
}

// register addr = 3rd operand
// immediate = 4th operand
// immediate shift = LSL log2(accessSize)
constexpr unsigned ADDR_PAC_REGISTER_IMM_LSL_3_TABLE[] = {
    // ldraa	x0, [x27]!
    llvm::AArch64::LDRAAwriteback,
    llvm::AArch64::LDRABwriteback,
};

constexpr size_t ADDR_PAC_REGISTER_IMM_LSL_3_SIZE =
    sizeof(ADDR_PAC_REGISTER_IMM_LSL_3_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec
ADDR_PAC_REGISTER_IMM_LSL_3_FN(const Patch &patch, bool writeAccess, Reg dest) {
  return ADDR_PAC_REGISTER_IMM_LSL_FN(patch, writeAccess, dest, 2, 3);
}

/* Address in a register + immediate LSL log2(accessSize/2)
 * ========================================================
 */

RelocatableInst::UniquePtrVec
ADDR_REGISTER_IMM_LSL2_FN(const Patch &patch, bool writeAccess, Reg dest,
                          unsigned operandOff1, unsigned operandOff2) {
  const LLVMCPU &llvmcpu = *patch.llvmcpu;
  const llvm::MCInst &inst = patch.metadata.inst;

  QBDI_REQUIRE_ABORT_PATCH(operandOff1 < inst.getNumOperands(), patch,
                           "Invalid operand {}", operandOff1);
  QBDI_REQUIRE_ABORT_PATCH(operandOff2 < inst.getNumOperands(), patch,
                           "Invalid operand {}", operandOff2);

  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff1).isReg(), patch,
                           "Unexpected operand type");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff2).isImm(), patch,
                           "Unexpected operand type");
  RegLLVM addrReg = inst.getOperand(operandOff1).getReg();
  sword imm = inst.getOperand(operandOff2).getImm();

  size_t accessSize;
  if (writeAccess) {
    accessSize = getWriteSize(inst, llvmcpu);
  } else {
    accessSize = getReadSize(inst, llvmcpu);
  }

  return Addc(dest, addrReg, Constant(imm * (accessSize / 2)), dest);
}

// register addr = 3rd operand
// immediate = 4th operand
// immediate shift = LSL log2(accessSize/2)
constexpr unsigned ADDR_REGISTER_IMM_LSL2_3_TABLE[] = {
    // ldnp w0, w1, [x17, #4]
    llvm::AArch64::LDNPDi, llvm::AArch64::LDNPQi, llvm::AArch64::LDNPSi,
    llvm::AArch64::LDNPWi, llvm::AArch64::LDNPXi, llvm::AArch64::LDPDi,
    llvm::AArch64::LDPQi,  llvm::AArch64::LDPSWi, llvm::AArch64::LDPSi,
    llvm::AArch64::LDPWi,  llvm::AArch64::LDPXi,  llvm::AArch64::STNPDi,
    llvm::AArch64::STNPQi, llvm::AArch64::STNPSi, llvm::AArch64::STNPWi,
    llvm::AArch64::STNPXi, llvm::AArch64::STPDi,  llvm::AArch64::STPQi,
    llvm::AArch64::STPSi,  llvm::AArch64::STPWi,  llvm::AArch64::STPXi,
};

constexpr size_t ADDR_REGISTER_IMM_LSL2_3_SIZE =
    sizeof(ADDR_REGISTER_IMM_LSL2_3_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec
ADDR_REGISTER_IMM_LSL2_3_FN(const Patch &patch, bool writeAccess, Reg dest) {
  return ADDR_REGISTER_IMM_LSL2_FN(patch, writeAccess, dest, 2, 3);
}

// register addr = 4th operand
// immediate = 5th operand
// immediate shift = LSL log2(accessSize/2)
constexpr unsigned ADDR_REGISTER_IMM_LSL2_4_TABLE[] = {
    // ldp w0, w1, [x17, #4]!
    llvm::AArch64::LDPDpre, llvm::AArch64::LDPQpre, llvm::AArch64::LDPSWpre,
    llvm::AArch64::LDPSpre, llvm::AArch64::LDPWpre, llvm::AArch64::LDPXpre,
    llvm::AArch64::STPDpre, llvm::AArch64::STPQpre, llvm::AArch64::STPSpre,
    llvm::AArch64::STPWpre, llvm::AArch64::STPXpre,
};

constexpr size_t ADDR_REGISTER_IMM_LSL2_4_SIZE =
    sizeof(ADDR_REGISTER_IMM_LSL2_4_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec
ADDR_REGISTER_IMM_LSL2_4_FN(const Patch &patch, bool writeAccess, Reg dest) {
  return ADDR_REGISTER_IMM_LSL2_FN(patch, writeAccess, dest, 3, 4);
}

/* regbase address + ( extend register (UXTW|SXTW) SHIFT )
 * =======================================================
 */
RelocatableInst::UniquePtrVec
ADDR_REGISTER_EXT_FN(const Patch &patch, bool writeAccess, Reg dest,
                     unsigned operandOff1, unsigned operandOff2,
                     unsigned operandOff3, unsigned operandOff4) {
  const LLVMCPU &llvmcpu = *patch.llvmcpu;
  const llvm::MCInst &inst = patch.metadata.inst;

  QBDI_REQUIRE_ABORT_PATCH(operandOff1 < inst.getNumOperands(), patch,
                           "Invalid operand {}", operandOff1);
  QBDI_REQUIRE_ABORT_PATCH(operandOff2 < inst.getNumOperands(), patch,
                           "Invalid operand {}", operandOff2);
  QBDI_REQUIRE_ABORT_PATCH(operandOff3 < inst.getNumOperands(), patch,
                           "Invalid operand {}", operandOff3);
  QBDI_REQUIRE_ABORT_PATCH(operandOff4 < inst.getNumOperands(), patch,
                           "Invalid operand {}", operandOff4);

  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff1).isReg(), patch,
                           "Unexpected operand type");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff2).isReg(), patch,
                           "Unexpected operand type");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff3).isImm(), patch,
                           "Unexpected operand type");
  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff4).isImm(), patch,
                           "Unexpected operand type");
  RegLLVM addrReg = inst.getOperand(operandOff1).getReg();
  RegLLVM extReg = inst.getOperand(operandOff2).getReg();
  bool isSigned = inst.getOperand(operandOff3).getImm();
  bool hasShift = inst.getOperand(operandOff4).getImm();

  size_t shiftValue = 0;
  if (hasShift) {
    size_t accessSize;
    if (writeAccess) {
      accessSize = getWriteSize(inst, llvmcpu);
    } else {
      accessSize = getReadSize(inst, llvmcpu);
    }
    switch (accessSize) {
      case 1:
        break;
      case 2:
        shiftValue = 1;
        break;
      case 4:
        shiftValue = 2;
        break;
      case 8:
        shiftValue = 3;
        break;
      case 16:
        shiftValue = 4;
        break;
      default:
        QBDI_ABORT_PATCH(patch, "Unexpected access size {}", accessSize);
    }
  }
  unsigned extRegSize = getRegisterSize(extReg);
  QBDI_REQUIRE_ABORT_PATCH(extRegSize == 4 || extRegSize == 8, patch,
                           "Unexpected register size {}", extRegSize);

  if (extRegSize == 4) {
    if (isSigned) {
      return conv_unique<RelocatableInst>(
          Addr(dest, addrReg, extReg, SXTW, Constant(shiftValue)));
    } else {
      return conv_unique<RelocatableInst>(
          Addr(dest, addrReg, extReg, UXTW, Constant(shiftValue)));
    }
  } else {
    if (isSigned) {
      return conv_unique<RelocatableInst>(
          Addr(dest, addrReg, extReg, SXTX, Constant(shiftValue)));
    } else {
      return conv_unique<RelocatableInst>(
          Addr(dest, addrReg, extReg, UXTX, Constant(shiftValue)));
    }
  }
}

// base addr register = 2nd operand
// extend register = 3rd operand
// extend is signed = 4th operand
// extend has shift = 5th operand
constexpr unsigned ADDR_REGISTER_EXT_2_TABLE[] = {
    llvm::AArch64::LDRBBroW,  llvm::AArch64::LDRBBroX,
    llvm::AArch64::LDRBroW,   llvm::AArch64::LDRBroX,
    llvm::AArch64::LDRDroW,   llvm::AArch64::LDRDroX,
    llvm::AArch64::LDRHHroW,  llvm::AArch64::LDRHHroX,
    llvm::AArch64::LDRHroW,   llvm::AArch64::LDRHroX,
    llvm::AArch64::LDRQroW,   llvm::AArch64::LDRQroX,
    llvm::AArch64::LDRSBWroW, llvm::AArch64::LDRSBWroX,
    llvm::AArch64::LDRSBXroW, llvm::AArch64::LDRSBXroX,
    llvm::AArch64::LDRSHWroW, llvm::AArch64::LDRSHWroX,
    llvm::AArch64::LDRSHXroW, llvm::AArch64::LDRSHXroX,
    llvm::AArch64::LDRSWroW,  llvm::AArch64::LDRSWroX,
    llvm::AArch64::LDRSroW,   llvm::AArch64::LDRSroX,
    llvm::AArch64::LDRWroW,   llvm::AArch64::LDRWroX,
    llvm::AArch64::LDRXroW,   llvm::AArch64::LDRXroX,
    llvm::AArch64::STRBBroW,  llvm::AArch64::STRBBroX,
    llvm::AArch64::STRBroW,   llvm::AArch64::STRBroX,
    llvm::AArch64::STRDroW,   llvm::AArch64::STRDroX,
    llvm::AArch64::STRHHroW,  llvm::AArch64::STRHHroX,
    llvm::AArch64::STRHroW,   llvm::AArch64::STRHroX,
    llvm::AArch64::STRQroW,   llvm::AArch64::STRQroX,
    llvm::AArch64::STRSroW,   llvm::AArch64::STRSroX,
    llvm::AArch64::STRWroW,   llvm::AArch64::STRWroX,
    llvm::AArch64::STRXroW,   llvm::AArch64::STRXroX,
};

constexpr size_t ADDR_REGISTER_EXT_2_SIZE =
    sizeof(ADDR_REGISTER_EXT_2_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec
ADDR_REGISTER_EXT_2_FN(const Patch &patch, bool writeAccess, Reg dest) {
  return ADDR_REGISTER_EXT_FN(patch, writeAccess, dest, 1, 2, 3, 4);
}

/* PC rel + offset * 4
 * ===================
 */

RelocatableInst::UniquePtrVec ADDR_REGISTER_PC_FN(const Patch &patch,
                                                  bool writeAccess, Reg dest,
                                                  unsigned operandOff) {
  const llvm::MCInst &inst = patch.metadata.inst;
  const rword address = patch.metadata.address;

  QBDI_REQUIRE_ABORT_PATCH(operandOff < inst.getNumOperands(), patch,
                           "Invalid operand {}", operandOff);

  QBDI_REQUIRE_ABORT_PATCH(inst.getOperand(operandOff).isImm(), patch,
                           "Unexpected operand type");
  sword offset = inst.getOperand(operandOff).getImm();

  return conv_unique<RelocatableInst>(
      LoadImm::unique(dest, Constant(address + 4 * offset)));
}

// offset value = 2nd operand
constexpr unsigned ADDR_REGISTER_PC_2_TABLE[] = {
    llvm::AArch64::LDRDl, llvm::AArch64::LDRQl, llvm::AArch64::LDRSWl,
    llvm::AArch64::LDRSl, llvm::AArch64::LDRWl, llvm::AArch64::LDRXl,
};

constexpr size_t ADDR_REGISTER_PC_2_SIZE =
    sizeof(ADDR_REGISTER_PC_2_TABLE) / sizeof(unsigned);

RelocatableInst::UniquePtrVec
ADDR_REGISTER_PC_2_FN(const Patch &patch, bool writeAccess, Reg dest) {
  return ADDR_REGISTER_PC_FN(patch, writeAccess, dest, 1);
}

struct MemoryAccessInfoArray {
  AddressGenFn *addrFn[13] = {};
  uint8_t addrArr[llvm::AArch64::INSTRUCTION_LIST_END] = {0};

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
    addData(index++, ADDR_REGISTER_2_TABLE, ADDR_REGISTER_2_SIZE,
            ADDR_REGISTER_2_FN);
    addData(index++, ADDR_REGISTER_3_TABLE, ADDR_REGISTER_3_SIZE,
            ADDR_REGISTER_3_FN);
    addData(index++, ADDR_REGISTER_4_TABLE, ADDR_REGISTER_4_SIZE,
            ADDR_REGISTER_4_FN);
    addData(index++, ADDR_REGISTER_5_TABLE, ADDR_REGISTER_5_SIZE,
            ADDR_REGISTER_5_FN);
    addData(index++, ADDR_REGISTER_IMM_2_TABLE, ADDR_REGISTER_IMM_2_SIZE,
            ADDR_REGISTER_IMM_2_FN);
    addData(index++, ADDR_REGISTER_IMM_3_TABLE, ADDR_REGISTER_IMM_3_SIZE,
            ADDR_REGISTER_IMM_3_FN);
    addData(index++, ADDR_REGISTER_IMM_LSL_2_TABLE,
            ADDR_REGISTER_IMM_LSL_2_SIZE, ADDR_REGISTER_IMM_LSL_2_FN);
    addData(index++, ADDR_PAC_REGISTER_IMM_LSL_2_TABLE,
            ADDR_PAC_REGISTER_IMM_LSL_2_SIZE, ADDR_PAC_REGISTER_IMM_LSL_2_FN);
    addData(index++, ADDR_PAC_REGISTER_IMM_LSL_3_TABLE,
            ADDR_PAC_REGISTER_IMM_LSL_3_SIZE, ADDR_PAC_REGISTER_IMM_LSL_3_FN);
    addData(index++, ADDR_REGISTER_IMM_LSL2_3_TABLE,
            ADDR_REGISTER_IMM_LSL2_3_SIZE, ADDR_REGISTER_IMM_LSL2_3_FN);
    addData(index++, ADDR_REGISTER_IMM_LSL2_4_TABLE,
            ADDR_REGISTER_IMM_LSL2_4_SIZE, ADDR_REGISTER_IMM_LSL2_4_FN);
    addData(index++, ADDR_REGISTER_EXT_2_TABLE, ADDR_REGISTER_EXT_2_SIZE,
            ADDR_REGISTER_EXT_2_FN);
    addData(index++, ADDR_REGISTER_PC_2_TABLE, ADDR_REGISTER_PC_2_SIZE,
            ADDR_REGISTER_PC_2_FN);
  }
};

constexpr MemoryAccessInfoArray memoryAccessInfo;

#if CHECK_MEMORYACCESS_TABLE

struct AddressGenerator {
  const unsigned (&insts)[];
  size_t nbInsts;
  AddressGenFn *fn;
};

int checkTable() {
  const std::vector<AddressGenerator> addrInfo = {
      {ADDR_REGISTER_2_TABLE, ADDR_REGISTER_2_SIZE, ADDR_REGISTER_2_FN},
      {ADDR_REGISTER_3_TABLE, ADDR_REGISTER_3_SIZE, ADDR_REGISTER_3_FN},
      {ADDR_REGISTER_4_TABLE, ADDR_REGISTER_4_SIZE, ADDR_REGISTER_4_FN},
      {ADDR_REGISTER_5_TABLE, ADDR_REGISTER_5_SIZE, ADDR_REGISTER_5_FN},
      {ADDR_REGISTER_IMM_2_TABLE, ADDR_REGISTER_IMM_2_SIZE,
       ADDR_REGISTER_IMM_2_FN},
      {ADDR_REGISTER_IMM_3_TABLE, ADDR_REGISTER_IMM_3_SIZE,
       ADDR_REGISTER_IMM_3_FN},
      {ADDR_REGISTER_IMM_LSL_2_TABLE, ADDR_REGISTER_IMM_LSL_2_SIZE,
       ADDR_REGISTER_IMM_LSL_2_FN},
      {ADDR_PAC_REGISTER_IMM_LSL_2_TABLE, ADDR_PAC_REGISTER_IMM_LSL_2_SIZE,
       ADDR_PAC_REGISTER_IMM_LSL_2_FN},
      {ADDR_PAC_REGISTER_IMM_LSL_3_TABLE, ADDR_PAC_REGISTER_IMM_LSL_3_SIZE,
       ADDR_PAC_REGISTER_IMM_LSL_3_FN},
      {ADDR_REGISTER_IMM_LSL2_3_TABLE, ADDR_REGISTER_IMM_LSL2_3_SIZE,
       ADDR_REGISTER_IMM_LSL2_3_FN},
      {ADDR_REGISTER_IMM_LSL2_4_TABLE, ADDR_REGISTER_IMM_LSL2_4_SIZE,
       ADDR_REGISTER_IMM_LSL2_4_FN},
      {ADDR_REGISTER_EXT_2_TABLE, ADDR_REGISTER_EXT_2_SIZE,
       ADDR_REGISTER_EXT_2_FN},
      {ADDR_REGISTER_PC_2_TABLE, ADDR_REGISTER_PC_2_SIZE,
       ADDR_REGISTER_PC_2_FN}};

  const LLVMCPUs llvmcpus{"", {}, Options::NO_OPT};
  const LLVMCPU &llvmcpu = llvmcpus.getCPU(CPUMode::DEFAULT);

  for (unsigned op = 0; op < llvm::AArch64::INSTRUCTION_LIST_END; op++) {
    llvm::MCInst inst;
    inst.setOpcode(op);
    const char *opcode = llvmcpu.getInstOpcodeName(inst);

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

  return memoryAccessInfo.addrFn[index](patch, writeAccess, dest);
}

// Generate dynamic PatchGenerator for instruction
// ===============================================

namespace {

enum MemoryTag : uint16_t {
  MEM_READ_ADDRESS_TAG = MEMORY_TAG_BEGIN + 0,
  MEM_WRITE_ADDRESS_TAG = MEMORY_TAG_BEGIN + 1,

  MEM_READ_VALUE_TAG = MEMORY_TAG_BEGIN + 2,
  MEM_WRITE_VALUE_TAG = MEMORY_TAG_BEGIN + 3,
  MEM_VALUE_EXTENDED_TAG = MEMORY_TAG_BEGIN + 4,

  MEM_MOPS_SIZE_TAG = MEMORY_TAG_BEGIN + 5,
};

const PatchGenerator::UniquePtrVec &
generateReadInstrumentPatch(Patch &patch, const LLVMCPU &llvmcpu) {
  switch (getReadSize(patch.metadata.inst, llvmcpu)) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 6:
    case 8: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
          GetReadValue::unique(Temp(0), Temp(0), 0),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_VALUE_TAG)));
      return r;
    }
    case 12: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
          GetReadValue::unique(Temp(1), Temp(0), 0),
          WriteTemp::unique(Temp(1), Shadow(MEM_READ_VALUE_TAG)),
          GetReadValue::unique(Temp(1), Temp(0), 1),
          WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)));
      return r;
    }
    case 16: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
          GetReadValueX2::unique(Temp(1), Temp(2), Temp(0), 0),
          WriteTemp::unique(Temp(1), Shadow(MEM_READ_VALUE_TAG)),
          WriteTemp::unique(Temp(2), Shadow(MEM_VALUE_EXTENDED_TAG)));
      return r;
    }
    case 24: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
          GetReadValueX2::unique(Temp(1), Temp(2), Temp(0), 0),
          WriteTemp::unique(Temp(1), Shadow(MEM_READ_VALUE_TAG)),
          WriteTemp::unique(Temp(2), Shadow(MEM_VALUE_EXTENDED_TAG)),
          GetReadValue::unique(Temp(1), Temp(0), 2),
          WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)));
      return r;
    }
    case 32: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
          GetReadValueX2::unique(Temp(1), Temp(2), Temp(0), 0),
          WriteTemp::unique(Temp(1), Shadow(MEM_READ_VALUE_TAG)),
          WriteTemp::unique(Temp(2), Shadow(MEM_VALUE_EXTENDED_TAG)),
          GetReadValueX2::unique(Temp(1), Temp(2), Temp(0), 2),
          WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)),
          WriteTemp::unique(Temp(2), Shadow(MEM_VALUE_EXTENDED_TAG)));
      return r;
    }
    case 48: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
          GetReadValueX2::unique(Temp(1), Temp(2), Temp(0), 0),
          WriteTemp::unique(Temp(1), Shadow(MEM_READ_VALUE_TAG)),
          WriteTemp::unique(Temp(2), Shadow(MEM_VALUE_EXTENDED_TAG)),
          GetReadValueX2::unique(Temp(1), Temp(2), Temp(0), 2),
          WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)),
          WriteTemp::unique(Temp(2), Shadow(MEM_VALUE_EXTENDED_TAG)),
          GetReadValueX2::unique(Temp(1), Temp(2), Temp(0), 4),
          WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)),
          WriteTemp::unique(Temp(2), Shadow(MEM_VALUE_EXTENDED_TAG)));
      return r;
    }
    case 64: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetReadAddress::unique(Temp(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
          GetReadValueX2::unique(Temp(1), Temp(2), Temp(0), 0),
          WriteTemp::unique(Temp(1), Shadow(MEM_READ_VALUE_TAG)),
          WriteTemp::unique(Temp(2), Shadow(MEM_VALUE_EXTENDED_TAG)),
          GetReadValueX2::unique(Temp(1), Temp(2), Temp(0), 2),
          WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)),
          WriteTemp::unique(Temp(2), Shadow(MEM_VALUE_EXTENDED_TAG)),
          GetReadValueX2::unique(Temp(1), Temp(2), Temp(0), 4),
          WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)),
          WriteTemp::unique(Temp(2), Shadow(MEM_VALUE_EXTENDED_TAG)),
          GetReadValueX2::unique(Temp(1), Temp(2), Temp(0), 6),
          WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)),
          WriteTemp::unique(Temp(2), Shadow(MEM_VALUE_EXTENDED_TAG)));
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
    case 3:
    case 4:
    case 6:
    case 8: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          ReadTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
          GetWrittenValue::unique(Temp(0), Temp(0), 0),
          WriteTemp::unique(Temp(0), Shadow(MEM_WRITE_VALUE_TAG)));
      return r;
    }
    case 12: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          ReadTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
          GetWrittenValue::unique(Temp(1), Temp(0), 0),
          WriteTemp::unique(Temp(1), Shadow(MEM_WRITE_VALUE_TAG)),
          GetWrittenValue::unique(Temp(1), Temp(0), 1),
          WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)));
      return r;
    }
    case 16: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          ReadTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
          GetWrittenValueX2::unique(Temp(1), Temp(2), Temp(0), 0),
          WriteTemp::unique(Temp(1), Shadow(MEM_WRITE_VALUE_TAG)),
          WriteTemp::unique(Temp(2), Shadow(MEM_VALUE_EXTENDED_TAG)));
      return r;
    }
    case 24: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          ReadTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
          GetWrittenValueX2::unique(Temp(1), Temp(2), Temp(0), 0),
          WriteTemp::unique(Temp(1), Shadow(MEM_WRITE_VALUE_TAG)),
          WriteTemp::unique(Temp(2), Shadow(MEM_VALUE_EXTENDED_TAG)),
          GetWrittenValue::unique(Temp(1), Temp(0), 2),
          WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)));
      return r;
    }
    case 32: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          ReadTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
          GetWrittenValueX2::unique(Temp(1), Temp(2), Temp(0), 0),
          WriteTemp::unique(Temp(1), Shadow(MEM_WRITE_VALUE_TAG)),
          WriteTemp::unique(Temp(2), Shadow(MEM_VALUE_EXTENDED_TAG)),
          GetWrittenValueX2::unique(Temp(1), Temp(2), Temp(0), 2),
          WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)),
          WriteTemp::unique(Temp(2), Shadow(MEM_VALUE_EXTENDED_TAG)));
      return r;
    }
    case 48: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          ReadTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
          GetWrittenValueX2::unique(Temp(1), Temp(2), Temp(0), 0),
          WriteTemp::unique(Temp(1), Shadow(MEM_WRITE_VALUE_TAG)),
          WriteTemp::unique(Temp(2), Shadow(MEM_VALUE_EXTENDED_TAG)),
          GetWrittenValueX2::unique(Temp(1), Temp(2), Temp(0), 2),
          WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)),
          WriteTemp::unique(Temp(2), Shadow(MEM_VALUE_EXTENDED_TAG)),
          GetWrittenValueX2::unique(Temp(1), Temp(2), Temp(0), 4),
          WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)),
          WriteTemp::unique(Temp(2), Shadow(MEM_VALUE_EXTENDED_TAG)));
      return r;
    }
    case 64: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          ReadTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
          GetWrittenValueX2::unique(Temp(1), Temp(2), Temp(0), 0),
          WriteTemp::unique(Temp(1), Shadow(MEM_WRITE_VALUE_TAG)),
          WriteTemp::unique(Temp(2), Shadow(MEM_VALUE_EXTENDED_TAG)),
          GetWrittenValueX2::unique(Temp(1), Temp(2), Temp(0), 2),
          WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)),
          WriteTemp::unique(Temp(2), Shadow(MEM_VALUE_EXTENDED_TAG)),
          GetWrittenValueX2::unique(Temp(1), Temp(2), Temp(0), 4),
          WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)),
          WriteTemp::unique(Temp(2), Shadow(MEM_VALUE_EXTENDED_TAG)),
          GetWrittenValueX2::unique(Temp(1), Temp(2), Temp(0), 6),
          WriteTemp::unique(Temp(1), Shadow(MEM_VALUE_EXTENDED_TAG)),
          WriteTemp::unique(Temp(2), Shadow(MEM_VALUE_EXTENDED_TAG)));
      return r;
    }
    default:
      QBDI_ABORT_PATCH(patch, "Unexpected number of memory Access {}",
                       getWriteSize(patch.metadata.inst, llvmcpu));
  }
}

// MOPS prologue instruction
const PatchGenerator::UniquePtrVec &
generateMOPSReadInstrumentPatch(Patch &patch, const LLVMCPU &llvmcpu) {

  static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
      GetOperand::unique(Temp(0), Operand(1)),
      WriteTemp::unique(Temp(0), Shadow(MEM_READ_ADDRESS_TAG)),
      GetOperand::unique(Temp(0), Operand(2)),
      WriteTemp::unique(Temp(0), Shadow(MEM_MOPS_SIZE_TAG)));
  return r;
}

const PatchGenerator::UniquePtrVec &
generateMOPSWriteInstrumentPatch(Patch &patch, const LLVMCPU &llvmcpu) {

  switch (patch.metadata.inst.getOpcode()) {
    case llvm::AArch64::CPYFP:
    case llvm::AArch64::CPYFPN:
    case llvm::AArch64::CPYFPRN:
    case llvm::AArch64::CPYFPRT:
    case llvm::AArch64::CPYFPRTN:
    case llvm::AArch64::CPYFPRTRN:
    case llvm::AArch64::CPYFPRTWN:
    case llvm::AArch64::CPYFPT:
    case llvm::AArch64::CPYFPTN:
    case llvm::AArch64::CPYFPTRN:
    case llvm::AArch64::CPYFPTWN:
    case llvm::AArch64::CPYFPWN:
    case llvm::AArch64::CPYFPWT:
    case llvm::AArch64::CPYFPWTN:
    case llvm::AArch64::CPYFPWTRN:
    case llvm::AArch64::CPYFPWTWN:
    case llvm::AArch64::CPYP:
    case llvm::AArch64::CPYPN:
    case llvm::AArch64::CPYPRN:
    case llvm::AArch64::CPYPRT:
    case llvm::AArch64::CPYPRTN:
    case llvm::AArch64::CPYPRTRN:
    case llvm::AArch64::CPYPRTWN:
    case llvm::AArch64::CPYPT:
    case llvm::AArch64::CPYPTN:
    case llvm::AArch64::CPYPTRN:
    case llvm::AArch64::CPYPTWN:
    case llvm::AArch64::CPYPWN:
    case llvm::AArch64::CPYPWT:
    case llvm::AArch64::CPYPWTN:
    case llvm::AArch64::CPYPWTRN:
    case llvm::AArch64::CPYPWTWN: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetOperand::unique(Temp(0), Operand(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
          GetOperand::unique(Temp(0), Operand(2)),
          WriteTemp::unique(Temp(0), Shadow(MEM_MOPS_SIZE_TAG)));
      return r;
    }
    case llvm::AArch64::SETGP:
    case llvm::AArch64::SETGPN:
    case llvm::AArch64::SETGPT:
    case llvm::AArch64::SETGPTN:
    case llvm::AArch64::SETP:
    case llvm::AArch64::SETPN:
    case llvm::AArch64::SETPT:
    case llvm::AArch64::SETPTN: {
      static const PatchGenerator::UniquePtrVec r = conv_unique<PatchGenerator>(
          GetOperand::unique(Temp(0), Operand(0)),
          WriteTemp::unique(Temp(0), Shadow(MEM_WRITE_ADDRESS_TAG)),
          GetOperand::unique(Temp(0), Operand(1)),
          WriteTemp::unique(Temp(0), Shadow(MEM_MOPS_SIZE_TAG)));
      return r;
    }
    default:
      QBDI_ABORT_PATCH(patch, "Unexpected instruction");
  }
}

} // anonymous namespace

std::vector<std::unique_ptr<InstrRule>> getInstrRuleMemAccessRead() {
  return conv_unique<InstrRule>(
      InstrRuleDynamic::unique(
          DoesReadAccess::unique(), generateReadInstrumentPatch, PREINST, false,
          PRIORITY_MEMACCESS_LIMIT + 1, RelocTagPreInstMemAccess),
      InstrRuleDynamic::unique(IsMOPSReadPrologue::unique(),
                               generateMOPSReadInstrumentPatch, PREINST, false,
                               PRIORITY_MEMACCESS_LIMIT + 1,
                               RelocTagPreInstMemAccess));
}

std::vector<std::unique_ptr<InstrRule>> getInstrRuleMemAccessWrite() {
  return conv_unique<InstrRule>(
      InstrRuleDynamic::unique(
          DoesWriteAccess::unique(), generatePreWriteInstrumentPatch, PREINST,
          false, PRIORITY_MEMACCESS_LIMIT, RelocTagPreInstMemAccess),
      InstrRuleDynamic::unique(
          DoesWriteAccess::unique(), generatePostWriteInstrumentPatch, POSTINST,
          false, PRIORITY_MEMACCESS_LIMIT, RelocTagPostInstMemAccess),
      InstrRuleDynamic::unique(
          IsMOPSWritePrologue::unique(), generateMOPSWriteInstrumentPatch,
          PREINST, false, PRIORITY_MEMACCESS_LIMIT, RelocTagPreInstMemAccess));
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
    // special case for MOPS instruction
    if (shadows[index].tag == MEM_MOPS_SIZE_TAG) {
      rword size = curExecBlock.getShadow(shadows[index].shadowID);
      access.value = size;
      if (size > 0xffff) {
        access.size = 0xffff;
        access.flags = MEMORY_UNKNOWN_VALUE | MEMORY_MINIMUM_SIZE;
      } else {
        access.size = size;
        access.flags = MEMORY_UNKNOWN_VALUE;
      }
      dest.push_back(access);
      return;
    }
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
