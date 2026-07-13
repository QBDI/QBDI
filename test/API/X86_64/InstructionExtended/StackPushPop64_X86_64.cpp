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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PUSH64i32") {
  const char source[] =
      "xchg %rsp, %rdx\n"
      "pushq $0x11223344\n"
      "xchg %rsp, %rdx\n";
  QBDI::rword tmpStack[10] = {0};
  QBDI::rword stackAddr = (QBDI::rword)&tmpStack[9] - 8;
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {stackAddr, 0x11223344ULL, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PUSH64i32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PUSH64i32", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rdx = (QBDI::rword)&tmpStack[9];
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*reinterpret_cast<uint64_t *>(stackAddr) == 0x11223344ULL);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PUSH64i8") {
  const char source[] =
      "xchg %rsp, %rdx\n"
      "pushq $0x5\n"
      "xchg %rsp, %rdx\n";
  QBDI::rword tmpStack[10] = {0};
  QBDI::rword stackAddr = (QBDI::rword)&tmpStack[9] - 8;
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {stackAddr, 0x5, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PUSH64i8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PUSH64i8", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rdx = (QBDI::rword)&tmpStack[9];
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*reinterpret_cast<uint64_t *>(stackAddr) == 0x5);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PUSH64r") {
  const char source[] =
      "xchg %rsp, %rdx\n"
      "pushq %rax\n"
      "xchg %rsp, %rdx\n";
  QBDI::rword tmpStack[10] = {0};
  QBDI::rword stackAddr = (QBDI::rword)&tmpStack[9] - 8;
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {stackAddr, 0x1122334455667788ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PUSH64r", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PUSH64r", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rax = 0x1122334455667788ULL;
  state->rdx = (QBDI::rword)&tmpStack[9];
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*reinterpret_cast<uint64_t *>(stackAddr) == 0x1122334455667788ULL);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PUSHF64") {
  const char source[] =
      "xchg %rsp, %rdx\n"
      "pushfq\n"
      "xchg %rsp, %rdx\n";
  QBDI::rword tmpStack[10] = {0};
  QBDI::rword stackAddr = (QBDI::rword)&tmpStack[9] - 8;
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {stackAddr, 0x0246, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PUSHF64", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PUSHF64", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->eflags = 0x0246;
  state->rdx = (QBDI::rword)&tmpStack[9];
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-POP64r") {
  const char source[] =
      "xchg %rsp, %rdx\n"
      "popq %rax\n"
      "xchg %rsp, %rdx\n";
  QBDI::rword tmpStack[10] = {0};
  uint64_t *stackTop = reinterpret_cast<uint64_t *>(&tmpStack[8]);
  *stackTop = 0x99aabbccddeeff11ULL;
  QBDI::rword stackAddr = (QBDI::rword)stackTop;
  ExpectedMemoryAccesses expectedPre = {{
      {stackAddr, 0x99aabbccddeeff11ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("POP64r", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("POP64r", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rdx = (QBDI::rword)stackTop;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-POPF64") {
  const char source[] =
      "xchg %rsp, %rdx\n"
      "popfq\n"
      "xchg %rsp, %rdx\n";
  QBDI::rword tmpStack[10] = {0};
  uint64_t *stackTop = reinterpret_cast<uint64_t *>(&tmpStack[8]);
  *stackTop = 0x0246;
  QBDI::rword stackAddr = (QBDI::rword)stackTop;
  ExpectedMemoryAccesses expectedPre = {{
      {stackAddr, 0x0246, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("POPF64", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("POPF64", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rdx = (QBDI::rword)stackTop;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}
