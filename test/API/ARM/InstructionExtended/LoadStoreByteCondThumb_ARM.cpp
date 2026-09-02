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

static void checkT2Ldrbi12Cond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(0);
  std::string source = std::string("it ") + cond.suffix + "\nldrb" +
                       cond.suffix + ".w r1, [r0, #4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("t2LDRBi12", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2LDRBi12", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0x88u : 0xdeadbeef));
}

static void checkT2Ldrbi8Cond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(1);
  std::string source = std::string("it ") + cond.suffix + "\nldrb" +
                       cond.suffix + ".w r1, [r0, #-4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x84, 1, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("t2LDRBi8", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2LDRBi8", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0x84u : 0xdeadbeef));
}

static void checkT2LdrbpciCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(2);
  std::string source = std::string(
                           "b 1f\n"
                           "label:\n"
                           ".byte 0x9b, 0x00\n"
                           "1:\n"
                           "it ") +
                       cond.suffix + "\nldrb" + cond.suffix + ".w r1, label\n";

  bool seenPre = false, seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("t2LDRBpci", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             auto accesses = vmi->getInstMemoryAccess();
                             if (taken) {
                               REQUIRE(accesses.size() == 1);
                               CHECK(accesses[0].value == 0x9b);
                               CHECK(accesses[0].size == 1);
                               CHECK(accesses[0].type == QBDI::MEMORY_READ);
                             } else {
                               CHECK(accesses.empty());
                             }
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("t2LDRBpci", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             auto accesses = vmi->getInstMemoryAccess();
                             if (taken) {
                               REQUIRE(accesses.size() == 1);
                               CHECK(accesses[0].value == 0x9b);
                               CHECK(accesses[0].size == 1);
                               CHECK(accesses[0].type == QBDI::MEMORY_READ);
                               CHECK(gprState->r1 == 0x9b);
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
  CHECK(finalState->r1 == (taken ? 0x9bu : 0xdeadbeef));
}

static void checkT2LdrbPostCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(3);
  std::string source = std::string("it ") + cond.suffix + "\nldrb" +
                       cond.suffix + ".w r1, [r0], #4\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x84, 1, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("t2LDRB_POST", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2LDRB_POST", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0x84u : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkT2LdrbPreCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(4);
  std::string source = std::string("it ") + cond.suffix + "\nldrb" +
                       cond.suffix + ".w r1, [r0, #4]!\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("t2LDRB_PRE", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2LDRB_PRE", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0x88u : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkT2LdrbsCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(5);
  std::string source = std::string("it ") + cond.suffix + "\nldrb" +
                       cond.suffix + ".w r1, [r0, r2]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("t2LDRBs", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("t2LDRBs", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0x88u : 0xdeadbeef));
}

static void checkT2LdrbtCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(6);
  std::string source = std::string("it ") + cond.suffix + "\nldrbt" +
                       cond.suffix + " r1, [r0, #4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("t2LDRBT", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("t2LDRBT", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0x88u : 0xdeadbeef));
}

static void checkT2Ldrsbi12Cond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(7);
  std::string source = std::string("it ") + cond.suffix + "\nldrsb" +
                       cond.suffix + ".w r1, [r0, #4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("t2LDRSBi12", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2LDRSBi12", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0xffffff88u : 0xdeadbeef));
}

static void checkT2Ldrsbi8Cond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(8);
  std::string source = std::string("it ") + cond.suffix + "\nldrsb" +
                       cond.suffix + ".w r1, [r0, #-4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x84, 1, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("t2LDRSBi8", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2LDRSBi8", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0xffffff84u : 0xdeadbeef));
}

static void checkT2LdrsbpciCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(9);
  std::string source = std::string(
                           "b 1f\n"
                           "label:\n"
                           ".byte 0x9b, 0x00\n"
                           "1:\n"
                           "it ") +
                       cond.suffix + "\nldrsb" + cond.suffix + ".w r1, label\n";

  bool seenPre = false, seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("t2LDRSBpci", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             auto accesses = vmi->getInstMemoryAccess();
                             if (taken) {
                               REQUIRE(accesses.size() == 1);
                               CHECK(accesses[0].value == 0x9b);
                               CHECK(accesses[0].size == 1);
                               CHECK(accesses[0].type == QBDI::MEMORY_READ);
                             } else {
                               CHECK(accesses.empty());
                             }
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("t2LDRSBpci", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             auto accesses = vmi->getInstMemoryAccess();
                             if (taken) {
                               REQUIRE(accesses.size() == 1);
                               CHECK(accesses[0].value == 0x9b);
                               CHECK(accesses[0].size == 1);
                               CHECK(accesses[0].type == QBDI::MEMORY_READ);
                               CHECK(gprState->r1 == 0xffffff9b);
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
  CHECK(finalState->r1 == (taken ? 0xffffff9bu : 0xdeadbeef));
}

static void checkT2LdrsbPostCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(10);
  std::string source = std::string("it ") + cond.suffix + "\nldrsb" +
                       cond.suffix + ".w r1, [r0], #4\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x84, 1, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("t2LDRSB_POST", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2LDRSB_POST", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0xffffff84u : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkT2LdrsbPreCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(11);
  std::string source = std::string("it ") + cond.suffix + "\nldrsb" +
                       cond.suffix + ".w r1, [r0, #4]!\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("t2LDRSB_PRE", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2LDRSB_PRE", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0xffffff88u : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkT2LdrsbsCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(12);
  std::string source = std::string("it ") + cond.suffix + "\nldrsb" +
                       cond.suffix + ".w r1, [r0, r2]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("t2LDRSBs", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2LDRSBs", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0xffffff88u : 0xdeadbeef));
}

static void checkT2LdrsbtCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(13);
  std::string source = std::string("it ") + cond.suffix + "\nldrsbt" +
                       cond.suffix + " r1, [r0, #4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("t2LDRSBT", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2LDRSBT", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0xffffff88u : 0xdeadbeef));
}

static void checkT2Strbi12Cond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(14);
  std::string source = std::string("it ") + cond.suffix + "\nstrb" +
                       cond.suffix + ".w r1, [r0, #4]\n";

  uint8_t buf[16];
  std::fill(std::begin(buf), std::end(buf), 0x55);
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xab, 1, QBDI::MEMORY_WRITE,
                              QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("t2STRBi12", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2STRBi12", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xab;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == (taken ? 0xab : 0x55));
}

static void checkT2Strbi8Cond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(15);
  std::string source = std::string("it ") + cond.suffix + "\nstrb" +
                       cond.suffix + ".w r1, [r0, #-4]\n";

  uint8_t buf[16];
  std::fill(std::begin(buf), std::end(buf), 0x55);
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xab, 1, QBDI::MEMORY_WRITE,
                              QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("t2STRBi8", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2STRBi8", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[8];
  state->r1 = 0xab;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == (taken ? 0xab : 0x55));
}

static void checkT2StrbPostCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(16);
  std::string source = std::string("it ") + cond.suffix + "\nstrb" +
                       cond.suffix + ".w r1, [r0], #4\n";

  uint8_t buf[16];
  std::fill(std::begin(buf), std::end(buf), 0x55);
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[0], 0xab, 1, QBDI::MEMORY_WRITE,
                              QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("t2STRB_POST", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2STRB_POST", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xab;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == (taken ? 0xab : 0x55));
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[4] : (QBDI::rword)&buf[0]));
}

static void checkT2StrbPreCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(17);
  std::string source = std::string("it ") + cond.suffix + "\nstrb" +
                       cond.suffix + ".w r1, [r0, #4]!\n";

  uint8_t buf[16];
  std::fill(std::begin(buf), std::end(buf), 0x55);
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xab, 1, QBDI::MEMORY_WRITE,
                              QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("t2STRB_PRE", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2STRB_PRE", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xab;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == (taken ? 0xab : 0x55));
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[4] : (QBDI::rword)&buf[0]));
}

static void checkT2StrbsCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(18);
  std::string source = std::string("it ") + cond.suffix + "\nstrb" +
                       cond.suffix + ".w r1, [r0, r2]\n";

  uint8_t buf[16];
  std::fill(std::begin(buf), std::end(buf), 0x55);
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xab, 1, QBDI::MEMORY_WRITE,
                              QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("t2STRBs", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("t2STRBs", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xab;
  state->r2 = 4;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == (taken ? 0xab : 0x55));
}

static void checkT2StrbtCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(19);
  std::string source = std::string("it ") + cond.suffix + "\nstrbt" +
                       cond.suffix + " r1, [r0, #4]\n";

  uint8_t buf[16];
  std::fill(std::begin(buf), std::end(buf), 0x55);
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xab, 1, QBDI::MEMORY_WRITE,
                              QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("t2STRBT", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("t2STRBT", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xab;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == (taken ? 0xab : 0x55));
}

static void checkTLdrbiCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(20);
  std::string source = std::string("it ") + cond.suffix + "\nldrb" +
                       cond.suffix + " r1, [r0, #4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("tLDRBi", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tLDRBi", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0x88u : 0xdeadbeef));
}

static void checkTLdrbrCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(21);
  std::string source = std::string("it ") + cond.suffix + "\nldrb" +
                       cond.suffix + " r1, [r0, r2]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("tLDRBr", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tLDRBr", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0x88u : 0xdeadbeef));
}

static void checkTLdrsbCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(22);
  std::string source = std::string("it ") + cond.suffix + "\nldrsb" +
                       cond.suffix + " r1, [r0, r2]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("tLDRSB", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tLDRSB", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0xffffff88u : 0xdeadbeef));
}

static void checkTStrbiCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(23);
  std::string source = std::string("it ") + cond.suffix + "\nstrb" +
                       cond.suffix + " r1, [r0, #4]\n";

  uint8_t buf[16];
  std::fill(std::begin(buf), std::end(buf), 0x55);
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xab, 1, QBDI::MEMORY_WRITE,
                              QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("tSTRBi", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tSTRBi", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xab;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == (taken ? 0xab : 0x55));
}

static void checkTStrbrCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(24);
  std::string source = std::string("it ") + cond.suffix + "\nstrb" +
                       cond.suffix + " r1, [r0, r2]\n";

  uint8_t buf[16];
  std::fill(std::begin(buf), std::end(buf), 0x55);
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xab, 1, QBDI::MEMORY_WRITE,
                              QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("tSTRBr", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tSTRBr", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xab;
  state->r2 = 4;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == (taken ? 0xab : 0x55));
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrbi12_cond_taken") {
  checkT2Ldrbi12Cond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrbi12_cond_not_taken") {
  checkT2Ldrbi12Cond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrbi8_cond_taken") {
  checkT2Ldrbi8Cond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrbi8_cond_not_taken") {
  checkT2Ldrbi8Cond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrbpci_cond_taken") {
  checkT2LdrbpciCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrbpci_cond_not_taken") {
  checkT2LdrbpciCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrb_post_cond_taken") {
  checkT2LdrbPostCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrb_post_cond_not_taken") {
  checkT2LdrbPostCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrb_pre_cond_taken") {
  checkT2LdrbPreCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrb_pre_cond_not_taken") {
  checkT2LdrbPreCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrbs_cond_taken") {
  checkT2LdrbsCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrbs_cond_not_taken") {
  checkT2LdrbsCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrbt_cond_taken") {
  checkT2LdrbtCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrbt_cond_not_taken") {
  checkT2LdrbtCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrsbi12_cond_taken") {
  checkT2Ldrsbi12Cond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrsbi12_cond_not_taken") {
  checkT2Ldrsbi12Cond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrsbi8_cond_taken") {
  checkT2Ldrsbi8Cond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrsbi8_cond_not_taken") {
  checkT2Ldrsbi8Cond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrsbpci_cond_taken") {
  checkT2LdrsbpciCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrsbpci_cond_not_taken") {
  checkT2LdrsbpciCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrsb_post_cond_taken") {
  checkT2LdrsbPostCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrsb_post_cond_not_taken") {
  checkT2LdrsbPostCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrsb_pre_cond_taken") {
  checkT2LdrsbPreCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrsb_pre_cond_not_taken") {
  checkT2LdrsbPreCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrsbs_cond_taken") {
  checkT2LdrsbsCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrsbs_cond_not_taken") {
  checkT2LdrsbsCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrsbt_cond_taken") {
  checkT2LdrsbtCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrsbt_cond_not_taken") {
  checkT2LdrsbtCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2strbi12_cond_taken") {
  checkT2Strbi12Cond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2strbi12_cond_not_taken") {
  checkT2Strbi12Cond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2strbi8_cond_taken") {
  checkT2Strbi8Cond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2strbi8_cond_not_taken") {
  checkT2Strbi8Cond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2strb_post_cond_taken") {
  checkT2StrbPostCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2strb_post_cond_not_taken") {
  checkT2StrbPostCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2strb_pre_cond_taken") {
  checkT2StrbPreCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2strb_pre_cond_not_taken") {
  checkT2StrbPreCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2strbs_cond_taken") {
  checkT2StrbsCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2strbs_cond_not_taken") {
  checkT2StrbsCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2strbt_cond_taken") {
  checkT2StrbtCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2strbt_cond_not_taken") {
  checkT2StrbtCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tldrbi_cond_taken") {
  checkTLdrbiCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tldrbi_cond_not_taken") {
  checkTLdrbiCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tldrbr_cond_taken") {
  checkTLdrbrCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tldrbr_cond_not_taken") {
  checkTLdrbrCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tldrsb_cond_taken") {
  checkTLdrsbCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tldrsb_cond_not_taken") {
  checkTLdrsbCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tstrbi_cond_taken") {
  checkTStrbiCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tstrbi_cond_not_taken") {
  checkTStrbiCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tstrbr_cond_taken") {
  checkTStrbrCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tstrbr_cond_not_taken") {
  checkTStrbrCond(*this, false);
}
