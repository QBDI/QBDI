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
#include <string>

#include "ConditionTestUtils_ARM.h"
#include "MemAccessTestUtils_ARM.h"

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::checkFeature;
using QBDITestBatch2::conditionForIndex;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;
using QBDITestBatch2::setConditionCPSR;

static void checkLdabCond(APITest &fixture, bool taken) {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const auto &cond = conditionForIndex(0);
  std::string source = std::string("ldab") + cond.suffix + " r1, [r0]\n";

  constexpr uint8_t expected = 0x42;
  uint8_t v = expected;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {
        {(QBDI::rword)&v, expected, 1, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
    };
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDAB", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("LDAB", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r1 = 0xdead0001;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::ARM,
                              {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? (QBDI::rword)expected : 0xdead0001u));
}

static void checkStlbCond(APITest &fixture, bool taken) {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const auto &cond = conditionForIndex(1);
  std::string source = std::string("stlb") + cond.suffix + " r1, [r0]\n";

  uint8_t v = 0x10;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {
        {(QBDI::rword)&v, 0xab, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    };
  }
  fixture.vm.addMnemonicCB("STLB", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("STLB", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r1 = 0xab;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::ARM,
                              {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == (taken ? 0xabu : 0x10u));
}

static void checkLdahCond(APITest &fixture, bool taken) {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const auto &cond = conditionForIndex(2);
  std::string source = std::string("ldah") + cond.suffix + " r1, [r0]\n";

  constexpr uint16_t expected = 0x1234;
  uint16_t v = expected;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {
        {(QBDI::rword)&v, expected, 2, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
    };
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDAH", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("LDAH", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r1 = 0xdead0001;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::ARM,
                              {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? (QBDI::rword)expected : 0xdead0001u));
}

static void checkStlhCond(APITest &fixture, bool taken) {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const auto &cond = conditionForIndex(3);
  std::string source = std::string("stlh") + cond.suffix + " r1, [r0]\n";

  uint16_t v = 0x1010;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {
        {(QBDI::rword)&v, 0xabcd, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    };
  }
  fixture.vm.addMnemonicCB("STLH", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("STLH", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r1 = 0xabcd;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::ARM,
                              {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == (taken ? 0xabcdu : 0x1010u));
}

static void checkLdaCond(APITest &fixture, bool taken) {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const auto &cond = conditionForIndex(4);
  std::string source = std::string("lda") + cond.suffix + " r1, [r0]\n";

  constexpr uint32_t expected = 0x12345678;
  uint32_t v = expected;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {
        {(QBDI::rword)&v, expected, 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
    };
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDA", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("LDA", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r1 = 0xdead0001;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::ARM,
                              {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? expected : 0xdead0001u));
}

static void checkStlCond(APITest &fixture, bool taken) {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const auto &cond = conditionForIndex(5);
  std::string source = std::string("stl") + cond.suffix + " r1, [r0]\n";

  uint32_t v = 0x11223344;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {
        {(QBDI::rword)&v, 0xaabbccdd, 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
    };
  }
  fixture.vm.addMnemonicCB("STL", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("STL", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r1 = 0xaabbccdd;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::ARM,
                              {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == (taken ? 0xaabbccddu : 0x11223344u));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldab_cond_taken") {
  checkLdabCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldab_cond_not_taken") {
  checkLdabCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-stlb_cond_taken") {
  checkStlbCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-stlb_cond_not_taken") {
  checkStlbCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldah_cond_taken") {
  checkLdahCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldah_cond_not_taken") {
  checkLdahCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-stlh_cond_taken") {
  checkStlhCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-stlh_cond_not_taken") {
  checkStlhCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-lda_cond_taken") {
  checkLdaCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-lda_cond_not_taken") {
  checkLdaCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-stl_cond_taken") {
  checkStlCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-stl_cond_not_taken") {
  checkStlCond(*this, false);
}
