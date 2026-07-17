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
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrh_pc") {
  const char source[] =
      "ldrh r1, [pc, #4]\n"
      "bx lr\n"
      ".word 0\n"
      ".hword 0x1234\n"
      ".hword 0\n";
  QBDI::rword codeAddr = genASM(source);

  ExpectedMemoryAccesses expectedPre = {{
      {codeAddr + 12, 0x1234, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRH", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x1234);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrsb_pc") {
  const char source[] =
      "ldrsb r1, [pc, #4]\n"
      "bx lr\n"
      ".word 0\n"
      ".byte 0x92\n";
  QBDI::rword codeAddr = genASM(source);

  ExpectedMemoryAccesses expectedPre = {{
      {codeAddr + 12, 0x92, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRSB", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRSB", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == (QBDI::rword)(int32_t)(int8_t)0x92);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrsh_pc") {
  const char source[] =
      "ldrsh r1, [pc, #4]\n"
      "bx lr\n"
      ".word 0\n"
      ".hword 0x9234\n"
      ".hword 0\n";
  QBDI::rword codeAddr = genASM(source);

  ExpectedMemoryAccesses expectedPre = {{
      {codeAddr + 12, 0x9234, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRSH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRSH", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == (QBDI::rword)(int32_t)(int16_t)0x9234);
}
