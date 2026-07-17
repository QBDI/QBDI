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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrhh_post") {
  const char source[] = "ldrh w1, [x0], #5\n";

  constexpr uint16_t expected = 0xa1c2;
  uint8_t v[] = {(uint8_t)((0xa1c2 >> 0) & 0xff),
                 (uint8_t)((0xa1c2 >> 8) & 0xff),
                 0,
                 0,
                 0,
                 0,
                 0,
                 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRHHpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRHHpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRHHpost", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffff) == expected);
                     CHECK(gprState->x0 == (QBDI::rword)&v[0] + 5);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrhh_pre") {
  const char source[] = "ldrh w1, [x0, #5]!\n";

  constexpr uint16_t expected = 0xa2c3;
  uint8_t v[] = {0,
                 0,
                 0,
                 0,
                 0,
                 (uint8_t)((0xa2c3 >> 0) & 0xff),
                 (uint8_t)((0xa2c3 >> 8) & 0xff),
                 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[5], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRHHpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRHHpre", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRHHpre", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffff) == expected);
                     CHECK(gprState->x0 == (QBDI::rword)&v[5]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrhh_ui") {
  const char source[] = "ldrh w1, [x0, #10]\n";

  constexpr uint16_t expected = 0xa3c4;
  uint8_t v[] = {0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 (uint8_t)((0xa3c4 >> 0) & 0xff),
                 (uint8_t)((0xa3c4 >> 8) & 0xff),
                 0,
                 0,
                 0,
                 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[10], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[10], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRHHui", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRHHui", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRHHui", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffff) == expected);
                     CHECK(gprState->x0 == (QBDI::rword)&v[0]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strhh_post") {
  const char source[] = "strh w1, [x0], #5\n";

  uint8_t v[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], 0xb1c1, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STRHHpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRHHpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STRHHpost", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&v[0] + 5);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = 0xb1c1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint16_t *)&v[0] == 0xb1c1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strhh_pre") {
  const char source[] = "strh w1, [x0, #5]!\n";

  uint8_t v[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], 0xb2c2, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STRHHpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRHHpre", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STRHHpre", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&v[5]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = 0xb2c2;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint16_t *)&v[5] == 0xb2c2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strhh_ui") {
  const char source[] = "strh w1, [x0, #10]\n";

  uint8_t v[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[10], 0xb3c3, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STRHHui", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRHHui", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STRHHui", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&v[0]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = 0xb3c3;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint16_t *)&v[10] == 0xb3c3);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrh_fpr_post") {
  const char source[] = "ldr h1, [x0], #5\n";

  constexpr uint16_t expected = 0xc1d1;
  uint8_t v[] = {(uint8_t)((0xc1d1 >> 0) & 0xff),
                 (uint8_t)((0xc1d1 >> 8) & 0xff),
                 0,
                 0,
                 0,
                 0,
                 0,
                 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRHpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRHpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRHpost", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint16_t)fprState->v1 == expected);
                     CHECK(gprState->x0 == (QBDI::rword)&v[0] + 5);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrh_fpr_pre") {
  const char source[] = "ldr h1, [x0, #5]!\n";

  constexpr uint16_t expected = 0xc2d2;
  uint8_t v[] = {0,
                 0,
                 0,
                 0,
                 0,
                 (uint8_t)((0xc2d2 >> 0) & 0xff),
                 (uint8_t)((0xc2d2 >> 8) & 0xff),
                 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[5], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRHpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRHpre", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRHpre", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint16_t)fprState->v1 == expected);
                     CHECK(gprState->x0 == (QBDI::rword)&v[5]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrh_fpr_ui") {
  const char source[] = "ldr h1, [x0, #10]\n";

  constexpr uint16_t expected = 0xc3d3;
  uint8_t v[] = {0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 (uint8_t)((0xc3d3 >> 0) & 0xff),
                 (uint8_t)((0xc3d3 >> 8) & 0xff),
                 0,
                 0,
                 0,
                 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[10], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[10], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRHui", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRHui", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRHui", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint16_t)fprState->v1 == expected);
                     CHECK(gprState->x0 == (QBDI::rword)&v[0]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strh_fpr_post") {
  const char source[] = "str h1, [x0], #5\n";

  uint8_t v[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], 0xd1e1, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STRHpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRHpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STRHpost", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&v[0] + 5);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = 0xd1e1;
  vm.setFPRState(fpr);
  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint16_t *)&v[0] == 0xd1e1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strh_fpr_pre") {
  const char source[] = "str h1, [x0, #5]!\n";

  uint8_t v[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], 0xd2e2, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STRHpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRHpre", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STRHpre", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&v[5]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = 0xd2e2;
  vm.setFPRState(fpr);
  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint16_t *)&v[5] == 0xd2e2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strh_fpr_ui") {
  const char source[] = "str h1, [x0, #10]\n";

  uint8_t v[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[10], 0xd3e3, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STRHui", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRHui", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STRHui", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&v[0]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = 0xd3e3;
  vm.setFPRState(fpr);
  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint16_t *)&v[10] == 0xd3e3);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrshw_post") {
  const char source[] = "ldrsh w1, [x0], #5\n";

  constexpr uint16_t expected = 0x9012;
  uint8_t v[] = {(uint8_t)((0x9012 >> 0) & 0xff),
                 (uint8_t)((0x9012 >> 8) & 0xff),
                 0,
                 0,
                 0,
                 0,
                 0,
                 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRSHWpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRSHWpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRSHWpost", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffffffff) == 0xffff9012);
                     CHECK(gprState->x0 == (QBDI::rword)&v[0] + 5);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrshw_pre") {
  const char source[] = "ldrsh w1, [x0, #5]!\n";

  constexpr uint16_t expected = 0x9112;
  uint8_t v[] = {0,
                 0,
                 0,
                 0,
                 0,
                 (uint8_t)((0x9112 >> 0) & 0xff),
                 (uint8_t)((0x9112 >> 8) & 0xff),
                 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[5], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRSHWpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRSHWpre", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRSHWpre", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffffffff) == 0xffff9112);
                     CHECK(gprState->x0 == (QBDI::rword)&v[5]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrshw_ui") {
  const char source[] = "ldrsh w1, [x0, #10]\n";

  constexpr uint16_t expected = 0x9212;
  uint8_t v[] = {0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 (uint8_t)((0x9212 >> 0) & 0xff),
                 (uint8_t)((0x9212 >> 8) & 0xff),
                 0,
                 0,
                 0,
                 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[10], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[10], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRSHWui", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRSHWui", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRSHWui", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffffffff) == 0xffff9212);
                     CHECK(gprState->x0 == (QBDI::rword)&v[0]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrshx_post") {
  const char source[] = "ldrsh x1, [x0], #5\n";

  constexpr uint16_t expected = 0x9312;
  uint8_t v[] = {(uint8_t)((0x9312 >> 0) & 0xff),
                 (uint8_t)((0x9312 >> 8) & 0xff),
                 0,
                 0,
                 0,
                 0,
                 0,
                 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRSHXpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRSHXpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRSHXpost", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == 0xffffffffffff9312ULL);
                     CHECK(gprState->x0 == (QBDI::rword)&v[0] + 5);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrshx_pre") {
  const char source[] = "ldrsh x1, [x0, #5]!\n";

  constexpr uint16_t expected = 0x9412;
  uint8_t v[] = {0,
                 0,
                 0,
                 0,
                 0,
                 (uint8_t)((0x9412 >> 0) & 0xff),
                 (uint8_t)((0x9412 >> 8) & 0xff),
                 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[5], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRSHXpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRSHXpre", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRSHXpre", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == 0xffffffffffff9412ULL);
                     CHECK(gprState->x0 == (QBDI::rword)&v[5]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrshx_ui") {
  const char source[] = "ldrsh x1, [x0, #10]\n";

  constexpr uint16_t expected = 0x9512;
  uint8_t v[] = {0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 (uint8_t)((0x9512 >> 0) & 0xff),
                 (uint8_t)((0x9512 >> 8) & 0xff),
                 0,
                 0,
                 0,
                 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[10], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[10], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRSHXui", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRSHXui", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRSHXui", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == 0xffffffffffff9512ULL);
                     CHECK(gprState->x0 == (QBDI::rword)&v[0]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldtrh") {
  const char source[] = "ldtrh w1, [x0, #10]\n";

  constexpr uint16_t expected = 0xe1f1;
  uint8_t v[] = {0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 (uint8_t)((0xe1f1 >> 0) & 0xff),
                 (uint8_t)((0xe1f1 >> 8) & 0xff),
                 0,
                 0,
                 0,
                 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[10], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[10], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDTRHi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDTRHi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDTRHi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffff) == expected);
                     CHECK(gprState->x0 == (QBDI::rword)&v[0]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldtrsh_w") {
  const char source[] = "ldtrsh w1, [x0, #10]\n";

  constexpr uint16_t expected = 0xe2f2;
  uint8_t v[] = {0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 (uint8_t)((0xe2f2 >> 0) & 0xff),
                 (uint8_t)((0xe2f2 >> 8) & 0xff),
                 0,
                 0,
                 0,
                 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[10], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[10], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDTRSHWi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDTRSHWi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDTRSHWi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffffffff) == 0xffffe2f2);
                     CHECK(gprState->x0 == (QBDI::rword)&v[0]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldtrsh_x") {
  const char source[] = "ldtrsh x1, [x0, #10]\n";

  constexpr uint16_t expected = 0xe3f3;
  uint8_t v[] = {0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 (uint8_t)((0xe3f3 >> 0) & 0xff),
                 (uint8_t)((0xe3f3 >> 8) & 0xff),
                 0,
                 0,
                 0,
                 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[10], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[10], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDTRSHXi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDTRSHXi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDTRSHXi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == 0xffffffffffffe3f3ULL);
                     CHECK(gprState->x0 == (QBDI::rword)&v[0]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-sttrh") {
  const char source[] = "sttrh w1, [x0, #10]\n";

  uint8_t v[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[10], 0xf1a1, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STTRHi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STTRHi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STTRHi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&v[0]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = 0xf1a1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint16_t *)&v[10] == 0xf1a1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldurhh") {
  const char source[] = "ldurh w1, [x0, #10]\n";

  constexpr uint16_t expected = 0x1121;
  uint8_t v[] = {0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 (uint8_t)((0x1121 >> 0) & 0xff),
                 (uint8_t)((0x1121 >> 8) & 0xff),
                 0,
                 0,
                 0,
                 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[10], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[10], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDURHHi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDURHHi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDURHHi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffff) == expected);
                     CHECK(gprState->x0 == (QBDI::rword)&v[0]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldurh_fpr") {
  const char source[] = "ldur h1, [x0, #10]\n";

  constexpr uint16_t expected = 0x1222;
  uint8_t v[] = {0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 (uint8_t)((0x1222 >> 0) & 0xff),
                 (uint8_t)((0x1222 >> 8) & 0xff),
                 0,
                 0,
                 0,
                 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[10], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[10], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDURHi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDURHi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDURHi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint16_t)fprState->v1 == expected);
                     CHECK(gprState->x0 == (QBDI::rword)&v[0]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldursh_w") {
  const char source[] = "ldursh w1, [x0, #10]\n";

  constexpr uint16_t expected = 0x9612;
  uint8_t v[] = {0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 (uint8_t)((0x9612 >> 0) & 0xff),
                 (uint8_t)((0x9612 >> 8) & 0xff),
                 0,
                 0,
                 0,
                 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[10], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[10], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDURSHWi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDURSHWi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDURSHWi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffffffff) == 0xffff9612);
                     CHECK(gprState->x0 == (QBDI::rword)&v[0]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldursh_x") {
  const char source[] = "ldursh x1, [x0, #10]\n";

  constexpr uint16_t expected = 0x9712;
  uint8_t v[] = {0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 0,
                 (uint8_t)((0x9712 >> 0) & 0xff),
                 (uint8_t)((0x9712 >> 8) & 0xff),
                 0,
                 0,
                 0,
                 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[10], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[10], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDURSHXi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDURSHXi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDURSHXi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == 0xffffffffffff9712ULL);
                     CHECK(gprState->x0 == (QBDI::rword)&v[0]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-sturhh") {
  const char source[] = "sturh w1, [x0, #10]\n";

  uint8_t v[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[10], 0x1521, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STURHHi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STURHHi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STURHHi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&v[0]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = 0x1521;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint16_t *)&v[10] == 0x1521);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-sturh_fpr") {
  const char source[] = "stur h1, [x0, #10]\n";

  uint8_t v[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[10], 0x1622, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STURHi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STURHi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STURHi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&v[0]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = 0x1622;
  vm.setFPRState(fpr);
  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint16_t *)&v[10] == 0x1622);
}
