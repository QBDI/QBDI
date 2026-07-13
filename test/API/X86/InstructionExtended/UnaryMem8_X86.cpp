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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-DEC8m") {
  const char source[] = "decb 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x11;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x10, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("DEC8m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("DEC8m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x10);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-INC8m") {
  const char source[] = "incb 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x11;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x12, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("INC8m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("INC8m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x12);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-NEG8m") {
  const char source[] = "negb 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x11;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0xef, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("NEG8m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("NEG8m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0xef);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-NOT8m") {
  const char source[] = "notb 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x11;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0xee, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("NOT8m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("NOT8m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0xee);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-DIV8m") {
  const char source[] = "divb 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x03;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("DIV8m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("DIV8m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x0a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x03);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-IDIV8m") {
  const char source[] = "idivb 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x03;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IDIV8m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IDIV8m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x0a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x03);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-IMUL8m") {
  const char source[] = "imulb 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x03;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IMUL8m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IMUL8m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x0a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x03);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-MUL8m") {
  const char source[] = "mulb 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x03;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MUL8m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MUL8m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x0a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x03);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}
