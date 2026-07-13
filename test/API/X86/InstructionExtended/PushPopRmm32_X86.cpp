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

#include "MemAccessTestUtils_X86.h"

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::checkFeature;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-PUSH32rmm") {
  const char source[] =
      "xchg %esp, %edx\n"
      "pushl 0x11(%ebx,%esi,4)\n"
      "xchg %esp, %edx\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x12345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  QBDI::rword tmpStack[10] = {0};
  QBDI::rword stackAddr = (QBDI::rword)&tmpStack[9] - 4;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x12345678, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x12345678, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {stackAddr, 0x12345678, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PUSH32rmm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PUSH32rmm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->edx = (QBDI::rword)&tmpStack[9];
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*reinterpret_cast<uint32_t *>(stackAddr) == 0x12345678);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-POP32rmm") {
  const char source[] =
      "xchg %esp, %edx\n"
      "popl 0x11(%ebx,%esi,4)\n"
      "xchg %esp, %edx\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x00000000;
  QBDI::rword targetAddr = (QBDI::rword)target;
  QBDI::rword tmpStack[10] = {0};
  uint32_t *stackTop = reinterpret_cast<uint32_t *>(&tmpStack[8]);
  *stackTop = 0x87654321;
  QBDI::rword stackAddr = (QBDI::rword)stackTop;
  ExpectedMemoryAccesses expectedPre = {{
      {stackAddr, 0x87654321, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {stackAddr, 0x87654321, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x87654321, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("POP32rmm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("POP32rmm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->edx = (QBDI::rword)stackTop;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x87654321);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}
