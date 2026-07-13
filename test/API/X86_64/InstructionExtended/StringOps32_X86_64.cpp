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

#include "MemAccessTestUtils_X86_64.h"

#include <vector>

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::checkEmptyAccess;
using QBDITestBatch2::checkFeature;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-CMPSL") {
  const char source[] = "cmpsl\n";
  uint32_t bufA[4] = {0x11223344, 0x33445566, 0x55667788, 0x778899aa};
  uint32_t bufB[4] = {0x99999999, 0x33445566, 0x55667788, 0x778899aa};
  size_t seenCount = 0;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("CMPSL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     std::vector<QBDI::MemoryAccess> accesses =
                         vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 2);
                     CHECK(accesses[0].accessAddress == (QBDI::rword)&bufA[0]);
                     CHECK(accesses[0].value == 0x11223344);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(accesses[1].accessAddress == (QBDI::rword)&bufB[0]);
                     CHECK(accesses[1].value == 0x99999999);
                     CHECK(accesses[1].size == 4);
                     CHECK(accesses[1].type == QBDI::MEMORY_READ);
                     seenCount++;
                     return QBDI::VMAction::CONTINUE;
                   });
  QBDI::GPRState *state = vm.getGPRState();
  state->rsi = (QBDI::rword)&bufA[0];
  state->rdi = (QBDI::rword)&bufB[0];
  state->eflags &= ~(1 << 10);
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(seenCount == 1);
  CHECK(state->rsi == (QBDI::rword)&bufA[1]);
  CHECK(state->rdi == (QBDI::rword)&bufB[1]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-LODSL") {
  const char source[] = "lodsl\n";
  uint32_t buf[4] = {0x11223344, 0x33445566, 0x55667788, 0x778899aa};
  bool seen = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LODSL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     std::vector<QBDI::MemoryAccess> accesses =
                         vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == (QBDI::rword)&buf[0]);
                     CHECK(accesses[0].value == 0x11223344);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  QBDI::GPRState *state = vm.getGPRState();
  state->rsi = (QBDI::rword)&buf[0];
  state->rax = 0;
  state->eflags &= ~(1 << 10);
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(seen);
  CHECK((state->rax & 0xffffffff) == 0x11223344);
  CHECK(state->rsi == (QBDI::rword)&buf[1]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-MOVSL") {
  const char source[] = "movsl\n";
  uint32_t bufA[4] = {0x11223344, 0x33445566, 0x55667788, 0x778899aa};
  uint32_t bufB[4] = {0, 0, 0, 0};
  bool seen = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOVSL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     std::vector<QBDI::MemoryAccess> accesses =
                         vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 2);
                     CHECK(accesses[0].accessAddress == (QBDI::rword)&bufA[0]);
                     CHECK(accesses[0].value == 0x11223344);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(accesses[1].accessAddress == (QBDI::rword)&bufB[0]);
                     CHECK(accesses[1].value == 0x11223344);
                     CHECK(accesses[1].size == 4);
                     CHECK(accesses[1].type == QBDI::MEMORY_WRITE);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  QBDI::GPRState *state = vm.getGPRState();
  state->rsi = (QBDI::rword)&bufA[0];
  state->rdi = (QBDI::rword)&bufB[0];
  state->eflags &= ~(1 << 10);
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(seen);
  CHECK(bufB[0] == 0x11223344);
  CHECK(state->rsi == (QBDI::rword)&bufA[1]);
  CHECK(state->rdi == (QBDI::rword)&bufB[1]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SCASL") {
  const char source[] = "scasl\n";
  uint32_t buf[4] = {0x99999999, 0x33445566, 0x55667788, 0x778899aa};
  bool seen = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SCASL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     std::vector<QBDI::MemoryAccess> accesses =
                         vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == (QBDI::rword)&buf[0]);
                     CHECK(accesses[0].value == 0x99999999);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  QBDI::GPRState *state = vm.getGPRState();
  state->rdi = (QBDI::rword)&buf[0];
  state->rax = 0x11223344;
  state->eflags &= ~(1 << 10);
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(seen);
  CHECK(state->rdi == (QBDI::rword)&buf[1]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-STOSL") {
  const char source[] = "stosl\n";
  uint32_t buf[4] = {0, 0, 0, 0};
  bool seen = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STOSL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     std::vector<QBDI::MemoryAccess> accesses =
                         vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == (QBDI::rword)&buf[0]);
                     CHECK(accesses[0].value == 0x55667788);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_WRITE);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  QBDI::GPRState *state = vm.getGPRState();
  state->rdi = (QBDI::rword)&buf[0];
  state->rax = 0x55667788;
  state->eflags &= ~(1 << 10);
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(seen);
  CHECK(buf[0] == 0x55667788);
  CHECK(state->rdi == (QBDI::rword)&buf[1]);
}
