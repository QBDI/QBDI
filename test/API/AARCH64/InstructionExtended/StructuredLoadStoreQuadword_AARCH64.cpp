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
#include "MemAccessTestUtils_AARCH64.h"

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::checkFeature;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1onev16b") {
  const char source[] = "ld1 {v0.16b}, [x0]\n";

  uint8_t buf[16] = {21,  45,  69, 93, 117, 141, 165, 189,
                     213, 237, 5,  29, 53,  77,  101, 125};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0xbda58d755d452d15ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x7d654d351d05edd5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xbda58d755d452d15ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x7d654d351d05edd5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Onev16b", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Onev16b", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Onev16b", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x7d654d351d05edd5ULL << 64 |
                            0xbda58d755d452d15ULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1onev16b_post") {
  const char source[] = "ld1 {v0.16b}, [x0], #16\n";

  uint8_t buf[16] = {21,  45,  69, 93, 117, 141, 165, 189,
                     213, 237, 5,  29, 53,  77,  101, 125};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0xbda58d755d452d15ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x7d654d351d05edd5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xbda58d755d452d15ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x7d654d351d05edd5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Onev16b_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Onev16b_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD1Onev16b_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x7d654d351d05edd5ULL << 64 |
                            0xbda58d755d452d15ULL));
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 16);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1twov16b") {
  const char source[] = "ld1 {v0.16b, v1.16b}, [x0]\n";

  uint8_t buf[32] = {24,  51,  78,  105, 132, 159, 186, 213, 240, 11, 38,
                     65,  92,  119, 146, 173, 200, 227, 254, 25,  52, 79,
                     106, 133, 160, 187, 214, 241, 12,  39,  66,  93};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0xd5ba9f84694e3318ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xad92775c41260bf0ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x856a4f3419fee3c8ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x5d42270cf1d6bba0ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xd5ba9f84694e3318ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xad92775c41260bf0ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x856a4f3419fee3c8ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x5d42270cf1d6bba0ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Twov16b", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Twov16b", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Twov16b", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0xad92775c41260bf0ULL << 64 |
                            0xd5ba9f84694e3318ULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x5d42270cf1d6bba0ULL << 64 |
                            0x856a4f3419fee3c8ULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1twov16b_post") {
  const char source[] = "ld1 {v0.16b, v1.16b}, [x0], #32\n";

  uint8_t buf[32] = {24,  51,  78,  105, 132, 159, 186, 213, 240, 11, 38,
                     65,  92,  119, 146, 173, 200, 227, 254, 25,  52, 79,
                     106, 133, 160, 187, 214, 241, 12,  39,  66,  93};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0xd5ba9f84694e3318ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xad92775c41260bf0ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x856a4f3419fee3c8ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x5d42270cf1d6bba0ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xd5ba9f84694e3318ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xad92775c41260bf0ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x856a4f3419fee3c8ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x5d42270cf1d6bba0ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Twov16b_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Twov16b_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD1Twov16b_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0xad92775c41260bf0ULL << 64 |
                            0xd5ba9f84694e3318ULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x5d42270cf1d6bba0ULL << 64 |
                            0x856a4f3419fee3c8ULL));
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 32);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1threev16b") {
  const char source[] = "ld1 {v0.16b, v1.16b, v2.16b}, [x0]\n";

  uint8_t buf[48] = {27,  57,  87,  117, 147, 177, 207, 237, 11,  41,
                     71,  101, 131, 161, 191, 221, 251, 25,  55,  85,
                     115, 145, 175, 205, 235, 9,   39,  69,  99,  129,
                     159, 189, 219, 249, 23,  53,  83,  113, 143, 173,
                     203, 233, 7,   37,  67,  97,  127, 157};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0xedcfb1937557391bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xddbfa1836547290bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xcdaf9173553719fbULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xbd9f8163452709ebULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0xad8f71533517f9dbULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x9d7f61432507e9cbULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xedcfb1937557391bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xddbfa1836547290bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xcdaf9173553719fbULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xbd9f8163452709ebULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0xad8f71533517f9dbULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x9d7f61432507e9cbULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Threev16b", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Threev16b", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Threev16b", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0xddbfa1836547290bULL << 64 |
                            0xedcfb1937557391bULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0xbd9f8163452709ebULL << 64 |
                            0xcdaf9173553719fbULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x9d7f61432507e9cbULL << 64 |
                            0xad8f71533517f9dbULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1threev16b_post") {
  const char source[] = "ld1 {v0.16b, v1.16b, v2.16b}, [x0], #48\n";

  uint8_t buf[48] = {27,  57,  87,  117, 147, 177, 207, 237, 11,  41,
                     71,  101, 131, 161, 191, 221, 251, 25,  55,  85,
                     115, 145, 175, 205, 235, 9,   39,  69,  99,  129,
                     159, 189, 219, 249, 23,  53,  83,  113, 143, 173,
                     203, 233, 7,   37,  67,  97,  127, 157};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0xedcfb1937557391bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xddbfa1836547290bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xcdaf9173553719fbULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xbd9f8163452709ebULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0xad8f71533517f9dbULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x9d7f61432507e9cbULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xedcfb1937557391bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xddbfa1836547290bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xcdaf9173553719fbULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xbd9f8163452709ebULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0xad8f71533517f9dbULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x9d7f61432507e9cbULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Threev16b_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("LD1Threev16b_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD1Threev16b_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0xddbfa1836547290bULL << 64 |
                            0xedcfb1937557391bULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0xbd9f8163452709ebULL << 64 |
                            0xcdaf9173553719fbULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x9d7f61432507e9cbULL << 64 |
                            0xad8f71533517f9dbULL));
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 48);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1fourv16b_post") {
  const char source[] = "ld1 {v0.16b, v1.16b, v2.16b, v3.16b}, [x0], #64\n";

  uint8_t buf[64] = {30,  63,  96,  129, 162, 195, 228, 5,   38,  71,  104,
                     137, 170, 203, 236, 13,  46,  79,  112, 145, 178, 211,
                     244, 21,  54,  87,  120, 153, 186, 219, 252, 29,  62,
                     95,  128, 161, 194, 227, 4,   37,  70,  103, 136, 169,
                     202, 235, 12,  45,  78,  111, 144, 177, 210, 243, 20,
                     53,  86,  119, 152, 185, 218, 251, 28,  61};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x05e4c3a281603f1eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x0deccbaa89684726ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x15f4d3b291704f2eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x1dfcdbba99785736ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x2504e3c2a1805f3eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x2d0cebcaa9886746ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x3514f3d2b1906f4eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x3d1cfbdab9987756ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x05e4c3a281603f1eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x0deccbaa89684726ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x15f4d3b291704f2eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x1dfcdbba99785736ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x2504e3c2a1805f3eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x2d0cebcaa9886746ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x3514f3d2b1906f4eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x3d1cfbdab9987756ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Fourv16b_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("LD1Fourv16b_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD1Fourv16b_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x0deccbaa89684726ULL << 64 |
                            0x05e4c3a281603f1eULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x1dfcdbba99785736ULL << 64 |
                            0x15f4d3b291704f2eULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x2d0cebcaa9886746ULL << 64 |
                            0x2504e3c2a1805f3eULL));
                     CHECK((__uint128_t)fprState->v3 ==
                           ((__uint128_t)0x3d1cfbdab9987756ULL << 64 |
                            0x3514f3d2b1906f4eULL));
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 64);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1onev8h") {
  const char source[] = "ld1 {v0.8h}, [x0]\n";

  uint8_t buf[16] = {22,  47,  72, 97, 122, 147, 172, 197,
                     222, 247, 16, 41, 66,  91,  116, 141};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0xc5ac937a61482f16ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x8d745b422910f7deULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xc5ac937a61482f16ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x8d745b422910f7deULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Onev8h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Onev8h", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Onev8h", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x8d745b422910f7deULL << 64 |
                            0xc5ac937a61482f16ULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1onev8h_post") {
  const char source[] = "ld1 {v0.8h}, [x0], #16\n";

  uint8_t buf[16] = {22,  47,  72, 97, 122, 147, 172, 197,
                     222, 247, 16, 41, 66,  91,  116, 141};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0xc5ac937a61482f16ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x8d745b422910f7deULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xc5ac937a61482f16ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x8d745b422910f7deULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Onev8h_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Onev8h_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD1Onev8h_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x8d745b422910f7deULL << 64 |
                            0xc5ac937a61482f16ULL));
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 16);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1twov8h") {
  const char source[] = "ld1 {v0.8h, v1.8h}, [x0]\n";

  uint8_t buf[32] = {25,  53,  81,  109, 137, 165, 193, 221, 249, 21, 49,
                     77,  105, 133, 161, 189, 217, 245, 17,  45,  73, 101,
                     129, 157, 185, 213, 241, 13,  41,  69,  97,  125};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0xddc1a5896d513519ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xbda185694d3115f9ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x9d8165492d11f5d9ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x7d6145290df1d5b9ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xddc1a5896d513519ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xbda185694d3115f9ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x9d8165492d11f5d9ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x7d6145290df1d5b9ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Twov8h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Twov8h", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Twov8h", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0xbda185694d3115f9ULL << 64 |
                            0xddc1a5896d513519ULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x7d6145290df1d5b9ULL << 64 |
                            0x9d8165492d11f5d9ULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1twov8h_post") {
  const char source[] = "ld1 {v0.8h, v1.8h}, [x0], #32\n";

  uint8_t buf[32] = {25,  53,  81,  109, 137, 165, 193, 221, 249, 21, 49,
                     77,  105, 133, 161, 189, 217, 245, 17,  45,  73, 101,
                     129, 157, 185, 213, 241, 13,  41,  69,  97,  125};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0xddc1a5896d513519ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xbda185694d3115f9ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x9d8165492d11f5d9ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x7d6145290df1d5b9ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xddc1a5896d513519ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xbda185694d3115f9ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x9d8165492d11f5d9ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x7d6145290df1d5b9ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Twov8h_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Twov8h_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD1Twov8h_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0xbda185694d3115f9ULL << 64 |
                            0xddc1a5896d513519ULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x7d6145290df1d5b9ULL << 64 |
                            0x9d8165492d11f5d9ULL));
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 32);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1threev8h") {
  const char source[] = "ld1 {v0.8h, v1.8h, v2.8h}, [x0]\n";

  uint8_t buf[48] = {28,  59,  90,  121, 152, 183, 214, 245, 20,  51,
                     82,  113, 144, 175, 206, 237, 12,  43,  74,  105,
                     136, 167, 198, 229, 4,   35,  66,  97,  128, 159,
                     190, 221, 252, 27,  58,  89,  120, 151, 182, 213,
                     244, 19,  50,  81,  112, 143, 174, 205};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0xf5d6b798795a3b1cULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xedceaf9071523314ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xe5c6a788694a2b0cULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xddbe9f8061422304ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0xd5b69778593a1bfcULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xcdae8f70513213f4ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xf5d6b798795a3b1cULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xedceaf9071523314ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xe5c6a788694a2b0cULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xddbe9f8061422304ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0xd5b69778593a1bfcULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xcdae8f70513213f4ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Threev8h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Threev8h", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Threev8h", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0xedceaf9071523314ULL << 64 |
                            0xf5d6b798795a3b1cULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0xddbe9f8061422304ULL << 64 |
                            0xe5c6a788694a2b0cULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0xcdae8f70513213f4ULL << 64 |
                            0xd5b69778593a1bfcULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1threev8h_post") {
  const char source[] = "ld1 {v0.8h, v1.8h, v2.8h}, [x0], #48\n";

  uint8_t buf[48] = {28,  59,  90,  121, 152, 183, 214, 245, 20,  51,
                     82,  113, 144, 175, 206, 237, 12,  43,  74,  105,
                     136, 167, 198, 229, 4,   35,  66,  97,  128, 159,
                     190, 221, 252, 27,  58,  89,  120, 151, 182, 213,
                     244, 19,  50,  81,  112, 143, 174, 205};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0xf5d6b798795a3b1cULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xedceaf9071523314ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xe5c6a788694a2b0cULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xddbe9f8061422304ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0xd5b69778593a1bfcULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xcdae8f70513213f4ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xf5d6b798795a3b1cULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xedceaf9071523314ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xe5c6a788694a2b0cULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xddbe9f8061422304ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0xd5b69778593a1bfcULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xcdae8f70513213f4ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Threev8h_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("LD1Threev8h_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD1Threev8h_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0xedceaf9071523314ULL << 64 |
                            0xf5d6b798795a3b1cULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0xddbe9f8061422304ULL << 64 |
                            0xe5c6a788694a2b0cULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0xcdae8f70513213f4ULL << 64 |
                            0xd5b69778593a1bfcULL));
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 48);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1fourv8h") {
  const char source[] = "ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x0]\n";

  uint8_t buf[64] = {31,  65,  99,  133, 167, 201, 235, 13,  47,  81,  115,
                     149, 183, 217, 251, 29,  63,  97,  131, 165, 199, 233,
                     11,  45,  79,  113, 147, 181, 215, 249, 27,  61,  95,
                     129, 163, 197, 231, 9,   43,  77,  111, 145, 179, 213,
                     247, 25,  59,  93,  127, 161, 195, 229, 7,   41,  75,
                     109, 143, 177, 211, 245, 23,  57,  91,  125};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x0debc9a78563411fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1dfbd9b79573512fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x2d0be9c7a583613fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x3d1bf9d7b593714fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x4d2b09e7c5a3815fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x5d3b19f7d5b3916fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x6d4b2907e5c3a17fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x7d5b3917f5d3b18fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x0debc9a78563411fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1dfbd9b79573512fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x2d0be9c7a583613fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x3d1bf9d7b593714fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x4d2b09e7c5a3815fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x5d3b19f7d5b3916fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x6d4b2907e5c3a17fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x7d5b3917f5d3b18fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Fourv8h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Fourv8h", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Fourv8h", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x1dfbd9b79573512fULL << 64 |
                            0x0debc9a78563411fULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x3d1bf9d7b593714fULL << 64 |
                            0x2d0be9c7a583613fULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x5d3b19f7d5b3916fULL << 64 |
                            0x4d2b09e7c5a3815fULL));
                     CHECK((__uint128_t)fprState->v3 ==
                           ((__uint128_t)0x7d5b3917f5d3b18fULL << 64 |
                            0x6d4b2907e5c3a17fULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1fourv8h_post") {
  const char source[] = "ld1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x0], #64\n";

  uint8_t buf[64] = {31,  65,  99,  133, 167, 201, 235, 13,  47,  81,  115,
                     149, 183, 217, 251, 29,  63,  97,  131, 165, 199, 233,
                     11,  45,  79,  113, 147, 181, 215, 249, 27,  61,  95,
                     129, 163, 197, 231, 9,   43,  77,  111, 145, 179, 213,
                     247, 25,  59,  93,  127, 161, 195, 229, 7,   41,  75,
                     109, 143, 177, 211, 245, 23,  57,  91,  125};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x0debc9a78563411fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1dfbd9b79573512fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x2d0be9c7a583613fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x3d1bf9d7b593714fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x4d2b09e7c5a3815fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x5d3b19f7d5b3916fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x6d4b2907e5c3a17fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x7d5b3917f5d3b18fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x0debc9a78563411fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1dfbd9b79573512fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x2d0be9c7a583613fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x3d1bf9d7b593714fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x4d2b09e7c5a3815fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x5d3b19f7d5b3916fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x6d4b2907e5c3a17fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x7d5b3917f5d3b18fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Fourv8h_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Fourv8h_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD1Fourv8h_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x1dfbd9b79573512fULL << 64 |
                            0x0debc9a78563411fULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x3d1bf9d7b593714fULL << 64 |
                            0x2d0be9c7a583613fULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x5d3b19f7d5b3916fULL << 64 |
                            0x4d2b09e7c5a3815fULL));
                     CHECK((__uint128_t)fprState->v3 ==
                           ((__uint128_t)0x7d5b3917f5d3b18fULL << 64 |
                            0x6d4b2907e5c3a17fULL));
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 64);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1onev4s") {
  const char source[] = "ld1 {v0.4s}, [x0]\n";

  uint8_t buf[16] = {24,  51, 78, 105, 132, 159, 186, 213,
                     240, 11, 38, 65,  92,  119, 146, 173};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0xd5ba9f84694e3318ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xad92775c41260bf0ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xd5ba9f84694e3318ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xad92775c41260bf0ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Onev4s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Onev4s", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Onev4s", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0xad92775c41260bf0ULL << 64 |
                            0xd5ba9f84694e3318ULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1onev4s_post") {
  const char source[] = "ld1 {v0.4s}, [x0], #16\n";

  uint8_t buf[16] = {24,  51, 78, 105, 132, 159, 186, 213,
                     240, 11, 38, 65,  92,  119, 146, 173};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0xd5ba9f84694e3318ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xad92775c41260bf0ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xd5ba9f84694e3318ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xad92775c41260bf0ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Onev4s_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Onev4s_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD1Onev4s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0xad92775c41260bf0ULL << 64 |
                            0xd5ba9f84694e3318ULL));
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 16);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1twov4s") {
  const char source[] = "ld1 {v0.4s, v1.4s}, [x0]\n";

  uint8_t buf[32] = {27,  57,  87,  117, 147, 177, 207, 237, 11,  41,  71,
                     101, 131, 161, 191, 221, 251, 25,  55,  85,  115, 145,
                     175, 205, 235, 9,   39,  69,  99,  129, 159, 189};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0xedcfb1937557391bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xddbfa1836547290bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xcdaf9173553719fbULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xbd9f8163452709ebULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xedcfb1937557391bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xddbfa1836547290bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xcdaf9173553719fbULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xbd9f8163452709ebULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Twov4s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Twov4s", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Twov4s", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0xddbfa1836547290bULL << 64 |
                            0xedcfb1937557391bULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0xbd9f8163452709ebULL << 64 |
                            0xcdaf9173553719fbULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1twov4s_post") {
  const char source[] = "ld1 {v0.4s, v1.4s}, [x0], #32\n";

  uint8_t buf[32] = {27,  57,  87,  117, 147, 177, 207, 237, 11,  41,  71,
                     101, 131, 161, 191, 221, 251, 25,  55,  85,  115, 145,
                     175, 205, 235, 9,   39,  69,  99,  129, 159, 189};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0xedcfb1937557391bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xddbfa1836547290bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xcdaf9173553719fbULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xbd9f8163452709ebULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xedcfb1937557391bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xddbfa1836547290bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xcdaf9173553719fbULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xbd9f8163452709ebULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Twov4s_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Twov4s_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD1Twov4s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0xddbfa1836547290bULL << 64 |
                            0xedcfb1937557391bULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0xbd9f8163452709ebULL << 64 |
                            0xcdaf9173553719fbULL));
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 32);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1threev4s") {
  const char source[] = "ld1 {v0.4s, v1.4s, v2.4s}, [x0]\n";

  uint8_t buf[48] = {30,  63,  96,  129, 162, 195, 228, 5,   38,  71,  104, 137,
                     170, 203, 236, 13,  46,  79,  112, 145, 178, 211, 244, 21,
                     54,  87,  120, 153, 186, 219, 252, 29,  62,  95,  128, 161,
                     194, 227, 4,   37,  70,  103, 136, 169, 202, 235, 12,  45};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x05e4c3a281603f1eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x0deccbaa89684726ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x15f4d3b291704f2eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x1dfcdbba99785736ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x2504e3c2a1805f3eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x2d0cebcaa9886746ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x05e4c3a281603f1eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x0deccbaa89684726ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x15f4d3b291704f2eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x1dfcdbba99785736ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x2504e3c2a1805f3eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x2d0cebcaa9886746ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Threev4s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Threev4s", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Threev4s", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x0deccbaa89684726ULL << 64 |
                            0x05e4c3a281603f1eULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x1dfcdbba99785736ULL << 64 |
                            0x15f4d3b291704f2eULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x2d0cebcaa9886746ULL << 64 |
                            0x2504e3c2a1805f3eULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1threev4s_post") {
  const char source[] = "ld1 {v0.4s, v1.4s, v2.4s}, [x0], #48\n";

  uint8_t buf[48] = {30,  63,  96,  129, 162, 195, 228, 5,   38,  71,  104, 137,
                     170, 203, 236, 13,  46,  79,  112, 145, 178, 211, 244, 21,
                     54,  87,  120, 153, 186, 219, 252, 29,  62,  95,  128, 161,
                     194, 227, 4,   37,  70,  103, 136, 169, 202, 235, 12,  45};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x05e4c3a281603f1eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x0deccbaa89684726ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x15f4d3b291704f2eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x1dfcdbba99785736ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x2504e3c2a1805f3eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x2d0cebcaa9886746ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x05e4c3a281603f1eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x0deccbaa89684726ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x15f4d3b291704f2eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x1dfcdbba99785736ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x2504e3c2a1805f3eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x2d0cebcaa9886746ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Threev4s_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("LD1Threev4s_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD1Threev4s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x0deccbaa89684726ULL << 64 |
                            0x05e4c3a281603f1eULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x1dfcdbba99785736ULL << 64 |
                            0x15f4d3b291704f2eULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x2d0cebcaa9886746ULL << 64 |
                            0x2504e3c2a1805f3eULL));
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 48);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1fourv4s") {
  const char source[] = "ld1 {v0.4s, v1.4s, v2.4s, v3.4s}, [x0]\n";

  uint8_t buf[64] = {33,  69,  105, 141, 177, 213, 249, 29,  65,  101, 137,
                     173, 209, 245, 25,  61,  97,  133, 169, 205, 241, 21,
                     57,  93,  129, 165, 201, 237, 17,  53,  89,  125, 161,
                     197, 233, 13,  49,  85,  121, 157, 193, 229, 9,   45,
                     81,  117, 153, 189, 225, 5,   41,  77,  113, 149, 185,
                     221, 1,   37,  73,  109, 145, 181, 217, 253};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x1df9d5b18d694521ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x3d19f5d1ad896541ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x5d3915f1cda98561ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x7d593511edc9a581ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x9d7955310de9c5a1ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xbd9975512d09e5c1ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0xddb995714d2905e1ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0xfdd9b5916d492501ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x1df9d5b18d694521ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x3d19f5d1ad896541ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x5d3915f1cda98561ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x7d593511edc9a581ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x9d7955310de9c5a1ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xbd9975512d09e5c1ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0xddb995714d2905e1ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0xfdd9b5916d492501ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Fourv4s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Fourv4s", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Fourv4s", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x3d19f5d1ad896541ULL << 64 |
                            0x1df9d5b18d694521ULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x7d593511edc9a581ULL << 64 |
                            0x5d3915f1cda98561ULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0xbd9975512d09e5c1ULL << 64 |
                            0x9d7955310de9c5a1ULL));
                     CHECK((__uint128_t)fprState->v3 ==
                           ((__uint128_t)0xfdd9b5916d492501ULL << 64 |
                            0xddb995714d2905e1ULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1fourv4s_post") {
  const char source[] = "ld1 {v0.4s, v1.4s, v2.4s, v3.4s}, [x0], #64\n";

  uint8_t buf[64] = {33,  69,  105, 141, 177, 213, 249, 29,  65,  101, 137,
                     173, 209, 245, 25,  61,  97,  133, 169, 205, 241, 21,
                     57,  93,  129, 165, 201, 237, 17,  53,  89,  125, 161,
                     197, 233, 13,  49,  85,  121, 157, 193, 229, 9,   45,
                     81,  117, 153, 189, 225, 5,   41,  77,  113, 149, 185,
                     221, 1,   37,  73,  109, 145, 181, 217, 253};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x1df9d5b18d694521ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x3d19f5d1ad896541ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x5d3915f1cda98561ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x7d593511edc9a581ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x9d7955310de9c5a1ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xbd9975512d09e5c1ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0xddb995714d2905e1ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0xfdd9b5916d492501ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x1df9d5b18d694521ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x3d19f5d1ad896541ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x5d3915f1cda98561ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x7d593511edc9a581ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x9d7955310de9c5a1ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xbd9975512d09e5c1ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0xddb995714d2905e1ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0xfdd9b5916d492501ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Fourv4s_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Fourv4s_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD1Fourv4s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x3d19f5d1ad896541ULL << 64 |
                            0x1df9d5b18d694521ULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x7d593511edc9a581ULL << 64 |
                            0x5d3915f1cda98561ULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0xbd9975512d09e5c1ULL << 64 |
                            0x9d7955310de9c5a1ULL));
                     CHECK((__uint128_t)fprState->v3 ==
                           ((__uint128_t)0xfdd9b5916d492501ULL << 64 |
                            0xddb995714d2905e1ULL));
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 64);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1onev2d") {
  const char source[] = "ld1 {v0.2d}, [x0]\n";

  uint8_t buf[16] = {28, 59, 90, 121, 152, 183, 214, 245,
                     20, 51, 82, 113, 144, 175, 206, 237};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0xf5d6b798795a3b1cULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xedceaf9071523314ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xf5d6b798795a3b1cULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xedceaf9071523314ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Onev2d", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Onev2d", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Onev2d", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0xedceaf9071523314ULL << 64 |
                            0xf5d6b798795a3b1cULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1onev2d_post") {
  const char source[] = "ld1 {v0.2d}, [x0], #16\n";

  uint8_t buf[16] = {28, 59, 90, 121, 152, 183, 214, 245,
                     20, 51, 82, 113, 144, 175, 206, 237};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0xf5d6b798795a3b1cULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xedceaf9071523314ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xf5d6b798795a3b1cULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xedceaf9071523314ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Onev2d_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Onev2d_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD1Onev2d_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0xedceaf9071523314ULL << 64 |
                            0xf5d6b798795a3b1cULL));
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 16);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1twov2d") {
  const char source[] = "ld1 {v0.2d, v1.2d}, [x0]\n";

  uint8_t buf[32] = {31,  65,  99,  133, 167, 201, 235, 13,  47,  81,  115,
                     149, 183, 217, 251, 29,  63,  97,  131, 165, 199, 233,
                     11,  45,  79,  113, 147, 181, 215, 249, 27,  61};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x0debc9a78563411fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1dfbd9b79573512fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x2d0be9c7a583613fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x3d1bf9d7b593714fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x0debc9a78563411fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1dfbd9b79573512fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x2d0be9c7a583613fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x3d1bf9d7b593714fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Twov2d", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Twov2d", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Twov2d", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x1dfbd9b79573512fULL << 64 |
                            0x0debc9a78563411fULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x3d1bf9d7b593714fULL << 64 |
                            0x2d0be9c7a583613fULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1twov2d_post") {
  const char source[] = "ld1 {v0.2d, v1.2d}, [x0], #32\n";

  uint8_t buf[32] = {31,  65,  99,  133, 167, 201, 235, 13,  47,  81,  115,
                     149, 183, 217, 251, 29,  63,  97,  131, 165, 199, 233,
                     11,  45,  79,  113, 147, 181, 215, 249, 27,  61};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x0debc9a78563411fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1dfbd9b79573512fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x2d0be9c7a583613fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x3d1bf9d7b593714fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x0debc9a78563411fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1dfbd9b79573512fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x2d0be9c7a583613fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x3d1bf9d7b593714fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Twov2d_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Twov2d_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD1Twov2d_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x1dfbd9b79573512fULL << 64 |
                            0x0debc9a78563411fULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x3d1bf9d7b593714fULL << 64 |
                            0x2d0be9c7a583613fULL));
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 32);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1threev2d") {
  const char source[] = "ld1 {v0.2d, v1.2d, v2.2d}, [x0]\n";

  uint8_t buf[48] = {34,  71,  108, 145, 182, 219, 165, 37,  74,  111,
                     148, 185, 222, 3,   40,  77,  114, 151, 188, 225,
                     6,   43,  80,  117, 154, 191, 228, 9,   46,  83,
                     120, 157, 194, 231, 12,  49,  86,  123, 160, 197,
                     234, 15,  52,  89,  126, 163, 200, 237};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x25a5dbb6916c4722ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x4d2803deb9946f4aULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x75502b06e1bc9772ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x9d78532e09e4bf9aULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0xc5a07b56310ce7c2ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xedc8a37e59340feaULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x25a5dbb6916c4722ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x4d2803deb9946f4aULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x75502b06e1bc9772ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x9d78532e09e4bf9aULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0xc5a07b56310ce7c2ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xedc8a37e59340feaULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Threev2d", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Threev2d", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Threev2d", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x4d2803deb9946f4aULL << 64 |
                            0x25a5dbb6916c4722ULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x9d78532e09e4bf9aULL << 64 |
                            0x75502b06e1bc9772ULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0xedc8a37e59340feaULL << 64 |
                            0xc5a07b56310ce7c2ULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1threev2d_post") {
  const char source[] = "ld1 {v0.2d, v1.2d, v2.2d}, [x0], #48\n";

  uint8_t buf[48] = {34,  71,  108, 145, 182, 219, 165, 37,  74,  111,
                     148, 185, 222, 3,   40,  77,  114, 151, 188, 225,
                     6,   43,  80,  117, 154, 191, 228, 9,   46,  83,
                     120, 157, 194, 231, 12,  49,  86,  123, 160, 197,
                     234, 15,  52,  89,  126, 163, 200, 237};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x25a5dbb6916c4722ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x4d2803deb9946f4aULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x75502b06e1bc9772ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x9d78532e09e4bf9aULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0xc5a07b56310ce7c2ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xedc8a37e59340feaULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x25a5dbb6916c4722ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x4d2803deb9946f4aULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x75502b06e1bc9772ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x9d78532e09e4bf9aULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0xc5a07b56310ce7c2ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xedc8a37e59340feaULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Threev2d_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("LD1Threev2d_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD1Threev2d_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x4d2803deb9946f4aULL << 64 |
                            0x25a5dbb6916c4722ULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x9d78532e09e4bf9aULL << 64 |
                            0x75502b06e1bc9772ULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0xedc8a37e59340feaULL << 64 |
                            0xc5a07b56310ce7c2ULL));
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 48);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1fourv2d") {
  const char source[] = "ld1 {v0.2d, v1.2d, v2.2d, v3.2d}, [x0]\n";

  uint8_t buf[64] = {37,  77,  117, 157, 197, 237, 21,  61,  101, 141, 181,
                     221, 5,   45,  85,  125, 165, 205, 245, 29,  69,  109,
                     149, 189, 229, 13,  53,  93,  133, 173, 213, 253, 37,
                     77,  117, 157, 197, 237, 21,  61,  101, 141, 181, 221,
                     5,   45,  85,  125, 165, 205, 245, 29,  69,  109, 149,
                     189, 229, 13,  53,  93,  133, 173, 213, 253};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x3d15edc59d754d25ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x7d552d05ddb58d65ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xbd956d451df5cda5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xfdd5ad855d350de5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x3d15edc59d754d25ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x7d552d05ddb58d65ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0xbd956d451df5cda5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0xfdd5ad855d350de5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x3d15edc59d754d25ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x7d552d05ddb58d65ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xbd956d451df5cda5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xfdd5ad855d350de5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x3d15edc59d754d25ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x7d552d05ddb58d65ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0xbd956d451df5cda5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0xfdd5ad855d350de5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Fourv2d", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Fourv2d", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Fourv2d", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x7d552d05ddb58d65ULL << 64 |
                            0x3d15edc59d754d25ULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0xfdd5ad855d350de5ULL << 64 |
                            0xbd956d451df5cda5ULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x7d552d05ddb58d65ULL << 64 |
                            0x3d15edc59d754d25ULL));
                     CHECK((__uint128_t)fprState->v3 ==
                           ((__uint128_t)0xfdd5ad855d350de5ULL << 64 |
                            0xbd956d451df5cda5ULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1fourv2d_post") {
  const char source[] = "ld1 {v0.2d, v1.2d, v2.2d, v3.2d}, [x0], #64\n";

  uint8_t buf[64] = {37,  77,  117, 157, 197, 237, 21,  61,  101, 141, 181,
                     221, 5,   45,  85,  125, 165, 205, 245, 29,  69,  109,
                     149, 189, 229, 13,  53,  93,  133, 173, 213, 253, 37,
                     77,  117, 157, 197, 237, 21,  61,  101, 141, 181, 221,
                     5,   45,  85,  125, 165, 205, 245, 29,  69,  109, 149,
                     189, 229, 13,  53,  93,  133, 173, 213, 253};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x3d15edc59d754d25ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x7d552d05ddb58d65ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xbd956d451df5cda5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xfdd5ad855d350de5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x3d15edc59d754d25ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x7d552d05ddb58d65ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0xbd956d451df5cda5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0xfdd5ad855d350de5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x3d15edc59d754d25ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x7d552d05ddb58d65ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xbd956d451df5cda5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xfdd5ad855d350de5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x3d15edc59d754d25ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x7d552d05ddb58d65ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0xbd956d451df5cda5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0xfdd5ad855d350de5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Fourv2d_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Fourv2d_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD1Fourv2d_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x7d552d05ddb58d65ULL << 64 |
                            0x3d15edc59d754d25ULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0xfdd5ad855d350de5ULL << 64 |
                            0xbd956d451df5cda5ULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x7d552d05ddb58d65ULL << 64 |
                            0x3d15edc59d754d25ULL));
                     CHECK((__uint128_t)fprState->v3 ==
                           ((__uint128_t)0xfdd5ad855d350de5ULL << 64 |
                            0xbd956d451df5cda5ULL));
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 64);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2twov16b") {
  const char source[] = "ld2 {v0.16b, v1.16b}, [x0]\n";

  uint8_t buf[32] = {40,  83, 126, 169, 212, 255, 42,  85, 128, 171, 214,
                     1,   44, 87,  130, 173, 216, 3,   46, 89,  132, 175,
                     218, 5,  48,  91,  134, 177, 220, 7,  50,  93};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x552affd4a97e5328ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xad82572c01d6ab80ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x05daaf84592e03d8ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x5d3207dcb1865b30ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x552affd4a97e5328ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xad82572c01d6ab80ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x05daaf84592e03d8ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x5d3207dcb1865b30ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2Twov16b", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2Twov16b", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD2Twov16b", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x32dc8630da842ed8ULL << 64 |
                            0x822cd6802ad47e28ULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x5d07b15b05af5903ULL << 64 |
                            0xad5701ab55ffa953ULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2twov16b_post") {
  const char source[] = "ld2 {v0.16b, v1.16b}, [x0], #32\n";

  uint8_t buf[32] = {40,  83, 126, 169, 212, 255, 42,  85, 128, 171, 214,
                     1,   44, 87,  130, 173, 216, 3,   46, 89,  132, 175,
                     218, 5,  48,  91,  134, 177, 220, 7,  50,  93};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x552affd4a97e5328ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xad82572c01d6ab80ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x05daaf84592e03d8ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x5d3207dcb1865b30ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x552affd4a97e5328ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xad82572c01d6ab80ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x05daaf84592e03d8ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x5d3207dcb1865b30ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2Twov16b_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2Twov16b_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD2Twov16b_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x32dc8630da842ed8ULL << 64 |
                            0x822cd6802ad47e28ULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x5d07b15b05af5903ULL << 64 |
                            0xad5701ab55ffa953ULL));
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 32);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3threev16b") {
  const char source[] = "ld3 {v0.16b, v1.16b, v2.16b}, [x0]\n";

  uint8_t buf[48] = {
      45, 93, 141, 189, 237, 29, 77, 125, 173, 221, 13, 61, 109, 157, 205, 253,
      45, 93, 141, 189, 237, 29, 77, 125, 173, 221, 13, 61, 109, 157, 205, 253,
      45, 93, 141, 189, 237, 29, 77, 125, 173, 221, 13, 61, 109, 157, 205, 253};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x7d4d1dedbd8d5d2dULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xfdcd9d6d3d0dddadULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x7d4d1dedbd8d5d2dULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xfdcd9d6d3d0dddadULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x7d4d1dedbd8d5d2dULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xfdcd9d6d3d0dddadULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x7d4d1dedbd8d5d2dULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xfdcd9d6d3d0dddadULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x7d4d1dedbd8d5d2dULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xfdcd9d6d3d0dddadULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x7d4d1dedbd8d5d2dULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xfdcd9d6d3d0dddadULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3Threev16b", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD3Threev16b", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD3Threev16b", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x9d0d7ded5dcd3dadULL << 64 |
                            0x1d8dfd6ddd4dbd2dULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0xcd3dad1d8dfd6dddULL << 64 |
                            0x4dbd2d9d0d7ded5dULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0xfd6ddd4dbd2d9d0dULL << 64 |
                            0x7ded5dcd3dad1d8dULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3threev16b_post") {
  const char source[] = "ld3 {v0.16b, v1.16b, v2.16b}, [x0], #48\n";

  uint8_t buf[48] = {
      45, 93, 141, 189, 237, 29, 77, 125, 173, 221, 13, 61, 109, 157, 205, 253,
      45, 93, 141, 189, 237, 29, 77, 125, 173, 221, 13, 61, 109, 157, 205, 253,
      45, 93, 141, 189, 237, 29, 77, 125, 173, 221, 13, 61, 109, 157, 205, 253};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x7d4d1dedbd8d5d2dULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xfdcd9d6d3d0dddadULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x7d4d1dedbd8d5d2dULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xfdcd9d6d3d0dddadULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x7d4d1dedbd8d5d2dULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xfdcd9d6d3d0dddadULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x7d4d1dedbd8d5d2dULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xfdcd9d6d3d0dddadULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x7d4d1dedbd8d5d2dULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xfdcd9d6d3d0dddadULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x7d4d1dedbd8d5d2dULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xfdcd9d6d3d0dddadULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3Threev16b_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("LD3Threev16b_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD3Threev16b_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x9d0d7ded5dcd3dadULL << 64 |
                            0x1d8dfd6ddd4dbd2dULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0xcd3dad1d8dfd6dddULL << 64 |
                            0x4dbd2d9d0d7ded5dULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0xfd6ddd4dbd2d9d0dULL << 64 |
                            0x7ded5dcd3dad1d8dULL));
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 48);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4fourv16b") {
  const char source[] = "ld4 {v0.16b, v1.16b, v2.16b, v3.16b}, [x0]\n";

  uint8_t buf[64] = {50,  103, 156, 209, 6,   59,  112, 165, 218, 15,  68,
                     121, 174, 227, 24,  77,  130, 183, 236, 33,  86,  139,
                     192, 245, 42,  95,  148, 201, 254, 51,  104, 157, 210,
                     7,   60,  113, 166, 219, 16,  69,  122, 175, 228, 25,
                     78,  131, 184, 237, 34,  87,  140, 193, 246, 43,  96,
                     149, 202, 255, 52,  105, 158, 211, 8,   61};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0xa5703b06d19c6732ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x4d18e3ae79440fdaULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xf5c08b5621ecb782ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x9d6833fec9945f2aULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x4510dba6713c07d2ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xedb8834e19e4af7aULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x95602bf6c18c5722ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x3d08d39e6934ffcaULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xa5703b06d19c6732ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x4d18e3ae79440fdaULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xf5c08b5621ecb782ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x9d6833fec9945f2aULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x4510dba6713c07d2ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xedb8834e19e4af7aULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x95602bf6c18c5722ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x3d08d39e6934ffcaULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4Fourv16b", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4Fourv16b", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD4Fourv16b", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x9ecaf6224e7aa6d2ULL << 64 |
                            0xfe2a5682aeda0632ULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0xd3ff2b5783afdb07ULL << 64 |
                            0x335f8bb7e30f3b67ULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x0834608cb8e4103cULL << 64 |
                            0x6894c0ec1844709cULL));
                     CHECK((__uint128_t)fprState->v3 ==
                           ((__uint128_t)0x3d6995c1ed194571ULL << 64 |
                            0x9dc9f5214d79a5d1ULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4fourv16b_post") {
  const char source[] = "ld4 {v0.16b, v1.16b, v2.16b, v3.16b}, [x0], #64\n";

  uint8_t buf[64] = {50,  103, 156, 209, 6,   59,  112, 165, 218, 15,  68,
                     121, 174, 227, 24,  77,  130, 183, 236, 33,  86,  139,
                     192, 245, 42,  95,  148, 201, 254, 51,  104, 157, 210,
                     7,   60,  113, 166, 219, 16,  69,  122, 175, 228, 25,
                     78,  131, 184, 237, 34,  87,  140, 193, 246, 43,  96,
                     149, 202, 255, 52,  105, 158, 211, 8,   61};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0xa5703b06d19c6732ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x4d18e3ae79440fdaULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xf5c08b5621ecb782ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x9d6833fec9945f2aULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x4510dba6713c07d2ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xedb8834e19e4af7aULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x95602bf6c18c5722ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x3d08d39e6934ffcaULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xa5703b06d19c6732ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x4d18e3ae79440fdaULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xf5c08b5621ecb782ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x9d6833fec9945f2aULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x4510dba6713c07d2ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xedb8834e19e4af7aULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x95602bf6c18c5722ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x3d08d39e6934ffcaULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4Fourv16b_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("LD4Fourv16b_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD4Fourv16b_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x9ecaf6224e7aa6d2ULL << 64 |
                            0xfe2a5682aeda0632ULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0xd3ff2b5783afdb07ULL << 64 |
                            0x335f8bb7e30f3b67ULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x0834608cb8e4103cULL << 64 |
                            0x6894c0ec1844709cULL));
                     CHECK((__uint128_t)fprState->v3 ==
                           ((__uint128_t)0x3d6995c1ed194571ULL << 64 |
                            0x9dc9f5214d79a5d1ULL));
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 64);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2twov8h") {
  const char source[] = "ld2 {v0.8h, v1.8h}, [x0]\n";

  uint8_t buf[32] = {41,  85, 129, 173, 217, 5,   49,  93, 137, 181, 225,
                     13,  57, 101, 145, 189, 233, 21,  65, 109, 153, 197,
                     241, 29, 73,  117, 161, 205, 249, 37, 81,  125};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x5d3105d9ad815529ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xbd9165390de1b589ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x1df1c5996d4115e9ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x7d5125f9cda17549ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x5d3105d9ad815529ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xbd9165390de1b589ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x1df1c5996d4115e9ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x7d5125f9cda17549ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2Twov8h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2Twov8h", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD2Twov8h", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x25f97549c59915e9ULL << 64 |
                            0x6539b58905d95529ULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x7d51cda11df16d41ULL << 64 |
                            0xbd910de15d31ad81ULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2twov8h_post") {
  const char source[] = "ld2 {v0.8h, v1.8h}, [x0], #32\n";

  uint8_t buf[32] = {41,  85, 129, 173, 217, 5,   49,  93, 137, 181, 225,
                     13,  57, 101, 145, 189, 233, 21,  65, 109, 153, 197,
                     241, 29, 73,  117, 161, 205, 249, 37, 81,  125};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x5d3105d9ad815529ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xbd9165390de1b589ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x1df1c5996d4115e9ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x7d5125f9cda17549ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x5d3105d9ad815529ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xbd9165390de1b589ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x1df1c5996d4115e9ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x7d5125f9cda17549ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2Twov8h_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2Twov8h_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD2Twov8h_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x25f97549c59915e9ULL << 64 |
                            0x6539b58905d95529ULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x7d51cda11df16d41ULL << 64 |
                            0xbd910de15d31ad81ULL));
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 32);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3threev8h") {
  const char source[] = "ld3 {v0.8h, v1.8h, v2.8h}, [x0]\n";

  uint8_t buf[48] = {46,  95,  144, 193, 242, 35,  84,  133, 182, 231, 24,  73,
                     122, 171, 220, 13,  62,  111, 160, 209, 2,   51,  100, 149,
                     198, 247, 40,  89,  138, 187, 236, 29,  78,  127, 176, 225,
                     18,  67,  116, 165, 214, 7,   56,  105, 154, 203, 252, 45};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x855423f2c1905f2eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x0ddcab7a4918e7b6ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x95643302d1a06f3eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x1decbb8a5928f7c6ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0xa5744312e1b07f4eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x2dfccb9a693807d6ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x855423f2c1905f2eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x0ddcab7a4918e7b6ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x95643302d1a06f3eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x1decbb8a5928f7c6ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0xa5744312e1b07f4eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x2dfccb9a693807d6ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3Threev8h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD3Threev8h", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD3Threev8h", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x693843121decf7c6ULL << 64 |
                            0xd1a0ab7a85545f2eULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0xcb9aa5747f4e5928ULL << 64 |
                            0x33020ddce7b6c190ULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x2dfc07d6e1b0bb8aULL << 64 |
                            0x95646f3e491823f2ULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3threev8h_post") {
  const char source[] = "ld3 {v0.8h, v1.8h, v2.8h}, [x0], #48\n";

  uint8_t buf[48] = {46,  95,  144, 193, 242, 35,  84,  133, 182, 231, 24,  73,
                     122, 171, 220, 13,  62,  111, 160, 209, 2,   51,  100, 149,
                     198, 247, 40,  89,  138, 187, 236, 29,  78,  127, 176, 225,
                     18,  67,  116, 165, 214, 7,   56,  105, 154, 203, 252, 45};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x855423f2c1905f2eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x0ddcab7a4918e7b6ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x95643302d1a06f3eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x1decbb8a5928f7c6ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0xa5744312e1b07f4eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x2dfccb9a693807d6ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x855423f2c1905f2eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x0ddcab7a4918e7b6ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x95643302d1a06f3eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x1decbb8a5928f7c6ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0xa5744312e1b07f4eULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x2dfccb9a693807d6ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3Threev8h_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("LD3Threev8h_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD3Threev8h_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x693843121decf7c6ULL << 64 |
                            0xd1a0ab7a85545f2eULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0xcb9aa5747f4e5928ULL << 64 |
                            0x33020ddce7b6c190ULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x2dfc07d6e1b0bb8aULL << 64 |
                            0x95646f3e491823f2ULL));
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 48);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4fourv8h") {
  const char source[] = "ld4 {v0.8h, v1.8h, v2.8h, v3.8h}, [x0]\n";

  uint8_t buf[64] = {51,  105, 159, 213, 11,  65,  119, 173, 227, 25,  79,
                     133, 187, 241, 39,  93,  147, 201, 255, 53,  107, 161,
                     215, 13,  67,  121, 175, 229, 27,  81,  135, 189, 243,
                     41,  95,  149, 203, 1,   55,  109, 163, 217, 15,  69,
                     123, 177, 231, 29,  83,  137, 191, 245, 43,  97,  151,
                     205, 3,   57,  111, 165, 219, 17,  71,  125};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0xad77410bd59f6933ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x5d27f1bb854f19e3ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x0dd7a16b35ffc993ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xbd87511be5af7943ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x6d3701cb955f29f3ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x1de7b17b450fd9a3ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0xcd97612bf5bf8953ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x7d4711dba56f3903ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xad77410bd59f6933ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x5d27f1bb854f19e3ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x0dd7a16b35ffc993ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xbd87511be5af7943ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x6d3701cb955f29f3ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x1de7b17b450fd9a3ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0xcd97612bf5bf8953ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x7d4711dba56f3903ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4Fourv8h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4Fourv8h", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD4Fourv8h", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x39038953d9a329f3ULL << 64 |
                            0x7943c99319e36933ULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0xa56ff5bf450f955fULL << 64 |
                            0xe5af35ff854fd59fULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x11db612bb17b01cbULL << 64 |
                            0x511ba16bf1bb410bULL));
                     CHECK((__uint128_t)fprState->v3 ==
                           ((__uint128_t)0x7d47cd971de76d37ULL << 64 |
                            0xbd870dd75d27ad77ULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4fourv8h_post") {
  const char source[] = "ld4 {v0.8h, v1.8h, v2.8h, v3.8h}, [x0], #64\n";

  uint8_t buf[64] = {51,  105, 159, 213, 11,  65,  119, 173, 227, 25,  79,
                     133, 187, 241, 39,  93,  147, 201, 255, 53,  107, 161,
                     215, 13,  67,  121, 175, 229, 27,  81,  135, 189, 243,
                     41,  95,  149, 203, 1,   55,  109, 163, 217, 15,  69,
                     123, 177, 231, 29,  83,  137, 191, 245, 43,  97,  151,
                     205, 3,   57,  111, 165, 219, 17,  71,  125};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0xad77410bd59f6933ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x5d27f1bb854f19e3ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x0dd7a16b35ffc993ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xbd87511be5af7943ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x6d3701cb955f29f3ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x1de7b17b450fd9a3ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0xcd97612bf5bf8953ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x7d4711dba56f3903ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xad77410bd59f6933ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x5d27f1bb854f19e3ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x0dd7a16b35ffc993ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xbd87511be5af7943ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x6d3701cb955f29f3ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x1de7b17b450fd9a3ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0xcd97612bf5bf8953ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x7d4711dba56f3903ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4Fourv8h_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4Fourv8h_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD4Fourv8h_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x39038953d9a329f3ULL << 64 |
                            0x7943c99319e36933ULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0xa56ff5bf450f955fULL << 64 |
                            0xe5af35ff854fd59fULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x11db612bb17b01cbULL << 64 |
                            0x511ba16bf1bb410bULL));
                     CHECK((__uint128_t)fprState->v3 ==
                           ((__uint128_t)0x7d47cd971de76d37ULL << 64 |
                            0xbd870dd75d27ad77ULL));
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 64);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2twov4s") {
  const char source[] = "ld2 {v0.4s, v1.4s}, [x0]\n";

  uint8_t buf[32] = {43, 89, 135, 181, 227, 17, 63, 109, 155, 201, 247,
                     37, 83, 129, 175, 221, 11, 57, 103, 149, 195, 241,
                     31, 77, 123, 169, 215, 5,  51, 97,  143, 189};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x6d3f11e3b587592bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xddaf815325f7c99bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x4d1ff1c39567390bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xbd8f613305d7a97bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x6d3f11e3b587592bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xddaf815325f7c99bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x4d1ff1c39567390bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xbd8f613305d7a97bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2Twov4s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2Twov4s", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD2Twov4s", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x05d7a97b9567390bULL << 64 |
                            0x25f7c99bb587592bULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0xbd8f61334d1ff1c3ULL << 64 |
                            0xddaf81536d3f11e3ULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2twov4s_post") {
  const char source[] = "ld2 {v0.4s, v1.4s}, [x0], #32\n";

  uint8_t buf[32] = {43, 89, 135, 181, 227, 17, 63, 109, 155, 201, 247,
                     37, 83, 129, 175, 221, 11, 57, 103, 149, 195, 241,
                     31, 77, 123, 169, 215, 5,  51, 97,  143, 189};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x6d3f11e3b587592bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xddaf815325f7c99bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x4d1ff1c39567390bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xbd8f613305d7a97bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x6d3f11e3b587592bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xddaf815325f7c99bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x4d1ff1c39567390bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xbd8f613305d7a97bULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2Twov4s_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2Twov4s_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD2Twov4s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x05d7a97b9567390bULL << 64 |
                            0x25f7c99bb587592bULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0xbd8f61334d1ff1c3ULL << 64 |
                            0xddaf81536d3f11e3ULL));
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 32);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3threev4s") {
  const char source[] = "ld3 {v0.4s, v1.4s, v2.4s}, [x0]\n";

  uint8_t buf[48] = {48, 99, 150, 201, 252, 47, 98, 149, 200, 251,
                     46, 97, 148, 199, 250, 45, 96, 147, 198, 249,
                     44, 95, 146, 197, 248, 43, 94, 145, 196, 247,
                     42, 93, 144, 195, 246, 41, 92, 143, 194, 245,
                     40, 91, 142, 193, 244, 39, 90, 141};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x95622ffcc9966330ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x2dfac794612efbc8ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xc5925f2cf9c69360ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x5d2af7c4915e2bf8ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0xf5c28f5c29f6c390ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x8d5a27f4c18e5b28ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x95622ffcc9966330ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x2dfac794612efbc8ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xc5925f2cf9c69360ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x5d2af7c4915e2bf8ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0xf5c28f5c29f6c390ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x8d5a27f4c18e5b28ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3Threev4s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD3Threev4s", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD3Threev4s", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0xf5c28f5c915e2bf8ULL << 64 |
                            0x2dfac794c9966330ULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0xc18e5b285d2af7c4ULL << 64 |
                            0xf9c6936095622ffcULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x8d5a27f429f6c390ULL << 64 |
                            0xc5925f2c612efbc8ULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3threev4s_post") {
  const char source[] = "ld3 {v0.4s, v1.4s, v2.4s}, [x0], #48\n";

  uint8_t buf[48] = {48, 99, 150, 201, 252, 47, 98, 149, 200, 251,
                     46, 97, 148, 199, 250, 45, 96, 147, 198, 249,
                     44, 95, 146, 197, 248, 43, 94, 145, 196, 247,
                     42, 93, 144, 195, 246, 41, 92, 143, 194, 245,
                     40, 91, 142, 193, 244, 39, 90, 141};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x95622ffcc9966330ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x2dfac794612efbc8ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xc5925f2cf9c69360ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x5d2af7c4915e2bf8ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0xf5c28f5c29f6c390ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x8d5a27f4c18e5b28ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x95622ffcc9966330ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x2dfac794612efbc8ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xc5925f2cf9c69360ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x5d2af7c4915e2bf8ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0xf5c28f5c29f6c390ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x8d5a27f4c18e5b28ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3Threev4s_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("LD3Threev4s_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD3Threev4s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0xf5c28f5c915e2bf8ULL << 64 |
                            0x2dfac794c9966330ULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0xc18e5b285d2af7c4ULL << 64 |
                            0xf9c6936095622ffcULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x8d5a27f429f6c390ULL << 64 |
                            0xc5925f2c612efbc8ULL));
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 48);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4fourv4s") {
  const char source[] = "ld4 {v0.4s, v1.4s, v2.4s, v3.4s}, [x0]\n";

  uint8_t buf[64] = {53,  109, 165, 221, 21,  77,  133, 189, 245, 45,  101,
                     157, 213, 13,  69,  125, 181, 237, 37,  93,  149, 205,
                     5,   61,  117, 173, 229, 29,  85,  141, 197, 253, 53,
                     109, 165, 221, 21,  77,  133, 189, 245, 45,  101, 157,
                     213, 13,  69,  125, 181, 237, 37,  93,  149, 205, 5,
                     61,  117, 173, 229, 29,  85,  141, 197, 253};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0xbd854d15dda56d35ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x7d450dd59d652df5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x3d05cd955d25edb5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xfdc58d551de5ad75ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0xbd854d15dda56d35ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x7d450dd59d652df5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x3d05cd955d25edb5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0xfdc58d551de5ad75ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xbd854d15dda56d35ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x7d450dd59d652df5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x3d05cd955d25edb5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xfdc58d551de5ad75ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0xbd854d15dda56d35ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x7d450dd59d652df5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x3d05cd955d25edb5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0xfdc58d551de5ad75ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4Fourv4s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4Fourv4s", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD4Fourv4s", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x5d25edb5dda56d35ULL << 64 |
                            0x5d25edb5dda56d35ULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x3d05cd95bd854d15ULL << 64 |
                            0x3d05cd95bd854d15ULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x1de5ad759d652df5ULL << 64 |
                            0x1de5ad759d652df5ULL));
                     CHECK((__uint128_t)fprState->v3 ==
                           ((__uint128_t)0xfdc58d557d450dd5ULL << 64 |
                            0xfdc58d557d450dd5ULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4fourv4s_post") {
  const char source[] = "ld4 {v0.4s, v1.4s, v2.4s, v3.4s}, [x0], #64\n";

  uint8_t buf[64] = {53,  109, 165, 221, 21,  77,  133, 189, 245, 45,  101,
                     157, 213, 13,  69,  125, 181, 237, 37,  93,  149, 205,
                     5,   61,  117, 173, 229, 29,  85,  141, 197, 253, 53,
                     109, 165, 221, 21,  77,  133, 189, 245, 45,  101, 157,
                     213, 13,  69,  125, 181, 237, 37,  93,  149, 205, 5,
                     61,  117, 173, 229, 29,  85,  141, 197, 253};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0xbd854d15dda56d35ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x7d450dd59d652df5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x3d05cd955d25edb5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xfdc58d551de5ad75ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0xbd854d15dda56d35ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x7d450dd59d652df5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x3d05cd955d25edb5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0xfdc58d551de5ad75ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xbd854d15dda56d35ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x7d450dd59d652df5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x3d05cd955d25edb5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xfdc58d551de5ad75ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0xbd854d15dda56d35ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x7d450dd59d652df5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x3d05cd955d25edb5ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0xfdc58d551de5ad75ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4Fourv4s_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4Fourv4s_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD4Fourv4s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x5d25edb5dda56d35ULL << 64 |
                            0x5d25edb5dda56d35ULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x3d05cd95bd854d15ULL << 64 |
                            0x3d05cd95bd854d15ULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x1de5ad759d652df5ULL << 64 |
                            0x1de5ad759d652df5ULL));
                     CHECK((__uint128_t)fprState->v3 ==
                           ((__uint128_t)0xfdc58d557d450dd5ULL << 64 |
                            0xfdc58d557d450dd5ULL));
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 64);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2twov2d") {
  const char source[] = "ld2 {v0.2d, v1.2d}, [x0]\n";

  uint8_t buf[32] = {47,  97,  147, 197, 247, 41,  91,  141, 191, 241, 35,
                     85,  135, 185, 235, 29,  79,  129, 179, 229, 23,  73,
                     123, 173, 223, 17,  67,  117, 167, 217, 11,  61};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x8d5b29f7c593612fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1debb9875523f1bfULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xad7b4917e5b3814fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x3d0bd9a7754311dfULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x8d5b29f7c593612fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1debb9875523f1bfULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xad7b4917e5b3814fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x3d0bd9a7754311dfULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2Twov2d", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2Twov2d", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD2Twov2d", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0xad7b4917e5b3814fULL << 64 |
                            0x8d5b29f7c593612fULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x3d0bd9a7754311dfULL << 64 |
                            0x1debb9875523f1bfULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2twov2d_post") {
  const char source[] = "ld2 {v0.2d, v1.2d}, [x0], #32\n";

  uint8_t buf[32] = {47,  97,  147, 197, 247, 41,  91,  141, 191, 241, 35,
                     85,  135, 185, 235, 29,  79,  129, 179, 229, 23,  73,
                     123, 173, 223, 17,  67,  117, 167, 217, 11,  61};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x8d5b29f7c593612fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1debb9875523f1bfULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xad7b4917e5b3814fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x3d0bd9a7754311dfULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x8d5b29f7c593612fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1debb9875523f1bfULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xad7b4917e5b3814fULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x3d0bd9a7754311dfULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2Twov2d_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2Twov2d_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD2Twov2d_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0xad7b4917e5b3814fULL << 64 |
                            0x8d5b29f7c593612fULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x3d0bd9a7754311dfULL << 64 |
                            0x1debb9875523f1bfULL));
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 32);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3threev2d") {
  const char source[] = "ld3 {v0.2d, v1.2d, v2.2d}, [x0]\n";

  uint8_t buf[48] = {52,  107, 162, 217, 16,  71,  126, 181, 236, 35,  90,  145,
                     200, 255, 54,  109, 164, 219, 18,  73,  128, 183, 238, 37,
                     92,  147, 202, 1,   56,  111, 166, 221, 20,  75,  130, 185,
                     240, 39,  94,  149, 204, 3,   58,  113, 168, 223, 22,  77};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0xb57e4710d9a26b34ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x6d36ffc8915a23ecULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x25eeb7804912dba4ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xdda66f3801ca935cULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x955e27f0b9824b14ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x4d16dfa8713a03ccULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xb57e4710d9a26b34ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x6d36ffc8915a23ecULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x25eeb7804912dba4ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xdda66f3801ca935cULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x955e27f0b9824b14ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x4d16dfa8713a03ccULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3Threev2d", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD3Threev2d", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD3Threev2d", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0xdda66f3801ca935cULL << 64 |
                            0xb57e4710d9a26b34ULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x955e27f0b9824b14ULL << 64 |
                            0x6d36ffc8915a23ecULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x4d16dfa8713a03ccULL << 64 |
                            0x25eeb7804912dba4ULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3threev2d_post") {
  const char source[] = "ld3 {v0.2d, v1.2d, v2.2d}, [x0], #48\n";

  uint8_t buf[48] = {52,  107, 162, 217, 16,  71,  126, 181, 236, 35,  90,  145,
                     200, 255, 54,  109, 164, 219, 18,  73,  128, 183, 238, 37,
                     92,  147, 202, 1,   56,  111, 166, 221, 20,  75,  130, 185,
                     240, 39,  94,  149, 204, 3,   58,  113, 168, 223, 22,  77};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0xb57e4710d9a26b34ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x6d36ffc8915a23ecULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x25eeb7804912dba4ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xdda66f3801ca935cULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x955e27f0b9824b14ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x4d16dfa8713a03ccULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xb57e4710d9a26b34ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x6d36ffc8915a23ecULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x25eeb7804912dba4ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xdda66f3801ca935cULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x955e27f0b9824b14ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x4d16dfa8713a03ccULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3Threev2d_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("LD3Threev2d_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD3Threev2d_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0xdda66f3801ca935cULL << 64 |
                            0xb57e4710d9a26b34ULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x955e27f0b9824b14ULL << 64 |
                            0x6d36ffc8915a23ecULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x4d16dfa8713a03ccULL << 64 |
                            0x25eeb7804912dba4ULL));
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 48);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4fourv2d") {
  const char source[] = "ld4 {v0.2d, v1.2d, v2.2d, v3.2d}, [x0]\n";

  uint8_t buf[64] = {57,  117, 177, 237, 41,  101, 161, 221, 25,  85,  145,
                     205, 9,   69,  129, 189, 249, 53,  113, 173, 233, 37,
                     97,  157, 217, 21,  81,  141, 201, 5,   65,  125, 185,
                     245, 49,  109, 169, 229, 33,  93,  153, 213, 17,  77,
                     137, 197, 1,   61,  121, 181, 241, 45,  105, 165, 225,
                     29,  89,  149, 209, 13,  73,  133, 193, 253};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0xdda16529edb17539ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xbd814509cd915519ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x9d6125e9ad7135f9ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x7d4105c98d5115d9ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x5d21e5a96d31f5b9ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x3d01c5894d11d599ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x1de1a5692df1b579ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0xfdc185490dd19559ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xdda16529edb17539ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xbd814509cd915519ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x9d6125e9ad7135f9ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x7d4105c98d5115d9ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x5d21e5a96d31f5b9ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x3d01c5894d11d599ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x1de1a5692df1b579ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0xfdc185490dd19559ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4Fourv2d", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4Fourv2d", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD4Fourv2d", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x5d21e5a96d31f5b9ULL << 64 |
                            0xdda16529edb17539ULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x3d01c5894d11d599ULL << 64 |
                            0xbd814509cd915519ULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x1de1a5692df1b579ULL << 64 |
                            0x9d6125e9ad7135f9ULL));
                     CHECK((__uint128_t)fprState->v3 ==
                           ((__uint128_t)0xfdc185490dd19559ULL << 64 |
                            0x7d4105c98d5115d9ULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4fourv2d_post") {
  const char source[] = "ld4 {v0.2d, v1.2d, v2.2d, v3.2d}, [x0], #64\n";

  uint8_t buf[64] = {57,  117, 177, 237, 41,  101, 161, 221, 25,  85,  145,
                     205, 9,   69,  129, 189, 249, 53,  113, 173, 233, 37,
                     97,  157, 217, 21,  81,  141, 201, 5,   65,  125, 185,
                     245, 49,  109, 169, 229, 33,  93,  153, 213, 17,  77,
                     137, 197, 1,   61,  121, 181, 241, 45,  105, 165, 225,
                     29,  89,  149, 209, 13,  73,  133, 193, 253};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0xdda16529edb17539ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xbd814509cd915519ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x9d6125e9ad7135f9ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x7d4105c98d5115d9ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x5d21e5a96d31f5b9ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x3d01c5894d11d599ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x1de1a5692df1b579ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0xfdc185490dd19559ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xdda16529edb17539ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xbd814509cd915519ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x9d6125e9ad7135f9ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x7d4105c98d5115d9ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x5d21e5a96d31f5b9ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x3d01c5894d11d599ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x1de1a5692df1b579ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0xfdc185490dd19559ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4Fourv2d_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4Fourv2d_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD4Fourv2d_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x5d21e5a96d31f5b9ULL << 64 |
                            0xdda16529edb17539ULL));
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x3d01c5894d11d599ULL << 64 |
                            0xbd814509cd915519ULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x1de1a5692df1b579ULL << 64 |
                            0x9d6125e9ad7135f9ULL));
                     CHECK((__uint128_t)fprState->v3 ==
                           ((__uint128_t)0xfdc185490dd19559ULL << 64 |
                            0x7d4105c98d5115d9ULL));
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 64);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1onev16b") {
  const char source[] = "st1 {v0.16b}, [x0]\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x9d693501cd996531ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x3d09d5a16d3905d1ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Onev16b", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Onev16b", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x3d09d5a16d3905d1ULL << 64 | 0x9d693501cd996531ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == 0x9d693501cd996531ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x3d09d5a16d3905d1ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1onev16b_post") {
  const char source[] = "st1 {v0.16b}, [x0], #16\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x9d693501cd996531ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x3d09d5a16d3905d1ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Onev16b_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Onev16b_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Onev16b_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 16);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x3d09d5a16d3905d1ULL << 64 | 0x9d693501cd996531ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0x9d693501cd996531ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x3d09d5a16d3905d1ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1twov16b") {
  const char source[] = "st1 {v0.16b, v1.16b}, [x0]\n";

  uint8_t buf[32] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xd59a5f24e9ae7338ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xad7237fcc1864b10ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x854a0fd4995e23e8ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x5d22e7ac7136fbc0ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Twov16b", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Twov16b", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0xad7237fcc1864b10ULL << 64 | 0xd59a5f24e9ae7338ULL);
  fpr->v1 = ((__uint128_t)0x5d22e7ac7136fbc0ULL << 64 | 0x854a0fd4995e23e8ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == 0xd59a5f24e9ae7338ULL);
  CHECK(*(uint64_t *)&buf[8] == 0xad7237fcc1864b10ULL);
  CHECK(*(uint64_t *)&buf[16] == 0x854a0fd4995e23e8ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x5d22e7ac7136fbc0ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1twov16b_post") {
  const char source[] = "st1 {v0.16b, v1.16b}, [x0], #32\n";

  uint8_t buf[32] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xd59a5f24e9ae7338ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xad7237fcc1864b10ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x854a0fd4995e23e8ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x5d22e7ac7136fbc0ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Twov16b_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Twov16b_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Twov16b_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 32);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0xad7237fcc1864b10ULL << 64 | 0xd59a5f24e9ae7338ULL);
  fpr->v1 = ((__uint128_t)0x5d22e7ac7136fbc0ULL << 64 | 0x854a0fd4995e23e8ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0xd59a5f24e9ae7338ULL);
  CHECK(*(uint64_t *)&buf[8] == 0xad7237fcc1864b10ULL);
  CHECK(*(uint64_t *)&buf[16] == 0x854a0fd4995e23e8ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x5d22e7ac7136fbc0ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1threev16b") {
  const char source[] = "st1 {v0.16b, v1.16b, v2.16b}, [x0]\n";

  uint8_t buf[48] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x0dcb894705c3813fULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1ddb995715d3914fULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x2deba96725e3a15fULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x3dfbb97735f3b16fULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x4d0bc9874503c17fULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x5d1bd9975513d18fULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Threev16b", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Threev16b", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x1ddb995715d3914fULL << 64 | 0x0dcb894705c3813fULL);
  fpr->v1 = ((__uint128_t)0x3dfbb97735f3b16fULL << 64 | 0x2deba96725e3a15fULL);
  fpr->v2 = ((__uint128_t)0x5d1bd9975513d18fULL << 64 | 0x4d0bc9874503c17fULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == 0x0dcb894705c3813fULL);
  CHECK(*(uint64_t *)&buf[8] == 0x1ddb995715d3914fULL);
  CHECK(*(uint64_t *)&buf[16] == 0x2deba96725e3a15fULL);
  CHECK(*(uint64_t *)&buf[24] == 0x3dfbb97735f3b16fULL);
  CHECK(*(uint64_t *)&buf[32] == 0x4d0bc9874503c17fULL);
  CHECK(*(uint64_t *)&buf[40] == 0x5d1bd9975513d18fULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1threev16b_post") {
  const char source[] = "st1 {v0.16b, v1.16b, v2.16b}, [x0], #48\n";

  uint8_t buf[48] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x0dcb894705c3813fULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1ddb995715d3914fULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x2deba96725e3a15fULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x3dfbb97735f3b16fULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x4d0bc9874503c17fULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x5d1bd9975513d18fULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Threev16b_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("ST1Threev16b_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Threev16b_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 48);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x1ddb995715d3914fULL << 64 | 0x0dcb894705c3813fULL);
  fpr->v1 = ((__uint128_t)0x3dfbb97735f3b16fULL << 64 | 0x2deba96725e3a15fULL);
  fpr->v2 = ((__uint128_t)0x5d1bd9975513d18fULL << 64 | 0x4d0bc9874503c17fULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0x0dcb894705c3813fULL);
  CHECK(*(uint64_t *)&buf[8] == 0x1ddb995715d3914fULL);
  CHECK(*(uint64_t *)&buf[16] == 0x2deba96725e3a15fULL);
  CHECK(*(uint64_t *)&buf[24] == 0x3dfbb97735f3b16fULL);
  CHECK(*(uint64_t *)&buf[32] == 0x4d0bc9874503c17fULL);
  CHECK(*(uint64_t *)&buf[40] == 0x5d1bd9975513d18fULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1fourv16b") {
  const char source[] = "st1 {v0.16b, v1.16b, v2.16b, v3.16b}, [x0]\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x45fcb36a21d88f46ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x8d44fbb26920d78eULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xd58c43fab1681fd6ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x1dd48b42f9b0671eULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x651cd38a41f8af66ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xad641bd28940f7aeULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0xf5ac631ad1883ff6ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x3df4ab6219d0873eULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Fourv16b", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Fourv16b", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x8d44fbb26920d78eULL << 64 | 0x45fcb36a21d88f46ULL);
  fpr->v1 = ((__uint128_t)0x1dd48b42f9b0671eULL << 64 | 0xd58c43fab1681fd6ULL);
  fpr->v2 = ((__uint128_t)0xad641bd28940f7aeULL << 64 | 0x651cd38a41f8af66ULL);
  fpr->v3 = ((__uint128_t)0x3df4ab6219d0873eULL << 64 | 0xf5ac631ad1883ff6ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == 0x45fcb36a21d88f46ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x8d44fbb26920d78eULL);
  CHECK(*(uint64_t *)&buf[16] == 0xd58c43fab1681fd6ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x1dd48b42f9b0671eULL);
  CHECK(*(uint64_t *)&buf[32] == 0x651cd38a41f8af66ULL);
  CHECK(*(uint64_t *)&buf[40] == 0xad641bd28940f7aeULL);
  CHECK(*(uint64_t *)&buf[48] == 0xf5ac631ad1883ff6ULL);
  CHECK(*(uint64_t *)&buf[56] == 0x3df4ab6219d0873eULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1fourv16b_post") {
  const char source[] = "st1 {v0.16b, v1.16b, v2.16b, v3.16b}, [x0], #64\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x45fcb36a21d88f46ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x8d44fbb26920d78eULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xd58c43fab1681fd6ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x1dd48b42f9b0671eULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x651cd38a41f8af66ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xad641bd28940f7aeULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0xf5ac631ad1883ff6ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x3df4ab6219d0873eULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Fourv16b_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("ST1Fourv16b_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Fourv16b_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 64);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x8d44fbb26920d78eULL << 64 | 0x45fcb36a21d88f46ULL);
  fpr->v1 = ((__uint128_t)0x1dd48b42f9b0671eULL << 64 | 0xd58c43fab1681fd6ULL);
  fpr->v2 = ((__uint128_t)0xad641bd28940f7aeULL << 64 | 0x651cd38a41f8af66ULL);
  fpr->v3 = ((__uint128_t)0x3df4ab6219d0873eULL << 64 | 0xf5ac631ad1883ff6ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0x45fcb36a21d88f46ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x8d44fbb26920d78eULL);
  CHECK(*(uint64_t *)&buf[16] == 0xd58c43fab1681fd6ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x1dd48b42f9b0671eULL);
  CHECK(*(uint64_t *)&buf[32] == 0x651cd38a41f8af66ULL);
  CHECK(*(uint64_t *)&buf[40] == 0xad641bd28940f7aeULL);
  CHECK(*(uint64_t *)&buf[48] == 0xf5ac631ad1883ff6ULL);
  CHECK(*(uint64_t *)&buf[56] == 0x3df4ab6219d0873eULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1onev8h") {
  const char source[] = "st1 {v0.8h}, [x0]\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xa5703b06d19c6732ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x4d18e3ae79440fdaULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Onev8h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Onev8h", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x4d18e3ae79440fdaULL << 64 | 0xa5703b06d19c6732ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == 0xa5703b06d19c6732ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x4d18e3ae79440fdaULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1onev8h_post") {
  const char source[] = "st1 {v0.8h}, [x0], #16\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xa5703b06d19c6732ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x4d18e3ae79440fdaULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Onev8h_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Onev8h_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Onev8h_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 16);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x4d18e3ae79440fdaULL << 64 | 0xa5703b06d19c6732ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0xa5703b06d19c6732ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x4d18e3ae79440fdaULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1twov8h") {
  const char source[] = "st1 {v0.8h, v1.8h}, [x0]\n";

  uint8_t buf[32] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xdda16529edb17539ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xbd814509cd915519ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x9d6125e9ad7135f9ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x7d4105c98d5115d9ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Twov8h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Twov8h", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0xbd814509cd915519ULL << 64 | 0xdda16529edb17539ULL);
  fpr->v1 = ((__uint128_t)0x7d4105c98d5115d9ULL << 64 | 0x9d6125e9ad7135f9ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == 0xdda16529edb17539ULL);
  CHECK(*(uint64_t *)&buf[8] == 0xbd814509cd915519ULL);
  CHECK(*(uint64_t *)&buf[16] == 0x9d6125e9ad7135f9ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x7d4105c98d5115d9ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1twov8h_post") {
  const char source[] = "st1 {v0.8h, v1.8h}, [x0], #32\n";

  uint8_t buf[32] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xdda16529edb17539ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xbd814509cd915519ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x9d6125e9ad7135f9ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x7d4105c98d5115d9ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Twov8h_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Twov8h_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Twov8h_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 32);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0xbd814509cd915519ULL << 64 | 0xdda16529edb17539ULL);
  fpr->v1 = ((__uint128_t)0x7d4105c98d5115d9ULL << 64 | 0x9d6125e9ad7135f9ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0xdda16529edb17539ULL);
  CHECK(*(uint64_t *)&buf[8] == 0xbd814509cd915519ULL);
  CHECK(*(uint64_t *)&buf[16] == 0x9d6125e9ad7135f9ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x7d4105c98d5115d9ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1threev8h") {
  const char source[] = "st1 {v0.8h, v1.8h, v2.8h}, [x0]\n";

  uint8_t buf[48] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x15d28f4c09c68340ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x2deaa76421de9b58ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x4502bf7c39f6b370ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x5d1ad794510ecb88ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x7532efac6926e3a0ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x8d4a07c4813efbb8ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Threev8h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Threev8h", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x2deaa76421de9b58ULL << 64 | 0x15d28f4c09c68340ULL);
  fpr->v1 = ((__uint128_t)0x5d1ad794510ecb88ULL << 64 | 0x4502bf7c39f6b370ULL);
  fpr->v2 = ((__uint128_t)0x8d4a07c4813efbb8ULL << 64 | 0x7532efac6926e3a0ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == 0x15d28f4c09c68340ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x2deaa76421de9b58ULL);
  CHECK(*(uint64_t *)&buf[16] == 0x4502bf7c39f6b370ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x5d1ad794510ecb88ULL);
  CHECK(*(uint64_t *)&buf[32] == 0x7532efac6926e3a0ULL);
  CHECK(*(uint64_t *)&buf[40] == 0x8d4a07c4813efbb8ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1threev8h_post") {
  const char source[] = "st1 {v0.8h, v1.8h, v2.8h}, [x0], #48\n";

  uint8_t buf[48] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x15d28f4c09c68340ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x2deaa76421de9b58ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x4502bf7c39f6b370ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x5d1ad794510ecb88ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x7532efac6926e3a0ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x8d4a07c4813efbb8ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Threev8h_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("ST1Threev8h_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Threev8h_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 48);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x2deaa76421de9b58ULL << 64 | 0x15d28f4c09c68340ULL);
  fpr->v1 = ((__uint128_t)0x5d1ad794510ecb88ULL << 64 | 0x4502bf7c39f6b370ULL);
  fpr->v2 = ((__uint128_t)0x8d4a07c4813efbb8ULL << 64 | 0x7532efac6926e3a0ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0x15d28f4c09c68340ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x2deaa76421de9b58ULL);
  CHECK(*(uint64_t *)&buf[16] == 0x4502bf7c39f6b370ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x5d1ad794510ecb88ULL);
  CHECK(*(uint64_t *)&buf[32] == 0x7532efac6926e3a0ULL);
  CHECK(*(uint64_t *)&buf[40] == 0x8d4a07c4813efbb8ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1fourv8h") {
  const char source[] = "st1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x0]\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x4d03b96f25db9147ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x9d5309bf752be197ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xeda3590fc57b31e7ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x3df3a95f15cb8137ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x8d43f9af651bd187ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xdd9349ffb56b21d7ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x2de3994f05bb7127ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x7d33e99f550bc177ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Fourv8h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Fourv8h", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x9d5309bf752be197ULL << 64 | 0x4d03b96f25db9147ULL);
  fpr->v1 = ((__uint128_t)0x3df3a95f15cb8137ULL << 64 | 0xeda3590fc57b31e7ULL);
  fpr->v2 = ((__uint128_t)0xdd9349ffb56b21d7ULL << 64 | 0x8d43f9af651bd187ULL);
  fpr->v3 = ((__uint128_t)0x7d33e99f550bc177ULL << 64 | 0x2de3994f05bb7127ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == 0x4d03b96f25db9147ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x9d5309bf752be197ULL);
  CHECK(*(uint64_t *)&buf[16] == 0xeda3590fc57b31e7ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x3df3a95f15cb8137ULL);
  CHECK(*(uint64_t *)&buf[32] == 0x8d43f9af651bd187ULL);
  CHECK(*(uint64_t *)&buf[40] == 0xdd9349ffb56b21d7ULL);
  CHECK(*(uint64_t *)&buf[48] == 0x2de3994f05bb7127ULL);
  CHECK(*(uint64_t *)&buf[56] == 0x7d33e99f550bc177ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1fourv8h_post") {
  const char source[] = "st1 {v0.8h, v1.8h, v2.8h, v3.8h}, [x0], #64\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x4d03b96f25db9147ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x9d5309bf752be197ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xeda3590fc57b31e7ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x3df3a95f15cb8137ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x8d43f9af651bd187ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xdd9349ffb56b21d7ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x2de3994f05bb7127ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x7d33e99f550bc177ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Fourv8h_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Fourv8h_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Fourv8h_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 64);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x9d5309bf752be197ULL << 64 | 0x4d03b96f25db9147ULL);
  fpr->v1 = ((__uint128_t)0x3df3a95f15cb8137ULL << 64 | 0xeda3590fc57b31e7ULL);
  fpr->v2 = ((__uint128_t)0xdd9349ffb56b21d7ULL << 64 | 0x8d43f9af651bd187ULL);
  fpr->v3 = ((__uint128_t)0x7d33e99f550bc177ULL << 64 | 0x2de3994f05bb7127ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0x4d03b96f25db9147ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x9d5309bf752be197ULL);
  CHECK(*(uint64_t *)&buf[16] == 0xeda3590fc57b31e7ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x3df3a95f15cb8137ULL);
  CHECK(*(uint64_t *)&buf[32] == 0x8d43f9af651bd187ULL);
  CHECK(*(uint64_t *)&buf[40] == 0xdd9349ffb56b21d7ULL);
  CHECK(*(uint64_t *)&buf[48] == 0x2de3994f05bb7127ULL);
  CHECK(*(uint64_t *)&buf[56] == 0x7d33e99f550bc177ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1onev4s") {
  const char source[] = "st1 {v0.4s}, [x0]\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xb57e4710d9a26b34ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x6d36ffc8915a23ecULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Onev4s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Onev4s", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x6d36ffc8915a23ecULL << 64 | 0xb57e4710d9a26b34ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == 0xb57e4710d9a26b34ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x6d36ffc8915a23ecULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1onev4s_post") {
  const char source[] = "st1 {v0.4s}, [x0], #16\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xb57e4710d9a26b34ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x6d36ffc8915a23ecULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Onev4s_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Onev4s_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Onev4s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 16);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x6d36ffc8915a23ecULL << 64 | 0xb57e4710d9a26b34ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0xb57e4710d9a26b34ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x6d36ffc8915a23ecULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1twov4s") {
  const char source[] = "st1 {v0.4s, v1.4s}, [x0]\n";

  uint8_t buf[32] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xedaf7133f5b7793bULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xdd9f6123e5a7692bULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xcd8f5113d597591bULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xbd7f4103c587490bULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Twov4s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Twov4s", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0xdd9f6123e5a7692bULL << 64 | 0xedaf7133f5b7793bULL);
  fpr->v1 = ((__uint128_t)0xbd7f4103c587490bULL << 64 | 0xcd8f5113d597591bULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == 0xedaf7133f5b7793bULL);
  CHECK(*(uint64_t *)&buf[8] == 0xdd9f6123e5a7692bULL);
  CHECK(*(uint64_t *)&buf[16] == 0xcd8f5113d597591bULL);
  CHECK(*(uint64_t *)&buf[24] == 0xbd7f4103c587490bULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1twov4s_post") {
  const char source[] = "st1 {v0.4s, v1.4s}, [x0], #32\n";

  uint8_t buf[32] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xedaf7133f5b7793bULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xdd9f6123e5a7692bULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xcd8f5113d597591bULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xbd7f4103c587490bULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Twov4s_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Twov4s_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Twov4s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 32);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0xdd9f6123e5a7692bULL << 64 | 0xedaf7133f5b7793bULL);
  fpr->v1 = ((__uint128_t)0xbd7f4103c587490bULL << 64 | 0xcd8f5113d597591bULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0xedaf7133f5b7793bULL);
  CHECK(*(uint64_t *)&buf[8] == 0xdd9f6123e5a7692bULL);
  CHECK(*(uint64_t *)&buf[16] == 0xcd8f5113d597591bULL);
  CHECK(*(uint64_t *)&buf[24] == 0xbd7f4103c587490bULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1threev4s") {
  const char source[] = "st1 {v0.4s, v1.4s, v2.4s}, [x0]\n";

  uint8_t buf[48] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x25e09b5611cc8742ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x4d08c37e39f4af6aULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x7530eba6611cd792ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x9d5813ce8944ffbaULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0xc5803bf6b16c27e2ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xeda8631ed9944f0aULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Threev4s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Threev4s", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x4d08c37e39f4af6aULL << 64 | 0x25e09b5611cc8742ULL);
  fpr->v1 = ((__uint128_t)0x9d5813ce8944ffbaULL << 64 | 0x7530eba6611cd792ULL);
  fpr->v2 = ((__uint128_t)0xeda8631ed9944f0aULL << 64 | 0xc5803bf6b16c27e2ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == 0x25e09b5611cc8742ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x4d08c37e39f4af6aULL);
  CHECK(*(uint64_t *)&buf[16] == 0x7530eba6611cd792ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x9d5813ce8944ffbaULL);
  CHECK(*(uint64_t *)&buf[32] == 0xc5803bf6b16c27e2ULL);
  CHECK(*(uint64_t *)&buf[40] == 0xeda8631ed9944f0aULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1threev4s_post") {
  const char source[] = "st1 {v0.4s, v1.4s, v2.4s}, [x0], #48\n";

  uint8_t buf[48] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x25e09b5611cc8742ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x4d08c37e39f4af6aULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x7530eba6611cd792ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x9d5813ce8944ffbaULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0xc5803bf6b16c27e2ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xeda8631ed9944f0aULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Threev4s_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("ST1Threev4s_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Threev4s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 48);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x4d08c37e39f4af6aULL << 64 | 0x25e09b5611cc8742ULL);
  fpr->v1 = ((__uint128_t)0x9d5813ce8944ffbaULL << 64 | 0x7530eba6611cd792ULL);
  fpr->v2 = ((__uint128_t)0xeda8631ed9944f0aULL << 64 | 0xc5803bf6b16c27e2ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0x25e09b5611cc8742ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x4d08c37e39f4af6aULL);
  CHECK(*(uint64_t *)&buf[16] == 0x7530eba6611cd792ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x9d5813ce8944ffbaULL);
  CHECK(*(uint64_t *)&buf[32] == 0xc5803bf6b16c27e2ULL);
  CHECK(*(uint64_t *)&buf[40] == 0xeda8631ed9944f0aULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1fourv4s") {
  const char source[] = "st1 {v0.4s, v1.4s, v2.4s, v3.4s}, [x0]\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x5d11c5792de19549ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xbd7125d98d41f5a9ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x1dd18539eda15509ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x7d31e5994d01b569ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0xdd9145f9ad6115c9ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x3df1a5590dc17529ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x9d5105b96d21d589ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0xfdb16519cd8135e9ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Fourv4s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Fourv4s", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0xbd7125d98d41f5a9ULL << 64 | 0x5d11c5792de19549ULL);
  fpr->v1 = ((__uint128_t)0x7d31e5994d01b569ULL << 64 | 0x1dd18539eda15509ULL);
  fpr->v2 = ((__uint128_t)0x3df1a5590dc17529ULL << 64 | 0xdd9145f9ad6115c9ULL);
  fpr->v3 = ((__uint128_t)0xfdb16519cd8135e9ULL << 64 | 0x9d5105b96d21d589ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == 0x5d11c5792de19549ULL);
  CHECK(*(uint64_t *)&buf[8] == 0xbd7125d98d41f5a9ULL);
  CHECK(*(uint64_t *)&buf[16] == 0x1dd18539eda15509ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x7d31e5994d01b569ULL);
  CHECK(*(uint64_t *)&buf[32] == 0xdd9145f9ad6115c9ULL);
  CHECK(*(uint64_t *)&buf[40] == 0x3df1a5590dc17529ULL);
  CHECK(*(uint64_t *)&buf[48] == 0x9d5105b96d21d589ULL);
  CHECK(*(uint64_t *)&buf[56] == 0xfdb16519cd8135e9ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1fourv4s_post") {
  const char source[] = "st1 {v0.4s, v1.4s, v2.4s, v3.4s}, [x0], #64\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x5d11c5792de19549ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xbd7125d98d41f5a9ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x1dd18539eda15509ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x7d31e5994d01b569ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0xdd9145f9ad6115c9ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x3df1a5590dc17529ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x9d5105b96d21d589ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0xfdb16519cd8135e9ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Fourv4s_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Fourv4s_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Fourv4s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 64);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0xbd7125d98d41f5a9ULL << 64 | 0x5d11c5792de19549ULL);
  fpr->v1 = ((__uint128_t)0x7d31e5994d01b569ULL << 64 | 0x1dd18539eda15509ULL);
  fpr->v2 = ((__uint128_t)0x3df1a5590dc17529ULL << 64 | 0xdd9145f9ad6115c9ULL);
  fpr->v3 = ((__uint128_t)0xfdb16519cd8135e9ULL << 64 | 0x9d5105b96d21d589ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0x5d11c5792de19549ULL);
  CHECK(*(uint64_t *)&buf[8] == 0xbd7125d98d41f5a9ULL);
  CHECK(*(uint64_t *)&buf[16] == 0x1dd18539eda15509ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x7d31e5994d01b569ULL);
  CHECK(*(uint64_t *)&buf[32] == 0xdd9145f9ad6115c9ULL);
  CHECK(*(uint64_t *)&buf[40] == 0x3df1a5590dc17529ULL);
  CHECK(*(uint64_t *)&buf[48] == 0x9d5105b96d21d589ULL);
  CHECK(*(uint64_t *)&buf[56] == 0xfdb16519cd8135e9ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1onev2d") {
  const char source[] = "st1 {v0.2d}, [x0]\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xd59a5f24e9ae7338ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xad7237fcc1864b10ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Onev2d", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Onev2d", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0xad7237fcc1864b10ULL << 64 | 0xd59a5f24e9ae7338ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == 0xd59a5f24e9ae7338ULL);
  CHECK(*(uint64_t *)&buf[8] == 0xad7237fcc1864b10ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1onev2d_post") {
  const char source[] = "st1 {v0.2d}, [x0], #16\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xd59a5f24e9ae7338ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xad7237fcc1864b10ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Onev2d_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Onev2d_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Onev2d_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 16);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0xad7237fcc1864b10ULL << 64 | 0xd59a5f24e9ae7338ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0xd59a5f24e9ae7338ULL);
  CHECK(*(uint64_t *)&buf[8] == 0xad7237fcc1864b10ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1twov2d") {
  const char source[] = "st1 {v0.2d, v1.2d}, [x0]\n";

  uint8_t buf[32] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x0dcb894705c3813fULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1ddb995715d3914fULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x2deba96725e3a15fULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x3dfbb97735f3b16fULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Twov2d", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Twov2d", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x1ddb995715d3914fULL << 64 | 0x0dcb894705c3813fULL);
  fpr->v1 = ((__uint128_t)0x3dfbb97735f3b16fULL << 64 | 0x2deba96725e3a15fULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == 0x0dcb894705c3813fULL);
  CHECK(*(uint64_t *)&buf[8] == 0x1ddb995715d3914fULL);
  CHECK(*(uint64_t *)&buf[16] == 0x2deba96725e3a15fULL);
  CHECK(*(uint64_t *)&buf[24] == 0x3dfbb97735f3b16fULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1twov2d_post") {
  const char source[] = "st1 {v0.2d, v1.2d}, [x0], #32\n";

  uint8_t buf[32] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x0dcb894705c3813fULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1ddb995715d3914fULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x2deba96725e3a15fULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x3dfbb97735f3b16fULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Twov2d_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Twov2d_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Twov2d_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 32);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x1ddb995715d3914fULL << 64 | 0x0dcb894705c3813fULL);
  fpr->v1 = ((__uint128_t)0x3dfbb97735f3b16fULL << 64 | 0x2deba96725e3a15fULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0x0dcb894705c3813fULL);
  CHECK(*(uint64_t *)&buf[8] == 0x1ddb995715d3914fULL);
  CHECK(*(uint64_t *)&buf[16] == 0x2deba96725e3a15fULL);
  CHECK(*(uint64_t *)&buf[24] == 0x3dfbb97735f3b16fULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1threev2d") {
  const char source[] = "st1 {v0.2d, v1.2d, v2.2d}, [x0]\n";

  uint8_t buf[48] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x45fcb36a21d88f46ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x8d44fbb26920d78eULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xd58c43fab1681fd6ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x1dd48b42f9b0671eULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x651cd38a41f8af66ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xad641bd28940f7aeULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Threev2d", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Threev2d", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x8d44fbb26920d78eULL << 64 | 0x45fcb36a21d88f46ULL);
  fpr->v1 = ((__uint128_t)0x1dd48b42f9b0671eULL << 64 | 0xd58c43fab1681fd6ULL);
  fpr->v2 = ((__uint128_t)0xad641bd28940f7aeULL << 64 | 0x651cd38a41f8af66ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == 0x45fcb36a21d88f46ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x8d44fbb26920d78eULL);
  CHECK(*(uint64_t *)&buf[16] == 0xd58c43fab1681fd6ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x1dd48b42f9b0671eULL);
  CHECK(*(uint64_t *)&buf[32] == 0x651cd38a41f8af66ULL);
  CHECK(*(uint64_t *)&buf[40] == 0xad641bd28940f7aeULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1threev2d_post") {
  const char source[] = "st1 {v0.2d, v1.2d, v2.2d}, [x0], #48\n";

  uint8_t buf[48] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x45fcb36a21d88f46ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x8d44fbb26920d78eULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xd58c43fab1681fd6ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x1dd48b42f9b0671eULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x651cd38a41f8af66ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xad641bd28940f7aeULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Threev2d_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("ST1Threev2d_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Threev2d_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 48);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x8d44fbb26920d78eULL << 64 | 0x45fcb36a21d88f46ULL);
  fpr->v1 = ((__uint128_t)0x1dd48b42f9b0671eULL << 64 | 0xd58c43fab1681fd6ULL);
  fpr->v2 = ((__uint128_t)0xad641bd28940f7aeULL << 64 | 0x651cd38a41f8af66ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0x45fcb36a21d88f46ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x8d44fbb26920d78eULL);
  CHECK(*(uint64_t *)&buf[16] == 0xd58c43fab1681fd6ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x1dd48b42f9b0671eULL);
  CHECK(*(uint64_t *)&buf[32] == 0x651cd38a41f8af66ULL);
  CHECK(*(uint64_t *)&buf[40] == 0xad641bd28940f7aeULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1fourv2d") {
  const char source[] = "st1 {v0.2d, v1.2d, v2.2d, v3.2d}, [x0]\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x7d2ddd8d3ded9d4dULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xfdad5d0dbd6d1dcdULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x7d2ddd8d3ded9d4dULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xfdad5d0dbd6d1dcdULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x7d2ddd8d3ded9d4dULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xfdad5d0dbd6d1dcdULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x7d2ddd8d3ded9d4dULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0xfdad5d0dbd6d1dcdULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Fourv2d", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Fourv2d", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0xfdad5d0dbd6d1dcdULL << 64 | 0x7d2ddd8d3ded9d4dULL);
  fpr->v1 = ((__uint128_t)0xfdad5d0dbd6d1dcdULL << 64 | 0x7d2ddd8d3ded9d4dULL);
  fpr->v2 = ((__uint128_t)0xfdad5d0dbd6d1dcdULL << 64 | 0x7d2ddd8d3ded9d4dULL);
  fpr->v3 = ((__uint128_t)0xfdad5d0dbd6d1dcdULL << 64 | 0x7d2ddd8d3ded9d4dULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == 0x7d2ddd8d3ded9d4dULL);
  CHECK(*(uint64_t *)&buf[8] == 0xfdad5d0dbd6d1dcdULL);
  CHECK(*(uint64_t *)&buf[16] == 0x7d2ddd8d3ded9d4dULL);
  CHECK(*(uint64_t *)&buf[24] == 0xfdad5d0dbd6d1dcdULL);
  CHECK(*(uint64_t *)&buf[32] == 0x7d2ddd8d3ded9d4dULL);
  CHECK(*(uint64_t *)&buf[40] == 0xfdad5d0dbd6d1dcdULL);
  CHECK(*(uint64_t *)&buf[48] == 0x7d2ddd8d3ded9d4dULL);
  CHECK(*(uint64_t *)&buf[56] == 0xfdad5d0dbd6d1dcdULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1fourv2d_post") {
  const char source[] = "st1 {v0.2d, v1.2d, v2.2d, v3.2d}, [x0], #64\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x7d2ddd8d3ded9d4dULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xfdad5d0dbd6d1dcdULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x7d2ddd8d3ded9d4dULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xfdad5d0dbd6d1dcdULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x7d2ddd8d3ded9d4dULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xfdad5d0dbd6d1dcdULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x7d2ddd8d3ded9d4dULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0xfdad5d0dbd6d1dcdULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Fourv2d_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Fourv2d_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Fourv2d_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 64);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0xfdad5d0dbd6d1dcdULL << 64 | 0x7d2ddd8d3ded9d4dULL);
  fpr->v1 = ((__uint128_t)0xfdad5d0dbd6d1dcdULL << 64 | 0x7d2ddd8d3ded9d4dULL);
  fpr->v2 = ((__uint128_t)0xfdad5d0dbd6d1dcdULL << 64 | 0x7d2ddd8d3ded9d4dULL);
  fpr->v3 = ((__uint128_t)0xfdad5d0dbd6d1dcdULL << 64 | 0x7d2ddd8d3ded9d4dULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0x7d2ddd8d3ded9d4dULL);
  CHECK(*(uint64_t *)&buf[8] == 0xfdad5d0dbd6d1dcdULL);
  CHECK(*(uint64_t *)&buf[16] == 0x7d2ddd8d3ded9d4dULL);
  CHECK(*(uint64_t *)&buf[24] == 0xfdad5d0dbd6d1dcdULL);
  CHECK(*(uint64_t *)&buf[32] == 0x7d2ddd8d3ded9d4dULL);
  CHECK(*(uint64_t *)&buf[40] == 0xfdad5d0dbd6d1dcdULL);
  CHECK(*(uint64_t *)&buf[48] == 0x7d2ddd8d3ded9d4dULL);
  CHECK(*(uint64_t *)&buf[56] == 0xfdad5d0dbd6d1dcdULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st2twov16b") {
  const char source[] = "st2 {v0.16b, v1.16b}, [x0]\n";

  uint8_t buf[32] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x9ddde929357581c1ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x6dadb9f905455191ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x3d7d89c9d5152161ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x0d4d5999a5e5f131ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST2Twov16b", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST2Twov16b", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x4d99e5317dc91561ULL << 64 | 0xadf94591dd2975c1ULL);
  fpr->v1 = ((__uint128_t)0x0d59a5f13d89d521ULL << 64 | 0x6db905519de93581ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == 0x9ddde929357581c1ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x6dadb9f905455191ULL);
  CHECK(*(uint64_t *)&buf[16] == 0x3d7d89c9d5152161ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x0d4d5999a5e5f131ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st2twov16b_post") {
  const char source[] = "st2 {v0.16b, v1.16b}, [x0], #32\n";

  uint8_t buf[32] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x9ddde929357581c1ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x6dadb9f905455191ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x3d7d89c9d5152161ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x0d4d5999a5e5f131ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST2Twov16b_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST2Twov16b_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST2Twov16b_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 32);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x4d99e5317dc91561ULL << 64 | 0xadf94591dd2975c1ULL);
  fpr->v1 = ((__uint128_t)0x0d59a5f13d89d521ULL << 64 | 0x6db905519de93581ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0x9ddde929357581c1ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x6dadb9f905455191ULL);
  CHECK(*(uint64_t *)&buf[16] == 0x3d7d89c9d5152161ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x0d4d5999a5e5f131ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st3threev16b") {
  const char source[] = "st3 {v0.16b, v1.16b, v2.16b}, [x0]\n";

  uint8_t buf[48] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x33c3fa8a1a51e171ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xbef585154cdc6ca3ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xf0801047d7679e2eULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x7b0b42d2629929b9ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x063dcd5d9424b4ebULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x38c8588f1fafe676ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST3Threev16b", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST3Threev16b", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x58af065db40b62b9ULL << 64 | 0x1067be156cc31a71ULL);
  fpr->v1 = ((__uint128_t)0xc81f76cd247bd229ULL << 64 | 0x80d72e85dc338ae1ULL);
  fpr->v2 = ((__uint128_t)0x388fe63d94eb4299ULL << 64 | 0xf0479ef54ca3fa51ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == 0x33c3fa8a1a51e171ULL);
  CHECK(*(uint64_t *)&buf[8] == 0xbef585154cdc6ca3ULL);
  CHECK(*(uint64_t *)&buf[16] == 0xf0801047d7679e2eULL);
  CHECK(*(uint64_t *)&buf[24] == 0x7b0b42d2629929b9ULL);
  CHECK(*(uint64_t *)&buf[32] == 0x063dcd5d9424b4ebULL);
  CHECK(*(uint64_t *)&buf[40] == 0x38c8588f1fafe676ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st3threev16b_post") {
  const char source[] = "st3 {v0.16b, v1.16b, v2.16b}, [x0], #48\n";

  uint8_t buf[48] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x33c3fa8a1a51e171ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xbef585154cdc6ca3ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xf0801047d7679e2eULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x7b0b42d2629929b9ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x063dcd5d9424b4ebULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x38c8588f1fafe676ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST3Threev16b_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("ST3Threev16b_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST3Threev16b_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 48);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x58af065db40b62b9ULL << 64 | 0x1067be156cc31a71ULL);
  fpr->v1 = ((__uint128_t)0xc81f76cd247bd229ULL << 64 | 0x80d72e85dc338ae1ULL);
  fpr->v2 = ((__uint128_t)0x388fe63d94eb4299ULL << 64 | 0xf0479ef54ca3fa51ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0x33c3fa8a1a51e171ULL);
  CHECK(*(uint64_t *)&buf[8] == 0xbef585154cdc6ca3ULL);
  CHECK(*(uint64_t *)&buf[16] == 0xf0801047d7679e2eULL);
  CHECK(*(uint64_t *)&buf[24] == 0x7b0b42d2629929b9ULL);
  CHECK(*(uint64_t *)&buf[32] == 0x063dcd5d9424b4ebULL);
  CHECK(*(uint64_t *)&buf[40] == 0x38c8588f1fafe676ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st4fourv16b") {
  const char source[] = "st4 {v0.16b, v1.16b, v2.16b, v3.16b}, [x0]\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x1fffdfbf81614121ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x5b3b1bfbbd9d7d5dULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x97775737f9d9b999ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xd3b393733515f5d5ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x0fefcfaf71513111ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x4b2b0bebad8d6d4dULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x87674727e9c9a989ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0xc3a383632505e5c5ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST4Fourv16b", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST4Fourv16b", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x63c52789eb4daf11ULL << 64 | 0x73d53799fb5dbf21ULL);
  fpr->v1 = ((__uint128_t)0x83e547a90b6dcf31ULL << 64 | 0x93f557b91b7ddf41ULL);
  fpr->v2 = ((__uint128_t)0xa30567c92b8def51ULL << 64 | 0xb31577d93b9dff61ULL);
  fpr->v3 = ((__uint128_t)0xc32587e94bad0f71ULL << 64 | 0xd33597f95bbd1f81ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == 0x1fffdfbf81614121ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x5b3b1bfbbd9d7d5dULL);
  CHECK(*(uint64_t *)&buf[16] == 0x97775737f9d9b999ULL);
  CHECK(*(uint64_t *)&buf[24] == 0xd3b393733515f5d5ULL);
  CHECK(*(uint64_t *)&buf[32] == 0x0fefcfaf71513111ULL);
  CHECK(*(uint64_t *)&buf[40] == 0x4b2b0bebad8d6d4dULL);
  CHECK(*(uint64_t *)&buf[48] == 0x87674727e9c9a989ULL);
  CHECK(*(uint64_t *)&buf[56] == 0xc3a383632505e5c5ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st4fourv16b_post") {
  const char source[] = "st4 {v0.16b, v1.16b, v2.16b, v3.16b}, [x0], #64\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x1fffdfbf81614121ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x5b3b1bfbbd9d7d5dULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x97775737f9d9b999ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xd3b393733515f5d5ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x0fefcfaf71513111ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x4b2b0bebad8d6d4dULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x87674727e9c9a989ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0xc3a383632505e5c5ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST4Fourv16b_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("ST4Fourv16b_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST4Fourv16b_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 64);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x63c52789eb4daf11ULL << 64 | 0x73d53799fb5dbf21ULL);
  fpr->v1 = ((__uint128_t)0x83e547a90b6dcf31ULL << 64 | 0x93f557b91b7ddf41ULL);
  fpr->v2 = ((__uint128_t)0xa30567c92b8def51ULL << 64 | 0xb31577d93b9dff61ULL);
  fpr->v3 = ((__uint128_t)0xc32587e94bad0f71ULL << 64 | 0xd33597f95bbd1f81ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0x1fffdfbf81614121ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x5b3b1bfbbd9d7d5dULL);
  CHECK(*(uint64_t *)&buf[16] == 0x97775737f9d9b999ULL);
  CHECK(*(uint64_t *)&buf[24] == 0xd3b393733515f5d5ULL);
  CHECK(*(uint64_t *)&buf[32] == 0x0fefcfaf71513111ULL);
  CHECK(*(uint64_t *)&buf[40] == 0x4b2b0bebad8d6d4dULL);
  CHECK(*(uint64_t *)&buf[48] == 0x87674727e9c9a989ULL);
  CHECK(*(uint64_t *)&buf[56] == 0xc3a383632505e5c5ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st2twov8h") {
  const char source[] = "st2 {v0.8h, v1.8h}, [x0]\n";

  uint8_t buf[32] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x87841f1cd4d16c69ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xedea85823a37d2cfULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x5350ebe8a09d3835ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xb9b6514e06039e9bULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST2Twov8h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST2Twov8h", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x514e9e9bebe83835ULL << 64 | 0x8582d2cf1f1c6c69ULL);
  fpr->v1 = ((__uint128_t)0xb9b606035350a09dULL << 64 | 0xedea3a378784d4d1ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == 0x87841f1cd4d16c69ULL);
  CHECK(*(uint64_t *)&buf[8] == 0xedea85823a37d2cfULL);
  CHECK(*(uint64_t *)&buf[16] == 0x5350ebe8a09d3835ULL);
  CHECK(*(uint64_t *)&buf[24] == 0xb9b6514e06039e9bULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st2twov8h_post") {
  const char source[] = "st2 {v0.8h, v1.8h}, [x0], #32\n";

  uint8_t buf[32] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x87841f1cd4d16c69ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xedea85823a37d2cfULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x5350ebe8a09d3835ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xb9b6514e06039e9bULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST2Twov8h_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST2Twov8h_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST2Twov8h_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 32);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x514e9e9bebe83835ULL << 64 | 0x8582d2cf1f1c6c69ULL);
  fpr->v1 = ((__uint128_t)0xb9b606035350a09dULL << 64 | 0xedea3a378784d4d1ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0x87841f1cd4d16c69ULL);
  CHECK(*(uint64_t *)&buf[8] == 0xedea85823a37d2cfULL);
  CHECK(*(uint64_t *)&buf[16] == 0x5350ebe8a09d3835ULL);
  CHECK(*(uint64_t *)&buf[24] == 0xb9b6514e06039e9bULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st3threev8h") {
  const char source[] = "st3 {v0.8h, v1.8h, v2.8h}, [x0]\n";

  uint8_t buf[48] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x6c6944418481c4c1ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xd4d11411ece92c29ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x3c397c79bcb99491ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x0c09e4e124216461ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x7471b4b18c89ccc9ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xdcd91c195c593431ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST3Threev8h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST3Threev8h", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x5c59b4b10c096461ULL << 64 | 0xbcb914116c69c4c1ULL);
  fpr->v1 = ((__uint128_t)0x1c197471ccc92421ULL << 64 | 0x7c79d4d12c298481ULL);
  fpr->v2 = ((__uint128_t)0xdcd934318c89e4e1ULL << 64 | 0x3c399491ece94441ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == 0x6c6944418481c4c1ULL);
  CHECK(*(uint64_t *)&buf[8] == 0xd4d11411ece92c29ULL);
  CHECK(*(uint64_t *)&buf[16] == 0x3c397c79bcb99491ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x0c09e4e124216461ULL);
  CHECK(*(uint64_t *)&buf[32] == 0x7471b4b18c89ccc9ULL);
  CHECK(*(uint64_t *)&buf[40] == 0xdcd91c195c593431ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st3threev8h_post") {
  const char source[] = "st3 {v0.8h, v1.8h, v2.8h}, [x0], #48\n";

  uint8_t buf[48] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x6c6944418481c4c1ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xd4d11411ece92c29ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x3c397c79bcb99491ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x0c09e4e124216461ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x7471b4b18c89ccc9ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xdcd91c195c593431ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST3Threev8h_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("ST3Threev8h_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST3Threev8h_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 48);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x5c59b4b10c096461ULL << 64 | 0xbcb914116c69c4c1ULL);
  fpr->v1 = ((__uint128_t)0x1c197471ccc92421ULL << 64 | 0x7c79d4d12c298481ULL);
  fpr->v2 = ((__uint128_t)0xdcd934318c89e4e1ULL << 64 | 0x3c399491ece94441ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0x6c6944418481c4c1ULL);
  CHECK(*(uint64_t *)&buf[8] == 0xd4d11411ece92c29ULL);
  CHECK(*(uint64_t *)&buf[16] == 0x3c397c79bcb99491ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x0c09e4e124216461ULL);
  CHECK(*(uint64_t *)&buf[32] == 0x7471b4b18c89ccc9ULL);
  CHECK(*(uint64_t *)&buf[40] == 0xdcd91c195c593431ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st4fourv8h") {
  const char source[] = "st4 {v0.8h, v1.8h, v2.8h, v3.8h}, [x0]\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x64614c4934311c19ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x01fee9e6d1ceb9b6ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x9e9b86836e6b5653ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x3b3823200b08f3f0ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0xd8d5c0bda8a5908dULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x75725d5a45422d2aULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x120ffaf7e2dfcac7ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0xafac97947f7c6764ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST4Fourv8h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST4Fourv8h", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x6764cac72d2a908dULL << 64 | 0xf3f05653b9b61c19ULL);
  fpr->v1 = ((__uint128_t)0x7f7ce2df4542a8a5ULL << 64 | 0x0b086e6bd1ce3431ULL);
  fpr->v2 = ((__uint128_t)0x9794faf75d5ac0bdULL << 64 | 0x23208683e9e64c49ULL);
  fpr->v3 = ((__uint128_t)0xafac120f7572d8d5ULL << 64 | 0x3b389e9b01fe6461ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == 0x64614c4934311c19ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x01fee9e6d1ceb9b6ULL);
  CHECK(*(uint64_t *)&buf[16] == 0x9e9b86836e6b5653ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x3b3823200b08f3f0ULL);
  CHECK(*(uint64_t *)&buf[32] == 0xd8d5c0bda8a5908dULL);
  CHECK(*(uint64_t *)&buf[40] == 0x75725d5a45422d2aULL);
  CHECK(*(uint64_t *)&buf[48] == 0x120ffaf7e2dfcac7ULL);
  CHECK(*(uint64_t *)&buf[56] == 0xafac97947f7c6764ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st4fourv8h_post") {
  const char source[] = "st4 {v0.8h, v1.8h, v2.8h, v3.8h}, [x0], #64\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x64614c4934311c19ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x01fee9e6d1ceb9b6ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x9e9b86836e6b5653ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x3b3823200b08f3f0ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0xd8d5c0bda8a5908dULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x75725d5a45422d2aULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x120ffaf7e2dfcac7ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0xafac97947f7c6764ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST4Fourv8h_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST4Fourv8h_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST4Fourv8h_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 64);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x6764cac72d2a908dULL << 64 | 0xf3f05653b9b61c19ULL);
  fpr->v1 = ((__uint128_t)0x7f7ce2df4542a8a5ULL << 64 | 0x0b086e6bd1ce3431ULL);
  fpr->v2 = ((__uint128_t)0x9794faf75d5ac0bdULL << 64 | 0x23208683e9e64c49ULL);
  fpr->v3 = ((__uint128_t)0xafac120f7572d8d5ULL << 64 | 0x3b389e9b01fe6461ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0x64614c4934311c19ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x01fee9e6d1ceb9b6ULL);
  CHECK(*(uint64_t *)&buf[16] == 0x9e9b86836e6b5653ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x3b3823200b08f3f0ULL);
  CHECK(*(uint64_t *)&buf[32] == 0xd8d5c0bda8a5908dULL);
  CHECK(*(uint64_t *)&buf[40] == 0x75725d5a45422d2aULL);
  CHECK(*(uint64_t *)&buf[48] == 0x120ffaf7e2dfcac7ULL);
  CHECK(*(uint64_t *)&buf[56] == 0xafac97947f7c6764ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st2twov4s") {
  const char source[] = "st2 {v0.4s, v1.4s}, [x0]\n";

  uint8_t buf[32] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x827f7c794643403dULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x33302d2af7f4f1eeULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xe4e1dedba8a5a29fULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x95928f8c59565350ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST2Twov4s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST2Twov4s", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x59565350a8a5a29fULL << 64 | 0xf7f4f1ee4643403dULL);
  fpr->v1 = ((__uint128_t)0x95928f8ce4e1dedbULL << 64 | 0x33302d2a827f7c79ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == 0x827f7c794643403dULL);
  CHECK(*(uint64_t *)&buf[8] == 0x33302d2af7f4f1eeULL);
  CHECK(*(uint64_t *)&buf[16] == 0xe4e1dedba8a5a29fULL);
  CHECK(*(uint64_t *)&buf[24] == 0x95928f8c59565350ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st2twov4s_post") {
  const char source[] = "st2 {v0.4s, v1.4s}, [x0], #32\n";

  uint8_t buf[32] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x827f7c794643403dULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x33302d2af7f4f1eeULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xe4e1dedba8a5a29fULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x95928f8c59565350ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST2Twov4s_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST2Twov4s_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST2Twov4s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 32);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x59565350a8a5a29fULL << 64 | 0xf7f4f1ee4643403dULL);
  fpr->v1 = ((__uint128_t)0x95928f8ce4e1dedbULL << 64 | 0x33302d2a827f7c79ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0x827f7c794643403dULL);
  CHECK(*(uint64_t *)&buf[8] == 0x33302d2af7f4f1eeULL);
  CHECK(*(uint64_t *)&buf[16] == 0xe4e1dedba8a5a29fULL);
  CHECK(*(uint64_t *)&buf[24] == 0x95928f8c59565350ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st3threev4s") {
  const char source[] = "st3 {v0.4s, v1.4s, v2.4s}, [x0]\n";

  uint8_t buf[48] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xdad7d4d1726f6c69ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1815120f423f3c39ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xe8e5e2df807d7a77ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x2623201dbebbb8b5ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x64615e5b8e8b8885ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x34312e2bccc9c6c3ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST3Threev4s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST3Threev4s", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x64615e5bbebbb8b5ULL << 64 | 0x1815120f726f6c69ULL);
  fpr->v1 = ((__uint128_t)0xccc9c6c32623201dULL << 64 | 0x807d7a77dad7d4d1ULL);
  fpr->v2 = ((__uint128_t)0x34312e2b8e8b8885ULL << 64 | 0xe8e5e2df423f3c39ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == 0xdad7d4d1726f6c69ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x1815120f423f3c39ULL);
  CHECK(*(uint64_t *)&buf[16] == 0xe8e5e2df807d7a77ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x2623201dbebbb8b5ULL);
  CHECK(*(uint64_t *)&buf[32] == 0x64615e5b8e8b8885ULL);
  CHECK(*(uint64_t *)&buf[40] == 0x34312e2bccc9c6c3ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st3threev4s_post") {
  const char source[] = "st3 {v0.4s, v1.4s, v2.4s}, [x0], #48\n";

  uint8_t buf[48] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xdad7d4d1726f6c69ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1815120f423f3c39ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xe8e5e2df807d7a77ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x2623201dbebbb8b5ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x64615e5b8e8b8885ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x34312e2bccc9c6c3ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST3Threev4s_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("ST3Threev4s_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST3Threev4s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 48);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x64615e5bbebbb8b5ULL << 64 | 0x1815120f726f6c69ULL);
  fpr->v1 = ((__uint128_t)0xccc9c6c32623201dULL << 64 | 0x807d7a77dad7d4d1ULL);
  fpr->v2 = ((__uint128_t)0x34312e2b8e8b8885ULL << 64 | 0xe8e5e2df423f3c39ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0xdad7d4d1726f6c69ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x1815120f423f3c39ULL);
  CHECK(*(uint64_t *)&buf[16] == 0xe8e5e2df807d7a77ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x2623201dbebbb8b5ULL);
  CHECK(*(uint64_t *)&buf[32] == 0x64615e5b8e8b8885ULL);
  CHECK(*(uint64_t *)&buf[40] == 0x34312e2bccc9c6c3ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st4fourv4s") {
  const char source[] = "st4 {v0.4s, v1.4s, v2.4s, v3.4s}, [x0]\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x322f2c299e9b9895ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x5a575451c6c3c0bdULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xcdcac7c439363330ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xf5f2efec615e5b58ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x6865625fd4d1cecbULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x908d8a87fcf9f6f3ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x035afdfa6f6c6966ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x2b2825229794918eULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST4Fourv4s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST4Fourv4s", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x6f6c6966d4d1cecbULL << 64 | 0x393633309e9b9895ULL);
  fpr->v1 = ((__uint128_t)0x035afdfa6865625fULL << 64 | 0xcdcac7c4322f2c29ULL);
  fpr->v2 = ((__uint128_t)0x9794918efcf9f6f3ULL << 64 | 0x615e5b58c6c3c0bdULL);
  fpr->v3 = ((__uint128_t)0x2b282522908d8a87ULL << 64 | 0xf5f2efec5a575451ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == 0x322f2c299e9b9895ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x5a575451c6c3c0bdULL);
  CHECK(*(uint64_t *)&buf[16] == 0xcdcac7c439363330ULL);
  CHECK(*(uint64_t *)&buf[24] == 0xf5f2efec615e5b58ULL);
  CHECK(*(uint64_t *)&buf[32] == 0x6865625fd4d1cecbULL);
  CHECK(*(uint64_t *)&buf[40] == 0x908d8a87fcf9f6f3ULL);
  CHECK(*(uint64_t *)&buf[48] == 0x035afdfa6f6c6966ULL);
  CHECK(*(uint64_t *)&buf[56] == 0x2b2825229794918eULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st4fourv4s_post") {
  const char source[] = "st4 {v0.4s, v1.4s, v2.4s, v3.4s}, [x0], #64\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x322f2c299e9b9895ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x5a575451c6c3c0bdULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0xcdcac7c439363330ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0xf5f2efec615e5b58ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x6865625fd4d1cecbULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x908d8a87fcf9f6f3ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x035afdfa6f6c6966ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x2b2825229794918eULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST4Fourv4s_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST4Fourv4s_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST4Fourv4s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 64);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x6f6c6966d4d1cecbULL << 64 | 0x393633309e9b9895ULL);
  fpr->v1 = ((__uint128_t)0x035afdfa6865625fULL << 64 | 0xcdcac7c4322f2c29ULL);
  fpr->v2 = ((__uint128_t)0x9794918efcf9f6f3ULL << 64 | 0x615e5b58c6c3c0bdULL);
  fpr->v3 = ((__uint128_t)0x2b282522908d8a87ULL << 64 | 0xf5f2efec5a575451ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0x322f2c299e9b9895ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x5a575451c6c3c0bdULL);
  CHECK(*(uint64_t *)&buf[16] == 0xcdcac7c439363330ULL);
  CHECK(*(uint64_t *)&buf[24] == 0xf5f2efec615e5b58ULL);
  CHECK(*(uint64_t *)&buf[32] == 0x6865625fd4d1cecbULL);
  CHECK(*(uint64_t *)&buf[40] == 0x908d8a87fcf9f6f3ULL);
  CHECK(*(uint64_t *)&buf[48] == 0x035afdfa6f6c6966ULL);
  CHECK(*(uint64_t *)&buf[56] == 0x2b2825229794918eULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st2twov2d") {
  const char source[] = "st2 {v0.2d, v1.2d}, [x0]\n";

  uint8_t buf[32] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xbcb9b6b3b0adaaa7ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x625f5c595653504dULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x696663605d5a5754ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x0f0c0906035afdfaULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST2Twov2d", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST2Twov2d", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x696663605d5a5754ULL << 64 | 0xbcb9b6b3b0adaaa7ULL);
  fpr->v1 = ((__uint128_t)0x0f0c0906035afdfaULL << 64 | 0x625f5c595653504dULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == 0xbcb9b6b3b0adaaa7ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x625f5c595653504dULL);
  CHECK(*(uint64_t *)&buf[16] == 0x696663605d5a5754ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x0f0c0906035afdfaULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st2twov2d_post") {
  const char source[] = "st2 {v0.2d, v1.2d}, [x0], #32\n";

  uint8_t buf[32] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xbcb9b6b3b0adaaa7ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x625f5c595653504dULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x696663605d5a5754ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x0f0c0906035afdfaULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST2Twov2d_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST2Twov2d_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST2Twov2d_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 32);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x696663605d5a5754ULL << 64 | 0xbcb9b6b3b0adaaa7ULL);
  fpr->v1 = ((__uint128_t)0x0f0c0906035afdfaULL << 64 | 0x625f5c595653504dULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0xbcb9b6b3b0adaaa7ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x625f5c595653504dULL);
  CHECK(*(uint64_t *)&buf[16] == 0x696663605d5a5754ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x0f0c0906035afdfaULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st3threev2d") {
  const char source[] = "st3 {v0.2d, v1.2d, v2.2d}, [x0]\n";

  uint8_t buf[48] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xd2cfccc9c6c3c0bdULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x8e8b8885827f7c79ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x4a4744413e3b3835ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x74716e6b6865625fULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x302d2a2724211e1bULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xece9e6e3e0dddad7ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST3Threev2d", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST3Threev2d", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x74716e6b6865625fULL << 64 | 0xd2cfccc9c6c3c0bdULL);
  fpr->v1 = ((__uint128_t)0x302d2a2724211e1bULL << 64 | 0x8e8b8885827f7c79ULL);
  fpr->v2 = ((__uint128_t)0xece9e6e3e0dddad7ULL << 64 | 0x4a4744413e3b3835ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == 0xd2cfccc9c6c3c0bdULL);
  CHECK(*(uint64_t *)&buf[8] == 0x8e8b8885827f7c79ULL);
  CHECK(*(uint64_t *)&buf[16] == 0x4a4744413e3b3835ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x74716e6b6865625fULL);
  CHECK(*(uint64_t *)&buf[32] == 0x302d2a2724211e1bULL);
  CHECK(*(uint64_t *)&buf[40] == 0xece9e6e3e0dddad7ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st3threev2d_post") {
  const char source[] = "st3 {v0.2d, v1.2d, v2.2d}, [x0], #48\n";

  uint8_t buf[48] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xd2cfccc9c6c3c0bdULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x8e8b8885827f7c79ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x4a4744413e3b3835ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x74716e6b6865625fULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x302d2a2724211e1bULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xece9e6e3e0dddad7ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST3Threev2d_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("ST3Threev2d_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST3Threev2d_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 48);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x74716e6b6865625fULL << 64 | 0xd2cfccc9c6c3c0bdULL);
  fpr->v1 = ((__uint128_t)0x302d2a2724211e1bULL << 64 | 0x8e8b8885827f7c79ULL);
  fpr->v2 = ((__uint128_t)0xece9e6e3e0dddad7ULL << 64 | 0x4a4744413e3b3835ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0xd2cfccc9c6c3c0bdULL);
  CHECK(*(uint64_t *)&buf[8] == 0x8e8b8885827f7c79ULL);
  CHECK(*(uint64_t *)&buf[16] == 0x4a4744413e3b3835ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x74716e6b6865625fULL);
  CHECK(*(uint64_t *)&buf[32] == 0x302d2a2724211e1bULL);
  CHECK(*(uint64_t *)&buf[40] == 0xece9e6e3e0dddad7ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st4fourv2d") {
  const char source[] = "st4 {v0.2d, v1.2d, v2.2d, v3.2d}, [x0]\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xe8e5e2dfdcd9d6d3ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xbab7b4b1aeaba8a5ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x8c898683807d7a77ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x5e5b5855524f4c49ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x7f7c797673706d6aULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x514e4b4845423f3cULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x23201d1a1714110eULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0xf5f2efece9e6e3e0ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST4Fourv2d", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST4Fourv2d", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x7f7c797673706d6aULL << 64 | 0xe8e5e2dfdcd9d6d3ULL);
  fpr->v1 = ((__uint128_t)0x514e4b4845423f3cULL << 64 | 0xbab7b4b1aeaba8a5ULL);
  fpr->v2 = ((__uint128_t)0x23201d1a1714110eULL << 64 | 0x8c898683807d7a77ULL);
  fpr->v3 = ((__uint128_t)0xf5f2efece9e6e3e0ULL << 64 | 0x5e5b5855524f4c49ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == 0xe8e5e2dfdcd9d6d3ULL);
  CHECK(*(uint64_t *)&buf[8] == 0xbab7b4b1aeaba8a5ULL);
  CHECK(*(uint64_t *)&buf[16] == 0x8c898683807d7a77ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x5e5b5855524f4c49ULL);
  CHECK(*(uint64_t *)&buf[32] == 0x7f7c797673706d6aULL);
  CHECK(*(uint64_t *)&buf[40] == 0x514e4b4845423f3cULL);
  CHECK(*(uint64_t *)&buf[48] == 0x23201d1a1714110eULL);
  CHECK(*(uint64_t *)&buf[56] == 0xf5f2efece9e6e3e0ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st4fourv2d_post") {
  const char source[] = "st4 {v0.2d, v1.2d, v2.2d, v3.2d}, [x0], #64\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xe8e5e2dfdcd9d6d3ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xbab7b4b1aeaba8a5ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x8c898683807d7a77ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x5e5b5855524f4c49ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[32], 0x7f7c797673706d6aULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x514e4b4845423f3cULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x23201d1a1714110eULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0xf5f2efece9e6e3e0ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST4Fourv2d_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST4Fourv2d_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST4Fourv2d_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 64);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x7f7c797673706d6aULL << 64 | 0xe8e5e2dfdcd9d6d3ULL);
  fpr->v1 = ((__uint128_t)0x514e4b4845423f3cULL << 64 | 0xbab7b4b1aeaba8a5ULL);
  fpr->v2 = ((__uint128_t)0x23201d1a1714110eULL << 64 | 0x8c898683807d7a77ULL);
  fpr->v3 = ((__uint128_t)0xf5f2efece9e6e3e0ULL << 64 | 0x5e5b5855524f4c49ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0xe8e5e2dfdcd9d6d3ULL);
  CHECK(*(uint64_t *)&buf[8] == 0xbab7b4b1aeaba8a5ULL);
  CHECK(*(uint64_t *)&buf[16] == 0x8c898683807d7a77ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x5e5b5855524f4c49ULL);
  CHECK(*(uint64_t *)&buf[32] == 0x7f7c797673706d6aULL);
  CHECK(*(uint64_t *)&buf[40] == 0x514e4b4845423f3cULL);
  CHECK(*(uint64_t *)&buf[48] == 0x23201d1a1714110eULL);
  CHECK(*(uint64_t *)&buf[56] == 0xf5f2efece9e6e3e0ULL);
}
