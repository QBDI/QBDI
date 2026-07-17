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
#include <catch2/catch_test_macros.hpp>
#include "API/APITest.h"

#include "MemAccessTestUtils_ARM.h"

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::checkFeature;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vldrd_pc") {
  if (!checkFeature("vfp2")) {
    return;
  }
  const char source[] =
      "vldr d0, [pc, #8]\n"
      "bx lr\n"
      ".word 0\n"
      ".word 0\n"
      ".word 0x11223344\n"
      ".word 0x55667788\n";
  QBDI::rword codeAddr = genASM(source);

  ExpectedMemoryAccesses expectedPre = {{
      {codeAddr + 16, 0x11223344, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {codeAddr + 20, 0x55667788, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VLDRD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLDRD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  CHECK(fpr->vreg.q[0][0] == 0x44);
  CHECK(fpr->vreg.q[0][4] == 0x88);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vldrs_pc") {
  if (!checkFeature("vfp2")) {
    return;
  }
  const char source[] =
      "vldr s0, [pc, #8]\n"
      "bx lr\n"
      ".word 0\n"
      ".word 0\n"
      ".word 0x11223344\n";
  QBDI::rword codeAddr = genASM(source);

  ExpectedMemoryAccesses expectedPre = {{
      {codeAddr + 16, 0x11223344, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VLDRS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLDRS", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  CHECK(fpr->vreg.q[0][0] == 0x44);
  CHECK(fpr->vreg.q[0][3] == 0x11);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vldrh_pc") {
  if (!checkFeature("fullfp16")) {
    return;
  }
  const char source[] =
      "vldr.16 s0, [pc, #8]\n"
      "bx lr\n"
      ".word 0\n"
      ".word 0\n"
      ".hword 0x1234\n"
      ".hword 0\n";
  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::ARM, {"fullfp16"});

  ExpectedMemoryAccesses expectedPre = {{
      {codeAddr + 16, 0x1234, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VLDRH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLDRH", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  CHECK(fpr->vreg.q[0][0] == 0x34);
  CHECK(fpr->vreg.q[0][1] == 0x12);
}
