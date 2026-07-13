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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-DEC64m") {
  const char source[] = "decq 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x1234567812345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x1234567812345677, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("DEC64m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("DEC64m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345677);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-DEC64m_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} decq 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x1234567812345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x1234567812345677, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("DEC64m_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("DEC64m_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345677);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-DEC64m_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "decq 0x11(%rbx,%rsi,4), %r8\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x1234567812345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("DEC64m_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("DEC64m_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x1234567812345677);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-DEC64m_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} decq 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x1234567812345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x1234567812345677, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("DEC64m_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("DEC64m_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345677);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-DEC64m_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} decq 0x11(%rbx,%rsi,4), %r8\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x1234567812345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("DEC64m_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("DEC64m_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x1234567812345677);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-INC64m") {
  const char source[] = "incq 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x1234567812345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x1234567812345679, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("INC64m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("INC64m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345679);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-INC64m_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} incq 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x1234567812345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x1234567812345679, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("INC64m_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("INC64m_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345679);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-INC64m_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "incq 0x11(%rbx,%rsi,4), %r8\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x1234567812345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("INC64m_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("INC64m_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x1234567812345679);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-INC64m_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} incq 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x1234567812345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x1234567812345679, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("INC64m_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("INC64m_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345679);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-INC64m_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} incq 0x11(%rbx,%rsi,4), %r8\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x1234567812345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("INC64m_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("INC64m_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x1234567812345679);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-NEG64m") {
  const char source[] = "negq 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x1234567812345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0xedcba987edcba988, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("NEG64m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("NEG64m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0xedcba987edcba988);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-NEG64m_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} negq 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x1234567812345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0xedcba987edcba988, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("NEG64m_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("NEG64m_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0xedcba987edcba988);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-NEG64m_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "negq 0x11(%rbx,%rsi,4), %r8\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x1234567812345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("NEG64m_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("NEG64m_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0xedcba987edcba988);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-NEG64m_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} negq 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x1234567812345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0xedcba987edcba988, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("NEG64m_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("NEG64m_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0xedcba987edcba988);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-NEG64m_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} negq 0x11(%rbx,%rsi,4), %r8\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x1234567812345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("NEG64m_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("NEG64m_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0xedcba987edcba988);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-NOT64m") {
  const char source[] = "notq 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x1234567812345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0xedcba987edcba987, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("NOT64m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("NOT64m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0xedcba987edcba987);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-NOT64m_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} notq 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x1234567812345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0xedcba987edcba987, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("NOT64m_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("NOT64m_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0xedcba987edcba987);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-NOT64m_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "notq 0x11(%rbx,%rsi,4), %r8\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x1234567812345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x1234567812345678, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("NOT64m_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("NOT64m_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0xedcba987edcba987);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-DIV64m") {
  const char source[] = "divq 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("DIV64m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("DIV64m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x000000000000000a;
  state->rdx = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000003);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-DIV64m_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} divq 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("DIV64m_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("DIV64m_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x000000000000000a;
  state->rdx = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000003);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-DIV64m_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} divq 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("DIV64m_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("DIV64m_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x000000000000000a;
  state->rdx = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000003);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-IDIV64m") {
  const char source[] = "idivq 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IDIV64m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IDIV64m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x000000000000000a;
  state->rdx = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000003);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-IDIV64m_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} idivq 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IDIV64m_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IDIV64m_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x000000000000000a;
  state->rdx = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000003);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-IDIV64m_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} idivq 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IDIV64m_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IDIV64m_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x000000000000000a;
  state->rdx = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000003);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-MUL64m") {
  const char source[] = "mulq 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MUL64m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MUL64m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x000000000000000a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000003);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-MUL64m_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} mulq 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MUL64m_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MUL64m_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x000000000000000a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000003);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-MUL64m_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} mulq 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MUL64m_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MUL64m_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x000000000000000a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000003);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-IMUL64m") {
  const char source[] = "imulq 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IMUL64m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IMUL64m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x000000000000000a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000003);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-IMUL64m_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} imulq 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IMUL64m_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IMUL64m_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x000000000000000a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000003);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-IMUL64m_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} imulq 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IMUL64m_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IMUL64m_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x000000000000000a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000003);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-IMUL64rm") {
  const char source[] = "imulq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IMUL64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IMUL64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x000000000000000a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000003);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-IMUL64rm_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} imulq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IMUL64rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IMUL64rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x000000000000000a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000003);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-IMUL64rm_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "imulq 0x11(%rbx,%rsi,4), %rax, %r8\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IMUL64rm_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IMUL64rm_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x000000000000000a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000003);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-IMUL64rm_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} imulq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IMUL64rm_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IMUL64rm_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x000000000000000a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000003);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-IMUL64rm_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} imulq 0x11(%rbx,%rsi,4), %rax, %r8\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IMUL64rm_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IMUL64rm_NF_ND", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x000000000000000a;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000003);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-IMUL64rmi32") {
  const char source[] = "imulq $0x05050505, 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IMUL64rmi32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IMUL64rmi32", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000003);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-IMUL64rmi32_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} imulq $0x05050505, 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IMUL64rmi32_EVEX", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("IMUL64rmi32_EVEX", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000003);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-IMUL64rmi32_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} imulq $0x05050505, 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IMUL64rmi32_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IMUL64rmi32_NF", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000003);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-IMUL64rmi8") {
  const char source[] = "imulq $0x5, 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IMUL64rmi8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IMUL64rmi8", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000003);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-IMUL64rmi8_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} imulq $0x5, 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IMUL64rmi8_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IMUL64rmi8_EVEX", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000003);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-IMUL64rmi8_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} imulq $0x5, 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IMUL64rmi8_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IMUL64rmi8_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000003);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-IMULZU64rmi32") {
  if (!checkFeature("zu")) {
    return;
  }
  const char source[] = "imulzuq $0x05050505, 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IMULZU64rmi32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IMULZU64rmi32", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000003);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-IMULZU64rmi8") {
  if (!checkFeature("zu")) {
    return;
  }
  const char source[] = "imulzuq $0x5, 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000003;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000003, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IMULZU64rmi8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IMULZU64rmi8", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000003);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}
