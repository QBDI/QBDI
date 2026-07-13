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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-pusha_popa_32") {
  const char source[] =
      "pusha\n"
      "movl $0xdead0001, %eax\n"
      "movl $0xdead0002, %ecx\n"
      "movl $0xdead0003, %edx\n"
      "movl $0xdead0004, %ebx\n"
      "movl $0xdead0006, %ebp\n"
      "movl $0xdead0007, %esi\n"
      "movl $0xdead0008, %edi\n"
      "popa\n";

  QBDI::rword preEsp = 0;
  bool seenPush = false, seenPop = false;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PUSHA32", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     preEsp = gprState->esp;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("PUSHA32", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == preEsp - 32);
                     CHECK(accesses[0].size == 32);
                     CHECK(accesses[0].type == QBDI::MEMORY_WRITE);
                     CHECK((accesses[0].flags & QBDI::MEMORY_UNKNOWN_VALUE) !=
                           0);
                     auto *stack = (const uint32_t *)(preEsp - 32);
                     CHECK(stack[7] == 0x11111111); // EAX
                     CHECK(stack[6] == 0x22222222); // ECX
                     CHECK(stack[5] == 0x33333333); // EDX
                     CHECK(stack[4] == 0x44444444); // EBX
                     CHECK(stack[3] == preEsp);     // original ESP
                     CHECK(stack[2] == 0x66666666); // EBP
                     CHECK(stack[1] == 0x77777777); // ESI
                     CHECK(stack[0] == 0x88888888); // EDI
                     seenPush = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("POPA32", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == preEsp - 32);
                     CHECK(accesses[0].size == 32);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK((accesses[0].flags & QBDI::MEMORY_UNKNOWN_VALUE) !=
                           0);
                     seenPop = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->eax = 0x11111111;
  state->ecx = 0x22222222;
  state->edx = 0x33333333;
  state->ebx = 0x44444444;
  state->ebp = 0x66666666;
  state->esi = 0x77777777;
  state->edi = 0x88888888;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seenPush);
  CHECK(seenPop);
  CHECK(state->eax == 0x11111111);
  CHECK(state->ecx == 0x22222222);
  CHECK(state->edx == 0x33333333);
  CHECK(state->ebx == 0x44444444);
  CHECK(state->ebp == 0x66666666);
  CHECK(state->esi == 0x77777777);
  CHECK(state->edi == 0x88888888);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-pusha_popa_16") {
  const char source[] =
      "pushaw\n"
      "movl $0x1000d001, %eax\n"
      "movl $0x2000d002, %ecx\n"
      "movl $0x3000d003, %edx\n"
      "movl $0x4000d004, %ebx\n"
      "movl $0x6000d006, %ebp\n"
      "movl $0x7000d007, %esi\n"
      "movl $0x8000d008, %edi\n"
      "popaw\n";

  QBDI::rword preEsp = 0;
  bool seenPush = false, seenPop = false;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PUSHA16", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     preEsp = gprState->esp;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB(
      "PUSHA16", QBDI::POSTINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        auto accesses = vmi->getInstMemoryAccess();
        REQUIRE(accesses.size() == 1);
        CHECK(accesses[0].accessAddress == preEsp - 16);
        CHECK(accesses[0].size == 16);
        CHECK(accesses[0].type == QBDI::MEMORY_WRITE);
        CHECK((accesses[0].flags & QBDI::MEMORY_UNKNOWN_VALUE) != 0);
        auto *stack = (const uint16_t *)(preEsp - 16);
        CHECK(stack[7] == 0x1111);                      // AX
        CHECK(stack[6] == 0x2222);                      // CX
        CHECK(stack[5] == 0x3333);                      // DX
        CHECK(stack[4] == 0x4444);                      // BX
        CHECK(stack[3] == (uint16_t)(preEsp & 0xffff)); // original SP
        CHECK(stack[2] == 0x6666);                      // BP
        CHECK(stack[1] == 0x7777);                      // SI
        CHECK(stack[0] == 0x8888);                      // DI
        seenPush = true;
        return QBDI::VMAction::CONTINUE;
      });
  vm.addMnemonicCB("POPA16", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == preEsp - 16);
                     CHECK(accesses[0].size == 16);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK((accesses[0].flags & QBDI::MEMORY_UNKNOWN_VALUE) !=
                           0);
                     seenPop = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->eax = 0xaaaa1111;
  state->ecx = 0xbbbb2222;
  state->edx = 0xcccc3333;
  state->ebx = 0xdddd4444;
  state->ebp = 0xeeee6666;
  state->esi = 0xffff7777;
  state->edi = 0x12348888;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seenPush);
  CHECK(seenPop);
  CHECK(state->eax == 0x10001111);
  CHECK(state->ecx == 0x20002222);
  CHECK(state->edx == 0x30003333);
  CHECK(state->ebx == 0x40004444);
  CHECK(state->ebp == 0x60006666);
  CHECK(state->esi == 0x70007777);
  CHECK(state->edi == 0x80008888);
}
