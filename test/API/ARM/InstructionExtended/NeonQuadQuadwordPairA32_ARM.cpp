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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4d8") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.8 {d0, d1, d2, d3}, [r0]\n";

  uint8_t buf[32] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                     0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                     0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                     0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x21};
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
      {(QBDI::rword)&buf[24], 0xd0c0b0a, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[28], 0x21100f0e, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4d8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4d8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  uint8_t reg0[8] = {0x11, 0x55, 0x99, 0xdd, 0x02, 0x06, 0x0a, 0x0e};
  uint8_t reg1[8] = {0x22, 0x66, 0xaa, 0xee, 0x03, 0x07, 0x0b, 0x0f};
  uint8_t reg2[8] = {0x33, 0x77, 0xbb, 0xff, 0x04, 0x08, 0x0c, 0x10};
  uint8_t reg3[8] = {0x44, 0x88, 0xcc, 0x01, 0x05, 0x09, 0x0d, 0x21};
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][0 + i] == reg0[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][8 + i] == reg1[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][0 + i] == reg2[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][8 + i] == reg3[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4d8_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.8 {d0, d1, d2, d3}, [r0]!\n";

  uint8_t buf[32] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                     0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                     0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                     0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x21};
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
      {(QBDI::rword)&buf[24], 0xd0c0b0a, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[28], 0x21100f0e, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4d8_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4d8_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  uint8_t reg0[8] = {0x11, 0x55, 0x99, 0xdd, 0x02, 0x06, 0x0a, 0x0e};
  uint8_t reg1[8] = {0x22, 0x66, 0xaa, 0xee, 0x03, 0x07, 0x0b, 0x0f};
  uint8_t reg2[8] = {0x33, 0x77, 0xbb, 0xff, 0x04, 0x08, 0x0c, 0x10};
  uint8_t reg3[8] = {0x44, 0x88, 0xcc, 0x01, 0x05, 0x09, 0x0d, 0x21};
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][0 + i] == reg0[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][8 + i] == reg1[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][0 + i] == reg2[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][8 + i] == reg3[i]);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[32]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4q8") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.8 {d0, d2, d4, d6}, [r0]\n";

  uint8_t buf[32] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                     0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                     0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                     0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x21};
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
      {(QBDI::rword)&buf[24], 0xd0c0b0a, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[28], 0x21100f0e, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4q8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4q8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  uint8_t reg0[8] = {0x11, 0x55, 0x99, 0xdd, 0x02, 0x06, 0x0a, 0x0e};
  uint8_t reg1[8] = {0x22, 0x66, 0xaa, 0xee, 0x03, 0x07, 0x0b, 0x0f};
  uint8_t reg2[8] = {0x33, 0x77, 0xbb, 0xff, 0x04, 0x08, 0x0c, 0x10};
  uint8_t reg3[8] = {0x44, 0x88, 0xcc, 0x01, 0x05, 0x09, 0x0d, 0x21};
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][0 + i] == reg0[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][0 + i] == reg1[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[2][0 + i] == reg2[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[3][0 + i] == reg3[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4q8_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.8 {d0, d2, d4, d6}, [r0]!\n";

  uint8_t buf[32] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                     0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                     0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                     0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x21};
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
      {(QBDI::rword)&buf[24], 0xd0c0b0a, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[28], 0x21100f0e, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4q8_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4q8_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  uint8_t reg0[8] = {0x11, 0x55, 0x99, 0xdd, 0x02, 0x06, 0x0a, 0x0e};
  uint8_t reg1[8] = {0x22, 0x66, 0xaa, 0xee, 0x03, 0x07, 0x0b, 0x0f};
  uint8_t reg2[8] = {0x33, 0x77, 0xbb, 0xff, 0x04, 0x08, 0x0c, 0x10};
  uint8_t reg3[8] = {0x44, 0x88, 0xcc, 0x01, 0x05, 0x09, 0x0d, 0x21};
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][0 + i] == reg0[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][0 + i] == reg1[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[2][0 + i] == reg2[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[3][0 + i] == reg3[i]);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[32]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4d16") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.16 {d0, d1, d2, d3}, [r0]\n";

  uint8_t buf[32] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                     0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                     0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                     0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x21};
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
      {(QBDI::rword)&buf[24], 0xd0c0b0a, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[28], 0x21100f0e, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4d16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4d16", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  uint8_t reg0[8] = {0x11, 0x22, 0x99, 0xaa, 0x02, 0x03, 0x0a, 0x0b};
  uint8_t reg1[8] = {0x33, 0x44, 0xbb, 0xcc, 0x04, 0x05, 0x0c, 0x0d};
  uint8_t reg2[8] = {0x55, 0x66, 0xdd, 0xee, 0x06, 0x07, 0x0e, 0x0f};
  uint8_t reg3[8] = {0x77, 0x88, 0xff, 0x01, 0x08, 0x09, 0x10, 0x21};
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][0 + i] == reg0[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][8 + i] == reg1[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][0 + i] == reg2[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][8 + i] == reg3[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4d16_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.16 {d0, d1, d2, d3}, [r0]!\n";

  uint8_t buf[32] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                     0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                     0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                     0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x21};
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
      {(QBDI::rword)&buf[24], 0xd0c0b0a, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[28], 0x21100f0e, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4d16_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4d16_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  uint8_t reg0[8] = {0x11, 0x22, 0x99, 0xaa, 0x02, 0x03, 0x0a, 0x0b};
  uint8_t reg1[8] = {0x33, 0x44, 0xbb, 0xcc, 0x04, 0x05, 0x0c, 0x0d};
  uint8_t reg2[8] = {0x55, 0x66, 0xdd, 0xee, 0x06, 0x07, 0x0e, 0x0f};
  uint8_t reg3[8] = {0x77, 0x88, 0xff, 0x01, 0x08, 0x09, 0x10, 0x21};
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][0 + i] == reg0[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][8 + i] == reg1[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][0 + i] == reg2[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][8 + i] == reg3[i]);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[32]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4q16") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.16 {d0, d2, d4, d6}, [r0]\n";

  uint8_t buf[32] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                     0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                     0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                     0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x21};
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
      {(QBDI::rword)&buf[24], 0xd0c0b0a, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[28], 0x21100f0e, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4q16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4q16", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  uint8_t reg0[8] = {0x11, 0x22, 0x99, 0xaa, 0x02, 0x03, 0x0a, 0x0b};
  uint8_t reg1[8] = {0x33, 0x44, 0xbb, 0xcc, 0x04, 0x05, 0x0c, 0x0d};
  uint8_t reg2[8] = {0x55, 0x66, 0xdd, 0xee, 0x06, 0x07, 0x0e, 0x0f};
  uint8_t reg3[8] = {0x77, 0x88, 0xff, 0x01, 0x08, 0x09, 0x10, 0x21};
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][0 + i] == reg0[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][0 + i] == reg1[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[2][0 + i] == reg2[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[3][0 + i] == reg3[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4q16_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.16 {d0, d2, d4, d6}, [r0]!\n";

  uint8_t buf[32] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                     0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                     0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                     0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x21};
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
      {(QBDI::rword)&buf[24], 0xd0c0b0a, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[28], 0x21100f0e, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4q16_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4q16_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  uint8_t reg0[8] = {0x11, 0x22, 0x99, 0xaa, 0x02, 0x03, 0x0a, 0x0b};
  uint8_t reg1[8] = {0x33, 0x44, 0xbb, 0xcc, 0x04, 0x05, 0x0c, 0x0d};
  uint8_t reg2[8] = {0x55, 0x66, 0xdd, 0xee, 0x06, 0x07, 0x0e, 0x0f};
  uint8_t reg3[8] = {0x77, 0x88, 0xff, 0x01, 0x08, 0x09, 0x10, 0x21};
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][0 + i] == reg0[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][0 + i] == reg1[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[2][0 + i] == reg2[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[3][0 + i] == reg3[i]);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[32]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4d32") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.32 {d0, d1, d2, d3}, [r0]\n";

  uint8_t buf[32] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                     0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                     0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                     0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x21};
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
      {(QBDI::rword)&buf[24], 0xd0c0b0a, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[28], 0x21100f0e, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4d32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4d32", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  uint8_t reg0[8] = {0x11, 0x22, 0x33, 0x44, 0x02, 0x03, 0x04, 0x05};
  uint8_t reg1[8] = {0x55, 0x66, 0x77, 0x88, 0x06, 0x07, 0x08, 0x09};
  uint8_t reg2[8] = {0x99, 0xaa, 0xbb, 0xcc, 0x0a, 0x0b, 0x0c, 0x0d};
  uint8_t reg3[8] = {0xdd, 0xee, 0xff, 0x01, 0x0e, 0x0f, 0x10, 0x21};
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][0 + i] == reg0[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][8 + i] == reg1[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][0 + i] == reg2[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][8 + i] == reg3[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4d32_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.32 {d0, d1, d2, d3}, [r0]!\n";

  uint8_t buf[32] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                     0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                     0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                     0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x21};
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
      {(QBDI::rword)&buf[24], 0xd0c0b0a, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[28], 0x21100f0e, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4d32_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4d32_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  uint8_t reg0[8] = {0x11, 0x22, 0x33, 0x44, 0x02, 0x03, 0x04, 0x05};
  uint8_t reg1[8] = {0x55, 0x66, 0x77, 0x88, 0x06, 0x07, 0x08, 0x09};
  uint8_t reg2[8] = {0x99, 0xaa, 0xbb, 0xcc, 0x0a, 0x0b, 0x0c, 0x0d};
  uint8_t reg3[8] = {0xdd, 0xee, 0xff, 0x01, 0x0e, 0x0f, 0x10, 0x21};
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][0 + i] == reg0[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][8 + i] == reg1[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][0 + i] == reg2[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][8 + i] == reg3[i]);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[32]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4q32") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.32 {d0, d2, d4, d6}, [r0]\n";

  uint8_t buf[32] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                     0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                     0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                     0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x21};
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
      {(QBDI::rword)&buf[24], 0xd0c0b0a, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[28], 0x21100f0e, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4q32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4q32", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  uint8_t reg0[8] = {0x11, 0x22, 0x33, 0x44, 0x02, 0x03, 0x04, 0x05};
  uint8_t reg1[8] = {0x55, 0x66, 0x77, 0x88, 0x06, 0x07, 0x08, 0x09};
  uint8_t reg2[8] = {0x99, 0xaa, 0xbb, 0xcc, 0x0a, 0x0b, 0x0c, 0x0d};
  uint8_t reg3[8] = {0xdd, 0xee, 0xff, 0x01, 0x0e, 0x0f, 0x10, 0x21};
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][0 + i] == reg0[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][0 + i] == reg1[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[2][0 + i] == reg2[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[3][0 + i] == reg3[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4q32_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.32 {d0, d2, d4, d6}, [r0]!\n";

  uint8_t buf[32] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                     0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                     0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                     0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x21};
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
      {(QBDI::rword)&buf[24], 0xd0c0b0a, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[28], 0x21100f0e, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4q32_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4q32_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  uint8_t reg0[8] = {0x11, 0x22, 0x33, 0x44, 0x02, 0x03, 0x04, 0x05};
  uint8_t reg1[8] = {0x55, 0x66, 0x77, 0x88, 0x06, 0x07, 0x08, 0x09};
  uint8_t reg2[8] = {0x99, 0xaa, 0xbb, 0xcc, 0x0a, 0x0b, 0x0c, 0x0d};
  uint8_t reg3[8] = {0xdd, 0xee, 0xff, 0x01, 0x0e, 0x0f, 0x10, 0x21};
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][0 + i] == reg0[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][0 + i] == reg1[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[2][0 + i] == reg2[i]);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[3][0 + i] == reg3[i]);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[32]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst4d8") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst4.8 {d0, d1, d2, d3}, [r0]\n";

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
      {(QBDI::rword)&buf[24], 0xd0c0b0a, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[28], 0x21100f0e, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST4d8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST4d8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setReg0[8] = {0x11, 0x55, 0x99, 0xdd, 0x02, 0x06, 0x0a, 0x0e};
  uint8_t setReg1[8] = {0x22, 0x66, 0xaa, 0xee, 0x03, 0x07, 0x0b, 0x0f};
  uint8_t setReg2[8] = {0x33, 0x77, 0xbb, 0xff, 0x04, 0x08, 0x0c, 0x10};
  uint8_t setReg3[8] = {0x44, 0x88, 0xcc, 0x01, 0x05, 0x09, 0x0d, 0x21};
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][0 + i] = setReg0[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][8 + i] = setReg1[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[1][0 + i] = setReg2[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[1][8 + i] = setReg3[i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  uint8_t expBuf[32] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                        0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                        0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                        0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x21};
  for (int i = 0; i < 32; i++) {
    CHECK(buf[i] == expBuf[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst4d8_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst4.8 {d0, d1, d2, d3}, [r0]!\n";

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
      {(QBDI::rword)&buf[24], 0xd0c0b0a, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[28], 0x21100f0e, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST4d8_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST4d8_UPD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("VST4d8_UPD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->r0 == (QBDI::rword)&buf[32]);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setReg0[8] = {0x11, 0x55, 0x99, 0xdd, 0x02, 0x06, 0x0a, 0x0e};
  uint8_t setReg1[8] = {0x22, 0x66, 0xaa, 0xee, 0x03, 0x07, 0x0b, 0x0f};
  uint8_t setReg2[8] = {0x33, 0x77, 0xbb, 0xff, 0x04, 0x08, 0x0c, 0x10};
  uint8_t setReg3[8] = {0x44, 0x88, 0xcc, 0x01, 0x05, 0x09, 0x0d, 0x21};
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][0 + i] = setReg0[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][8 + i] = setReg1[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[1][0 + i] = setReg2[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[1][8 + i] = setReg3[i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  uint8_t expBuf[32] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                        0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                        0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                        0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x21};
  for (int i = 0; i < 32; i++) {
    CHECK(buf[i] == expBuf[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst4q8") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst4.8 {d0, d2, d4, d6}, [r0]\n";

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
      {(QBDI::rword)&buf[24], 0xd0c0b0a, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[28], 0x21100f0e, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST4q8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST4q8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setReg0[8] = {0x11, 0x55, 0x99, 0xdd, 0x02, 0x06, 0x0a, 0x0e};
  uint8_t setReg1[8] = {0x22, 0x66, 0xaa, 0xee, 0x03, 0x07, 0x0b, 0x0f};
  uint8_t setReg2[8] = {0x33, 0x77, 0xbb, 0xff, 0x04, 0x08, 0x0c, 0x10};
  uint8_t setReg3[8] = {0x44, 0x88, 0xcc, 0x01, 0x05, 0x09, 0x0d, 0x21};
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][0 + i] = setReg0[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[1][0 + i] = setReg1[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[2][0 + i] = setReg2[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[3][0 + i] = setReg3[i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  uint8_t expBuf[32] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                        0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                        0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                        0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x21};
  for (int i = 0; i < 32; i++) {
    CHECK(buf[i] == expBuf[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst4q8_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst4.8 {d0, d2, d4, d6}, [r0]!\n";

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
      {(QBDI::rword)&buf[24], 0xd0c0b0a, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[28], 0x21100f0e, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST4q8_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST4q8_UPD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("VST4q8_UPD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->r0 == (QBDI::rword)&buf[32]);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setReg0[8] = {0x11, 0x55, 0x99, 0xdd, 0x02, 0x06, 0x0a, 0x0e};
  uint8_t setReg1[8] = {0x22, 0x66, 0xaa, 0xee, 0x03, 0x07, 0x0b, 0x0f};
  uint8_t setReg2[8] = {0x33, 0x77, 0xbb, 0xff, 0x04, 0x08, 0x0c, 0x10};
  uint8_t setReg3[8] = {0x44, 0x88, 0xcc, 0x01, 0x05, 0x09, 0x0d, 0x21};
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][0 + i] = setReg0[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[1][0 + i] = setReg1[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[2][0 + i] = setReg2[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[3][0 + i] = setReg3[i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  uint8_t expBuf[32] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                        0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                        0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                        0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x21};
  for (int i = 0; i < 32; i++) {
    CHECK(buf[i] == expBuf[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst4d16") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst4.16 {d0, d1, d2, d3}, [r0]\n";

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
      {(QBDI::rword)&buf[24], 0xd0c0b0a, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[28], 0x21100f0e, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST4d16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST4d16", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setReg0[8] = {0x11, 0x22, 0x99, 0xaa, 0x02, 0x03, 0x0a, 0x0b};
  uint8_t setReg1[8] = {0x33, 0x44, 0xbb, 0xcc, 0x04, 0x05, 0x0c, 0x0d};
  uint8_t setReg2[8] = {0x55, 0x66, 0xdd, 0xee, 0x06, 0x07, 0x0e, 0x0f};
  uint8_t setReg3[8] = {0x77, 0x88, 0xff, 0x01, 0x08, 0x09, 0x10, 0x21};
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][0 + i] = setReg0[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][8 + i] = setReg1[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[1][0 + i] = setReg2[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[1][8 + i] = setReg3[i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  uint8_t expBuf[32] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                        0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                        0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                        0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x21};
  for (int i = 0; i < 32; i++) {
    CHECK(buf[i] == expBuf[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst4d16_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst4.16 {d0, d1, d2, d3}, [r0]!\n";

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
      {(QBDI::rword)&buf[24], 0xd0c0b0a, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[28], 0x21100f0e, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST4d16_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST4d16_UPD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("VST4d16_UPD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->r0 == (QBDI::rword)&buf[32]);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setReg0[8] = {0x11, 0x22, 0x99, 0xaa, 0x02, 0x03, 0x0a, 0x0b};
  uint8_t setReg1[8] = {0x33, 0x44, 0xbb, 0xcc, 0x04, 0x05, 0x0c, 0x0d};
  uint8_t setReg2[8] = {0x55, 0x66, 0xdd, 0xee, 0x06, 0x07, 0x0e, 0x0f};
  uint8_t setReg3[8] = {0x77, 0x88, 0xff, 0x01, 0x08, 0x09, 0x10, 0x21};
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][0 + i] = setReg0[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][8 + i] = setReg1[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[1][0 + i] = setReg2[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[1][8 + i] = setReg3[i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  uint8_t expBuf[32] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                        0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                        0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                        0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x21};
  for (int i = 0; i < 32; i++) {
    CHECK(buf[i] == expBuf[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst4q16") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst4.16 {d0, d2, d4, d6}, [r0]\n";

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
      {(QBDI::rword)&buf[24], 0xd0c0b0a, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[28], 0x21100f0e, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST4q16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST4q16", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setReg0[8] = {0x11, 0x22, 0x99, 0xaa, 0x02, 0x03, 0x0a, 0x0b};
  uint8_t setReg1[8] = {0x33, 0x44, 0xbb, 0xcc, 0x04, 0x05, 0x0c, 0x0d};
  uint8_t setReg2[8] = {0x55, 0x66, 0xdd, 0xee, 0x06, 0x07, 0x0e, 0x0f};
  uint8_t setReg3[8] = {0x77, 0x88, 0xff, 0x01, 0x08, 0x09, 0x10, 0x21};
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][0 + i] = setReg0[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[1][0 + i] = setReg1[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[2][0 + i] = setReg2[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[3][0 + i] = setReg3[i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  uint8_t expBuf[32] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                        0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                        0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                        0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x21};
  for (int i = 0; i < 32; i++) {
    CHECK(buf[i] == expBuf[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst4q16_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst4.16 {d0, d2, d4, d6}, [r0]!\n";

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
      {(QBDI::rword)&buf[24], 0xd0c0b0a, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[28], 0x21100f0e, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST4q16_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST4q16_UPD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("VST4q16_UPD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->r0 == (QBDI::rword)&buf[32]);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setReg0[8] = {0x11, 0x22, 0x99, 0xaa, 0x02, 0x03, 0x0a, 0x0b};
  uint8_t setReg1[8] = {0x33, 0x44, 0xbb, 0xcc, 0x04, 0x05, 0x0c, 0x0d};
  uint8_t setReg2[8] = {0x55, 0x66, 0xdd, 0xee, 0x06, 0x07, 0x0e, 0x0f};
  uint8_t setReg3[8] = {0x77, 0x88, 0xff, 0x01, 0x08, 0x09, 0x10, 0x21};
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][0 + i] = setReg0[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[1][0 + i] = setReg1[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[2][0 + i] = setReg2[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[3][0 + i] = setReg3[i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  uint8_t expBuf[32] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                        0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                        0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                        0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x21};
  for (int i = 0; i < 32; i++) {
    CHECK(buf[i] == expBuf[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst4d32") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst4.32 {d0, d1, d2, d3}, [r0]\n";

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
      {(QBDI::rword)&buf[24], 0xd0c0b0a, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[28], 0x21100f0e, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST4d32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST4d32", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setReg0[8] = {0x11, 0x22, 0x33, 0x44, 0x02, 0x03, 0x04, 0x05};
  uint8_t setReg1[8] = {0x55, 0x66, 0x77, 0x88, 0x06, 0x07, 0x08, 0x09};
  uint8_t setReg2[8] = {0x99, 0xaa, 0xbb, 0xcc, 0x0a, 0x0b, 0x0c, 0x0d};
  uint8_t setReg3[8] = {0xdd, 0xee, 0xff, 0x01, 0x0e, 0x0f, 0x10, 0x21};
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][0 + i] = setReg0[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][8 + i] = setReg1[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[1][0 + i] = setReg2[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[1][8 + i] = setReg3[i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  uint8_t expBuf[32] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                        0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                        0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                        0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x21};
  for (int i = 0; i < 32; i++) {
    CHECK(buf[i] == expBuf[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst4d32_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst4.32 {d0, d1, d2, d3}, [r0]!\n";

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
      {(QBDI::rword)&buf[24], 0xd0c0b0a, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[28], 0x21100f0e, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST4d32_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST4d32_UPD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("VST4d32_UPD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->r0 == (QBDI::rword)&buf[32]);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setReg0[8] = {0x11, 0x22, 0x33, 0x44, 0x02, 0x03, 0x04, 0x05};
  uint8_t setReg1[8] = {0x55, 0x66, 0x77, 0x88, 0x06, 0x07, 0x08, 0x09};
  uint8_t setReg2[8] = {0x99, 0xaa, 0xbb, 0xcc, 0x0a, 0x0b, 0x0c, 0x0d};
  uint8_t setReg3[8] = {0xdd, 0xee, 0xff, 0x01, 0x0e, 0x0f, 0x10, 0x21};
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][0 + i] = setReg0[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][8 + i] = setReg1[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[1][0 + i] = setReg2[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[1][8 + i] = setReg3[i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  uint8_t expBuf[32] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                        0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                        0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                        0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x21};
  for (int i = 0; i < 32; i++) {
    CHECK(buf[i] == expBuf[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst4q32") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst4.32 {d0, d2, d4, d6}, [r0]\n";

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
      {(QBDI::rword)&buf[24], 0xd0c0b0a, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[28], 0x21100f0e, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST4q32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST4q32", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setReg0[8] = {0x11, 0x22, 0x33, 0x44, 0x02, 0x03, 0x04, 0x05};
  uint8_t setReg1[8] = {0x55, 0x66, 0x77, 0x88, 0x06, 0x07, 0x08, 0x09};
  uint8_t setReg2[8] = {0x99, 0xaa, 0xbb, 0xcc, 0x0a, 0x0b, 0x0c, 0x0d};
  uint8_t setReg3[8] = {0xdd, 0xee, 0xff, 0x01, 0x0e, 0x0f, 0x10, 0x21};
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][0 + i] = setReg0[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[1][0 + i] = setReg1[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[2][0 + i] = setReg2[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[3][0 + i] = setReg3[i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  uint8_t expBuf[32] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                        0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                        0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                        0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x21};
  for (int i = 0; i < 32; i++) {
    CHECK(buf[i] == expBuf[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst4q32_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst4.32 {d0, d2, d4, d6}, [r0]!\n";

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
      {(QBDI::rword)&buf[24], 0xd0c0b0a, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[28], 0x21100f0e, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST4q32_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST4q32_UPD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("VST4q32_UPD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->r0 == (QBDI::rword)&buf[32]);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setReg0[8] = {0x11, 0x22, 0x33, 0x44, 0x02, 0x03, 0x04, 0x05};
  uint8_t setReg1[8] = {0x55, 0x66, 0x77, 0x88, 0x06, 0x07, 0x08, 0x09};
  uint8_t setReg2[8] = {0x99, 0xaa, 0xbb, 0xcc, 0x0a, 0x0b, 0x0c, 0x0d};
  uint8_t setReg3[8] = {0xdd, 0xee, 0xff, 0x01, 0x0e, 0x0f, 0x10, 0x21};
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][0 + i] = setReg0[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[1][0 + i] = setReg1[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[2][0 + i] = setReg2[i];
  }
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[3][0 + i] = setReg3[i];
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  uint8_t expBuf[32] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                        0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                        0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                        0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x21};
  for (int i = 0; i < 32; i++) {
    CHECK(buf[i] == expBuf[i]);
  }
}
