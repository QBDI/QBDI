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
#include <catch2/catch_test_macros.hpp>
#include "API/APITest.h"
#include "MemAccessTestUtils_ARM.h"

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::checkFeature;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld1dupd32") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld1.32 {d0[]}, [r0]\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD1DUPd32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD1DUPd32", QBDI::POSTINST, checkAccess, &expectedPost);

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
  for (int i = 0; i < 8; i += 4) {
    CHECK(fpr->vreg.q[0][i] == 0x11);
    CHECK(fpr->vreg.q[0][i + 1] == 0x22);
    CHECK(fpr->vreg.q[0][i + 2] == 0x33);
    CHECK(fpr->vreg.q[0][i + 3] == 0x44);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld1dupd32wb_fixed") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld1.32 {d0[]}, [r0]!\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD1DUPd32wb_fixed", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VLD1DUPd32wb_fixed", QBDI::POSTINST, checkAccess,
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
  for (int i = 0; i < 8; i += 4) {
    CHECK(fpr->vreg.q[0][i] == 0x11);
    CHECK(fpr->vreg.q[0][i + 1] == 0x22);
    CHECK(fpr->vreg.q[0][i + 2] == 0x33);
    CHECK(fpr->vreg.q[0][i + 3] == 0x44);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[4]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld1dupd32wb_register") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld1.32 {d0[]}, [r0], r2\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD1DUPd32wb_register", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VLD1DUPd32wb_register", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 6;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  QBDI::GPRState *finalState = vm.getGPRState();
  for (int i = 0; i < 8; i += 4) {
    CHECK(fpr->vreg.q[0][i] == 0x11);
    CHECK(fpr->vreg.q[0][i + 1] == 0x22);
    CHECK(fpr->vreg.q[0][i + 2] == 0x33);
    CHECK(fpr->vreg.q[0][i + 3] == 0x44);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[6]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld1dupq32") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld1.32 {q0[]}, [r0]\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD1DUPq32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD1DUPq32", QBDI::POSTINST, checkAccess, &expectedPost);

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
  for (int i = 0; i < 16; i += 4) {
    CHECK(fpr->vreg.q[0][i] == 0x11);
    CHECK(fpr->vreg.q[0][i + 1] == 0x22);
    CHECK(fpr->vreg.q[0][i + 2] == 0x33);
    CHECK(fpr->vreg.q[0][i + 3] == 0x44);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld1dupq32wb_fixed") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld1.32 {q0[]}, [r0]!\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD1DUPq32wb_fixed", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VLD1DUPq32wb_fixed", QBDI::POSTINST, checkAccess,
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
  for (int i = 0; i < 16; i += 4) {
    CHECK(fpr->vreg.q[0][i] == 0x11);
    CHECK(fpr->vreg.q[0][i + 1] == 0x22);
    CHECK(fpr->vreg.q[0][i + 2] == 0x33);
    CHECK(fpr->vreg.q[0][i + 3] == 0x44);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[4]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld1dupq32wb_register") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld1.32 {q0[]}, [r0], r2\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD1DUPq32wb_register", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VLD1DUPq32wb_register", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 6;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  QBDI::GPRState *finalState = vm.getGPRState();
  for (int i = 0; i < 16; i += 4) {
    CHECK(fpr->vreg.q[0][i] == 0x11);
    CHECK(fpr->vreg.q[0][i + 1] == 0x22);
    CHECK(fpr->vreg.q[0][i + 2] == 0x33);
    CHECK(fpr->vreg.q[0][i + 3] == 0x44);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[6]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld1lnd32") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld1.32 {d0[1]}, [r0]\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD1LNd32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD1LNd32", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][i] = 0xcc;
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  fpr = vm.getFPRState();
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(fpr->vreg.q[0][4] == 0x11);
  CHECK(fpr->vreg.q[0][5] == 0x22);
  CHECK(fpr->vreg.q[0][6] == 0x33);
  CHECK(fpr->vreg.q[0][7] == 0x44);
  for (int i = 0; i < 4; i++) {
    CHECK(fpr->vreg.q[0][i] == 0xcc);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld1lnd32_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld1.32 {d0[1]}, [r0]!\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD1LNd32_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD1LNd32_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][i] = 0xcc;
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  fpr = vm.getFPRState();
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(fpr->vreg.q[0][4] == 0x11);
  CHECK(fpr->vreg.q[0][5] == 0x22);
  CHECK(fpr->vreg.q[0][6] == 0x33);
  CHECK(fpr->vreg.q[0][7] == 0x44);
  CHECK(finalState->r0 == (QBDI::rword)&buf[4]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2dupd16") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.16 {d0[], d1[]}, [r0]\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD2DUPd16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD2DUPd16", QBDI::POSTINST, checkAccess, &expectedPost);

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
  for (int i = 0; i < 8; i += 2) {
    CHECK(fpr->vreg.q[0][i] == 0x11);
    CHECK(fpr->vreg.q[0][i + 1] == 0x22);
  }
  for (int i = 8; i < 16; i += 2) {
    CHECK(fpr->vreg.q[0][i] == 0x33);
    CHECK(fpr->vreg.q[0][i + 1] == 0x44);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2dupd16wb_fixed") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.16 {d0[], d1[]}, [r0]!\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD2DUPd16wb_fixed", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VLD2DUPd16wb_fixed", QBDI::POSTINST, checkAccess,
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
  for (int i = 0; i < 8; i += 2) {
    CHECK(fpr->vreg.q[0][i] == 0x11);
    CHECK(fpr->vreg.q[0][i + 1] == 0x22);
  }
  for (int i = 8; i < 16; i += 2) {
    CHECK(fpr->vreg.q[0][i] == 0x33);
    CHECK(fpr->vreg.q[0][i + 1] == 0x44);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[4]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2dupd16wb_register") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.16 {d0[], d1[]}, [r0], r2\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD2DUPd16wb_register", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VLD2DUPd16wb_register", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 6;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  QBDI::GPRState *finalState = vm.getGPRState();
  for (int i = 0; i < 8; i += 2) {
    CHECK(fpr->vreg.q[0][i] == 0x11);
    CHECK(fpr->vreg.q[0][i + 1] == 0x22);
  }
  for (int i = 8; i < 16; i += 2) {
    CHECK(fpr->vreg.q[0][i] == 0x33);
    CHECK(fpr->vreg.q[0][i + 1] == 0x44);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[6]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2dupd16x2") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.16 {d0[], d2[]}, [r0]\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD2DUPd16x2", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD2DUPd16x2", QBDI::POSTINST, checkAccess, &expectedPost);

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
  for (int i = 0; i < 8; i += 2) {
    CHECK(fpr->vreg.q[0][i] == 0x11);
    CHECK(fpr->vreg.q[0][i + 1] == 0x22);
  }
  for (int i = 0; i < 8; i += 2) {
    CHECK(fpr->vreg.q[1][i] == 0x33);
    CHECK(fpr->vreg.q[1][i + 1] == 0x44);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2dupd16x2wb_fixed") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.16 {d0[], d2[]}, [r0]!\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD2DUPd16x2wb_fixed", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VLD2DUPd16x2wb_fixed", QBDI::POSTINST, checkAccess,
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
  for (int i = 0; i < 8; i += 2) {
    CHECK(fpr->vreg.q[0][i] == 0x11);
    CHECK(fpr->vreg.q[0][i + 1] == 0x22);
  }
  for (int i = 0; i < 8; i += 2) {
    CHECK(fpr->vreg.q[1][i] == 0x33);
    CHECK(fpr->vreg.q[1][i + 1] == 0x44);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[4]);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-vld2dupd16x2wb_register") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.16 {d0[], d2[]}, [r0], r2\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD2DUPd16x2wb_register", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VLD2DUPd16x2wb_register", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 6;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  QBDI::GPRState *finalState = vm.getGPRState();
  for (int i = 0; i < 8; i += 2) {
    CHECK(fpr->vreg.q[0][i] == 0x11);
    CHECK(fpr->vreg.q[0][i + 1] == 0x22);
  }
  for (int i = 0; i < 8; i += 2) {
    CHECK(fpr->vreg.q[1][i] == 0x33);
    CHECK(fpr->vreg.q[1][i + 1] == 0x44);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[6]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2lnd16") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.16 {d0[1], d1[1]}, [r0]\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD2LNd16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD2LNd16", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = 0xcc;
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  fpr = vm.getFPRState();
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(fpr->vreg.q[0][2] == 0x11);
  CHECK(fpr->vreg.q[0][3] == 0x22);
  CHECK(fpr->vreg.q[0][10] == 0x33);
  CHECK(fpr->vreg.q[0][11] == 0x44);
  for (int i : {0, 1, 4, 5, 6, 7, 8, 9, 12, 13, 14, 15}) {
    CHECK(fpr->vreg.q[0][i] == 0xcc);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2lnd16_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.16 {d0[1], d1[1]}, [r0]!\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD2LNd16_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD2LNd16_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = 0xcc;
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  fpr = vm.getFPRState();
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(fpr->vreg.q[0][2] == 0x11);
  CHECK(fpr->vreg.q[0][3] == 0x22);
  CHECK(fpr->vreg.q[0][10] == 0x33);
  CHECK(fpr->vreg.q[0][11] == 0x44);
  CHECK(finalState->r0 == (QBDI::rword)&buf[4]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2lnq16") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.16 {d0[1], d2[1]}, [r0]\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD2LNq16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD2LNq16", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = 0xcc;
    fpr->vreg.q[1][i] = 0xcc;
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  fpr = vm.getFPRState();
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(fpr->vreg.q[0][2] == 0x11);
  CHECK(fpr->vreg.q[0][3] == 0x22);
  CHECK(fpr->vreg.q[1][2] == 0x33);
  CHECK(fpr->vreg.q[1][3] == 0x44);
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2lnq16_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.16 {d0[1], d2[1]}, [r0]!\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD2LNq16_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD2LNq16_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = 0xcc;
    fpr->vreg.q[1][i] = 0xcc;
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  fpr = vm.getFPRState();
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(fpr->vreg.q[0][2] == 0x11);
  CHECK(fpr->vreg.q[0][3] == 0x22);
  CHECK(fpr->vreg.q[1][2] == 0x33);
  CHECK(fpr->vreg.q[1][3] == 0x44);
  CHECK(finalState->r0 == (QBDI::rword)&buf[4]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4dupd8") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.8 {d0[], d1[], d2[], d3[]}, [r0]\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4DUPd8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4DUPd8", QBDI::POSTINST, checkAccess, &expectedPost);

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
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][i] == 0x11);
  }
  for (int i = 8; i < 16; i++) {
    CHECK(fpr->vreg.q[0][i] == 0x22);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][i] == 0x33);
  }
  for (int i = 8; i < 16; i++) {
    CHECK(fpr->vreg.q[1][i] == 0x44);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4dupd8_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.8 {d0[], d1[], d2[], d3[]}, [r0]!\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4DUPd8_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4DUPd8_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

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
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][i] == 0x11);
  }
  for (int i = 8; i < 16; i++) {
    CHECK(fpr->vreg.q[0][i] == 0x22);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][i] == 0x33);
  }
  for (int i = 8; i < 16; i++) {
    CHECK(fpr->vreg.q[1][i] == 0x44);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[4]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4dupq8") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.8 {d0[], d2[], d4[], d6[]}, [r0]\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4DUPq8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4DUPq8", QBDI::POSTINST, checkAccess, &expectedPost);

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
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][i] == 0x11);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][i] == 0x22);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[2][i] == 0x33);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[3][i] == 0x44);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4dupq8_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.8 {d0[], d2[], d4[], d6[]}, [r0]!\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4DUPq8_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4DUPq8_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

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
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][i] == 0x11);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][i] == 0x22);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[2][i] == 0x33);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[3][i] == 0x44);
  }
  CHECK(finalState->r0 == (QBDI::rword)&buf[4]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4lnd8") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.8 {d0[1], d1[1], d2[1], d3[1]}, [r0]\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4LNd8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4LNd8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = 0xcc;
    fpr->vreg.q[1][i] = 0xcc;
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  fpr = vm.getFPRState();
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(fpr->vreg.q[0][1] == 0x11);
  CHECK(fpr->vreg.q[0][9] == 0x22);
  CHECK(fpr->vreg.q[1][1] == 0x33);
  CHECK(fpr->vreg.q[1][9] == 0x44);
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4lnd8_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.8 {d0[1], d1[1], d2[1], d3[1]}, [r0]!\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4LNd8_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4LNd8_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = 0xcc;
    fpr->vreg.q[1][i] = 0xcc;
  }
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  fpr = vm.getFPRState();
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(fpr->vreg.q[0][1] == 0x11);
  CHECK(fpr->vreg.q[0][9] == 0x22);
  CHECK(fpr->vreg.q[1][1] == 0x33);
  CHECK(fpr->vreg.q[1][9] == 0x44);
  CHECK(finalState->r0 == (QBDI::rword)&buf[4]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst1lnd32") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst1.32 {d0[1]}, [r0]\n";

  uint8_t buf[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xaabbccdd, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST1LNd32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST1LNd32", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->vreg.q[0][0] = 0x11;
  fpr->vreg.q[0][1] = 0x22;
  fpr->vreg.q[0][2] = 0x33;
  fpr->vreg.q[0][3] = 0x44;
  fpr->vreg.q[0][4] = 0xdd;
  fpr->vreg.q[0][5] = 0xcc;
  fpr->vreg.q[0][6] = 0xbb;
  fpr->vreg.q[0][7] = 0xaa;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
  CHECK(*(uint32_t *)&buf[0] == 0xaabbccdd);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst1lnd32_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst1.32 {d0[1]}, [r0]!\n";

  uint8_t buf[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xaabbccdd, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST1LNd32_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST1LNd32_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->vreg.q[0][0] = 0x11;
  fpr->vreg.q[0][1] = 0x22;
  fpr->vreg.q[0][2] = 0x33;
  fpr->vreg.q[0][3] = 0x44;
  fpr->vreg.q[0][4] = 0xdd;
  fpr->vreg.q[0][5] = 0xcc;
  fpr->vreg.q[0][6] = 0xbb;
  fpr->vreg.q[0][7] = 0xaa;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[4]);
  CHECK(*(uint32_t *)&buf[0] == 0xaabbccdd);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst2lnd16") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst2.16 {d0[1], d1[1]}, [r0]\n";

  uint8_t buf[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x4433ccdd, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST2LNd16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST2LNd16", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = 0x11;
  }
  fpr->vreg.q[0][2] = 0xdd;
  fpr->vreg.q[0][3] = 0xcc;
  fpr->vreg.q[0][10] = 0x33;
  fpr->vreg.q[0][11] = 0x44;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
  CHECK(buf[0] == 0xdd);
  CHECK(buf[1] == 0xcc);
  CHECK(buf[2] == 0x33);
  CHECK(buf[3] == 0x44);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst2lnd16_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst2.16 {d0[1], d1[1]}, [r0]!\n";

  uint8_t buf[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x4433ccdd, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST2LNd16_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST2LNd16_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = 0x11;
  }
  fpr->vreg.q[0][2] = 0xdd;
  fpr->vreg.q[0][3] = 0xcc;
  fpr->vreg.q[0][10] = 0x33;
  fpr->vreg.q[0][11] = 0x44;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[4]);
  CHECK(buf[0] == 0xdd);
  CHECK(buf[1] == 0xcc);
  CHECK(buf[2] == 0x33);
  CHECK(buf[3] == 0x44);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst2lnq16") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst2.16 {d0[1], d2[1]}, [r0]\n";

  uint8_t buf[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x4433ccdd, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST2LNq16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST2LNq16", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = 0x11;
    fpr->vreg.q[1][i] = 0x11;
  }
  fpr->vreg.q[0][2] = 0xdd;
  fpr->vreg.q[0][3] = 0xcc;
  fpr->vreg.q[1][2] = 0x33;
  fpr->vreg.q[1][3] = 0x44;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
  CHECK(buf[0] == 0xdd);
  CHECK(buf[1] == 0xcc);
  CHECK(buf[2] == 0x33);
  CHECK(buf[3] == 0x44);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst2lnq16_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst2.16 {d0[1], d2[1]}, [r0]!\n";

  uint8_t buf[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x4433ccdd, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST2LNq16_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST2LNq16_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = 0x11;
    fpr->vreg.q[1][i] = 0x11;
  }
  fpr->vreg.q[0][2] = 0xdd;
  fpr->vreg.q[0][3] = 0xcc;
  fpr->vreg.q[1][2] = 0x33;
  fpr->vreg.q[1][3] = 0x44;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[4]);
  CHECK(buf[0] == 0xdd);
  CHECK(buf[1] == 0xcc);
  CHECK(buf[2] == 0x33);
  CHECK(buf[3] == 0x44);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst4lnd8") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst4.8 {d0[1], d1[1], d2[1], d3[1]}, [r0]\n";

  uint8_t buf[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x443322dd, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST4LNd8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST4LNd8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = 0x11;
    fpr->vreg.q[1][i] = 0x11;
  }
  fpr->vreg.q[0][1] = 0xdd;
  fpr->vreg.q[0][9] = 0x22;
  fpr->vreg.q[1][1] = 0x33;
  fpr->vreg.q[1][9] = 0x44;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
  CHECK(buf[0] == 0xdd);
  CHECK(buf[1] == 0x22);
  CHECK(buf[2] == 0x33);
  CHECK(buf[3] == 0x44);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst4lnd8_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst4.8 {d0[1], d1[1], d2[1], d3[1]}, [r0]!\n";

  uint8_t buf[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x443322dd, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST4LNd8_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST4LNd8_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = 0x11;
    fpr->vreg.q[1][i] = 0x11;
  }
  fpr->vreg.q[0][1] = 0xdd;
  fpr->vreg.q[0][9] = 0x22;
  fpr->vreg.q[1][1] = 0x33;
  fpr->vreg.q[1][9] = 0x44;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[4]);
  CHECK(buf[0] == 0xdd);
  CHECK(buf[1] == 0x22);
  CHECK(buf[2] == 0x33);
  CHECK(buf[3] == 0x44);
}
