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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vldrs") {
  const char source[] = "vldr s0, [r0, #4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8b8a8988, 4,
                           QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("VLDRS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLDRS", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::FPRState *finalFpr = vm.getFPRState();
  CHECK(finalFpr->vreg.q[0][0] == 0x88);
  CHECK(finalFpr->vreg.q[0][1] == 0x89);
  CHECK(finalFpr->vreg.q[0][2] == 0x8a);
  CHECK(finalFpr->vreg.q[0][3] == 0x8b);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vstrs") {
  const char source[] = "vstr s0, [r0, #4]\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xaabbccdd, 4,
                            QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  vm.addMnemonicCB("VSTRS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VSTRS", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->vreg.q[0][0] = 0xdd;
  fpr->vreg.q[0][1] = 0xcc;
  fpr->vreg.q[0][2] = 0xbb;
  fpr->vreg.q[0][3] = 0xaa;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[4] == 0xaabbccdd);
}
