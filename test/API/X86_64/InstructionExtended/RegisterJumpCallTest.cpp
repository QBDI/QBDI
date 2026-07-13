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

#include <catch2/catch_test_macros.hpp>
#include "API/APITest.h"

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-jmp64r") {
  const char source[] =
      "lea reg_jmp_landed(%rip), %rax\n"
      "jmp *%rax\n"
      "movq $0x1111, %rax\n"
      "jmp reg_jmp_over\n"
      "reg_jmp_landed:\n"
      "movq $0x2222, %rax\n"
      "reg_jmp_over:\n";

  bool seen = false;
  bool seenPost = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("JMP64r", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("JMP64r", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
  CHECK(seenPost);
  CHECK(retval == 0x2222);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-call64r") {
  const char source[] =
      "lea reg_call_callee(%rip), %rax\n"
      "call *%rax\n"
      "jmp reg_call_end\n"
      "reg_call_callee:\n"
      "movq $0x3333, %rax\n"
      "ret\n"
      "reg_call_end:\n";

  QBDI::rword preCallRsp = 0;
  QBDI::rword expectedReturnAddr = 0;
  bool seenPre = false, seenPost = false;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("CALL64r", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     preCallRsp = gprState->rsp;
                     expectedReturnAddr = ia->address + ia->instSize;
                     seenPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("CALL64r", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     std::vector<QBDI::MemoryAccess> accesses =
                         vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == preCallRsp - 8);
                     CHECK(accesses[0].value == expectedReturnAddr);
                     CHECK(accesses[0].size == 8);
                     CHECK(accesses[0].type == QBDI::MEMORY_WRITE);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == 0x3333);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-call16r") {
  const char source[] =
      "xchg %rsp, %rdx\n"
      "callw *%ax\n";
  QBDI::rword tmpStack[10] = {0};
  QBDI::rword stackTop = (QBDI::rword)&tmpStack[9];
  QBDI::rword preCallRsp = 0;
  QBDI::rword expectedReturnAddr = 0;
  bool seenPre = false, seenPost = false;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("CALL16r", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     preCallRsp = gprState->rsp;
                     expectedReturnAddr = ia->address + ia->instSize;
                     seenPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("CALL16r", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == preCallRsp - 2);
                     CHECK(accesses[0].value == (expectedReturnAddr & 0xffff));
                     CHECK(accesses[0].size == 2);
                     CHECK(accesses[0].type == QBDI::MEMORY_WRITE);
                     CHECK(gprState->rsp == preCallRsp - 2);
                     CHECK(gprState->rip == (gprState->rax & 0xffff));
                     seenPost = true;
                     return QBDI::VMAction::STOP;
                   });
  QBDI::GPRState *state = vm.getGPRState();
  state->rdx = stackTop;
  state->rax = 0x1122334455669abc;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-jmp16r") {
  const char source[] = "jmpw *%ax\n";
  bool seenPre = false, seenPost = false;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("JMP16r", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seenPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("JMP16r", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     CHECK(gprState->rip == (gprState->rax & 0xffff));
                     seenPost = true;
                     return QBDI::VMAction::STOP;
                   });
  QBDI::GPRState *state = vm.getGPRState();
  state->rax = 0x1122334455669abc;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
}
