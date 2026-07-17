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

#include "Engine/LLVMCPU.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/MC/MCInst.h"

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::conditionForIndex;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;
using QBDITestBatch2::setConditionCPSR;

static void checkLdrbi12Cond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(0);
  std::string source = std::string("ldrb") + cond.suffix + " r1, [r0, #4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDRBi12", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("LDRBi12", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0x88u : 0xdeadbeef));
}

static void checkLdrbPostImmCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(1);
  std::string source = std::string("ldrb") + cond.suffix + " r1, [r0], #4\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x84, 1, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDRB_POST_IMM", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("LDRB_POST_IMM", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0x84u : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkLdrbPostRegCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(2);
  std::string source = std::string("ldrb") + cond.suffix + " r1, [r0], r2\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x84, 1, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDRB_POST_REG", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("LDRB_POST_REG", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0x84u : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkLdrbPreImmCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(3);
  std::string source = std::string("ldrb") + cond.suffix + " r1, [r0, #4]!\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDRB_PRE_IMM", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("LDRB_PRE_IMM", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0x88u : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkLdrbPreRegCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(4);
  std::string source = std::string("ldrb") + cond.suffix + " r1, [r0, r2]!\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDRB_PRE_REG", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("LDRB_PRE_REG", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0x88u : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkLdrbrsCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(5);
  std::string source = std::string("ldrb") + cond.suffix + " r1, [r0, r2]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDRBrs", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("LDRBrs", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0x88u : 0xdeadbeef));
}

static void checkLdrbtPostImmCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(6);
  std::string source = std::string("ldrbt") + cond.suffix + " r1, [r0], #4\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x84, 1, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDRBT_POST_IMM", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("LDRBT_POST_IMM", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0x84u : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkLdrbtPostRegCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(7);
  std::string source = std::string("ldrbt") + cond.suffix + " r1, [r0], r2\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x84, 1, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDRBT_POST_REG", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("LDRBT_POST_REG", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0x84u : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkLdrsbCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(8);
  std::string source = std::string("ldrsb") + cond.suffix + " r1, [r0, #4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDRSB", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("LDRSB", QBDI::POSTINST, checkAccess, &expectedPost);

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
  CHECK(finalState->r1 == (taken ? 0xffffff88u : 0xdeadbeef));
}

static void checkLdrsbPostCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(9);
  std::string source = std::string("ldrsb") + cond.suffix + " r1, [r0], #4\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x84, 1, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDRSB_POST", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("LDRSB_POST", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0xffffff84u : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkLdrsbPreCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(10);
  std::string source = std::string("ldrsb") + cond.suffix + " r1, [r0, #4]!\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDRSB_PRE", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("LDRSB_PRE", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0xffffff88u : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkLdrsbtiCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(11);
  std::string source = std::string("ldrsbt") + cond.suffix + " r1, [r0], #4\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x84, 1, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDRSBTi", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("LDRSBTi", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0xffffff84u : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkLdrsbtrCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(12);
  std::string source = std::string("ldrsbt") + cond.suffix + " r1, [r0], r2\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x84, 1, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDRSBTr", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("LDRSBTr", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0xffffff84u : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkStrbi12Cond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(13);
  std::string source = std::string("strb") + cond.suffix + " r1, [r0, #4]\n";

  uint8_t buf[16];
  std::fill(std::begin(buf), std::end(buf), 0x55);
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xab, 1, QBDI::MEMORY_WRITE,
                              QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("STRBi12", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("STRBi12", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xab;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == (taken ? 0xab : 0x55));
}

static void checkStrbPostImmCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(14);
  std::string source = std::string("strb") + cond.suffix + " r1, [r0], #4\n";

  uint8_t buf[16];
  std::fill(std::begin(buf), std::end(buf), 0x55);
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[0], 0xab, 1, QBDI::MEMORY_WRITE,
                              QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("STRB_POST_IMM", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("STRB_POST_IMM", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xab;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == (taken ? 0xab : 0x55));
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[4] : (QBDI::rword)&buf[0]));
}

static void checkStrbPostRegCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(15);
  std::string source = std::string("strb") + cond.suffix + " r1, [r0], r2\n";

  uint8_t buf[16];
  std::fill(std::begin(buf), std::end(buf), 0x55);
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[0], 0xab, 1, QBDI::MEMORY_WRITE,
                              QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("STRB_POST_REG", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("STRB_POST_REG", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xab;
  state->r2 = 4;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == (taken ? 0xab : 0x55));
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[4] : (QBDI::rword)&buf[0]));
}

static void checkStrbPreImmCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(16);
  std::string source = std::string("strb") + cond.suffix + " r1, [r0, #4]!\n";

  uint8_t buf[16];
  std::fill(std::begin(buf), std::end(buf), 0x55);
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xab, 1, QBDI::MEMORY_WRITE,
                              QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("STRB_PRE_IMM", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("STRB_PRE_IMM", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xab;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == (taken ? 0xab : 0x55));
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[4] : (QBDI::rword)&buf[0]));
}

static void checkStrbPreRegCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(17);
  std::string source = std::string("strb") + cond.suffix + " r1, [r0, r2]!\n";

  uint8_t buf[16];
  std::fill(std::begin(buf), std::end(buf), 0x55);
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xab, 1, QBDI::MEMORY_WRITE,
                              QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("STRB_PRE_REG", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("STRB_PRE_REG", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xab;
  state->r2 = 4;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == (taken ? 0xab : 0x55));
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[4] : (QBDI::rword)&buf[0]));
}

static void checkStrbrsCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(18);
  std::string source = std::string("strb") + cond.suffix + " r1, [r0, r2]\n";

  uint8_t buf[16];
  std::fill(std::begin(buf), std::end(buf), 0x55);
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xab, 1, QBDI::MEMORY_WRITE,
                              QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("STRBrs", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("STRBrs", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xab;
  state->r2 = 4;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == (taken ? 0xab : 0x55));
}

static void checkStrbtPostImmCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(19);
  std::string source = std::string("strbt") + cond.suffix + " r1, [r0], #4\n";

  uint8_t buf[16];
  std::fill(std::begin(buf), std::end(buf), 0x55);
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[0], 0xab, 1, QBDI::MEMORY_WRITE,
                              QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("STRBT_POST_IMM", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("STRBT_POST_IMM", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xab;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == (taken ? 0xab : 0x55));
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[4] : (QBDI::rword)&buf[0]));
}

static void checkStrbtPostRegCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(20);
  std::string source = std::string("strbt") + cond.suffix + " r1, [r0], r2\n";

  uint8_t buf[16];
  std::fill(std::begin(buf), std::end(buf), 0x55);
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[0], 0xab, 1, QBDI::MEMORY_WRITE,
                              QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("STRBT_POST_REG", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("STRBT_POST_REG", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xab;
  state->r2 = 4;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == (taken ? 0xab : 0x55));
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[4] : (QBDI::rword)&buf[0]));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrbi12_cond_taken") {
  checkLdrbi12Cond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrbi12_cond_not_taken") {
  checkLdrbi12Cond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrb_post_imm_cond_taken") {
  checkLdrbPostImmCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrb_post_imm_cond_not_taken") {
  checkLdrbPostImmCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrb_post_reg_cond_taken") {
  checkLdrbPostRegCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrb_post_reg_cond_not_taken") {
  checkLdrbPostRegCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrb_pre_imm_cond_taken") {
  checkLdrbPreImmCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrb_pre_imm_cond_not_taken") {
  checkLdrbPreImmCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrb_pre_reg_cond_taken") {
  checkLdrbPreRegCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrb_pre_reg_cond_not_taken") {
  checkLdrbPreRegCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrbrs_cond_taken") {
  checkLdrbrsCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrbrs_cond_not_taken") {
  checkLdrbrsCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrbt_post_imm_cond_taken") {
  checkLdrbtPostImmCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrbt_post_imm_cond_not_taken") {
  checkLdrbtPostImmCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrbt_post_reg_cond_taken") {
  checkLdrbtPostRegCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrbt_post_reg_cond_not_taken") {
  checkLdrbtPostRegCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrsb_cond_taken") {
  checkLdrsbCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrsb_cond_not_taken") {
  checkLdrsbCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrsb_post_cond_taken") {
  checkLdrsbPostCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrsb_post_cond_not_taken") {
  checkLdrsbPostCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrsb_pre_cond_taken") {
  checkLdrsbPreCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrsb_pre_cond_not_taken") {
  checkLdrsbPreCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrsbti_cond_taken") {
  checkLdrsbtiCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrsbti_cond_not_taken") {
  checkLdrsbtiCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrsbtr_cond_taken") {
  checkLdrsbtrCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrsbtr_cond_not_taken") {
  checkLdrsbtrCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strbi12_cond_taken") {
  checkStrbi12Cond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-strbi12_cond_not_taken") {
  checkStrbi12Cond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-strb_post_imm_cond_taken") {
  checkStrbPostImmCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-strb_post_imm_cond_not_taken") {
  checkStrbPostImmCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-strb_post_reg_cond_taken") {
  checkStrbPostRegCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-strb_post_reg_cond_not_taken") {
  checkStrbPostRegCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-strb_pre_imm_cond_taken") {
  checkStrbPreImmCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-strb_pre_imm_cond_not_taken") {
  checkStrbPreImmCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-strb_pre_reg_cond_taken") {
  checkStrbPreRegCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-strb_pre_reg_cond_not_taken") {
  checkStrbPreRegCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strbrs_cond_taken") {
  checkStrbrsCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strbrs_cond_not_taken") {
  checkStrbrsCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-strbt_post_imm_cond_taken") {
  checkStrbtPostImmCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-strbt_post_imm_cond_not_taken") {
  checkStrbtPostImmCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-strbt_post_reg_cond_taken") {
  checkStrbtPostRegCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-strbt_post_reg_cond_not_taken") {
  checkStrbtPostRegCond(*this, false);
}

static bool isSwpFamilyDecodable() {
  QBDI::LLVMCPU probe("", "arm", {});
  llvm::MCInst inst;
  uint64_t size;
  const uint8_t bytes[] = {0x91, 0x20, 0x40, 0xe1}; // swpb r2, r1, [r0]
  return probe.getInstruction(inst, size, llvm::ArrayRef<uint8_t>(bytes, 4), 0);
}

static void checkSwpbCond(APITest &fixture, bool taken) {
  if (!isSwpFamilyDecodable()) {
    return;
  }
  const auto &cond = conditionForIndex(21);
  std::string source = std::string("swpb") + cond.suffix + " r2, r1, [r0]\n";

  uint8_t buf[4] = {0x11, 0x22, 0x33, 0x44};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[0], 0x11, 1, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = {{(QBDI::rword)&buf[0], 0x11, 1, QBDI::MEMORY_READ,
                              QBDI::MEMORY_NO_FLAGS},
                             {(QBDI::rword)&buf[0], 0x99, 1, QBDI::MEMORY_WRITE,
                              QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("SWPB", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("SWPB", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0x99;
  state->r2 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r2 == (taken ? 0x11u : 0xdeadbeefu));
  CHECK(buf[0] == (taken ? 0x99 : 0x11));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-swpb_cond_taken") {
  checkSwpbCond(*this, true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-swpb_cond_not_taken") {
  checkSwpbCond(*this, false);
}
