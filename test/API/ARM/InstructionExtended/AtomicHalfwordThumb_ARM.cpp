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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldah") {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const char source[] = "ldah r1, [r0]\n";

  constexpr uint16_t expected = 0x1234;
  uint16_t v = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {
      {(QBDI::rword)&v, expected, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("t2LDAH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDAH", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb, {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == expected);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2stlh") {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const char source[] = "stlh r1, [r0]\n";

  uint16_t v = 0x1010;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {
      {(QBDI::rword)&v, 0xabcd, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  vm.addMnemonicCB("t2STLH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2STLH", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r1 = 0xabcd;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb, {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == 0xabcd);
}
