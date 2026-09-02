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
using QBDITestBatch2::checkFeature;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

static const QBDI::MemoryAccessFlags kXstateFlags =
    QBDI::MEMORY_MINIMUM_SIZE | QBDI::MEMORY_UNKNOWN_VALUE;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-XSAVE") {
  if (!checkFeature("xsave")) {
    return;
  }
  const char source[] =
      "xor %eax, %eax\nxor %edx, %edx\nxsave 0x40(%ebx,%esi,8)\n";
  alignas(64) uint8_t buffer[768] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[128];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 576, QBDI::MEMORY_READ, kXstateFlags},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 576, QBDI::MEMORY_READ, kXstateFlags},
      {targetAddr, 0, 576, QBDI::MEMORY_WRITE, kXstateFlags},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("XSAVE", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("XSAVE", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 8;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-XSAVEOPT") {
  if (!checkFeature("xsaveopt")) {
    return;
  }
  const char source[] =
      "xor %eax, %eax\nxor %edx, %edx\nxsaveopt 0x40(%ebx,%esi,8)\n";
  alignas(64) uint8_t buffer[768] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[128];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 576, QBDI::MEMORY_READ, kXstateFlags},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 576, QBDI::MEMORY_READ, kXstateFlags},
      {targetAddr, 0, 576, QBDI::MEMORY_WRITE, kXstateFlags},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("XSAVEOPT", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("XSAVEOPT", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 8;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-XSAVEC") {
  if (!checkFeature("xsavec")) {
    return;
  }
  const char source[] =
      "xor %eax, %eax\nxor %edx, %edx\nxsavec 0x40(%ebx,%esi,8)\n";
  alignas(64) uint8_t buffer[768] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[128];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 576, QBDI::MEMORY_READ, kXstateFlags},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 576, QBDI::MEMORY_READ, kXstateFlags},
      {targetAddr, 0, 576, QBDI::MEMORY_WRITE, kXstateFlags},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("XSAVEC", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("XSAVEC", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 8;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-XRSTOR") {
  if (!checkFeature("xsave")) {
    return;
  }
  const char source[] =
      "xor %eax, %eax\nxor %edx, %edx\nxrstor 0x40(%ebx,%esi,8)\n";
  alignas(64) uint8_t buffer[768] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[128];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 576, QBDI::MEMORY_READ, kXstateFlags},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 576, QBDI::MEMORY_READ, kXstateFlags},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("XRSTOR", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("XRSTOR", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 8;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}
