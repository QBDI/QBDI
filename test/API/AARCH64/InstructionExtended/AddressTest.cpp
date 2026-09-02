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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-adr") {
  const char source[] =
      "adr x0, adr_target\n"
      "adr_target:\n"
      "mov x1, #0x2222\n";

  bool seen = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADR", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     CHECK(gprState->x0 == ia->address + ia->instSize);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-adr_offset_forward") {
  const char source[] =
      "adr x0, adr_fwd_target\n"
      ".rept 10\n"
      "nop\n"
      ".endr\n"
      "adr_fwd_target:\n"
      "mov x1, #0x2222\n";

  bool seen = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADR", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     CHECK(gprState->x0 == ia->address + ia->instSize + 40);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-adr_offset_backward") {
  const char source[] =
      "b adr_bwd_skip\n"
      "adr_bwd_target:\n"
      "mov x1, #0x3333\n"
      "b adr_bwd_end\n"
      "adr_bwd_skip:\n"
      ".rept 10\n"
      "nop\n"
      ".endr\n"
      "adr x0, adr_bwd_target\n"
      "adr_bwd_end:\n";

  bool seen = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADR", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     CHECK(gprState->x0 == ia->address - 48);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-adrp") {
  const char source[] =
      "b adrp_start\n"
      "adrp_target:\n"
      ".word 0x2222\n"
      ".space 8192\n"
      "adrp_start:\n"
      "adrp x0, adrp_target\n"
      "add x0, x0, :lo12:adrp_target\n";

  QBDI::rword expectedTarget = 0;
  bool seenAdrp = false, seenAdd = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADRP", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     expectedTarget = ia->address - 8196;
                     seenAdrp = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("ADDXri", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     CHECK(gprState->x0 == expectedTarget);
                     seenAdd = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seenAdrp);
  CHECK(seenAdd);
  CHECK(expectedTarget != 0);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldr_literal_x") {
  const char source[] =
      "b ldrx_start\n"
      "ldrx_data: .quad 0x123456789abcdef0\n"
      "ldrx_start:\n"
      "adr x1, ldrx_data\n"
      "ldr x0, ldrx_data\n";

  bool seen = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRXl", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == gprState->x1);
                     CHECK(accesses[0].size == 8);
                     CHECK(accesses[0].value == 0x123456789abcdef0ULL);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(gprState->x0 == 0x123456789abcdef0ULL);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldr_literal_w") {
  const char source[] =
      "b ldrw_start\n"
      "ldrw_data: .word 0x12345678\n"
      "ldrw_start:\n"
      "adr x1, ldrw_data\n"
      "ldr w0, ldrw_data\n";

  bool seen = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRWl", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == gprState->x1);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].value == 0x12345678);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(gprState->x0 == 0x12345678);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldr_literal_sw") {
  const char source[] =
      "b ldrsw_start\n"
      "ldrsw_data: .word 0x80000001\n"
      "ldrsw_start:\n"
      "adr x1, ldrsw_data\n"
      "ldrsw x0, ldrsw_data\n";

  bool seen = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRSWl", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == gprState->x1);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].value == 0x80000001);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(gprState->x0 == 0xffffffff80000001ULL);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
}
