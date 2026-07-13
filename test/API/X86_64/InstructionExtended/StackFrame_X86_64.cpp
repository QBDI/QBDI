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

#include "MemAccessTestUtils_X86_64.h"

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ENTER") {
  const char source[] =
      "xchg %rsp, %rdx\n"
      "enter $8, $0\n"
      "leave\n"
      "xchg %rsp, %rdx\n";
  QBDI::rword tmpStack[10] = {0};
  QBDI::rword stackAddr = (QBDI::rword)&tmpStack[9] - 8;
  constexpr uint64_t oldRbp = 0x99aabbccddeeff11ULL;
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {stackAddr, oldRbp, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ENTER", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ENTER", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbp = oldRbp;
  state->rdx = (QBDI::rword)&tmpStack[9];
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-LEAVE64") {
  const char source[] =
      "xchg %rsp, %rdx\n"
      "leave\n"
      "xchg %rsp, %rdx\n";
  QBDI::rword tmpStack[10] = {0};
  uint64_t *savedRbpSlot = reinterpret_cast<uint64_t *>(&tmpStack[8]);
  constexpr uint64_t savedRbp = 0x1122334455667788ULL;
  *savedRbpSlot = savedRbp;
  QBDI::rword rbpAddr = (QBDI::rword)savedRbpSlot;
  ExpectedMemoryAccesses expectedPre = {{
      {rbpAddr, savedRbp, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LEAVE64", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LEAVE64", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbp = rbpAddr;
  state->rdx = (QBDI::rword)&tmpStack[9];
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}
