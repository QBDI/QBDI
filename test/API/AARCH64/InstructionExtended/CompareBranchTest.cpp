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
