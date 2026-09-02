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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1fourv1d") {
  const char source[] = "ld1 {v0.1d, v1.1d, v2.1d, v3.1d}, [x0]\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  constexpr uint64_t e2 = 0x3333333333333333ULL;
  constexpr uint64_t e3 = 0x4444444444444444ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  *(uint64_t *)&buf[16] = e2;
  *(uint64_t *)&buf[24] = e3;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Fourv1d", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Fourv1d", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Fourv1d", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
                     CHECK((uint64_t)fprState->v2 == e2);
                     CHECK((uint64_t)fprState->v3 == e3);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1fourv2s") {
  const char source[] = "ld1 {v0.2s, v1.2s, v2.2s, v3.2s}, [x0]\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  constexpr uint64_t e2 = 0x3333333333333333ULL;
  constexpr uint64_t e3 = 0x4444444444444444ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  *(uint64_t *)&buf[16] = e2;
  *(uint64_t *)&buf[24] = e3;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Fourv2s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Fourv2s", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Fourv2s", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
                     CHECK((uint64_t)fprState->v2 == e2);
                     CHECK((uint64_t)fprState->v3 == e3);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1fourv2s_post") {
  const char source[] = "ld1 {v0.2s, v1.2s, v2.2s, v3.2s}, [x0], #32\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  constexpr uint64_t e2 = 0x3333333333333333ULL;
  constexpr uint64_t e3 = 0x4444444444444444ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  *(uint64_t *)&buf[16] = e2;
  *(uint64_t *)&buf[24] = e3;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Fourv2s_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Fourv2s_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD1Fourv2s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
                     CHECK((uint64_t)fprState->v2 == e2);
                     CHECK((uint64_t)fprState->v3 == e3);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1fourv4h") {
  const char source[] = "ld1 {v0.4h, v1.4h, v2.4h, v3.4h}, [x0]\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  constexpr uint64_t e2 = 0x3333333333333333ULL;
  constexpr uint64_t e3 = 0x4444444444444444ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  *(uint64_t *)&buf[16] = e2;
  *(uint64_t *)&buf[24] = e3;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Fourv4h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Fourv4h", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Fourv4h", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
                     CHECK((uint64_t)fprState->v2 == e2);
                     CHECK((uint64_t)fprState->v3 == e3);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1fourv4h_post") {
  const char source[] = "ld1 {v0.4h, v1.4h, v2.4h, v3.4h}, [x0], #32\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  constexpr uint64_t e2 = 0x3333333333333333ULL;
  constexpr uint64_t e3 = 0x4444444444444444ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  *(uint64_t *)&buf[16] = e2;
  *(uint64_t *)&buf[24] = e3;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Fourv4h_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Fourv4h_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD1Fourv4h_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
                     CHECK((uint64_t)fprState->v2 == e2);
                     CHECK((uint64_t)fprState->v3 == e3);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1i64") {
  const char source[] = "ld1 {v0.d}[0], [x0]\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1i64", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1i64", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1i64", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1onev1d") {
  const char source[] = "ld1 {v0.1d}, [x0]\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Onev1d", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Onev1d", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Onev1d", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1onev1d_post") {
  const char source[] = "ld1 {v0.1d}, [x0], #8\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Onev1d_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Onev1d_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD1Onev1d_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 8);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1onev2s") {
  const char source[] = "ld1 {v0.2s}, [x0]\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Onev2s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Onev2s", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Onev2s", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1onev2s_post") {
  const char source[] = "ld1 {v0.2s}, [x0], #8\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Onev2s_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Onev2s_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD1Onev2s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 8);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1onev4h") {
  const char source[] = "ld1 {v0.4h}, [x0]\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Onev4h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Onev4h", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Onev4h", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1onev4h_post") {
  const char source[] = "ld1 {v0.4h}, [x0], #8\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Onev4h_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Onev4h_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD1Onev4h_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 8);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1rv1d_post") {
  const char source[] = "ld1r {v0.1d}, [x0], #8\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Rv1d_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Rv1d_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Rv1d_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 8);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1rv2d_post") {
  const char source[] = "ld1r {v0.2d}, [x0], #8\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Rv2d_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Rv2d_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Rv2d_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 8);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1threev1d") {
  const char source[] = "ld1 {v0.1d, v1.1d, v2.1d}, [x0]\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  constexpr uint64_t e2 = 0x3333333333333333ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  *(uint64_t *)&buf[16] = e2;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Threev1d", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Threev1d", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Threev1d", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
                     CHECK((uint64_t)fprState->v2 == e2);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1threev1d_post") {
  const char source[] = "ld1 {v0.1d, v1.1d, v2.1d}, [x0], #24\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  constexpr uint64_t e2 = 0x3333333333333333ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  *(uint64_t *)&buf[16] = e2;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Threev1d_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("LD1Threev1d_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD1Threev1d_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
                     CHECK((uint64_t)fprState->v2 == e2);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 24);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1threev2s") {
  const char source[] = "ld1 {v0.2s, v1.2s, v2.2s}, [x0]\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  constexpr uint64_t e2 = 0x3333333333333333ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  *(uint64_t *)&buf[16] = e2;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Threev2s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Threev2s", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Threev2s", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
                     CHECK((uint64_t)fprState->v2 == e2);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1threev2s_post") {
  const char source[] = "ld1 {v0.2s, v1.2s, v2.2s}, [x0], #24\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  constexpr uint64_t e2 = 0x3333333333333333ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  *(uint64_t *)&buf[16] = e2;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Threev2s_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("LD1Threev2s_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD1Threev2s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
                     CHECK((uint64_t)fprState->v2 == e2);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 24);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1threev4h") {
  const char source[] = "ld1 {v0.4h, v1.4h, v2.4h}, [x0]\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  constexpr uint64_t e2 = 0x3333333333333333ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  *(uint64_t *)&buf[16] = e2;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Threev4h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Threev4h", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Threev4h", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
                     CHECK((uint64_t)fprState->v2 == e2);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1threev4h_post") {
  const char source[] = "ld1 {v0.4h, v1.4h, v2.4h}, [x0], #24\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  constexpr uint64_t e2 = 0x3333333333333333ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  *(uint64_t *)&buf[16] = e2;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Threev4h_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("LD1Threev4h_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD1Threev4h_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
                     CHECK((uint64_t)fprState->v2 == e2);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 24);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1threev8b_post") {
  const char source[] = "ld1 {v0.8b, v1.8b, v2.8b}, [x0], #24\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  constexpr uint64_t e2 = 0x3333333333333333ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  *(uint64_t *)&buf[16] = e2;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Threev8b_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("LD1Threev8b_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD1Threev8b_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
                     CHECK((uint64_t)fprState->v2 == e2);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 24);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1twov1d") {
  const char source[] = "ld1 {v0.1d, v1.1d}, [x0]\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Twov1d", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Twov1d", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Twov1d", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1twov1d_post") {
  const char source[] = "ld1 {v0.1d, v1.1d}, [x0], #16\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Twov1d_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Twov1d_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD1Twov1d_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1twov2s") {
  const char source[] = "ld1 {v0.2s, v1.2s}, [x0]\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Twov2s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Twov2s", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Twov2s", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1twov2s_post") {
  const char source[] = "ld1 {v0.2s, v1.2s}, [x0], #16\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Twov2s_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Twov2s_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD1Twov2s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1twov4h") {
  const char source[] = "ld1 {v0.4h, v1.4h}, [x0]\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Twov4h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Twov4h", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Twov4h", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1twov4h_post") {
  const char source[] = "ld1 {v0.4h, v1.4h}, [x0], #16\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Twov4h_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Twov4h_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD1Twov4h_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1twov8b_post") {
  const char source[] = "ld1 {v0.8b, v1.8b}, [x0], #16\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Twov8b_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Twov8b_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD1Twov8b_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2i64") {
  const char source[] = "ld2 {v0.d, v1.d}[0], [x0]\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2i64", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2i64", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD2i64", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2i64_post") {
  const char source[] = "ld2 {v0.d, v1.d}[0], [x0], #16\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2i64_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2i64_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD2i64_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2rv1d") {
  const char source[] = "ld2r {v0.1d, v1.1d}, [x0]\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2Rv1d", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2Rv1d", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD2Rv1d", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2rv1d_post") {
  const char source[] = "ld2r {v0.1d, v1.1d}, [x0], #16\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2Rv1d_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2Rv1d_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD2Rv1d_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2rv2d") {
  const char source[] = "ld2r {v0.2d, v1.2d}, [x0]\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2Rv2d", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2Rv2d", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD2Rv2d", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2rv2d_post") {
  const char source[] = "ld2r {v0.2d, v1.2d}, [x0], #16\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2Rv2d_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2Rv2d_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD2Rv2d_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2twov2s") {
  const char source[] = "ld2 {v0.2s, v1.2s}, [x0]\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t r0 = 0xc0b0a0904030201ULL;
  constexpr uint64_t r1 = 0x100f0e0d08070605ULL;
  uint8_t buf[40] = {0};
  buf[0] = 1;
  buf[1] = 2;
  buf[2] = 3;
  buf[3] = 4;
  buf[4] = 5;
  buf[5] = 6;
  buf[6] = 7;
  buf[7] = 8;
  buf[8] = 9;
  buf[9] = 10;
  buf[10] = 11;
  buf[11] = 12;
  buf[12] = 13;
  buf[13] = 14;
  buf[14] = 15;
  buf[15] = 16;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2Twov2s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2Twov2s", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD2Twov2s", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == r0);
                     CHECK((uint64_t)fprState->v1 == r1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2twov2s_post") {
  const char source[] = "ld2 {v0.2s, v1.2s}, [x0], #16\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t r0 = 0xc0b0a0904030201ULL;
  constexpr uint64_t r1 = 0x100f0e0d08070605ULL;
  uint8_t buf[40] = {0};
  buf[0] = 1;
  buf[1] = 2;
  buf[2] = 3;
  buf[3] = 4;
  buf[4] = 5;
  buf[5] = 6;
  buf[6] = 7;
  buf[7] = 8;
  buf[8] = 9;
  buf[9] = 10;
  buf[10] = 11;
  buf[11] = 12;
  buf[12] = 13;
  buf[13] = 14;
  buf[14] = 15;
  buf[15] = 16;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2Twov2s_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2Twov2s_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD2Twov2s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == r0);
                     CHECK((uint64_t)fprState->v1 == r1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2twov4h") {
  const char source[] = "ld2 {v0.4h, v1.4h}, [x0]\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t r0 = 0xe0d0a0906050201ULL;
  constexpr uint64_t r1 = 0x100f0c0b08070403ULL;
  uint8_t buf[40] = {0};
  buf[0] = 1;
  buf[1] = 2;
  buf[2] = 3;
  buf[3] = 4;
  buf[4] = 5;
  buf[5] = 6;
  buf[6] = 7;
  buf[7] = 8;
  buf[8] = 9;
  buf[9] = 10;
  buf[10] = 11;
  buf[11] = 12;
  buf[12] = 13;
  buf[13] = 14;
  buf[14] = 15;
  buf[15] = 16;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2Twov4h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2Twov4h", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD2Twov4h", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == r0);
                     CHECK((uint64_t)fprState->v1 == r1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2twov4h_post") {
  const char source[] = "ld2 {v0.4h, v1.4h}, [x0], #16\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t r0 = 0xe0d0a0906050201ULL;
  constexpr uint64_t r1 = 0x100f0c0b08070403ULL;
  uint8_t buf[40] = {0};
  buf[0] = 1;
  buf[1] = 2;
  buf[2] = 3;
  buf[3] = 4;
  buf[4] = 5;
  buf[5] = 6;
  buf[6] = 7;
  buf[7] = 8;
  buf[8] = 9;
  buf[9] = 10;
  buf[10] = 11;
  buf[11] = 12;
  buf[12] = 13;
  buf[13] = 14;
  buf[14] = 15;
  buf[15] = 16;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2Twov4h_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2Twov4h_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD2Twov4h_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == r0);
                     CHECK((uint64_t)fprState->v1 == r1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2twov8b_post") {
  const char source[] = "ld2 {v0.8b, v1.8b}, [x0], #16\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t r0 = 0xf0d0b0907050301ULL;
  constexpr uint64_t r1 = 0x100e0c0a08060402ULL;
  uint8_t buf[40] = {0};
  buf[0] = 1;
  buf[1] = 2;
  buf[2] = 3;
  buf[3] = 4;
  buf[4] = 5;
  buf[5] = 6;
  buf[6] = 7;
  buf[7] = 8;
  buf[8] = 9;
  buf[9] = 10;
  buf[10] = 11;
  buf[11] = 12;
  buf[12] = 13;
  buf[13] = 14;
  buf[14] = 15;
  buf[15] = 16;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2Twov8b_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2Twov8b_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD2Twov8b_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == r0);
                     CHECK((uint64_t)fprState->v1 == r1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3i64") {
  const char source[] = "ld3 {v0.d, v1.d, v2.d}[0], [x0]\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  constexpr uint64_t e2 = 0x3333333333333333ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  *(uint64_t *)&buf[16] = e2;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3i64", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD3i64", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD3i64", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
                     CHECK((uint64_t)fprState->v2 == e2);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3i64_post") {
  const char source[] = "ld3 {v0.d, v1.d, v2.d}[0], [x0], #24\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  constexpr uint64_t e2 = 0x3333333333333333ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  *(uint64_t *)&buf[16] = e2;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3i64_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD3i64_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD3i64_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
                     CHECK((uint64_t)fprState->v2 == e2);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 24);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3rv1d") {
  const char source[] = "ld3r {v0.1d, v1.1d, v2.1d}, [x0]\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  constexpr uint64_t e2 = 0x3333333333333333ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  *(uint64_t *)&buf[16] = e2;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3Rv1d", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD3Rv1d", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD3Rv1d", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
                     CHECK((uint64_t)fprState->v2 == e2);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3rv1d_post") {
  const char source[] = "ld3r {v0.1d, v1.1d, v2.1d}, [x0], #24\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  constexpr uint64_t e2 = 0x3333333333333333ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  *(uint64_t *)&buf[16] = e2;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3Rv1d_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD3Rv1d_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD3Rv1d_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
                     CHECK((uint64_t)fprState->v2 == e2);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 24);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3rv2d_post") {
  const char source[] = "ld3r {v0.2d, v1.2d, v2.2d}, [x0], #24\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  constexpr uint64_t e2 = 0x3333333333333333ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  *(uint64_t *)&buf[16] = e2;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3Rv2d_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD3Rv2d_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD3Rv2d_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
                     CHECK((uint64_t)fprState->v2 == e2);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 24);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3threev2s") {
  const char source[] = "ld3 {v0.2s, v1.2s, v2.2s}, [x0]\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t c2 = 0x1817161514131211ULL;
  constexpr uint64_t r0 = 0x100f0e0d04030201ULL;
  constexpr uint64_t r1 = 0x1413121108070605ULL;
  constexpr uint64_t r2 = 0x181716150c0b0a09ULL;
  uint8_t buf[40] = {0};
  buf[0] = 1;
  buf[1] = 2;
  buf[2] = 3;
  buf[3] = 4;
  buf[4] = 5;
  buf[5] = 6;
  buf[6] = 7;
  buf[7] = 8;
  buf[8] = 9;
  buf[9] = 10;
  buf[10] = 11;
  buf[11] = 12;
  buf[12] = 13;
  buf[13] = 14;
  buf[14] = 15;
  buf[15] = 16;
  buf[16] = 17;
  buf[17] = 18;
  buf[18] = 19;
  buf[19] = 20;
  buf[20] = 21;
  buf[21] = 22;
  buf[22] = 23;
  buf[23] = 24;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3Threev2s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD3Threev2s", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD3Threev2s", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == r0);
                     CHECK((uint64_t)fprState->v1 == r1);
                     CHECK((uint64_t)fprState->v2 == r2);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3threev2s_post") {
  const char source[] = "ld3 {v0.2s, v1.2s, v2.2s}, [x0], #24\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t c2 = 0x1817161514131211ULL;
  constexpr uint64_t r0 = 0x100f0e0d04030201ULL;
  constexpr uint64_t r1 = 0x1413121108070605ULL;
  constexpr uint64_t r2 = 0x181716150c0b0a09ULL;
  uint8_t buf[40] = {0};
  buf[0] = 1;
  buf[1] = 2;
  buf[2] = 3;
  buf[3] = 4;
  buf[4] = 5;
  buf[5] = 6;
  buf[6] = 7;
  buf[7] = 8;
  buf[8] = 9;
  buf[9] = 10;
  buf[10] = 11;
  buf[11] = 12;
  buf[12] = 13;
  buf[13] = 14;
  buf[14] = 15;
  buf[15] = 16;
  buf[16] = 17;
  buf[17] = 18;
  buf[18] = 19;
  buf[19] = 20;
  buf[20] = 21;
  buf[21] = 22;
  buf[22] = 23;
  buf[23] = 24;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3Threev2s_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("LD3Threev2s_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD3Threev2s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == r0);
                     CHECK((uint64_t)fprState->v1 == r1);
                     CHECK((uint64_t)fprState->v2 == r2);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 24);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3threev4h") {
  const char source[] = "ld3 {v0.4h, v1.4h, v2.4h}, [x0]\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t c2 = 0x1817161514131211ULL;
  constexpr uint64_t r0 = 0x14130e0d08070201ULL;
  constexpr uint64_t r1 = 0x1615100f0a090403ULL;
  constexpr uint64_t r2 = 0x181712110c0b0605ULL;
  uint8_t buf[40] = {0};
  buf[0] = 1;
  buf[1] = 2;
  buf[2] = 3;
  buf[3] = 4;
  buf[4] = 5;
  buf[5] = 6;
  buf[6] = 7;
  buf[7] = 8;
  buf[8] = 9;
  buf[9] = 10;
  buf[10] = 11;
  buf[11] = 12;
  buf[12] = 13;
  buf[13] = 14;
  buf[14] = 15;
  buf[15] = 16;
  buf[16] = 17;
  buf[17] = 18;
  buf[18] = 19;
  buf[19] = 20;
  buf[20] = 21;
  buf[21] = 22;
  buf[22] = 23;
  buf[23] = 24;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3Threev4h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD3Threev4h", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD3Threev4h", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == r0);
                     CHECK((uint64_t)fprState->v1 == r1);
                     CHECK((uint64_t)fprState->v2 == r2);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3threev4h_post") {
  const char source[] = "ld3 {v0.4h, v1.4h, v2.4h}, [x0], #24\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t c2 = 0x1817161514131211ULL;
  constexpr uint64_t r0 = 0x14130e0d08070201ULL;
  constexpr uint64_t r1 = 0x1615100f0a090403ULL;
  constexpr uint64_t r2 = 0x181712110c0b0605ULL;
  uint8_t buf[40] = {0};
  buf[0] = 1;
  buf[1] = 2;
  buf[2] = 3;
  buf[3] = 4;
  buf[4] = 5;
  buf[5] = 6;
  buf[6] = 7;
  buf[7] = 8;
  buf[8] = 9;
  buf[9] = 10;
  buf[10] = 11;
  buf[11] = 12;
  buf[12] = 13;
  buf[13] = 14;
  buf[14] = 15;
  buf[15] = 16;
  buf[16] = 17;
  buf[17] = 18;
  buf[18] = 19;
  buf[19] = 20;
  buf[20] = 21;
  buf[21] = 22;
  buf[22] = 23;
  buf[23] = 24;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3Threev4h_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("LD3Threev4h_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD3Threev4h_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == r0);
                     CHECK((uint64_t)fprState->v1 == r1);
                     CHECK((uint64_t)fprState->v2 == r2);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 24);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3threev8b_post") {
  const char source[] = "ld3 {v0.8b, v1.8b, v2.8b}, [x0], #24\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t c2 = 0x1817161514131211ULL;
  constexpr uint64_t r0 = 0x1613100d0a070401ULL;
  constexpr uint64_t r1 = 0x1714110e0b080502ULL;
  constexpr uint64_t r2 = 0x1815120f0c090603ULL;
  uint8_t buf[40] = {0};
  buf[0] = 1;
  buf[1] = 2;
  buf[2] = 3;
  buf[3] = 4;
  buf[4] = 5;
  buf[5] = 6;
  buf[6] = 7;
  buf[7] = 8;
  buf[8] = 9;
  buf[9] = 10;
  buf[10] = 11;
  buf[11] = 12;
  buf[12] = 13;
  buf[13] = 14;
  buf[14] = 15;
  buf[15] = 16;
  buf[16] = 17;
  buf[17] = 18;
  buf[18] = 19;
  buf[19] = 20;
  buf[20] = 21;
  buf[21] = 22;
  buf[22] = 23;
  buf[23] = 24;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3Threev8b_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("LD3Threev8b_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD3Threev8b_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == r0);
                     CHECK((uint64_t)fprState->v1 == r1);
                     CHECK((uint64_t)fprState->v2 == r2);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 24);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4fourv2s") {
  const char source[] = "ld4 {v0.2s, v1.2s, v2.2s, v3.2s}, [x0]\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t c2 = 0x1817161514131211ULL;
  constexpr uint64_t c3 = 0x201f1e1d1c1b1a19ULL;
  constexpr uint64_t r0 = 0x1413121104030201ULL;
  constexpr uint64_t r1 = 0x1817161508070605ULL;
  constexpr uint64_t r2 = 0x1c1b1a190c0b0a09ULL;
  constexpr uint64_t r3 = 0x201f1e1d100f0e0dULL;
  uint8_t buf[40] = {0};
  buf[0] = 1;
  buf[1] = 2;
  buf[2] = 3;
  buf[3] = 4;
  buf[4] = 5;
  buf[5] = 6;
  buf[6] = 7;
  buf[7] = 8;
  buf[8] = 9;
  buf[9] = 10;
  buf[10] = 11;
  buf[11] = 12;
  buf[12] = 13;
  buf[13] = 14;
  buf[14] = 15;
  buf[15] = 16;
  buf[16] = 17;
  buf[17] = 18;
  buf[18] = 19;
  buf[19] = 20;
  buf[20] = 21;
  buf[21] = 22;
  buf[22] = 23;
  buf[23] = 24;
  buf[24] = 25;
  buf[25] = 26;
  buf[26] = 27;
  buf[27] = 28;
  buf[28] = 29;
  buf[29] = 30;
  buf[30] = 31;
  buf[31] = 32;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], c3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], c3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4Fourv2s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4Fourv2s", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD4Fourv2s", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == r0);
                     CHECK((uint64_t)fprState->v1 == r1);
                     CHECK((uint64_t)fprState->v2 == r2);
                     CHECK((uint64_t)fprState->v3 == r3);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4fourv2s_post") {
  const char source[] = "ld4 {v0.2s, v1.2s, v2.2s, v3.2s}, [x0], #32\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t c2 = 0x1817161514131211ULL;
  constexpr uint64_t c3 = 0x201f1e1d1c1b1a19ULL;
  constexpr uint64_t r0 = 0x1413121104030201ULL;
  constexpr uint64_t r1 = 0x1817161508070605ULL;
  constexpr uint64_t r2 = 0x1c1b1a190c0b0a09ULL;
  constexpr uint64_t r3 = 0x201f1e1d100f0e0dULL;
  uint8_t buf[40] = {0};
  buf[0] = 1;
  buf[1] = 2;
  buf[2] = 3;
  buf[3] = 4;
  buf[4] = 5;
  buf[5] = 6;
  buf[6] = 7;
  buf[7] = 8;
  buf[8] = 9;
  buf[9] = 10;
  buf[10] = 11;
  buf[11] = 12;
  buf[12] = 13;
  buf[13] = 14;
  buf[14] = 15;
  buf[15] = 16;
  buf[16] = 17;
  buf[17] = 18;
  buf[18] = 19;
  buf[19] = 20;
  buf[20] = 21;
  buf[21] = 22;
  buf[22] = 23;
  buf[23] = 24;
  buf[24] = 25;
  buf[25] = 26;
  buf[26] = 27;
  buf[27] = 28;
  buf[28] = 29;
  buf[29] = 30;
  buf[30] = 31;
  buf[31] = 32;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], c3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], c3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4Fourv2s_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4Fourv2s_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD4Fourv2s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == r0);
                     CHECK((uint64_t)fprState->v1 == r1);
                     CHECK((uint64_t)fprState->v2 == r2);
                     CHECK((uint64_t)fprState->v3 == r3);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4fourv4h") {
  const char source[] = "ld4 {v0.4h, v1.4h, v2.4h, v3.4h}, [x0]\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t c2 = 0x1817161514131211ULL;
  constexpr uint64_t c3 = 0x201f1e1d1c1b1a19ULL;
  constexpr uint64_t r0 = 0x1a1912110a090201ULL;
  constexpr uint64_t r1 = 0x1c1b14130c0b0403ULL;
  constexpr uint64_t r2 = 0x1e1d16150e0d0605ULL;
  constexpr uint64_t r3 = 0x201f1817100f0807ULL;
  uint8_t buf[40] = {0};
  buf[0] = 1;
  buf[1] = 2;
  buf[2] = 3;
  buf[3] = 4;
  buf[4] = 5;
  buf[5] = 6;
  buf[6] = 7;
  buf[7] = 8;
  buf[8] = 9;
  buf[9] = 10;
  buf[10] = 11;
  buf[11] = 12;
  buf[12] = 13;
  buf[13] = 14;
  buf[14] = 15;
  buf[15] = 16;
  buf[16] = 17;
  buf[17] = 18;
  buf[18] = 19;
  buf[19] = 20;
  buf[20] = 21;
  buf[21] = 22;
  buf[22] = 23;
  buf[23] = 24;
  buf[24] = 25;
  buf[25] = 26;
  buf[26] = 27;
  buf[27] = 28;
  buf[28] = 29;
  buf[29] = 30;
  buf[30] = 31;
  buf[31] = 32;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], c3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], c3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4Fourv4h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4Fourv4h", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD4Fourv4h", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == r0);
                     CHECK((uint64_t)fprState->v1 == r1);
                     CHECK((uint64_t)fprState->v2 == r2);
                     CHECK((uint64_t)fprState->v3 == r3);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4fourv4h_post") {
  const char source[] = "ld4 {v0.4h, v1.4h, v2.4h, v3.4h}, [x0], #32\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t c2 = 0x1817161514131211ULL;
  constexpr uint64_t c3 = 0x201f1e1d1c1b1a19ULL;
  constexpr uint64_t r0 = 0x1a1912110a090201ULL;
  constexpr uint64_t r1 = 0x1c1b14130c0b0403ULL;
  constexpr uint64_t r2 = 0x1e1d16150e0d0605ULL;
  constexpr uint64_t r3 = 0x201f1817100f0807ULL;
  uint8_t buf[40] = {0};
  buf[0] = 1;
  buf[1] = 2;
  buf[2] = 3;
  buf[3] = 4;
  buf[4] = 5;
  buf[5] = 6;
  buf[6] = 7;
  buf[7] = 8;
  buf[8] = 9;
  buf[9] = 10;
  buf[10] = 11;
  buf[11] = 12;
  buf[12] = 13;
  buf[13] = 14;
  buf[14] = 15;
  buf[15] = 16;
  buf[16] = 17;
  buf[17] = 18;
  buf[18] = 19;
  buf[19] = 20;
  buf[20] = 21;
  buf[21] = 22;
  buf[22] = 23;
  buf[23] = 24;
  buf[24] = 25;
  buf[25] = 26;
  buf[26] = 27;
  buf[27] = 28;
  buf[28] = 29;
  buf[29] = 30;
  buf[30] = 31;
  buf[31] = 32;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], c3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], c3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4Fourv4h_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4Fourv4h_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD4Fourv4h_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == r0);
                     CHECK((uint64_t)fprState->v1 == r1);
                     CHECK((uint64_t)fprState->v2 == r2);
                     CHECK((uint64_t)fprState->v3 == r3);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4fourv8b_post") {
  const char source[] = "ld4 {v0.8b, v1.8b, v2.8b, v3.8b}, [x0], #32\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t c2 = 0x1817161514131211ULL;
  constexpr uint64_t c3 = 0x201f1e1d1c1b1a19ULL;
  constexpr uint64_t r0 = 0x1d1915110d090501ULL;
  constexpr uint64_t r1 = 0x1e1a16120e0a0602ULL;
  constexpr uint64_t r2 = 0x1f1b17130f0b0703ULL;
  constexpr uint64_t r3 = 0x201c1814100c0804ULL;
  uint8_t buf[40] = {0};
  buf[0] = 1;
  buf[1] = 2;
  buf[2] = 3;
  buf[3] = 4;
  buf[4] = 5;
  buf[5] = 6;
  buf[6] = 7;
  buf[7] = 8;
  buf[8] = 9;
  buf[9] = 10;
  buf[10] = 11;
  buf[11] = 12;
  buf[12] = 13;
  buf[13] = 14;
  buf[14] = 15;
  buf[15] = 16;
  buf[16] = 17;
  buf[17] = 18;
  buf[18] = 19;
  buf[19] = 20;
  buf[20] = 21;
  buf[21] = 22;
  buf[22] = 23;
  buf[23] = 24;
  buf[24] = 25;
  buf[25] = 26;
  buf[26] = 27;
  buf[27] = 28;
  buf[28] = 29;
  buf[29] = 30;
  buf[30] = 31;
  buf[31] = 32;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], c3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], c3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4Fourv8b_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4Fourv8b_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("LD4Fourv8b_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == r0);
                     CHECK((uint64_t)fprState->v1 == r1);
                     CHECK((uint64_t)fprState->v2 == r2);
                     CHECK((uint64_t)fprState->v3 == r3);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4i64") {
  const char source[] = "ld4 {v0.d, v1.d, v2.d, v3.d}[0], [x0]\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  constexpr uint64_t e2 = 0x3333333333333333ULL;
  constexpr uint64_t e3 = 0x4444444444444444ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  *(uint64_t *)&buf[16] = e2;
  *(uint64_t *)&buf[24] = e3;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4i64", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4i64", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD4i64", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
                     CHECK((uint64_t)fprState->v2 == e2);
                     CHECK((uint64_t)fprState->v3 == e3);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4i64_post") {
  const char source[] = "ld4 {v0.d, v1.d, v2.d, v3.d}[0], [x0], #32\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  constexpr uint64_t e2 = 0x3333333333333333ULL;
  constexpr uint64_t e3 = 0x4444444444444444ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  *(uint64_t *)&buf[16] = e2;
  *(uint64_t *)&buf[24] = e3;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4i64_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4i64_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD4i64_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
                     CHECK((uint64_t)fprState->v2 == e2);
                     CHECK((uint64_t)fprState->v3 == e3);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4rv1d") {
  const char source[] = "ld4r {v0.1d, v1.1d, v2.1d, v3.1d}, [x0]\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  constexpr uint64_t e2 = 0x3333333333333333ULL;
  constexpr uint64_t e3 = 0x4444444444444444ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  *(uint64_t *)&buf[16] = e2;
  *(uint64_t *)&buf[24] = e3;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4Rv1d", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4Rv1d", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD4Rv1d", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
                     CHECK((uint64_t)fprState->v2 == e2);
                     CHECK((uint64_t)fprState->v3 == e3);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4rv1d_post") {
  const char source[] = "ld4r {v0.1d, v1.1d, v2.1d, v3.1d}, [x0], #32\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  constexpr uint64_t e2 = 0x3333333333333333ULL;
  constexpr uint64_t e3 = 0x4444444444444444ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  *(uint64_t *)&buf[16] = e2;
  *(uint64_t *)&buf[24] = e3;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4Rv1d_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4Rv1d_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD4Rv1d_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
                     CHECK((uint64_t)fprState->v2 == e2);
                     CHECK((uint64_t)fprState->v3 == e3);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4rv2d") {
  const char source[] = "ld4r {v0.2d, v1.2d, v2.2d, v3.2d}, [x0]\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  constexpr uint64_t e2 = 0x3333333333333333ULL;
  constexpr uint64_t e3 = 0x4444444444444444ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  *(uint64_t *)&buf[16] = e2;
  *(uint64_t *)&buf[24] = e3;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4Rv2d", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4Rv2d", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD4Rv2d", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
                     CHECK((uint64_t)fprState->v2 == e2);
                     CHECK((uint64_t)fprState->v3 == e3);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4rv2d_post") {
  const char source[] = "ld4r {v0.2d, v1.2d, v2.2d, v3.2d}, [x0], #32\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  constexpr uint64_t e2 = 0x3333333333333333ULL;
  constexpr uint64_t e3 = 0x4444444444444444ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = e0;
  *(uint64_t *)&buf[8] = e1;
  *(uint64_t *)&buf[16] = e2;
  *(uint64_t *)&buf[24] = e3;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4Rv2d_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4Rv2d_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD4Rv2d_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == e0);
                     CHECK((uint64_t)fprState->v1 == e1);
                     CHECK((uint64_t)fprState->v2 == e2);
                     CHECK((uint64_t)fprState->v3 == e3);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1fourv1d") {
  const char source[] = "st1 {v0.1d, v1.1d, v2.1d, v3.1d}, [x0]\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  constexpr uint64_t e1 = 0x6666666666666666ULL;
  constexpr uint64_t e2 = 0x7777777777777777ULL;
  constexpr uint64_t e3 = 0x8888888888888888ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Fourv1d", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Fourv1d", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = e0;
  fpr->v1 = e1;
  fpr->v2 = e2;
  fpr->v3 = e3;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == e0);
  CHECK(*(uint64_t *)&buf[8] == e1);
  CHECK(*(uint64_t *)&buf[16] == e2);
  CHECK(*(uint64_t *)&buf[24] == e3);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1fourv1d_post") {
  const char source[] = "st1 {v0.1d, v1.1d, v2.1d, v3.1d}, [x0], #32\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  constexpr uint64_t e1 = 0x6666666666666666ULL;
  constexpr uint64_t e2 = 0x7777777777777777ULL;
  constexpr uint64_t e3 = 0x8888888888888888ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Fourv1d_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Fourv1d_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Fourv1d_POST", QBDI::POSTINST,
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
  fpr->v0 = e0;
  fpr->v1 = e1;
  fpr->v2 = e2;
  fpr->v3 = e3;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == e0);
  CHECK(*(uint64_t *)&buf[8] == e1);
  CHECK(*(uint64_t *)&buf[16] == e2);
  CHECK(*(uint64_t *)&buf[24] == e3);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1fourv2s") {
  const char source[] = "st1 {v0.2s, v1.2s, v2.2s, v3.2s}, [x0]\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  constexpr uint64_t e1 = 0x6666666666666666ULL;
  constexpr uint64_t e2 = 0x7777777777777777ULL;
  constexpr uint64_t e3 = 0x8888888888888888ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Fourv2s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Fourv2s", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = e0;
  fpr->v1 = e1;
  fpr->v2 = e2;
  fpr->v3 = e3;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == e0);
  CHECK(*(uint64_t *)&buf[8] == e1);
  CHECK(*(uint64_t *)&buf[16] == e2);
  CHECK(*(uint64_t *)&buf[24] == e3);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1fourv2s_post") {
  const char source[] = "st1 {v0.2s, v1.2s, v2.2s, v3.2s}, [x0], #32\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  constexpr uint64_t e1 = 0x6666666666666666ULL;
  constexpr uint64_t e2 = 0x7777777777777777ULL;
  constexpr uint64_t e3 = 0x8888888888888888ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Fourv2s_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Fourv2s_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Fourv2s_POST", QBDI::POSTINST,
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
  fpr->v0 = e0;
  fpr->v1 = e1;
  fpr->v2 = e2;
  fpr->v3 = e3;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == e0);
  CHECK(*(uint64_t *)&buf[8] == e1);
  CHECK(*(uint64_t *)&buf[16] == e2);
  CHECK(*(uint64_t *)&buf[24] == e3);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1fourv4h") {
  const char source[] = "st1 {v0.4h, v1.4h, v2.4h, v3.4h}, [x0]\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  constexpr uint64_t e1 = 0x6666666666666666ULL;
  constexpr uint64_t e2 = 0x7777777777777777ULL;
  constexpr uint64_t e3 = 0x8888888888888888ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Fourv4h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Fourv4h", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = e0;
  fpr->v1 = e1;
  fpr->v2 = e2;
  fpr->v3 = e3;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == e0);
  CHECK(*(uint64_t *)&buf[8] == e1);
  CHECK(*(uint64_t *)&buf[16] == e2);
  CHECK(*(uint64_t *)&buf[24] == e3);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1fourv4h_post") {
  const char source[] = "st1 {v0.4h, v1.4h, v2.4h, v3.4h}, [x0], #32\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  constexpr uint64_t e1 = 0x6666666666666666ULL;
  constexpr uint64_t e2 = 0x7777777777777777ULL;
  constexpr uint64_t e3 = 0x8888888888888888ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Fourv4h_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Fourv4h_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Fourv4h_POST", QBDI::POSTINST,
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
  fpr->v0 = e0;
  fpr->v1 = e1;
  fpr->v2 = e2;
  fpr->v3 = e3;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == e0);
  CHECK(*(uint64_t *)&buf[8] == e1);
  CHECK(*(uint64_t *)&buf[16] == e2);
  CHECK(*(uint64_t *)&buf[24] == e3);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1fourv8b") {
  const char source[] = "st1 {v0.8b, v1.8b, v2.8b, v3.8b}, [x0]\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  constexpr uint64_t e1 = 0x6666666666666666ULL;
  constexpr uint64_t e2 = 0x7777777777777777ULL;
  constexpr uint64_t e3 = 0x8888888888888888ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Fourv8b", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Fourv8b", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = e0;
  fpr->v1 = e1;
  fpr->v2 = e2;
  fpr->v3 = e3;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == e0);
  CHECK(*(uint64_t *)&buf[8] == e1);
  CHECK(*(uint64_t *)&buf[16] == e2);
  CHECK(*(uint64_t *)&buf[24] == e3);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1fourv8b_post") {
  const char source[] = "st1 {v0.8b, v1.8b, v2.8b, v3.8b}, [x0], #32\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  constexpr uint64_t e1 = 0x6666666666666666ULL;
  constexpr uint64_t e2 = 0x7777777777777777ULL;
  constexpr uint64_t e3 = 0x8888888888888888ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Fourv8b_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Fourv8b_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Fourv8b_POST", QBDI::POSTINST,
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
  fpr->v0 = e0;
  fpr->v1 = e1;
  fpr->v2 = e2;
  fpr->v3 = e3;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == e0);
  CHECK(*(uint64_t *)&buf[8] == e1);
  CHECK(*(uint64_t *)&buf[16] == e2);
  CHECK(*(uint64_t *)&buf[24] == e3);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1i64") {
  const char source[] = "st1 {v0.d}[0], [x0]\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1i64", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1i64", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = e0;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == e0);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1i64_post") {
  const char source[] = "st1 {v0.d}[0], [x0], #8\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1i64_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1i64_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("ST1i64_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 8);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = e0;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == e0);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1onev1d") {
  const char source[] = "st1 {v0.1d}, [x0]\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Onev1d", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Onev1d", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = e0;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == e0);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1onev1d_post") {
  const char source[] = "st1 {v0.1d}, [x0], #8\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Onev1d_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Onev1d_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Onev1d_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 8);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = e0;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == e0);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1onev2s") {
  const char source[] = "st1 {v0.2s}, [x0]\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Onev2s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Onev2s", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = e0;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == e0);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1onev2s_post") {
  const char source[] = "st1 {v0.2s}, [x0], #8\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Onev2s_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Onev2s_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Onev2s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 8);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = e0;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == e0);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1onev4h") {
  const char source[] = "st1 {v0.4h}, [x0]\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Onev4h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Onev4h", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = e0;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == e0);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1onev4h_post") {
  const char source[] = "st1 {v0.4h}, [x0], #8\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Onev4h_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Onev4h_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Onev4h_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 8);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = e0;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == e0);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1onev8b") {
  const char source[] = "st1 {v0.8b}, [x0]\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Onev8b", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Onev8b", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = e0;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == e0);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1onev8b_post") {
  const char source[] = "st1 {v0.8b}, [x0], #8\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Onev8b_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Onev8b_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Onev8b_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 8);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = e0;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == e0);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1threev1d") {
  const char source[] = "st1 {v0.1d, v1.1d, v2.1d}, [x0]\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  constexpr uint64_t e1 = 0x6666666666666666ULL;
  constexpr uint64_t e2 = 0x7777777777777777ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Threev1d", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Threev1d", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = e0;
  fpr->v1 = e1;
  fpr->v2 = e2;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == e0);
  CHECK(*(uint64_t *)&buf[8] == e1);
  CHECK(*(uint64_t *)&buf[16] == e2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1threev1d_post") {
  const char source[] = "st1 {v0.1d, v1.1d, v2.1d}, [x0], #24\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  constexpr uint64_t e1 = 0x6666666666666666ULL;
  constexpr uint64_t e2 = 0x7777777777777777ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Threev1d_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("ST1Threev1d_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Threev1d_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 24);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = e0;
  fpr->v1 = e1;
  fpr->v2 = e2;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == e0);
  CHECK(*(uint64_t *)&buf[8] == e1);
  CHECK(*(uint64_t *)&buf[16] == e2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1threev2s") {
  const char source[] = "st1 {v0.2s, v1.2s, v2.2s}, [x0]\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  constexpr uint64_t e1 = 0x6666666666666666ULL;
  constexpr uint64_t e2 = 0x7777777777777777ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Threev2s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Threev2s", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = e0;
  fpr->v1 = e1;
  fpr->v2 = e2;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == e0);
  CHECK(*(uint64_t *)&buf[8] == e1);
  CHECK(*(uint64_t *)&buf[16] == e2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1threev2s_post") {
  const char source[] = "st1 {v0.2s, v1.2s, v2.2s}, [x0], #24\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  constexpr uint64_t e1 = 0x6666666666666666ULL;
  constexpr uint64_t e2 = 0x7777777777777777ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Threev2s_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("ST1Threev2s_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Threev2s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 24);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = e0;
  fpr->v1 = e1;
  fpr->v2 = e2;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == e0);
  CHECK(*(uint64_t *)&buf[8] == e1);
  CHECK(*(uint64_t *)&buf[16] == e2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1threev4h") {
  const char source[] = "st1 {v0.4h, v1.4h, v2.4h}, [x0]\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  constexpr uint64_t e1 = 0x6666666666666666ULL;
  constexpr uint64_t e2 = 0x7777777777777777ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Threev4h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Threev4h", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = e0;
  fpr->v1 = e1;
  fpr->v2 = e2;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == e0);
  CHECK(*(uint64_t *)&buf[8] == e1);
  CHECK(*(uint64_t *)&buf[16] == e2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1threev4h_post") {
  const char source[] = "st1 {v0.4h, v1.4h, v2.4h}, [x0], #24\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  constexpr uint64_t e1 = 0x6666666666666666ULL;
  constexpr uint64_t e2 = 0x7777777777777777ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Threev4h_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("ST1Threev4h_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Threev4h_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 24);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = e0;
  fpr->v1 = e1;
  fpr->v2 = e2;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == e0);
  CHECK(*(uint64_t *)&buf[8] == e1);
  CHECK(*(uint64_t *)&buf[16] == e2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1twov1d") {
  const char source[] = "st1 {v0.1d, v1.1d}, [x0]\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  constexpr uint64_t e1 = 0x6666666666666666ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Twov1d", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Twov1d", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = e0;
  fpr->v1 = e1;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == e0);
  CHECK(*(uint64_t *)&buf[8] == e1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1twov1d_post") {
  const char source[] = "st1 {v0.1d, v1.1d}, [x0], #16\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  constexpr uint64_t e1 = 0x6666666666666666ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Twov1d_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Twov1d_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Twov1d_POST", QBDI::POSTINST,
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
  fpr->v0 = e0;
  fpr->v1 = e1;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == e0);
  CHECK(*(uint64_t *)&buf[8] == e1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1twov2s") {
  const char source[] = "st1 {v0.2s, v1.2s}, [x0]\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  constexpr uint64_t e1 = 0x6666666666666666ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Twov2s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Twov2s", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = e0;
  fpr->v1 = e1;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == e0);
  CHECK(*(uint64_t *)&buf[8] == e1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1twov2s_post") {
  const char source[] = "st1 {v0.2s, v1.2s}, [x0], #16\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  constexpr uint64_t e1 = 0x6666666666666666ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Twov2s_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Twov2s_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Twov2s_POST", QBDI::POSTINST,
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
  fpr->v0 = e0;
  fpr->v1 = e1;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == e0);
  CHECK(*(uint64_t *)&buf[8] == e1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1twov4h") {
  const char source[] = "st1 {v0.4h, v1.4h}, [x0]\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  constexpr uint64_t e1 = 0x6666666666666666ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Twov4h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Twov4h", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = e0;
  fpr->v1 = e1;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == e0);
  CHECK(*(uint64_t *)&buf[8] == e1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1twov4h_post") {
  const char source[] = "st1 {v0.4h, v1.4h}, [x0], #16\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  constexpr uint64_t e1 = 0x6666666666666666ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Twov4h_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Twov4h_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Twov4h_POST", QBDI::POSTINST,
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
  fpr->v0 = e0;
  fpr->v1 = e1;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == e0);
  CHECK(*(uint64_t *)&buf[8] == e1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1twov8b") {
  const char source[] = "st1 {v0.8b, v1.8b}, [x0]\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  constexpr uint64_t e1 = 0x6666666666666666ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1Twov8b", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Twov8b", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = e0;
  fpr->v1 = e1;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == e0);
  CHECK(*(uint64_t *)&buf[8] == e1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1twov8b_post") {
  const char source[] = "st1 {v0.8b, v1.8b}, [x0], #16\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  constexpr uint64_t e1 = 0x6666666666666666ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1Twov8b_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1Twov8b_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST1Twov8b_POST", QBDI::POSTINST,
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
  fpr->v0 = e0;
  fpr->v1 = e1;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == e0);
  CHECK(*(uint64_t *)&buf[8] == e1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st2i64") {
  const char source[] = "st2 {v0.d, v1.d}[0], [x0]\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  constexpr uint64_t e1 = 0x6666666666666666ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST2i64", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST2i64", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = e0;
  fpr->v1 = e1;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == e0);
  CHECK(*(uint64_t *)&buf[8] == e1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st2i64_post") {
  const char source[] = "st2 {v0.d, v1.d}[0], [x0], #16\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  constexpr uint64_t e1 = 0x6666666666666666ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST2i64_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST2i64_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("ST2i64_POST", QBDI::POSTINST,
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
  fpr->v0 = e0;
  fpr->v1 = e1;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == e0);
  CHECK(*(uint64_t *)&buf[8] == e1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st2twov2s") {
  const char source[] = "st2 {v0.2s, v1.2s}, [x0]\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t r0 = 0xc0b0a0904030201ULL;
  constexpr uint64_t r1 = 0x100f0e0d08070605ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST2Twov2s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST2Twov2s", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = r0;
  fpr->v1 = r1;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == c0);
  CHECK(*(uint64_t *)&buf[8] == c1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st2twov2s_post") {
  const char source[] = "st2 {v0.2s, v1.2s}, [x0], #16\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t r0 = 0xc0b0a0904030201ULL;
  constexpr uint64_t r1 = 0x100f0e0d08070605ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST2Twov2s_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST2Twov2s_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST2Twov2s_POST", QBDI::POSTINST,
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
  fpr->v0 = r0;
  fpr->v1 = r1;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == c0);
  CHECK(*(uint64_t *)&buf[8] == c1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st2twov4h") {
  const char source[] = "st2 {v0.4h, v1.4h}, [x0]\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t r0 = 0xe0d0a0906050201ULL;
  constexpr uint64_t r1 = 0x100f0c0b08070403ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST2Twov4h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST2Twov4h", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = r0;
  fpr->v1 = r1;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == c0);
  CHECK(*(uint64_t *)&buf[8] == c1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st2twov4h_post") {
  const char source[] = "st2 {v0.4h, v1.4h}, [x0], #16\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t r0 = 0xe0d0a0906050201ULL;
  constexpr uint64_t r1 = 0x100f0c0b08070403ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST2Twov4h_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST2Twov4h_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST2Twov4h_POST", QBDI::POSTINST,
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
  fpr->v0 = r0;
  fpr->v1 = r1;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == c0);
  CHECK(*(uint64_t *)&buf[8] == c1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st2twov8b") {
  const char source[] = "st2 {v0.8b, v1.8b}, [x0]\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t r0 = 0xf0d0b0907050301ULL;
  constexpr uint64_t r1 = 0x100e0c0a08060402ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST2Twov8b", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST2Twov8b", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = r0;
  fpr->v1 = r1;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == c0);
  CHECK(*(uint64_t *)&buf[8] == c1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st2twov8b_post") {
  const char source[] = "st2 {v0.8b, v1.8b}, [x0], #16\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t r0 = 0xf0d0b0907050301ULL;
  constexpr uint64_t r1 = 0x100e0c0a08060402ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST2Twov8b_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST2Twov8b_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST2Twov8b_POST", QBDI::POSTINST,
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
  fpr->v0 = r0;
  fpr->v1 = r1;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == c0);
  CHECK(*(uint64_t *)&buf[8] == c1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st3i64") {
  const char source[] = "st3 {v0.d, v1.d, v2.d}[0], [x0]\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  constexpr uint64_t e1 = 0x6666666666666666ULL;
  constexpr uint64_t e2 = 0x7777777777777777ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST3i64", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST3i64", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = e0;
  fpr->v1 = e1;
  fpr->v2 = e2;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == e0);
  CHECK(*(uint64_t *)&buf[8] == e1);
  CHECK(*(uint64_t *)&buf[16] == e2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st3i64_post") {
  const char source[] = "st3 {v0.d, v1.d, v2.d}[0], [x0], #24\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  constexpr uint64_t e1 = 0x6666666666666666ULL;
  constexpr uint64_t e2 = 0x7777777777777777ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST3i64_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST3i64_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("ST3i64_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 24);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = e0;
  fpr->v1 = e1;
  fpr->v2 = e2;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == e0);
  CHECK(*(uint64_t *)&buf[8] == e1);
  CHECK(*(uint64_t *)&buf[16] == e2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st3threev2s") {
  const char source[] = "st3 {v0.2s, v1.2s, v2.2s}, [x0]\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t c2 = 0x1817161514131211ULL;
  constexpr uint64_t r0 = 0x100f0e0d04030201ULL;
  constexpr uint64_t r1 = 0x1413121108070605ULL;
  constexpr uint64_t r2 = 0x181716150c0b0a09ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST3Threev2s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST3Threev2s", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = r0;
  fpr->v1 = r1;
  fpr->v2 = r2;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == c0);
  CHECK(*(uint64_t *)&buf[8] == c1);
  CHECK(*(uint64_t *)&buf[16] == c2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st3threev2s_post") {
  const char source[] = "st3 {v0.2s, v1.2s, v2.2s}, [x0], #24\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t c2 = 0x1817161514131211ULL;
  constexpr uint64_t r0 = 0x100f0e0d04030201ULL;
  constexpr uint64_t r1 = 0x1413121108070605ULL;
  constexpr uint64_t r2 = 0x181716150c0b0a09ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST3Threev2s_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("ST3Threev2s_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST3Threev2s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 24);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = r0;
  fpr->v1 = r1;
  fpr->v2 = r2;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == c0);
  CHECK(*(uint64_t *)&buf[8] == c1);
  CHECK(*(uint64_t *)&buf[16] == c2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st3threev4h") {
  const char source[] = "st3 {v0.4h, v1.4h, v2.4h}, [x0]\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t c2 = 0x1817161514131211ULL;
  constexpr uint64_t r0 = 0x14130e0d08070201ULL;
  constexpr uint64_t r1 = 0x1615100f0a090403ULL;
  constexpr uint64_t r2 = 0x181712110c0b0605ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST3Threev4h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST3Threev4h", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = r0;
  fpr->v1 = r1;
  fpr->v2 = r2;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == c0);
  CHECK(*(uint64_t *)&buf[8] == c1);
  CHECK(*(uint64_t *)&buf[16] == c2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st3threev4h_post") {
  const char source[] = "st3 {v0.4h, v1.4h, v2.4h}, [x0], #24\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t c2 = 0x1817161514131211ULL;
  constexpr uint64_t r0 = 0x14130e0d08070201ULL;
  constexpr uint64_t r1 = 0x1615100f0a090403ULL;
  constexpr uint64_t r2 = 0x181712110c0b0605ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST3Threev4h_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("ST3Threev4h_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST3Threev4h_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 24);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = r0;
  fpr->v1 = r1;
  fpr->v2 = r2;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == c0);
  CHECK(*(uint64_t *)&buf[8] == c1);
  CHECK(*(uint64_t *)&buf[16] == c2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st3threev8b") {
  const char source[] = "st3 {v0.8b, v1.8b, v2.8b}, [x0]\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t c2 = 0x1817161514131211ULL;
  constexpr uint64_t r0 = 0x1613100d0a070401ULL;
  constexpr uint64_t r1 = 0x1714110e0b080502ULL;
  constexpr uint64_t r2 = 0x1815120f0c090603ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST3Threev8b", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST3Threev8b", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = r0;
  fpr->v1 = r1;
  fpr->v2 = r2;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == c0);
  CHECK(*(uint64_t *)&buf[8] == c1);
  CHECK(*(uint64_t *)&buf[16] == c2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st3threev8b_post") {
  const char source[] = "st3 {v0.8b, v1.8b, v2.8b}, [x0], #24\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t c2 = 0x1817161514131211ULL;
  constexpr uint64_t r0 = 0x1613100d0a070401ULL;
  constexpr uint64_t r1 = 0x1714110e0b080502ULL;
  constexpr uint64_t r2 = 0x1815120f0c090603ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST3Threev8b_POST", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("ST3Threev8b_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST3Threev8b_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 24);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = r0;
  fpr->v1 = r1;
  fpr->v2 = r2;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == c0);
  CHECK(*(uint64_t *)&buf[8] == c1);
  CHECK(*(uint64_t *)&buf[16] == c2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st4fourv2s") {
  const char source[] = "st4 {v0.2s, v1.2s, v2.2s, v3.2s}, [x0]\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t c2 = 0x1817161514131211ULL;
  constexpr uint64_t c3 = 0x201f1e1d1c1b1a19ULL;
  constexpr uint64_t r0 = 0x1413121104030201ULL;
  constexpr uint64_t r1 = 0x1817161508070605ULL;
  constexpr uint64_t r2 = 0x1c1b1a190c0b0a09ULL;
  constexpr uint64_t r3 = 0x201f1e1d100f0e0dULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], c3, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST4Fourv2s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST4Fourv2s", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = r0;
  fpr->v1 = r1;
  fpr->v2 = r2;
  fpr->v3 = r3;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == c0);
  CHECK(*(uint64_t *)&buf[8] == c1);
  CHECK(*(uint64_t *)&buf[16] == c2);
  CHECK(*(uint64_t *)&buf[24] == c3);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st4fourv2s_post") {
  const char source[] = "st4 {v0.2s, v1.2s, v2.2s, v3.2s}, [x0], #32\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t c2 = 0x1817161514131211ULL;
  constexpr uint64_t c3 = 0x201f1e1d1c1b1a19ULL;
  constexpr uint64_t r0 = 0x1413121104030201ULL;
  constexpr uint64_t r1 = 0x1817161508070605ULL;
  constexpr uint64_t r2 = 0x1c1b1a190c0b0a09ULL;
  constexpr uint64_t r3 = 0x201f1e1d100f0e0dULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], c3, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST4Fourv2s_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST4Fourv2s_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST4Fourv2s_POST", QBDI::POSTINST,
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
  fpr->v0 = r0;
  fpr->v1 = r1;
  fpr->v2 = r2;
  fpr->v3 = r3;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == c0);
  CHECK(*(uint64_t *)&buf[8] == c1);
  CHECK(*(uint64_t *)&buf[16] == c2);
  CHECK(*(uint64_t *)&buf[24] == c3);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st4fourv4h") {
  const char source[] = "st4 {v0.4h, v1.4h, v2.4h, v3.4h}, [x0]\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t c2 = 0x1817161514131211ULL;
  constexpr uint64_t c3 = 0x201f1e1d1c1b1a19ULL;
  constexpr uint64_t r0 = 0x1a1912110a090201ULL;
  constexpr uint64_t r1 = 0x1c1b14130c0b0403ULL;
  constexpr uint64_t r2 = 0x1e1d16150e0d0605ULL;
  constexpr uint64_t r3 = 0x201f1817100f0807ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], c3, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST4Fourv4h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST4Fourv4h", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = r0;
  fpr->v1 = r1;
  fpr->v2 = r2;
  fpr->v3 = r3;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == c0);
  CHECK(*(uint64_t *)&buf[8] == c1);
  CHECK(*(uint64_t *)&buf[16] == c2);
  CHECK(*(uint64_t *)&buf[24] == c3);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st4fourv4h_post") {
  const char source[] = "st4 {v0.4h, v1.4h, v2.4h, v3.4h}, [x0], #32\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t c2 = 0x1817161514131211ULL;
  constexpr uint64_t c3 = 0x201f1e1d1c1b1a19ULL;
  constexpr uint64_t r0 = 0x1a1912110a090201ULL;
  constexpr uint64_t r1 = 0x1c1b14130c0b0403ULL;
  constexpr uint64_t r2 = 0x1e1d16150e0d0605ULL;
  constexpr uint64_t r3 = 0x201f1817100f0807ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], c3, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST4Fourv4h_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST4Fourv4h_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST4Fourv4h_POST", QBDI::POSTINST,
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
  fpr->v0 = r0;
  fpr->v1 = r1;
  fpr->v2 = r2;
  fpr->v3 = r3;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == c0);
  CHECK(*(uint64_t *)&buf[8] == c1);
  CHECK(*(uint64_t *)&buf[16] == c2);
  CHECK(*(uint64_t *)&buf[24] == c3);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st4fourv8b") {
  const char source[] = "st4 {v0.8b, v1.8b, v2.8b, v3.8b}, [x0]\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t c2 = 0x1817161514131211ULL;
  constexpr uint64_t c3 = 0x201f1e1d1c1b1a19ULL;
  constexpr uint64_t r0 = 0x1d1915110d090501ULL;
  constexpr uint64_t r1 = 0x1e1a16120e0a0602ULL;
  constexpr uint64_t r2 = 0x1f1b17130f0b0703ULL;
  constexpr uint64_t r3 = 0x201c1814100c0804ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], c3, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST4Fourv8b", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST4Fourv8b", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = r0;
  fpr->v1 = r1;
  fpr->v2 = r2;
  fpr->v3 = r3;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == c0);
  CHECK(*(uint64_t *)&buf[8] == c1);
  CHECK(*(uint64_t *)&buf[16] == c2);
  CHECK(*(uint64_t *)&buf[24] == c3);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st4fourv8b_post") {
  const char source[] = "st4 {v0.8b, v1.8b, v2.8b, v3.8b}, [x0], #32\n";

  constexpr uint64_t c0 = 0x807060504030201ULL;
  constexpr uint64_t c1 = 0x100f0e0d0c0b0a09ULL;
  constexpr uint64_t c2 = 0x1817161514131211ULL;
  constexpr uint64_t c3 = 0x201f1e1d1c1b1a19ULL;
  constexpr uint64_t r0 = 0x1d1915110d090501ULL;
  constexpr uint64_t r1 = 0x1e1a16120e0a0602ULL;
  constexpr uint64_t r2 = 0x1f1b17130f0b0703ULL;
  constexpr uint64_t r3 = 0x201c1814100c0804ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], c0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], c1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], c2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], c3, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST4Fourv8b_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST4Fourv8b_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("ST4Fourv8b_POST", QBDI::POSTINST,
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
  fpr->v0 = r0;
  fpr->v1 = r1;
  fpr->v2 = r2;
  fpr->v3 = r3;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == c0);
  CHECK(*(uint64_t *)&buf[8] == c1);
  CHECK(*(uint64_t *)&buf[16] == c2);
  CHECK(*(uint64_t *)&buf[24] == c3);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st4i64") {
  const char source[] = "st4 {v0.d, v1.d, v2.d, v3.d}[0], [x0]\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  constexpr uint64_t e1 = 0x6666666666666666ULL;
  constexpr uint64_t e2 = 0x7777777777777777ULL;
  constexpr uint64_t e3 = 0x8888888888888888ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST4i64", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST4i64", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = e0;
  fpr->v1 = e1;
  fpr->v2 = e2;
  fpr->v3 = e3;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == e0);
  CHECK(*(uint64_t *)&buf[8] == e1);
  CHECK(*(uint64_t *)&buf[16] == e2);
  CHECK(*(uint64_t *)&buf[24] == e3);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st4i64_post") {
  const char source[] = "st4 {v0.d, v1.d, v2.d, v3.d}[0], [x0], #32\n";

  constexpr uint64_t e0 = 0x5555555555555555ULL;
  constexpr uint64_t e1 = 0x6666666666666666ULL;
  constexpr uint64_t e2 = 0x7777777777777777ULL;
  constexpr uint64_t e3 = 0x8888888888888888ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], e2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], e3, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST4i64_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST4i64_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("ST4i64_POST", QBDI::POSTINST,
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
  fpr->v0 = e0;
  fpr->v1 = e1;
  fpr->v2 = e2;
  fpr->v3 = e3;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == e0);
  CHECK(*(uint64_t *)&buf[8] == e1);
  CHECK(*(uint64_t *)&buf[16] == e2);
  CHECK(*(uint64_t *)&buf[24] == e3);
}
