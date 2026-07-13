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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-BSF16rm") {
  const char source[] = "bsfw 0x11(%ebx,%esi,4), %ax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x28;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x28, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x28, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BSF16rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BSF16rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x28);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->eax & 0xffff) == 0x3);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-BSR16rm") {
  const char source[] = "bsrw 0x11(%ebx,%esi,4), %ax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x28;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x28, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x28, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BSR16rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BSR16rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x28);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->eax & 0xffff) == 0x5);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-DEC16m") {
  const char source[] = "decw 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x3030;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x3030, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x3030, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x302f, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("DEC16m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("DEC16m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x302f);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-DIV16m") {
  const char source[] = "divw 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x3;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x3, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x3, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("DIV16m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("DIV16m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0xa;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x3);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-IDIV16m") {
  const char source[] = "idivw 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x3;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x3, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x3, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IDIV16m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IDIV16m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0xa;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x3);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-IMUL16m") {
  const char source[] = "imulw 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x3;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x3, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x3, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IMUL16m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IMUL16m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0xa;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x3);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-IMUL16rm") {
  const char source[] = "imulw 0x11(%ebx,%esi,4), %ax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x5;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x5, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x5, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IMUL16rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IMUL16rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x5);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->eax & 0xffff) == 0xf);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-IMUL16rmi") {
  const char source[] = "imulw $0x1234, 0x11(%ebx,%esi,4), %ax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x5;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x5, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x5, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IMUL16rmi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IMUL16rmi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x5);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->eax & 0xffff) == 0x5b04);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-IMUL16rmi8") {
  const char source[] = "imulw $0x5, 0x11(%ebx,%esi,4), %ax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x5;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x5, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x5, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IMUL16rmi8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IMUL16rmi8", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x5);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->eax & 0xffff) == 0x19);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-INC16m") {
  const char source[] = "incw 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x3030;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x3030, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x3030, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x3031, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("INC16m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("INC16m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x3031);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-LZCNT16rm") {
  if (!checkFeature("lzcnt")) {
    return;
  }
  const char source[] = "lzcntw 0x11(%ebx,%esi,4), %ax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x28;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x28, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x28, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LZCNT16rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LZCNT16rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x28);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->eax & 0xffff) == 0xa);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-MUL16m") {
  const char source[] = "mulw 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x3;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x3, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x3, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MUL16m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MUL16m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0xa;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x3);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-NEG16m") {
  const char source[] = "negw 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x3030;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x3030, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x3030, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0xcfd0, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("NEG16m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("NEG16m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0xcfd0);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-NOT16m") {
  const char source[] = "notw 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x3030;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x3030, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x3030, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0xcfcf, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("NOT16m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("NOT16m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0xcfcf);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-POPCNT16rm") {
  if (!checkFeature("popcnt")) {
    return;
  }
  const char source[] = "popcntw 0x11(%ebx,%esi,4), %ax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x28;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x28, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x28, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("POPCNT16rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("POPCNT16rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x28);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->eax & 0xffff) == 0x2);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-TZCNT16rm") {
  if (!checkFeature("bmi")) {
    return;
  }
  const char source[] = "tzcntw 0x11(%ebx,%esi,4), %ax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x28;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x28, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x28, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("TZCNT16rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("TZCNT16rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x28);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->eax & 0xffff) == 0x3);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}
