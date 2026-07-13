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
using QBDITestBatch2::checkFeature;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-CMPSB") {
  const char source[] = "cmpsb\n";
  uint8_t bufA[4] = {0x11, 0x22, 0x33, 0x44};
  uint8_t bufB[4] = {0x99, 0x22, 0x33, 0x44};
  size_t seenCount = 0;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("CMPSB", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     std::vector<QBDI::MemoryAccess> accesses =
                         vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 2);
                     CHECK(accesses[0].accessAddress == (QBDI::rword)&bufA[0]);
                     CHECK(accesses[0].value == 0x11);
                     CHECK(accesses[0].size == 1);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(accesses[1].accessAddress == (QBDI::rword)&bufB[0]);
                     CHECK(accesses[1].value == 0x99);
                     CHECK(accesses[1].size == 1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-LODSB") {
  const char source[] = "lodsb\n";
  uint8_t buf[4] = {0x11, 0x22, 0x33, 0x44};
  bool seen = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LODSB", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     std::vector<QBDI::MemoryAccess> accesses =
                         vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == (QBDI::rword)&buf[0]);
                     CHECK(accesses[0].value == 0x11);
                     CHECK(accesses[0].size == 1);
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
  CHECK((state->rax & 0xff) == 0x11);
  CHECK(state->rsi == (QBDI::rword)&buf[1]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-STOSB") {
  const char source[] = "stosb\n";
  uint8_t buf[4] = {0, 0, 0, 0};
  bool seen = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STOSB", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     std::vector<QBDI::MemoryAccess> accesses =
                         vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == (QBDI::rword)&buf[0]);
                     CHECK(accesses[0].value == 0x55);
                     CHECK(accesses[0].size == 1);
                     CHECK(accesses[0].type == QBDI::MEMORY_WRITE);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  QBDI::GPRState *state = vm.getGPRState();
  state->rdi = (QBDI::rword)&buf[0];
  state->rax = 0x55;
  state->eflags &= ~(1 << 10);
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(seen);
  CHECK(buf[0] == 0x55);
  CHECK(state->rdi == (QBDI::rword)&buf[1]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-XLAT") {
  const char source[] = "xlatb\n";
  uint8_t table[8] = {0, 0, 0, 0x77, 0, 0, 0, 0};
  bool seen = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("XLAT", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     std::vector<QBDI::MemoryAccess> accesses =
                         vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].accessAddress == (QBDI::rword)&table[3]);
                     CHECK(accesses[0].value == 0x77);
                     CHECK(accesses[0].size == 1);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&table[0];
  state->rax = 3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(seen);
  CHECK((state->rax & 0xff) == 0x77);
}
