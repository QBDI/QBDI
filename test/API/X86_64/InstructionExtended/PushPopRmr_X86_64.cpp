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

#include "MemAccessTestUtils_X86_64.h"

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PUSH16rmr") {
  const char source[] =
      "xchg %rsp, %rdx\n"
      // push %ax (FF /6 register-direct ModRM encoding of
      // PUSH16rmr; the assembler always prefers the
      // shorter 50+rd encoding for this mnemonic instead)
      ".byte 0x66,0xff,0xf0\n"
      "xchg %rsp, %rdx\n";
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
  state->rax = 0x5678;
  state->rdx = (QBDI::rword)&tmpStack[9];
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*reinterpret_cast<uint16_t *>(stackAddr) == 0x5678);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-POP16rmr") {
  const char source[] =
      "xchg %rsp, %rdx\n"
      // pop %ax (8F /0 register-direct ModRM encoding of
      // POP16rmr; the assembler always prefers the
      // shorter 58+rd encoding for this mnemonic instead)
      ".byte 0x66,0x8f,0xc0\n"
      "xchg %rsp, %rdx\n";
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
  state->rdx = (QBDI::rword)stackTop;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK((vm.getGPRState()->rax & 0xffff) == 0x9abc);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PUSH64rmr") {
  const char source[] =
      "xchg %rsp, %rdx\n"
      // push %rax (FF /6 register-direct ModRM encoding
      // of PUSH64rmr; the assembler always prefers the
      // shorter 50+rd encoding for this mnemonic instead)
      ".byte 0xff,0xf0\n"
      "xchg %rsp, %rdx\n";
  QBDI::rword tmpStack[10] = {0};
  QBDI::rword stackAddr = (QBDI::rword)&tmpStack[9] - 8;
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {stackAddr, 0x1122334455667788, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PUSH64rmr", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PUSH64rmr", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rax = 0x1122334455667788;
  state->rdx = (QBDI::rword)&tmpStack[9];
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*reinterpret_cast<uint64_t *>(stackAddr) == 0x1122334455667788);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-POP64rmr") {
  const char source[] =
      "xchg %rsp, %rdx\n"
      // pop %rax (8F /0 register-direct ModRM encoding
      // of POP64rmr; the assembler always prefers the
      // shorter 58+rd encoding for this mnemonic instead)
      ".byte 0x8f,0xc0\n"
      "xchg %rsp, %rdx\n";
  QBDI::rword tmpStack[10] = {0};
  uint64_t *stackTop = reinterpret_cast<uint64_t *>(&tmpStack[8]);
  *stackTop = 0x1122334455667788;
  QBDI::rword stackAddr = (QBDI::rword)stackTop;
  ExpectedMemoryAccesses expectedPre = {{
      {stackAddr, 0x1122334455667788, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("POP64rmr", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("POP64rmr", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rdx = (QBDI::rword)stackTop;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(vm.getGPRState()->rax == 0x1122334455667788);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}
