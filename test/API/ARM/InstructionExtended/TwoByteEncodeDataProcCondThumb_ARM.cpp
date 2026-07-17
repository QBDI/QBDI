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

using QBDITestBatch2::APSR_C;
using QBDITestBatch2::APSR_N;
using QBDITestBatch2::APSR_NZCV_MASK;
using QBDITestBatch2::APSR_V;
using QBDITestBatch2::APSR_Z;
using QBDITestBatch2::checkAccess;
using QBDITestBatch2::conditionForIndex;
using QBDITestBatch2::ExpectedMemoryAccesses;
using QBDITestBatch2::nzcvForCondition;
using QBDITestBatch2::setConditionCPSR;

static void checkTAdcCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(0);
  std::string source = std::string("it ") + cond.suffix + "\n" + "adc" +
                       cond.suffix + " r0, r1\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tADC", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tADC", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 5;
  state->r1 = 10;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword carry = (nzcvForCondition(cond.cond, taken) & APSR_C) ? 1u : 0u;

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? (15u + carry) : 5u));
}

static void checkTAddi3Cond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(1);
  std::string source = std::string("it ") + cond.suffix + "\n" + "add" +
                       cond.suffix + " r0, r1, #3\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tADDi3", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tADDi3", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0xdeadbeef;
  state->r1 = 7;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? 10u : 0xdeadbeefu));
}

static void checkTAddi8Cond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(2);
  std::string source = std::string("it ") + cond.suffix + "\n" + "add" +
                       cond.suffix + " r0, #8\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tADDi8", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tADDi8", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 20;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? 28u : 20u));
}

static void checkTAddrSPCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(3);
  std::string source = std::string("it ") + cond.suffix + "\n" + "add" +
                       cond.suffix + " r0, sp, r0\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tADDrSP", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tADDrSP", QBDI::POSTINST, checkAccess,
                           &expectedPost);
  QBDI::rword spDuring = 0;
  fixture.vm.addMnemonicCB("tADDrSP", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             spDuring = gprState->sp;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0x100;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? (spDuring + 0x100) : 0x100u));
}

static void checkTAddrSPiCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(4);
  std::string source = std::string("it ") + cond.suffix + "\n" + "add" +
                       cond.suffix + " r0, sp, #4\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tADDrSPi", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("tADDrSPi", QBDI::POSTINST, checkAccess,
                           &expectedPost);
  QBDI::rword spDuring = 0;
  fixture.vm.addMnemonicCB("tADDrSPi", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             spDuring = gprState->sp;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? (spDuring + 4) : 0xdeadbeefu));
}

static void checkTAddspiCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(5);
  std::string source = std::string("it ") + cond.suffix + "\n" + "add" +
                       cond.suffix + " sp, #4\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tADDspi", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tADDspi", QBDI::POSTINST, checkAccess,
                           &expectedPost);
  QBDI::rword spDuring = 0;
  fixture.vm.addMnemonicCB("tADDspi", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             spDuring = gprState->sp;
                             return QBDI::VMAction::CONTINUE;
                           });

  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->sp == (taken ? (spDuring + 4) : spDuring));
}

static void checkTAddsprCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(6);
  std::string source = std::string("it ") + cond.suffix + "\n" + "add" +
                       cond.suffix + " sp, r0\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tADDspr", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tADDspr", QBDI::POSTINST, checkAccess,
                           &expectedPost);
  QBDI::rword spDuring = 0;
  fixture.vm.addMnemonicCB("tADDspr", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             spDuring = gprState->sp;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0x40;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->sp == (taken ? (spDuring + 0x40) : spDuring));
}

static void checkTAndCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(7);
  std::string source = std::string("it ") + cond.suffix + "\n" + "and" +
                       cond.suffix + " r0, r1\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tAND", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tAND", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0xFF00FF00;
  state->r1 = 0x0F0F0F0F;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? 0x0F000F00u : 0xFF00FF00u));
}

static void checkTAsrriCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(8);
  std::string source = std::string("it ") + cond.suffix + "\n" + "asr" +
                       cond.suffix + " r0, r1, #4\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tASRri", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tASRri", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0xdeadbeef;
  state->r1 = 0xFFFFFF00;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? 0xFFFFFFF0u : 0xdeadbeefu));
}

static void checkTAsrrrCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(9);
  std::string source = std::string("it ") + cond.suffix + "\n" + "asr" +
                       cond.suffix + " r0, r1\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tASRrr", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tASRrr", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0xFFFFFF00;
  state->r1 = 4;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? 0xFFFFFFF0u : 0xFFFFFF00u));
}

static void checkTBicCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(10);
  std::string source = std::string("it ") + cond.suffix + "\n" + "bic" +
                       cond.suffix + " r0, r1\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tBIC", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tBIC", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0xFFFFFFFF;
  state->r1 = 0x0000FFFF;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? 0xFFFF0000u : 0xFFFFFFFFu));
}

static void checkTCmnzCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(11);
  std::string source = std::string("it ") + cond.suffix + "\n" + "cmn" +
                       cond.suffix + " r0, r1\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tCMNz", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tCMNz", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0x7FFFFFFF;
  state->r1 = 1;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  QBDI::rword expectedFlags =
      taken ? (APSR_N | APSR_V) : nzcvForCondition(cond.cond, false);
  CHECK((finalState->cpsr & APSR_NZCV_MASK) == expectedFlags);
}

static void checkTCmphirCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(12);
  std::string source = std::string("it ") + cond.suffix + "\n" + "cmp" +
                       cond.suffix + " r0, r8\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tCMPhir", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tCMPhir", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0x80000000;
  state->r8 = 1;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  QBDI::rword expectedFlags =
      taken ? (APSR_C | APSR_V) : nzcvForCondition(cond.cond, false);
  CHECK((finalState->cpsr & APSR_NZCV_MASK) == expectedFlags);
}

static void checkTCmprCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(13);
  std::string source = std::string("it ") + cond.suffix + "\n" + "cmp" +
                       cond.suffix + " r0, r1\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tCMPr", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tCMPr", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0x80000000;
  state->r1 = 1;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  QBDI::rword expectedFlags =
      taken ? (APSR_C | APSR_V) : nzcvForCondition(cond.cond, false);
  CHECK((finalState->cpsr & APSR_NZCV_MASK) == expectedFlags);
}

static void checkTEorCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(14);
  std::string source = std::string("it ") + cond.suffix + "\n" + "eor" +
                       cond.suffix + " r0, r1\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tEOR", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tEOR", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0xFF00FF00;
  state->r1 = 0x0F0F0F0F;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? 0xF00FF00Fu : 0xFF00FF00u));
}

static void checkTLslriCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(15);
  std::string source = std::string("it ") + cond.suffix + "\n" + "lsl" +
                       cond.suffix + " r0, r1, #4\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tLSLri", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tLSLri", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0xdeadbeef;
  state->r1 = 0x0F0F0F0F;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? 0xF0F0F0F0u : 0xdeadbeefu));
}

static void checkTLslrrCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(16);
  std::string source = std::string("it ") + cond.suffix + "\n" + "lsl" +
                       cond.suffix + " r0, r1\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tLSLrr", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tLSLrr", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0x0F0F0F0F;
  state->r1 = 4;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? 0xF0F0F0F0u : 0x0F0F0F0Fu));
}

static void checkTLsrriCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(17);
  std::string source = std::string("it ") + cond.suffix + "\n" + "lsr" +
                       cond.suffix + " r0, r1, #4\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tLSRri", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tLSRri", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0xdeadbeef;
  state->r1 = 0xF0F0F0F0;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? 0x0F0F0F0Fu : 0xdeadbeefu));
}

static void checkTLsrrrCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(18);
  std::string source = std::string("it ") + cond.suffix + "\n" + "lsr" +
                       cond.suffix + " r0, r1\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tLSRrr", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tLSRrr", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0xF0F0F0F0;
  state->r1 = 4;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? 0x0F0F0F0Fu : 0xF0F0F0F0u));
}

static void checkTMovi8Cond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(19);
  std::string source = std::string("it ") + cond.suffix + "\n" + "mov" +
                       cond.suffix + " r0, #8\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tMOVi8", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tMOVi8", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? 8u : 0xdeadbeefu));
}

static void checkTMovrCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(20);
  std::string source = std::string("it ") + cond.suffix + "\n" + "mov" +
                       cond.suffix + " r0, r1\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tMOVr", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tMOVr", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0xdeadbeef;
  state->r1 = 0x12345678;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? 0x12345678u : 0xdeadbeefu));
}

static void checkTMulCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(21);
  std::string source = std::string("it ") + cond.suffix + "\n" + "mul" +
                       cond.suffix + " r0, r1\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tMUL", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tMUL", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 6;
  state->r1 = 7;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? 42u : 6u));
}

static void checkTMvnCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(22);
  std::string source = std::string("it ") + cond.suffix + "\n" + "mvn" +
                       cond.suffix + " r0, r1\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tMVN", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tMVN", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0xdeadbeef;
  state->r1 = 0x0F0F0F0F;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? 0xF0F0F0F0u : 0xdeadbeefu));
}

static void checkTOrrCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(23);
  std::string source = std::string("it ") + cond.suffix + "\n" + "orr" +
                       cond.suffix + " r0, r1\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tORR", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tORR", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0xF0F0F0F0;
  state->r1 = 0x0F0F0F0F;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? 0xFFFFFFFFu : 0xF0F0F0F0u));
}

static void checkTRevCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(24);
  std::string source = std::string("it ") + cond.suffix + "\n" + "rev" +
                       cond.suffix + " r0, r1\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tREV", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tREV", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0xdeadbeef;
  state->r1 = 0x12345678;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? 0x78563412u : 0xdeadbeefu));
}

static void checkTRev16Cond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(25);
  std::string source = std::string("it ") + cond.suffix + "\n" + "rev16" +
                       cond.suffix + " r0, r1\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tREV16", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tREV16", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0xdeadbeef;
  state->r1 = 0x12345678;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? 0x34127856u : 0xdeadbeefu));
}

static void checkTRevshCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(26);
  std::string source = std::string("it ") + cond.suffix + "\n" + "revsh" +
                       cond.suffix + " r0, r1\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tREVSH", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tREVSH", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0xdeadbeef;
  state->r1 = 0x1234FEDC;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? 0xFFFFDCFEu : 0xdeadbeefu));
}

static void checkTRorCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(27);
  std::string source = std::string("it ") + cond.suffix + "\n" + "ror" +
                       cond.suffix + " r0, r1\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tROR", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tROR", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 1;
  state->r1 = 1;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? 0x80000000u : 1u));
}

static void checkTRsbCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(28);
  std::string source = std::string("it ") + cond.suffix + "\n" + "rsb" +
                       cond.suffix + " r0, r1, #0\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tRSB", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tRSB", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0xdeadbeef;
  state->r1 = 5;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? 0xFFFFFFFBu : 0xdeadbeefu));
}

static void checkTSbcCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(29);
  std::string source = std::string("it ") + cond.suffix + "\n" + "sbc" +
                       cond.suffix + " r0, r1\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tSBC", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tSBC", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 20;
  state->r1 = 5;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword carry = (nzcvForCondition(cond.cond, taken) & APSR_C) ? 1u : 0u;

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? (14u + carry) : 20u));
}

static void checkTSubi3Cond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(30);
  std::string source = std::string("it ") + cond.suffix + "\n" + "sub" +
                       cond.suffix + " r0, r1, #3\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tSUBi3", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tSUBi3", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0xdeadbeef;
  state->r1 = 10;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? 7u : 0xdeadbeefu));
}

static void checkTSubi8Cond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(31);
  std::string source = std::string("it ") + cond.suffix + "\n" + "sub" +
                       cond.suffix + " r0, #8\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tSUBi8", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tSUBi8", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 20;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? 12u : 20u));
}

static void checkTSubrrCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(32);
  std::string source = std::string("it ") + cond.suffix + "\n" + "sub" +
                       cond.suffix + " r0, r1, r2\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tSUBrr", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tSUBrr", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0xdeadbeef;
  state->r1 = 20;
  state->r2 = 8;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? 12u : 0xdeadbeefu));
}

static void checkTSubspiCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(33);
  std::string source = std::string("it ") + cond.suffix + "\n" + "sub" +
                       cond.suffix + " sp, #4\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tSUBspi", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tSUBspi", QBDI::POSTINST, checkAccess,
                           &expectedPost);
  QBDI::rword spDuring = 0;
  fixture.vm.addMnemonicCB("tSUBspi", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             spDuring = gprState->sp;
                             return QBDI::VMAction::CONTINUE;
                           });

  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->sp == (taken ? (spDuring - 4) : spDuring));
}

static void checkTSxtbCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(34);
  std::string source = std::string("it ") + cond.suffix + "\n" + "sxtb" +
                       cond.suffix + " r0, r1\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tSXTB", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tSXTB", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0xdeadbeef;
  state->r1 = 0x000000F0;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? 0xFFFFFFF0u : 0xdeadbeefu));
}

static void checkTSxthCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(35);
  std::string source = std::string("it ") + cond.suffix + "\n" + "sxth" +
                       cond.suffix + " r0, r1\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tSXTH", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tSXTH", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0xdeadbeef;
  state->r1 = 0x0000F234;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? 0xFFFFF234u : 0xdeadbeefu));
}

static void checkTTstCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(36);
  std::string source = std::string("it ") + cond.suffix + "\n" + "tst" +
                       cond.suffix + " r0, r1\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tTST", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tTST", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0x80000000;
  state->r1 = 0xFFFFFFFF;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  QBDI::rword mask = APSR_N | APSR_Z;
  QBDI::rword expectedFlags =
      taken ? APSR_N : (nzcvForCondition(cond.cond, false) & mask);
  CHECK((finalState->cpsr & mask) == expectedFlags);
}

static void checkTUxtbCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(37);
  std::string source = std::string("it ") + cond.suffix + "\n" + "uxtb" +
                       cond.suffix + " r0, r1\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tUXTB", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tUXTB", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0xdeadbeef;
  state->r1 = 0xABCDEFF0;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? 0x000000F0u : 0xdeadbeefu));
}

static void checkTUxthCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(38);
  std::string source = std::string("it ") + cond.suffix + "\n" + "uxth" +
                       cond.suffix + " r0, r1\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("tUXTH", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("tUXTH", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0xdeadbeef;
  state->r1 = 0xABCDF234;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? 0x0000F234u : 0xdeadbeefu));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tadc_cond_taken") {
  checkTAdcCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tadc_cond_not_taken") {
  checkTAdcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-taddi3_cond_taken") {
  checkTAddi3Cond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-taddi3_cond_not_taken") {
  checkTAddi3Cond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-taddi8_cond_taken") {
  checkTAddi8Cond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-taddi8_cond_not_taken") {
  checkTAddi8Cond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-taddrsp_cond_taken") {
  checkTAddrSPCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-taddrsp_cond_not_taken") {
  checkTAddrSPCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-taddrspi_cond_taken") {
  checkTAddrSPiCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-taddrspi_cond_not_taken") {
  checkTAddrSPiCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-taddspi_cond_taken") {
  checkTAddspiCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-taddspi_cond_not_taken") {
  checkTAddspiCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-taddspr_cond_taken") {
  checkTAddsprCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-taddspr_cond_not_taken") {
  checkTAddsprCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tand_cond_taken") {
  checkTAndCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tand_cond_not_taken") {
  checkTAndCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tasrri_cond_taken") {
  checkTAsrriCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tasrri_cond_not_taken") {
  checkTAsrriCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tasrrr_cond_taken") {
  checkTAsrrrCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tasrrr_cond_not_taken") {
  checkTAsrrrCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tbic_cond_taken") {
  checkTBicCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tbic_cond_not_taken") {
  checkTBicCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tcmnz_cond_taken") {
  checkTCmnzCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tcmnz_cond_not_taken") {
  checkTCmnzCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tcmphir_cond_taken") {
  checkTCmphirCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tcmphir_cond_not_taken") {
  checkTCmphirCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tcmpr_cond_taken") {
  checkTCmprCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tcmpr_cond_not_taken") {
  checkTCmprCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-teor_cond_taken") {
  checkTEorCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-teor_cond_not_taken") {
  checkTEorCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tlslri_cond_taken") {
  checkTLslriCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tlslri_cond_not_taken") {
  checkTLslriCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tlslrr_cond_taken") {
  checkTLslrrCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tlslrr_cond_not_taken") {
  checkTLslrrCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tlsrri_cond_taken") {
  checkTLsrriCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tlsrri_cond_not_taken") {
  checkTLsrriCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tlsrrr_cond_taken") {
  checkTLsrrrCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tlsrrr_cond_not_taken") {
  checkTLsrrrCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tmovi8_cond_taken") {
  checkTMovi8Cond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tmovi8_cond_not_taken") {
  checkTMovi8Cond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tmovr_cond_taken") {
  checkTMovrCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tmovr_cond_not_taken") {
  checkTMovrCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tmul_cond_taken") {
  checkTMulCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tmul_cond_not_taken") {
  checkTMulCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tmvn_cond_taken") {
  checkTMvnCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tmvn_cond_not_taken") {
  checkTMvnCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-torr_cond_taken") {
  checkTOrrCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-torr_cond_not_taken") {
  checkTOrrCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-trev_cond_taken") {
  checkTRevCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-trev_cond_not_taken") {
  checkTRevCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-trev16_cond_taken") {
  checkTRev16Cond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-trev16_cond_not_taken") {
  checkTRev16Cond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-trevsh_cond_taken") {
  checkTRevshCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-trevsh_cond_not_taken") {
  checkTRevshCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tror_cond_taken") {
  checkTRorCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tror_cond_not_taken") {
  checkTRorCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-trsb_cond_taken") {
  checkTRsbCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-trsb_cond_not_taken") {
  checkTRsbCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tsbc_cond_taken") {
  checkTSbcCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tsbc_cond_not_taken") {
  checkTSbcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tsubi3_cond_taken") {
  checkTSubi3Cond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tsubi3_cond_not_taken") {
  checkTSubi3Cond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tsubi8_cond_taken") {
  checkTSubi8Cond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tsubi8_cond_not_taken") {
  checkTSubi8Cond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tsubrr_cond_taken") {
  checkTSubrrCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tsubrr_cond_not_taken") {
  checkTSubrrCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tsubspi_cond_taken") {
  checkTSubspiCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tsubspi_cond_not_taken") {
  checkTSubspiCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tsxtb_cond_taken") {
  checkTSxtbCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tsxtb_cond_not_taken") {
  checkTSxtbCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tsxth_cond_taken") {
  checkTSxthCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tsxth_cond_not_taken") {
  checkTSxthCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-ttst_cond_taken") {
  checkTTstCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-ttst_cond_not_taken") {
  checkTTstCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tuxtb_cond_taken") {
  checkTUxtbCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tuxtb_cond_not_taken") {
  checkTUxtbCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tuxth_cond_taken") {
  checkTUxthCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tuxth_cond_not_taken") {
  checkTUxthCond(*this, false);
}
