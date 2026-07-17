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

static void checkVldrdPcCond(APITest &fixture, bool taken) {
  if (!checkFeature("vfp2")) {
    return;
  }
  const auto &cond = conditionForIndex(5);
  std::string source = std::string("vldr") + cond.suffix +
                       " d0, [pc, #8]\n"
                       "bx lr\n"
                       ".word 0\n"
                       ".word 0\n"
                       ".word 0x11223344\n"
                       ".word 0x55667788\n";
  QBDI::rword codeAddr = fixture.genASM(source.c_str());

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {
        {codeAddr + 16, 0x11223344, 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
        {codeAddr + 20, 0x55667788, 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
    };
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("VLDRD", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("VLDRD", QBDI::POSTINST, checkAccess, &expectedPost);

  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::FPRState *fpr = fixture.vm.getFPRState();
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][i] = 0xa0 + i;
  }
  fixture.vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = fixture.vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::FPRState *finalFpr = fixture.vm.getFPRState();
  if (taken) {
    CHECK(finalFpr->vreg.q[0][0] == 0x44);
    CHECK(finalFpr->vreg.q[0][4] == 0x88);
  } else {
    for (int i = 0; i < 8; i++) {
      CHECK(finalFpr->vreg.q[0][i] == (uint8_t)(0xa0 + i));
    }
  }
}

static void checkVldrsPcCond(APITest &fixture, bool taken) {
  if (!checkFeature("vfp2")) {
    return;
  }
  const auto &cond = conditionForIndex(6);
  std::string source = std::string("vldr") + cond.suffix +
                       " s0, [pc, #8]\n"
                       "bx lr\n"
                       ".word 0\n"
                       ".word 0\n"
                       ".word 0x11223344\n";
  QBDI::rword codeAddr = fixture.genASM(source.c_str());

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {
        {codeAddr + 16, 0x11223344, 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
    };
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("VLDRS", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("VLDRS", QBDI::POSTINST, checkAccess, &expectedPost);

  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::FPRState *fpr = fixture.vm.getFPRState();
  fpr->vreg.q[0][0] = 0xde;
  fpr->vreg.q[0][1] = 0xad;
  fpr->vreg.q[0][2] = 0xbe;
  fpr->vreg.q[0][3] = 0xef;
  fixture.vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = fixture.vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::FPRState *finalFpr = fixture.vm.getFPRState();
  if (taken) {
    CHECK(finalFpr->vreg.q[0][0] == 0x44);
    CHECK(finalFpr->vreg.q[0][3] == 0x11);
  } else {
    CHECK(finalFpr->vreg.q[0][0] == 0xde);
    CHECK(finalFpr->vreg.q[0][1] == 0xad);
    CHECK(finalFpr->vreg.q[0][2] == 0xbe);
    CHECK(finalFpr->vreg.q[0][3] == 0xef);
  }
}

static void checkVldrhPcCond(APITest &fixture, bool taken) {
  if (!checkFeature("fullfp16")) {
    return;
  }
  const auto &cond = conditionForIndex(7);
  std::string source = std::string("vldr") + cond.suffix +
                       ".16 s0, [pc, #8]\n"
                       "bx lr\n"
                       ".word 0\n"
                       ".word 0\n"
                       ".hword 0x1234\n"
                       ".hword 0\n";
  QBDI::rword codeAddr =
      fixture.genASM(source.c_str(), QBDI::CPUMode::ARM, {"fullfp16"});

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {
        {codeAddr + 16, 0x1234, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    };
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("VLDRH", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("VLDRH", QBDI::POSTINST, checkAccess, &expectedPost);

  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::FPRState *fpr = fixture.vm.getFPRState();
  fpr->vreg.s[0] = 0.0f;
  fixture.vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = fixture.vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::FPRState *finalFpr = fixture.vm.getFPRState();
  if (taken) {
    CHECK(finalFpr->vreg.q[0][0] == 0x34);
    CHECK(finalFpr->vreg.q[0][1] == 0x12);
  } else {
    CHECK(finalFpr->vreg.s[0] == 0.0f);
  }
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vldrd_pc_cond_taken") {
  checkVldrdPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-vldrd_pc_cond_not_taken") {
  checkVldrdPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vldrs_pc_cond_taken") {
  checkVldrsPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-vldrs_pc_cond_not_taken") {
  checkVldrsPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vldrh_pc_cond_taken") {
  checkVldrhPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-vldrh_pc_cond_not_taken") {
  checkVldrhPcCond(*this, false);
}
