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
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;
using QBDITestBatch2::setConditionCPSR;

static void checkLdrhPcCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(2);
  std::string source = std::string("ldrh") + cond.suffix +
                       " r1, [pc, #4]\n"
                       "bx lr\n"
                       ".word 0\n"
                       ".hword 0x1234\n"
                       ".hword 0\n";
  QBDI::rword codeAddr = fixture.genASM(source.c_str());

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {
        {codeAddr + 12, 0x1234, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    };
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("LDRH", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("LDRH", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r1 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  state = fixture.vm.getGPRState();
  CHECK(state->r1 == (taken ? (QBDI::rword)0x1234 : (QBDI::rword)0xdeadbeef));
}

static void checkLdrsbPcCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(3);
  std::string source = std::string("ldrsb") + cond.suffix +
                       " r1, [pc, #4]\n"
                       "bx lr\n"
                       ".word 0\n"
                       ".byte 0x92\n";
  QBDI::rword codeAddr = fixture.genASM(source.c_str());

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {
        {codeAddr + 12, 0x92, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    };
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("LDRSB", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("LDRSB", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r1 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  state = fixture.vm.getGPRState();
  CHECK(state->r1 ==
        (taken ? (QBDI::rword)(int32_t)(int8_t)0x92 : (QBDI::rword)0xdeadbeef));
}

static void checkLdrshPcCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(4);
  std::string source = std::string("ldrsh") + cond.suffix +
                       " r1, [pc, #4]\n"
                       "bx lr\n"
                       ".word 0\n"
                       ".hword 0x9234\n"
                       ".hword 0\n";
  QBDI::rword codeAddr = fixture.genASM(source.c_str());

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {
        {codeAddr + 12, 0x9234, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    };
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("LDRSH", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("LDRSH", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r1 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  state = fixture.vm.getGPRState();
  CHECK(state->r1 == (taken ? (QBDI::rword)(int32_t)(int16_t)0x9234
                            : (QBDI::rword)0xdeadbeef));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrh_pc_cond_taken") {
  checkLdrhPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrh_pc_cond_not_taken") {
  checkLdrhPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrsb_pc_cond_taken") {
  checkLdrsbPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrsb_pc_cond_not_taken") {
  checkLdrsbPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrsh_pc_cond_taken") {
  checkLdrshPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrsh_pc_cond_not_taken") {
  checkLdrshPcCond(*this, false);
}
