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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrbb_post") {
  const char source[] = "ldrb w1, [x0], #5\n";

  constexpr uint8_t expected = 0xa1;
  uint8_t v[8] = {expected, 0, 0, 0, 0, 0, 0, 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRBBpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRBBpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRBBpost", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xff) == expected);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrbb_pre") {
  const char source[] = "ldrb w1, [x0, #5]!\n";

  constexpr uint8_t expected = 0xa2;
  uint8_t v[8] = {0, 0, 0, 0, 0, expected, 0, 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRBBpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRBBpre", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRBBpre", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xff) == expected);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrbb_ui") {
  const char source[] = "ldrb w1, [x0, #5]\n";

  constexpr uint8_t expected = 0xa3;
  uint8_t v[8] = {0, 0, 0, 0, 0, expected, 0, 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRBBui", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRBBui", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRBBui", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xff) == expected);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strbb_post") {
  const char source[] = "strb w1, [x0], #5\n";

  uint8_t v[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], 0xb1, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STRBBpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRBBpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STRBBpost", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&v[0] + 5);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = 0xb1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == 0xb1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strbb_pre") {
  const char source[] = "strb w1, [x0, #5]!\n";

  uint8_t v[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], 0xb2, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STRBBpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRBBpre", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STRBBpre", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&v[5]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = 0xb2;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[5] == 0xb2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strbb_ui") {
  const char source[] = "strb w1, [x0, #5]\n";

  uint8_t v[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], 0xb3, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STRBBui", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRBBui", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STRBBui", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&v[0]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = 0xb3;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[5] == 0xb3);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrb_fpr_post") {
  const char source[] = "ldr b1, [x0], #5\n";

  constexpr uint8_t expected = 0xc1;
  uint8_t v[8] = {expected, 0, 0, 0, 0, 0, 0, 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRBpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRBpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRBpost", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint8_t)fprState->v1 == expected);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrb_fpr_pre") {
  const char source[] = "ldr b1, [x0, #5]!\n";

  constexpr uint8_t expected = 0xc2;
  uint8_t v[8] = {0, 0, 0, 0, 0, expected, 0, 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRBpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRBpre", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRBpre", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint8_t)fprState->v1 == expected);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrb_fpr_ui") {
  const char source[] = "ldr b1, [x0, #5]\n";

  constexpr uint8_t expected = 0xc3;
  uint8_t v[8] = {0, 0, 0, 0, 0, expected, 0, 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRBui", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRBui", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRBui", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint8_t)fprState->v1 == expected);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strb_fpr_post") {
  const char source[] = "str b1, [x0], #5\n";

  uint8_t v[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], 0xd1, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STRBpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRBpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STRBpost", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&v[0] + 5);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = 0xd1;
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
  CHECK(v[0] == 0xd1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strb_fpr_pre") {
  const char source[] = "str b1, [x0, #5]!\n";

  uint8_t v[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], 0xd2, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STRBpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRBpre", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STRBpre", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&v[5]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = 0xd2;
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
  CHECK(v[5] == 0xd2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strb_fpr_ui") {
  const char source[] = "str b1, [x0, #5]\n";

  uint8_t v[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], 0xd3, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STRBui", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRBui", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STRBui", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&v[0]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = 0xd3;
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
  CHECK(v[5] == 0xd3);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrsbw_post") {
  const char source[] = "ldrsb w1, [x0], #5\n";

  constexpr uint8_t expected = 0x90;
  uint8_t v[8] = {expected, 0, 0, 0, 0, 0, 0, 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRSBWpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRSBWpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRSBWpost", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffffffff) == 0xffffff90);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrsbw_pre") {
  const char source[] = "ldrsb w1, [x0, #5]!\n";

  constexpr uint8_t expected = 0x91;
  uint8_t v[8] = {0, 0, 0, 0, 0, expected, 0, 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRSBWpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRSBWpre", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRSBWpre", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffffffff) == 0xffffff91);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrsbw_ui") {
  const char source[] = "ldrsb w1, [x0, #5]\n";

  constexpr uint8_t expected = 0x92;
  uint8_t v[8] = {0, 0, 0, 0, 0, expected, 0, 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRSBWui", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRSBWui", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRSBWui", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffffffff) == 0xffffff92);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrsbx_post") {
  const char source[] = "ldrsb x1, [x0], #5\n";

  constexpr uint8_t expected = 0x93;
  uint8_t v[8] = {expected, 0, 0, 0, 0, 0, 0, 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRSBXpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRSBXpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRSBXpost", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == 0xffffffffffffff93ULL);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrsbx_pre") {
  const char source[] = "ldrsb x1, [x0, #5]!\n";

  constexpr uint8_t expected = 0x94;
  uint8_t v[8] = {0, 0, 0, 0, 0, expected, 0, 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRSBXpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRSBXpre", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRSBXpre", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == 0xffffffffffffff94ULL);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrsbx_ui") {
  const char source[] = "ldrsb x1, [x0, #5]\n";

  constexpr uint8_t expected = 0x95;
  uint8_t v[8] = {0, 0, 0, 0, 0, expected, 0, 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRSBXui", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRSBXui", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRSBXui", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == 0xffffffffffffff95ULL);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldtrb") {
  const char source[] = "ldtrb w1, [x0, #5]\n";

  constexpr uint8_t expected = 0xe1;
  uint8_t v[8] = {0, 0, 0, 0, 0, expected, 0, 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDTRBi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDTRBi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDTRBi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xff) == expected);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldtrsb_w") {
  const char source[] = "ldtrsb w1, [x0, #5]\n";

  constexpr uint8_t expected = 0xe2;
  uint8_t v[8] = {0, 0, 0, 0, 0, expected, 0, 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDTRSBWi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDTRSBWi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDTRSBWi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffffffff) == 0xffffffe2);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldtrsb_x") {
  const char source[] = "ldtrsb x1, [x0, #5]\n";

  constexpr uint8_t expected = 0xe3;
  uint8_t v[8] = {0, 0, 0, 0, 0, expected, 0, 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDTRSBXi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDTRSBXi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDTRSBXi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == 0xffffffffffffffe3ULL);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-sttrb") {
  const char source[] = "sttrb w1, [x0, #5]\n";

  uint8_t v[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], 0xf1, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STTRBi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STTRBi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = 0xf1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v[5] == 0xf1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldurbb") {
  const char source[] = "ldurb w1, [x0, #5]\n";

  constexpr uint8_t expected = 0x11;
  uint8_t v[8] = {0, 0, 0, 0, 0, expected, 0, 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDURBBi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDURBBi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDURBBi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xff) == expected);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldurb_fpr") {
  const char source[] = "ldur b1, [x0, #5]\n";

  constexpr uint8_t expected = 0x12;
  uint8_t v[8] = {0, 0, 0, 0, 0, expected, 0, 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDURBi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDURBi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDURBi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint8_t)fprState->v1 == expected);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldursb_w") {
  const char source[] = "ldursb w1, [x0, #5]\n";

  constexpr uint8_t expected = 0x96;
  uint8_t v[8] = {0, 0, 0, 0, 0, expected, 0, 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDURSBWi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDURSBWi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDURSBWi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffffffff) == 0xffffff96);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldursb_x") {
  const char source[] = "ldursb x1, [x0, #5]\n";

  constexpr uint8_t expected = 0x97;
  uint8_t v[8] = {0, 0, 0, 0, 0, expected, 0, 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDURSBXi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDURSBXi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDURSBXi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == 0xffffffffffffff97ULL);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-sturbb") {
  const char source[] = "sturb w1, [x0, #5]\n";

  uint8_t v[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], 0x15, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STURBBi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STURBBi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = 0x15;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v[5] == 0x15);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-sturb_fpr") {
  const char source[] = "stur b1, [x0, #5]\n";

  uint8_t v[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], 0x16, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STURBi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STURBi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = 0x16;
  vm.setFPRState(fpr);
  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v[5] == 0x16);
}
