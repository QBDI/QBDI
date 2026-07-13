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
using QBDITestBatch2::checkEmptyAccess;
using QBDITestBatch2::checkFeature;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PUSH64rmm") {
  const char source[] =
      "xchg %rsp, %rdx\n"
      "pushq 0x11(%rbx,%rsi,4)\n"
      "xchg %rsp, %rdx\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x1122334455667788ULL;
  QBDI::rword targetAddr = (QBDI::rword)target;
  QBDI::rword tmpStack[10] = {0};
  QBDI::rword stackAddr = (QBDI::rword)&tmpStack[9] - 8;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x1122334455667788ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x1122334455667788ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {stackAddr, 0x1122334455667788ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PUSH64rmm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PUSH64rmm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rdx = (QBDI::rword)&tmpStack[9];
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*reinterpret_cast<uint64_t *>(stackAddr) == 0x1122334455667788ULL);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-POP64rmm") {
  const char source[] =
      "xchg %rsp, %rdx\n"
      "popq 0x11(%rbx,%rsi,4)\n"
      "xchg %rsp, %rdx\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0;
  QBDI::rword targetAddr = (QBDI::rword)target;
  QBDI::rword tmpStack[10] = {0};
  uint64_t *stackTop = reinterpret_cast<uint64_t *>(&tmpStack[8]);
  *stackTop = 0x5566778899001122ULL;
  QBDI::rword stackAddr = (QBDI::rword)stackTop;
  ExpectedMemoryAccesses expectedPre = {{
      {stackAddr, 0x5566778899001122ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {stackAddr, 0x5566778899001122ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x5566778899001122ULL, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("POP64rmm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("POP64rmm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rdx = stackAddr;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x5566778899001122ULL);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-JMP64m") {
  const char source[] =
      "lea mem_jmp_landed(%rip), %rdx\n"
      "movq %rdx, 0x10(%rax,%rcx,8)\n"
      "jmp *0x10(%rax,%rcx,8)\n"
      "movq $0x1111, %rax\n"
      "jmp mem_jmp_over\n"
      "mem_jmp_landed:\n"
      "movq $0x2222, %rax\n"
      "mem_jmp_over:\n";
  QBDI::rword buffer[8] = {0};
  bool seenJumpPre = false, seenJumpPost = false;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[3];
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("JMP64m", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == targetAddr);
                     CHECK(accesses[0].size == 8);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     seenJumpPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("JMP64m", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == targetAddr);
                     CHECK(accesses[0].size == 8);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     seenJumpPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  QBDI::GPRState *state = vm.getGPRState();
  state->rax = (QBDI::rword)&buffer[0];
  state->rcx = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(seenJumpPre);
  CHECK(seenJumpPost);
  CHECK(retval == 0x2222);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-CALL64m") {
  const char source[] =
      "lea mem_call_callee(%rip), %rdx\n"
      "movq %rdx, 0x10(%rax,%rcx,8)\n"
      "call *0x10(%rax,%rcx,8)\n"
      "jmp mem_call_end\n"
      "mem_call_callee:\n"
      "movq $0x3333, %rax\n"
      "ret\n"
      "mem_call_end:\n";
  QBDI::rword buffer[8] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[3];
  QBDI::rword preCallRsp = 0;
  QBDI::rword expectedReturnAddr = 0;
  bool seenCallPre = false, seenCallPost = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("CALL64m", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == targetAddr);
                     CHECK(accesses[0].size == 8);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     preCallRsp = gprState->rsp;
                     expectedReturnAddr = ia->address + ia->instSize;
                     seenCallPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("CALL64m", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 2);
                     CHECK(accesses[0].accessAddress == targetAddr);
                     CHECK(accesses[0].size == 8);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(accesses[1].accessAddress == preCallRsp - 8);
                     CHECK(accesses[1].value == expectedReturnAddr);
                     CHECK(accesses[1].size == 8);
                     CHECK(accesses[1].type == QBDI::MEMORY_WRITE);
                     seenCallPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  QBDI::GPRState *state = vm.getGPRState();
  state->rax = (QBDI::rword)&buffer[0];
  state->rcx = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(seenCallPre);
  CHECK(seenCallPost);
  CHECK(retval == 0x3333);
}
