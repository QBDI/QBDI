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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld1d8T") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld1.8 {d0, d1, d2}, [r0]\n";

  uint8_t buf[24] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                     0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                     0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xccbbaa99, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1ffeedd, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x5040302, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x9080706, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD1d8T", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD1d8T", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 16; i++) {
    CHECK(fpr->vreg.q[0][i] == buf[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][i] == buf[16 + i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld1d8Twb_fixed") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld1.8 {d0, d1, d2}, [r0]!\n";

  uint8_t buf[24] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                     0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                     0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xccbbaa99, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1ffeedd, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x5040302, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x9080706, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD1d8Twb_fixed", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD1d8Twb_fixed", QBDI::POSTINST, checkAccess,
                   &expectedPost);

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
  for (int i = 0; i < 16; i++) {
    CHECK(fpr->vreg.q[0][i] == buf[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][i] == buf[16 + i]);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[24]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld1d8Twb_register") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld1.8 {d0, d1, d2}, [r0], r2\n";

  uint8_t buf[32] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                     0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                     0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                     0,    0,    0,    0,    0,    0,    0,    0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xccbbaa99, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1ffeedd, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x5040302, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x9080706, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD1d8Twb_register", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VLD1d8Twb_register", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 28;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  QBDI::GPRState *finalState = vm.getGPRState();
  for (int i = 0; i < 16; i++) {
    CHECK(fpr->vreg.q[0][i] == buf[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][i] == buf[16 + i]);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[28]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld1d16T") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld1.16 {d0, d1, d2}, [r0]\n";

  uint8_t buf[24] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                     0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                     0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xccbbaa99, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1ffeedd, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x5040302, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x9080706, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD1d16T", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD1d16T", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 16; i++) {
    CHECK(fpr->vreg.q[0][i] == buf[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][i] == buf[16 + i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld1d16Twb_fixed") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld1.16 {d0, d1, d2}, [r0]!\n";

  uint8_t buf[24] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                     0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                     0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xccbbaa99, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1ffeedd, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x5040302, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x9080706, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD1d16Twb_fixed", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VLD1d16Twb_fixed", QBDI::POSTINST, checkAccess,
                   &expectedPost);

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
  for (int i = 0; i < 16; i++) {
    CHECK(fpr->vreg.q[0][i] == buf[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][i] == buf[16 + i]);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[24]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld1d16Twb_register") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld1.16 {d0, d1, d2}, [r0], r2\n";

  uint8_t buf[32] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                     0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                     0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                     0,    0,    0,    0,    0,    0,    0,    0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xccbbaa99, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1ffeedd, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x5040302, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x9080706, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD1d16Twb_register", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VLD1d16Twb_register", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 28;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  QBDI::GPRState *finalState = vm.getGPRState();
  for (int i = 0; i < 16; i++) {
    CHECK(fpr->vreg.q[0][i] == buf[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][i] == buf[16 + i]);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[28]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld1d32T") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld1.32 {d0, d1, d2}, [r0]\n";

  uint8_t buf[24] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                     0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                     0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xccbbaa99, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1ffeedd, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x5040302, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x9080706, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD1d32T", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD1d32T", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 16; i++) {
    CHECK(fpr->vreg.q[0][i] == buf[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][i] == buf[16 + i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld1d32Twb_fixed") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld1.32 {d0, d1, d2}, [r0]!\n";

  uint8_t buf[24] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                     0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                     0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xccbbaa99, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1ffeedd, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x5040302, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x9080706, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD1d32Twb_fixed", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VLD1d32Twb_fixed", QBDI::POSTINST, checkAccess,
                   &expectedPost);

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
  for (int i = 0; i < 16; i++) {
    CHECK(fpr->vreg.q[0][i] == buf[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][i] == buf[16 + i]);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[24]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld1d32Twb_register") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld1.32 {d0, d1, d2}, [r0], r2\n";

  uint8_t buf[32] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                     0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                     0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                     0,    0,    0,    0,    0,    0,    0,    0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xccbbaa99, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1ffeedd, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x5040302, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x9080706, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD1d32Twb_register", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VLD1d32Twb_register", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 28;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  QBDI::GPRState *finalState = vm.getGPRState();
  for (int i = 0; i < 16; i++) {
    CHECK(fpr->vreg.q[0][i] == buf[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][i] == buf[16 + i]);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[28]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld1d64T") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld1.64 {d0, d1, d2}, [r0]\n";

  uint8_t buf[24] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                     0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                     0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xccbbaa99, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1ffeedd, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x5040302, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x9080706, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD1d64T", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD1d64T", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 16; i++) {
    CHECK(fpr->vreg.q[0][i] == buf[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][i] == buf[16 + i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld1d64Twb_fixed") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld1.64 {d0, d1, d2}, [r0]!\n";

  uint8_t buf[24] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                     0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                     0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xccbbaa99, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1ffeedd, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x5040302, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x9080706, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD1d64Twb_fixed", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VLD1d64Twb_fixed", QBDI::POSTINST, checkAccess,
                   &expectedPost);

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
  for (int i = 0; i < 16; i++) {
    CHECK(fpr->vreg.q[0][i] == buf[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][i] == buf[16 + i]);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[24]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld1d64Twb_register") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld1.64 {d0, d1, d2}, [r0], r2\n";

  uint8_t buf[32] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                     0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                     0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                     0,    0,    0,    0,    0,    0,    0,    0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xccbbaa99, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1ffeedd, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x5040302, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x9080706, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD1d64Twb_register", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VLD1d64Twb_register", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 28;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  QBDI::GPRState *finalState = vm.getGPRState();
  for (int i = 0; i < 16; i++) {
    CHECK(fpr->vreg.q[0][i] == buf[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][i] == buf[16 + i]);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[28]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst1d8T") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst1.8 {d0, d1, d2}, [r0]\n";

  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xccbbaa99, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1ffeedd, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x5040302, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x9080706, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST1d8T", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST1d8T", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setBytes[24] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                          0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = setBytes[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[1][i] = setBytes[16 + i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  for (int i = 0; i < 24; i++) {
    CHECK(buf[i] == setBytes[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst1d8Twb_fixed") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst1.8 {d0, d1, d2}, [r0]!\n";

  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xccbbaa99, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1ffeedd, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x5040302, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x9080706, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST1d8Twb_fixed", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST1d8Twb_fixed", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setBytes[24] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                          0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = setBytes[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[1][i] = setBytes[16 + i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  for (int i = 0; i < 24; i++) {
    CHECK(buf[i] == setBytes[i]);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[24]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst1d8Twb_register") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst1.8 {d0, d1, d2}, [r0], r2\n";

  uint8_t buf[32] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xccbbaa99, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1ffeedd, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x5040302, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x9080706, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST1d8Twb_register", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VST1d8Twb_register", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 28;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setBytes[24] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                          0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = setBytes[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[1][i] = setBytes[16 + i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  for (int i = 0; i < 24; i++) {
    CHECK(buf[i] == setBytes[i]);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[28]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst1d16T") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst1.16 {d0, d1, d2}, [r0]\n";

  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xccbbaa99, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1ffeedd, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x5040302, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x9080706, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST1d16T", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST1d16T", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setBytes[24] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                          0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = setBytes[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[1][i] = setBytes[16 + i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  for (int i = 0; i < 24; i++) {
    CHECK(buf[i] == setBytes[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst1d16Twb_fixed") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst1.16 {d0, d1, d2}, [r0]!\n";

  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xccbbaa99, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1ffeedd, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x5040302, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x9080706, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST1d16Twb_fixed", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VST1d16Twb_fixed", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setBytes[24] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                          0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = setBytes[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[1][i] = setBytes[16 + i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  for (int i = 0; i < 24; i++) {
    CHECK(buf[i] == setBytes[i]);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[24]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst1d16Twb_register") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst1.16 {d0, d1, d2}, [r0], r2\n";

  uint8_t buf[32] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xccbbaa99, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1ffeedd, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x5040302, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x9080706, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST1d16Twb_register", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VST1d16Twb_register", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 28;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setBytes[24] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                          0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = setBytes[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[1][i] = setBytes[16 + i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  for (int i = 0; i < 24; i++) {
    CHECK(buf[i] == setBytes[i]);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[28]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst1d32T") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst1.32 {d0, d1, d2}, [r0]\n";

  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xccbbaa99, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1ffeedd, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x5040302, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x9080706, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST1d32T", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST1d32T", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setBytes[24] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                          0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = setBytes[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[1][i] = setBytes[16 + i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  for (int i = 0; i < 24; i++) {
    CHECK(buf[i] == setBytes[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst1d32Twb_fixed") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst1.32 {d0, d1, d2}, [r0]!\n";

  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xccbbaa99, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1ffeedd, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x5040302, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x9080706, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST1d32Twb_fixed", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VST1d32Twb_fixed", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setBytes[24] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                          0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = setBytes[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[1][i] = setBytes[16 + i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  for (int i = 0; i < 24; i++) {
    CHECK(buf[i] == setBytes[i]);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[24]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst1d32Twb_register") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst1.32 {d0, d1, d2}, [r0], r2\n";

  uint8_t buf[32] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xccbbaa99, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1ffeedd, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x5040302, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x9080706, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST1d32Twb_register", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VST1d32Twb_register", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 28;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setBytes[24] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                          0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = setBytes[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[1][i] = setBytes[16 + i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  for (int i = 0; i < 24; i++) {
    CHECK(buf[i] == setBytes[i]);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[28]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst1d64T") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst1.64 {d0, d1, d2}, [r0]\n";

  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xccbbaa99, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1ffeedd, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x5040302, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x9080706, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST1d64T", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST1d64T", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setBytes[24] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                          0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = setBytes[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[1][i] = setBytes[16 + i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  for (int i = 0; i < 24; i++) {
    CHECK(buf[i] == setBytes[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst1d64Twb_fixed") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst1.64 {d0, d1, d2}, [r0]!\n";

  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xccbbaa99, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1ffeedd, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x5040302, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x9080706, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST1d64Twb_fixed", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VST1d64Twb_fixed", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setBytes[24] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                          0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = setBytes[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[1][i] = setBytes[16 + i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  for (int i = 0; i < 24; i++) {
    CHECK(buf[i] == setBytes[i]);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[24]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst1d64Twb_register") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst1.64 {d0, d1, d2}, [r0], r2\n";

  uint8_t buf[32] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], 0xccbbaa99, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x1ffeedd, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[16], 0x5040302, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[20], 0x9080706, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST1d64Twb_register", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VST1d64Twb_register", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 28;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setBytes[24] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                          0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                          0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = setBytes[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[1][i] = setBytes[16 + i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  for (int i = 0; i < 24; i++) {
    CHECK(buf[i] == setBytes[i]);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[28]);
}
