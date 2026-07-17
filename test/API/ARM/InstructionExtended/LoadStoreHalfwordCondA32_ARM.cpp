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

static void checkLdrhCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(0);
  std::string source = std::string("ldrh") + cond.suffix + " r1, [r0, #4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8988, 2, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDRH", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("LDRH", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r1 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? 0x8988u : 0xdeadbeef));
}

static void checkLdrhPostCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(1);
  std::string source = std::string("ldrh") + cond.suffix + " r1, [r0], #4\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x8584, 2, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDRH_POST", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("LDRH_POST", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r1 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? 0x8584u : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkLdrhPreCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(2);
  std::string source = std::string("ldrh") + cond.suffix + " r1, [r0, #4]!\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8988, 2, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDRH_PRE", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("LDRH_PRE", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r1 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? 0x8988u : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkLdrhtiCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(3);
  std::string source = std::string("ldrht") + cond.suffix + " r1, [r0], #4\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x8584, 2, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDRHTi", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("LDRHTi", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r1 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? 0x8584u : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkLdrhtrCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(4);
  std::string source = std::string("ldrht") + cond.suffix + " r1, [r0], r2\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x8584, 2, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDRHTr", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("LDRHTr", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r2 = 4;
  state->r1 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? 0x8584u : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkLdrshCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(5);
  std::string source = std::string("ldrsh") + cond.suffix + " r1, [r0, #4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8988, 2, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDRSH", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("LDRSH", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r1 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? 0xffff8988u : 0xdeadbeef));
}

static void checkLdrshPostCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(6);
  std::string source = std::string("ldrsh") + cond.suffix + " r1, [r0], #4\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x8584, 2, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDRSH_POST", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("LDRSH_POST", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r1 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? 0xffff8584u : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkLdrshPreCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(7);
  std::string source = std::string("ldrsh") + cond.suffix + " r1, [r0, #4]!\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8988, 2, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDRSH_PRE", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("LDRSH_PRE", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r1 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? 0xffff8988u : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkLdrshtiCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(8);
  std::string source = std::string("ldrsht") + cond.suffix + " r1, [r0], #4\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x8584, 2, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDRSHTi", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("LDRSHTi", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r1 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? 0xffff8584u : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkLdrshtrCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(9);
  std::string source = std::string("ldrsht") + cond.suffix + " r1, [r0], r2\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x8584, 2, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDRSHTr", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("LDRSHTr", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r2 = 4;
  state->r1 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r1 == (taken ? 0xffff8584u : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkStrhCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(10);
  std::string source = std::string("strh") + cond.suffix + " r1, [r0, #4]\n";

  uint8_t buf[16] = {0};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xabcd, 2,
                              QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("STRH", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("STRH", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xabcd;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == (taken ? 0xcd : 0x00));
  CHECK(buf[5] == (taken ? 0xab : 0x00));
}

static void checkStrhPostCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(11);
  std::string source = std::string("strh") + cond.suffix + " r1, [r0], #4\n";

  uint8_t buf[16] = {0};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[0], 0xabcd, 2,
                              QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("STRH_POST", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("STRH_POST", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xabcd;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == (taken ? 0xcd : 0x00));
  CHECK(buf[1] == (taken ? 0xab : 0x00));
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[4] : (QBDI::rword)&buf[0]));
}

static void checkStrhPreCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(12);
  std::string source = std::string("strh") + cond.suffix + " r1, [r0, #4]!\n";

  uint8_t buf[16] = {0};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xabcd, 2,
                              QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("STRH_PRE", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("STRH_PRE", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xabcd;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == (taken ? 0xcd : 0x00));
  CHECK(buf[5] == (taken ? 0xab : 0x00));
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[4] : (QBDI::rword)&buf[0]));
}

static void checkStrhtiCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(13);
  std::string source = std::string("strht") + cond.suffix + " r1, [r0], #4\n";

  uint8_t buf[16] = {0};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[0], 0xabcd, 2,
                              QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("STRHTi", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("STRHTi", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xabcd;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == (taken ? 0xcd : 0x00));
  CHECK(buf[1] == (taken ? 0xab : 0x00));
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[4] : (QBDI::rword)&buf[0]));
}

static void checkStrhtrCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(14);
  std::string source = std::string("strht") + cond.suffix + " r1, [r0], r2\n";

  uint8_t buf[16] = {0};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[0], 0xabcd, 2,
                              QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("STRHTr", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("STRHTr", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xabcd;
  state->r2 = 4;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == (taken ? 0xcd : 0x00));
  CHECK(buf[1] == (taken ? 0xab : 0x00));
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[4] : (QBDI::rword)&buf[0]));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrh_cond_taken") {
  checkLdrhCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrh_cond_not_taken") {
  checkLdrhCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrh_post_cond_taken") {
  checkLdrhPostCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrh_post_cond_not_taken") {
  checkLdrhPostCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrh_pre_cond_taken") {
  checkLdrhPreCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrh_pre_cond_not_taken") {
  checkLdrhPreCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrhti_cond_taken") {
  checkLdrhtiCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrhti_cond_not_taken") {
  checkLdrhtiCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrhtr_cond_taken") {
  checkLdrhtrCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrhtr_cond_not_taken") {
  checkLdrhtrCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrsh_cond_taken") {
  checkLdrshCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrsh_cond_not_taken") {
  checkLdrshCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrsh_post_cond_taken") {
  checkLdrshPostCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrsh_post_cond_not_taken") {
  checkLdrshPostCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrsh_pre_cond_taken") {
  checkLdrshPreCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrsh_pre_cond_not_taken") {
  checkLdrshPreCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrshti_cond_taken") {
  checkLdrshtiCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrshti_cond_not_taken") {
  checkLdrshtiCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrshtr_cond_taken") {
  checkLdrshtrCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrshtr_cond_not_taken") {
  checkLdrshtrCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strh_cond_taken") {
  checkStrhCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strh_cond_not_taken") {
  checkStrhCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strh_post_cond_taken") {
  checkStrhPostCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-strh_post_cond_not_taken") {
  checkStrhPostCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strh_pre_cond_taken") {
  checkStrhPreCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-strh_pre_cond_not_taken") {
  checkStrhPreCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strhti_cond_taken") {
  checkStrhtiCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strhti_cond_not_taken") {
  checkStrhtiCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strhtr_cond_taken") {
  checkStrhtrCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strhtr_cond_not_taken") {
  checkStrhtrCond(*this, false);
}
