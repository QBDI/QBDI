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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrqrow_uxtw_s0") {
  const char source[] = "ldr q1, [x0, w3, uxtw]\n";

  uint8_t buf[64] = {0};
  *(uint64_t *)&buf[32] = 0x0123456789abcdefULL;
  *(uint64_t *)&buf[40] = 0xfedcba9876543210ULL;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[32], 0x0123456789abcdefULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xfedcba9876543210ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[32], 0x0123456789abcdefULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xfedcba9876543210ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRQroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRQroW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRQroW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0xfedcba9876543210ULL << 64 |
                            0x0123456789abcdefULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 32;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrqrow_uxtw_s4") {
  const char source[] = "ldr q1, [x0, w3, uxtw #4]\n";

  uint8_t buf[64] = {0};
  *(uint64_t *)&buf[32] = 0x0123456789abcdefULL;
  *(uint64_t *)&buf[40] = 0xfedcba9876543210ULL;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[32], 0x0123456789abcdefULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xfedcba9876543210ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[32], 0x0123456789abcdefULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xfedcba9876543210ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRQroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRQroW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRQroW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0xfedcba9876543210ULL << 64 |
                            0x0123456789abcdefULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 2;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrqrow_sxtw_s0") {
  const char source[] = "ldr q1, [x0, w3, sxtw]\n";

  uint8_t buf[64] = {0};
  *(uint64_t *)&buf[32] = 0x0123456789abcdefULL;
  *(uint64_t *)&buf[40] = 0xfedcba9876543210ULL;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[32], 0x0123456789abcdefULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xfedcba9876543210ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[32], 0x0123456789abcdefULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xfedcba9876543210ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRQroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRQroW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRQroW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0xfedcba9876543210ULL << 64 |
                            0x0123456789abcdefULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 32;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrqrow_sxtw_s4") {
  const char source[] = "ldr q1, [x0, w3, sxtw #4]\n";

  uint8_t buf[64] = {0};
  *(uint64_t *)&buf[32] = 0x0123456789abcdefULL;
  *(uint64_t *)&buf[40] = 0xfedcba9876543210ULL;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[32], 0x0123456789abcdefULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xfedcba9876543210ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[32], 0x0123456789abcdefULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xfedcba9876543210ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRQroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRQroW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRQroW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0xfedcba9876543210ULL << 64 |
                            0x0123456789abcdefULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 2;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrqrox_lsl_s0") {
  const char source[] = "ldr q1, [x0, x3, lsl #0]\n";

  uint8_t buf[64] = {0};
  *(uint64_t *)&buf[32] = 0x0123456789abcdefULL;
  *(uint64_t *)&buf[40] = 0xfedcba9876543210ULL;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[32], 0x0123456789abcdefULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xfedcba9876543210ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[32], 0x0123456789abcdefULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xfedcba9876543210ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRQroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRQroX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRQroX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0xfedcba9876543210ULL << 64 |
                            0x0123456789abcdefULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 32;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrqrox_lsl_s4") {
  const char source[] = "ldr q1, [x0, x3, lsl #4]\n";

  uint8_t buf[64] = {0};
  *(uint64_t *)&buf[32] = 0x0123456789abcdefULL;
  *(uint64_t *)&buf[40] = 0xfedcba9876543210ULL;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[32], 0x0123456789abcdefULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xfedcba9876543210ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[32], 0x0123456789abcdefULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xfedcba9876543210ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRQroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRQroX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRQroX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0xfedcba9876543210ULL << 64 |
                            0x0123456789abcdefULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 2;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrqrox_sxtx_s0") {
  const char source[] = "ldr q1, [x0, x3, sxtx]\n";

  uint8_t buf[64] = {0};
  *(uint64_t *)&buf[32] = 0x0123456789abcdefULL;
  *(uint64_t *)&buf[40] = 0xfedcba9876543210ULL;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[32], 0x0123456789abcdefULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xfedcba9876543210ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[32], 0x0123456789abcdefULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xfedcba9876543210ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRQroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRQroX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRQroX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0xfedcba9876543210ULL << 64 |
                            0x0123456789abcdefULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 32;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrqrox_sxtx_s4") {
  const char source[] = "ldr q1, [x0, x3, sxtx #4]\n";

  uint8_t buf[64] = {0};
  *(uint64_t *)&buf[32] = 0x0123456789abcdefULL;
  *(uint64_t *)&buf[40] = 0xfedcba9876543210ULL;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[32], 0x0123456789abcdefULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xfedcba9876543210ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[32], 0x0123456789abcdefULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0xfedcba9876543210ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRQroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRQroX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRQroX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0xfedcba9876543210ULL << 64 |
                            0x0123456789abcdefULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 2;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strqrow_uxtw_s0") {
  const char source[] = "str q1, [x0, w3, uxtw]\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[32], 0x1a2b3c4d5e6f7081ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x91a2b3c4d5e6f708ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRQroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRQroW", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 32;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = ((__uint128_t)0x91a2b3c4d5e6f708ULL << 64 | 0x1a2b3c4d5e6f7081ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[32] == 0x1a2b3c4d5e6f7081ULL);
  CHECK(*(uint64_t *)&buf[40] == 0x91a2b3c4d5e6f708ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strqrow_uxtw_s4") {
  const char source[] = "str q1, [x0, w3, uxtw #4]\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[32], 0x1a2b3c4d5e6f7081ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x91a2b3c4d5e6f708ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRQroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRQroW", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 2;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = ((__uint128_t)0x91a2b3c4d5e6f708ULL << 64 | 0x1a2b3c4d5e6f7081ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[32] == 0x1a2b3c4d5e6f7081ULL);
  CHECK(*(uint64_t *)&buf[40] == 0x91a2b3c4d5e6f708ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strqrow_sxtw_s0") {
  const char source[] = "str q1, [x0, w3, sxtw]\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[32], 0x1a2b3c4d5e6f7081ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x91a2b3c4d5e6f708ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRQroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRQroW", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 32;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = ((__uint128_t)0x91a2b3c4d5e6f708ULL << 64 | 0x1a2b3c4d5e6f7081ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[32] == 0x1a2b3c4d5e6f7081ULL);
  CHECK(*(uint64_t *)&buf[40] == 0x91a2b3c4d5e6f708ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strqrow_sxtw_s4") {
  const char source[] = "str q1, [x0, w3, sxtw #4]\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[32], 0x1a2b3c4d5e6f7081ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x91a2b3c4d5e6f708ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRQroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRQroW", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 2;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = ((__uint128_t)0x91a2b3c4d5e6f708ULL << 64 | 0x1a2b3c4d5e6f7081ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[32] == 0x1a2b3c4d5e6f7081ULL);
  CHECK(*(uint64_t *)&buf[40] == 0x91a2b3c4d5e6f708ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strqrox_lsl_s0") {
  const char source[] = "str q1, [x0, x3, lsl #0]\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[32], 0x1a2b3c4d5e6f7081ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x91a2b3c4d5e6f708ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRQroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRQroX", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 32;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = ((__uint128_t)0x91a2b3c4d5e6f708ULL << 64 | 0x1a2b3c4d5e6f7081ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[32] == 0x1a2b3c4d5e6f7081ULL);
  CHECK(*(uint64_t *)&buf[40] == 0x91a2b3c4d5e6f708ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strqrox_lsl_s4") {
  const char source[] = "str q1, [x0, x3, lsl #4]\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[32], 0x1a2b3c4d5e6f7081ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x91a2b3c4d5e6f708ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRQroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRQroX", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 2;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = ((__uint128_t)0x91a2b3c4d5e6f708ULL << 64 | 0x1a2b3c4d5e6f7081ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[32] == 0x1a2b3c4d5e6f7081ULL);
  CHECK(*(uint64_t *)&buf[40] == 0x91a2b3c4d5e6f708ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strqrox_sxtx_s0") {
  const char source[] = "str q1, [x0, x3, sxtx]\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[32], 0x1a2b3c4d5e6f7081ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x91a2b3c4d5e6f708ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRQroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRQroX", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 32;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = ((__uint128_t)0x91a2b3c4d5e6f708ULL << 64 | 0x1a2b3c4d5e6f7081ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[32] == 0x1a2b3c4d5e6f7081ULL);
  CHECK(*(uint64_t *)&buf[40] == 0x91a2b3c4d5e6f708ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strqrox_sxtx_s4") {
  const char source[] = "str q1, [x0, x3, sxtx #4]\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[32], 0x1a2b3c4d5e6f7081ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x91a2b3c4d5e6f708ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRQroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRQroX", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 2;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = ((__uint128_t)0x91a2b3c4d5e6f708ULL << 64 | 0x1a2b3c4d5e6f7081ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[32] == 0x1a2b3c4d5e6f7081ULL);
  CHECK(*(uint64_t *)&buf[40] == 0x91a2b3c4d5e6f708ULL);
}
