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

static void checkLdaexCondThumb(APITest &fixture, bool taken) {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const auto &cond = conditionForIndex(8);
  std::string source = std::string("it ") + cond.suffix + "\nldaex" +
                       cond.suffix + " r1, [r0]\n";

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
  fixture.vm.addMnemonicCB("t2LDAEX", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("t2LDAEX", QBDI::POSTINST, checkAccess,
                           &expectedPost);
  fixture.vm.addMnemonicCB(
      "t2LDAEX", QBDI::POSTINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        if (taken) {
          CHECK(gprState->r1 == expected);
          CHECK(gprState->localMonitor.enable == 4);
          CHECK(gprState->localMonitor.addr == (QBDI::rword)&v);
        } else {
          CHECK(gprState->r1 == 0x11111111);
          CHECK(gprState->localMonitor.enable == 0);
        }
        return QBDI::VMAction::CONTINUE;
      });

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r1 = taken ? 0 : 0x11111111;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb,
                              {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

static void checkLdaexbCondThumb(APITest &fixture, bool taken) {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const auto &cond = conditionForIndex(9);
  std::string source = std::string("it ") + cond.suffix + "\nldaexb" +
                       cond.suffix + " r1, [r0]\n";

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
  fixture.vm.addMnemonicCB("t2LDAEXB", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2LDAEXB", QBDI::POSTINST, checkAccess,
                           &expectedPost);
  fixture.vm.addMnemonicCB(
      "t2LDAEXB", QBDI::POSTINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        if (taken) {
          CHECK((gprState->r1 & 0xff) == expected);
          CHECK(gprState->localMonitor.enable == 1);
          CHECK(gprState->localMonitor.addr == (QBDI::rword)&v);
        } else {
          CHECK((gprState->r1 & 0xff) == 0x11);
          CHECK(gprState->localMonitor.enable == 0);
        }
        return QBDI::VMAction::CONTINUE;
      });

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r1 = taken ? 0 : 0x11;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb,
                              {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

static void checkLdaexhCondThumb(APITest &fixture, bool taken) {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const auto &cond = conditionForIndex(10);
  std::string source = std::string("it ") + cond.suffix + "\nldaexh" +
                       cond.suffix + " r1, [r0]\n";

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
  fixture.vm.addMnemonicCB("t2LDAEXH", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2LDAEXH", QBDI::POSTINST, checkAccess,
                           &expectedPost);
  fixture.vm.addMnemonicCB(
      "t2LDAEXH", QBDI::POSTINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        if (taken) {
          CHECK((gprState->r1 & 0xffff) == expected);
          CHECK(gprState->localMonitor.enable == 2);
          CHECK(gprState->localMonitor.addr == (QBDI::rword)&v);
        } else {
          CHECK((gprState->r1 & 0xffff) == 0x1111);
          CHECK(gprState->localMonitor.enable == 0);
        }
        return QBDI::VMAction::CONTINUE;
      });

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r1 = taken ? 0 : 0x1111;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb,
                              {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

static void checkLdaexdCondThumb(APITest &fixture, bool taken) {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const auto &cond = conditionForIndex(11);
  std::string source = std::string("it ") + cond.suffix + "\nldaexd" +
                       cond.suffix + " r2, r3, [r0]\n";

  constexpr uint64_t expected = 0x1122334455667788ULL;
  uint64_t v = expected;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {
        {(QBDI::rword)&v, (uint32_t)expected, 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
        {(QBDI::rword)&v + 4, (uint32_t)(expected >> 32), 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
    };
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("t2LDAEXD", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2LDAEXD", QBDI::POSTINST, checkAccess,
                           &expectedPost);
  fixture.vm.addMnemonicCB(
      "t2LDAEXD", QBDI::POSTINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        if (taken) {
          CHECK(gprState->r2 == (uint32_t)expected);
          CHECK(gprState->r3 == (uint32_t)(expected >> 32));
          CHECK(gprState->localMonitor.enable == 8);
          CHECK(gprState->localMonitor.addr == (QBDI::rword)&v);
        } else {
          CHECK(gprState->r2 == 0x11111111);
          CHECK(gprState->r3 == 0x22222222);
          CHECK(gprState->localMonitor.enable == 0);
        }
        return QBDI::VMAction::CONTINUE;
      });

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r2 = taken ? 0 : 0x11111111;
  state->r3 = taken ? 0 : 0x22222222;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb,
                              {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

static void checkStlexCondThumb(APITest &fixture, bool taken) {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const auto &cond = conditionForIndex(12);
  std::string source = std::string("ldaex r2, [r0]\nit ") + cond.suffix +
                       "\nstlex" + cond.suffix + " r3, r1, [r0]\n";

  uint32_t v = 0x10101010;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {
        {(QBDI::rword)&v, 0xcafebabe, 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
    };
  }
  fixture.vm.addMnemonicCB("t2STLEX", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("t2STLEX", QBDI::POSTINST, checkAccess,
                           &expectedPost);
  fixture.vm.addMnemonicCB(
      "t2STLEX", QBDI::POSTINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        if (taken) {
          CHECK(gprState->r3 == 0);
          CHECK(gprState->localMonitor.enable == 0);
        } else {
          CHECK(gprState->r3 == 0x99999999);
          CHECK(gprState->localMonitor.enable == 4);
          CHECK(gprState->localMonitor.addr == (QBDI::rword)&v);
        }
        return QBDI::VMAction::CONTINUE;
      });

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r1 = 0xcafebabe;
  state->r3 = taken ? 0 : 0x99999999;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb,
                              {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == (taken ? 0xcafebabeu : 0x10101010u));
}

static void checkStlexbCondThumb(APITest &fixture, bool taken) {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const auto &cond = conditionForIndex(13);
  std::string source = std::string("ldaexb r2, [r0]\nit ") + cond.suffix +
                       "\nstlexb" + cond.suffix + " r3, r1, [r0]\n";

  uint8_t v = 0x10;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {
        {(QBDI::rword)&v, 0xca, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    };
  }
  fixture.vm.addMnemonicCB("t2STLEXB", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2STLEXB", QBDI::POSTINST, checkAccess,
                           &expectedPost);
  fixture.vm.addMnemonicCB(
      "t2STLEXB", QBDI::POSTINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        if (taken) {
          CHECK(gprState->r3 == 0);
          CHECK(gprState->localMonitor.enable == 0);
        } else {
          CHECK(gprState->r3 == 0x99);
          CHECK(gprState->localMonitor.enable == 1);
          CHECK(gprState->localMonitor.addr == (QBDI::rword)&v);
        }
        return QBDI::VMAction::CONTINUE;
      });

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r1 = 0xca;
  state->r3 = taken ? 0 : 0x99;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb,
                              {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == (taken ? 0xcau : 0x10u));
}

static void checkStlexhCondThumb(APITest &fixture, bool taken) {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const auto &cond = conditionForIndex(14);
  std::string source = std::string("ldaexh r2, [r0]\nit ") + cond.suffix +
                       "\nstlexh" + cond.suffix + " r3, r1, [r0]\n";

  uint16_t v = 0x1010;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {
        {(QBDI::rword)&v, 0xcafe, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    };
  }
  fixture.vm.addMnemonicCB("t2STLEXH", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2STLEXH", QBDI::POSTINST, checkAccess,
                           &expectedPost);
  fixture.vm.addMnemonicCB(
      "t2STLEXH", QBDI::POSTINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        if (taken) {
          CHECK(gprState->r3 == 0);
          CHECK(gprState->localMonitor.enable == 0);
        } else {
          CHECK(gprState->r3 == 0x9999);
          CHECK(gprState->localMonitor.enable == 2);
          CHECK(gprState->localMonitor.addr == (QBDI::rword)&v);
        }
        return QBDI::VMAction::CONTINUE;
      });

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r1 = 0xcafe;
  state->r3 = taken ? 0 : 0x9999;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb,
                              {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == (taken ? 0xcafeu : 0x1010u));
}

static void checkStlexdCondThumb(APITest &fixture, bool taken) {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const auto &cond = conditionForIndex(15);
  std::string source = std::string("ldaexd r2, r3, [r0]\nit ") + cond.suffix +
                       "\nstlexd" + cond.suffix + " r5, r6, r7, [r0]\n";

  uint64_t v = 0x1010101010101010ULL;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {
        {(QBDI::rword)&v, 0xcafebabe, 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
        {(QBDI::rword)&v + 4, 0xdeadbeef, 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
    };
  }
  fixture.vm.addMnemonicCB("t2STLEXD", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2STLEXD", QBDI::POSTINST, checkAccess,
                           &expectedPost);
  fixture.vm.addMnemonicCB(
      "t2STLEXD", QBDI::POSTINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        if (taken) {
          CHECK(gprState->r5 == 0);
          CHECK(gprState->localMonitor.enable == 0);
        } else {
          CHECK(gprState->r5 == 0x99999999);
          CHECK(gprState->localMonitor.enable == 8);
          CHECK(gprState->localMonitor.addr == (QBDI::rword)&v);
        }
        return QBDI::VMAction::CONTINUE;
      });

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r5 = taken ? 0 : 0x99999999;
  state->r6 = 0xcafebabe;
  state->r7 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb,
                              {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == (taken ? 0xdeadbeefcafebabeULL : 0x1010101010101010ULL));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-ldaex_cond_taken") {
  checkLdaexCondThumb(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-ldaex_cond_not_taken") {
  checkLdaexCondThumb(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-ldaexb_cond_taken") {
  checkLdaexbCondThumb(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-ldaexb_cond_not_taken") {
  checkLdaexbCondThumb(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-ldaexh_cond_taken") {
  checkLdaexhCondThumb(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-ldaexh_cond_not_taken") {
  checkLdaexhCondThumb(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-ldaexd_cond_taken") {
  checkLdaexdCondThumb(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-ldaexd_cond_not_taken") {
  checkLdaexdCondThumb(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-stlex_cond_taken") {
  checkStlexCondThumb(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-stlex_cond_not_taken") {
  checkStlexCondThumb(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-stlexb_cond_taken") {
  checkStlexbCondThumb(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-stlexb_cond_not_taken") {
  checkStlexbCondThumb(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-stlexh_cond_taken") {
  checkStlexhCondThumb(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-stlexh_cond_not_taken") {
  checkStlexhCondThumb(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-stlexd_cond_taken") {
  checkStlexdCondThumb(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-stlexd_cond_not_taken") {
  checkStlexdCondThumb(*this, false);
}
