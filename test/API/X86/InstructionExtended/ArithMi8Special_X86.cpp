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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-ADC8mi8") {
  // adcb $0x5, 0x11(%ebx,%esi,4)
  const char source[] = ".byte 0x82,0x94,0xb3,0x11,0x00,0x00,0x00,0x05\n";
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
  vm.addMnemonicCB("ADC8mi8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADC8mi8", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x15);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-ADD8mi8") {
  // addb $0x5, 0x11(%ebx,%esi,4)
  const char source[] = ".byte 0x82,0x84,0xb3,0x11,0x00,0x00,0x00,0x05\n";
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
  vm.addMnemonicCB("ADD8mi8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADD8mi8", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x15);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-AND8mi8") {
  // andb $0xf, 0x11(%ebx,%esi,4)
  const char source[] = ".byte 0x82,0xa4,0xb3,0x11,0x00,0x00,0x00,0x0f\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0xf3;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0xf3, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0xf3, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x03, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("AND8mi8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("AND8mi8", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x03);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-CMP8mi8") {
  // cmpb $0x5, 0x11(%ebx,%esi,4)
  const char source[] = ".byte 0x82,0xbc,0xb3,0x11,0x00,0x00,0x00,0x05\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x30;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x30, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x30, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("CMP8mi8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("CMP8mi8", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x30);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-OR8mi8") {
  // orb $0x5, 0x11(%ebx,%esi,4)
  const char source[] = ".byte 0x82,0x8c,0xb3,0x11,0x00,0x00,0x00,0x05\n";
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
  vm.addMnemonicCB("OR8mi8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("OR8mi8", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x15);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-SUB8mi8") {
  // subb $0x5, 0x11(%ebx,%esi,4)
  const char source[] = ".byte 0x82,0xac,0xb3,0x11,0x00,0x00,0x00,0x05\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x30;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x30, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x30, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x2b, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SUB8mi8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SUB8mi8", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x2b);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-XOR8mi8") {
  // xorb $0xf, 0x11(%ebx,%esi,4)
  const char source[] = ".byte 0x82,0xb4,0xb3,0x11,0x00,0x00,0x00,0x0f\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x33;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x33, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x33, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x3c, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("XOR8mi8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("XOR8mi8", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x3c);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}
