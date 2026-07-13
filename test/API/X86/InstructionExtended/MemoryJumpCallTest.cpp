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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-jmp32m") {
  const char source[] =
      "call mem_jmp_getpc\n"
      "mem_jmp_getpc:\n"
      "pop %edx\n"
      "leal mem_jmp_landed-mem_jmp_getpc(%edx), %edx\n"
      "movl %edx, 0x10(%eax,%ecx,4)\n"
      "jmp *0x10(%eax,%ecx,4)\n"
      "movl $0x1111, %eax\n"
      "jmp mem_jmp_over\n"
      "mem_jmp_landed:\n"
      "movl $0x2222, %eax\n"
      "mem_jmp_over:\n";

  QBDI::rword buffer[8] = {0};
  bool seenStore = false, seenJump = false, seenJumpPost = false;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOV32mr", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress ==
                           (QBDI::rword)&buffer[5]);
                     CHECK(accesses[0].size == 4);
                     seenStore = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("JMP32m", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress ==
                           (QBDI::rword)&buffer[5]);
                     CHECK(accesses[0].size == 4);
                     seenJump = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("JMP32m", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress ==
                           (QBDI::rword)&buffer[5]);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     seenJumpPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->eax = (QBDI::rword)&buffer[0];
  state->ecx = 1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seenStore);
  CHECK(seenJump);
  CHECK(seenJumpPost);
  CHECK(retval == 0x2222);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-call32m") {
  const char source[] =
      "call mem_call_getpc\n"
      "mem_call_getpc:\n"
      "pop %edx\n"
      "leal mem_call_callee-mem_call_getpc(%edx), %edx\n"
      "movl %edx, 0x10(%eax,%ecx,4)\n"
      "call *0x10(%eax,%ecx,4)\n"
      "jmp mem_call_end\n"
      "mem_call_callee:\n"
      "movl $0x3333, %eax\n"
      "ret\n"
      "mem_call_end:\n";

  QBDI::rword buffer[8] = {0};
  QBDI::rword preCallEsp = 0, expectedReturnAddr = 0;
  bool seenStore = false, seenCallPre = false, seenCallPost = false;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOV32mr", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress ==
                           (QBDI::rword)&buffer[5]);
                     CHECK(accesses[0].size == 4);
                     seenStore = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("CALL32m", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress ==
                           (QBDI::rword)&buffer[5]);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     preCallEsp = gprState->esp;
                     expectedReturnAddr = ia->address + ia->instSize;
                     seenCallPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("CALL32m", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 2);
                     CHECK(accesses[0].accessAddress ==
                           (QBDI::rword)&buffer[5]);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(accesses[1].accessAddress == preCallEsp - 4);
                     CHECK(accesses[1].value == expectedReturnAddr);
                     CHECK(accesses[1].size == 4);
                     CHECK(accesses[1].type == QBDI::MEMORY_WRITE);
                     seenCallPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->eax = (QBDI::rword)&buffer[0];
  state->ecx = 1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seenStore);
  CHECK(seenCallPre);
  CHECK(seenCallPost);
  CHECK(retval == 0x3333);
}
