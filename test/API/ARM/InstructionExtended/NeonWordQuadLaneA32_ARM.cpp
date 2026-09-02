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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2dupd32") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.32 {d0[], d1[]}, [r0]\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD2DUPd32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD2DUPd32", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 8; i += 4) {
    CHECK(fpr->vreg.q[0][i] == 0x11);
    CHECK(fpr->vreg.q[0][i + 1] == 0x22);
    CHECK(fpr->vreg.q[0][i + 2] == 0x33);
    CHECK(fpr->vreg.q[0][i + 3] == 0x44);
  }
  for (int i = 8; i < 16; i += 4) {
    CHECK(fpr->vreg.q[0][i] == 0x55);
    CHECK(fpr->vreg.q[0][i + 1] == 0x66);
    CHECK(fpr->vreg.q[0][i + 2] == 0x77);
    CHECK(fpr->vreg.q[0][i + 3] == 0x88);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2dupd32wb_fixed") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.32 {d0[], d1[]}, [r0]!\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD2DUPd32wb_fixed", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VLD2DUPd32wb_fixed", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[8]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2dupd32wb_register") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.32 {d0[], d1[]}, [r0], r2\n";

  uint8_t buf[16] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD2DUPd32wb_register", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VLD2DUPd32wb_register", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 10;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[10]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2dupd32x2") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.32 {d0[], d2[]}, [r0]\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD2DUPd32x2", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD2DUPd32x2", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  for (int i = 0; i < 8; i += 4) {
    CHECK(fpr->vreg.q[0][i] == 0x11);
    CHECK(fpr->vreg.q[0][i + 1] == 0x22);
    CHECK(fpr->vreg.q[0][i + 2] == 0x33);
    CHECK(fpr->vreg.q[0][i + 3] == 0x44);
  }
  for (int i = 0; i < 8; i += 4) {
    CHECK(fpr->vreg.q[1][i] == 0x55);
    CHECK(fpr->vreg.q[1][i + 1] == 0x66);
    CHECK(fpr->vreg.q[1][i + 2] == 0x77);
    CHECK(fpr->vreg.q[1][i + 3] == 0x88);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2dupd32x2wb_fixed") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.32 {d0[], d2[]}, [r0]!\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD2DUPd32x2wb_fixed", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VLD2DUPd32x2wb_fixed", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[8]);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-vld2dupd32x2wb_register") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.32 {d0[], d2[]}, [r0], r2\n";

  uint8_t buf[16] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD2DUPd32x2wb_register", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VLD2DUPd32x2wb_register", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 10;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[10]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2lnd32") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.32 {d0[0], d1[0]}, [r0]\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD2LNd32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD2LNd32", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0xcc, sizeof(fpr->vreg));
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  fpr = vm.getFPRState();
  CHECK(fpr->vreg.q[0][0] == 0x11);
  CHECK(fpr->vreg.q[0][1] == 0x22);
  CHECK(fpr->vreg.q[0][2] == 0x33);
  CHECK(fpr->vreg.q[0][3] == 0x44);
  CHECK(fpr->vreg.q[0][8] == 0x55);
  CHECK(fpr->vreg.q[0][9] == 0x66);
  CHECK(fpr->vreg.q[0][10] == 0x77);
  CHECK(fpr->vreg.q[0][11] == 0x88);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2lnd32_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.32 {d0[0], d1[0]}, [r0]!\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD2LNd32_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD2LNd32_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  CHECK(fpr->vreg.q[0][0] == 0x11);
  CHECK(fpr->vreg.q[0][8] == 0x55);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[8]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2lnq32") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.32 {d0[1], d2[1]}, [r0]\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD2LNq32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD2LNq32", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0xcc, sizeof(fpr->vreg));
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  fpr = vm.getFPRState();
  CHECK(fpr->vreg.q[0][4] == 0x11);
  CHECK(fpr->vreg.q[0][5] == 0x22);
  CHECK(fpr->vreg.q[0][6] == 0x33);
  CHECK(fpr->vreg.q[0][7] == 0x44);
  CHECK(fpr->vreg.q[1][4] == 0x55);
  CHECK(fpr->vreg.q[1][5] == 0x66);
  CHECK(fpr->vreg.q[1][6] == 0x77);
  CHECK(fpr->vreg.q[1][7] == 0x88);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld2lnq32_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld2.32 {d0[1], d2[1]}, [r0]!\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD2LNq32_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD2LNq32_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  CHECK(fpr->vreg.q[0][4] == 0x11);
  CHECK(fpr->vreg.q[1][4] == 0x55);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[8]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst2lnd32") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst2.32 {d0[0], d1[0]}, [r0]\n";

  uint8_t buf[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST2LNd32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST2LNd32", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  *(uint32_t *)&fpr->vreg.q[0][0] = 0x44332211;
  *(uint32_t *)&fpr->vreg.q[0][8] = 0x88776655;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[0] == 0x44332211);
  CHECK(*(uint32_t *)&buf[4] == 0x88776655);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst2lnd32_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst2.32 {d0[0], d1[0]}, [r0]!\n";

  uint8_t buf[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST2LNd32_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST2LNd32_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  *(uint32_t *)&fpr->vreg.q[0][0] = 0x44332211;
  *(uint32_t *)&fpr->vreg.q[0][8] = 0x88776655;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[0] == 0x44332211);
  CHECK(*(uint32_t *)&buf[4] == 0x88776655);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[8]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst2lnq32") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst2.32 {d0[1], d2[1]}, [r0]\n";

  uint8_t buf[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST2LNq32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST2LNq32", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  *(uint32_t *)&fpr->vreg.q[0][4] = 0x44332211;
  *(uint32_t *)&fpr->vreg.q[1][4] = 0x88776655;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[0] == 0x44332211);
  CHECK(*(uint32_t *)&buf[4] == 0x88776655);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst2lnq32_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst2.32 {d0[1], d2[1]}, [r0]!\n";

  uint8_t buf[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST2LNq32_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST2LNq32_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  *(uint32_t *)&fpr->vreg.q[0][4] = 0x44332211;
  *(uint32_t *)&fpr->vreg.q[1][4] = 0x88776655;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[0] == 0x44332211);
  CHECK(*(uint32_t *)&buf[4] == 0x88776655);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[8]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4dupd16") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.16 {d0[], d1[], d2[], d3[]}, [r0]\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4DUPd16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4DUPd16", QBDI::POSTINST, checkAccess, &expectedPost);

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
    CHECK(fpr->vreg.q[0][i + 8] == 0x33);
    CHECK(fpr->vreg.q[0][i + 9] == 0x44);
  }
  for (int i = 0; i < 8; i += 2) {
    CHECK(fpr->vreg.q[1][i] == 0x55);
    CHECK(fpr->vreg.q[1][i + 1] == 0x66);
    CHECK(fpr->vreg.q[1][i + 8] == 0x77);
    CHECK(fpr->vreg.q[1][i + 9] == 0x88);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4dupd16_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.16 {d0[], d1[], d2[], d3[]}, [r0]!\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4DUPd16_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4DUPd16_UPD", QBDI::POSTINST, checkAccess,
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
  CHECK(fpr->vreg.q[0][0] == 0x11);
  CHECK(fpr->vreg.q[1][8] == 0x77);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[8]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4dupq16") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.16 {d0[], d2[], d4[], d6[]}, [r0]\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4DUPq16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4DUPq16", QBDI::POSTINST, checkAccess, &expectedPost);

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
  for (int i = 0; i < 8; i += 2) {
    CHECK(fpr->vreg.q[1][i] == 0x33);
    CHECK(fpr->vreg.q[1][i + 1] == 0x44);
  }
  for (int i = 0; i < 8; i += 2) {
    CHECK(fpr->vreg.q[2][i] == 0x55);
    CHECK(fpr->vreg.q[2][i + 1] == 0x66);
  }
  for (int i = 0; i < 8; i += 2) {
    CHECK(fpr->vreg.q[3][i] == 0x77);
    CHECK(fpr->vreg.q[3][i + 1] == 0x88);
  }
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4dupq16_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.16 {d0[], d2[], d4[], d6[]}, [r0]!\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4DUPq16_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4DUPq16_UPD", QBDI::POSTINST, checkAccess,
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
  CHECK(fpr->vreg.q[0][0] == 0x11);
  CHECK(fpr->vreg.q[3][0] == 0x77);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[8]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4lnd16") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.16 {d0[0], d1[0], d2[0], d3[0]}, [r0]\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4LNd16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4LNd16", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0xcc, sizeof(fpr->vreg));
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  fpr = vm.getFPRState();
  CHECK(fpr->vreg.q[0][0] == 0x11);
  CHECK(fpr->vreg.q[0][1] == 0x22);
  CHECK(fpr->vreg.q[0][8] == 0x33);
  CHECK(fpr->vreg.q[0][9] == 0x44);
  CHECK(fpr->vreg.q[1][0] == 0x55);
  CHECK(fpr->vreg.q[1][1] == 0x66);
  CHECK(fpr->vreg.q[1][8] == 0x77);
  CHECK(fpr->vreg.q[1][9] == 0x88);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4lnd16_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.16 {d0[0], d1[0], d2[0], d3[0]}, [r0]!\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4LNd16_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4LNd16_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  CHECK(fpr->vreg.q[0][0] == 0x11);
  CHECK(fpr->vreg.q[1][0] == 0x55);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[8]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4lnq16") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.16 {d1[1], d3[1], d5[1], d7[1]}, [r0]\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4LNq16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4LNq16", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0xcc, sizeof(fpr->vreg));
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  fpr = vm.getFPRState();
  CHECK(fpr->vreg.q[0][10] == 0x11);
  CHECK(fpr->vreg.q[0][11] == 0x22);
  CHECK(fpr->vreg.q[1][10] == 0x33);
  CHECK(fpr->vreg.q[1][11] == 0x44);
  CHECK(fpr->vreg.q[2][10] == 0x55);
  CHECK(fpr->vreg.q[2][11] == 0x66);
  CHECK(fpr->vreg.q[3][10] == 0x77);
  CHECK(fpr->vreg.q[3][11] == 0x88);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4lnq16_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.16 {d1[1], d3[1], d5[1], d7[1]}, [r0]!\n";

  uint8_t buf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4LNq16_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4LNq16_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  CHECK(fpr->vreg.q[0][10] == 0x11);
  CHECK(fpr->vreg.q[3][10] == 0x77);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[8]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst4lnd16") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst4.16 {d0[0], d1[0], d2[0], d3[0]}, [r0]\n";

  uint8_t buf[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST4LNd16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST4LNd16", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  *(uint16_t *)&fpr->vreg.q[0][0] = 0x2211;
  *(uint16_t *)&fpr->vreg.q[0][8] = 0x4433;
  *(uint16_t *)&fpr->vreg.q[1][0] = 0x6655;
  *(uint16_t *)&fpr->vreg.q[1][8] = 0x8877;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[0] == 0x44332211);
  CHECK(*(uint32_t *)&buf[4] == 0x88776655);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst4lnd16_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst4.16 {d0[0], d1[0], d2[0], d3[0]}, [r0]!\n";

  uint8_t buf[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST4LNd16_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST4LNd16_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  *(uint16_t *)&fpr->vreg.q[0][0] = 0x2211;
  *(uint16_t *)&fpr->vreg.q[0][8] = 0x4433;
  *(uint16_t *)&fpr->vreg.q[1][0] = 0x6655;
  *(uint16_t *)&fpr->vreg.q[1][8] = 0x8877;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[0] == 0x44332211);
  CHECK(*(uint32_t *)&buf[4] == 0x88776655);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[8]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst4lnq16") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst4.16 {d1[1], d3[1], d5[1], d7[1]}, [r0]\n";

  uint8_t buf[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST4LNq16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST4LNq16", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  *(uint16_t *)&fpr->vreg.q[0][10] = 0x2211;
  *(uint16_t *)&fpr->vreg.q[1][10] = 0x4433;
  *(uint16_t *)&fpr->vreg.q[2][10] = 0x6655;
  *(uint16_t *)&fpr->vreg.q[3][10] = 0x8877;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[0] == 0x44332211);
  CHECK(*(uint32_t *)&buf[4] == 0x88776655);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst4lnq16_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst4.16 {d1[1], d3[1], d5[1], d7[1]}, [r0]!\n";

  uint8_t buf[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST4LNq16_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST4LNq16_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  *(uint16_t *)&fpr->vreg.q[0][10] = 0x2211;
  *(uint16_t *)&fpr->vreg.q[1][10] = 0x4433;
  *(uint16_t *)&fpr->vreg.q[2][10] = 0x6655;
  *(uint16_t *)&fpr->vreg.q[3][10] = 0x8877;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[0] == 0x44332211);
  CHECK(*(uint32_t *)&buf[4] == 0x88776655);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[8]);
}
