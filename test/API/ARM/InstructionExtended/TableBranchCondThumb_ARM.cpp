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

static void checkT2TbbCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(0);
  std::string source = std::string("it ") + cond.suffix + "\n" + "tbb" +
                       cond.suffix +
                       " [r0, r1]\n"
                       "b t2tbb_skip\n"
                       "t2tbb_table:\n"
                       "  .byte 0x02, 0x00\n"
                       "t2tbb_landed:\n"
                       "  movs r2, #0x22\n"
                       "  b t2tbb_end\n"
                       "t2tbb_skip:\n"
                       "  movs r2, #0x11\n"
                       "t2tbb_end:\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  QBDI::rword codeAddr = fixture.genASM(source.c_str(), QBDI::CPUMode::Thumb);
  QBDI::rword tableAddr = (codeAddr & ~(QBDI::rword)1) + 8;

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {
        {tableAddr, 0x02, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    };
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("t2TBB", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("t2TBB", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = tableAddr;
  state->r1 = 0;
  state->r2 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.vm.call(&retval, codeAddr);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r2 == (taken ? 0x22u : 0x11u));
}

static void checkT2TbhCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(1);
  std::string source = std::string("it ") + cond.suffix + "\n" + "tbh" +
                       cond.suffix +
                       " [r0, r1, lsl #1]\n"
                       "b t2tbh_skip\n"
                       "t2tbh_table:\n"
                       "  .short 0x0002\n"
                       "t2tbh_landed:\n"
                       "  movs r2, #0x22\n"
                       "  b t2tbh_end\n"
                       "t2tbh_skip:\n"
                       "  movs r2, #0x11\n"
                       "t2tbh_end:\n";

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  QBDI::rword codeAddr = fixture.genASM(source.c_str(), QBDI::CPUMode::Thumb);
  QBDI::rword tableAddr = (codeAddr & ~(QBDI::rword)1) + 8;

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {
        {tableAddr, 0x02, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    };
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("t2TBH", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("t2TBH", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = tableAddr;
  state->r1 = 0;
  state->r2 = 0xdeadbeef;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.vm.call(&retval, codeAddr);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r2 == (taken ? 0x22u : 0x11u));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2tbb_cond_taken") {
  checkT2TbbCond(*this, true);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2tbb_cond_not_taken") {
  checkT2TbbCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2tbh_cond_taken") {
  checkT2TbhCond(*this, true);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2tbh_cond_not_taken") {
  checkT2TbhCond(*this, false);
}
