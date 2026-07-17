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

static void checkLdri12Cond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(0);
  std::string source = std::string("ldr") + cond.suffix + " r1, [r0, #4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8b8a8988, 4,
                             QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDRi12", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("LDRi12", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0x8b8a8988 : 0xdeadbeef));
}

static void checkLdrrsCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(1);
  std::string source = std::string("ldr") + cond.suffix + " r1, [r0, r2]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8b8a8988, 4,
                             QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDRrs", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("LDRrs", QBDI::POSTINST, checkAccess, &expectedPost);

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
  CHECK(finalState->r1 == (taken ? 0x8b8a8988 : 0xdeadbeef));
}

static void checkLdrPreImmCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(2);
  std::string source = std::string("ldr") + cond.suffix + " r1, [r0, #4]!\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8b8a8988, 4,
                             QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDR_PRE_IMM", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("LDR_PRE_IMM", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0x8b8a8988 : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkLdrPreRegCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(3);
  std::string source = std::string("ldr") + cond.suffix + " r1, [r0, r2]!\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8b8a8988, 4,
                             QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDR_PRE_REG", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("LDR_PRE_REG", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0x8b8a8988 : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkLdrPostImmCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(4);
  std::string source = std::string("ldr") + cond.suffix + " r1, [r0], #4\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x87868584, 4,
                             QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDR_POST_IMM", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("LDR_POST_IMM", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0x87868584 : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkLdrPostRegCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(5);
  std::string source = std::string("ldr") + cond.suffix + " r1, [r0], r2\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x87868584, 4,
                             QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDR_POST_REG", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("LDR_POST_REG", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0x87868584 : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkLdrtPostImmCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(6);
  std::string source = std::string("ldrt") + cond.suffix + " r1, [r0], #4\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x87868584, 4,
                             QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDRT_POST_IMM", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("LDRT_POST_IMM", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0x87868584 : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkLdrtPostRegCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(7);
  std::string source = std::string("ldrt") + cond.suffix + " r1, [r0], r2\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x87868584, 4,
                             QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDRT_POST_REG", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("LDRT_POST_REG", QBDI::POSTINST, checkAccess,
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
  CHECK(finalState->r1 == (taken ? 0x87868584 : 0xdeadbeef));
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[8] : (QBDI::rword)&buf[4]));
}

static void checkStri12Cond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(8);
  std::string source = std::string("str") + cond.suffix + " r1, [r0, #4]\n";

  uint8_t buf[16];
  std::fill(std::begin(buf), std::end(buf), 0x55);
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xaabbccdd, 4,
                              QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("STRi12", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("STRi12", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xaabbccdd;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[4] == (taken ? 0xaabbccdd : 0x55555555));
}

static void checkStrrsCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(9);
  std::string source = std::string("str") + cond.suffix + " r1, [r0, r2]\n";

  uint8_t buf[16];
  std::fill(std::begin(buf), std::end(buf), 0x55);
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xaabbccdd, 4,
                              QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("STRrs", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("STRrs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xaabbccdd;
  state->r2 = 4;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[4] == (taken ? 0xaabbccdd : 0x55555555));
}

static void checkStrPreImmCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(10);
  std::string source = std::string("str") + cond.suffix + " r1, [r0, #4]!\n";

  uint8_t buf[16];
  std::fill(std::begin(buf), std::end(buf), 0x55);
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xaabbccdd, 4,
                              QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("STR_PRE_IMM", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("STR_PRE_IMM", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xaabbccdd;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[4] == (taken ? 0xaabbccdd : 0x55555555));
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[4] : (QBDI::rword)&buf[0]));
}

static void checkStrPreRegCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(11);
  std::string source = std::string("str") + cond.suffix + " r1, [r0, r2]!\n";

  uint8_t buf[16];
  std::fill(std::begin(buf), std::end(buf), 0x55);
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xaabbccdd, 4,
                              QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("STR_PRE_REG", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("STR_PRE_REG", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xaabbccdd;
  state->r2 = 4;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[4] == (taken ? 0xaabbccdd : 0x55555555));
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[4] : (QBDI::rword)&buf[0]));
}

static void checkStrPostImmCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(12);
  std::string source = std::string("str") + cond.suffix + " r1, [r0], #4\n";

  uint8_t buf[16];
  std::fill(std::begin(buf), std::end(buf), 0x55);
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[0], 0xaabbccdd, 4,
                              QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("STR_POST_IMM", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("STR_POST_IMM", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xaabbccdd;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[0] == (taken ? 0xaabbccdd : 0x55555555));
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[4] : (QBDI::rword)&buf[0]));
}

static void checkStrPostRegCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(13);
  std::string source = std::string("str") + cond.suffix + " r1, [r0], r2\n";

  uint8_t buf[16];
  std::fill(std::begin(buf), std::end(buf), 0x55);
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[0], 0xaabbccdd, 4,
                              QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("STR_POST_REG", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("STR_POST_REG", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xaabbccdd;
  state->r2 = 4;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[0] == (taken ? 0xaabbccdd : 0x55555555));
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[4] : (QBDI::rword)&buf[0]));
}

static void checkStrtPostImmCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(14);
  std::string source = std::string("strt") + cond.suffix + " r1, [r0], #4\n";

  uint8_t buf[16];
  std::fill(std::begin(buf), std::end(buf), 0x55);
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[0], 0xaabbccdd, 4,
                              QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("STRT_POST_IMM", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("STRT_POST_IMM", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xaabbccdd;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[0] == (taken ? 0xaabbccdd : 0x55555555));
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[4] : (QBDI::rword)&buf[0]));
}

static void checkStrtPostRegCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(15);
  std::string source = std::string("strt") + cond.suffix + " r1, [r0], r2\n";

  uint8_t buf[16];
  std::fill(std::begin(buf), std::end(buf), 0x55);
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&buf[0], 0xaabbccdd, 4,
                              QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("STRT_POST_REG", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("STRT_POST_REG", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xaabbccdd;
  state->r2 = 4;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[0] == (taken ? 0xaabbccdd : 0x55555555));
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 ==
        (taken ? (QBDI::rword)&buf[4] : (QBDI::rword)&buf[0]));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldri12_cond_taken") {
  checkLdri12Cond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldri12_cond_not_taken") {
  checkLdri12Cond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrrs_cond_taken") {
  checkLdrrsCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrrs_cond_not_taken") {
  checkLdrrsCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldr_pre_imm_cond_taken") {
  checkLdrPreImmCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldr_pre_imm_cond_not_taken") {
  checkLdrPreImmCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldr_pre_reg_cond_taken") {
  checkLdrPreRegCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldr_pre_reg_cond_not_taken") {
  checkLdrPreRegCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldr_post_imm_cond_taken") {
  checkLdrPostImmCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldr_post_imm_cond_not_taken") {
  checkLdrPostImmCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldr_post_reg_cond_taken") {
  checkLdrPostRegCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldr_post_reg_cond_not_taken") {
  checkLdrPostRegCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrt_post_imm_cond_taken") {
  checkLdrtPostImmCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrt_post_imm_cond_not_taken") {
  checkLdrtPostImmCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrt_post_reg_cond_taken") {
  checkLdrtPostRegCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrt_post_reg_cond_not_taken") {
  checkLdrtPostRegCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-stri12_cond_taken") {
  checkStri12Cond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-stri12_cond_not_taken") {
  checkStri12Cond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strrs_cond_taken") {
  checkStrrsCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strrs_cond_not_taken") {
  checkStrrsCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-str_pre_imm_cond_taken") {
  checkStrPreImmCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-str_pre_imm_cond_not_taken") {
  checkStrPreImmCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-str_pre_reg_cond_taken") {
  checkStrPreRegCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-str_pre_reg_cond_not_taken") {
  checkStrPreRegCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-str_post_imm_cond_taken") {
  checkStrPostImmCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-str_post_imm_cond_not_taken") {
  checkStrPostImmCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-str_post_reg_cond_taken") {
  checkStrPostRegCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-str_post_reg_cond_not_taken") {
  checkStrPostRegCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-strt_post_imm_cond_taken") {
  checkStrtPostImmCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-strt_post_imm_cond_not_taken") {
  checkStrtPostImmCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-strt_post_reg_cond_taken") {
  checkStrtPostRegCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-strt_post_reg_cond_not_taken") {
  checkStrtPostRegCond(*this, false);
}

static bool isSwpFamilyDecodable() {
  QBDI::LLVMCPU probe("", "arm", {});
  llvm::MCInst inst;
  uint64_t size;
  const uint8_t bytes[] = {0x91, 0x20, 0x00, 0xe1}; // swp r2, r1, [r0]
  return probe.getInstruction(inst, size, llvm::ArrayRef<uint8_t>(bytes, 4), 0);
}

static void checkSwpCond(APITest &fixture, bool taken) {
  if (!isSwpFamilyDecodable()) {
    return;
  }
  const auto &cond = conditionForIndex(16);
  std::string source = std::string("swp") + cond.suffix + " r2, r1, [r0]\n";

  uint8_t buf[4] = {0x11, 0x22, 0x33, 0x44};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&buf[0], 0x44332211, 4,
                             QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = {{(QBDI::rword)&buf[0], 0x44332211, 4,
                              QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
                             {(QBDI::rword)&buf[0], 0xaabbccdd, 4,
                              QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("SWP", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("SWP", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xaabbccdd;
  state->r2 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r2 == (taken ? 0x44332211u : 0xdeadbeefu));
  CHECK(*(uint32_t *)&buf[0] == (taken ? 0xaabbccddu : 0x44332211u));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-swp_cond_taken") {
  checkSwpCond(*this, true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-swp_cond_not_taken") {
  checkSwpCond(*this, false);
}
