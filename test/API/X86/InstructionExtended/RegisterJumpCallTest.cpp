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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-jmp32r") {
  const char source[] =
      "call reg_jmp_getpc\n"
      "reg_jmp_getpc:\n"
      "pop %eax\n"
      "leal reg_jmp_landed-reg_jmp_getpc(%eax), %eax\n"
      "jmp *%eax\n"
      "movl $0x1111, %eax\n"
      "jmp reg_jmp_over\n"
      "reg_jmp_landed:\n"
      "movl $0x2222, %eax\n"
      "reg_jmp_over:\n";

  bool seen = false;
  bool seenPost = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("JMP32r", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("JMP32r", QBDI::POSTINST,
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-call32r") {
  const char source[] =
      "call reg_call_getpc\n"
      "reg_call_getpc:\n"
      "pop %eax\n"
      "leal reg_call_callee-reg_call_getpc(%eax), %eax\n"
      "call *%eax\n"
      "jmp reg_call_end\n"
      "reg_call_callee:\n"
      "movl $0x3333, %eax\n"
      "ret\n"
      "reg_call_end:\n";

  QBDI::rword preCallEsp = 0;
  QBDI::rword expectedReturnAddr = 0;
  bool seenPre = false, seenPost = false;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("CALL32r", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     preCallEsp = gprState->esp;
                     expectedReturnAddr = ia->address + ia->instSize;
                     seenPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("CALL32r", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == preCallEsp - 4);
                     CHECK(accesses[0].value == expectedReturnAddr);
                     CHECK(accesses[0].size == 4);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-call16r") {
  const char source[] =
      "xchg %esp, %edx\n"
      "callw *%ax\n";
  QBDI::rword tmpStack[10] = {0};
  QBDI::rword stackTop = (QBDI::rword)&tmpStack[9];
  QBDI::rword preCallEsp = 0;
  QBDI::rword expectedReturnAddr = 0;
  bool seenPre = false, seenPost = false;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("CALL16r", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     preCallEsp = gprState->esp;
                     expectedReturnAddr = ia->address + ia->instSize;
                     seenPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("CALL16r", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == preCallEsp - 2);
                     CHECK(accesses[0].value == (expectedReturnAddr & 0xffff));
                     CHECK(accesses[0].size == 2);
                     CHECK(accesses[0].type == QBDI::MEMORY_WRITE);
                     CHECK(gprState->esp == preCallEsp - 2);
                     CHECK(gprState->eip == (gprState->eax & 0xffff));
                     seenPost = true;
                     return QBDI::VMAction::STOP;
                   });
  QBDI::GPRState *state = vm.getGPRState();
  state->edx = stackTop;
  state->eax = 0x11229abc;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-jmp16r") {
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
                     CHECK(gprState->eip == (gprState->eax & 0xffff));
                     seenPost = true;
                     return QBDI::VMAction::STOP;
                   });
  QBDI::GPRState *state = vm.getGPRState();
  state->eax = 0x11229abc;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
}
