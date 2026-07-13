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

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::checkFeature;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-CMPXCHG16B") {
  if (!checkFeature("cx16")) {
    return;
  }
  const char source[] = "cmpxchg16b 0x1c(%rdi,%rsi,4)\n";
  alignas(16) uint8_t buffer[80] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[32];
  *reinterpret_cast<QBDI::rword *>(&buffer[32]) = 0x1111111111111111ULL;
  *reinterpret_cast<QBDI::rword *>(&buffer[40]) = 0x2222222222222222ULL;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
      {targetAddr, 0, 16, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("CMPXCHG16B", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("CMPXCHG16B", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rdi = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1111111111111111ULL;
  state->rdx = 0x2222222222222222ULL;
  state->rbx = 0x3333333333333333ULL;
  state->rcx = 0x4444444444444444ULL;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}
