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

#include "MemAccessTestUtils_X86.h"

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::checkedSnprintf;
using QBDITestBatch2::checkFeature;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-MOV32ao32") {
  uint8_t buffer[64] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[16]);
  *target = 0x2a2a2a2a;
  QBDI::rword targetAddr = (QBDI::rword)target;
  char source[128];
  checkedSnprintf(source, "movl 0x%x, %%eax\n", (unsigned)targetAddr);
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x2a2a2a2a, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x2a2a2a2a, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOV32ao32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MOV32ao32", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->eax = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK((vm.getGPRState()->eax & 0xffffffff) == 0x2a2a2a2a);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-MOV32o32a") {
  uint8_t buffer[64] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[16]);
  *target = 0;
  QBDI::rword targetAddr = (QBDI::rword)target;
  char source[128];
  checkedSnprintf(source, "movl %%eax, 0x%x\n", (unsigned)targetAddr);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x2a2a2a2a, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOV32o32a", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MOV32o32a", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->eax = 0x2a2a2a2a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x2a2a2a2a);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}
