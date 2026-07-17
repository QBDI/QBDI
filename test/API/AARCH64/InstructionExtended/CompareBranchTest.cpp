/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2026 Quarkslab
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

#include <catch2/catch_test_macros.hpp>
#include "API/APITest.h"
#include "MemAccessTestUtils_AARCH64.h"

using QBDITestBatch2::checkFeature;

static void checkRegBranch(APITest &fixture, const char *mnemonic,
                           const char *source, QBDI::rword x0Taken,
                           QBDI::rword x0NotTaken, bool taken) {
  bool seen = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB(mnemonic, QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seen = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB(mnemonic, QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->x0 = taken ? x0Taken : x0NotTaken;
  fixture.vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkCmpBranch2Reg(APITest &fixture, const char *mnemonic,
                               const char *source, QBDI::rword x0,
                               QBDI::rword x1, bool taken) {
  bool seen = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB(mnemonic, QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seen = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB(mnemonic, QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->x0 = x0;
  state->x1 = x1;
  fixture.vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cbz_x_taken") {
  const char source[] =
      "cbz x0, cbz_landed\n"
      "mov x0, #0x1111\n"
      "b cbz_end\n"
      "cbz_landed:\n"
      "mov x0, #0x2222\n"
      "cbz_end:\n";
  checkRegBranch(*this, "CBZX", source, 0, 5, true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cbz_x_not_taken") {
  const char source[] =
      "cbz x0, cbz_landed\n"
      "mov x0, #0x1111\n"
      "b cbz_end\n"
      "cbz_landed:\n"
      "mov x0, #0x2222\n"
      "cbz_end:\n";
  checkRegBranch(*this, "CBZX", source, 0, 5, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cbz_w_taken") {
  const char source[] =
      "cbz w0, cbzw_landed\n"
      "mov x0, #0x1111\n"
      "b cbzw_end\n"
      "cbzw_landed:\n"
      "mov x0, #0x2222\n"
      "cbzw_end:\n";
  checkRegBranch(*this, "CBZW", source, 0xdeadbeef00000000ULL, 5, true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cbnz_x_taken") {
  const char source[] =
      "cbnz x0, cbnz_landed\n"
      "mov x0, #0x1111\n"
      "b cbnz_end\n"
      "cbnz_landed:\n"
      "mov x0, #0x2222\n"
      "cbnz_end:\n";
  checkRegBranch(*this, "CBNZX", source, 5, 0, true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cbnz_x_not_taken") {
  const char source[] =
      "cbnz x0, cbnz_landed\n"
      "mov x0, #0x1111\n"
      "b cbnz_end\n"
      "cbnz_landed:\n"
      "mov x0, #0x2222\n"
      "cbnz_end:\n";
  checkRegBranch(*this, "CBNZX", source, 5, 0, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cbnz_w_taken") {
  const char source[] =
      "cbnz w0, cbnzw_landed\n"
      "mov x0, #0x1111\n"
      "b cbnzw_end\n"
      "cbnzw_landed:\n"
      "mov x0, #0x2222\n"
      "cbnzw_end:\n";
  checkRegBranch(*this, "CBNZW", source, 5, 0, true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-tbz_taken") {
  const char source[] =
      "tbz x0, #35, tbz_landed\n"
      "mov x0, #0x1111\n"
      "b tbz_end\n"
      "tbz_landed:\n"
      "mov x0, #0x2222\n"
      "tbz_end:\n";
  checkRegBranch(*this, "TBZX", source, 0, (1ULL << 35), true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-tbz_not_taken") {
  const char source[] =
      "tbz x0, #35, tbz_landed\n"
      "mov x0, #0x1111\n"
      "b tbz_end\n"
      "tbz_landed:\n"
      "mov x0, #0x2222\n"
      "tbz_end:\n";
  checkRegBranch(*this, "TBZX", source, 0, (1ULL << 35), false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-tbz_w_taken") {
  const char source[] =
      "tbz w0, #3, tbzw_landed\n"
      "mov x0, #0x1111\n"
      "b tbzw_end\n"
      "tbzw_landed:\n"
      "mov x0, #0x2222\n"
      "tbzw_end:\n";
  checkRegBranch(*this, "TBZW", source, 0, (1ULL << 3), true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-tbnz_taken") {
  const char source[] =
      "tbnz x0, #35, tbnz_landed\n"
      "mov x0, #0x1111\n"
      "b tbnz_end\n"
      "tbnz_landed:\n"
      "mov x0, #0x2222\n"
      "tbnz_end:\n";
  checkRegBranch(*this, "TBNZX", source, (1ULL << 35), 0, true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-tbnz_not_taken") {
  const char source[] =
      "tbnz x0, #35, tbnz_landed\n"
      "mov x0, #0x1111\n"
      "b tbnz_end\n"
      "tbnz_landed:\n"
      "mov x0, #0x2222\n"
      "tbnz_end:\n";
  checkRegBranch(*this, "TBNZX", source, (1ULL << 35), 0, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-tbnz_w_taken") {
  const char source[] =
      "tbnz w0, #3, tbnzw_landed\n"
      "mov x0, #0x1111\n"
      "b tbnzw_end\n"
      "tbnzw_landed:\n"
      "mov x0, #0x2222\n"
      "tbnzw_end:\n";
  checkRegBranch(*this, "TBNZW", source, (1ULL << 3), 0, true);
}

#define CMPBR_RR_TEST(NAME, ASMCC, OPCODE, REG, X0, X1, TAKEN)         \
  TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-" NAME) { \
    if (!checkFeature("cmpbr")) {                                      \
      return;                                                          \
    }                                                                  \
    const char source[] = ASMCC "\t" REG "0, " REG                     \
                                "1, cmpbr_landed\n"                    \
                                "mov x0, #0x1111\n"                    \
                                "b cmpbr_end\n"                        \
                                "cmpbr_landed:\n"                      \
                                "mov x0, #0x2222\n"                    \
                                "cmpbr_end:\n";                        \
    checkCmpBranch2Reg(*this, OPCODE, source, X0, X1, TAKEN);          \
  }

#define CMPBR_RI_TEST(NAME, ASMCC, OPCODE, REG, IMM, X0, TAKEN)        \
  TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-" NAME) { \
    if (!checkFeature("cmpbr")) {                                      \
      return;                                                          \
    }                                                                  \
    const char source[] = ASMCC "\t" REG "0, #" IMM                    \
                                ", cmpbr_landed\n"                     \
                                "mov x0, #0x1111\n"                    \
                                "b cmpbr_end\n"                        \
                                "cmpbr_landed:\n"                      \
                                "mov x0, #0x2222\n"                    \
                                "cmpbr_end:\n";                        \
    checkRegBranch(*this, OPCODE, source, X0, X0, TAKEN);              \
  }

// register-register form, X width
CMPBR_RR_TEST("cbgt_x_rr_taken", "cbgt", "CBGTXrr", "x", 5, 0, true)
CMPBR_RR_TEST("cbgt_x_rr_not_taken", "cbgt", "CBGTXrr", "x", 0, 5, false)
CMPBR_RR_TEST("cbge_x_rr_taken", "cbge", "CBGEXrr", "x", 5, 5, true)
CMPBR_RR_TEST("cbge_x_rr_not_taken", "cbge", "CBGEXrr", "x", 0, 5, false)
CMPBR_RR_TEST("cbhi_x_rr_taken", "cbhi", "CBHIXrr", "x", 5, 0, true)
CMPBR_RR_TEST("cbhi_x_rr_not_taken", "cbhi", "CBHIXrr", "x", 0, 5, false)
CMPBR_RR_TEST("cbhs_x_rr_taken", "cbhs", "CBHSXrr", "x", 5, 5, true)
CMPBR_RR_TEST("cbhs_x_rr_not_taken", "cbhs", "CBHSXrr", "x", 0, 5, false)
CMPBR_RR_TEST("cbeq_x_rr_taken", "cbeq", "CBEQXrr", "x", 5, 5, true)
CMPBR_RR_TEST("cbeq_x_rr_not_taken", "cbeq", "CBEQXrr", "x", 5, 0, false)
CMPBR_RR_TEST("cbne_x_rr_taken", "cbne", "CBNEXrr", "x", 5, 0, true)
CMPBR_RR_TEST("cbne_x_rr_not_taken", "cbne", "CBNEXrr", "x", 5, 5, false)

// register-register form, W width
CMPBR_RR_TEST("cbgt_w_rr_taken", "cbgt", "CBGTWrr", "w", 5, 0, true)
CMPBR_RR_TEST("cbgt_w_rr_not_taken", "cbgt", "CBGTWrr", "w", 0, 5, false)
CMPBR_RR_TEST("cbge_w_rr_taken", "cbge", "CBGEWrr", "w", 5, 5, true)
CMPBR_RR_TEST("cbge_w_rr_not_taken", "cbge", "CBGEWrr", "w", 0, 5, false)
CMPBR_RR_TEST("cbhi_w_rr_taken", "cbhi", "CBHIWrr", "w", 5, 0, true)
CMPBR_RR_TEST("cbhi_w_rr_not_taken", "cbhi", "CBHIWrr", "w", 0, 5, false)
CMPBR_RR_TEST("cbhs_w_rr_taken", "cbhs", "CBHSWrr", "w", 5, 5, true)
CMPBR_RR_TEST("cbhs_w_rr_not_taken", "cbhs", "CBHSWrr", "w", 0, 5, false)
CMPBR_RR_TEST("cbeq_w_rr_taken", "cbeq", "CBEQWrr", "w", 5, 5, true)
CMPBR_RR_TEST("cbeq_w_rr_not_taken", "cbeq", "CBEQWrr", "w", 5, 0, false)
CMPBR_RR_TEST("cbne_w_rr_taken", "cbne", "CBNEWrr", "w", 5, 0, true)
CMPBR_RR_TEST("cbne_w_rr_not_taken", "cbne", "CBNEWrr", "w", 5, 5, false)

// register-immediate form, X width (imm = 3)
CMPBR_RI_TEST("cbgt_x_ri_taken", "cbgt", "CBGTXri", "x", "3", 5, true)
CMPBR_RI_TEST("cbgt_x_ri_not_taken", "cbgt", "CBGTXri", "x", "3", 1, false)
CMPBR_RI_TEST("cblt_x_ri_taken", "cblt", "CBLTXri", "x", "3", 1, true)
CMPBR_RI_TEST("cblt_x_ri_not_taken", "cblt", "CBLTXri", "x", "3", 5, false)
CMPBR_RI_TEST("cbhi_x_ri_taken", "cbhi", "CBHIXri", "x", "3", 5, true)
CMPBR_RI_TEST("cbhi_x_ri_not_taken", "cbhi", "CBHIXri", "x", "3", 1, false)
CMPBR_RI_TEST("cblo_x_ri_taken", "cblo", "CBLOXri", "x", "3", 1, true)
CMPBR_RI_TEST("cblo_x_ri_not_taken", "cblo", "CBLOXri", "x", "3", 5, false)
CMPBR_RI_TEST("cbeq_x_ri_taken", "cbeq", "CBEQXri", "x", "3", 3, true)
CMPBR_RI_TEST("cbeq_x_ri_not_taken", "cbeq", "CBEQXri", "x", "3", 1, false)
CMPBR_RI_TEST("cbne_x_ri_taken", "cbne", "CBNEXri", "x", "3", 1, true)
CMPBR_RI_TEST("cbne_x_ri_not_taken", "cbne", "CBNEXri", "x", "3", 3, false)

// register-immediate form, W width (imm = 3)
CMPBR_RI_TEST("cbgt_w_ri_taken", "cbgt", "CBGTWri", "w", "3", 5, true)
CMPBR_RI_TEST("cbgt_w_ri_not_taken", "cbgt", "CBGTWri", "w", "3", 1, false)
CMPBR_RI_TEST("cblt_w_ri_taken", "cblt", "CBLTWri", "w", "3", 1, true)
CMPBR_RI_TEST("cblt_w_ri_not_taken", "cblt", "CBLTWri", "w", "3", 5, false)
CMPBR_RI_TEST("cbhi_w_ri_taken", "cbhi", "CBHIWri", "w", "3", 5, true)
CMPBR_RI_TEST("cbhi_w_ri_not_taken", "cbhi", "CBHIWri", "w", "3", 1, false)
CMPBR_RI_TEST("cblo_w_ri_taken", "cblo", "CBLOWri", "w", "3", 1, true)
CMPBR_RI_TEST("cblo_w_ri_not_taken", "cblo", "CBLOWri", "w", "3", 5, false)
CMPBR_RI_TEST("cbeq_w_ri_taken", "cbeq", "CBEQWri", "w", "3", 3, true)
CMPBR_RI_TEST("cbeq_w_ri_not_taken", "cbeq", "CBEQWri", "w", "3", 1, false)
CMPBR_RI_TEST("cbne_w_ri_taken", "cbne", "CBNEWri", "w", "3", 1, true)
CMPBR_RI_TEST("cbne_w_ri_not_taken", "cbne", "CBNEWri", "w", "3", 3, false)

// narrow byte compare, register-register, W-only
CMPBR_RR_TEST("cbbgt_w_taken", "cbbgt", "CBBGTWrr", "w", 5, 0, true)
CMPBR_RR_TEST("cbbgt_w_not_taken", "cbbgt", "CBBGTWrr", "w", 0, 5, false)
CMPBR_RR_TEST("cbbge_w_taken", "cbbge", "CBBGEWrr", "w", 5, 5, true)
CMPBR_RR_TEST("cbbge_w_not_taken", "cbbge", "CBBGEWrr", "w", 0, 5, false)
CMPBR_RR_TEST("cbbhi_w_taken", "cbbhi", "CBBHIWrr", "w", 5, 0, true)
CMPBR_RR_TEST("cbbhi_w_not_taken", "cbbhi", "CBBHIWrr", "w", 0, 5, false)
CMPBR_RR_TEST("cbbhs_w_taken", "cbbhs", "CBBHSWrr", "w", 5, 5, true)
CMPBR_RR_TEST("cbbhs_w_not_taken", "cbbhs", "CBBHSWrr", "w", 0, 5, false)
CMPBR_RR_TEST("cbbeq_w_taken", "cbbeq", "CBBEQWrr", "w", 5, 5, true)
CMPBR_RR_TEST("cbbeq_w_not_taken", "cbbeq", "CBBEQWrr", "w", 5, 0, false)
CMPBR_RR_TEST("cbbne_w_taken", "cbbne", "CBBNEWrr", "w", 5, 0, true)
CMPBR_RR_TEST("cbbne_w_not_taken", "cbbne", "CBBNEWrr", "w", 5, 5, false)

// narrow halfword compare, register-register, W-only
CMPBR_RR_TEST("cbhgt_w_taken", "cbhgt", "CBHGTWrr", "w", 5, 0, true)
CMPBR_RR_TEST("cbhgt_w_not_taken", "cbhgt", "CBHGTWrr", "w", 0, 5, false)
CMPBR_RR_TEST("cbhge_w_taken", "cbhge", "CBHGEWrr", "w", 5, 5, true)
CMPBR_RR_TEST("cbhge_w_not_taken", "cbhge", "CBHGEWrr", "w", 0, 5, false)
CMPBR_RR_TEST("cbhhi_w_taken", "cbhhi", "CBHHIWrr", "w", 5, 0, true)
CMPBR_RR_TEST("cbhhi_w_not_taken", "cbhhi", "CBHHIWrr", "w", 0, 5, false)
CMPBR_RR_TEST("cbhhs_w_taken", "cbhhs", "CBHHSWrr", "w", 5, 5, true)
CMPBR_RR_TEST("cbhhs_w_not_taken", "cbhhs", "CBHHSWrr", "w", 0, 5, false)
CMPBR_RR_TEST("cbheq_w_taken", "cbheq", "CBHEQWrr", "w", 5, 5, true)
CMPBR_RR_TEST("cbheq_w_not_taken", "cbheq", "CBHEQWrr", "w", 5, 0, false)
CMPBR_RR_TEST("cbhne_w_taken", "cbhne", "CBHNEWrr", "w", 5, 0, true)
CMPBR_RR_TEST("cbhne_w_not_taken", "cbhne", "CBHNEWrr", "w", 5, 5, false)

#undef CMPBR_RR_TEST
#undef CMPBR_RI_TEST
