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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4dupd32") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.32 {d0[], d1[], d2[], d3[]}, [r0]\n";

  uint32_t buf[4] = {0x11111111, 0x22222222, 0x33333333, 0x44444444};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x11111111, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[1], 0x22222222, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], 0x33333333, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[3], 0x44444444, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4DUPd32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4DUPd32", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4dupd32_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.32 {d0[], d1[], d2[], d3[]}, [r0]!\n";

  uint32_t buf[4] = {0x11111111, 0x22222222, 0x33333333, 0x44444444};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x11111111, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[1], 0x22222222, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], 0x33333333, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[3], 0x44444444, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4DUPd32_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4DUPd32_UPD", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("VLD4DUPd32_UPD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->r0 == (QBDI::rword)&buf[4]);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4dupq32") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.32 {d0[], d2[], d4[], d6[]}, [r0]\n";

  uint32_t buf[4] = {0x11111111, 0x22222222, 0x33333333, 0x44444444};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x11111111, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[1], 0x22222222, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], 0x33333333, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[3], 0x44444444, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4DUPq32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4DUPq32", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4dupq32_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.32 {d0[], d2[], d4[], d6[]}, [r0]!\n";

  uint32_t buf[4] = {0x11111111, 0x22222222, 0x33333333, 0x44444444};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x11111111, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[1], 0x22222222, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], 0x33333333, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[3], 0x44444444, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4DUPq32_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4DUPq32_UPD", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  vm.addMnemonicCB("VLD4DUPq32_UPD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->r0 == (QBDI::rword)&buf[4]);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4lnd32") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.32 {d0[0], d1[0], d2[0], d3[0]}, [r0]\n";

  uint32_t buf[4] = {0x11111111, 0x22222222, 0x33333333, 0x44444444};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x11111111, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[1], 0x22222222, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], 0x33333333, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[3], 0x44444444, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4LNd32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4LNd32", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4lnd32_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.32 {d0[0], d1[0], d2[0], d3[0]}, [r0]!\n";

  uint32_t buf[4] = {0x11111111, 0x22222222, 0x33333333, 0x44444444};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x11111111, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[1], 0x22222222, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], 0x33333333, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[3], 0x44444444, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4LNd32_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4LNd32_UPD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("VLD4LNd32_UPD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->r0 == (QBDI::rword)&buf[4]);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4lnq32") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.32 {d1[1], d3[1], d5[1], d7[1]}, [r0]\n";

  uint32_t buf[4] = {0x11111111, 0x22222222, 0x33333333, 0x44444444};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x11111111, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[1], 0x22222222, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], 0x33333333, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[3], 0x44444444, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4LNq32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4LNq32", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vld4lnq32_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vld4.32 {d1[1], d3[1], d5[1], d7[1]}, [r0]!\n";

  uint32_t buf[4] = {0x11111111, 0x22222222, 0x33333333, 0x44444444};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], 0x11111111, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[1], 0x22222222, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], 0x33333333, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[3], 0x44444444, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("VLD4LNq32_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLD4LNq32_UPD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("VLD4LNq32_UPD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->r0 == (QBDI::rword)&buf[4]);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst4lnd32") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst4.32 {d0[0], d1[0], d2[0], d3[0]}, [r0]\n";

  uint32_t buf[4] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xaaaaaaaa, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[1], 0xbbbbbbbb, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], 0xcccccccc, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[3], 0xdddddddd, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST4LNd32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST4LNd32", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  *(uint32_t *)&fpr->vreg.q[0][0] = 0xaaaaaaaa;
  *(uint32_t *)&fpr->vreg.q[0][8] = 0xbbbbbbbb;
  *(uint32_t *)&fpr->vreg.q[1][0] = 0xcccccccc;
  *(uint32_t *)&fpr->vreg.q[1][8] = 0xdddddddd;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == 0xaaaaaaaa);
  CHECK(buf[1] == 0xbbbbbbbb);
  CHECK(buf[2] == 0xcccccccc);
  CHECK(buf[3] == 0xdddddddd);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst4lnd32_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst4.32 {d0[0], d1[0], d2[0], d3[0]}, [r0]!\n";

  uint32_t buf[4] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xaaaaaaaa, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[1], 0xbbbbbbbb, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], 0xcccccccc, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[3], 0xdddddddd, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST4LNd32_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST4LNd32_UPD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("VST4LNd32_UPD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->r0 == (QBDI::rword)&buf[4]);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  *(uint32_t *)&fpr->vreg.q[0][0] = 0xaaaaaaaa;
  *(uint32_t *)&fpr->vreg.q[0][8] = 0xbbbbbbbb;
  *(uint32_t *)&fpr->vreg.q[1][0] = 0xcccccccc;
  *(uint32_t *)&fpr->vreg.q[1][8] = 0xdddddddd;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == 0xaaaaaaaa);
  CHECK(buf[1] == 0xbbbbbbbb);
  CHECK(buf[2] == 0xcccccccc);
  CHECK(buf[3] == 0xdddddddd);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst4lnq32") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst4.32 {d1[1], d3[1], d5[1], d7[1]}, [r0]\n";

  uint32_t buf[4] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xaaaaaaaa, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[1], 0xbbbbbbbb, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], 0xcccccccc, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[3], 0xdddddddd, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST4LNq32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST4LNq32", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  *(uint32_t *)&fpr->vreg.q[0][12] = 0xaaaaaaaa;
  *(uint32_t *)&fpr->vreg.q[1][12] = 0xbbbbbbbb;
  *(uint32_t *)&fpr->vreg.q[2][12] = 0xcccccccc;
  *(uint32_t *)&fpr->vreg.q[3][12] = 0xdddddddd;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == 0xaaaaaaaa);
  CHECK(buf[1] == 0xbbbbbbbb);
  CHECK(buf[2] == 0xcccccccc);
  CHECK(buf[3] == 0xdddddddd);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vst4lnq32_upd") {
  if (!checkFeature("neon")) {
    return;
  }
  const char source[] = "vst4.32 {d1[1], d3[1], d5[1], d7[1]}, [r0]!\n";

  uint32_t buf[4] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0xaaaaaaaa, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[1], 0xbbbbbbbb, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], 0xcccccccc, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[3], 0xdddddddd, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("VST4LNq32_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VST4LNq32_UPD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("VST4LNq32_UPD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->r0 == (QBDI::rword)&buf[4]);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  memset(&fpr->vreg, 0, sizeof(fpr->vreg));
  *(uint32_t *)&fpr->vreg.q[0][12] = 0xaaaaaaaa;
  *(uint32_t *)&fpr->vreg.q[1][12] = 0xbbbbbbbb;
  *(uint32_t *)&fpr->vreg.q[2][12] = 0xcccccccc;
  *(uint32_t *)&fpr->vreg.q[3][12] = 0xdddddddd;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"neon"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == 0xaaaaaaaa);
  CHECK(buf[1] == 0xbbbbbbbb);
  CHECK(buf[2] == 0xcccccccc);
  CHECK(buf[3] == 0xdddddddd);
}
