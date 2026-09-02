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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-ret32") {
  const char source[] =
      "movl %esp, %ebx\n"
      "call ret32_callee\n"
      "jmp ret32_end\n"
      "ret32_callee:\n"
      "ret\n"
      "ret32_end:\n"
      "subl %esp, %ebx\n"
      "movl %ebx, %eax\n";

  QBDI::rword expectedReturnAddr = 0;
  QBDI::rword preCallEsp = 0;
  bool seenCall = false, seenCallPost = false, seenRet = false,
       seenRetPost = false;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("CALLpcrel32", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     preCallEsp = gprState->esp;
                     expectedReturnAddr = ia->address + ia->instSize;
                     seenCall = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("CALLpcrel32", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == preCallEsp - 4);
                     CHECK(accesses[0].value == expectedReturnAddr);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_WRITE);
                     seenCallPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("RET32", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     if (!seenRet) {
                       auto accesses = vmi->getInstMemoryAccess();
                       REQUIRE(accesses.size() == 1);
                       CHECK(accesses[0].accessAddress == gprState->esp);
                       CHECK(accesses[0].value == expectedReturnAddr);
                       CHECK(accesses[0].size == 4);
                       CHECK(accesses[0].type == QBDI::MEMORY_READ);
                       seenRet = true;
                     }
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("RET32", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     if (!seenRetPost) {
                       auto accesses = vmi->getInstMemoryAccess();
                       REQUIRE(accesses.size() == 1);
                       CHECK(accesses[0].value == expectedReturnAddr);
                       CHECK(accesses[0].size == 4);
                       CHECK(accesses[0].type == QBDI::MEMORY_READ);
                       seenRetPost = true;
                     }
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seenCall);
  CHECK(seenCallPost);
  CHECK(seenRet);
  CHECK(seenRetPost);
  CHECK(retval == 0);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-reti32") {
  const char source[] =
      "movl %esp, %ebx\n"
      "pushl %eax\n"
      "call reti32_callee\n"
      "jmp reti32_end\n"
      "reti32_callee:\n"
      "ret $4\n"
      "reti32_end:\n"
      "subl %esp, %ebx\n"
      "movl %ebx, %eax\n";

  QBDI::rword expectedReturnAddr = 0;
  QBDI::rword preCallEsp = 0;
  bool seenCall = false, seenCallPost = false, seenRet = false,
       seenRetPost = false;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("CALLpcrel32", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     preCallEsp = gprState->esp;
                     expectedReturnAddr = ia->address + ia->instSize;
                     seenCall = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("CALLpcrel32", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == preCallEsp - 4);
                     CHECK(accesses[0].value == expectedReturnAddr);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_WRITE);
                     seenCallPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("RETI32", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == gprState->esp);
                     CHECK(accesses[0].value == expectedReturnAddr);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     seenRet = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("RETI32", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].value == expectedReturnAddr);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     seenRetPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seenCall);
  CHECK(seenCallPost);
  CHECK(seenRet);
  CHECK(seenRetPost);
  CHECK(retval == 0);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-ret16") {
  const char source[] =
      "xchg %esp, %edx\n"
      "retw\n";
  QBDI::rword tmpStack[10] = {0};
  uint16_t *stackTop = reinterpret_cast<uint16_t *>(&tmpStack[8]);
  *stackTop = 0x9abc;
  QBDI::rword stackAddr = (QBDI::rword)stackTop;
  QBDI::rword rspBefore = 0;
  bool seenPre = false, seenPost = false;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("RET16", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     rspBefore = gprState->esp;
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == stackAddr);
                     CHECK(accesses[0].value == 0x9abc);
                     CHECK(accesses[0].size == 2);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     seenPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("RET16", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == stackAddr);
                     CHECK(accesses[0].value == 0x9abc);
                     CHECK(accesses[0].size == 2);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(gprState->esp == rspBefore + 2);
                     CHECK(gprState->eip == 0x9abc);
                     seenPost = true;
                     return QBDI::VMAction::STOP;
                   });
  QBDI::GPRState *state = vm.getGPRState();
  state->edx = stackAddr;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-reti16") {
  const char source[] =
      "xchg %esp, %edx\n"
      "retw $4\n";
  QBDI::rword tmpStack[10] = {0};
  uint16_t *stackTop = reinterpret_cast<uint16_t *>(&tmpStack[8]);
  *stackTop = 0x9abc;
  QBDI::rword stackAddr = (QBDI::rword)stackTop;
  QBDI::rword rspBefore = 0;
  bool seenPre = false, seenPost = false;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("RETI16", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     rspBefore = gprState->esp;
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == stackAddr);
                     CHECK(accesses[0].value == 0x9abc);
                     CHECK(accesses[0].size == 2);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     seenPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("RETI16", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == stackAddr);
                     CHECK(accesses[0].value == 0x9abc);
                     CHECK(accesses[0].size == 2);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(gprState->esp == rspBefore + 2 + 4);
                     CHECK(gprState->eip == 0x9abc);
                     seenPost = true;
                     return QBDI::VMAction::STOP;
                   });
  QBDI::GPRState *state = vm.getGPRState();
  state->edx = stackAddr;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-lret32") {
  const char source[] =
      "xchg %esp, %edx\n"
      "lretl\n";
  QBDI::rword tmpStack[10] = {0};
  uint32_t *stackTop = reinterpret_cast<uint32_t *>(&tmpStack[6]);
  stackTop[0] = 0x11223344;
  stackTop[1] = 0x33; // CS slot, value irrelevant: QBDI discards it
  QBDI::rword stackAddr = (QBDI::rword)stackTop;
  QBDI::rword rspBefore = 0;
  bool seenPre = false, seenPost = false;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LRET32", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     rspBefore = gprState->esp;
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == stackAddr);
                     CHECK(accesses[0].value == 0x11223344);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     seenPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("LRET32", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].value == 0x11223344);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(gprState->esp == rspBefore + 8);
                     CHECK(gprState->eip == 0x11223344);
                     seenPost = true;
                     return QBDI::VMAction::STOP;
                   });
  QBDI::GPRState *state = vm.getGPRState();
  state->edx = stackAddr;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-lreti32") {
  const char source[] =
      "xchg %esp, %edx\n"
      "lretl $8\n";
  QBDI::rword tmpStack[10] = {0};
  uint32_t *stackTop = reinterpret_cast<uint32_t *>(&tmpStack[6]);
  stackTop[0] = 0x11223344;
  stackTop[1] = 0x33;
  QBDI::rword stackAddr = (QBDI::rword)stackTop;
  QBDI::rword rspBefore = 0;
  bool seenPre = false, seenPost = false;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LRETI32", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     rspBefore = gprState->esp;
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == stackAddr);
                     CHECK(accesses[0].value == 0x11223344);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     seenPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("LRETI32", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].value == 0x11223344);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(gprState->esp == rspBefore + 8 + 8);
                     CHECK(gprState->eip == 0x11223344);
                     seenPost = true;
                     return QBDI::VMAction::STOP;
                   });
  QBDI::GPRState *state = vm.getGPRState();
  state->edx = stackAddr;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-lret16") {
  const char source[] =
      "xchg %esp, %edx\n"
      "lretw\n";
  QBDI::rword tmpStack[10] = {0};
  uint16_t *stackTop = reinterpret_cast<uint16_t *>(&tmpStack[8]);
  stackTop[0] = 0x9abc;
  stackTop[1] = 0x33; // CS slot, value irrelevant
  QBDI::rword stackAddr = (QBDI::rword)stackTop;
  QBDI::rword rspBefore = 0;
  bool seenPre = false, seenPost = false;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LRET16", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     rspBefore = gprState->esp;
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == stackAddr);
                     CHECK(accesses[0].value == 0x9abc);
                     CHECK(accesses[0].size == 2);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     seenPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("LRET16", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].value == 0x9abc);
                     CHECK(accesses[0].size == 2);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(gprState->esp == rspBefore + 4);
                     CHECK(gprState->eip == 0x9abc);
                     seenPost = true;
                     return QBDI::VMAction::STOP;
                   });
  QBDI::GPRState *state = vm.getGPRState();
  state->edx = stackAddr;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-lreti16") {
  const char source[] =
      "xchg %esp, %edx\n"
      "lretw $4\n";
  QBDI::rword tmpStack[10] = {0};
  uint16_t *stackTop = reinterpret_cast<uint16_t *>(&tmpStack[8]);
  stackTop[0] = 0x9abc;
  stackTop[1] = 0x33;
  QBDI::rword stackAddr = (QBDI::rword)stackTop;
  QBDI::rword rspBefore = 0;
  bool seenPre = false, seenPost = false;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LRETI16", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     rspBefore = gprState->esp;
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == stackAddr);
                     CHECK(accesses[0].value == 0x9abc);
                     CHECK(accesses[0].size == 2);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     seenPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("LRETI16", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].value == 0x9abc);
                     CHECK(accesses[0].size == 2);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(gprState->esp == rspBefore + 4 + 4);
                     CHECK(gprState->eip == 0x9abc);
                     seenPost = true;
                     return QBDI::VMAction::STOP;
                   });
  QBDI::GPRState *state = vm.getGPRState();
  state->edx = stackAddr;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
}
