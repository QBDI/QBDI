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
#include <cstring>

#include "MemAccessTestUtils_ARM.h"

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::checkFeature;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld3d8") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld3.8 {d0, d1, d2}, [r0]\n";

  uint8_t buf[24] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                     0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x13121110, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x17161514, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1b1a1918, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1f1e1d1c, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x23222120, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x27262524, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD3d8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD3d8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  uint8_t d0[8] = {0x10, 0x13, 0x16, 0x19, 0x1c, 0x1f, 0x22, 0x25};
  uint8_t d1[8] = {0x11, 0x14, 0x17, 0x1a, 0x1d, 0x20, 0x23, 0x26};
  uint8_t d2[8] = {0x12, 0x15, 0x18, 0x1b, 0x1e, 0x21, 0x24, 0x27};
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][i] == d0[i]);
    CHECK(fpr->vreg.q[0][8 + i] == d1[i]);
    CHECK(fpr->vreg.q[1][i] == d2[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld3d8_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld3.8 {d0, d1, d2}, [r0]!\n";

  uint8_t buf[24] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                     0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x13121110, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x17161514, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1b1a1918, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1f1e1d1c, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x23222120, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x27262524, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD3d8_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD3d8_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  QBDI::GPRState *finalState = vm.getGPRState();
  uint8_t d0[8] = {0x10, 0x13, 0x16, 0x19, 0x1c, 0x1f, 0x22, 0x25};
  uint8_t d1[8] = {0x11, 0x14, 0x17, 0x1a, 0x1d, 0x20, 0x23, 0x26};
  uint8_t d2[8] = {0x12, 0x15, 0x18, 0x1b, 0x1e, 0x21, 0x24, 0x27};
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][i] == d0[i]);
    CHECK(fpr->vreg.q[0][8 + i] == d1[i]);
    CHECK(fpr->vreg.q[1][i] == d2[i]);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[24]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld3d16") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld3.16 {d0, d1, d2}, [r0]\n";

  uint8_t buf[24] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                     0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x13121110, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x17161514, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1b1a1918, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1f1e1d1c, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x23222120, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x27262524, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD3d16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD3d16", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  uint8_t d0[8] = {0x10, 0x11, 0x16, 0x17, 0x1c, 0x1d, 0x22, 0x23};
  uint8_t d1[8] = {0x12, 0x13, 0x18, 0x19, 0x1e, 0x1f, 0x24, 0x25};
  uint8_t d2[8] = {0x14, 0x15, 0x1a, 0x1b, 0x20, 0x21, 0x26, 0x27};
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][i] == d0[i]);
    CHECK(fpr->vreg.q[0][8 + i] == d1[i]);
    CHECK(fpr->vreg.q[1][i] == d2[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld3d16_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld3.16 {d0, d1, d2}, [r0]!\n";

  uint8_t buf[24] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                     0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x13121110, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x17161514, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1b1a1918, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1f1e1d1c, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x23222120, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x27262524, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD3d16_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD3d16_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  QBDI::GPRState *finalState = vm.getGPRState();
  uint8_t d0[8] = {0x10, 0x11, 0x16, 0x17, 0x1c, 0x1d, 0x22, 0x23};
  uint8_t d1[8] = {0x12, 0x13, 0x18, 0x19, 0x1e, 0x1f, 0x24, 0x25};
  uint8_t d2[8] = {0x14, 0x15, 0x1a, 0x1b, 0x20, 0x21, 0x26, 0x27};
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][i] == d0[i]);
    CHECK(fpr->vreg.q[0][8 + i] == d1[i]);
    CHECK(fpr->vreg.q[1][i] == d2[i]);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[24]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld3d32") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld3.32 {d0, d1, d2}, [r0]\n";

  uint8_t buf[24] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                     0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x13121110, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x17161514, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1b1a1918, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1f1e1d1c, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x23222120, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x27262524, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD3d32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD3d32", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  uint8_t d0[8] = {0x10, 0x11, 0x12, 0x13, 0x1c, 0x1d, 0x1e, 0x1f};
  uint8_t d1[8] = {0x14, 0x15, 0x16, 0x17, 0x20, 0x21, 0x22, 0x23};
  uint8_t d2[8] = {0x18, 0x19, 0x1a, 0x1b, 0x24, 0x25, 0x26, 0x27};
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][i] == d0[i]);
    CHECK(fpr->vreg.q[0][8 + i] == d1[i]);
    CHECK(fpr->vreg.q[1][i] == d2[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld3d32_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld3.32 {d0, d1, d2}, [r0]!\n";

  uint8_t buf[24] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                     0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x13121110, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x17161514, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1b1a1918, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1f1e1d1c, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x23222120, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x27262524, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD3d32_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD3d32_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  QBDI::GPRState *finalState = vm.getGPRState();
  uint8_t d0[8] = {0x10, 0x11, 0x12, 0x13, 0x1c, 0x1d, 0x1e, 0x1f};
  uint8_t d1[8] = {0x14, 0x15, 0x16, 0x17, 0x20, 0x21, 0x22, 0x23};
  uint8_t d2[8] = {0x18, 0x19, 0x1a, 0x1b, 0x24, 0x25, 0x26, 0x27};
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][i] == d0[i]);
    CHECK(fpr->vreg.q[0][8 + i] == d1[i]);
    CHECK(fpr->vreg.q[1][i] == d2[i]);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[24]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld3q8") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld3.8 {d0, d2, d4}, [r0]\n";

  uint8_t buf[24] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                     0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x13121110, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x17161514, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1b1a1918, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1f1e1d1c, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x23222120, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x27262524, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD3q8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD3q8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  uint8_t d0[8] = {0x10, 0x13, 0x16, 0x19, 0x1c, 0x1f, 0x22, 0x25};
  uint8_t d2[8] = {0x11, 0x14, 0x17, 0x1a, 0x1d, 0x20, 0x23, 0x26};
  uint8_t d4[8] = {0x12, 0x15, 0x18, 0x1b, 0x1e, 0x21, 0x24, 0x27};
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][i] == d0[i]);
    CHECK(fpr->vreg.q[1][i] == d2[i]);
    CHECK(fpr->vreg.q[2][i] == d4[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld3q8_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld3.8 {d0, d2, d4}, [r0]!\n";

  uint8_t buf[24] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                     0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x13121110, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x17161514, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1b1a1918, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1f1e1d1c, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x23222120, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x27262524, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD3q8_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD3q8_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  QBDI::GPRState *finalState = vm.getGPRState();
  uint8_t d0[8] = {0x10, 0x13, 0x16, 0x19, 0x1c, 0x1f, 0x22, 0x25};
  uint8_t d2[8] = {0x11, 0x14, 0x17, 0x1a, 0x1d, 0x20, 0x23, 0x26};
  uint8_t d4[8] = {0x12, 0x15, 0x18, 0x1b, 0x1e, 0x21, 0x24, 0x27};
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][i] == d0[i]);
    CHECK(fpr->vreg.q[1][i] == d2[i]);
    CHECK(fpr->vreg.q[2][i] == d4[i]);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[24]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld3q16") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld3.16 {d0, d2, d4}, [r0]\n";

  uint8_t buf[24] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                     0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x13121110, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x17161514, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1b1a1918, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1f1e1d1c, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x23222120, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x27262524, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD3q16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD3q16", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  uint8_t d0[8] = {0x10, 0x11, 0x16, 0x17, 0x1c, 0x1d, 0x22, 0x23};
  uint8_t d2[8] = {0x12, 0x13, 0x18, 0x19, 0x1e, 0x1f, 0x24, 0x25};
  uint8_t d4[8] = {0x14, 0x15, 0x1a, 0x1b, 0x20, 0x21, 0x26, 0x27};
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][i] == d0[i]);
    CHECK(fpr->vreg.q[1][i] == d2[i]);
    CHECK(fpr->vreg.q[2][i] == d4[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld3q16_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld3.16 {d0, d2, d4}, [r0]!\n";

  uint8_t buf[24] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                     0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x13121110, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x17161514, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1b1a1918, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1f1e1d1c, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x23222120, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x27262524, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD3q16_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD3q16_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  QBDI::GPRState *finalState = vm.getGPRState();
  uint8_t d0[8] = {0x10, 0x11, 0x16, 0x17, 0x1c, 0x1d, 0x22, 0x23};
  uint8_t d2[8] = {0x12, 0x13, 0x18, 0x19, 0x1e, 0x1f, 0x24, 0x25};
  uint8_t d4[8] = {0x14, 0x15, 0x1a, 0x1b, 0x20, 0x21, 0x26, 0x27};
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][i] == d0[i]);
    CHECK(fpr->vreg.q[1][i] == d2[i]);
    CHECK(fpr->vreg.q[2][i] == d4[i]);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[24]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld3q32") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld3.32 {d0, d2, d4}, [r0]\n";

  uint8_t buf[24] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                     0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x13121110, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x17161514, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1b1a1918, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1f1e1d1c, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x23222120, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x27262524, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD3q32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD3q32", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  uint8_t d0[8] = {0x10, 0x11, 0x12, 0x13, 0x1c, 0x1d, 0x1e, 0x1f};
  uint8_t d2[8] = {0x14, 0x15, 0x16, 0x17, 0x20, 0x21, 0x22, 0x23};
  uint8_t d4[8] = {0x18, 0x19, 0x1a, 0x1b, 0x24, 0x25, 0x26, 0x27};
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][i] == d0[i]);
    CHECK(fpr->vreg.q[1][i] == d2[i]);
    CHECK(fpr->vreg.q[2][i] == d4[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld3q32_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld3.32 {d0, d2, d4}, [r0]!\n";

  uint8_t buf[24] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                     0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x13121110, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x17161514, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1b1a1918, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1f1e1d1c, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x23222120, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x27262524, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD3q32_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD3q32_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  QBDI::GPRState *finalState = vm.getGPRState();
  uint8_t d0[8] = {0x10, 0x11, 0x12, 0x13, 0x1c, 0x1d, 0x1e, 0x1f};
  uint8_t d2[8] = {0x14, 0x15, 0x16, 0x17, 0x20, 0x21, 0x22, 0x23};
  uint8_t d4[8] = {0x18, 0x19, 0x1a, 0x1b, 0x24, 0x25, 0x26, 0x27};
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][i] == d0[i]);
    CHECK(fpr->vreg.q[1][i] == d2[i]);
    CHECK(fpr->vreg.q[2][i] == d4[i]);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[24]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst3d8") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst3.8 {d0, d1, d2}, [r0]\n";

  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x13121110, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x17161514, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1b1a1918, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1f1e1d1c, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x23222120, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x27262524, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST3d8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST3d8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t d0[8] = {0x10, 0x13, 0x16, 0x19, 0x1c, 0x1f, 0x22, 0x25};
  uint8_t d1[8] = {0x11, 0x14, 0x17, 0x1a, 0x1d, 0x20, 0x23, 0x26};
  uint8_t d2[8] = {0x12, 0x15, 0x18, 0x1b, 0x1e, 0x21, 0x24, 0x27};
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][i] = d0[i];
    fpr->vreg.q[0][8 + i] = d1[i];
    fpr->vreg.q[1][i] = d2[i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  uint8_t expBytes[24] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                          0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                          0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
  for (int i = 0; i < 24; i++) {
    CHECK(buf[i] == expBytes[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst3d8_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst3.8 {d0, d1, d2}, [r0]!\n";

  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x13121110, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x17161514, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1b1a1918, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1f1e1d1c, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x23222120, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x27262524, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST3d8_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST3d8_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t d0[8] = {0x10, 0x13, 0x16, 0x19, 0x1c, 0x1f, 0x22, 0x25};
  uint8_t d1[8] = {0x11, 0x14, 0x17, 0x1a, 0x1d, 0x20, 0x23, 0x26};
  uint8_t d2[8] = {0x12, 0x15, 0x18, 0x1b, 0x1e, 0x21, 0x24, 0x27};
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][i] = d0[i];
    fpr->vreg.q[0][8 + i] = d1[i];
    fpr->vreg.q[1][i] = d2[i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  uint8_t expBytes[24] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                          0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                          0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
  for (int i = 0; i < 24; i++) {
    CHECK(buf[i] == expBytes[i]);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[24]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst3d16") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst3.16 {d0, d1, d2}, [r0]\n";

  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x13121110, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x17161514, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1b1a1918, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1f1e1d1c, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x23222120, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x27262524, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST3d16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST3d16", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t d0[8] = {0x10, 0x11, 0x16, 0x17, 0x1c, 0x1d, 0x22, 0x23};
  uint8_t d1[8] = {0x12, 0x13, 0x18, 0x19, 0x1e, 0x1f, 0x24, 0x25};
  uint8_t d2[8] = {0x14, 0x15, 0x1a, 0x1b, 0x20, 0x21, 0x26, 0x27};
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][i] = d0[i];
    fpr->vreg.q[0][8 + i] = d1[i];
    fpr->vreg.q[1][i] = d2[i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  uint8_t expBytes[24] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                          0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                          0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
  for (int i = 0; i < 24; i++) {
    CHECK(buf[i] == expBytes[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst3d16_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst3.16 {d0, d1, d2}, [r0]!\n";

  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x13121110, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x17161514, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1b1a1918, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1f1e1d1c, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x23222120, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x27262524, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST3d16_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST3d16_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t d0[8] = {0x10, 0x11, 0x16, 0x17, 0x1c, 0x1d, 0x22, 0x23};
  uint8_t d1[8] = {0x12, 0x13, 0x18, 0x19, 0x1e, 0x1f, 0x24, 0x25};
  uint8_t d2[8] = {0x14, 0x15, 0x1a, 0x1b, 0x20, 0x21, 0x26, 0x27};
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][i] = d0[i];
    fpr->vreg.q[0][8 + i] = d1[i];
    fpr->vreg.q[1][i] = d2[i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  uint8_t expBytes[24] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                          0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                          0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
  for (int i = 0; i < 24; i++) {
    CHECK(buf[i] == expBytes[i]);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[24]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst3d32") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst3.32 {d0, d1, d2}, [r0]\n";

  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x13121110, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x17161514, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1b1a1918, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1f1e1d1c, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x23222120, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x27262524, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST3d32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST3d32", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t d0[8] = {0x10, 0x11, 0x12, 0x13, 0x1c, 0x1d, 0x1e, 0x1f};
  uint8_t d1[8] = {0x14, 0x15, 0x16, 0x17, 0x20, 0x21, 0x22, 0x23};
  uint8_t d2[8] = {0x18, 0x19, 0x1a, 0x1b, 0x24, 0x25, 0x26, 0x27};
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][i] = d0[i];
    fpr->vreg.q[0][8 + i] = d1[i];
    fpr->vreg.q[1][i] = d2[i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  uint8_t expBytes[24] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                          0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                          0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
  for (int i = 0; i < 24; i++) {
    CHECK(buf[i] == expBytes[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst3d32_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst3.32 {d0, d1, d2}, [r0]!\n";

  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x13121110, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x17161514, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1b1a1918, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1f1e1d1c, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x23222120, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x27262524, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST3d32_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST3d32_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t d0[8] = {0x10, 0x11, 0x12, 0x13, 0x1c, 0x1d, 0x1e, 0x1f};
  uint8_t d1[8] = {0x14, 0x15, 0x16, 0x17, 0x20, 0x21, 0x22, 0x23};
  uint8_t d2[8] = {0x18, 0x19, 0x1a, 0x1b, 0x24, 0x25, 0x26, 0x27};
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][i] = d0[i];
    fpr->vreg.q[0][8 + i] = d1[i];
    fpr->vreg.q[1][i] = d2[i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  uint8_t expBytes[24] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                          0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                          0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
  for (int i = 0; i < 24; i++) {
    CHECK(buf[i] == expBytes[i]);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[24]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst3q8") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst3.8 {d0, d2, d4}, [r0]\n";

  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x13121110, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x17161514, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1b1a1918, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1f1e1d1c, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x23222120, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x27262524, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST3q8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST3q8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t d0[8] = {0x10, 0x13, 0x16, 0x19, 0x1c, 0x1f, 0x22, 0x25};
  uint8_t d2[8] = {0x11, 0x14, 0x17, 0x1a, 0x1d, 0x20, 0x23, 0x26};
  uint8_t d4[8] = {0x12, 0x15, 0x18, 0x1b, 0x1e, 0x21, 0x24, 0x27};
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][i] = d0[i];
    fpr->vreg.q[1][i] = d2[i];
    fpr->vreg.q[2][i] = d4[i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  uint8_t expBytes[24] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                          0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                          0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
  for (int i = 0; i < 24; i++) {
    CHECK(buf[i] == expBytes[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst3q8_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst3.8 {d0, d2, d4}, [r0]!\n";

  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x13121110, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x17161514, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1b1a1918, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1f1e1d1c, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x23222120, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x27262524, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST3q8_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST3q8_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t d0[8] = {0x10, 0x13, 0x16, 0x19, 0x1c, 0x1f, 0x22, 0x25};
  uint8_t d2[8] = {0x11, 0x14, 0x17, 0x1a, 0x1d, 0x20, 0x23, 0x26};
  uint8_t d4[8] = {0x12, 0x15, 0x18, 0x1b, 0x1e, 0x21, 0x24, 0x27};
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][i] = d0[i];
    fpr->vreg.q[1][i] = d2[i];
    fpr->vreg.q[2][i] = d4[i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  uint8_t expBytes[24] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                          0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                          0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
  for (int i = 0; i < 24; i++) {
    CHECK(buf[i] == expBytes[i]);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[24]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst3q16") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst3.16 {d0, d2, d4}, [r0]\n";

  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x13121110, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x17161514, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1b1a1918, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1f1e1d1c, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x23222120, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x27262524, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST3q16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST3q16", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t d0[8] = {0x10, 0x11, 0x16, 0x17, 0x1c, 0x1d, 0x22, 0x23};
  uint8_t d2[8] = {0x12, 0x13, 0x18, 0x19, 0x1e, 0x1f, 0x24, 0x25};
  uint8_t d4[8] = {0x14, 0x15, 0x1a, 0x1b, 0x20, 0x21, 0x26, 0x27};
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][i] = d0[i];
    fpr->vreg.q[1][i] = d2[i];
    fpr->vreg.q[2][i] = d4[i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  uint8_t expBytes[24] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                          0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                          0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
  for (int i = 0; i < 24; i++) {
    CHECK(buf[i] == expBytes[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst3q16_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst3.16 {d0, d2, d4}, [r0]!\n";

  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x13121110, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x17161514, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1b1a1918, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1f1e1d1c, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x23222120, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x27262524, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST3q16_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST3q16_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t d0[8] = {0x10, 0x11, 0x16, 0x17, 0x1c, 0x1d, 0x22, 0x23};
  uint8_t d2[8] = {0x12, 0x13, 0x18, 0x19, 0x1e, 0x1f, 0x24, 0x25};
  uint8_t d4[8] = {0x14, 0x15, 0x1a, 0x1b, 0x20, 0x21, 0x26, 0x27};
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][i] = d0[i];
    fpr->vreg.q[1][i] = d2[i];
    fpr->vreg.q[2][i] = d4[i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  uint8_t expBytes[24] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                          0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                          0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
  for (int i = 0; i < 24; i++) {
    CHECK(buf[i] == expBytes[i]);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[24]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst3q32") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst3.32 {d0, d2, d4}, [r0]\n";

  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x13121110, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x17161514, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1b1a1918, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1f1e1d1c, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x23222120, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x27262524, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST3q32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST3q32", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t d0[8] = {0x10, 0x11, 0x12, 0x13, 0x1c, 0x1d, 0x1e, 0x1f};
  uint8_t d2[8] = {0x14, 0x15, 0x16, 0x17, 0x20, 0x21, 0x22, 0x23};
  uint8_t d4[8] = {0x18, 0x19, 0x1a, 0x1b, 0x24, 0x25, 0x26, 0x27};
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][i] = d0[i];
    fpr->vreg.q[1][i] = d2[i];
    fpr->vreg.q[2][i] = d4[i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  uint8_t expBytes[24] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                          0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                          0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
  for (int i = 0; i < 24; i++) {
    CHECK(buf[i] == expBytes[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst3q32_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst3.32 {d0, d2, d4}, [r0]!\n";

  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x13121110, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x17161514, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0x1b1a1918, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1f1e1d1c, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x23222120, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x27262524, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST3q32_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST3q32_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t d0[8] = {0x10, 0x11, 0x12, 0x13, 0x1c, 0x1d, 0x1e, 0x1f};
  uint8_t d2[8] = {0x14, 0x15, 0x16, 0x17, 0x20, 0x21, 0x22, 0x23};
  uint8_t d4[8] = {0x18, 0x19, 0x1a, 0x1b, 0x24, 0x25, 0x26, 0x27};
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][i] = d0[i];
    fpr->vreg.q[1][i] = d2[i];
    fpr->vreg.q[2][i] = d4[i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  uint8_t expBytes[24] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                          0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                          0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
  for (int i = 0; i < 24; i++) {
    CHECK(buf[i] == expBytes[i]);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[24]);
}
