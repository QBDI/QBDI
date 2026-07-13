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
using QBDITestBatch2::checkEmptyAccess;
using QBDITestBatch2::checkFeature;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-DEC8m") {
  const char source[] = "decb 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x11;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x10, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("DEC8m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("DEC8m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x10);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-DEC8m_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} decb 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x11;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x10, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("DEC8m_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("DEC8m_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x10);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-DEC8m_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "decb 0x11(%rbx,%rsi,4), %r8b\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x11;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("DEC8m_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("DEC8m_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x11);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xff) == 0x10);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-DEC8m_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} decb 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x11;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x10, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("DEC8m_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("DEC8m_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x10);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-DEC8m_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} decb 0x11(%rbx,%rsi,4), %r8b\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x11;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("DEC8m_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("DEC8m_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x11);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xff) == 0x10);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-INC8m") {
  const char source[] = "incb 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x11;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x12, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("INC8m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("INC8m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x12);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-INC8m_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} incb 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x11;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x12, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("INC8m_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("INC8m_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x12);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-INC8m_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "incb 0x11(%rbx,%rsi,4), %r8b\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x11;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("INC8m_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("INC8m_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x11);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xff) == 0x12);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-INC8m_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} incb 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x11;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x12, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("INC8m_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("INC8m_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x12);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-INC8m_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} incb 0x11(%rbx,%rsi,4), %r8b\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x11;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("INC8m_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("INC8m_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x11);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xff) == 0x12);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-NEG8m") {
  const char source[] = "negb 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x11;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0xef, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("NEG8m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("NEG8m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0xef);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-NEG8m_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} negb 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x11;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0xef, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("NEG8m_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("NEG8m_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0xef);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-NEG8m_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "negb 0x11(%rbx,%rsi,4), %r8b\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x11;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("NEG8m_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("NEG8m_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x11);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xff) == 0xef);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-NEG8m_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} negb 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x11;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0xef, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("NEG8m_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("NEG8m_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0xef);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-NEG8m_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} negb 0x11(%rbx,%rsi,4), %r8b\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x11;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("NEG8m_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("NEG8m_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x11);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xff) == 0xef);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-NOT8m") {
  const char source[] = "notb 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x11;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0xee, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("NOT8m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("NOT8m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0xee);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-NOT8m_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} notb 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x11;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0xee, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("NOT8m_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("NOT8m_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0xee);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-NOT8m_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "notb 0x11(%rbx,%rsi,4), %r8b\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x11;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x11, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("NOT8m_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("NOT8m_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x11);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xff) == 0xee);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-DIV8m") {
  const char source[] = "divb 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x03;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("DIV8m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("DIV8m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x03);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-DIV8m_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} divb 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x03;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("DIV8m_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("DIV8m_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x03);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-DIV8m_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} divb 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x03;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("DIV8m_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("DIV8m_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x03);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-IDIV8m") {
  const char source[] = "idivb 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x03;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IDIV8m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IDIV8m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x03);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-IDIV8m_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} idivb 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x03;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IDIV8m_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IDIV8m_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x03);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-IDIV8m_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} idivb 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x03;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IDIV8m_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IDIV8m_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x03);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-IMUL8m") {
  const char source[] = "imulb 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x03;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IMUL8m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IMUL8m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x03);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-IMUL8m_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} imulb 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x03;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IMUL8m_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IMUL8m_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x03);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-IMUL8m_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} imulb 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x03;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IMUL8m_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IMUL8m_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x03);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-MUL8m") {
  const char source[] = "mulb 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x03;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MUL8m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MUL8m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x03);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-MUL8m_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} mulb 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x03;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MUL8m_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MUL8m_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x03);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-MUL8m_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} mulb 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x03;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x03, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MUL8m_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MUL8m_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x03);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}
