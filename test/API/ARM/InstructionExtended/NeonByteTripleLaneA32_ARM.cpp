#include <cstring>
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
#include "MemAccessTestUtils_ARM.h"

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::checkFeature;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld3dupd8") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld3.8 {d0[], d1[], d2[]}, [r0]\n";

  uint8_t buf[3] = {0x11, 0x22, 0x33};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x332211, 3, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD3DUPd8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD3DUPd8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld3dupd8_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld3.8 {d0[], d1[], d2[]}, [r0]!\n";

  uint8_t buf[3] = {0x11, 0x22, 0x33};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x332211, 3, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD3DUPd8_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD3DUPd8_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[3]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld3dupq8") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld3.8 {d0[], d2[], d4[]}, [r0]\n";

  uint8_t buf[3] = {0x11, 0x22, 0x33};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x332211, 3, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD3DUPq8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD3DUPq8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld3dupq8_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld3.8 {d0[], d2[], d4[]}, [r0]!\n";

  uint8_t buf[3] = {0x11, 0x22, 0x33};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x332211, 3, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD3DUPq8_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD3DUPq8_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[3]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld3lnd8") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld3.8 {d0[0], d1[0], d2[0]}, [r0]\n";

  uint8_t buf[3] = {0x11, 0x22, 0x33};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x332211, 3, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD3LNd8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD3LNd8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld3lnd8_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld3.8 {d0[0], d1[0], d2[0]}, [r0]!\n";

  uint8_t buf[3] = {0x11, 0x22, 0x33};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x332211, 3, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD3LNd8_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD3LNd8_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[3]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst3lnd8") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst3.8 {d0[0], d1[0], d2[0]}, [r0]\n";

  uint8_t buf[3] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x332211, 3, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST3LNd8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST3LNd8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  fpr->vreg.q[0][0] = 0x11;
  fpr->vreg.q[0][8] = 0x22;
  fpr->vreg.q[1][0] = 0x33;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == 0x11);
  CHECK(buf[1] == 0x22);
  CHECK(buf[2] == 0x33);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst3lnd8_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst3.8 {d0[0], d1[0], d2[0]}, [r0]!\n";

  uint8_t buf[3] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x332211, 3, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST3LNd8_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST3LNd8_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  fpr->vreg.q[0][0] = 0x11;
  fpr->vreg.q[0][8] = 0x22;
  fpr->vreg.q[1][0] = 0x33;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == 0x11);
  CHECK(buf[1] == 0x22);
  CHECK(buf[2] == 0x33);

  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[3]);
}
