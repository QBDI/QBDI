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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-DEC32m") {
  const char source[] = "decl 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x12345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x12345678, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x12345678, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x12345677, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("DEC32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("DEC32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x12345677);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-INC32m") {
  const char source[] = "incl 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x12345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x12345678, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x12345678, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x12345679, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("INC32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("INC32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x12345679);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-NEG32m") {
  const char source[] = "negl 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x12345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x12345678, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x12345678, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0xedcba988, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("NEG32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("NEG32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0xedcba988);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-NOT32m") {
  const char source[] = "notl 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x12345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x12345678, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x12345678, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0xedcba987, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("NOT32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("NOT32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0xedcba987);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-DIV32m") {
  const char source[] = "divl 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x00000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x00000003, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x00000003, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("DIV32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("DIV32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x0000000a;
  state->edx = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x00000003);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-IDIV32m") {
  const char source[] = "idivl 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x00000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x00000003, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x00000003, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IDIV32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IDIV32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x0000000a;
  state->edx = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x00000003);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-MUL32m") {
  const char source[] = "mull 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x00000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x00000003, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x00000003, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MUL32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MUL32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x0000000a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x00000003);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-IMUL32m") {
  const char source[] = "imull 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x00000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x00000003, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x00000003, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IMUL32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IMUL32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x0000000a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x00000003);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-IMUL32rm") {
  const char source[] = "imull 0x11(%ebx,%esi,4), %eax\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x00000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x00000003, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x00000003, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IMUL32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IMUL32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x0000000a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x00000003);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-IMUL32rmi") {
  const char source[] = "imull $0x5, 0x11(%ebx,%esi,4), %eax\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x00000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x00000003, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x00000003, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IMUL32rmi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IMUL32rmi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x00000003);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-IMUL32rmi8") {
  const char source[] = "imull $0x5, 0x11(%ebx,%esi,4), %eax\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x00000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x00000003, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x00000003, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IMUL32rmi8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IMUL32rmi8", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x00000003);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}
