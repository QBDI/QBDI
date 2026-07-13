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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-CMPXCHG8rm") {
  const char source[] = "cmpxchgb %bl, 0x11(%edx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x11;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x99, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("CMPXCHG8rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("CMPXCHG8rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->edx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x11;
  state->ebx = 0x99;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x99);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-XADD8rm") {
  const char source[] = "xaddb %bl, 0x11(%edx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x10;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x10, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x10, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x15, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("XADD8rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("XADD8rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->edx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->ebx = 0x05;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x15);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->ebx & 0xff) == 0x10);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-XCHG8rm") {
  const char source[] = "xchgb %bl, 0x11(%edx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x11;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x22, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("XCHG8rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("XCHG8rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->edx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->ebx = 0x22;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x22);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->ebx & 0xff) == 0x11);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}
