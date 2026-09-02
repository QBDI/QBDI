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

#include <cstring>
#include <vector>

#include "QBDI/Memory.hpp"
#include "QBDI/Platform.h"
#include "QBDI/Range.h"

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-rip_lea_pos_disp") {
  const char source[] = "lea 0x20(%rip), %rax\n";

  bool seen = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LEA64r", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     CHECK(gprState->rax == ia->address + ia->instSize + 0x20);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-rip_lea_neg_disp") {
  const char source[] = "lea -0x20(%rip), %rax\n";

  bool seen = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LEA64r", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     CHECK(gprState->rax == ia->address + ia->instSize - 0x20);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-rip_mov_read") {
  const char source[] =
      "jmp rip_mov_read_start\n"
      "rip_mov_read_data: .long 0x12345678\n"
      "rip_mov_read_start:\n"
      "lea rip_mov_read_data(%rip), %rbx\n"
      "movl rip_mov_read_data(%rip), %eax\n";

  bool seen = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOV32rm", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     std::vector<QBDI::MemoryAccess> accesses =
                         vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].value == 0x12345678);
                     CHECK(accesses[0].accessAddress == gprState->rbx);
                     CHECK(gprState->rax == 0x12345678);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
}

static QBDI::rword copySnippetToWritableBuffer(QBDI::VM &vm,
                                               QBDI::rword snippetAddr,
                                               std::vector<uint8_t> &buf) {
  memcpy(buf.data(), (void *)snippetAddr, buf.size());
  QBDI::rword bufAddr = (QBDI::rword)buf.data();
  vm.addInstrumentedRange(bufAddr, bufAddr + buf.size());
  vm.clearCache(bufAddr, bufAddr + buf.size());
  return bufAddr;
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-rip_write") {
  const char snippetSrc[] =
      "movl %eax, rip_write_data(%rip)\n"
      "movl rip_write_data(%rip), %ecx\n"
      "movl %ecx, %eax\n"
      "ret\n"
      "rip_write_data: .long 0\n";

  QBDI::rword snippetAddr = genASM(snippetSrc);
  std::vector<uint8_t> codeBuf(64, 0);
  QBDI::rword bufAddr = copySnippetToWritableBuffer(vm, snippetAddr, codeBuf);

  QBDI::rword writeAddr = 0;
  bool seenWrite = false, seenRead = false;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOV32mr", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].value == 0xcafebabe);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_WRITE);
                     writeAddr = accesses[0].accessAddress;
                     seenWrite = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("MOV32rm", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == writeAddr);
                     CHECK(accesses[0].value == 0xcafebabe);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     seenRead = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->rax = 0xcafebabe;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = vm.call(&retval, bufAddr);

  CHECK(ran);
  CHECK(seenWrite);
  CHECK(seenRead);
  CHECK(writeAddr != 0);
  CHECK(retval == 0xcafebabe);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-rip_jmp_indirect") {
  const char snippetSrc[] =
      "lea rip_jmp_ind_landed(%rip), %rax\n"
      "movq %rax, rip_jmp_ind_data(%rip)\n"
      "jmp *rip_jmp_ind_data(%rip)\n"
      "movq $0x1111, %rax\n"
      "jmp rip_jmp_ind_end\n"
      "rip_jmp_ind_landed:\n"
      "movq $0x2222, %rax\n"
      "rip_jmp_ind_end:\n"
      "ret\n"
      "rip_jmp_ind_data: .quad 0\n";

  QBDI::rword snippetAddr = genASM(snippetSrc);
  std::vector<uint8_t> codeBuf(128, 0);
  QBDI::rword bufAddr = copySnippetToWritableBuffer(vm, snippetAddr, codeBuf);

  QBDI::rword dataSlotAddr = 0, landedAddr = 0;
  bool seenStore = false, seenJump = false, seenJumpPost = false;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOV64mr", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].size == 8);
                     CHECK(accesses[0].type == QBDI::MEMORY_WRITE);
                     dataSlotAddr = accesses[0].accessAddress;
                     landedAddr = accesses[0].value;
                     seenStore = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("JMP64m", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == dataSlotAddr);
                     CHECK(accesses[0].value == landedAddr);
                     CHECK(accesses[0].size == 8);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     seenJump = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("JMP64m", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == dataSlotAddr);
                     CHECK(accesses[0].value == landedAddr);
                     CHECK(accesses[0].size == 8);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     seenJumpPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = vm.call(&retval, bufAddr);

  CHECK(ran);
  CHECK(seenStore);
  CHECK(seenJump);
  CHECK(seenJumpPost);
  CHECK(dataSlotAddr != 0);
  CHECK(retval == 0x2222);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-rip_call_indirect") {
  const char snippetSrc[] =
      "lea rip_call_ind_callee(%rip), %rax\n"
      "movq %rax, rip_call_ind_data(%rip)\n"
      "call *rip_call_ind_data(%rip)\n"
      "jmp rip_call_ind_end\n"
      "rip_call_ind_callee:\n"
      "movq $0x3333, %rax\n"
      "ret\n"
      "rip_call_ind_end:\n"
      "ret\n"
      "rip_call_ind_data: .quad 0\n";

  QBDI::rword snippetAddr = genASM(snippetSrc);
  std::vector<uint8_t> codeBuf(128, 0);
  QBDI::rword bufAddr = copySnippetToWritableBuffer(vm, snippetAddr, codeBuf);

  QBDI::rword dataSlotAddr = 0, calleeAddr = 0;
  QBDI::rword preCallRsp = 0, expectedReturnAddr = 0;
  bool seenStore = false, seenCallPre = false, seenCallPost = false;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOV64mr", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].size == 8);
                     CHECK(accesses[0].type == QBDI::MEMORY_WRITE);
                     dataSlotAddr = accesses[0].accessAddress;
                     calleeAddr = accesses[0].value;
                     seenStore = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("CALL64m", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == dataSlotAddr);
                     CHECK(accesses[0].value == calleeAddr);
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
                     CHECK(accesses[0].accessAddress == dataSlotAddr);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(accesses[1].accessAddress == preCallRsp - 8);
                     CHECK(accesses[1].value == expectedReturnAddr);
                     CHECK(accesses[1].size == 8);
                     CHECK(accesses[1].type == QBDI::MEMORY_WRITE);
                     seenCallPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = vm.call(&retval, bufAddr);

  CHECK(ran);
  CHECK(seenStore);
  CHECK(seenCallPre);
  CHECK(seenCallPost);
  CHECK(retval == 0x3333);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-movabs_abs64") {
  const char source[] =
      "movabsq $movabs_data, %rax\n"
      "movl (%rax), %ecx\n"
      "movl %ecx, %eax\n"
      "jmp movabs_end\n"
      "movabs_data: .long 0x12345678\n"
      "movabs_end:\n";

  bool seen = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOV32rm", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].value == 0x12345678);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
  CHECK(retval == 0x12345678);
}
