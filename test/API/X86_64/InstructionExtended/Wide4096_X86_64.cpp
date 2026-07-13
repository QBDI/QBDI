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
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-FXRSTOR") {
  const char source[] = "fxrstor 0x20(%rbx,%rsi,8)\n";
  alignas(16) uint8_t buffer[560] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[64];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 512, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 512, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("FXRSTOR", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("FXRSTOR", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-FXRSTOR64") {
  const char source[] = "fxrstor64 0x20(%rbx,%rsi,8)\n";
  alignas(16) uint8_t buffer[560] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[64];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 512, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 512, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("FXRSTOR64", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("FXRSTOR64", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-FXSAVE") {
  const char source[] = "fxsave 0x20(%rbx,%rsi,8)\n";
  alignas(16) uint8_t buffer[560] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[64];
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 512, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("FXSAVE", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-FXSAVE64") {
  const char source[] = "fxsave64 0x20(%rbx,%rsi,8)\n";
  alignas(16) uint8_t buffer[560] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[64];
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 512, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("FXSAVE64", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-MOVDIR64B32") {
  if (!QBDI::isHostCPUFeaturePresent("movdir64b")) {
    return;
  }
  const char source[] = "movdir64b 0x20(%ebx,%esi,8), %eax\n";
  alignas(64) uint8_t src[192] = {0};
  alignas(64) uint8_t dst[64] = {0};
  QBDI::rword srcAddr = (QBDI::rword)&src[96];
  QBDI::rword dstAddr = (QBDI::rword)&dst[0];
  ExpectedMemoryAccesses expectedPre = {{
      {srcAddr, 0, 512, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {srcAddr, 0, 512, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
      {dstAddr, 0, 512, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOVDIR64B32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MOVDIR64B32", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rax = dstAddr;
  state->rbx = (QBDI::rword)&src[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-MOVDIR64B32_EVEX") {
  if (!QBDI::isHostCPUFeaturePresent("movdir64b")) {
    return;
  }
  const char source[] = "{evex} movdir64b 0x20(%ebx,%esi,8), %eax\n";
  alignas(64) uint8_t src[192] = {0};
  alignas(64) uint8_t dst[64] = {0};
  QBDI::rword srcAddr = (QBDI::rword)&src[96];
  QBDI::rword dstAddr = (QBDI::rword)&dst[0];
  ExpectedMemoryAccesses expectedPre = {{
      {srcAddr, 0, 512, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {srcAddr, 0, 512, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
      {dstAddr, 0, 512, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOVDIR64B32_EVEX", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("MOVDIR64B32_EVEX", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rax = dstAddr;
  state->rbx = (QBDI::rword)&src[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-MOVDIR64B64") {
  if (!QBDI::isHostCPUFeaturePresent("movdir64b")) {
    return;
  }
  const char source[] = "movdir64b 0x20(%rbx,%rsi,8), %rax\n";
  alignas(64) uint8_t src[192] = {0};
  alignas(64) uint8_t dst[64] = {0};
  QBDI::rword srcAddr = (QBDI::rword)&src[96];
  QBDI::rword dstAddr = (QBDI::rword)&dst[0];
  ExpectedMemoryAccesses expectedPre = {{
      {srcAddr, 0, 512, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {srcAddr, 0, 512, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
      {dstAddr, 0, 512, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOVDIR64B64", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MOVDIR64B64", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rax = dstAddr;
  state->rbx = (QBDI::rword)&src[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-MOVDIR64B64_EVEX") {
  if (!QBDI::isHostCPUFeaturePresent("movdir64b")) {
    return;
  }
  const char source[] = "{evex} movdir64b 0x20(%rbx,%rsi,8), %rax\n";
  alignas(64) uint8_t src[192] = {0};
  alignas(64) uint8_t dst[64] = {0};
  QBDI::rword srcAddr = (QBDI::rword)&src[96];
  QBDI::rword dstAddr = (QBDI::rword)&dst[0];
  ExpectedMemoryAccesses expectedPre = {{
      {srcAddr, 0, 512, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {srcAddr, 0, 512, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
      {dstAddr, 0, 512, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOVDIR64B64_EVEX", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("MOVDIR64B64_EVEX", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rax = dstAddr;
  state->rbx = (QBDI::rword)&src[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}
