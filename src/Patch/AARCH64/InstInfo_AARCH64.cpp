/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2025 Quarkslab
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

#include "AArch64InstrInfo.h"
#include "llvm/MC/MCInst.h"

#include "devVariable.h"
#include "Patch/AARCH64/InstInfo_AARCH64.h"
#include "Patch/InstInfo.h"
#include "Utility/LogSys.h"

#include "QBDI/Config.h"
#include "QBDI/State.h"

// TODO:
// st2	{ v0.4s, v1.4s }, [x16] -> ST2Twov4s
// ld2	{ v2.4s, v3.4s }, [x17] -> LD2Twov4s
// etc

namespace QBDI {
namespace {

// Read Instructions
// =================

constexpr unsigned READ_8_P1[] = {
    // clang-format off
    llvm::AArch64::CASAB,
    llvm::AArch64::CASALB,
    llvm::AArch64::CASB,
    llvm::AArch64::CASLB,
    llvm::AArch64::LD1Rv16b,
    llvm::AArch64::LD1Rv16b_POST,
    llvm::AArch64::LD1Rv8b,
    llvm::AArch64::LD1Rv8b_POST,
    llvm::AArch64::LD1i8,
    llvm::AArch64::LD1i8_POST,
    llvm::AArch64::LDADDAB,
    llvm::AArch64::LDADDALB,
    llvm::AArch64::LDADDB,
    llvm::AArch64::LDADDLB,
    llvm::AArch64::LDAPRB,
    llvm::AArch64::LDAPURBi,
    llvm::AArch64::LDAPURSBWi,
    llvm::AArch64::LDAPURSBXi,
    llvm::AArch64::LDAPURbi,
    llvm::AArch64::LDARB,
    llvm::AArch64::LDAXRB,
    llvm::AArch64::LDCLRAB,
    llvm::AArch64::LDCLRALB,
    llvm::AArch64::LDCLRB,
    llvm::AArch64::LDCLRLB,
    llvm::AArch64::LDEORAB,
    llvm::AArch64::LDEORALB,
    llvm::AArch64::LDEORB,
    llvm::AArch64::LDEORLB,
    llvm::AArch64::LDLARB,
    llvm::AArch64::LDRBBpost,
    llvm::AArch64::LDRBBpre,
    llvm::AArch64::LDRBBroW,
    llvm::AArch64::LDRBBroX,
    llvm::AArch64::LDRBBui,
    llvm::AArch64::LDRBpost,
    llvm::AArch64::LDRBpre,
    llvm::AArch64::LDRBroW,
    llvm::AArch64::LDRBroX,
    llvm::AArch64::LDRBui,
    llvm::AArch64::LDRSBWpost,
    llvm::AArch64::LDRSBWpre,
    llvm::AArch64::LDRSBWroW,
    llvm::AArch64::LDRSBWroX,
    llvm::AArch64::LDRSBWui,
    llvm::AArch64::LDRSBXpost,
    llvm::AArch64::LDRSBXpre,
    llvm::AArch64::LDRSBXroW,
    llvm::AArch64::LDRSBXroX,
    llvm::AArch64::LDRSBXui,
    llvm::AArch64::LDSETAB,
    llvm::AArch64::LDSETALB,
    llvm::AArch64::LDSETB,
    llvm::AArch64::LDSETLB,
    llvm::AArch64::LDSMAXAB,
    llvm::AArch64::LDSMAXALB,
    llvm::AArch64::LDSMAXB,
    llvm::AArch64::LDSMAXLB,
    llvm::AArch64::LDSMINAB,
    llvm::AArch64::LDSMINALB,
    llvm::AArch64::LDSMINB,
    llvm::AArch64::LDSMINLB,
    llvm::AArch64::LDTRBi,
    llvm::AArch64::LDTRSBWi,
    llvm::AArch64::LDTRSBXi,
    llvm::AArch64::LDUMAXAB,
    llvm::AArch64::LDUMAXALB,
    llvm::AArch64::LDUMAXB,
    llvm::AArch64::LDUMAXLB,
    llvm::AArch64::LDUMINAB,
    llvm::AArch64::LDUMINALB,
    llvm::AArch64::LDUMINB,
    llvm::AArch64::LDUMINLB,
    llvm::AArch64::LDURBBi,
    llvm::AArch64::LDURBi,
    llvm::AArch64::LDURSBWi,
    llvm::AArch64::LDURSBXi,
    llvm::AArch64::LDXRB,
    llvm::AArch64::SWPAB,
    llvm::AArch64::SWPALB,
    llvm::AArch64::SWPB,
    llvm::AArch64::SWPLB,
    // clang-format on
};

constexpr size_t READ_8_P1_SIZE = sizeof(READ_8_P1) / sizeof(unsigned);

constexpr unsigned READ_8_P2[] = {
    // clang-format off
    llvm::AArch64::LD2Rv16b,
    llvm::AArch64::LD2Rv16b_POST,
    llvm::AArch64::LD2Rv8b,
    llvm::AArch64::LD2Rv8b_POST,
    llvm::AArch64::LD2i8,
    llvm::AArch64::LD2i8_POST,
    // clang-format on
};

constexpr size_t READ_8_P2_SIZE = sizeof(READ_8_P2) / sizeof(unsigned);

constexpr unsigned READ_8_P3[] = {
    // clang-format off
    llvm::AArch64::LD3Rv16b,
    llvm::AArch64::LD3Rv16b_POST,
    llvm::AArch64::LD3Rv8b,
    llvm::AArch64::LD3Rv8b_POST,
    llvm::AArch64::LD3i8,
    llvm::AArch64::LD3i8_POST,
    // clang-format on
};

constexpr size_t READ_8_P3_SIZE = sizeof(READ_8_P3) / sizeof(unsigned);

constexpr unsigned READ_8_P4[] = {
    // clang-format off
    llvm::AArch64::LD4Rv16b,
    llvm::AArch64::LD4Rv16b_POST,
    llvm::AArch64::LD4Rv8b,
    llvm::AArch64::LD4Rv8b_POST,
    llvm::AArch64::LD4i8,
    llvm::AArch64::LD4i8_POST,
    // clang-format on
};

constexpr size_t READ_8_P4_SIZE = sizeof(READ_8_P4) / sizeof(unsigned);

constexpr unsigned READ_16_P1[] = {
    // clang-format off
    llvm::AArch64::CASAH,
    llvm::AArch64::CASALH,
    llvm::AArch64::CASH,
    llvm::AArch64::CASLH,
    llvm::AArch64::LD1Rv4h,
    llvm::AArch64::LD1Rv4h_POST,
    llvm::AArch64::LD1Rv8h,
    llvm::AArch64::LD1Rv8h_POST,
    llvm::AArch64::LD1i16,
    llvm::AArch64::LD1i16_POST,
    llvm::AArch64::LDADDAH,
    llvm::AArch64::LDADDALH,
    llvm::AArch64::LDADDH,
    llvm::AArch64::LDADDLH,
    llvm::AArch64::LDAPRH,
    llvm::AArch64::LDAPURHi,
    llvm::AArch64::LDAPURSHWi,
    llvm::AArch64::LDAPURSHXi,
    llvm::AArch64::LDAPURhi,
    llvm::AArch64::LDARH,
    llvm::AArch64::LDAXRH,
    llvm::AArch64::LDCLRAH,
    llvm::AArch64::LDCLRALH,
    llvm::AArch64::LDCLRH,
    llvm::AArch64::LDCLRLH,
    llvm::AArch64::LDEORAH,
    llvm::AArch64::LDEORALH,
    llvm::AArch64::LDEORH,
    llvm::AArch64::LDEORLH,
    llvm::AArch64::LDLARH,
    llvm::AArch64::LDRHHpost,
    llvm::AArch64::LDRHHpre,
    llvm::AArch64::LDRHHroW,
    llvm::AArch64::LDRHHroX,
    llvm::AArch64::LDRHHui,
    llvm::AArch64::LDRHpost,
    llvm::AArch64::LDRHpre,
    llvm::AArch64::LDRHroW,
    llvm::AArch64::LDRHroX,
    llvm::AArch64::LDRHui,
    llvm::AArch64::LDRSHWpost,
    llvm::AArch64::LDRSHWpre,
    llvm::AArch64::LDRSHWroW,
    llvm::AArch64::LDRSHWroX,
    llvm::AArch64::LDRSHWui,
    llvm::AArch64::LDRSHXpost,
    llvm::AArch64::LDRSHXpre,
    llvm::AArch64::LDRSHXroW,
    llvm::AArch64::LDRSHXroX,
    llvm::AArch64::LDRSHXui,
    llvm::AArch64::LDSETAH,
    llvm::AArch64::LDSETALH,
    llvm::AArch64::LDSETH,
    llvm::AArch64::LDSETLH,
    llvm::AArch64::LDSMAXAH,
    llvm::AArch64::LDSMAXALH,
    llvm::AArch64::LDSMAXH,
    llvm::AArch64::LDSMAXLH,
    llvm::AArch64::LDSMINAH,
    llvm::AArch64::LDSMINALH,
    llvm::AArch64::LDSMINH,
    llvm::AArch64::LDSMINLH,
    llvm::AArch64::LDTRHi,
    llvm::AArch64::LDTRSHWi,
    llvm::AArch64::LDTRSHXi,
    llvm::AArch64::LDUMAXAH,
    llvm::AArch64::LDUMAXALH,
    llvm::AArch64::LDUMAXH,
    llvm::AArch64::LDUMAXLH,
    llvm::AArch64::LDUMINAH,
    llvm::AArch64::LDUMINALH,
    llvm::AArch64::LDUMINH,
    llvm::AArch64::LDUMINLH,
    llvm::AArch64::LDURHHi,
    llvm::AArch64::LDURHi,
    llvm::AArch64::LDURSHWi,
    llvm::AArch64::LDURSHXi,
    llvm::AArch64::LDXRH,
    llvm::AArch64::SWPAH,
    llvm::AArch64::SWPALH,
    llvm::AArch64::SWPH,
    llvm::AArch64::SWPLH,
    // clang-format on
};

constexpr size_t READ_16_P1_SIZE = sizeof(READ_16_P1) / sizeof(unsigned);

constexpr unsigned READ_16_P2[] = {
    // clang-format off
    llvm::AArch64::LD2Rv4h,
    llvm::AArch64::LD2Rv4h_POST,
    llvm::AArch64::LD2Rv8h,
    llvm::AArch64::LD2Rv8h_POST,
    llvm::AArch64::LD2i16,
    llvm::AArch64::LD2i16_POST,
    // clang-format on
};

constexpr size_t READ_16_P2_SIZE = sizeof(READ_16_P2) / sizeof(unsigned);

constexpr unsigned READ_16_P3[] = {
    // clang-format off
    llvm::AArch64::LD3Rv4h,
    llvm::AArch64::LD3Rv4h_POST,
    llvm::AArch64::LD3Rv8h,
    llvm::AArch64::LD3Rv8h_POST,
    llvm::AArch64::LD3i16,
    llvm::AArch64::LD3i16_POST,
    // clang-format on
};

constexpr size_t READ_16_P3_SIZE = sizeof(READ_16_P3) / sizeof(unsigned);

constexpr unsigned READ_16_P4[] = {
    // clang-format off
    llvm::AArch64::LD4Rv4h,
    llvm::AArch64::LD4Rv4h_POST,
    llvm::AArch64::LD4Rv8h,
    llvm::AArch64::LD4Rv8h_POST,
    llvm::AArch64::LD4i16,
    llvm::AArch64::LD4i16_POST,
    // clang-format on
};

constexpr size_t READ_16_P4_SIZE = sizeof(READ_16_P4) / sizeof(unsigned);

constexpr unsigned READ_32_P1[] = {
    // clang-format off
    llvm::AArch64::CASALW,
    llvm::AArch64::CASAW,
    llvm::AArch64::CASLW,
    llvm::AArch64::CASW,
    llvm::AArch64::LD1Rv2s,
    llvm::AArch64::LD1Rv2s_POST,
    llvm::AArch64::LD1Rv4s,
    llvm::AArch64::LD1Rv4s_POST,
    llvm::AArch64::LD1i32,
    llvm::AArch64::LD1i32_POST,
    llvm::AArch64::LDADDALW,
    llvm::AArch64::LDADDAW,
    llvm::AArch64::LDADDLW,
    llvm::AArch64::LDADDW,
    llvm::AArch64::LDAPRW,
    llvm::AArch64::LDAPRWpost,
    llvm::AArch64::LDAPURSWi,
    llvm::AArch64::LDAPURi,
    llvm::AArch64::LDAPURsi,
    llvm::AArch64::LDARW,
    llvm::AArch64::LDAXRW,
    llvm::AArch64::LDCLRALW,
    llvm::AArch64::LDCLRAW,
    llvm::AArch64::LDCLRLW,
    llvm::AArch64::LDCLRW,
    llvm::AArch64::LDEORALW,
    llvm::AArch64::LDEORAW,
    llvm::AArch64::LDEORLW,
    llvm::AArch64::LDEORW,
    llvm::AArch64::LDLARW,
    llvm::AArch64::LDRSWl,
    llvm::AArch64::LDRSWpost,
    llvm::AArch64::LDRSWpre,
    llvm::AArch64::LDRSWroW,
    llvm::AArch64::LDRSWroX,
    llvm::AArch64::LDRSWui,
    llvm::AArch64::LDRSl,
    llvm::AArch64::LDRSpost,
    llvm::AArch64::LDRSpre,
    llvm::AArch64::LDRSroW,
    llvm::AArch64::LDRSroX,
    llvm::AArch64::LDRSui,
    llvm::AArch64::LDRWl,
    llvm::AArch64::LDRWpost,
    llvm::AArch64::LDRWpre,
    llvm::AArch64::LDRWroW,
    llvm::AArch64::LDRWroX,
    llvm::AArch64::LDRWui,
    llvm::AArch64::LDSETALW,
    llvm::AArch64::LDSETAW,
    llvm::AArch64::LDSETLW,
    llvm::AArch64::LDSETW,
    llvm::AArch64::LDSMAXALW,
    llvm::AArch64::LDSMAXAW,
    llvm::AArch64::LDSMAXLW,
    llvm::AArch64::LDSMAXW,
    llvm::AArch64::LDSMINALW,
    llvm::AArch64::LDSMINAW,
    llvm::AArch64::LDSMINLW,
    llvm::AArch64::LDSMINW,
    llvm::AArch64::LDTRSWi,
    llvm::AArch64::LDTRWi,
    llvm::AArch64::LDUMAXALW,
    llvm::AArch64::LDUMAXAW,
    llvm::AArch64::LDUMAXLW,
    llvm::AArch64::LDUMAXW,
    llvm::AArch64::LDUMINALW,
    llvm::AArch64::LDUMINAW,
    llvm::AArch64::LDUMINLW,
    llvm::AArch64::LDUMINW,
    llvm::AArch64::LDURSWi,
    llvm::AArch64::LDURSi,
    llvm::AArch64::LDURWi,
    llvm::AArch64::LDXRW,
    llvm::AArch64::SWPALW,
    llvm::AArch64::SWPAW,
    llvm::AArch64::SWPLW,
    llvm::AArch64::SWPW,
    // clang-format on
};

constexpr size_t READ_32_P1_SIZE = sizeof(READ_32_P1) / sizeof(unsigned);

constexpr unsigned READ_32_P2[] = {
    // clang-format off
    llvm::AArch64::CASPALW,
    llvm::AArch64::CASPAW,
    llvm::AArch64::CASPLW,
    llvm::AArch64::CASPW,
    llvm::AArch64::LD2Rv2s,
    llvm::AArch64::LD2Rv2s_POST,
    llvm::AArch64::LD2Rv4s,
    llvm::AArch64::LD2Rv4s_POST,
    llvm::AArch64::LD2i32,
    llvm::AArch64::LD2i32_POST,
    llvm::AArch64::LDAXPW,
    llvm::AArch64::LDIAPPW,
    llvm::AArch64::LDIAPPWpost,
    llvm::AArch64::LDNPSi,
    llvm::AArch64::LDNPWi,
    llvm::AArch64::LDPSWi,
    llvm::AArch64::LDPSWpost,
    llvm::AArch64::LDPSWpre,
    llvm::AArch64::LDPSi,
    llvm::AArch64::LDPSpost,
    llvm::AArch64::LDPSpre,
    llvm::AArch64::LDPWi,
    llvm::AArch64::LDPWpost,
    llvm::AArch64::LDPWpre,
    llvm::AArch64::LDXPW,
    // clang-format on
};

constexpr size_t READ_32_P2_SIZE = sizeof(READ_32_P2) / sizeof(unsigned);

constexpr unsigned READ_32_P3[] = {
    // clang-format off
    llvm::AArch64::LD3Rv2s,
    llvm::AArch64::LD3Rv2s_POST,
    llvm::AArch64::LD3Rv4s,
    llvm::AArch64::LD3Rv4s_POST,
    llvm::AArch64::LD3i32,
    llvm::AArch64::LD3i32_POST,
    // clang-format on
};

constexpr size_t READ_32_P3_SIZE = sizeof(READ_32_P3) / sizeof(unsigned);

constexpr unsigned READ_32_P4[] = {
    // clang-format off
    llvm::AArch64::LD4Rv2s,
    llvm::AArch64::LD4Rv2s_POST,
    llvm::AArch64::LD4Rv4s,
    llvm::AArch64::LD4Rv4s_POST,
    llvm::AArch64::LD4i32,
    llvm::AArch64::LD4i32_POST,
    // clang-format on
};

constexpr size_t READ_32_P4_SIZE = sizeof(READ_32_P4) / sizeof(unsigned);

constexpr unsigned READ_64_P1[] = {
    // clang-format off
    llvm::AArch64::CASALX,
    llvm::AArch64::CASAX,
    llvm::AArch64::CASLX,
    llvm::AArch64::CASX,
    llvm::AArch64::LD1Onev1d,
    llvm::AArch64::LD1Onev1d_POST,
    llvm::AArch64::LD1Onev2s,
    llvm::AArch64::LD1Onev2s_POST,
    llvm::AArch64::LD1Onev4h,
    llvm::AArch64::LD1Onev4h_POST,
    llvm::AArch64::LD1Onev8b,
    llvm::AArch64::LD1Onev8b_POST,
    llvm::AArch64::LD1Rv1d,
    llvm::AArch64::LD1Rv1d_POST,
    llvm::AArch64::LD1Rv2d,
    llvm::AArch64::LD1Rv2d_POST,
    llvm::AArch64::LD1i64,
    llvm::AArch64::LD1i64_POST,
    llvm::AArch64::LDADDALX,
    llvm::AArch64::LDADDAX,
    llvm::AArch64::LDADDLX,
    llvm::AArch64::LDADDX,
    llvm::AArch64::LDAP1,
    llvm::AArch64::LDAPRX,
    llvm::AArch64::LDAPRXpost,
    llvm::AArch64::LDAPURXi,
    llvm::AArch64::LDAPURdi,
    llvm::AArch64::LDARX,
    llvm::AArch64::LDAXRX,
    llvm::AArch64::LDCLRALX,
    llvm::AArch64::LDCLRAX,
    llvm::AArch64::LDCLRLX,
    llvm::AArch64::LDCLRX,
    llvm::AArch64::LDEORALX,
    llvm::AArch64::LDEORAX,
    llvm::AArch64::LDEORLX,
    llvm::AArch64::LDEORX,
    llvm::AArch64::LDLARX,
    llvm::AArch64::LDRAAindexed,
    llvm::AArch64::LDRAAwriteback,
    llvm::AArch64::LDRABindexed,
    llvm::AArch64::LDRABwriteback,
    llvm::AArch64::LDRDl,
    llvm::AArch64::LDRDpost,
    llvm::AArch64::LDRDpre,
    llvm::AArch64::LDRDroW,
    llvm::AArch64::LDRDroX,
    llvm::AArch64::LDRDui,
    llvm::AArch64::LDRXl,
    llvm::AArch64::LDRXpost,
    llvm::AArch64::LDRXpre,
    llvm::AArch64::LDRXroW,
    llvm::AArch64::LDRXroX,
    llvm::AArch64::LDRXui,
    llvm::AArch64::LDSETALX,
    llvm::AArch64::LDSETAX,
    llvm::AArch64::LDSETLX,
    llvm::AArch64::LDSETX,
    llvm::AArch64::LDSMAXALX,
    llvm::AArch64::LDSMAXAX,
    llvm::AArch64::LDSMAXLX,
    llvm::AArch64::LDSMAXX,
    llvm::AArch64::LDSMINALX,
    llvm::AArch64::LDSMINAX,
    llvm::AArch64::LDSMINLX,
    llvm::AArch64::LDSMINX,
    llvm::AArch64::LDTRXi,
    llvm::AArch64::LDUMAXALX,
    llvm::AArch64::LDUMAXAX,
    llvm::AArch64::LDUMAXLX,
    llvm::AArch64::LDUMAXX,
    llvm::AArch64::LDUMINALX,
    llvm::AArch64::LDUMINAX,
    llvm::AArch64::LDUMINLX,
    llvm::AArch64::LDUMINX,
    llvm::AArch64::LDURDi,
    llvm::AArch64::LDURXi,
    llvm::AArch64::LDXRX,
    llvm::AArch64::RCWCAS,
    llvm::AArch64::RCWCASA,
    llvm::AArch64::RCWCASAL,
    llvm::AArch64::RCWCASL,
    llvm::AArch64::RCWCLR,
    llvm::AArch64::RCWCLRA,
    llvm::AArch64::RCWCLRAL,
    llvm::AArch64::RCWCLRL,
    llvm::AArch64::RCWCLRS,
    llvm::AArch64::RCWCLRSA,
    llvm::AArch64::RCWCLRSAL,
    llvm::AArch64::RCWCLRSL,
    llvm::AArch64::RCWSCAS,
    llvm::AArch64::RCWSCASA,
    llvm::AArch64::RCWSCASAL,
    llvm::AArch64::RCWSCASL,
    llvm::AArch64::RCWSET,
    llvm::AArch64::RCWSETA,
    llvm::AArch64::RCWSETAL,
    llvm::AArch64::RCWSETL,
    llvm::AArch64::RCWSETS,
    llvm::AArch64::RCWSETSA,
    llvm::AArch64::RCWSETSAL,
    llvm::AArch64::RCWSETSL,
    llvm::AArch64::RCWSWP,
    llvm::AArch64::RCWSWPA,
    llvm::AArch64::RCWSWPAL,
    llvm::AArch64::RCWSWPL,
    llvm::AArch64::RCWSWPS,
    llvm::AArch64::RCWSWPSA,
    llvm::AArch64::RCWSWPSAL,
    llvm::AArch64::RCWSWPSL,
    llvm::AArch64::SWPALX,
    llvm::AArch64::SWPAX,
    llvm::AArch64::SWPLX,
    llvm::AArch64::SWPX,
    // clang-format on
};

constexpr size_t READ_64_P1_SIZE = sizeof(READ_64_P1) / sizeof(unsigned);

constexpr unsigned READ_64_P2[] = {
    // clang-format off
    llvm::AArch64::CASPALX,
    llvm::AArch64::CASPAX,
    llvm::AArch64::CASPLX,
    llvm::AArch64::CASPX,
    llvm::AArch64::LD1Twov1d,
    llvm::AArch64::LD1Twov1d_POST,
    llvm::AArch64::LD1Twov2s,
    llvm::AArch64::LD1Twov2s_POST,
    llvm::AArch64::LD1Twov4h,
    llvm::AArch64::LD1Twov4h_POST,
    llvm::AArch64::LD1Twov8b,
    llvm::AArch64::LD1Twov8b_POST,
    llvm::AArch64::LD2Rv1d,
    llvm::AArch64::LD2Rv1d_POST,
    llvm::AArch64::LD2Rv2d,
    llvm::AArch64::LD2Rv2d_POST,
    llvm::AArch64::LD2Twov2s,
    llvm::AArch64::LD2Twov2s_POST,
    llvm::AArch64::LD2Twov4h,
    llvm::AArch64::LD2Twov4h_POST,
    llvm::AArch64::LD2Twov8b,
    llvm::AArch64::LD2Twov8b_POST,
    llvm::AArch64::LD2i64,
    llvm::AArch64::LD2i64_POST,
    llvm::AArch64::LDAXPX,
    llvm::AArch64::LDIAPPX,
    llvm::AArch64::LDIAPPXpost,
    llvm::AArch64::LDNPDi,
    llvm::AArch64::LDNPXi,
    llvm::AArch64::LDPDi,
    llvm::AArch64::LDPDpost,
    llvm::AArch64::LDPDpre,
    llvm::AArch64::LDPXi,
    llvm::AArch64::LDPXpost,
    llvm::AArch64::LDPXpre,
    llvm::AArch64::LDXPX,
    llvm::AArch64::RCWCASP,
    llvm::AArch64::RCWCASPA,
    llvm::AArch64::RCWCASPAL,
    llvm::AArch64::RCWCASPL,
    llvm::AArch64::RCWCLRP,
    llvm::AArch64::RCWCLRPA,
    llvm::AArch64::RCWCLRPAL,
    llvm::AArch64::RCWCLRPL,
    llvm::AArch64::RCWCLRSP,
    llvm::AArch64::RCWCLRSPA,
    llvm::AArch64::RCWCLRSPAL,
    llvm::AArch64::RCWCLRSPL,
    llvm::AArch64::RCWSCASP,
    llvm::AArch64::RCWSCASPA,
    llvm::AArch64::RCWSCASPAL,
    llvm::AArch64::RCWSCASPL,
    llvm::AArch64::RCWSETP,
    llvm::AArch64::RCWSETPA,
    llvm::AArch64::RCWSETPAL,
    llvm::AArch64::RCWSETPL,
    llvm::AArch64::RCWSETSP,
    llvm::AArch64::RCWSETSPA,
    llvm::AArch64::RCWSETSPAL,
    llvm::AArch64::RCWSETSPL,
    llvm::AArch64::RCWSWPP,
    llvm::AArch64::RCWSWPPA,
    llvm::AArch64::RCWSWPPAL,
    llvm::AArch64::RCWSWPPL,
    llvm::AArch64::RCWSWPSP,
    llvm::AArch64::RCWSWPSPA,
    llvm::AArch64::RCWSWPSPAL,
    llvm::AArch64::RCWSWPSPL,
    llvm::AArch64::SWPP,
    llvm::AArch64::SWPPA,
    llvm::AArch64::SWPPAL,
    llvm::AArch64::SWPPL,
    // clang-format on
};

constexpr size_t READ_64_P2_SIZE = sizeof(READ_64_P2) / sizeof(unsigned);

constexpr unsigned READ_64_P3[] = {
    // clang-format off
    llvm::AArch64::LD1Threev1d,
    llvm::AArch64::LD1Threev1d_POST,
    llvm::AArch64::LD1Threev2s,
    llvm::AArch64::LD1Threev2s_POST,
    llvm::AArch64::LD1Threev4h,
    llvm::AArch64::LD1Threev4h_POST,
    llvm::AArch64::LD1Threev8b,
    llvm::AArch64::LD1Threev8b_POST,
    llvm::AArch64::LD3Rv1d,
    llvm::AArch64::LD3Rv1d_POST,
    llvm::AArch64::LD3Rv2d,
    llvm::AArch64::LD3Rv2d_POST,
    llvm::AArch64::LD3Threev2s,
    llvm::AArch64::LD3Threev2s_POST,
    llvm::AArch64::LD3Threev4h,
    llvm::AArch64::LD3Threev4h_POST,
    llvm::AArch64::LD3Threev8b,
    llvm::AArch64::LD3Threev8b_POST,
    llvm::AArch64::LD3i64,
    llvm::AArch64::LD3i64_POST,
    // clang-format on
};

constexpr size_t READ_64_P3_SIZE = sizeof(READ_64_P3) / sizeof(unsigned);

constexpr unsigned READ_64_P4[] = {
    // clang-format off
    llvm::AArch64::LD1Fourv1d,
    llvm::AArch64::LD1Fourv1d_POST,
    llvm::AArch64::LD1Fourv2s,
    llvm::AArch64::LD1Fourv2s_POST,
    llvm::AArch64::LD1Fourv4h,
    llvm::AArch64::LD1Fourv4h_POST,
    llvm::AArch64::LD1Fourv8b,
    llvm::AArch64::LD1Fourv8b_POST,
    llvm::AArch64::LD4Fourv2s,
    llvm::AArch64::LD4Fourv2s_POST,
    llvm::AArch64::LD4Fourv4h,
    llvm::AArch64::LD4Fourv4h_POST,
    llvm::AArch64::LD4Fourv8b,
    llvm::AArch64::LD4Fourv8b_POST,
    llvm::AArch64::LD4Rv1d,
    llvm::AArch64::LD4Rv1d_POST,
    llvm::AArch64::LD4Rv2d,
    llvm::AArch64::LD4Rv2d_POST,
    llvm::AArch64::LD4i64,
    llvm::AArch64::LD4i64_POST,
    // clang-format on
};

constexpr size_t READ_64_P4_SIZE = sizeof(READ_64_P4) / sizeof(unsigned);

constexpr unsigned READ_64_P8[] = {
    // clang-format off
    llvm::AArch64::LD64B,
    // clang-format on
};

constexpr size_t READ_64_P8_SIZE = sizeof(READ_64_P8) / sizeof(unsigned);

constexpr unsigned READ_128_P1[] = {
    // clang-format off
    llvm::AArch64::LD1Onev16b,
    llvm::AArch64::LD1Onev16b_POST,
    llvm::AArch64::LD1Onev2d,
    llvm::AArch64::LD1Onev2d_POST,
    llvm::AArch64::LD1Onev4s,
    llvm::AArch64::LD1Onev4s_POST,
    llvm::AArch64::LD1Onev8h,
    llvm::AArch64::LD1Onev8h_POST,
    llvm::AArch64::LDAPURqi,
    llvm::AArch64::LDCLRP,
    llvm::AArch64::LDCLRPA,
    llvm::AArch64::LDCLRPAL,
    llvm::AArch64::LDCLRPL,
    llvm::AArch64::LDRQl,
    llvm::AArch64::LDRQpost,
    llvm::AArch64::LDRQpre,
    llvm::AArch64::LDRQroW,
    llvm::AArch64::LDRQroX,
    llvm::AArch64::LDRQui,
    llvm::AArch64::LDSETP,
    llvm::AArch64::LDSETPA,
    llvm::AArch64::LDSETPAL,
    llvm::AArch64::LDSETPL,
    llvm::AArch64::LDURQi,
    // clang-format on
};

constexpr size_t READ_128_P1_SIZE = sizeof(READ_128_P1) / sizeof(unsigned);

constexpr unsigned READ_128_P2[] = {
    // clang-format off
    llvm::AArch64::LD1Twov16b,
    llvm::AArch64::LD1Twov16b_POST,
    llvm::AArch64::LD1Twov2d,
    llvm::AArch64::LD1Twov2d_POST,
    llvm::AArch64::LD1Twov4s,
    llvm::AArch64::LD1Twov4s_POST,
    llvm::AArch64::LD1Twov8h,
    llvm::AArch64::LD1Twov8h_POST,
    llvm::AArch64::LD2Twov16b,
    llvm::AArch64::LD2Twov16b_POST,
    llvm::AArch64::LD2Twov2d,
    llvm::AArch64::LD2Twov2d_POST,
    llvm::AArch64::LD2Twov4s,
    llvm::AArch64::LD2Twov4s_POST,
    llvm::AArch64::LD2Twov8h,
    llvm::AArch64::LD2Twov8h_POST,
    llvm::AArch64::LDNPQi,
    llvm::AArch64::LDPQi,
    llvm::AArch64::LDPQpost,
    llvm::AArch64::LDPQpre,
    // clang-format on
};

constexpr size_t READ_128_P2_SIZE = sizeof(READ_128_P2) / sizeof(unsigned);

constexpr unsigned READ_128_P3[] = {
    // clang-format off
    llvm::AArch64::LD1Threev16b,
    llvm::AArch64::LD1Threev16b_POST,
    llvm::AArch64::LD1Threev2d,
    llvm::AArch64::LD1Threev2d_POST,
    llvm::AArch64::LD1Threev4s,
    llvm::AArch64::LD1Threev4s_POST,
    llvm::AArch64::LD1Threev8h,
    llvm::AArch64::LD1Threev8h_POST,
    llvm::AArch64::LD3Threev16b,
    llvm::AArch64::LD3Threev16b_POST,
    llvm::AArch64::LD3Threev2d,
    llvm::AArch64::LD3Threev2d_POST,
    llvm::AArch64::LD3Threev4s,
    llvm::AArch64::LD3Threev4s_POST,
    llvm::AArch64::LD3Threev8h,
    llvm::AArch64::LD3Threev8h_POST,
    // clang-format on
};

constexpr size_t READ_128_P3_SIZE = sizeof(READ_128_P3) / sizeof(unsigned);

constexpr unsigned READ_128_P4[] = {
    // clang-format off
    llvm::AArch64::LD1Fourv16b,
    llvm::AArch64::LD1Fourv16b_POST,
    llvm::AArch64::LD1Fourv2d,
    llvm::AArch64::LD1Fourv2d_POST,
    llvm::AArch64::LD1Fourv4s,
    llvm::AArch64::LD1Fourv4s_POST,
    llvm::AArch64::LD1Fourv8h,
    llvm::AArch64::LD1Fourv8h_POST,
    llvm::AArch64::LD4Fourv16b,
    llvm::AArch64::LD4Fourv16b_POST,
    llvm::AArch64::LD4Fourv2d,
    llvm::AArch64::LD4Fourv2d_POST,
    llvm::AArch64::LD4Fourv4s,
    llvm::AArch64::LD4Fourv4s_POST,
    llvm::AArch64::LD4Fourv8h,
    llvm::AArch64::LD4Fourv8h_POST,
    // clang-format on
};

constexpr size_t READ_128_P4_SIZE = sizeof(READ_128_P4) / sizeof(unsigned);

constexpr unsigned READ_DYN[] = {
    // clang-format off
    llvm::AArch64::CPYE,
    llvm::AArch64::CPYEN,
    llvm::AArch64::CPYERN,
    llvm::AArch64::CPYERT,
    llvm::AArch64::CPYERTN,
    llvm::AArch64::CPYERTRN,
    llvm::AArch64::CPYERTWN,
    llvm::AArch64::CPYET,
    llvm::AArch64::CPYETN,
    llvm::AArch64::CPYETRN,
    llvm::AArch64::CPYETWN,
    llvm::AArch64::CPYEWN,
    llvm::AArch64::CPYEWT,
    llvm::AArch64::CPYEWTN,
    llvm::AArch64::CPYEWTRN,
    llvm::AArch64::CPYEWTWN,
    llvm::AArch64::CPYFE,
    llvm::AArch64::CPYFEN,
    llvm::AArch64::CPYFERN,
    llvm::AArch64::CPYFERT,
    llvm::AArch64::CPYFERTN,
    llvm::AArch64::CPYFERTRN,
    llvm::AArch64::CPYFERTWN,
    llvm::AArch64::CPYFET,
    llvm::AArch64::CPYFETN,
    llvm::AArch64::CPYFETRN,
    llvm::AArch64::CPYFETWN,
    llvm::AArch64::CPYFEWN,
    llvm::AArch64::CPYFEWT,
    llvm::AArch64::CPYFEWTN,
    llvm::AArch64::CPYFEWTRN,
    llvm::AArch64::CPYFEWTWN,
    llvm::AArch64::CPYFM,
    llvm::AArch64::CPYFMN,
    llvm::AArch64::CPYFMRN,
    llvm::AArch64::CPYFMRT,
    llvm::AArch64::CPYFMRTN,
    llvm::AArch64::CPYFMRTRN,
    llvm::AArch64::CPYFMRTWN,
    llvm::AArch64::CPYFMT,
    llvm::AArch64::CPYFMTN,
    llvm::AArch64::CPYFMTRN,
    llvm::AArch64::CPYFMTWN,
    llvm::AArch64::CPYFMWN,
    llvm::AArch64::CPYFMWT,
    llvm::AArch64::CPYFMWTN,
    llvm::AArch64::CPYFMWTRN,
    llvm::AArch64::CPYFMWTWN,
    llvm::AArch64::CPYFP,
    llvm::AArch64::CPYFPN,
    llvm::AArch64::CPYFPRN,
    llvm::AArch64::CPYFPRT,
    llvm::AArch64::CPYFPRTN,
    llvm::AArch64::CPYFPRTRN,
    llvm::AArch64::CPYFPRTWN,
    llvm::AArch64::CPYFPT,
    llvm::AArch64::CPYFPTN,
    llvm::AArch64::CPYFPTRN,
    llvm::AArch64::CPYFPTWN,
    llvm::AArch64::CPYFPWN,
    llvm::AArch64::CPYFPWT,
    llvm::AArch64::CPYFPWTN,
    llvm::AArch64::CPYFPWTRN,
    llvm::AArch64::CPYFPWTWN,
    llvm::AArch64::CPYM,
    llvm::AArch64::CPYMN,
    llvm::AArch64::CPYMRN,
    llvm::AArch64::CPYMRT,
    llvm::AArch64::CPYMRTN,
    llvm::AArch64::CPYMRTRN,
    llvm::AArch64::CPYMRTWN,
    llvm::AArch64::CPYMT,
    llvm::AArch64::CPYMTN,
    llvm::AArch64::CPYMTRN,
    llvm::AArch64::CPYMTWN,
    llvm::AArch64::CPYMWN,
    llvm::AArch64::CPYMWT,
    llvm::AArch64::CPYMWTN,
    llvm::AArch64::CPYMWTRN,
    llvm::AArch64::CPYMWTWN,
    llvm::AArch64::CPYP,
    llvm::AArch64::CPYPN,
    llvm::AArch64::CPYPRN,
    llvm::AArch64::CPYPRT,
    llvm::AArch64::CPYPRTN,
    llvm::AArch64::CPYPRTRN,
    llvm::AArch64::CPYPRTWN,
    llvm::AArch64::CPYPT,
    llvm::AArch64::CPYPTN,
    llvm::AArch64::CPYPTRN,
    llvm::AArch64::CPYPTWN,
    llvm::AArch64::CPYPWN,
    llvm::AArch64::CPYPWT,
    llvm::AArch64::CPYPWTN,
    llvm::AArch64::CPYPWTRN,
    llvm::AArch64::CPYPWTWN,
    // clang-format on
};

constexpr size_t READ_DYN_SIZE = sizeof(READ_DYN) / sizeof(unsigned);

// Write Instructions
// ==================
constexpr unsigned WRITE_8_P1[] = {
    // clang-format off
    llvm::AArch64::CASAB,
    llvm::AArch64::CASALB,
    llvm::AArch64::CASB,
    llvm::AArch64::CASLB,
    llvm::AArch64::LDADDAB,
    llvm::AArch64::LDADDALB,
    llvm::AArch64::LDADDB,
    llvm::AArch64::LDADDLB,
    llvm::AArch64::LDCLRAB,
    llvm::AArch64::LDCLRALB,
    llvm::AArch64::LDCLRB,
    llvm::AArch64::LDCLRLB,
    llvm::AArch64::LDEORAB,
    llvm::AArch64::LDEORALB,
    llvm::AArch64::LDEORB,
    llvm::AArch64::LDEORLB,
    llvm::AArch64::LDSETAB,
    llvm::AArch64::LDSETALB,
    llvm::AArch64::LDSETB,
    llvm::AArch64::LDSETLB,
    llvm::AArch64::LDSMAXAB,
    llvm::AArch64::LDSMAXALB,
    llvm::AArch64::LDSMAXB,
    llvm::AArch64::LDSMAXLB,
    llvm::AArch64::LDSMINAB,
    llvm::AArch64::LDSMINALB,
    llvm::AArch64::LDSMINB,
    llvm::AArch64::LDSMINLB,
    llvm::AArch64::LDUMAXAB,
    llvm::AArch64::LDUMAXALB,
    llvm::AArch64::LDUMAXB,
    llvm::AArch64::LDUMAXLB,
    llvm::AArch64::LDUMINAB,
    llvm::AArch64::LDUMINALB,
    llvm::AArch64::LDUMINB,
    llvm::AArch64::LDUMINLB,
    llvm::AArch64::ST1i8,
    llvm::AArch64::ST1i8_POST,
    llvm::AArch64::STLLRB,
    llvm::AArch64::STLRB,
    llvm::AArch64::STLURBi,
    llvm::AArch64::STLURbi,
    llvm::AArch64::STLXRB,
    llvm::AArch64::STRBBpost,
    llvm::AArch64::STRBBpre,
    llvm::AArch64::STRBBroW,
    llvm::AArch64::STRBBroX,
    llvm::AArch64::STRBBui,
    llvm::AArch64::STRBpost,
    llvm::AArch64::STRBpre,
    llvm::AArch64::STRBroW,
    llvm::AArch64::STRBroX,
    llvm::AArch64::STRBui,
    llvm::AArch64::STTRBi,
    llvm::AArch64::STURBBi,
    llvm::AArch64::STURBi,
    llvm::AArch64::STXRB,
    llvm::AArch64::SWPAB,
    llvm::AArch64::SWPALB,
    llvm::AArch64::SWPB,
    llvm::AArch64::SWPLB,
    // clang-format on
};

constexpr size_t WRITE_8_P1_SIZE = sizeof(WRITE_8_P1) / sizeof(unsigned);

constexpr unsigned WRITE_8_P2[] = {
    // clang-format off
    llvm::AArch64::ST2i8,
    llvm::AArch64::ST2i8_POST,
    // clang-format on
};

constexpr size_t WRITE_8_P2_SIZE = sizeof(WRITE_8_P2) / sizeof(unsigned);

constexpr unsigned WRITE_8_P3[] = {
    // clang-format off
    llvm::AArch64::ST3i8,
    llvm::AArch64::ST3i8_POST,
    // clang-format on
};

constexpr size_t WRITE_8_P3_SIZE = sizeof(WRITE_8_P3) / sizeof(unsigned);

constexpr unsigned WRITE_8_P4[] = {
    // clang-format off
    llvm::AArch64::ST4i8,
    llvm::AArch64::ST4i8_POST,
    // clang-format on
};

constexpr size_t WRITE_8_P4_SIZE = sizeof(WRITE_8_P4) / sizeof(unsigned);

constexpr unsigned WRITE_16_P1[] = {
    // clang-format off
    llvm::AArch64::CASAH,
    llvm::AArch64::CASALH,
    llvm::AArch64::CASH,
    llvm::AArch64::CASLH,
    llvm::AArch64::LDADDAH,
    llvm::AArch64::LDADDALH,
    llvm::AArch64::LDADDH,
    llvm::AArch64::LDADDLH,
    llvm::AArch64::LDCLRAH,
    llvm::AArch64::LDCLRALH,
    llvm::AArch64::LDCLRH,
    llvm::AArch64::LDCLRLH,
    llvm::AArch64::LDEORAH,
    llvm::AArch64::LDEORALH,
    llvm::AArch64::LDEORH,
    llvm::AArch64::LDEORLH,
    llvm::AArch64::LDSETAH,
    llvm::AArch64::LDSETALH,
    llvm::AArch64::LDSETH,
    llvm::AArch64::LDSETLH,
    llvm::AArch64::LDSMAXAH,
    llvm::AArch64::LDSMAXALH,
    llvm::AArch64::LDSMAXH,
    llvm::AArch64::LDSMAXLH,
    llvm::AArch64::LDSMINAH,
    llvm::AArch64::LDSMINALH,
    llvm::AArch64::LDSMINH,
    llvm::AArch64::LDSMINLH,
    llvm::AArch64::LDUMAXAH,
    llvm::AArch64::LDUMAXALH,
    llvm::AArch64::LDUMAXH,
    llvm::AArch64::LDUMAXLH,
    llvm::AArch64::LDUMINAH,
    llvm::AArch64::LDUMINALH,
    llvm::AArch64::LDUMINH,
    llvm::AArch64::LDUMINLH,
    llvm::AArch64::ST1i16,
    llvm::AArch64::ST1i16_POST,
    llvm::AArch64::STLLRH,
    llvm::AArch64::STLRH,
    llvm::AArch64::STLURHi,
    llvm::AArch64::STLURhi,
    llvm::AArch64::STLXRH,
    llvm::AArch64::STRHHpost,
    llvm::AArch64::STRHHpre,
    llvm::AArch64::STRHHroW,
    llvm::AArch64::STRHHroX,
    llvm::AArch64::STRHHui,
    llvm::AArch64::STRHpost,
    llvm::AArch64::STRHpre,
    llvm::AArch64::STRHroW,
    llvm::AArch64::STRHroX,
    llvm::AArch64::STRHui,
    llvm::AArch64::STTRHi,
    llvm::AArch64::STURHHi,
    llvm::AArch64::STURHi,
    llvm::AArch64::STXRH,
    llvm::AArch64::SWPAH,
    llvm::AArch64::SWPALH,
    llvm::AArch64::SWPH,
    llvm::AArch64::SWPLH,
    // clang-format on
};

constexpr size_t WRITE_16_P1_SIZE = sizeof(WRITE_16_P1) / sizeof(unsigned);

constexpr unsigned WRITE_16_P2[] = {
    // clang-format off
    llvm::AArch64::ST2i16,
    llvm::AArch64::ST2i16_POST,
    // clang-format on
};

constexpr size_t WRITE_16_P2_SIZE = sizeof(WRITE_16_P2) / sizeof(unsigned);

constexpr unsigned WRITE_16_P3[] = {
    // clang-format off
    llvm::AArch64::ST3i16,
    llvm::AArch64::ST3i16_POST,
    // clang-format on
};

constexpr size_t WRITE_16_P3_SIZE = sizeof(WRITE_16_P3) / sizeof(unsigned);

constexpr unsigned WRITE_16_P4[] = {
    // clang-format off
    llvm::AArch64::ST4i16,
    llvm::AArch64::ST4i16_POST,
    // clang-format on
};

constexpr size_t WRITE_16_P4_SIZE = sizeof(WRITE_16_P4) / sizeof(unsigned);

constexpr unsigned WRITE_32_P1[] = {
    // clang-format off
    llvm::AArch64::CASALW,
    llvm::AArch64::CASAW,
    llvm::AArch64::CASLW,
    llvm::AArch64::CASW,
    llvm::AArch64::LDADDALW,
    llvm::AArch64::LDADDAW,
    llvm::AArch64::LDADDLW,
    llvm::AArch64::LDADDW,
    llvm::AArch64::LDCLRALW,
    llvm::AArch64::LDCLRAW,
    llvm::AArch64::LDCLRLW,
    llvm::AArch64::LDCLRW,
    llvm::AArch64::LDEORALW,
    llvm::AArch64::LDEORAW,
    llvm::AArch64::LDEORLW,
    llvm::AArch64::LDEORW,
    llvm::AArch64::LDSETALW,
    llvm::AArch64::LDSETAW,
    llvm::AArch64::LDSETLW,
    llvm::AArch64::LDSETW,
    llvm::AArch64::LDSMAXALW,
    llvm::AArch64::LDSMAXAW,
    llvm::AArch64::LDSMAXLW,
    llvm::AArch64::LDSMAXW,
    llvm::AArch64::LDSMINALW,
    llvm::AArch64::LDSMINAW,
    llvm::AArch64::LDSMINLW,
    llvm::AArch64::LDSMINW,
    llvm::AArch64::LDUMAXALW,
    llvm::AArch64::LDUMAXAW,
    llvm::AArch64::LDUMAXLW,
    llvm::AArch64::LDUMAXW,
    llvm::AArch64::LDUMINALW,
    llvm::AArch64::LDUMINAW,
    llvm::AArch64::LDUMINLW,
    llvm::AArch64::LDUMINW,
    llvm::AArch64::ST1i32,
    llvm::AArch64::ST1i32_POST,
    llvm::AArch64::STLLRW,
    llvm::AArch64::STLRW,
    llvm::AArch64::STLRWpre,
    llvm::AArch64::STLURWi,
    llvm::AArch64::STLURsi,
    llvm::AArch64::STLXRW,
    llvm::AArch64::STRSpost,
    llvm::AArch64::STRSpre,
    llvm::AArch64::STRSroW,
    llvm::AArch64::STRSroX,
    llvm::AArch64::STRSui,
    llvm::AArch64::STRWpost,
    llvm::AArch64::STRWpre,
    llvm::AArch64::STRWroW,
    llvm::AArch64::STRWroX,
    llvm::AArch64::STRWui,
    llvm::AArch64::STTRWi,
    llvm::AArch64::STURSi,
    llvm::AArch64::STURWi,
    llvm::AArch64::STXRW,
    llvm::AArch64::SWPALW,
    llvm::AArch64::SWPAW,
    llvm::AArch64::SWPLW,
    llvm::AArch64::SWPW,
    // clang-format on
};

constexpr size_t WRITE_32_P1_SIZE = sizeof(WRITE_32_P1) / sizeof(unsigned);

constexpr unsigned WRITE_32_P2[] = {
    // clang-format off
    llvm::AArch64::CASPALW,
    llvm::AArch64::CASPAW,
    llvm::AArch64::CASPLW,
    llvm::AArch64::CASPW,
    llvm::AArch64::ST2i32,
    llvm::AArch64::ST2i32_POST,
    llvm::AArch64::STILPW,
    llvm::AArch64::STILPWpre,
    llvm::AArch64::STLXPW,
    llvm::AArch64::STNPSi,
    llvm::AArch64::STNPWi,
    llvm::AArch64::STPSi,
    llvm::AArch64::STPSpost,
    llvm::AArch64::STPSpre,
    llvm::AArch64::STPWi,
    llvm::AArch64::STPWpost,
    llvm::AArch64::STPWpre,
    llvm::AArch64::STXPW,
    // clang-format on
};

constexpr size_t WRITE_32_P2_SIZE = sizeof(WRITE_32_P2) / sizeof(unsigned);

constexpr unsigned WRITE_32_P3[] = {
    // clang-format off
    llvm::AArch64::ST3i32,
    llvm::AArch64::ST3i32_POST,
    // clang-format on
};

constexpr size_t WRITE_32_P3_SIZE = sizeof(WRITE_32_P3) / sizeof(unsigned);

constexpr unsigned WRITE_32_P4[] = {
    // clang-format off
    llvm::AArch64::ST4i32,
    llvm::AArch64::ST4i32_POST,
    // clang-format on
};

constexpr size_t WRITE_32_P4_SIZE = sizeof(WRITE_32_P4) / sizeof(unsigned);

constexpr unsigned WRITE_64_P1[] = {
    // clang-format off
    llvm::AArch64::CASALX,
    llvm::AArch64::CASAX,
    llvm::AArch64::CASLX,
    llvm::AArch64::CASX,
    llvm::AArch64::GCSSTR,
    llvm::AArch64::GCSSTTR,
    llvm::AArch64::LDADDALX,
    llvm::AArch64::LDADDAX,
    llvm::AArch64::LDADDLX,
    llvm::AArch64::LDADDX,
    llvm::AArch64::LDCLRALX,
    llvm::AArch64::LDCLRAX,
    llvm::AArch64::LDCLRLX,
    llvm::AArch64::LDCLRX,
    llvm::AArch64::LDEORALX,
    llvm::AArch64::LDEORAX,
    llvm::AArch64::LDEORLX,
    llvm::AArch64::LDEORX,
    llvm::AArch64::LDSETALX,
    llvm::AArch64::LDSETAX,
    llvm::AArch64::LDSETLX,
    llvm::AArch64::LDSETX,
    llvm::AArch64::LDSMAXALX,
    llvm::AArch64::LDSMAXAX,
    llvm::AArch64::LDSMAXLX,
    llvm::AArch64::LDSMAXX,
    llvm::AArch64::LDSMINALX,
    llvm::AArch64::LDSMINAX,
    llvm::AArch64::LDSMINLX,
    llvm::AArch64::LDSMINX,
    llvm::AArch64::LDUMAXALX,
    llvm::AArch64::LDUMAXAX,
    llvm::AArch64::LDUMAXLX,
    llvm::AArch64::LDUMAXX,
    llvm::AArch64::LDUMINALX,
    llvm::AArch64::LDUMINAX,
    llvm::AArch64::LDUMINLX,
    llvm::AArch64::LDUMINX,
    llvm::AArch64::RCWCAS,
    llvm::AArch64::RCWCASA,
    llvm::AArch64::RCWCASAL,
    llvm::AArch64::RCWCASL,
    llvm::AArch64::RCWCLR,
    llvm::AArch64::RCWCLRA,
    llvm::AArch64::RCWCLRAL,
    llvm::AArch64::RCWCLRL,
    llvm::AArch64::RCWCLRS,
    llvm::AArch64::RCWCLRSA,
    llvm::AArch64::RCWCLRSAL,
    llvm::AArch64::RCWCLRSL,
    llvm::AArch64::RCWSCAS,
    llvm::AArch64::RCWSCASA,
    llvm::AArch64::RCWSCASAL,
    llvm::AArch64::RCWSCASL,
    llvm::AArch64::RCWSET,
    llvm::AArch64::RCWSETA,
    llvm::AArch64::RCWSETAL,
    llvm::AArch64::RCWSETL,
    llvm::AArch64::RCWSETS,
    llvm::AArch64::RCWSETSA,
    llvm::AArch64::RCWSETSAL,
    llvm::AArch64::RCWSETSL,
    llvm::AArch64::RCWSWP,
    llvm::AArch64::RCWSWPA,
    llvm::AArch64::RCWSWPAL,
    llvm::AArch64::RCWSWPL,
    llvm::AArch64::RCWSWPS,
    llvm::AArch64::RCWSWPSA,
    llvm::AArch64::RCWSWPSAL,
    llvm::AArch64::RCWSWPSL,
    llvm::AArch64::ST1Onev1d,
    llvm::AArch64::ST1Onev1d_POST,
    llvm::AArch64::ST1Onev2s,
    llvm::AArch64::ST1Onev2s_POST,
    llvm::AArch64::ST1Onev4h,
    llvm::AArch64::ST1Onev4h_POST,
    llvm::AArch64::ST1Onev8b,
    llvm::AArch64::ST1Onev8b_POST,
    llvm::AArch64::ST1i64,
    llvm::AArch64::ST1i64_POST,
    llvm::AArch64::STL1,
    llvm::AArch64::STLLRX,
    llvm::AArch64::STLRX,
    llvm::AArch64::STLRXpre,
    llvm::AArch64::STLURXi,
    llvm::AArch64::STLURdi,
    llvm::AArch64::STLXRX,
    llvm::AArch64::STRDpost,
    llvm::AArch64::STRDpre,
    llvm::AArch64::STRDroW,
    llvm::AArch64::STRDroX,
    llvm::AArch64::STRDui,
    llvm::AArch64::STRXpost,
    llvm::AArch64::STRXpre,
    llvm::AArch64::STRXroW,
    llvm::AArch64::STRXroX,
    llvm::AArch64::STRXui,
    llvm::AArch64::STTRXi,
    llvm::AArch64::STURDi,
    llvm::AArch64::STURXi,
    llvm::AArch64::STXRX,
    llvm::AArch64::SWPALX,
    llvm::AArch64::SWPAX,
    llvm::AArch64::SWPLX,
    llvm::AArch64::SWPX,
    // clang-format on
};

constexpr size_t WRITE_64_P1_SIZE = sizeof(WRITE_64_P1) / sizeof(unsigned);

constexpr unsigned WRITE_64_P2[] = {
    // clang-format off
    llvm::AArch64::CASPALX,
    llvm::AArch64::CASPAX,
    llvm::AArch64::CASPLX,
    llvm::AArch64::CASPX,
    llvm::AArch64::RCWCASP,
    llvm::AArch64::RCWCASPA,
    llvm::AArch64::RCWCASPAL,
    llvm::AArch64::RCWCASPL,
    llvm::AArch64::RCWCLRP,
    llvm::AArch64::RCWCLRPA,
    llvm::AArch64::RCWCLRPAL,
    llvm::AArch64::RCWCLRPL,
    llvm::AArch64::RCWCLRSP,
    llvm::AArch64::RCWCLRSPA,
    llvm::AArch64::RCWCLRSPAL,
    llvm::AArch64::RCWCLRSPL,
    llvm::AArch64::RCWSCASP,
    llvm::AArch64::RCWSCASPA,
    llvm::AArch64::RCWSCASPAL,
    llvm::AArch64::RCWSCASPL,
    llvm::AArch64::RCWSETP,
    llvm::AArch64::RCWSETPA,
    llvm::AArch64::RCWSETPAL,
    llvm::AArch64::RCWSETPL,
    llvm::AArch64::RCWSETSP,
    llvm::AArch64::RCWSETSPA,
    llvm::AArch64::RCWSETSPAL,
    llvm::AArch64::RCWSETSPL,
    llvm::AArch64::RCWSWPP,
    llvm::AArch64::RCWSWPPA,
    llvm::AArch64::RCWSWPPAL,
    llvm::AArch64::RCWSWPPL,
    llvm::AArch64::RCWSWPSP,
    llvm::AArch64::RCWSWPSPA,
    llvm::AArch64::RCWSWPSPAL,
    llvm::AArch64::RCWSWPSPL,
    llvm::AArch64::ST1Twov1d,
    llvm::AArch64::ST1Twov1d_POST,
    llvm::AArch64::ST1Twov2s,
    llvm::AArch64::ST1Twov2s_POST,
    llvm::AArch64::ST1Twov4h,
    llvm::AArch64::ST1Twov4h_POST,
    llvm::AArch64::ST1Twov8b,
    llvm::AArch64::ST1Twov8b_POST,
    llvm::AArch64::ST2Twov2s,
    llvm::AArch64::ST2Twov2s_POST,
    llvm::AArch64::ST2Twov4h,
    llvm::AArch64::ST2Twov4h_POST,
    llvm::AArch64::ST2Twov8b,
    llvm::AArch64::ST2Twov8b_POST,
    llvm::AArch64::ST2i64,
    llvm::AArch64::ST2i64_POST,
    llvm::AArch64::STILPX,
    llvm::AArch64::STILPXpre,
    llvm::AArch64::STLXPX,
    llvm::AArch64::STNPDi,
    llvm::AArch64::STNPXi,
    llvm::AArch64::STPDi,
    llvm::AArch64::STPDpost,
    llvm::AArch64::STPDpre,
    llvm::AArch64::STPXi,
    llvm::AArch64::STPXpost,
    llvm::AArch64::STPXpre,
    llvm::AArch64::STXPX,
    llvm::AArch64::SWPP,
    llvm::AArch64::SWPPA,
    llvm::AArch64::SWPPAL,
    llvm::AArch64::SWPPL,
    // clang-format on
};

constexpr size_t WRITE_64_P2_SIZE = sizeof(WRITE_64_P2) / sizeof(unsigned);

constexpr unsigned WRITE_64_P3[] = {
    // clang-format off
    llvm::AArch64::ST1Threev1d,
    llvm::AArch64::ST1Threev1d_POST,
    llvm::AArch64::ST1Threev2s,
    llvm::AArch64::ST1Threev2s_POST,
    llvm::AArch64::ST1Threev4h,
    llvm::AArch64::ST1Threev4h_POST,
    llvm::AArch64::ST1Threev8b,
    llvm::AArch64::ST1Threev8b_POST,
    llvm::AArch64::ST3Threev2s,
    llvm::AArch64::ST3Threev2s_POST,
    llvm::AArch64::ST3Threev4h,
    llvm::AArch64::ST3Threev4h_POST,
    llvm::AArch64::ST3Threev8b,
    llvm::AArch64::ST3Threev8b_POST,
    llvm::AArch64::ST3i64,
    llvm::AArch64::ST3i64_POST,
    // clang-format on
};

constexpr size_t WRITE_64_P3_SIZE = sizeof(WRITE_64_P3) / sizeof(unsigned);

constexpr unsigned WRITE_64_P4[] = {
    // clang-format off
    llvm::AArch64::ST1Fourv1d,
    llvm::AArch64::ST1Fourv1d_POST,
    llvm::AArch64::ST1Fourv2s,
    llvm::AArch64::ST1Fourv2s_POST,
    llvm::AArch64::ST1Fourv4h,
    llvm::AArch64::ST1Fourv4h_POST,
    llvm::AArch64::ST1Fourv8b,
    llvm::AArch64::ST1Fourv8b_POST,
    llvm::AArch64::ST4Fourv2s,
    llvm::AArch64::ST4Fourv2s_POST,
    llvm::AArch64::ST4Fourv4h,
    llvm::AArch64::ST4Fourv4h_POST,
    llvm::AArch64::ST4Fourv8b,
    llvm::AArch64::ST4Fourv8b_POST,
    llvm::AArch64::ST4i64,
    llvm::AArch64::ST4i64_POST,
    // clang-format on
};

constexpr size_t WRITE_64_P4_SIZE = sizeof(WRITE_64_P4) / sizeof(unsigned);

constexpr unsigned WRITE_64_P8[] = {
    // clang-format off
    llvm::AArch64::ST64B,
    llvm::AArch64::ST64BV,
    llvm::AArch64::ST64BV0,
    // clang-format on
};

constexpr size_t WRITE_64_P8_SIZE = sizeof(WRITE_64_P8) / sizeof(unsigned);

constexpr unsigned WRITE_128_P1[] = {
    // clang-format off
    llvm::AArch64::LDCLRP,
    llvm::AArch64::LDCLRPA,
    llvm::AArch64::LDCLRPAL,
    llvm::AArch64::LDCLRPL,
    llvm::AArch64::LDSETP,
    llvm::AArch64::LDSETPA,
    llvm::AArch64::LDSETPAL,
    llvm::AArch64::LDSETPL,
    llvm::AArch64::ST1Onev16b,
    llvm::AArch64::ST1Onev16b_POST,
    llvm::AArch64::ST1Onev2d,
    llvm::AArch64::ST1Onev2d_POST,
    llvm::AArch64::ST1Onev4s,
    llvm::AArch64::ST1Onev4s_POST,
    llvm::AArch64::ST1Onev8h,
    llvm::AArch64::ST1Onev8h_POST,
    llvm::AArch64::STLURqi,
    llvm::AArch64::STRQpost,
    llvm::AArch64::STRQpre,
    llvm::AArch64::STRQroW,
    llvm::AArch64::STRQroX,
    llvm::AArch64::STRQui,
    llvm::AArch64::STURQi,
    // clang-format on
};

constexpr size_t WRITE_128_P1_SIZE = sizeof(WRITE_128_P1) / sizeof(unsigned);

constexpr unsigned WRITE_128_P2[] = {
    // clang-format off
    llvm::AArch64::ST1Twov16b,
    llvm::AArch64::ST1Twov16b_POST,
    llvm::AArch64::ST1Twov2d,
    llvm::AArch64::ST1Twov2d_POST,
    llvm::AArch64::ST1Twov4s,
    llvm::AArch64::ST1Twov4s_POST,
    llvm::AArch64::ST1Twov8h,
    llvm::AArch64::ST1Twov8h_POST,
    llvm::AArch64::ST2Twov16b,
    llvm::AArch64::ST2Twov16b_POST,
    llvm::AArch64::ST2Twov2d,
    llvm::AArch64::ST2Twov2d_POST,
    llvm::AArch64::ST2Twov4s,
    llvm::AArch64::ST2Twov4s_POST,
    llvm::AArch64::ST2Twov8h,
    llvm::AArch64::ST2Twov8h_POST,
    llvm::AArch64::STNPQi,
    llvm::AArch64::STPQi,
    llvm::AArch64::STPQpost,
    llvm::AArch64::STPQpre,
    // clang-format on
};

constexpr size_t WRITE_128_P2_SIZE = sizeof(WRITE_128_P2) / sizeof(unsigned);

constexpr unsigned WRITE_128_P3[] = {
    // clang-format off
    llvm::AArch64::ST1Threev16b,
    llvm::AArch64::ST1Threev16b_POST,
    llvm::AArch64::ST1Threev2d,
    llvm::AArch64::ST1Threev2d_POST,
    llvm::AArch64::ST1Threev4s,
    llvm::AArch64::ST1Threev4s_POST,
    llvm::AArch64::ST1Threev8h,
    llvm::AArch64::ST1Threev8h_POST,
    llvm::AArch64::ST3Threev16b,
    llvm::AArch64::ST3Threev16b_POST,
    llvm::AArch64::ST3Threev2d,
    llvm::AArch64::ST3Threev2d_POST,
    llvm::AArch64::ST3Threev4s,
    llvm::AArch64::ST3Threev4s_POST,
    llvm::AArch64::ST3Threev8h,
    llvm::AArch64::ST3Threev8h_POST,
    // clang-format on
};

constexpr size_t WRITE_128_P3_SIZE = sizeof(WRITE_128_P3) / sizeof(unsigned);

constexpr unsigned WRITE_128_P4[] = {
    // clang-format off
    llvm::AArch64::ST1Fourv16b,
    llvm::AArch64::ST1Fourv16b_POST,
    llvm::AArch64::ST1Fourv2d,
    llvm::AArch64::ST1Fourv2d_POST,
    llvm::AArch64::ST1Fourv4s,
    llvm::AArch64::ST1Fourv4s_POST,
    llvm::AArch64::ST1Fourv8h,
    llvm::AArch64::ST1Fourv8h_POST,
    llvm::AArch64::ST4Fourv16b,
    llvm::AArch64::ST4Fourv16b_POST,
    llvm::AArch64::ST4Fourv2d,
    llvm::AArch64::ST4Fourv2d_POST,
    llvm::AArch64::ST4Fourv4s,
    llvm::AArch64::ST4Fourv4s_POST,
    llvm::AArch64::ST4Fourv8h,
    llvm::AArch64::ST4Fourv8h_POST,
    // clang-format on
};

constexpr size_t WRITE_128_P4_SIZE = sizeof(WRITE_128_P4) / sizeof(unsigned);

constexpr unsigned WRITE_DYN[] = {
    // clang-format off
    llvm::AArch64::CPYE,
    llvm::AArch64::CPYEN,
    llvm::AArch64::CPYERN,
    llvm::AArch64::CPYERT,
    llvm::AArch64::CPYERTN,
    llvm::AArch64::CPYERTRN,
    llvm::AArch64::CPYERTWN,
    llvm::AArch64::CPYET,
    llvm::AArch64::CPYETN,
    llvm::AArch64::CPYETRN,
    llvm::AArch64::CPYETWN,
    llvm::AArch64::CPYEWN,
    llvm::AArch64::CPYEWT,
    llvm::AArch64::CPYEWTN,
    llvm::AArch64::CPYEWTRN,
    llvm::AArch64::CPYEWTWN,
    llvm::AArch64::CPYFE,
    llvm::AArch64::CPYFEN,
    llvm::AArch64::CPYFERN,
    llvm::AArch64::CPYFERT,
    llvm::AArch64::CPYFERTN,
    llvm::AArch64::CPYFERTRN,
    llvm::AArch64::CPYFERTWN,
    llvm::AArch64::CPYFET,
    llvm::AArch64::CPYFETN,
    llvm::AArch64::CPYFETRN,
    llvm::AArch64::CPYFETWN,
    llvm::AArch64::CPYFEWN,
    llvm::AArch64::CPYFEWT,
    llvm::AArch64::CPYFEWTN,
    llvm::AArch64::CPYFEWTRN,
    llvm::AArch64::CPYFEWTWN,
    llvm::AArch64::CPYFM,
    llvm::AArch64::CPYFMN,
    llvm::AArch64::CPYFMRN,
    llvm::AArch64::CPYFMRT,
    llvm::AArch64::CPYFMRTN,
    llvm::AArch64::CPYFMRTRN,
    llvm::AArch64::CPYFMRTWN,
    llvm::AArch64::CPYFMT,
    llvm::AArch64::CPYFMTN,
    llvm::AArch64::CPYFMTRN,
    llvm::AArch64::CPYFMTWN,
    llvm::AArch64::CPYFMWN,
    llvm::AArch64::CPYFMWT,
    llvm::AArch64::CPYFMWTN,
    llvm::AArch64::CPYFMWTRN,
    llvm::AArch64::CPYFMWTWN,
    llvm::AArch64::CPYFP,
    llvm::AArch64::CPYFPN,
    llvm::AArch64::CPYFPRN,
    llvm::AArch64::CPYFPRT,
    llvm::AArch64::CPYFPRTN,
    llvm::AArch64::CPYFPRTRN,
    llvm::AArch64::CPYFPRTWN,
    llvm::AArch64::CPYFPT,
    llvm::AArch64::CPYFPTN,
    llvm::AArch64::CPYFPTRN,
    llvm::AArch64::CPYFPTWN,
    llvm::AArch64::CPYFPWN,
    llvm::AArch64::CPYFPWT,
    llvm::AArch64::CPYFPWTN,
    llvm::AArch64::CPYFPWTRN,
    llvm::AArch64::CPYFPWTWN,
    llvm::AArch64::CPYM,
    llvm::AArch64::CPYMN,
    llvm::AArch64::CPYMRN,
    llvm::AArch64::CPYMRT,
    llvm::AArch64::CPYMRTN,
    llvm::AArch64::CPYMRTRN,
    llvm::AArch64::CPYMRTWN,
    llvm::AArch64::CPYMT,
    llvm::AArch64::CPYMTN,
    llvm::AArch64::CPYMTRN,
    llvm::AArch64::CPYMTWN,
    llvm::AArch64::CPYMWN,
    llvm::AArch64::CPYMWT,
    llvm::AArch64::CPYMWTN,
    llvm::AArch64::CPYMWTRN,
    llvm::AArch64::CPYMWTWN,
    llvm::AArch64::CPYP,
    llvm::AArch64::CPYPN,
    llvm::AArch64::CPYPRN,
    llvm::AArch64::CPYPRT,
    llvm::AArch64::CPYPRTN,
    llvm::AArch64::CPYPRTRN,
    llvm::AArch64::CPYPRTWN,
    llvm::AArch64::CPYPT,
    llvm::AArch64::CPYPTN,
    llvm::AArch64::CPYPTRN,
    llvm::AArch64::CPYPTWN,
    llvm::AArch64::CPYPWN,
    llvm::AArch64::CPYPWT,
    llvm::AArch64::CPYPWTN,
    llvm::AArch64::CPYPWTRN,
    llvm::AArch64::CPYPWTWN,
    llvm::AArch64::MOPSSETGE,
    llvm::AArch64::MOPSSETGEN,
    llvm::AArch64::MOPSSETGET,
    llvm::AArch64::MOPSSETGETN,
    llvm::AArch64::SETE,
    llvm::AArch64::SETEN,
    llvm::AArch64::SETET,
    llvm::AArch64::SETETN,
    llvm::AArch64::SETGM,
    llvm::AArch64::SETGMN,
    llvm::AArch64::SETGMT,
    llvm::AArch64::SETGMTN,
    llvm::AArch64::SETGP,
    llvm::AArch64::SETGPN,
    llvm::AArch64::SETGPT,
    llvm::AArch64::SETGPTN,
    llvm::AArch64::SETM,
    llvm::AArch64::SETMN,
    llvm::AArch64::SETMT,
    llvm::AArch64::SETMTN,
    llvm::AArch64::SETP,
    llvm::AArch64::SETPN,
    llvm::AArch64::SETPT,
    llvm::AArch64::SETPTN,
    // clang-format on
};

constexpr size_t WRITE_DYN_SIZE = sizeof(WRITE_DYN) / sizeof(unsigned);

/* Highest 16 bits are the write access, lowest 16 bits are the read access.
 *
 * ------------------------------------------------------------
 * | 0x1f                  WRITE ACCESS                  0x10 |
 * ------------------------------------------------------------
 * | 1 bit dyn | 4 bits unused | 11 bits unsigned access size |
 * ------------------------------------------------------------
 *
 * ------------------------------------------------------------
 * | 0xf                   READ ACCESS                    0x0 |
 * ------------------------------------------------------------
 * | 1 bit dyn | 4 bits unused | 11 bits unsigned access size |
 * ------------------------------------------------------------
 */

constexpr uint32_t READ_DYN_FLAGS = 15;
constexpr uint32_t WRITE_POSITION = 16;
constexpr uint32_t WRITE_DYN_FLAGS = WRITE_POSITION + READ_DYN_FLAGS;
constexpr uint32_t READ(uint32_t s, uint32_t p) { return (s * p) & 0x3ff; }
constexpr uint32_t WRITE(uint32_t s, uint32_t p) {
  return ((s * p) & 0x3ff) << WRITE_POSITION;
}
constexpr uint32_t GET_READ_SIZE(uint32_t v) { return v & 0x3ff; }
constexpr uint32_t GET_WRITE_SIZE(uint32_t v) {
  return (v >> WRITE_POSITION) & 0x3ff;
}
constexpr uint32_t IS_READ_DYN(uint32_t v) { return (v >> READ_DYN_FLAGS) & 1; }
constexpr uint32_t IS_WRITE_DYN(uint32_t v) {
  return (v >> WRITE_DYN_FLAGS) & 1;
}

struct MemAccessArray {
  uint32_t arr[llvm::AArch64::INSTRUCTION_LIST_END] = {0};

  constexpr inline void _initMemAccessRead(const unsigned buff[],
                                           const size_t buffSize, uint32_t len,
                                           uint32_t pack) {
    for (size_t i = 0; i < buffSize; i++) {
      arr[buff[i]] |= READ(len, pack);
    }
  }

  constexpr inline void _initMemAccessWrite(const unsigned buff[],
                                            const size_t buffSize, uint32_t len,
                                            uint32_t pack) {
    for (size_t i = 0; i < buffSize; i++) {
      arr[buff[i]] |= WRITE(len, pack);
    }
  }

  constexpr inline void _initMemAccessFlags(const unsigned buff[],
                                            const size_t buffSize,
                                            uint32_t flags) {
    for (size_t i = 0; i < buffSize; i++) {
      arr[buff[i]] |= flags;
    }
  }

  constexpr MemAccessArray() {
    // read
    _initMemAccessRead(READ_8_P1, READ_8_P1_SIZE, 1, 1);
    _initMemAccessRead(READ_8_P2, READ_8_P2_SIZE, 1, 2);
    _initMemAccessRead(READ_8_P3, READ_8_P3_SIZE, 1, 3);
    _initMemAccessRead(READ_8_P4, READ_8_P4_SIZE, 1, 4);
    _initMemAccessRead(READ_16_P1, READ_16_P1_SIZE, 2, 1);
    _initMemAccessRead(READ_16_P2, READ_16_P2_SIZE, 2, 2);
    _initMemAccessRead(READ_16_P3, READ_16_P3_SIZE, 2, 3);
    _initMemAccessRead(READ_16_P4, READ_16_P4_SIZE, 2, 4);
    _initMemAccessRead(READ_32_P1, READ_32_P1_SIZE, 4, 1);
    _initMemAccessRead(READ_32_P2, READ_32_P2_SIZE, 4, 2);
    _initMemAccessRead(READ_32_P3, READ_32_P3_SIZE, 4, 3);
    _initMemAccessRead(READ_32_P4, READ_32_P4_SIZE, 4, 4);
    _initMemAccessRead(READ_64_P1, READ_64_P1_SIZE, 8, 1);
    _initMemAccessRead(READ_64_P2, READ_64_P2_SIZE, 8, 2);
    _initMemAccessRead(READ_64_P3, READ_64_P3_SIZE, 8, 3);
    _initMemAccessRead(READ_64_P4, READ_64_P4_SIZE, 8, 4);
    _initMemAccessRead(READ_64_P8, READ_64_P8_SIZE, 8, 8);
    _initMemAccessRead(READ_128_P1, READ_128_P1_SIZE, 16, 1);
    _initMemAccessRead(READ_128_P2, READ_128_P2_SIZE, 16, 2);
    _initMemAccessRead(READ_128_P3, READ_128_P3_SIZE, 16, 3);
    _initMemAccessRead(READ_128_P4, READ_128_P4_SIZE, 16, 4);
    // read dyn
    _initMemAccessFlags(READ_DYN, READ_DYN_SIZE, (1 << READ_DYN_FLAGS));
    // write
    _initMemAccessWrite(WRITE_8_P1, WRITE_8_P1_SIZE, 1, 1);
    _initMemAccessWrite(WRITE_8_P2, WRITE_8_P2_SIZE, 1, 2);
    _initMemAccessWrite(WRITE_8_P3, WRITE_8_P3_SIZE, 1, 3);
    _initMemAccessWrite(WRITE_8_P4, WRITE_8_P4_SIZE, 1, 4);
    _initMemAccessWrite(WRITE_16_P1, WRITE_16_P1_SIZE, 2, 1);
    _initMemAccessWrite(WRITE_16_P2, WRITE_16_P2_SIZE, 2, 2);
    _initMemAccessWrite(WRITE_16_P3, WRITE_16_P3_SIZE, 2, 3);
    _initMemAccessWrite(WRITE_16_P4, WRITE_16_P4_SIZE, 2, 4);
    _initMemAccessWrite(WRITE_32_P1, WRITE_32_P1_SIZE, 4, 1);
    _initMemAccessWrite(WRITE_32_P2, WRITE_32_P2_SIZE, 4, 2);
    _initMemAccessWrite(WRITE_32_P3, WRITE_32_P3_SIZE, 4, 3);
    _initMemAccessWrite(WRITE_32_P4, WRITE_32_P4_SIZE, 4, 4);
    _initMemAccessWrite(WRITE_64_P1, WRITE_64_P1_SIZE, 8, 1);
    _initMemAccessWrite(WRITE_64_P2, WRITE_64_P2_SIZE, 8, 2);
    _initMemAccessWrite(WRITE_64_P3, WRITE_64_P3_SIZE, 8, 3);
    _initMemAccessWrite(WRITE_64_P4, WRITE_64_P4_SIZE, 8, 4);
    _initMemAccessWrite(WRITE_64_P8, WRITE_64_P8_SIZE, 8, 8);
    _initMemAccessWrite(WRITE_128_P1, WRITE_128_P1_SIZE, 16, 1);
    _initMemAccessWrite(WRITE_128_P2, WRITE_128_P2_SIZE, 16, 2);
    _initMemAccessWrite(WRITE_128_P3, WRITE_128_P3_SIZE, 16, 3);
    _initMemAccessWrite(WRITE_128_P4, WRITE_128_P4_SIZE, 16, 4);
    // write dyn
    _initMemAccessFlags(WRITE_DYN, WRITE_DYN_SIZE, (1 << WRITE_DYN_FLAGS));
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
    check_table(READ_8_P1, READ_8_P1_SIZE, READ(1, 1), 0x7ff);
    check_table(READ_8_P2, READ_8_P2_SIZE, READ(1, 2), 0x7ff);
    check_table(READ_8_P3, READ_8_P3_SIZE, READ(1, 3), 0x7ff);
    check_table(READ_8_P4, READ_8_P4_SIZE, READ(1, 4), 0x7ff);
    check_table(READ_16_P1, READ_16_P1_SIZE, READ(2, 1), 0x7ff);
    check_table(READ_16_P2, READ_16_P2_SIZE, READ(2, 2), 0x7ff);
    check_table(READ_16_P3, READ_16_P3_SIZE, READ(2, 3), 0x7ff);
    check_table(READ_16_P4, READ_16_P4_SIZE, READ(2, 4), 0x7ff);
    check_table(READ_32_P1, READ_32_P1_SIZE, READ(4, 1), 0x7ff);
    check_table(READ_32_P2, READ_32_P2_SIZE, READ(4, 2), 0x7ff);
    check_table(READ_32_P3, READ_32_P3_SIZE, READ(4, 3), 0x7ff);
    check_table(READ_32_P4, READ_32_P4_SIZE, READ(4, 4), 0x7ff);
    check_table(READ_64_P1, READ_64_P1_SIZE, READ(8, 1), 0x7ff);
    check_table(READ_64_P2, READ_64_P2_SIZE, READ(8, 2), 0x7ff);
    check_table(READ_64_P3, READ_64_P3_SIZE, READ(8, 3), 0x7ff);
    check_table(READ_64_P4, READ_64_P4_SIZE, READ(8, 4), 0x7ff);
    check_table(READ_64_P8, READ_64_P8_SIZE, READ(8, 8), 0x7ff);
    check_table(READ_128_P1, READ_128_P1_SIZE, READ(16, 1), 0x7ff);
    check_table(READ_128_P2, READ_128_P2_SIZE, READ(16, 2), 0x7ff);
    check_table(READ_128_P3, READ_128_P3_SIZE, READ(16, 3), 0x7ff);
    check_table(READ_128_P4, READ_128_P4_SIZE, READ(16, 4), 0x7ff);
    check_table(READ_DYN, READ_DYN_SIZE, (1 << READ_DYN_FLAGS),
                (1 << READ_DYN_FLAGS));
    // write
    check_table(WRITE_8_P1, WRITE_8_P1_SIZE, WRITE(1, 1),
                0x7ff << WRITE_POSITION);
    check_table(WRITE_8_P2, WRITE_8_P2_SIZE, WRITE(1, 2),
                0x7ff << WRITE_POSITION);
    check_table(WRITE_8_P3, WRITE_8_P3_SIZE, WRITE(1, 3),
                0x7ff << WRITE_POSITION);
    check_table(WRITE_8_P4, WRITE_8_P4_SIZE, WRITE(1, 4),
                0x7ff << WRITE_POSITION);
    check_table(WRITE_16_P1, WRITE_16_P1_SIZE, WRITE(2, 1),
                0x7ff << WRITE_POSITION);
    check_table(WRITE_16_P2, WRITE_16_P2_SIZE, WRITE(2, 2),
                0x7ff << WRITE_POSITION);
    check_table(WRITE_16_P3, WRITE_16_P3_SIZE, WRITE(2, 3),
                0x7ff << WRITE_POSITION);
    check_table(WRITE_16_P4, WRITE_16_P4_SIZE, WRITE(2, 4),
                0x7ff << WRITE_POSITION);
    check_table(WRITE_32_P1, WRITE_32_P1_SIZE, WRITE(4, 1),
                0x7ff << WRITE_POSITION);
    check_table(WRITE_32_P2, WRITE_32_P2_SIZE, WRITE(4, 2),
                0x7ff << WRITE_POSITION);
    check_table(WRITE_32_P3, WRITE_32_P3_SIZE, WRITE(4, 3),
                0x7ff << WRITE_POSITION);
    check_table(WRITE_32_P4, WRITE_32_P4_SIZE, WRITE(4, 4),
                0x7ff << WRITE_POSITION);
    check_table(WRITE_64_P1, WRITE_64_P1_SIZE, WRITE(8, 1),
                0x7ff << WRITE_POSITION);
    check_table(WRITE_64_P2, WRITE_64_P2_SIZE, WRITE(8, 2),
                0x7ff << WRITE_POSITION);
    check_table(WRITE_64_P3, WRITE_64_P3_SIZE, WRITE(8, 3),
                0x7ff << WRITE_POSITION);
    check_table(WRITE_64_P4, WRITE_64_P4_SIZE, WRITE(8, 4),
                0x7ff << WRITE_POSITION);
    check_table(WRITE_64_P8, WRITE_64_P8_SIZE, WRITE(8, 8),
                0x7ff << WRITE_POSITION);
    check_table(WRITE_128_P1, WRITE_128_P1_SIZE, WRITE(16, 1),
                0x7ff << WRITE_POSITION);
    check_table(WRITE_128_P2, WRITE_128_P2_SIZE, WRITE(16, 2),
                0x7ff << WRITE_POSITION);
    check_table(WRITE_128_P3, WRITE_128_P3_SIZE, WRITE(16, 3),
                0x7ff << WRITE_POSITION);
    check_table(WRITE_128_P4, WRITE_128_P4_SIZE, WRITE(16, 4),
                0x7ff << WRITE_POSITION);
    check_table(WRITE_DYN, WRITE_DYN_SIZE, (1 << WRITE_DYN_FLAGS),
                (1 << WRITE_DYN_FLAGS));
    return 0;
  }
#endif

  inline uint32_t get(size_t op) const {
    if (op < llvm::AArch64::INSTRUCTION_LIST_END) {
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
  return GET_READ_SIZE(memAccessCache.get(inst.getOpcode()));
}

unsigned getWriteSize(const llvm::MCInst &inst, const LLVMCPU &llvmcpu) {
  return GET_WRITE_SIZE(memAccessCache.get(inst.getOpcode()));
}

unsigned getInstSize(const llvm::MCInst &inst, const LLVMCPU &llvmcpu) {
  return 4;
}

sword getFixedOperandValue(const llvm::MCInst &inst, const LLVMCPU &llvmcpu,
                           unsigned index, int64_t value) {
  return static_cast<rword>(value);
}

unsigned getImmediateSize(const llvm::MCInst &inst, const LLVMCPU &llvmcpu) {
  // TODO check llvm
  return 2;
}

bool unsupportedRead(const llvm::MCInst &inst) {
  return IS_READ_DYN(memAccessCache.get(inst.getOpcode()));
}

bool unsupportedWrite(const llvm::MCInst &inst) {
  return IS_WRITE_DYN(memAccessCache.get(inst.getOpcode()));
}

bool variadicOpsIsWrite(const llvm::MCInst &inst) { return false; }

bool isMOPSPrologue(const llvm::MCInst &inst) {

  switch (inst.getOpcode()) {
    default:
      return false;
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
    case llvm::AArch64::CPYPWTWN:
    case llvm::AArch64::SETGP:
    case llvm::AArch64::SETGPN:
    case llvm::AArch64::SETGPT:
    case llvm::AArch64::SETGPTN:
    case llvm::AArch64::SETP:
    case llvm::AArch64::SETPN:
    case llvm::AArch64::SETPT:
    case llvm::AArch64::SETPTN:
      return true;
  }
}

} // namespace QBDI
