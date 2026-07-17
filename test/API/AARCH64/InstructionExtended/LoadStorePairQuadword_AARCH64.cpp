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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldnpqi") {
  const char source[] = "ldnp q1, q2, [x0, #32]\n";

  uint8_t buf[64] = {0};
  *(uint64_t *)&buf[32] = 0x1122334455667788ULL;
  *(uint64_t *)&buf[40] = 0x99aabbccddeeff00ULL;
  *(uint64_t *)&buf[48] = 0x0102030405060708ULL;
  *(uint64_t *)&buf[56] = 0x1112131415161718ULL;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[32], 0x1122334455667788ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x99aabbccddeeff00ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x0102030405060708ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x1112131415161718ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  expectedPost.see = false;
  bool seenPost = false;
  vm.addMnemonicCB("LDNPQi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDNPQi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDNPQi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x99aabbccddeeff00ULL << 64 |
                            0x1122334455667788ULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x1112131415161718ULL << 64 |
                            0x0102030405060708ULL));
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldpqi") {
  const char source[] = "ldp q1, q2, [x0, #32]\n";

  uint8_t buf[64] = {0};
  *(uint64_t *)&buf[32] = 0x1122334455667788ULL;
  *(uint64_t *)&buf[40] = 0x99aabbccddeeff00ULL;
  *(uint64_t *)&buf[48] = 0x0102030405060708ULL;
  *(uint64_t *)&buf[56] = 0x1112131415161718ULL;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[32], 0x1122334455667788ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x99aabbccddeeff00ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x0102030405060708ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x1112131415161718ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  expectedPost.see = false;
  bool seenPost = false;
  vm.addMnemonicCB("LDPQi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDPQi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDPQi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x99aabbccddeeff00ULL << 64 |
                            0x1122334455667788ULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x1112131415161718ULL << 64 |
                            0x0102030405060708ULL));
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldpqpost") {
  const char source[] = "ldp q1, q2, [x0], #32\n";

  uint8_t buf[64] = {0};
  *(uint64_t *)&buf[0] = 0x1122334455667788ULL;
  *(uint64_t *)&buf[8] = 0x99aabbccddeeff00ULL;
  *(uint64_t *)&buf[16] = 0x0102030405060708ULL;
  *(uint64_t *)&buf[24] = 0x1112131415161718ULL;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x1122334455667788ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x99aabbccddeeff00ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x0102030405060708ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x1112131415161718ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  expectedPost.see = false;
  bool seenPost = false;
  vm.addMnemonicCB("LDPQpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDPQpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDPQpost", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x99aabbccddeeff00ULL << 64 |
                            0x1122334455667788ULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x1112131415161718ULL << 64 |
                            0x0102030405060708ULL));
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldpqpre") {
  const char source[] = "ldp q1, q2, [x0, #32]!\n";

  uint8_t buf[64] = {0};
  *(uint64_t *)&buf[32] = 0x1122334455667788ULL;
  *(uint64_t *)&buf[40] = 0x99aabbccddeeff00ULL;
  *(uint64_t *)&buf[48] = 0x0102030405060708ULL;
  *(uint64_t *)&buf[56] = 0x1112131415161718ULL;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[32], 0x1122334455667788ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x99aabbccddeeff00ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x0102030405060708ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x1112131415161718ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  expectedPost.see = false;
  bool seenPost = false;
  vm.addMnemonicCB("LDPQpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDPQpre", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDPQpre", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x99aabbccddeeff00ULL << 64 |
                            0x1122334455667788ULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x1112131415161718ULL << 64 |
                            0x0102030405060708ULL));
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldtnpqi") {
  if (!checkFeature("lsui")) {
    return;
  }

  const char source[] = "ldtnp q1, q2, [x0, #32]\n";

  uint8_t buf[64] = {0};
  *(uint64_t *)&buf[32] = 0x1122334455667788ULL;
  *(uint64_t *)&buf[40] = 0x99aabbccddeeff00ULL;
  *(uint64_t *)&buf[48] = 0x0102030405060708ULL;
  *(uint64_t *)&buf[56] = 0x1112131415161718ULL;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[32], 0x1122334455667788ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x99aabbccddeeff00ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x0102030405060708ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x1112131415161718ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  expectedPost.see = false;
  bool seenPost = false;
  vm.addMnemonicCB("LDTNPQi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDTNPQi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDTNPQi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x99aabbccddeeff00ULL << 64 |
                            0x1122334455667788ULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x1112131415161718ULL << 64 |
                            0x0102030405060708ULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsui"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldtpqi") {
  if (!checkFeature("lsui")) {
    return;
  }

  const char source[] = "ldtp q1, q2, [x0, #32]\n";

  uint8_t buf[64] = {0};
  *(uint64_t *)&buf[32] = 0x1122334455667788ULL;
  *(uint64_t *)&buf[40] = 0x99aabbccddeeff00ULL;
  *(uint64_t *)&buf[48] = 0x0102030405060708ULL;
  *(uint64_t *)&buf[56] = 0x1112131415161718ULL;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[32], 0x1122334455667788ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x99aabbccddeeff00ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x0102030405060708ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x1112131415161718ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  expectedPost.see = false;
  bool seenPost = false;
  vm.addMnemonicCB("LDTPQi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDTPQi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDTPQi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x99aabbccddeeff00ULL << 64 |
                            0x1122334455667788ULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x1112131415161718ULL << 64 |
                            0x0102030405060708ULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsui"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldtpqpost") {
  if (!checkFeature("lsui")) {
    return;
  }

  const char source[] = "ldtp q1, q2, [x0], #32\n";

  uint8_t buf[64] = {0};
  *(uint64_t *)&buf[0] = 0x1122334455667788ULL;
  *(uint64_t *)&buf[8] = 0x99aabbccddeeff00ULL;
  *(uint64_t *)&buf[16] = 0x0102030405060708ULL;
  *(uint64_t *)&buf[24] = 0x1112131415161718ULL;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x1122334455667788ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x99aabbccddeeff00ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x0102030405060708ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x1112131415161718ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  expectedPost.see = false;
  bool seenPost = false;
  vm.addMnemonicCB("LDTPQpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDTPQpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDTPQpost", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x99aabbccddeeff00ULL << 64 |
                            0x1122334455667788ULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x1112131415161718ULL << 64 |
                            0x0102030405060708ULL));
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 32);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsui"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldtpqpre") {
  if (!checkFeature("lsui")) {
    return;
  }

  const char source[] = "ldtp q1, q2, [x0, #32]!\n";

  uint8_t buf[64] = {0};
  *(uint64_t *)&buf[32] = 0x1122334455667788ULL;
  *(uint64_t *)&buf[40] = 0x99aabbccddeeff00ULL;
  *(uint64_t *)&buf[48] = 0x0102030405060708ULL;
  *(uint64_t *)&buf[56] = 0x1112131415161718ULL;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[32], 0x1122334455667788ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x99aabbccddeeff00ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x0102030405060708ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x1112131415161718ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  expectedPost.see = false;
  bool seenPost = false;
  vm.addMnemonicCB("LDTPQpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDTPQpre", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDTPQpre", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v1 ==
                           ((__uint128_t)0x99aabbccddeeff00ULL << 64 |
                            0x1122334455667788ULL));
                     CHECK((__uint128_t)fprState->v2 ==
                           ((__uint128_t)0x1112131415161718ULL << 64 |
                            0x0102030405060708ULL));
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 32);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsui"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stnpqi") {
  const char source[] = "stnp q1, q2, [x0, #32]\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[32], 0x5555555555555555ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x6666666666666666ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x7777777777777777ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x8888888888888888ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STNPQi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STNPQi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STNPQi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = ((__uint128_t)0x6666666666666666ULL << 64 | 0x5555555555555555ULL);
  fpr->v2 = ((__uint128_t)0x8888888888888888ULL << 64 | 0x7777777777777777ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[32] == 0x5555555555555555ULL);
  CHECK(*(uint64_t *)&buf[40] == 0x6666666666666666ULL);
  CHECK(*(uint64_t *)&buf[48] == 0x7777777777777777ULL);
  CHECK(*(uint64_t *)&buf[56] == 0x8888888888888888ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stpqi") {
  const char source[] = "stp q1, q2, [x0, #32]\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[32], 0x5555555555555555ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x6666666666666666ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x7777777777777777ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x8888888888888888ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STPQi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STPQi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STPQi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = ((__uint128_t)0x6666666666666666ULL << 64 | 0x5555555555555555ULL);
  fpr->v2 = ((__uint128_t)0x8888888888888888ULL << 64 | 0x7777777777777777ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[32] == 0x5555555555555555ULL);
  CHECK(*(uint64_t *)&buf[40] == 0x6666666666666666ULL);
  CHECK(*(uint64_t *)&buf[48] == 0x7777777777777777ULL);
  CHECK(*(uint64_t *)&buf[56] == 0x8888888888888888ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stpqpost") {
  const char source[] = "stp q1, q2, [x0], #32\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x5555555555555555ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x6666666666666666ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x7777777777777777ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x8888888888888888ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STPQpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STPQpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STPQpost", QBDI::POSTINST,
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
  fpr->v1 = ((__uint128_t)0x6666666666666666ULL << 64 | 0x5555555555555555ULL);
  fpr->v2 = ((__uint128_t)0x8888888888888888ULL << 64 | 0x7777777777777777ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0x5555555555555555ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x6666666666666666ULL);
  CHECK(*(uint64_t *)&buf[16] == 0x7777777777777777ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x8888888888888888ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stpqpre") {
  const char source[] = "stp q1, q2, [x0, #32]!\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[32], 0x5555555555555555ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x6666666666666666ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x7777777777777777ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x8888888888888888ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STPQpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STPQpre", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STPQpre", QBDI::POSTINST,
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
  fpr->v1 = ((__uint128_t)0x6666666666666666ULL << 64 | 0x5555555555555555ULL);
  fpr->v2 = ((__uint128_t)0x8888888888888888ULL << 64 | 0x7777777777777777ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[32] == 0x5555555555555555ULL);
  CHECK(*(uint64_t *)&buf[40] == 0x6666666666666666ULL);
  CHECK(*(uint64_t *)&buf[48] == 0x7777777777777777ULL);
  CHECK(*(uint64_t *)&buf[56] == 0x8888888888888888ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-sttnpqi") {
  if (!checkFeature("lsui")) {
    return;
  }

  const char source[] = "sttnp q1, q2, [x0, #32]\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[32], 0x5555555555555555ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x6666666666666666ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x7777777777777777ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x8888888888888888ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STTNPQi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STTNPQi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STTNPQi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = ((__uint128_t)0x6666666666666666ULL << 64 | 0x5555555555555555ULL);
  fpr->v2 = ((__uint128_t)0x8888888888888888ULL << 64 | 0x7777777777777777ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsui"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[32] == 0x5555555555555555ULL);
  CHECK(*(uint64_t *)&buf[40] == 0x6666666666666666ULL);
  CHECK(*(uint64_t *)&buf[48] == 0x7777777777777777ULL);
  CHECK(*(uint64_t *)&buf[56] == 0x8888888888888888ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-sttpqi") {
  if (!checkFeature("lsui")) {
    return;
  }

  const char source[] = "sttp q1, q2, [x0, #32]\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[32], 0x5555555555555555ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x6666666666666666ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x7777777777777777ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x8888888888888888ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STTPQi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STTPQi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STTPQi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = ((__uint128_t)0x6666666666666666ULL << 64 | 0x5555555555555555ULL);
  fpr->v2 = ((__uint128_t)0x8888888888888888ULL << 64 | 0x7777777777777777ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsui"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[32] == 0x5555555555555555ULL);
  CHECK(*(uint64_t *)&buf[40] == 0x6666666666666666ULL);
  CHECK(*(uint64_t *)&buf[48] == 0x7777777777777777ULL);
  CHECK(*(uint64_t *)&buf[56] == 0x8888888888888888ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-sttpqpost") {
  if (!checkFeature("lsui")) {
    return;
  }

  const char source[] = "sttp q1, q2, [x0], #32\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x5555555555555555ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x6666666666666666ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x7777777777777777ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x8888888888888888ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STTPQpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STTPQpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STTPQpost", QBDI::POSTINST,
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
  fpr->v1 = ((__uint128_t)0x6666666666666666ULL << 64 | 0x5555555555555555ULL);
  fpr->v2 = ((__uint128_t)0x8888888888888888ULL << 64 | 0x7777777777777777ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsui"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0x5555555555555555ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x6666666666666666ULL);
  CHECK(*(uint64_t *)&buf[16] == 0x7777777777777777ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x8888888888888888ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-sttpqpre") {
  if (!checkFeature("lsui")) {
    return;
  }

  const char source[] = "sttp q1, q2, [x0, #32]!\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[32], 0x5555555555555555ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x6666666666666666ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[48], 0x7777777777777777ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[56], 0x8888888888888888ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STTPQpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STTPQpre", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STTPQpre", QBDI::POSTINST,
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
  fpr->v1 = ((__uint128_t)0x6666666666666666ULL << 64 | 0x5555555555555555ULL);
  fpr->v2 = ((__uint128_t)0x8888888888888888ULL << 64 | 0x7777777777777777ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsui"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[32] == 0x5555555555555555ULL);
  CHECK(*(uint64_t *)&buf[40] == 0x6666666666666666ULL);
  CHECK(*(uint64_t *)&buf[48] == 0x7777777777777777ULL);
  CHECK(*(uint64_t *)&buf[56] == 0x8888888888888888ULL);
}
