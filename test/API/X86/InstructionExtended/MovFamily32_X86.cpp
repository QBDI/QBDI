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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-MOV32mi") {
  const char source[] = "movl $0x2a2a2a2a, 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x2a2a2a2a, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOV32mi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MOV32mi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x2a2a2a2a);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-MOV32mr") {
  const char source[] = "movl %eax, 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x2b2b2b2b, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOV32mr", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MOV32mr", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x2b2b2b2b;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x2b2b2b2b);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-MOV32rm") {
  const char source[] = "movl 0x11(%ebx,%esi,4), %eax\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x2c2c2c2c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x2c2c2c2c, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x2c2c2c2c, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOV32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MOV32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->eax & 0xffffffff) == 0x2c2c2c2c);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-MOVBE32rm") {
  if (!checkFeature("movbe")) {
    return;
  }
  const char source[] = "movbel 0x11(%ebx,%esi,4), %eax\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x11223344;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x11223344, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x11223344, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOVBE32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MOVBE32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->eax & 0xffffffff) == 0x44332211);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-MOVBE32mr") {
  if (!checkFeature("movbe")) {
    return;
  }
  const char source[] = "movbel %eax, 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x44332211, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOVBE32mr", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MOVBE32mr", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x11223344;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x44332211);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}
