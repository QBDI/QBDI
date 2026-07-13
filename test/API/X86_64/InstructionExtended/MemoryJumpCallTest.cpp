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

#include "QBDI/Memory.hpp"
#include "QBDI/Platform.h"
#include "QBDI/Range.h"

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-jmp64m") {
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
  bool seenStore = false, seenJump = false, seenJumpPost = false;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOV64mr", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress ==
                           (QBDI::rword)&buffer[3]);
                     CHECK(accesses[0].size == 8);
                     seenStore = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("JMP64m", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress ==
                           (QBDI::rword)&buffer[3]);
                     CHECK(accesses[0].size == 8);
                     seenJump = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("JMP64m", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress ==
                           (QBDI::rword)&buffer[3]);
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
  CHECK(seenStore);
  CHECK(seenJump);
  CHECK(seenJumpPost);
  CHECK(retval == 0x2222);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-call64m") {
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
  QBDI::rword preCallRsp = 0;
  QBDI::rword expectedReturnAddr = 0;
  bool seenStore = false, seenCallPre = false, seenCallPost = false;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOV64mr", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress ==
                           (QBDI::rword)&buffer[3]);
                     CHECK(accesses[0].size == 8);
                     seenStore = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("CALL64m", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress ==
                           (QBDI::rword)&buffer[3]);
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
                     CHECK(accesses[0].accessAddress ==
                           (QBDI::rword)&buffer[3]);
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
  CHECK(seenStore);
  CHECK(seenCallPre);
  CHECK(seenCallPost);
  CHECK(retval == 0x3333);
}
