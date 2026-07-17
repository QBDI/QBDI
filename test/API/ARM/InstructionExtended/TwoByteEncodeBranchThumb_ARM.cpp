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
#include <string>

#include <catch2/catch_test_macros.hpp>
#include "API/APITest.h"
#include "MemAccessTestUtils_ARM.h"

#include "QBDI/InstAnalysis.h"

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tb") {
  QBDI::rword addr = genASM("b 1f\n1:\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tB");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tblxr") {
  const char source[] =
      "mov r1, lr\n"
      "ldr r0, tblxr_lit\n"
      "blx r0\n"
      "mov lr, r1\n"
      "b tblxr_end\n"
      ".align 2\n"
      "tblxr_lit:\n"
      ".word tblxr_callee\n"
      ".arm\n"
      "tblxr_callee:\n"
      "mov r0, #0x4444\n"
      "bx lr\n"
      ".thumb\n"
      "tblxr_end:\n";

  QBDI::rword expectedReturnAddr = 0;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  vm.addMnemonicCB("tBLXr", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("tBLXr", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     expectedReturnAddr = ia->address + ia->instSize;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("tBLXr", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("tBLXr", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->lr == (expectedReturnAddr | 1));
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(retval == 0x4444);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tblxi") {
  const char source[] =
      "mov r1, lr\n"
      "blx tblxi_callee\n"
      "mov lr, r1\n"
      "b tblxi_end\n"
      ".arm\n"
      "tblxi_callee:\n"
      "mov r0, #0x3333\n"
      "bx lr\n"
      ".thumb\n"
      "tblxi_end:\n";

  QBDI::rword expectedReturnAddr = 0;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  vm.addMnemonicCB("tBLXi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("tBLXi", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     expectedReturnAddr = ia->address + ia->instSize;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("tBLXi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("tBLXi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->lr == (expectedReturnAddr | 1));
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(retval == 0x3333);
}
