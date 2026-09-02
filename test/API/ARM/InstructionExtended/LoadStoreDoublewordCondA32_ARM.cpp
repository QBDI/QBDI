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
#include <algorithm>
#include <string>

#include "ConditionTestUtils_ARM.h"
#include "MemAccessTestUtils_ARM.h"

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::conditionForIndex;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;
using QBDITestBatch2::setConditionCPSR;

static void checkLdrdCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(0);
  std::string source =
      std::string("ldrd") + cond.suffix + " r2, r3, [r0, #8]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {
        {(QBDI::rword)&buf[8], 0x44332211, 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
        {(QBDI::rword)&buf[12], 0x88776655, 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
    };
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDRD", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("LDRD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 0xdead0002;
  state->r3 = 0xdead0003;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r2 == (taken ? 0x44332211u : 0xdead0002u));
  CHECK(finalState->r3 == (taken ? 0x88776655u : 0xdead0003u));
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
}

static void checkLdrdPostCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(1);
  std::string source =
      std::string("ldrd") + cond.suffix + " r2, r3, [r0], #8\n";

  uint8_t buf[16] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {
        {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
        {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
    };
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDRD_POST", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("LDRD_POST", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 0xdead0002;
  state->r3 = 0xdead0003;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r2 == (taken ? 0x44332211u : 0xdead0002u));
  CHECK(finalState->r3 == (taken ? 0x88776655u : 0xdead0003u));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[0]));
}

static void checkLdrdPreCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(2);
  std::string source =
      std::string("ldrd") + cond.suffix + " r2, r3, [r0, #8]!\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {
        {(QBDI::rword)&buf[8], 0x44332211, 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
        {(QBDI::rword)&buf[12], 0x88776655, 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
    };
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDRD_PRE", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("LDRD_PRE", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 0xdead0002;
  state->r3 = 0xdead0003;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r2 == (taken ? 0x44332211u : 0xdead0002u));
  CHECK(finalState->r3 == (taken ? 0x88776655u : 0xdead0003u));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[0]));
}

static void checkStrdCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(3);
  std::string source =
      std::string("strd") + cond.suffix + " r2, r3, [r0, #8]\n";

  uint8_t buf[16] = {0};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {
        {(QBDI::rword)&buf[8], 0x44332211, 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
        {(QBDI::rword)&buf[12], 0x88776655, 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
    };
  }
  fixture.vm.addMnemonicCB("STRD", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("STRD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 0x44332211;
  state->r3 = 0x88776655;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[8] == (taken ? 0x44332211u : 0x00000000u));
  CHECK(*(uint32_t *)&buf[12] == (taken ? 0x88776655u : 0x00000000u));
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
}

static void checkStrdPostCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(4);
  std::string source =
      std::string("strd") + cond.suffix + " r2, r3, [r0], #8\n";

  uint8_t buf[16] = {0};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {
        {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
        {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
    };
  }
  fixture.vm.addMnemonicCB("STRD_POST", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("STRD_POST", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 0x44332211;
  state->r3 = 0x88776655;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[0] == (taken ? 0x44332211u : 0x00000000u));
  CHECK(*(uint32_t *)&buf[4] == (taken ? 0x88776655u : 0x00000000u));
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[0]));
}

static void checkStrdPreCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(5);
  std::string source =
      std::string("strd") + cond.suffix + " r2, r3, [r0, #8]!\n";

  uint8_t buf[16] = {0};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {
        {(QBDI::rword)&buf[8], 0x44332211, 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
        {(QBDI::rword)&buf[12], 0x88776655, 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
    };
  }
  fixture.vm.addMnemonicCB("STRD_PRE", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("STRD_PRE", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 0x44332211;
  state->r3 = 0x88776655;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[8] == (taken ? 0x44332211u : 0x00000000u));
  CHECK(*(uint32_t *)&buf[12] == (taken ? 0x88776655u : 0x00000000u));
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[0]));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrd_cond_taken") {
  checkLdrdCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrd_cond_not_taken") {
  checkLdrdCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrd_post_cond_taken") {
  checkLdrdPostCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrd_post_cond_not_taken") {
  checkLdrdPostCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrd_pre_cond_taken") {
  checkLdrdPreCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrd_pre_cond_not_taken") {
  checkLdrdPreCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strd_cond_taken") {
  checkStrdCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strd_cond_not_taken") {
  checkStrdCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strd_post_cond_taken") {
  checkStrdPostCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-strd_post_cond_not_taken") {
  checkStrdPostCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strd_pre_cond_taken") {
  checkStrdPreCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-strd_pre_cond_not_taken") {
  checkStrdPreCond(*this, false);
}
