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
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-PUSH16rmr") {
  const char source[] =
      "xchg %esp, %edx\n"
      // pushw %ax (FF /6 register-direct ModRM encoding of
      // PUSH16rmr; the assembler always prefers the
      // shorter 50+rd encoding for this mnemonic instead)
      ".byte 0x66,0xff,0xf0\n"
      "xchg %esp, %edx\n";
  QBDI::rword tmpStack[10] = {0};
  QBDI::rword stackAddr = (QBDI::rword)&tmpStack[9] - 2;
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {stackAddr, 0x5678, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PUSH16rmr", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PUSH16rmr", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->eax = 0x5678;
  state->edx = (QBDI::rword)&tmpStack[9];
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*reinterpret_cast<uint16_t *>(stackAddr) == 0x5678);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-POP16rmr") {
  const char source[] =
      "xchg %esp, %edx\n"
      // popw %ax (8F /0 register-direct ModRM encoding of
      // POP16rmr; the assembler always prefers the
      // shorter 58+rd encoding for this mnemonic instead)
      ".byte 0x66,0x8f,0xc0\n"
      "xchg %esp, %edx\n";
  QBDI::rword tmpStack[10] = {0};
  uint16_t *stackTop = reinterpret_cast<uint16_t *>(&tmpStack[8]);
  *stackTop = 0x9abc;
  QBDI::rword stackAddr = (QBDI::rword)stackTop;
  ExpectedMemoryAccesses expectedPre = {{
      {stackAddr, 0x9abc, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("POP16rmr", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("POP16rmr", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->edx = (QBDI::rword)stackTop;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK((vm.getGPRState()->eax & 0xffff) == 0x9abc);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-PUSH32rmr") {
  const char source[] =
      "xchg %esp, %edx\n"
      // pushl %eax (FF /6 register-direct ModRM encoding
      // of PUSH32rmr; the assembler always prefers the
      // shorter 50+rd encoding for this mnemonic instead)
      ".byte 0xff,0xf0\n"
      "xchg %esp, %edx\n";
  QBDI::rword tmpStack[10] = {0};
  QBDI::rword stackAddr = (QBDI::rword)&tmpStack[9] - 4;
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {stackAddr, 0x11223344, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PUSH32rmr", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PUSH32rmr", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->eax = 0x11223344;
  state->edx = (QBDI::rword)&tmpStack[9];
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*reinterpret_cast<uint32_t *>(stackAddr) == 0x11223344);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-POP32rmr") {
  const char source[] =
      "xchg %esp, %edx\n"
      // popl %eax (8F /0 register-direct ModRM encoding
      // of POP32rmr; the assembler always prefers the
      // shorter 58+rd encoding for this mnemonic instead)
      ".byte 0x8f,0xc0\n"
      "xchg %esp, %edx\n";
  QBDI::rword tmpStack[10] = {0};
  uint32_t *stackTop = reinterpret_cast<uint32_t *>(&tmpStack[8]);
  *stackTop = 0x11223344;
  QBDI::rword stackAddr = (QBDI::rword)stackTop;
  ExpectedMemoryAccesses expectedPre = {{
      {stackAddr, 0x11223344, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("POP32rmr", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("POP32rmr", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->edx = (QBDI::rword)stackTop;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(vm.getGPRState()->eax == 0x11223344);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}
