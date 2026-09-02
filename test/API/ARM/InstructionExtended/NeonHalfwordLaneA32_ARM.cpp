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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld1dupd16") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld1.16 {d0[]}, [r0]\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x2211, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD1DUPd16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD1DUPd16", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 8; i += 2) {
    CHECK(fpr->vreg.q[0][i] == 0x11);
    CHECK(fpr->vreg.q[0][i + 1] == 0x22);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld1dupd16wb_fixed") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld1.16 {d0[]}, [r0]!\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x2211, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD1DUPd16wb_fixed", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VLD1DUPd16wb_fixed", QBDI::POSTINST, checkAccess,
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
  for (int i = 0; i < 8; i += 2) {
    CHECK(fpr->vreg.q[0][i] == 0x11);
    CHECK(fpr->vreg.q[0][i + 1] == 0x22);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[2]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld1dupd16wb_register") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld1.16 {d0[]}, [r0], r2\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x2211, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD1DUPd16wb_register", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VLD1DUPd16wb_register", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 8; i += 2) {
    CHECK(fpr->vreg.q[0][i] == 0x11);
    CHECK(fpr->vreg.q[0][i + 1] == 0x22);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[4]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld1dupq16") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld1.16 {q0[]}, [r0]\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x2211, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD1DUPq16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD1DUPq16", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 16; i += 2) {
    CHECK(fpr->vreg.q[0][i] == 0x11);
    CHECK(fpr->vreg.q[0][i + 1] == 0x22);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld1dupq16wb_fixed") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld1.16 {q0[]}, [r0]!\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x2211, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD1DUPq16wb_fixed", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VLD1DUPq16wb_fixed", QBDI::POSTINST, checkAccess,
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
  for (int i = 0; i < 16; i += 2) {
    CHECK(fpr->vreg.q[0][i] == 0x11);
    CHECK(fpr->vreg.q[0][i + 1] == 0x22);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[2]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld1dupq16wb_register") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld1.16 {q0[]}, [r0], r2\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x2211, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD1DUPq16wb_register", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VLD1DUPq16wb_register", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 16; i += 2) {
    CHECK(fpr->vreg.q[0][i] == 0x11);
    CHECK(fpr->vreg.q[0][i + 1] == 0x22);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[4]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld1lnd16") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld1.16 {d0[3]}, [r0]\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x2211, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD1LNd16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD1LNd16", QBDI::POSTINST, checkAccess, &expectedPost);

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
  CHECK(fpr->vreg.q[0][6] == 0x11);
  CHECK(fpr->vreg.q[0][7] == 0x22);
  for (int i = 0; i < 6; i++) {
    CHECK(fpr->vreg.q[0][i] == 0xcc);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld1lnd16_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld1.16 {d0[3]}, [r0]!\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x2211, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD1LNd16_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD1LNd16_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

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
  CHECK(fpr->vreg.q[0][6] == 0x11);
  CHECK(fpr->vreg.q[0][7] == 0x22);
  for (int i = 0; i < 6; i++) {
    CHECK(fpr->vreg.q[0][i] == 0xcc);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[2]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2dupd8") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.8 {d0[], d1[]}, [r0]\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x2211, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD2DUPd8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD2DUPd8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][i] == 0x11);
  }
  for (int i = 8; i < 16; i++) {
    CHECK(fpr->vreg.q[0][i] == 0x22);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2dupd8wb_fixed") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.8 {d0[], d1[]}, [r0]!\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x2211, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD2DUPd8wb_fixed", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VLD2DUPd8wb_fixed", QBDI::POSTINST, checkAccess,
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
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][i] == 0x11);
  }
  for (int i = 8; i < 16; i++) {
    CHECK(fpr->vreg.q[0][i] == 0x22);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[2]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2dupd8wb_register") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.8 {d0[], d1[]}, [r0], r2\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x2211, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD2DUPd8wb_register", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VLD2DUPd8wb_register", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][i] == 0x11);
  }
  for (int i = 8; i < 16; i++) {
    CHECK(fpr->vreg.q[0][i] == 0x22);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[4]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2dupd8x2") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.8 {d0[], d2[]}, [r0]\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x2211, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD2DUPd8x2", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD2DUPd8x2", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][i] == 0x11);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][i] == 0x22);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2dupd8x2wb_fixed") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.8 {d0[], d2[]}, [r0]!\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x2211, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD2DUPd8x2wb_fixed", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VLD2DUPd8x2wb_fixed", QBDI::POSTINST, checkAccess,
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
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][i] == 0x11);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][i] == 0x22);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[2]);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-vld2dupd8x2wb_register") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.8 {d0[], d2[]}, [r0], r2\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x2211, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD2DUPd8x2wb_register", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VLD2DUPd8x2wb_register", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[0][i] == 0x11);
  }
  for (int i = 0; i < 8; i++) {
    CHECK(fpr->vreg.q[1][i] == 0x22);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[4]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2lnd8") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.8 {d0[3], d1[3]}, [r0]\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x2211, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD2LNd8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD2LNd8", QBDI::POSTINST, checkAccess, &expectedPost);

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
  CHECK(fpr->vreg.q[0][3] == 0x11);
  CHECK(fpr->vreg.q[0][11] == 0x22);
  for (int i = 0; i < 8; i++) {
    if (i != 3) {
      CHECK(fpr->vreg.q[0][i] == 0xcc);
    }
    if (i != 3) {
      CHECK(fpr->vreg.q[0][i + 8] == 0xcc);
    }
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2lnd8_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.8 {d0[3], d1[3]}, [r0]!\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x2211, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD2LNd8_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD2LNd8_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

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
  CHECK(fpr->vreg.q[0][3] == 0x11);
  CHECK(fpr->vreg.q[0][11] == 0x22);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[2]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst1lnd16") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst1.16 {d0[3]}, [r0]\n";

  uint8_t buf[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xabcd, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST1LNd16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST1LNd16", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][i] = 0x11 * (i + 1);
  }
  fpr->vreg.q[0][6] = 0xcd;
  fpr->vreg.q[0][7] = 0xab;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == 0xcd);
  CHECK(buf[1] == 0xab);

  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst1lnd16_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst1.16 {d0[3]}, [r0]!\n";

  uint8_t buf[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xabcd, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST1LNd16_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST1LNd16_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][i] = 0x11 * (i + 1);
  }
  fpr->vreg.q[0][6] = 0xcd;
  fpr->vreg.q[0][7] = 0xab;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == 0xcd);
  CHECK(buf[1] == 0xab);

  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[2]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst2lnd8") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst2.8 {d0[3], d1[3]}, [r0]\n";

  uint8_t buf[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x22cd, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST2LNd8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST2LNd8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = 0x11 * (i + 1);
  }
  fpr->vreg.q[0][3] = 0xcd;
  fpr->vreg.q[0][11] = 0x22;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == 0xcd);
  CHECK(buf[1] == 0x22);

  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst2lnd8_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst2.8 {d0[3], d1[3]}, [r0]!\n";

  uint8_t buf[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x22cd, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST2LNd8_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST2LNd8_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 16; i++) {
    fpr->vreg.q[0][i] = 0x11 * (i + 1);
  }
  fpr->vreg.q[0][3] = 0xcd;
  fpr->vreg.q[0][11] = 0x22;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == 0xcd);
  CHECK(buf[1] == 0x22);

  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[2]);
}
