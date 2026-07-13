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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VLDDQUYrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vlddqu 0x11(%rbx,%rsi,4), %ymm0\n";
  alignas(32) uint8_t buffer[96] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VLDDQUYrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLDDQUYrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VMOVAPDYmr") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vmovapd %ymm0, 0x20(%rbx,%rsi,8)\n";
  alignas(32) uint8_t buffer[96] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[64];
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 32, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VMOVAPDYmr", QBDI::POSTINST, checkAccess, &expectedPost);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VMOVAPDYrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vmovapd 0x20(%rbx,%rsi,8), %ymm0\n";
  alignas(32) uint8_t buffer[96] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[64];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VMOVAPDYrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VMOVAPDYrm", QBDI::POSTINST, checkAccess, &expectedPost);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VMOVAPSYmr") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vmovaps %ymm0, 0x20(%rbx,%rsi,8)\n";
  alignas(32) uint8_t buffer[96] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[64];
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 32, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VMOVAPSYmr", QBDI::POSTINST, checkAccess, &expectedPost);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VMOVAPSYrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vmovaps 0x20(%rbx,%rsi,8), %ymm0\n";
  alignas(32) uint8_t buffer[96] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[64];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VMOVAPSYrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VMOVAPSYrm", QBDI::POSTINST, checkAccess, &expectedPost);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VMOVDDUPYrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vmovddup 0x11(%rbx,%rsi,4), %ymm0\n";
  alignas(32) uint8_t buffer[96] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VMOVDDUPYrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VMOVDDUPYrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VMOVDQAYmr") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vmovdqa %ymm0, 0x20(%rbx,%rsi,8)\n";
  alignas(32) uint8_t buffer[96] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[64];
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 32, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VMOVDQAYmr", QBDI::POSTINST, checkAccess, &expectedPost);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VMOVDQAYrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vmovdqa 0x20(%rbx,%rsi,8), %ymm0\n";
  alignas(32) uint8_t buffer[96] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[64];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VMOVDQAYrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VMOVDQAYrm", QBDI::POSTINST, checkAccess, &expectedPost);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VMOVDQUYmr") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vmovdqu %ymm0, 0x11(%rbx,%rsi,4)\n";
  alignas(32) uint8_t buffer[96] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 32, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VMOVDQUYmr", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VMOVDQUYrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vmovdqu 0x11(%rbx,%rsi,4), %ymm0\n";
  alignas(32) uint8_t buffer[96] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VMOVDQUYrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VMOVDQUYrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VMOVNTDQAYrm") {
  if (!checkFeature("avx2")) {
    return;
  }
  const char source[] = "vmovntdqa 0x20(%rbx,%rsi,8), %ymm0\n";
  alignas(32) uint8_t buffer[96] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[64];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VMOVNTDQAYrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VMOVNTDQAYrm", QBDI::POSTINST, checkAccess, &expectedPost);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VMOVNTDQYmr") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vmovntdq %ymm0, 0x20(%rbx,%rsi,8)\n";
  alignas(32) uint8_t buffer[96] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[64];
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 32, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VMOVNTDQYmr", QBDI::POSTINST, checkAccess, &expectedPost);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VMOVNTPDYmr") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vmovntpd %ymm0, 0x20(%rbx,%rsi,8)\n";
  alignas(32) uint8_t buffer[96] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[64];
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 32, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VMOVNTPDYmr", QBDI::POSTINST, checkAccess, &expectedPost);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VMOVNTPSYmr") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vmovntps %ymm0, 0x20(%rbx,%rsi,8)\n";
  alignas(32) uint8_t buffer[96] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[64];
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 32, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VMOVNTPSYmr", QBDI::POSTINST, checkAccess, &expectedPost);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VMOVSHDUPYrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vmovshdup 0x11(%rbx,%rsi,4), %ymm0\n";
  alignas(32) uint8_t buffer[96] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VMOVSHDUPYrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VMOVSHDUPYrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VMOVSLDUPYrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vmovsldup 0x11(%rbx,%rsi,4), %ymm0\n";
  alignas(32) uint8_t buffer[96] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VMOVSLDUPYrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VMOVSLDUPYrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VMOVUPDYmr") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vmovupd %ymm0, 0x11(%rbx,%rsi,4)\n";
  alignas(32) uint8_t buffer[96] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 32, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VMOVUPDYmr", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VMOVUPDYrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vmovupd 0x11(%rbx,%rsi,4), %ymm0\n";
  alignas(32) uint8_t buffer[96] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VMOVUPDYrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VMOVUPDYrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VMOVUPSYmr") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vmovups %ymm0, 0x11(%rbx,%rsi,4)\n";
  alignas(32) uint8_t buffer[96] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 32, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VMOVUPSYmr", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VMOVUPSYrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vmovups 0x11(%rbx,%rsi,4), %ymm0\n";
  alignas(32) uint8_t buffer[96] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VMOVUPSYrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VMOVUPSYrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}
