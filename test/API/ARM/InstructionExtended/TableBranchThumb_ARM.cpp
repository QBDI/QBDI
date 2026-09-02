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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2tbb") {
  const char source[] =
      "tbb [r0, r1]\n"
      "  .byte 0x01, 0x00\n"
      "  mov r3, #0xaa\n";

  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb);
  QBDI::rword tableAddr = (codeAddr & ~(QBDI::rword)1) + 4;
  QBDI::rword expectedTarget = tableAddr + 2;

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = tableAddr;
  state->r1 = 0;
  vm.setGPRState(state);

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{
      {{tableAddr, 1, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS}}};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("t2TBB", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2TBB", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("t2TBB", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->pc & ~(QBDI::rword)1) == expectedTarget);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(vm.getGPRState()->r3 == 0xaa);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2tbh") {
  const char source[] =
      "tbh [r0, r1, lsl #1]\n"
      "  .short 0x0001\n"
      "  mov r3, #0xaa\n";

  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb);
  QBDI::rword tableAddr = (codeAddr & ~(QBDI::rword)1) + 4;
  QBDI::rword expectedTarget = tableAddr + 2;

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = tableAddr;
  state->r1 = 0;
  vm.setGPRState(state);

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{
      {{tableAddr, 1, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS}}};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("t2TBH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2TBH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("t2TBH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->pc & ~(QBDI::rword)1) == expectedTarget);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(vm.getGPRState()->r3 == 0xaa);
}
