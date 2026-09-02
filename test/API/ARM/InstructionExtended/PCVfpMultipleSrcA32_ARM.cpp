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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vldmdia_pc") {
  if (!checkFeature("vfp2")) {
    return;
  }
  const char source[] =
      "vldmia pc, {d0}\n"
      "bx lr\n"
      ".word 0x11223344\n"
      ".word 0x55667788\n";
  QBDI::rword codeAddr = genASM(source);

  ExpectedMemoryAccesses expectedPre = {{
      {codeAddr + 8, 0x11223344, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {codeAddr + 8 + 4, 0x55667788, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VLDMDIA", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLDMDIA", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  CHECK(fpr->vreg.q[0][0] == 0x44);
  CHECK(fpr->vreg.q[0][4] == 0x88);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-fldmxia_pc") {
  if (!checkFeature("vfp2")) {
    return;
  }
  const char source[] =
      "fldmiax pc, {d0}\n"
      "bx lr\n"
      ".word 0x11223344\n"
      ".word 0x55667788\n";
  QBDI::rword codeAddr = genASM(source);

  ExpectedMemoryAccesses expectedPre = {{
      {codeAddr + 8, 0x11223344, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {codeAddr + 8 + 4, 0x55667788, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("FLDMXIA", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("FLDMXIA", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  CHECK(fpr->vreg.q[0][0] == 0x44);
  CHECK(fpr->vreg.q[0][4] == 0x88);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vldmsia_pc") {
  if (!checkFeature("vfp2")) {
    return;
  }
  const char source[] =
      "vldmia pc, {s0}\n"
      "bx lr\n"
      ".word 0x11223344\n";
  QBDI::rword codeAddr = genASM(source);

  ExpectedMemoryAccesses expectedPre = {{
      {codeAddr + 8, 0x11223344, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VLDMSIA", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLDMSIA", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  CHECK(fpr->vreg.q[0][0] == 0x44);
}
