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

static void checkT2LdabCond(APITest &fixture, bool taken) {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const auto &cond = conditionForIndex(0);
  std::string source =
      std::string("it ") + cond.suffix + "\nldab" + cond.suffix + " r1, [r0]\n";

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
  fixture.vm.addMnemonicCB("t2LDAB", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("t2LDAB", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r1 = 0xdead0001;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb,
                              {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? (QBDI::rword)expected : 0xdead0001u));
}

static void checkT2StlbCond(APITest &fixture, bool taken) {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const auto &cond = conditionForIndex(1);
  std::string source =
      std::string("it ") + cond.suffix + "\nstlb" + cond.suffix + " r1, [r0]\n";

  uint8_t v = 0x10;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {
        {(QBDI::rword)&v, 0xab, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    };
  }
  fixture.vm.addMnemonicCB("t2STLB", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("t2STLB", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r1 = 0xab;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb,
                              {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == (taken ? 0xabu : 0x10u));
}

static void checkT2LdahCond(APITest &fixture, bool taken) {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const auto &cond = conditionForIndex(2);
  std::string source =
      std::string("it ") + cond.suffix + "\nldah" + cond.suffix + " r1, [r0]\n";

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
  fixture.vm.addMnemonicCB("t2LDAH", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("t2LDAH", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r1 = 0xdead0001;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb,
                              {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? (QBDI::rword)expected : 0xdead0001u));
}

static void checkT2StlhCond(APITest &fixture, bool taken) {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const auto &cond = conditionForIndex(3);
  std::string source =
      std::string("it ") + cond.suffix + "\nstlh" + cond.suffix + " r1, [r0]\n";

  uint16_t v = 0x1010;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {
        {(QBDI::rword)&v, 0xabcd, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    };
  }
  fixture.vm.addMnemonicCB("t2STLH", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("t2STLH", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r1 = 0xabcd;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb,
                              {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == (taken ? 0xabcdu : 0x1010u));
}

static void checkT2LdaCond(APITest &fixture, bool taken) {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const auto &cond = conditionForIndex(4);
  std::string source =
      std::string("it ") + cond.suffix + "\nlda" + cond.suffix + " r1, [r0]\n";

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
  fixture.vm.addMnemonicCB("t2LDA", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("t2LDA", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r1 = 0xdead0001;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb,
                              {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? expected : 0xdead0001u));
}

static void checkT2StlCond(APITest &fixture, bool taken) {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const auto &cond = conditionForIndex(5);
  std::string source =
      std::string("it ") + cond.suffix + "\nstl" + cond.suffix + " r1, [r0]\n";

  uint32_t v = 0x11223344;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {
        {(QBDI::rword)&v, 0xaabbccdd, 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
    };
  }
  fixture.vm.addMnemonicCB("t2STL", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("t2STL", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r1 = 0xaabbccdd;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb,
                              {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == (taken ? 0xaabbccddu : 0x11223344u));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldab_cond_taken") {
  checkT2LdabCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldab_cond_not_taken") {
  checkT2LdabCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2stlb_cond_taken") {
  checkT2StlbCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2stlb_cond_not_taken") {
  checkT2StlbCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldah_cond_taken") {
  checkT2LdahCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldah_cond_not_taken") {
  checkT2LdahCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2stlh_cond_taken") {
  checkT2StlhCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2stlh_cond_not_taken") {
  checkT2StlhCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2lda_cond_taken") {
  checkT2LdaCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2lda_cond_not_taken") {
  checkT2LdaCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2stl_cond_taken") {
  checkT2StlCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2stl_cond_not_taken") {
  checkT2StlCond(*this, false);
}
