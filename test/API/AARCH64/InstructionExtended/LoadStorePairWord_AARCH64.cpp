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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldnpsi") {
  const char source[] = "ldnp s1, s2, [x0, #16]\n";

  constexpr uint32_t val0 = 0x11111111;
  constexpr uint32_t val1 = 0x22222222;
  constexpr uint64_t combined = ((uint64_t)val1 << 32) | val0;
  uint8_t buf[32] = {0};
  *(uint32_t *)&buf[16] = val0;
  *(uint32_t *)&buf[20] = val1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[16], combined, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], combined, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDNPSi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDNPSi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDNPSi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v1 == 0x11111111);
                     CHECK((uint32_t)fprState->v2 == 0x22222222);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldnpwi") {
  const char source[] = "ldnp w1, w2, [x0, #16]\n";

  constexpr uint32_t val0 = 0x33333333;
  constexpr uint32_t val1 = 0x44444444;
  constexpr uint64_t combined = ((uint64_t)val1 << 32) | val0;
  uint8_t buf[32] = {0};
  *(uint32_t *)&buf[16] = val0;
  *(uint32_t *)&buf[20] = val1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[16], combined, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], combined, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDNPWi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDNPWi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDNPWi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)gprState->x1 == 0x33333333);
                     CHECK((uint32_t)gprState->x2 == 0x44444444);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldpsi") {
  const char source[] = "ldp s1, s2, [x0, #16]\n";

  constexpr uint32_t val0 = 0x55555555;
  constexpr uint32_t val1 = 0x66666666;
  constexpr uint64_t combined = ((uint64_t)val1 << 32) | val0;
  uint8_t buf[32] = {0};
  *(uint32_t *)&buf[16] = val0;
  *(uint32_t *)&buf[20] = val1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[16], combined, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], combined, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDPSi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDPSi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDPSi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v1 == 0x55555555);
                     CHECK((uint32_t)fprState->v2 == 0x66666666);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldpspost") {
  const char source[] = "ldp s1, s2, [x0], #16\n";

  constexpr uint32_t val0 = 0x77777777;
  constexpr uint32_t val1 = 0x88888888;
  constexpr uint64_t combined = ((uint64_t)val1 << 32) | val0;
  uint8_t buf[32] = {0};
  *(uint32_t *)&buf[0] = val0;
  *(uint32_t *)&buf[4] = val1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], combined, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], combined, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDPSpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDPSpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDPSpost", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v1 == 0x77777777);
                     CHECK((uint32_t)fprState->v2 == 0x88888888);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldpspre") {
  const char source[] = "ldp s1, s2, [x0, #16]!\n";

  constexpr uint32_t val0 = 0x99999999;
  constexpr uint32_t val1 = 0xaaaaaaaa;
  constexpr uint64_t combined = ((uint64_t)val1 << 32) | val0;
  uint8_t buf[32] = {0};
  *(uint32_t *)&buf[16] = val0;
  *(uint32_t *)&buf[20] = val1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[16], combined, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], combined, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDPSpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDPSpre", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDPSpre", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v1 == 0x99999999);
                     CHECK((uint32_t)fprState->v2 == 0xaaaaaaaa);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldpswi") {
  const char source[] = "ldpsw x1, x2, [x0, #16]\n";

  constexpr uint32_t val0 = 0x80901234;
  constexpr uint32_t val1 = 0x90abcdef;
  constexpr uint64_t combined = ((uint64_t)val1 << 32) | val0;
  uint8_t buf[32] = {0};
  *(uint32_t *)&buf[16] = val0;
  *(uint32_t *)&buf[20] = val1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[16], combined, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], combined, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDPSWi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDPSWi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDPSWi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == 0xffffffff80901234ULL);
                     CHECK(gprState->x2 == 0xffffffff90abcdefULL);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldpswpost") {
  const char source[] = "ldpsw x1, x2, [x0], #16\n";

  constexpr uint32_t val0 = 0xa1b2c3d4;
  constexpr uint32_t val1 = 0xb1c2d3e4;
  constexpr uint64_t combined = ((uint64_t)val1 << 32) | val0;
  uint8_t buf[32] = {0};
  *(uint32_t *)&buf[0] = val0;
  *(uint32_t *)&buf[4] = val1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], combined, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], combined, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDPSWpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDPSWpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDPSWpost", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == 0xffffffffa1b2c3d4ULL);
                     CHECK(gprState->x2 == 0xffffffffb1c2d3e4ULL);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldpswpre") {
  const char source[] = "ldpsw x1, x2, [x0, #16]!\n";

  constexpr uint32_t val0 = 0xc1d2e3f4;
  constexpr uint32_t val1 = 0xd1e2f3a4;
  constexpr uint64_t combined = ((uint64_t)val1 << 32) | val0;
  uint8_t buf[32] = {0};
  *(uint32_t *)&buf[16] = val0;
  *(uint32_t *)&buf[20] = val1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[16], combined, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], combined, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDPSWpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDPSWpre", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDPSWpre", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == 0xffffffffc1d2e3f4ULL);
                     CHECK(gprState->x2 == 0xffffffffd1e2f3a4ULL);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldpwpost") {
  const char source[] = "ldp w1, w2, [x0], #16\n";

  constexpr uint32_t val0 = 0xbbbbbbbb;
  constexpr uint32_t val1 = 0xcccccccc;
  constexpr uint64_t combined = ((uint64_t)val1 << 32) | val0;
  uint8_t buf[32] = {0};
  *(uint32_t *)&buf[0] = val0;
  *(uint32_t *)&buf[4] = val1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], combined, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], combined, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDPWpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDPWpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDPWpost", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)gprState->x1 == 0xbbbbbbbb);
                     CHECK((uint32_t)gprState->x2 == 0xcccccccc);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stnpsi") {
  const char source[] = "stnp s1, s2, [x0, #16]\n";

  constexpr uint32_t val0 = 0x11223344;
  constexpr uint32_t val1 = 0x22334455;
  constexpr uint64_t combined = ((uint64_t)val1 << 32) | val0;
  uint8_t buf[32] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], combined, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STNPSi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STNPSi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];

  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = val0;
  fpr->v2 = val1;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[16] == val0);
  CHECK(*(uint32_t *)&buf[20] == val1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stnpwi") {
  const char source[] = "stnp w1, w2, [x0, #16]\n";

  constexpr uint32_t val0 = 0x33445566;
  constexpr uint32_t val1 = 0x44556677;
  constexpr uint64_t combined = ((uint64_t)val1 << 32) | val0;
  uint8_t buf[32] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], combined, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STNPWi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STNPWi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x1 = val0;
  state->x2 = val1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[16] == val0);
  CHECK(*(uint32_t *)&buf[20] == val1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stpsi") {
  const char source[] = "stp s1, s2, [x0, #16]\n";

  constexpr uint32_t val0 = 0x55667788;
  constexpr uint32_t val1 = 0x66778899;
  constexpr uint64_t combined = ((uint64_t)val1 << 32) | val0;
  uint8_t buf[32] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], combined, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STPSi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STPSi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];

  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = val0;
  fpr->v2 = val1;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[16] == val0);
  CHECK(*(uint32_t *)&buf[20] == val1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stpspost") {
  const char source[] = "stp s1, s2, [x0], #16\n";

  constexpr uint32_t val0 = 0x778899aa;
  constexpr uint32_t val1 = 0x8899aabb;
  constexpr uint64_t combined = ((uint64_t)val1 << 32) | val0;
  uint8_t buf[32] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], combined, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STPSpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STPSpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STPSpost", QBDI::POSTINST,
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
  fpr->v1 = val0;
  fpr->v2 = val1;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint32_t *)&buf[0] == val0);
  CHECK(*(uint32_t *)&buf[4] == val1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stpspre") {
  const char source[] = "stp s1, s2, [x0, #16]!\n";

  constexpr uint32_t val0 = 0x99aabbcc;
  constexpr uint32_t val1 = 0xaabbccdd;
  constexpr uint64_t combined = ((uint64_t)val1 << 32) | val0;
  uint8_t buf[32] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], combined, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STPSpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STPSpre", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STPSpre", QBDI::POSTINST,
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
  fpr->v1 = val0;
  fpr->v2 = val1;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint32_t *)&buf[16] == val0);
  CHECK(*(uint32_t *)&buf[20] == val1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stpwi") {
  const char source[] = "stp w1, w2, [x0, #16]\n";

  constexpr uint32_t val0 = 0xbbccddee;
  constexpr uint32_t val1 = 0xccddeeff;
  constexpr uint64_t combined = ((uint64_t)val1 << 32) | val0;
  uint8_t buf[32] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], combined, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STPWi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STPWi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x1 = val0;
  state->x2 = val1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[16] == val0);
  CHECK(*(uint32_t *)&buf[20] == val1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stpwpost") {
  const char source[] = "stp w1, w2, [x0], #16\n";

  constexpr uint32_t val0 = 0xddeeff00;
  constexpr uint32_t val1 = 0xeeff0011;
  constexpr uint64_t combined = ((uint64_t)val1 << 32) | val0;
  uint8_t buf[32] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], combined, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STPWpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STPWpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STPWpost", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 16);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x1 = val0;
  state->x2 = val1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint32_t *)&buf[0] == val0);
  CHECK(*(uint32_t *)&buf[4] == val1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stpwpre") {
  const char source[] = "stp w1, w2, [x0, #16]!\n";

  constexpr uint32_t val0 = 0xff001122;
  constexpr uint32_t val1 = 0x00112233;
  constexpr uint64_t combined = ((uint64_t)val1 << 32) | val0;
  uint8_t buf[32] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], combined, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STPWpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STPWpre", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STPWpre", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 16);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x1 = val0;
  state->x2 = val1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint32_t *)&buf[16] == val0);
  CHECK(*(uint32_t *)&buf[20] == val1);
}
