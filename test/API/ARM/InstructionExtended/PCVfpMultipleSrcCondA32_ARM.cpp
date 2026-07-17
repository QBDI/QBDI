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

static void checkVldmdiaPcCond(APITest &fixture, bool taken) {
  if (!checkFeature("vfp2")) {
    return;
  }
  const auto &cond = conditionForIndex(0);
  std::string source = std::string("vldmia") + cond.suffix +
                       " pc, {d0}\n"
                       "bx lr\n"
                       ".word 0x11223344\n"
                       ".word 0x55667788\n";
  QBDI::rword codeAddr = fixture.genASM(source.c_str());

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {
        {codeAddr + 8, 0x11223344, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        {codeAddr + 12, 0x55667788, 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
    };
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("VLDMDIA", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("VLDMDIA", QBDI::POSTINST, checkAccess,
                           &expectedPost);

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

static void checkVldmsiaPcCond(APITest &fixture, bool taken) {
  if (!checkFeature("vfp2")) {
    return;
  }
  const auto &cond = conditionForIndex(1);
  std::string source = std::string("vldmia") + cond.suffix +
                       " pc, {s0}\n"
                       "bx lr\n"
                       ".word 0x11223344\n";
  QBDI::rword codeAddr = fixture.genASM(source.c_str());

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {
        {codeAddr + 8, 0x11223344, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    };
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("VLDMSIA", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("VLDMSIA", QBDI::POSTINST, checkAccess,
                           &expectedPost);

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
  } else {
    CHECK(finalFpr->vreg.q[0][0] == 0xde);
    CHECK(finalFpr->vreg.q[0][1] == 0xad);
    CHECK(finalFpr->vreg.q[0][2] == 0xbe);
    CHECK(finalFpr->vreg.q[0][3] == 0xef);
  }
}

static void checkFldmxiaPcCond(APITest &fixture, bool taken) {
  if (!checkFeature("vfp2")) {
    return;
  }
  const auto &cond = conditionForIndex(2);
  std::string source = std::string("fldmiax") + cond.suffix +
                       " pc, {d0}\n"
                       "bx lr\n"
                       ".word 0x11223344\n"
                       ".word 0x55667788\n";
  QBDI::rword codeAddr = fixture.genASM(source.c_str());

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {
        {codeAddr + 8, 0x11223344, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        {codeAddr + 12, 0x55667788, 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
    };
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("FLDMXIA", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("FLDMXIA", QBDI::POSTINST, checkAccess,
                           &expectedPost);

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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vldmdia_pc_cond_taken") {
  checkVldmdiaPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-vldmdia_pc_cond_not_taken") {
  checkVldmdiaPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vldmsia_pc_cond_taken") {
  checkVldmsiaPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-vldmsia_pc_cond_not_taken") {
  checkVldmsiaPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-fldmxia_pc_cond_taken") {
  checkFldmxiaPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-fldmxia_pc_cond_not_taken") {
  checkFldmxiaPcCond(*this, false);
}
