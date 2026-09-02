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
using QBDITestBatch2::checkFeature;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PUSH2") {
  if (!checkFeature("push2pop2")) {
    return;
  }
  const char source[] =
      "xchg %rsp, %rdx\n"
      "push2 %rax, %rbx\n"
      "xchg %rsp, %rdx\n";
  alignas(16) QBDI::rword tmpStack[10] = {0};
  QBDI::rword stackAddr = (QBDI::rword)&tmpStack[8] - 16;
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {stackAddr, 0, 16, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PUSH2", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PUSH2", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rax = 0x1111111111111111ULL;
  state->rbx = 0x2222222222222222ULL;
  state->rdx = (QBDI::rword)&tmpStack[8];
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*reinterpret_cast<uint64_t *>(stackAddr) == 0x1111111111111111ULL);
  CHECK(*reinterpret_cast<uint64_t *>(stackAddr + 8) == 0x2222222222222222ULL);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PUSH2P") {
  if (!checkFeature("push2pop2")) {
    return;
  }
  const char source[] =
      "xchg %rsp, %rdx\n"
      "push2p %rax, %rbx\n"
      "xchg %rsp, %rdx\n";
  alignas(16) QBDI::rword tmpStack[10] = {0};
  QBDI::rword stackAddr = (QBDI::rword)&tmpStack[8] - 16;
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {stackAddr, 0, 16, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PUSH2P", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PUSH2P", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rax = 0x1111111111111111ULL;
  state->rbx = 0x2222222222222222ULL;
  state->rdx = (QBDI::rword)&tmpStack[8];
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*reinterpret_cast<uint64_t *>(stackAddr) == 0x1111111111111111ULL);
  CHECK(*reinterpret_cast<uint64_t *>(stackAddr + 8) == 0x2222222222222222ULL);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-POP2") {
  if (!checkFeature("push2pop2")) {
    return;
  }
  const char source[] =
      "xchg %rsp, %rdx\n"
      "pop2 %rax, %rbx\n"
      "xchg %rsp, %rdx\n";
  alignas(16) QBDI::rword tmpStack[10] = {0};
  uint64_t *stackTop = reinterpret_cast<uint64_t *>(&tmpStack[6]);
  stackTop[1] = 0x3333333333333333ULL;
  stackTop[0] = 0x4444444444444444ULL;
  QBDI::rword stackAddr = (QBDI::rword)stackTop;
  ExpectedMemoryAccesses expectedPre = {{
      {stackAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("POP2", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("POP2", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rdx = (QBDI::rword)stackTop;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(vm.getGPRState()->rax == 0x3333333333333333ULL);
  CHECK(vm.getGPRState()->rbx == 0x4444444444444444ULL);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-POP2P") {
  if (!checkFeature("push2pop2")) {
    return;
  }
  const char source[] =
      "xchg %rsp, %rdx\n"
      "pop2p %rax, %rbx\n"
      "xchg %rsp, %rdx\n";
  alignas(16) QBDI::rword tmpStack[10] = {0};
  uint64_t *stackTop = reinterpret_cast<uint64_t *>(&tmpStack[6]);
  stackTop[1] = 0x3333333333333333ULL;
  stackTop[0] = 0x4444444444444444ULL;
  QBDI::rword stackAddr = (QBDI::rword)stackTop;
  ExpectedMemoryAccesses expectedPre = {{
      {stackAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("POP2P", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("POP2P", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rdx = (QBDI::rword)stackTop;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(vm.getGPRState()->rax == 0x3333333333333333ULL);
  CHECK(vm.getGPRState()->rbx == 0x4444444444444444ULL);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PUSHP64r") {
  if (!checkFeature("ppx")) {
    return;
  }
  const char source[] =
      "xchg %rsp, %rdx\n"
      "pushp %rax\n"
      "xchg %rsp, %rdx\n";
  alignas(16) QBDI::rword tmpStack[10] = {0};
  QBDI::rword stackAddr = (QBDI::rword)&tmpStack[9] - 8;
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {stackAddr, 0x1122334455667788ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PUSHP64r", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PUSHP64r", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rax = 0x1122334455667788ULL;
  state->rdx = (QBDI::rword)&tmpStack[9];
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-POPP64r") {
  if (!checkFeature("ppx")) {
    return;
  }
  const char source[] =
      "xchg %rsp, %rdx\n"
      "popp %rax\n"
      "xchg %rsp, %rdx\n";
  alignas(16) QBDI::rword tmpStack[10] = {0};
  uint64_t *stackTop = reinterpret_cast<uint64_t *>(&tmpStack[8]);
  *stackTop = 0x99aabbccddeeff11ULL;
  QBDI::rword stackAddr = (QBDI::rword)stackTop;
  ExpectedMemoryAccesses expectedPre = {{
      {stackAddr, 0x99aabbccddeeff11ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("POPP64r", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("POPP64r", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rdx = (QBDI::rword)stackTop;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}
