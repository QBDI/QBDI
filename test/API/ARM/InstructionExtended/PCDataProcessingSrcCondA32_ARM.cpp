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
using QBDITestBatch2::conditionForIndex;
using QBDITestBatch2::ExpectedMemoryAccesses;
using QBDITestBatch2::nzcvForCondition;
using QBDITestBatch2::setConditionCPSR;

static constexpr QBDI::rword CPSR_N = (1u << 31);
static constexpr QBDI::rword CPSR_Z = (1u << 30);
static constexpr QBDI::rword CPSR_C = (1u << 29);
static constexpr QBDI::rword CPSR_V = (1u << 28);

struct NZCV {
  bool n, z, c, v;
};

static NZCV addFlags(uint32_t a, uint32_t b, bool carryIn) {
  uint64_t result = (uint64_t)a + (uint64_t)b + (carryIn ? 1u : 0u);
  uint32_t r32 = (uint32_t)result;
  bool c = (result >> 32) != 0;
  bool v = (((~(a ^ b)) & (a ^ r32)) & 0x80000000u) != 0;
  return {(r32 & 0x80000000u) != 0, r32 == 0, c, v};
}

static NZCV subFlags(uint32_t a, uint32_t b) { return addFlags(a, ~b, true); }

static NZCV logicFlags(uint32_t result, bool carryIn, bool overflowIn) {
  return {(result & 0x80000000u) != 0, result == 0, carryIn, overflowIn};
}

static void checkFlags(QBDI::rword cpsr, NZCV expected) {
  CHECK(((cpsr & CPSR_N) != 0) == expected.n);
  CHECK(((cpsr & CPSR_Z) != 0) == expected.z);
  CHECK(((cpsr & CPSR_C) != 0) == expected.c);
  CHECK(((cpsr & CPSR_V) != 0) == expected.v);
}

static NZCV fromCondBits(QBDI::rword bits) {
  return {(bits & QBDITestBatch2::APSR_N) != 0,
          (bits & QBDITestBatch2::APSR_Z) != 0,
          (bits & QBDITestBatch2::APSR_C) != 0,
          (bits & QBDITestBatch2::APSR_V) != 0};
}

static void checkCmnriCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(0);
  std::string source = std::string("cmn") + cond.suffix + " pc, #0x40000000\n";

  QBDI::rword pcVal = 0;
  bool seen = false;
  fixture.vm.addMnemonicCB(
      "CMNri", QBDI::PREINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        const QBDI::InstAnalysis *ia =
            vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
        pcVal = ia->address + 8;
        return QBDI::VMAction::CONTINUE;
      });
  fixture.vm.addMnemonicCB("CMNri", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             seen = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seen);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  NZCV expected = taken ? addFlags(pcVal, 0x40000000u, false)
                        : fromCondBits(nzcvForCondition(cond.cond, false));
  checkFlags(finalState->cpsr, expected);
}

static void checkCmpriCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(1);
  std::string source = std::string("cmp") + cond.suffix + " pc, #0x80000000\n";

  QBDI::rword pcVal = 0;
  bool seen = false;
  fixture.vm.addMnemonicCB(
      "CMPri", QBDI::PREINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        const QBDI::InstAnalysis *ia =
            vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
        pcVal = ia->address + 8;
        return QBDI::VMAction::CONTINUE;
      });
  fixture.vm.addMnemonicCB("CMPri", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             seen = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seen);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  NZCV expected = taken ? subFlags(pcVal, 0x80000000u)
                        : fromCondBits(nzcvForCondition(cond.cond, false));
  checkFlags(finalState->cpsr, expected);
}

static void checkTeqriCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(2);
  std::string source = std::string("teq") + cond.suffix + " pc, #4\n";

  QBDI::rword pcVal = 0;
  bool seen = false;
  fixture.vm.addMnemonicCB(
      "TEQri", QBDI::PREINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        const QBDI::InstAnalysis *ia =
            vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
        pcVal = ia->address + 8;
        return QBDI::VMAction::CONTINUE;
      });
  fixture.vm.addMnemonicCB("TEQri", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             seen = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seen);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  QBDI::rword takenBits = nzcvForCondition(cond.cond, true);
  NZCV expected =
      taken ? logicFlags(pcVal ^ 4u, (takenBits & QBDITestBatch2::APSR_C) != 0,
                         (takenBits & QBDITestBatch2::APSR_V) != 0)
            : fromCondBits(nzcvForCondition(cond.cond, false));
  checkFlags(finalState->cpsr, expected);
}

static void checkTstriCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(3);
  std::string source = std::string("tst") + cond.suffix + " pc, #4\n";

  QBDI::rword pcVal = 0;
  bool seen = false;
  fixture.vm.addMnemonicCB(
      "TSTri", QBDI::PREINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        const QBDI::InstAnalysis *ia =
            vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
        pcVal = ia->address + 8;
        return QBDI::VMAction::CONTINUE;
      });
  fixture.vm.addMnemonicCB("TSTri", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             seen = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seen);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  QBDI::rword takenBits = nzcvForCondition(cond.cond, true);
  NZCV expected =
      taken ? logicFlags(pcVal & 4u, (takenBits & QBDITestBatch2::APSR_C) != 0,
                         (takenBits & QBDITestBatch2::APSR_V) != 0)
            : fromCondBits(nzcvForCondition(cond.cond, false));
  checkFlags(finalState->cpsr, expected);
}

static void checkCmnzrrCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(4);
  std::string source = std::string("cmn") + cond.suffix + " pc, r1\n";

  QBDI::rword pcVal = 0;
  bool seen = false;
  fixture.vm.addMnemonicCB(
      "CMNzrr", QBDI::PREINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        const QBDI::InstAnalysis *ia =
            vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
        pcVal = ia->address + 8;
        return QBDI::VMAction::CONTINUE;
      });
  fixture.vm.addMnemonicCB("CMNzrr", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             seen = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r1 = 0x40000000;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seen);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  NZCV expected = taken ? addFlags(pcVal, 0x40000000u, false)
                        : fromCondBits(nzcvForCondition(cond.cond, false));
  checkFlags(finalState->cpsr, expected);
}

static void checkCmnzrsiCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(5);
  std::string source = std::string("cmn") + cond.suffix + " pc, r1, lsl #2\n";

  QBDI::rword pcVal = 0;
  bool seen = false;
  fixture.vm.addMnemonicCB(
      "CMNzrsi", QBDI::PREINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        const QBDI::InstAnalysis *ia =
            vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
        pcVal = ia->address + 8;
        return QBDI::VMAction::CONTINUE;
      });
  fixture.vm.addMnemonicCB("CMNzrsi", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             seen = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r1 = 0x10000000;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seen);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  NZCV expected = taken ? addFlags(pcVal, 0x10000000u << 2, false)
                        : fromCondBits(nzcvForCondition(cond.cond, false));
  checkFlags(finalState->cpsr, expected);
}

static void checkCmprrCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(6);
  std::string source = std::string("cmp") + cond.suffix + " pc, r1\n";

  QBDI::rword pcVal = 0;
  bool seen = false;
  fixture.vm.addMnemonicCB(
      "CMPrr", QBDI::PREINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        const QBDI::InstAnalysis *ia =
            vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
        pcVal = ia->address + 8;
        return QBDI::VMAction::CONTINUE;
      });
  fixture.vm.addMnemonicCB("CMPrr", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             seen = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r1 = 0x80000000;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seen);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  NZCV expected = taken ? subFlags(pcVal, 0x80000000u)
                        : fromCondBits(nzcvForCondition(cond.cond, false));
  checkFlags(finalState->cpsr, expected);
}

static void checkTeqrrCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(7);
  std::string source = std::string("teq") + cond.suffix + " pc, r1\n";

  QBDI::rword pcVal = 0;
  bool seen = false;
  fixture.vm.addMnemonicCB(
      "TEQrr", QBDI::PREINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        const QBDI::InstAnalysis *ia =
            vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
        pcVal = ia->address + 8;
        return QBDI::VMAction::CONTINUE;
      });
  fixture.vm.addMnemonicCB("TEQrr", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             seen = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r1 = 4;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seen);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  QBDI::rword takenBits = nzcvForCondition(cond.cond, true);
  NZCV expected =
      taken ? logicFlags(pcVal ^ 4u, (takenBits & QBDITestBatch2::APSR_C) != 0,
                         (takenBits & QBDITestBatch2::APSR_V) != 0)
            : fromCondBits(nzcvForCondition(cond.cond, false));
  checkFlags(finalState->cpsr, expected);
}

static void checkTstrrCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(8);
  std::string source = std::string("tst") + cond.suffix + " pc, r1\n";

  QBDI::rword pcVal = 0;
  bool seen = false;
  fixture.vm.addMnemonicCB(
      "TSTrr", QBDI::PREINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        const QBDI::InstAnalysis *ia =
            vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
        pcVal = ia->address + 8;
        return QBDI::VMAction::CONTINUE;
      });
  fixture.vm.addMnemonicCB("TSTrr", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             seen = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r1 = 4;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seen);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  QBDI::rword takenBits = nzcvForCondition(cond.cond, true);
  NZCV expected =
      taken ? logicFlags(pcVal & 4u, (takenBits & QBDITestBatch2::APSR_C) != 0,
                         (takenBits & QBDITestBatch2::APSR_V) != 0)
            : fromCondBits(nzcvForCondition(cond.cond, false));
  checkFlags(finalState->cpsr, expected);
}

static void checkStri12PcValueCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(9);
  QBDI::rword valBuf = 0xAAAAAAAA;
  std::string source =
      std::string("str") + cond.suffix + " pc, [r0, #4]\n" + "bx lr\n";
  QBDI::rword codeAddr = fixture.genASM(source.c_str());

  ExpectedMemoryAccesses expectedPre;
  ExpectedMemoryAccesses expectedPost;
  if (taken) {
    expectedPost.accesses.push_back({(QBDI::rword)&valBuf, codeAddr + 8, 4,
                                     QBDI::MEMORY_WRITE,
                                     QBDI::MEMORY_NO_FLAGS});
  }

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("STRi12", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("STRi12", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&valBuf - 4;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(valBuf == (taken ? codeAddr + 8 : 0xAAAAAAAAu));
}

static void checkStrrsPcValueCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(10);
  QBDI::rword valBuf = 0xAAAAAAAA;
  std::string source =
      std::string("str") + cond.suffix + " pc, [r0, r2]\n" + "bx lr\n";
  QBDI::rword codeAddr = fixture.genASM(source.c_str());

  ExpectedMemoryAccesses expectedPre;
  ExpectedMemoryAccesses expectedPost;
  if (taken) {
    expectedPost.accesses.push_back({(QBDI::rword)&valBuf, codeAddr + 8, 4,
                                     QBDI::MEMORY_WRITE,
                                     QBDI::MEMORY_NO_FLAGS});
  }

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("STRrs", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("STRrs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&valBuf - 4;
  state->r2 = 4;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(valBuf == (taken ? codeAddr + 8 : 0xAAAAAAAAu));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-cmnri_cond_taken") {
  checkCmnriCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-cmnri_cond_not_taken") {
  checkCmnriCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-cmpri_cond_taken") {
  checkCmpriCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-cmpri_cond_not_taken") {
  checkCmpriCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-teqri_cond_taken") {
  checkTeqriCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-teqri_cond_not_taken") {
  checkTeqriCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-tstri_cond_taken") {
  checkTstriCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-tstri_cond_not_taken") {
  checkTstriCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-cmnzrr_cond_taken") {
  checkCmnzrrCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-cmnzrr_cond_not_taken") {
  checkCmnzrrCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-cmnzrsi_cond_taken") {
  checkCmnzrsiCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-cmnzrsi_cond_not_taken") {
  checkCmnzrsiCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-cmprr_cond_taken") {
  checkCmprrCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-cmprr_cond_not_taken") {
  checkCmprrCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-teqrr_cond_taken") {
  checkTeqrrCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-teqrr_cond_not_taken") {
  checkTeqrrCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-tstrr_cond_taken") {
  checkTstrrCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-tstrr_cond_not_taken") {
  checkTstrrCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-stri12_pc_value_cond_taken") {
  checkStri12PcValueCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-stri12_pc_value_cond_not_taken") {
  checkStri12PcValueCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-strrs_pc_value_cond_taken") {
  checkStrrsPcValueCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-strrs_pc_value_cond_not_taken") {
  checkStrrsPcValueCond(*this, false);
}
