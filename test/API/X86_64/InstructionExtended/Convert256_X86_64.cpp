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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VCVTDQ2PSYrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vcvtdq2ps 0x11(%rbx,%rsi,4), %ymm0\n";
  alignas(32) uint8_t buffer[96] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VCVTDQ2PSYrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VCVTDQ2PSYrm", QBDI::POSTINST, checkAccess, &expectedPost);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VCVTNEEBF162PSYrm") {
  if (!checkFeature("avxneconvert")) {
    return;
  }
  const char source[] = "vcvtneebf162ps 0x11(%rbx,%rsi,4), %ymm0\n";
  alignas(32) uint8_t buffer[96] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VCVTNEEBF162PSYrm", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VCVTNEEBF162PSYrm", QBDI::POSTINST, checkAccess,
                   &expectedPost);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VCVTNEEPH2PSYrm") {
  if (!checkFeature("avxneconvert")) {
    return;
  }
  const char source[] = "vcvtneeph2ps 0x11(%rbx,%rsi,4), %ymm0\n";
  alignas(32) uint8_t buffer[96] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VCVTNEEPH2PSYrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VCVTNEEPH2PSYrm", QBDI::POSTINST, checkAccess,
                   &expectedPost);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VCVTNEOBF162PSYrm") {
  if (!checkFeature("avxneconvert")) {
    return;
  }
  const char source[] = "vcvtneobf162ps 0x11(%rbx,%rsi,4), %ymm0\n";
  alignas(32) uint8_t buffer[96] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VCVTNEOBF162PSYrm", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VCVTNEOBF162PSYrm", QBDI::POSTINST, checkAccess,
                   &expectedPost);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VCVTNEOPH2PSYrm") {
  if (!checkFeature("avxneconvert")) {
    return;
  }
  const char source[] = "vcvtneoph2ps 0x11(%rbx,%rsi,4), %ymm0\n";
  alignas(32) uint8_t buffer[96] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VCVTNEOPH2PSYrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VCVTNEOPH2PSYrm", QBDI::POSTINST, checkAccess,
                   &expectedPost);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VCVTPD2DQYrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vcvtpd2dqy 0x11(%rbx,%rsi,4), %xmm0\n";
  alignas(32) uint8_t buffer[96] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VCVTPD2DQYrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VCVTPD2DQYrm", QBDI::POSTINST, checkAccess, &expectedPost);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VCVTPD2PSYrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vcvtpd2psy 0x11(%rbx,%rsi,4), %xmm0\n";
  alignas(32) uint8_t buffer[96] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VCVTPD2PSYrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VCVTPD2PSYrm", QBDI::POSTINST, checkAccess, &expectedPost);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VCVTPS2DQYrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vcvtps2dq 0x11(%rbx,%rsi,4), %ymm0\n";
  alignas(32) uint8_t buffer[96] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VCVTPS2DQYrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VCVTPS2DQYrm", QBDI::POSTINST, checkAccess, &expectedPost);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VCVTTPD2DQYrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vcvttpd2dqy 0x11(%rbx,%rsi,4), %xmm0\n";
  alignas(32) uint8_t buffer[96] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VCVTTPD2DQYrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VCVTTPD2DQYrm", QBDI::POSTINST, checkAccess, &expectedPost);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VCVTTPS2DQYrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vcvttps2dq 0x11(%rbx,%rsi,4), %ymm0\n";
  alignas(32) uint8_t buffer[96] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 32, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VCVTTPS2DQYrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VCVTTPS2DQYrm", QBDI::POSTINST, checkAccess, &expectedPost);
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
