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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ret64") {
  const char source[] =
      "movq %rsp, %rbx\n"
      "call ret64_callee\n"
      "jmp ret64_end\n"
      "ret64_callee:\n"
      "ret\n"
      "ret64_end:\n"
      "subq %rsp, %rbx\n"
      "movq %rbx, %rax\n";

  QBDI::rword expectedReturnAddr = 0;
  QBDI::rword preCallRsp = 0;
  bool seenCall = false, seenCallPost = false, seenRet = false,
       seenRetPost = false;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("CALL64pcrel32", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     preCallRsp = gprState->rsp;
                     expectedReturnAddr = ia->address + ia->instSize;
                     seenCall = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("CALL64pcrel32", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == preCallRsp - 8);
                     CHECK(accesses[0].value == expectedReturnAddr);
                     CHECK(accesses[0].size == 8);
                     CHECK(accesses[0].type == QBDI::MEMORY_WRITE);
                     seenCallPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("RET64", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     if (!seenRet) {
                       auto accesses = vmi->getInstMemoryAccess();
                       REQUIRE(accesses.size() == 1);
                       CHECK(accesses[0].accessAddress == gprState->rsp);
                       CHECK(accesses[0].value == expectedReturnAddr);
                       CHECK(accesses[0].size == 8);
                       CHECK(accesses[0].type == QBDI::MEMORY_READ);
                       seenRet = true;
                     }
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("RET64", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     if (!seenRetPost) {
                       auto accesses = vmi->getInstMemoryAccess();
                       REQUIRE(accesses.size() == 1);
                       CHECK(accesses[0].value == expectedReturnAddr);
                       CHECK(accesses[0].size == 8);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-reti64") {
  const char source[] =
      "movq %rsp, %rbx\n"
      "pushq %rax\n"
      "call reti64_callee\n"
      "jmp reti64_end\n"
      "reti64_callee:\n"
      "ret $8\n"
      "reti64_end:\n"
      "subq %rsp, %rbx\n"
      "movq %rbx, %rax\n";

  QBDI::rword expectedReturnAddr = 0;
  QBDI::rword preCallRsp = 0;
  bool seenCall = false, seenCallPost = false, seenRet = false,
       seenRetPost = false;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("CALL64pcrel32", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     preCallRsp = gprState->rsp;
                     expectedReturnAddr = ia->address + ia->instSize;
                     seenCall = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("CALL64pcrel32", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == preCallRsp - 8);
                     CHECK(accesses[0].value == expectedReturnAddr);
                     CHECK(accesses[0].size == 8);
                     CHECK(accesses[0].type == QBDI::MEMORY_WRITE);
                     seenCallPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("RETI64", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == gprState->rsp);
                     CHECK(accesses[0].value == expectedReturnAddr);
                     CHECK(accesses[0].size == 8);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     seenRet = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("RETI64", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].value == expectedReturnAddr);
                     CHECK(accesses[0].size == 8);
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
