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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-CMOV16rm") {
  const char source[] = "cmovew 0x11(%ebx,%esi,4), %ax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x1234;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x1234, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x1234, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("CMOV16rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("CMOV16rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x5678;
  state->eflags |= 1 << 6;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->eax & 0xffff) == 0x1234);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-LAR16rm") {
  const char source[] = "larw 0x11(%ebx,%esi,4), %ax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x0000;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LAR16rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LAR16rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-LAR32rm") {
  const char source[] = "larl 0x11(%ebx,%esi,4), %eax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x0000;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LAR32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LAR32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-LSL16rm") {
  const char source[] = "lslw 0x11(%ebx,%esi,4), %ax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x0000;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LSL16rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LSL16rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-LSL32rm") {
  const char source[] = "lsll 0x11(%ebx,%esi,4), %eax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x0000;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LSL32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LSL32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}
