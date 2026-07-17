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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrql") {
  const char source[] =
      "ldr q0, 1f\n"
      "b 2f\n"
      "1:\n"
      ".xword 0x123456789abcdef0\n"
      ".xword 0x0fedcba987654321\n"
      "2:\n";

  QBDI::rword addr = genASM(source);
  QBDI::rword literalAddr = addr + 2 * 4;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {literalAddr, 0x123456789abcdef0ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {literalAddr + 8, 0x0fedcba987654321ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {literalAddr, 0x123456789abcdef0ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {literalAddr + 8, 0x0fedcba987654321ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRQl", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRQl", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRQl", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x0fedcba987654321ULL << 64 |
                            0x123456789abcdef0ULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = vm.call(&retval, addr);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrqpost") {
  const char source[] = "ldr q0, [x0], #16\n";

  uint8_t buf[64] = {0};
  *(uint64_t *)&buf[0] = 0x99aabbccca10abc1ULL;
  *(uint64_t *)&buf[8] = 0x1122334401a7b4fbULL;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x99aabbccca10abc1ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1122334401a7b4fbULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x99aabbccca10abc1ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1122334401a7b4fbULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRQpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRQpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRQpost", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x1122334401a7b4fbULL << 64 |
                            0x99aabbccca10abc1ULL));
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrqpre") {
  const char source[] = "ldr q0, [x0, #16]!\n";

  uint8_t buf[64] = {0};
  *(uint64_t *)&buf[16] = 0x99aabbccde194241ULL;
  *(uint64_t *)&buf[24] = 0x11223344e82774fdULL;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[16], 0x99aabbccde194241ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x11223344e82774fdULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], 0x99aabbccde194241ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x11223344e82774fdULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRQpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRQpre", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRQpre", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x11223344e82774fdULL << 64 |
                            0x99aabbccde194241ULL));
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrqui") {
  const char source[] = "ldr q0, [x0, #32]\n";

  uint8_t buf[64] = {0};
  *(uint64_t *)&buf[32] = 0x99aabbcc8933ada7ULL;
  *(uint64_t *)&buf[40] = 0x1122334407c15113ULL;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[32], 0x99aabbcc8933ada7ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x1122334407c15113ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[32], 0x99aabbcc8933ada7ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[40], 0x1122334407c15113ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRQui", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRQui", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRQui", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x1122334407c15113ULL << 64 |
                            0x99aabbcc8933ada7ULL));
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldurqi") {
  const char source[] = "ldur q0, [x0, #1]\n";

  uint8_t buf[64] = {0};
  *(uint64_t *)&buf[1] = 0x99aabbcce5310013ULL;
  *(uint64_t *)&buf[9] = 0x11223344aa7575d3ULL;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[1], 0x99aabbcce5310013ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[9], 0x11223344aa7575d3ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[1], 0x99aabbcce5310013ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[9], 0x11223344aa7575d3ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDURQi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDURQi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDURQi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x11223344aa7575d3ULL << 64 |
                            0x99aabbcce5310013ULL));
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldapurqi") {
  if (!checkFeature("rcpc3")) {
    return;
  }

  const char source[] = "ldapur q0, [x0]\n";

  uint8_t buf[64] = {0};
  *(uint64_t *)&buf[0] = 0x99aabbcc19ffd3caULL;
  *(uint64_t *)&buf[8] = 0x1122334479ac3cc9ULL;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x99aabbcc19ffd3caULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1122334479ac3cc9ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x99aabbcc19ffd3caULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1122334479ac3cc9ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDAPURqi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDAPURqi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDAPURqi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((__uint128_t)fprState->v0 ==
                           ((__uint128_t)0x1122334479ac3cc9ULL << 64 |
                            0x99aabbcc19ffd3caULL));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"rcpc3"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strqpost") {
  const char source[] = "str q0, [x0], #16\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x6666666601a7ab5bULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x5555555598687ee3ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STRQpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRQpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STRQpost", QBDI::POSTINST,
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
  fpr->v0 = ((__uint128_t)0x5555555598687ee3ULL << 64 | 0x6666666601a7ab5bULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0x6666666601a7ab5bULL);
  CHECK(*(uint64_t *)&buf[8] == 0x5555555598687ee3ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strqpre") {
  const char source[] = "str q0, [x0, #16]!\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], 0x666666666d0d8572ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], 0x55555555b6410cddULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STRQpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRQpre", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STRQpre", QBDI::POSTINST,
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
  fpr->v0 = ((__uint128_t)0x55555555b6410cddULL << 64 | 0x666666666d0d8572ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[16] == 0x666666666d0d8572ULL);
  CHECK(*(uint64_t *)&buf[24] == 0x55555555b6410cddULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strqui") {
  const char source[] = "str q0, [x0]\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x666666664aee2ec2ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x555555551df1cd9dULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STRQui", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRQui", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STRQui", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x555555551df1cd9dULL << 64 | 0x666666664aee2ec2ULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0x666666664aee2ec2ULL);
  CHECK(*(uint64_t *)&buf[8] == 0x555555551df1cd9dULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-sturqi") {
  const char source[] = "stur q0, [x0, #1]\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[1], 0x666666667a84ee3dULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[9], 0x55555555dd0e9174ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STURQi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STURQi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STURQi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x55555555dd0e9174ULL << 64 | 0x666666667a84ee3dULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[1] == 0x666666667a84ee3dULL);
  CHECK(*(uint64_t *)&buf[9] == 0x55555555dd0e9174ULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stlurqi") {
  if (!checkFeature("rcpc3")) {
    return;
  }

  const char source[] = "stlur q0, [x0]\n";

  uint8_t buf[64] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x66666666ffd168acULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x555555555b9f6e8dULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STLURqi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STLURqi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STLURqi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((__uint128_t)0x555555555b9f6e8dULL << 64 | 0x66666666ffd168acULL);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"rcpc3"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == 0x66666666ffd168acULL);
  CHECK(*(uint64_t *)&buf[8] == 0x555555555b9f6e8dULL);
}
