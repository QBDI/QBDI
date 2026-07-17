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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2q8") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.8 {d0, d1, d2, d3}, [r0]\n";

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
  vm.addMnemonicCB("VLD2q8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD2q8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  uint8_t expQ0[16] = {0x11, 0x33, 0x55, 0x77, 0x99, 0xbb, 0xdd, 0xff,
                       0x02, 0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0e, 0x10};
  uint8_t expQ1[16] = {0x22, 0x44, 0x66, 0x88, 0xaa, 0xcc, 0xee, 0x01,
                       0x03, 0x05, 0x07, 0x09, 0x0b, 0x0d, 0x0f, 0x21};
  for (int i = 0; i < 16; i++) {
    CHECK(fpr->vreg.q[0][i] == expQ0[i]);
    CHECK(fpr->vreg.q[1][i] == expQ1[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2q8wb_fixed") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.8 {d0, d1, d2, d3}, [r0]!\n";

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
  vm.addMnemonicCB("VLD2q8wb_fixed", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD2q8wb_fixed", QBDI::POSTINST, checkAccess,
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
  uint8_t expQ0[16] = {0x11, 0x33, 0x55, 0x77, 0x99, 0xbb, 0xdd, 0xff,
                       0x02, 0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0e, 0x10};
  uint8_t expQ1[16] = {0x22, 0x44, 0x66, 0x88, 0xaa, 0xcc, 0xee, 0x01,
                       0x03, 0x05, 0x07, 0x09, 0x0b, 0x0d, 0x0f, 0x21};
  for (int i = 0; i < 16; i++) {
    CHECK(fpr->vreg.q[0][i] == expQ0[i]);
    CHECK(fpr->vreg.q[1][i] == expQ1[i]);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[32]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2q8wb_register") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.8 {d0, d1, d2, d3}, [r0], r2\n";

  uint8_t buf[40] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                     0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                     0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                     0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x21,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
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
  vm.addMnemonicCB("VLD2q8wb_register", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VLD2q8wb_register", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 36;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  uint8_t expQ0[16] = {0x11, 0x33, 0x55, 0x77, 0x99, 0xbb, 0xdd, 0xff,
                       0x02, 0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0e, 0x10};
  uint8_t expQ1[16] = {0x22, 0x44, 0x66, 0x88, 0xaa, 0xcc, 0xee, 0x01,
                       0x03, 0x05, 0x07, 0x09, 0x0b, 0x0d, 0x0f, 0x21};
  for (int i = 0; i < 16; i++) {
    CHECK(fpr->vreg.q[0][i] == expQ0[i]);
    CHECK(fpr->vreg.q[1][i] == expQ1[i]);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[36]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2q16") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.16 {d0, d1, d2, d3}, [r0]\n";

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
  vm.addMnemonicCB("VLD2q16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD2q16", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  uint8_t expQ0[16] = {0x11, 0x22, 0x55, 0x66, 0x99, 0xaa, 0xdd, 0xee,
                       0x02, 0x03, 0x06, 0x07, 0x0a, 0x0b, 0x0e, 0x0f};
  uint8_t expQ1[16] = {0x33, 0x44, 0x77, 0x88, 0xbb, 0xcc, 0xff, 0x01,
                       0x04, 0x05, 0x08, 0x09, 0x0c, 0x0d, 0x10, 0x21};
  for (int i = 0; i < 16; i++) {
    CHECK(fpr->vreg.q[0][i] == expQ0[i]);
    CHECK(fpr->vreg.q[1][i] == expQ1[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2q16wb_fixed") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.16 {d0, d1, d2, d3}, [r0]!\n";

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
  vm.addMnemonicCB("VLD2q16wb_fixed", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD2q16wb_fixed", QBDI::POSTINST, checkAccess,
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
  uint8_t expQ0[16] = {0x11, 0x22, 0x55, 0x66, 0x99, 0xaa, 0xdd, 0xee,
                       0x02, 0x03, 0x06, 0x07, 0x0a, 0x0b, 0x0e, 0x0f};
  uint8_t expQ1[16] = {0x33, 0x44, 0x77, 0x88, 0xbb, 0xcc, 0xff, 0x01,
                       0x04, 0x05, 0x08, 0x09, 0x0c, 0x0d, 0x10, 0x21};
  for (int i = 0; i < 16; i++) {
    CHECK(fpr->vreg.q[0][i] == expQ0[i]);
    CHECK(fpr->vreg.q[1][i] == expQ1[i]);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[32]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2q16wb_register") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.16 {d0, d1, d2, d3}, [r0], r2\n";

  uint8_t buf[40] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                     0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                     0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                     0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x21,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
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
  vm.addMnemonicCB("VLD2q16wb_register", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VLD2q16wb_register", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 36;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  uint8_t expQ0[16] = {0x11, 0x22, 0x55, 0x66, 0x99, 0xaa, 0xdd, 0xee,
                       0x02, 0x03, 0x06, 0x07, 0x0a, 0x0b, 0x0e, 0x0f};
  uint8_t expQ1[16] = {0x33, 0x44, 0x77, 0x88, 0xbb, 0xcc, 0xff, 0x01,
                       0x04, 0x05, 0x08, 0x09, 0x0c, 0x0d, 0x10, 0x21};
  for (int i = 0; i < 16; i++) {
    CHECK(fpr->vreg.q[0][i] == expQ0[i]);
    CHECK(fpr->vreg.q[1][i] == expQ1[i]);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[36]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2q32") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.32 {d0, d1, d2, d3}, [r0]\n";

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
  vm.addMnemonicCB("VLD2q32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD2q32", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  uint8_t expQ0[16] = {0x11, 0x22, 0x33, 0x44, 0x99, 0xaa, 0xbb, 0xcc,
                       0x02, 0x03, 0x04, 0x05, 0x0a, 0x0b, 0x0c, 0x0d};
  uint8_t expQ1[16] = {0x55, 0x66, 0x77, 0x88, 0xdd, 0xee, 0xff, 0x01,
                       0x06, 0x07, 0x08, 0x09, 0x0e, 0x0f, 0x10, 0x21};
  for (int i = 0; i < 16; i++) {
    CHECK(fpr->vreg.q[0][i] == expQ0[i]);
    CHECK(fpr->vreg.q[1][i] == expQ1[i]);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2q32wb_fixed") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.32 {d0, d1, d2, d3}, [r0]!\n";

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
  vm.addMnemonicCB("VLD2q32wb_fixed", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD2q32wb_fixed", QBDI::POSTINST, checkAccess,
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
  uint8_t expQ0[16] = {0x11, 0x22, 0x33, 0x44, 0x99, 0xaa, 0xbb, 0xcc,
                       0x02, 0x03, 0x04, 0x05, 0x0a, 0x0b, 0x0c, 0x0d};
  uint8_t expQ1[16] = {0x55, 0x66, 0x77, 0x88, 0xdd, 0xee, 0xff, 0x01,
                       0x06, 0x07, 0x08, 0x09, 0x0e, 0x0f, 0x10, 0x21};
  for (int i = 0; i < 16; i++) {
    CHECK(fpr->vreg.q[0][i] == expQ0[i]);
    CHECK(fpr->vreg.q[1][i] == expQ1[i]);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[32]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2q32wb_register") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.32 {d0, d1, d2, d3}, [r0], r2\n";

  uint8_t buf[40] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                     0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
                     0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                     0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x21,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
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
  vm.addMnemonicCB("VLD2q32wb_register", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VLD2q32wb_register", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 36;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  uint8_t expQ0[16] = {0x11, 0x22, 0x33, 0x44, 0x99, 0xaa, 0xbb, 0xcc,
                       0x02, 0x03, 0x04, 0x05, 0x0a, 0x0b, 0x0c, 0x0d};
  uint8_t expQ1[16] = {0x55, 0x66, 0x77, 0x88, 0xdd, 0xee, 0xff, 0x01,
                       0x06, 0x07, 0x08, 0x09, 0x0e, 0x0f, 0x10, 0x21};
  for (int i = 0; i < 16; i++) {
    CHECK(fpr->vreg.q[0][i] == expQ0[i]);
    CHECK(fpr->vreg.q[1][i] == expQ1[i]);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[36]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst2q8") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst2.8 {d0, d1, d2, d3}, [r0]\n";

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
  vm.addMnemonicCB("VST2q8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST2q8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setQ0[16] = {0x11, 0x33, 0x55, 0x77, 0x99, 0xbb, 0xdd, 0xff,
                       0x02, 0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0e, 0x10};
  uint8_t setQ1[16] = {0x22, 0x44, 0x66, 0x88, 0xaa, 0xcc, 0xee, 0x01,
                       0x03, 0x05, 0x07, 0x09, 0x0b, 0x0d, 0x0f, 0x21};
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = setQ0[i];
    fpr->vreg.q[1][i] = setQ1[i];
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst2q8wb_fixed") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst2.8 {d0, d1, d2, d3}, [r0]!\n";

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
  vm.addMnemonicCB("VST2q8wb_fixed", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST2q8wb_fixed", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setQ0[16] = {0x11, 0x33, 0x55, 0x77, 0x99, 0xbb, 0xdd, 0xff,
                       0x02, 0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0e, 0x10};
  uint8_t setQ1[16] = {0x22, 0x44, 0x66, 0x88, 0xaa, 0xcc, 0xee, 0x01,
                       0x03, 0x05, 0x07, 0x09, 0x0b, 0x0d, 0x0f, 0x21};
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = setQ0[i];
    fpr->vreg.q[1][i] = setQ1[i];
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
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[32]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst2q8wb_register") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst2.8 {d0, d1, d2, d3}, [r0], r2\n";

  uint8_t buf[40] = {0};
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
  vm.addMnemonicCB("VST2q8wb_register", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VST2q8wb_register", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 36;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setQ0[16] = {0x11, 0x33, 0x55, 0x77, 0x99, 0xbb, 0xdd, 0xff,
                       0x02, 0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0e, 0x10};
  uint8_t setQ1[16] = {0x22, 0x44, 0x66, 0x88, 0xaa, 0xcc, 0xee, 0x01,
                       0x03, 0x05, 0x07, 0x09, 0x0b, 0x0d, 0x0f, 0x21};
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = setQ0[i];
    fpr->vreg.q[1][i] = setQ1[i];
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
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[36]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst2q16") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst2.16 {d0, d1, d2, d3}, [r0]\n";

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
  vm.addMnemonicCB("VST2q16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST2q16", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setQ0[16] = {0x11, 0x22, 0x55, 0x66, 0x99, 0xaa, 0xdd, 0xee,
                       0x02, 0x03, 0x06, 0x07, 0x0a, 0x0b, 0x0e, 0x0f};
  uint8_t setQ1[16] = {0x33, 0x44, 0x77, 0x88, 0xbb, 0xcc, 0xff, 0x01,
                       0x04, 0x05, 0x08, 0x09, 0x0c, 0x0d, 0x10, 0x21};
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = setQ0[i];
    fpr->vreg.q[1][i] = setQ1[i];
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst2q16wb_fixed") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst2.16 {d0, d1, d2, d3}, [r0]!\n";

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
  vm.addMnemonicCB("VST2q16wb_fixed", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST2q16wb_fixed", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setQ0[16] = {0x11, 0x22, 0x55, 0x66, 0x99, 0xaa, 0xdd, 0xee,
                       0x02, 0x03, 0x06, 0x07, 0x0a, 0x0b, 0x0e, 0x0f};
  uint8_t setQ1[16] = {0x33, 0x44, 0x77, 0x88, 0xbb, 0xcc, 0xff, 0x01,
                       0x04, 0x05, 0x08, 0x09, 0x0c, 0x0d, 0x10, 0x21};
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = setQ0[i];
    fpr->vreg.q[1][i] = setQ1[i];
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
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[32]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst2q16wb_register") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst2.16 {d0, d1, d2, d3}, [r0], r2\n";

  uint8_t buf[40] = {0};
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
  vm.addMnemonicCB("VST2q16wb_register", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VST2q16wb_register", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 36;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setQ0[16] = {0x11, 0x22, 0x55, 0x66, 0x99, 0xaa, 0xdd, 0xee,
                       0x02, 0x03, 0x06, 0x07, 0x0a, 0x0b, 0x0e, 0x0f};
  uint8_t setQ1[16] = {0x33, 0x44, 0x77, 0x88, 0xbb, 0xcc, 0xff, 0x01,
                       0x04, 0x05, 0x08, 0x09, 0x0c, 0x0d, 0x10, 0x21};
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = setQ0[i];
    fpr->vreg.q[1][i] = setQ1[i];
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
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[36]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst2q32") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst2.32 {d0, d1, d2, d3}, [r0]\n";

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
  vm.addMnemonicCB("VST2q32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST2q32", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setQ0[16] = {0x11, 0x22, 0x33, 0x44, 0x99, 0xaa, 0xbb, 0xcc,
                       0x02, 0x03, 0x04, 0x05, 0x0a, 0x0b, 0x0c, 0x0d};
  uint8_t setQ1[16] = {0x55, 0x66, 0x77, 0x88, 0xdd, 0xee, 0xff, 0x01,
                       0x06, 0x07, 0x08, 0x09, 0x0e, 0x0f, 0x10, 0x21};
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = setQ0[i];
    fpr->vreg.q[1][i] = setQ1[i];
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst2q32wb_fixed") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst2.32 {d0, d1, d2, d3}, [r0]!\n";

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
  vm.addMnemonicCB("VST2q32wb_fixed", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST2q32wb_fixed", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setQ0[16] = {0x11, 0x22, 0x33, 0x44, 0x99, 0xaa, 0xbb, 0xcc,
                       0x02, 0x03, 0x04, 0x05, 0x0a, 0x0b, 0x0c, 0x0d};
  uint8_t setQ1[16] = {0x55, 0x66, 0x77, 0x88, 0xdd, 0xee, 0xff, 0x01,
                       0x06, 0x07, 0x08, 0x09, 0x0e, 0x0f, 0x10, 0x21};
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = setQ0[i];
    fpr->vreg.q[1][i] = setQ1[i];
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
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[32]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst2q32wb_register") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst2.32 {d0, d1, d2, d3}, [r0], r2\n";

  uint8_t buf[40] = {0};
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
  vm.addMnemonicCB("VST2q32wb_register", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VST2q32wb_register", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 36;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  uint8_t setQ0[16] = {0x11, 0x22, 0x33, 0x44, 0x99, 0xaa, 0xbb, 0xcc,
                       0x02, 0x03, 0x04, 0x05, 0x0a, 0x0b, 0x0c, 0x0d};
  uint8_t setQ1[16] = {0x55, 0x66, 0x77, 0x88, 0xdd, 0xee, 0xff, 0x01,
                       0x06, 0x07, 0x08, 0x09, 0x0e, 0x0f, 0x10, 0x21};
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = setQ0[i];
    fpr->vreg.q[1][i] = setQ1[i];
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
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[36]);
}
