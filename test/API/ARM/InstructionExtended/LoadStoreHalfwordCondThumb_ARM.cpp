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

static void checkT2Ldrhi12Cond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(0);
  std::string source = std::string("it ") + cond.suffix + "\nldrh" +
                       cond.suffix + ".w r1, [r0, #4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8988, 2, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("t2LDRHi12", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2LDRHi12", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r1 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? 0x8988u : 0xdeadbeef));
}

static void checkT2Ldrhi8Cond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(1);
  std::string source = std::string("it ") + cond.suffix + "\nldrh" +
                       cond.suffix + ".w r1, [r0, #-4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x8584, 2, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("t2LDRHi8", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2LDRHi8", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[8];
  state->r1 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? 0x8584u : 0xdeadbeef));
}

static void checkT2LdrhpciCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(2);
  std::string source = std::string(
                           "b 1f\n"
                           "label:\n"
                           ".byte 0x34, 0x12\n"
                           "1:\n"
                           "it ") +
                       cond.suffix + "\nldrh" + cond.suffix + ".w r1, label\n";

  bool seenPre = false, seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("t2LDRHpci", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             auto accesses = vmi->getInstMemoryAccess();
                             if (taken) {
                               REQUIRE(accesses.size() == 1);
                               CHECK(accesses[0].value == 0x1234);
                               CHECK(accesses[0].size == 2);
                               CHECK(accesses[0].type == QBDI::MEMORY_READ);
                             } else {
                               CHECK(accesses.empty());
                             }
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("t2LDRHpci", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             auto accesses = vmi->getInstMemoryAccess();
                             if (taken) {
                               REQUIRE(accesses.size() == 1);
                               CHECK(accesses[0].value == 0x1234);
                               CHECK(accesses[0].size == 2);
                               CHECK(accesses[0].type == QBDI::MEMORY_READ);
                               CHECK(gprState->r1 == 0x1234);
                             } else {
                               CHECK(accesses.empty());
                               CHECK(gprState->r1 == 0xdeadbeef);
                             }
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r1 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? 0x1234u : 0xdeadbeef));
}

static void checkT2LdrhPostCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(3);
  std::string source = std::string("it ") + cond.suffix + "\nldrh" +
                       cond.suffix + ".w r1, [r0], #4\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x8584, 2, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("t2LDRH_POST", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2LDRH_POST", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r1 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? 0x8584u : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkT2LdrhPreCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(4);
  std::string source = std::string("it ") + cond.suffix + "\nldrh" +
                       cond.suffix + ".w r1, [r0, #4]!\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8988, 2, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("t2LDRH_PRE", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2LDRH_PRE", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r1 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? 0x8988u : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkT2LdrhsCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(5);
  std::string source = std::string("it ") + cond.suffix + "\nldrh" +
                       cond.suffix + ".w r1, [r0, r2]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8988, 2, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("t2LDRHs", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("t2LDRHs", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r2 = 4;
  state->r1 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? 0x8988u : 0xdeadbeef));
}

static void checkT2LdrhtCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(6);
  std::string source = std::string("it ") + cond.suffix + "\nldrht" +
                       cond.suffix + " r1, [r0, #4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8988, 2, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("t2LDRHT", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("t2LDRHT", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r1 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? 0x8988u : 0xdeadbeef));
}

static void checkT2Ldrshi12Cond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(7);
  std::string source = std::string("it ") + cond.suffix + "\nldrsh" +
                       cond.suffix + ".w r1, [r0, #4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8988, 2, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("t2LDRSHi12", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2LDRSHi12", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r1 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? 0xffff8988u : 0xdeadbeef));
}

static void checkT2Ldrshi8Cond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(8);
  std::string source = std::string("it ") + cond.suffix + "\nldrsh" +
                       cond.suffix + ".w r1, [r0, #-4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x8584, 2, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("t2LDRSHi8", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2LDRSHi8", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[8];
  state->r1 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? 0xffff8584u : 0xdeadbeef));
}

static void checkT2LdrshpciCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(9);
  std::string source = std::string(
                           "b 1f\n"
                           "label:\n"
                           ".byte 0x34, 0x12\n"
                           "1:\n"
                           "it ") +
                       cond.suffix + "\nldrsh" + cond.suffix + ".w r1, label\n";

  bool seenPre = false, seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("t2LDRSHpci", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             auto accesses = vmi->getInstMemoryAccess();
                             if (taken) {
                               REQUIRE(accesses.size() == 1);
                               CHECK(accesses[0].value == 0x1234);
                               CHECK(accesses[0].size == 2);
                               CHECK(accesses[0].type == QBDI::MEMORY_READ);
                             } else {
                               CHECK(accesses.empty());
                             }
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("t2LDRSHpci", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             auto accesses = vmi->getInstMemoryAccess();
                             if (taken) {
                               REQUIRE(accesses.size() == 1);
                               CHECK(accesses[0].value == 0x1234);
                               CHECK(accesses[0].size == 2);
                               CHECK(accesses[0].type == QBDI::MEMORY_READ);
                               CHECK(gprState->r1 == 0x1234);
                             } else {
                               CHECK(accesses.empty());
                               CHECK(gprState->r1 == 0xdeadbeef);
                             }
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r1 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? 0x1234u : 0xdeadbeef));
}

static void checkT2LdrshPostCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(10);
  std::string source = std::string("it ") + cond.suffix + "\nldrsh" +
                       cond.suffix + ".w r1, [r0], #4\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x8584, 2, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("t2LDRSH_POST", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2LDRSH_POST", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r1 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? 0xffff8584u : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkT2LdrshPreCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(11);
  std::string source = std::string("it ") + cond.suffix + "\nldrsh" +
                       cond.suffix + ".w r1, [r0, #4]!\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8988, 2, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("t2LDRSH_PRE", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2LDRSH_PRE", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r1 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? 0xffff8988u : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkT2LdrshsCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(12);
  std::string source = std::string("it ") + cond.suffix + "\nldrsh" +
                       cond.suffix + ".w r1, [r0, r2]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8988, 2, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("t2LDRSHs", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2LDRSHs", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r2 = 4;
  state->r1 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? 0xffff8988u : 0xdeadbeef));
}

static void checkT2LdrshtCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(13);
  std::string source = std::string("it ") + cond.suffix + "\nldrsht" +
                       cond.suffix + " r1, [r0, #4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8988, 2, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("t2LDRSHT", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2LDRSHT", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r1 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? 0xffff8988u : 0xdeadbeef));
}

static void checkT2Strhi12Cond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(14);
  std::string source = std::string("it ") + cond.suffix + "\nstrh" +
                       cond.suffix + ".w r1, [r0, #4]\n";

  uint8_t buf[16] = {0};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xabcd, 2,
                              QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("t2STRHi12", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2STRHi12", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xabcd;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == (taken ? 0xcd : 0x00));
  CHECK(buf[5] == (taken ? 0xab : 0x00));
}

static void checkT2Strhi8Cond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(15);
  std::string source = std::string("it ") + cond.suffix + "\nstrh" +
                       cond.suffix + ".w r1, [r0, #-4]\n";

  uint8_t buf[16] = {0};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xabcd, 2,
                              QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("t2STRHi8", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2STRHi8", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[8];
  state->r1 = 0xabcd;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == (taken ? 0xcd : 0x00));
  CHECK(buf[5] == (taken ? 0xab : 0x00));
}

static void checkT2StrhPostCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(16);
  std::string source = std::string("it ") + cond.suffix + "\nstrh" +
                       cond.suffix + ".w r1, [r0], #4\n";

  uint8_t buf[16] = {0};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[0], 0xabcd, 2,
                              QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("t2STRH_POST", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2STRH_POST", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xabcd;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == (taken ? 0xcd : 0x00));
  CHECK(buf[1] == (taken ? 0xab : 0x00));
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[4] : (QBDI::rword)&buf[0]));
}

static void checkT2StrhPreCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(17);
  std::string source = std::string("it ") + cond.suffix + "\nstrh" +
                       cond.suffix + ".w r1, [r0, #4]!\n";

  uint8_t buf[16] = {0};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xabcd, 2,
                              QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("t2STRH_PRE", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2STRH_PRE", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xabcd;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == (taken ? 0xcd : 0x00));
  CHECK(buf[5] == (taken ? 0xab : 0x00));
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[4] : (QBDI::rword)&buf[0]));
}

static void checkT2StrhtCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(18);
  std::string source = std::string("it ") + cond.suffix + "\nstrht" +
                       cond.suffix + " r1, [r0, #4]\n";

  uint8_t buf[16] = {0};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xabcd, 2,
                              QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("t2STRHT", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("t2STRHT", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xabcd;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == (taken ? 0xcd : 0x00));
  CHECK(buf[5] == (taken ? 0xab : 0x00));
}

static void checkT2StrhsCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(19);
  std::string source = std::string("it ") + cond.suffix + "\nstrh" +
                       cond.suffix + ".w r1, [r0, r2]\n";

  uint8_t buf[16] = {0};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xabcd, 2,
                              QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("t2STRHs", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("t2STRHs", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xabcd;
  state->r2 = 4;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == (taken ? 0xcd : 0x00));
  CHECK(buf[5] == (taken ? 0xab : 0x00));
}

static void checkTLdrhiCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(20);
  std::string source = std::string("it ") + cond.suffix + "\nldrh" +
                       cond.suffix + " r1, [r0, #4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8988, 2, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("tLDRHi", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tLDRHi", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r1 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? 0x8988u : 0xdeadbeef));
}

static void checkTLdrhrCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(21);
  std::string source = std::string("it ") + cond.suffix + "\nldrh" +
                       cond.suffix + " r1, [r0, r2]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8988, 2, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("tLDRHr", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tLDRHr", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r2 = 4;
  state->r1 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? 0x8988u : 0xdeadbeef));
}

static void checkTLdrshCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(22);
  std::string source = std::string("it ") + cond.suffix + "\nldrsh" +
                       cond.suffix + " r1, [r0, r2]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8988, 2, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("tLDRSH", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tLDRSH", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r2 = 4;
  state->r1 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? 0xffff8988u : 0xdeadbeef));
}

static void checkTStrhiCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(23);
  std::string source = std::string("it ") + cond.suffix + "\nstrh" +
                       cond.suffix + " r1, [r0, #4]\n";

  uint8_t buf[16] = {0};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xabcd, 2,
                              QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("tSTRHi", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tSTRHi", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xabcd;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == (taken ? 0xcd : 0x00));
  CHECK(buf[5] == (taken ? 0xab : 0x00));
}

static void checkTStrhrCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(24);
  std::string source = std::string("it ") + cond.suffix + "\nstrh" +
                       cond.suffix + " r1, [r0, r2]\n";

  uint8_t buf[16] = {0};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xabcd, 2,
                              QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("tSTRHr", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tSTRHr", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xabcd;
  state->r2 = 4;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == (taken ? 0xcd : 0x00));
  CHECK(buf[5] == (taken ? 0xab : 0x00));
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrhi12_cond_taken") {
  checkT2Ldrhi12Cond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrhi12_cond_not_taken") {
  checkT2Ldrhi12Cond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrhi8_cond_taken") {
  checkT2Ldrhi8Cond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrhi8_cond_not_taken") {
  checkT2Ldrhi8Cond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrhpci_cond_taken") {
  checkT2LdrhpciCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrhpci_cond_not_taken") {
  checkT2LdrhpciCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrh_post_cond_taken") {
  checkT2LdrhPostCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrh_post_cond_not_taken") {
  checkT2LdrhPostCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrh_pre_cond_taken") {
  checkT2LdrhPreCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrh_pre_cond_not_taken") {
  checkT2LdrhPreCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrhs_cond_taken") {
  checkT2LdrhsCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrhs_cond_not_taken") {
  checkT2LdrhsCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrht_cond_taken") {
  checkT2LdrhtCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrht_cond_not_taken") {
  checkT2LdrhtCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrshi12_cond_taken") {
  checkT2Ldrshi12Cond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrshi12_cond_not_taken") {
  checkT2Ldrshi12Cond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrshi8_cond_taken") {
  checkT2Ldrshi8Cond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrshi8_cond_not_taken") {
  checkT2Ldrshi8Cond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrshpci_cond_taken") {
  checkT2LdrshpciCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrshpci_cond_not_taken") {
  checkT2LdrshpciCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrsh_post_cond_taken") {
  checkT2LdrshPostCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrsh_post_cond_not_taken") {
  checkT2LdrshPostCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrsh_pre_cond_taken") {
  checkT2LdrshPreCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrsh_pre_cond_not_taken") {
  checkT2LdrshPreCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrshs_cond_taken") {
  checkT2LdrshsCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrshs_cond_not_taken") {
  checkT2LdrshsCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrsht_cond_taken") {
  checkT2LdrshtCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrsht_cond_not_taken") {
  checkT2LdrshtCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2strhi12_cond_taken") {
  checkT2Strhi12Cond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2strhi12_cond_not_taken") {
  checkT2Strhi12Cond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2strhi8_cond_taken") {
  checkT2Strhi8Cond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2strhi8_cond_not_taken") {
  checkT2Strhi8Cond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2strh_post_cond_taken") {
  checkT2StrhPostCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2strh_post_cond_not_taken") {
  checkT2StrhPostCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2strh_pre_cond_taken") {
  checkT2StrhPreCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2strh_pre_cond_not_taken") {
  checkT2StrhPreCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2strht_cond_taken") {
  checkT2StrhtCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2strht_cond_not_taken") {
  checkT2StrhtCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2strhs_cond_taken") {
  checkT2StrhsCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2strhs_cond_not_taken") {
  checkT2StrhsCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tldrhi_cond_taken") {
  checkTLdrhiCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tldrhi_cond_not_taken") {
  checkTLdrhiCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tldrhr_cond_taken") {
  checkTLdrhrCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tldrhr_cond_not_taken") {
  checkTLdrhrCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tldrsh_cond_taken") {
  checkTLdrshCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tldrsh_cond_not_taken") {
  checkTLdrshCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tstrhi_cond_taken") {
  checkTStrhiCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tstrhi_cond_not_taken") {
  checkTStrhiCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tstrhr_cond_taken") {
  checkTStrhrCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tstrhr_cond_not_taken") {
  checkTStrhrCond(*this, false);
}
