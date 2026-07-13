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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-ADC8rm") {
  const char source[] = "adcb 0x11(%ebx,%esi,4), %al\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x05;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADC8rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADC8rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x10;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x05);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->eax & 0xff) == 0x15);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-ADD8rm") {
  const char source[] = "addb 0x11(%ebx,%esi,4), %al\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x05;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADD8rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADD8rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x10;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x05);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->eax & 0xff) == 0x15);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-AND8rm") {
  const char source[] = "andb 0x11(%ebx,%esi,4), %al\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x0f;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0f, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0f, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("AND8rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("AND8rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0xf3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x0f);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->eax & 0xff) == 0x03);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-OR8rm") {
  const char source[] = "orb 0x11(%ebx,%esi,4), %al\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x05;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("OR8rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("OR8rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x10;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x05);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->eax & 0xff) == 0x15);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-SBB8rm") {
  const char source[] = "sbbb 0x11(%ebx,%esi,4), %al\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x05;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SBB8rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SBB8rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x30;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x05);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->eax & 0xff) == 0x2b);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-SUB8rm") {
  const char source[] = "subb 0x11(%ebx,%esi,4), %al\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x05;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SUB8rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SUB8rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x30;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x05);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->eax & 0xff) == 0x2b);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-XOR8rm") {
  const char source[] = "xorb 0x11(%ebx,%esi,4), %al\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x0f;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0f, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0f, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("XOR8rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("XOR8rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x33;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x0f);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->eax & 0xff) == 0x3c);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}
