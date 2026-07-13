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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-RCL64m1") {
  const char source[] = "rclq 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x2468acf02468acf0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("RCL64m1", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCL64m1", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x2468acf02468acf0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-RCL64mCL") {
  const char source[] = "rclq %cl, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x91a2b3c091a2b3c0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("RCL64mCL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCL64mCL", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x91a2b3c091a2b3c0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-RCL64mi") {
  const char source[] = "rclq $0x5, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x468acf02468acf01, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("RCL64mi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCL64mi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x468acf02468acf01);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-RCL64m1_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} rclq 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x2468acf02468acf0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("RCL64m1_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCL64m1_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x2468acf02468acf0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-RCL64mCL_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} rclq %cl, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x91a2b3c091a2b3c0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("RCL64mCL_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCL64mCL_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x91a2b3c091a2b3c0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-RCL64mi_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} rclq $0x5, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x468acf02468acf01, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("RCL64mi_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCL64mi_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x468acf02468acf01);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-RCL64m1_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "rclq 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("RCL64m1_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCL64m1_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->r8 = 0;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x2468acf02468acf0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-RCL64mCL_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "rclq %cl, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("RCL64mCL_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCL64mCL_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  state->r8 = 0;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x91a2b3c091a2b3c0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-RCL64mi_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "rclq $0x5, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("RCL64mi_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCL64mi_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->r8 = 0;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x468acf02468acf01);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-RCR64m1") {
  const char source[] = "rcrq 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x091a2b3c091a2b3c, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("RCR64m1", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCR64m1", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x091a2b3c091a2b3c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-RCR64mCL") {
  const char source[] = "rcrq %cl, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x02468acf02468acf, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("RCR64mCL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCR64mCL", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x02468acf02468acf);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-RCR64mi") {
  const char source[] = "rcrq $0x5, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x8091a2b3c091a2b3, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("RCR64mi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCR64mi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x8091a2b3c091a2b3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-RCR64m1_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} rcrq 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x091a2b3c091a2b3c, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("RCR64m1_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCR64m1_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x091a2b3c091a2b3c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-RCR64mCL_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} rcrq %cl, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x02468acf02468acf, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("RCR64mCL_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCR64mCL_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x02468acf02468acf);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-RCR64mi_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} rcrq $0x5, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x8091a2b3c091a2b3, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("RCR64mi_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCR64mi_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x8091a2b3c091a2b3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-RCR64m1_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "rcrq 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("RCR64m1_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCR64m1_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->r8 = 0;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x091a2b3c091a2b3c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-RCR64mCL_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "rcrq %cl, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("RCR64mCL_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCR64mCL_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  state->r8 = 0;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x02468acf02468acf);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-RCR64mi_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "rcrq $0x5, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("RCR64mi_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCR64mi_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->r8 = 0;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x8091a2b3c091a2b3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROL64m1") {
  const char source[] = "rolq 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x2468acf02468acf0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ROL64m1", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROL64m1", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x2468acf02468acf0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROL64mCL") {
  const char source[] = "rolq %cl, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x91a2b3c091a2b3c0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ROL64mCL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROL64mCL", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x91a2b3c091a2b3c0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROL64mi") {
  const char source[] = "rolq $0x5, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x468acf02468acf02, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ROL64mi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROL64mi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x468acf02468acf02);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROL64m1_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} rolq 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x2468acf02468acf0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ROL64m1_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROL64m1_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x2468acf02468acf0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROL64mCL_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} rolq %cl, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x91a2b3c091a2b3c0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ROL64mCL_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROL64mCL_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x91a2b3c091a2b3c0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROL64mi_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} rolq $0x5, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x468acf02468acf02, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ROL64mi_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROL64mi_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x468acf02468acf02);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROL64m1_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "rolq 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("ROL64m1_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROL64m1_ND", QBDI::POSTINST, checkAccess, &expectedPost);
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
  CHECK(finalState->r8 == 0x2468acf02468acf0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROL64mCL_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "rolq %cl, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("ROL64mCL_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROL64mCL_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x91a2b3c091a2b3c0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROL64mi_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "rolq $0x5, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("ROL64mi_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROL64mi_ND", QBDI::POSTINST, checkAccess, &expectedPost);
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
  CHECK(finalState->r8 == 0x468acf02468acf02);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROL64m1_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} rolq 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x2468acf02468acf0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ROL64m1_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROL64m1_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x2468acf02468acf0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROL64mCL_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} rolq %cl, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x91a2b3c091a2b3c0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ROL64mCL_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROL64mCL_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x91a2b3c091a2b3c0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROL64mi_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} rolq $0x5, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x468acf02468acf02, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ROL64mi_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROL64mi_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x468acf02468acf02);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROL64m1_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} rolq 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("ROL64m1_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROL64m1_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
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
  CHECK(finalState->r8 == 0x2468acf02468acf0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROL64mCL_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} rolq %cl, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("ROL64mCL_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROL64mCL_NF_ND", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x91a2b3c091a2b3c0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROL64mi_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} rolq $0x5, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("ROL64mi_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROL64mi_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
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
  CHECK(finalState->r8 == 0x468acf02468acf02);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROR64m1") {
  const char source[] = "rorq 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x091a2b3c091a2b3c, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ROR64m1", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROR64m1", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x091a2b3c091a2b3c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROR64mCL") {
  const char source[] = "rorq %cl, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x02468acf02468acf, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ROR64mCL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROR64mCL", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x02468acf02468acf);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROR64mi") {
  const char source[] = "rorq $0x5, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0xc091a2b3c091a2b3, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ROR64mi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROR64mi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0xc091a2b3c091a2b3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROR64m1_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} rorq 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x091a2b3c091a2b3c, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ROR64m1_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROR64m1_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x091a2b3c091a2b3c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROR64mCL_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} rorq %cl, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x02468acf02468acf, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ROR64mCL_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROR64mCL_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x02468acf02468acf);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROR64mi_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} rorq $0x5, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0xc091a2b3c091a2b3, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ROR64mi_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROR64mi_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0xc091a2b3c091a2b3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROR64m1_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "rorq 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("ROR64m1_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROR64m1_ND", QBDI::POSTINST, checkAccess, &expectedPost);
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
  CHECK(finalState->r8 == 0x091a2b3c091a2b3c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROR64mCL_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "rorq %cl, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("ROR64mCL_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROR64mCL_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x02468acf02468acf);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROR64mi_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "rorq $0x5, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("ROR64mi_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROR64mi_ND", QBDI::POSTINST, checkAccess, &expectedPost);
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
  CHECK(finalState->r8 == 0xc091a2b3c091a2b3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROR64m1_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} rorq 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x091a2b3c091a2b3c, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ROR64m1_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROR64m1_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x091a2b3c091a2b3c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROR64mCL_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} rorq %cl, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x02468acf02468acf, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ROR64mCL_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROR64mCL_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x02468acf02468acf);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROR64mi_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} rorq $0x5, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0xc091a2b3c091a2b3, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ROR64mi_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROR64mi_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0xc091a2b3c091a2b3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROR64m1_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} rorq 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("ROR64m1_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROR64m1_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
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
  CHECK(finalState->r8 == 0x091a2b3c091a2b3c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROR64mCL_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} rorq %cl, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("ROR64mCL_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROR64mCL_NF_ND", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x02468acf02468acf);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROR64mi_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} rorq $0x5, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("ROR64mi_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROR64mi_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
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
  CHECK(finalState->r8 == 0xc091a2b3c091a2b3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SAR64m1") {
  const char source[] = "sarq 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x091a2b3c091a2b3c, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SAR64m1", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SAR64m1", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x091a2b3c091a2b3c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SAR64mCL") {
  const char source[] = "sarq %cl, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x02468acf02468acf, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SAR64mCL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SAR64mCL", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x02468acf02468acf);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SAR64mi") {
  const char source[] = "sarq $0x5, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x0091a2b3c091a2b3, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SAR64mi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SAR64mi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0091a2b3c091a2b3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SAR64m1_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} sarq 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x091a2b3c091a2b3c, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SAR64m1_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SAR64m1_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x091a2b3c091a2b3c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SAR64mCL_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} sarq %cl, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x02468acf02468acf, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SAR64mCL_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SAR64mCL_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x02468acf02468acf);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SAR64mi_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} sarq $0x5, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x0091a2b3c091a2b3, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SAR64mi_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SAR64mi_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0091a2b3c091a2b3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SAR64m1_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "sarq 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("SAR64m1_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SAR64m1_ND", QBDI::POSTINST, checkAccess, &expectedPost);
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
  CHECK(finalState->r8 == 0x091a2b3c091a2b3c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SAR64mCL_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "sarq %cl, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("SAR64mCL_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SAR64mCL_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x02468acf02468acf);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SAR64mi_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "sarq $0x5, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("SAR64mi_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SAR64mi_ND", QBDI::POSTINST, checkAccess, &expectedPost);
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
  CHECK(finalState->r8 == 0x0091a2b3c091a2b3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SAR64m1_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} sarq 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x091a2b3c091a2b3c, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SAR64m1_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SAR64m1_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x091a2b3c091a2b3c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SAR64mCL_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} sarq %cl, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x02468acf02468acf, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SAR64mCL_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SAR64mCL_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x02468acf02468acf);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SAR64mi_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} sarq $0x5, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x0091a2b3c091a2b3, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SAR64mi_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SAR64mi_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0091a2b3c091a2b3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SAR64m1_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} sarq 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("SAR64m1_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SAR64m1_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
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
  CHECK(finalState->r8 == 0x091a2b3c091a2b3c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SAR64mCL_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} sarq %cl, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("SAR64mCL_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SAR64mCL_NF_ND", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x02468acf02468acf);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SAR64mi_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} sarq $0x5, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("SAR64mi_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SAR64mi_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
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
  CHECK(finalState->r8 == 0x0091a2b3c091a2b3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHL64m1") {
  const char source[] = "shlq 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x2468acf02468acf0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHL64m1", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHL64m1", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x2468acf02468acf0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHL64mCL") {
  const char source[] = "shlq %cl, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x91a2b3c091a2b3c0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHL64mCL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHL64mCL", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x91a2b3c091a2b3c0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHL64mi") {
  const char source[] = "shlq $0x5, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x468acf02468acf00, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHL64mi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHL64mi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x468acf02468acf00);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHL64m1_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} shlq 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x2468acf02468acf0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHL64m1_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHL64m1_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x2468acf02468acf0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHL64mCL_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} shlq %cl, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x91a2b3c091a2b3c0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHL64mCL_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHL64mCL_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x91a2b3c091a2b3c0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHL64mi_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} shlq $0x5, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x468acf02468acf00, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHL64mi_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHL64mi_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x468acf02468acf00);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHL64m1_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "shlq 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("SHL64m1_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHL64m1_ND", QBDI::POSTINST, checkAccess, &expectedPost);
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
  CHECK(finalState->r8 == 0x2468acf02468acf0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHL64mCL_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "shlq %cl, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("SHL64mCL_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHL64mCL_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x91a2b3c091a2b3c0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHL64mi_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "shlq $0x5, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("SHL64mi_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHL64mi_ND", QBDI::POSTINST, checkAccess, &expectedPost);
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
  CHECK(finalState->r8 == 0x468acf02468acf00);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHL64m1_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} shlq 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x2468acf02468acf0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHL64m1_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHL64m1_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x2468acf02468acf0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHL64mCL_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} shlq %cl, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x91a2b3c091a2b3c0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHL64mCL_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHL64mCL_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x91a2b3c091a2b3c0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHL64mi_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} shlq $0x5, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x468acf02468acf00, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHL64mi_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHL64mi_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x468acf02468acf00);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHL64m1_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} shlq 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("SHL64m1_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHL64m1_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
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
  CHECK(finalState->r8 == 0x2468acf02468acf0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHL64mCL_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} shlq %cl, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("SHL64mCL_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHL64mCL_NF_ND", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x91a2b3c091a2b3c0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHL64mi_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} shlq $0x5, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("SHL64mi_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHL64mi_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
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
  CHECK(finalState->r8 == 0x468acf02468acf00);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHR64m1") {
  const char source[] = "shrq 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x091a2b3c091a2b3c, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHR64m1", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHR64m1", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x091a2b3c091a2b3c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHR64mCL") {
  const char source[] = "shrq %cl, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x02468acf02468acf, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHR64mCL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHR64mCL", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x02468acf02468acf);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHR64mi") {
  const char source[] = "shrq $0x5, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x0091a2b3c091a2b3, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHR64mi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHR64mi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0091a2b3c091a2b3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHR64m1_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} shrq 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x091a2b3c091a2b3c, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHR64m1_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHR64m1_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x091a2b3c091a2b3c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHR64mCL_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} shrq %cl, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x02468acf02468acf, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHR64mCL_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHR64mCL_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x02468acf02468acf);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHR64mi_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} shrq $0x5, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x0091a2b3c091a2b3, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHR64mi_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHR64mi_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0091a2b3c091a2b3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHR64m1_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "shrq 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("SHR64m1_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHR64m1_ND", QBDI::POSTINST, checkAccess, &expectedPost);
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
  CHECK(finalState->r8 == 0x091a2b3c091a2b3c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHR64mCL_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "shrq %cl, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("SHR64mCL_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHR64mCL_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x02468acf02468acf);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHR64mi_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "shrq $0x5, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("SHR64mi_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHR64mi_ND", QBDI::POSTINST, checkAccess, &expectedPost);
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
  CHECK(finalState->r8 == 0x0091a2b3c091a2b3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHR64m1_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} shrq 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x091a2b3c091a2b3c, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHR64m1_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHR64m1_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x091a2b3c091a2b3c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHR64mCL_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} shrq %cl, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x02468acf02468acf, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHR64mCL_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHR64mCL_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x02468acf02468acf);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHR64mi_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} shrq $0x5, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x0091a2b3c091a2b3, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHR64mi_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHR64mi_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0091a2b3c091a2b3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHR64m1_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} shrq 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("SHR64m1_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHR64m1_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
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
  CHECK(finalState->r8 == 0x091a2b3c091a2b3c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHR64mCL_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} shrq %cl, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("SHR64mCL_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHR64mCL_NF_ND", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x02468acf02468acf);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHR64mi_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} shrq $0x5, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("SHR64mi_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHR64mi_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
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
  CHECK(finalState->r8 == 0x0091a2b3c091a2b3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHLD64mrCL") {
  const char source[] = "shldq %cl, %rax, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x91a2b3c091a2b3c0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHLD64mrCL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHLD64mrCL", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1111111111111111;
  state->rcx = 0x3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x91a2b3c091a2b3c0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHLD64mri8") {
  const char source[] = "shldq $0x5, %rax, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x468acf02468acf02, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHLD64mri8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHLD64mri8", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1111111111111111;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x468acf02468acf02);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHLD64mrCL_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} shldq %cl, %rax, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x91a2b3c091a2b3c0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHLD64mrCL_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHLD64mrCL_EVEX", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1111111111111111;
  state->rcx = 0x3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x91a2b3c091a2b3c0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHLD64mri8_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} shldq $0x5, %rax, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x468acf02468acf02, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHLD64mri8_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHLD64mri8_EVEX", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1111111111111111;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x468acf02468acf02);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHLD64mrCL_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "shldq %cl, %rax, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("SHLD64mrCL_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHLD64mrCL_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1111111111111111;
  state->rcx = 0x3;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x91a2b3c091a2b3c0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHLD64mri8_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "shldq $0x5, %rax, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("SHLD64mri8_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHLD64mri8_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1111111111111111;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x468acf02468acf02);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHLD64mrCL_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} shldq %cl, %rax, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x91a2b3c091a2b3c0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHLD64mrCL_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHLD64mrCL_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1111111111111111;
  state->rcx = 0x3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x91a2b3c091a2b3c0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHLD64mri8_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} shldq $0x5, %rax, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x468acf02468acf02, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHLD64mri8_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHLD64mri8_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1111111111111111;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x468acf02468acf02);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHLD64mrCL_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} shldq %cl, %rax, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("SHLD64mrCL_NF_ND", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("SHLD64mrCL_NF_ND", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1111111111111111;
  state->rcx = 0x3;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x91a2b3c091a2b3c0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHLD64mri8_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} shldq $0x5, %rax, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("SHLD64mri8_NF_ND", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("SHLD64mri8_NF_ND", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1111111111111111;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x468acf02468acf02);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHRD64mrCL") {
  const char source[] = "shrdq %cl, %rax, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x22468acf02468acf, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHRD64mrCL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHRD64mrCL", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1111111111111111;
  state->rcx = 0x3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x22468acf02468acf);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHRD64mri8") {
  const char source[] = "shrdq $0x5, %rax, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x8891a2b3c091a2b3, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHRD64mri8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHRD64mri8", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1111111111111111;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x8891a2b3c091a2b3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHRD64mrCL_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} shrdq %cl, %rax, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x22468acf02468acf, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHRD64mrCL_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHRD64mrCL_EVEX", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1111111111111111;
  state->rcx = 0x3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x22468acf02468acf);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHRD64mri8_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} shrdq $0x5, %rax, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x8891a2b3c091a2b3, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHRD64mri8_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHRD64mri8_EVEX", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1111111111111111;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x8891a2b3c091a2b3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHRD64mrCL_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "shrdq %cl, %rax, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("SHRD64mrCL_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHRD64mrCL_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1111111111111111;
  state->rcx = 0x3;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x22468acf02468acf);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHRD64mri8_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "shrdq $0x5, %rax, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("SHRD64mri8_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHRD64mri8_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1111111111111111;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x8891a2b3c091a2b3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHRD64mrCL_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} shrdq %cl, %rax, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x22468acf02468acf, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHRD64mrCL_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHRD64mrCL_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1111111111111111;
  state->rcx = 0x3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x22468acf02468acf);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHRD64mri8_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} shrdq $0x5, %rax, 0x11(%rbx,%rsi,4)\n";
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
      {targetAddr, 0x8891a2b3c091a2b3, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHRD64mri8_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHRD64mri8_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1111111111111111;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x8891a2b3c091a2b3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHRD64mrCL_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} shrdq %cl, %rax, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("SHRD64mrCL_NF_ND", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("SHRD64mrCL_NF_ND", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1111111111111111;
  state->rcx = 0x3;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x22468acf02468acf);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHRD64mri8_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} shrdq $0x5, %rax, 0x11(%rbx,%rsi,4), %r8\n";
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
  vm.addMnemonicCB("SHRD64mri8_NF_ND", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("SHRD64mri8_NF_ND", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1111111111111111;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x8891a2b3c091a2b3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHLX64rm") {
  if (!checkFeature("bmi2")) {
    return;
  }
  const char source[] = "shlxq %rcx, 0x11(%rbx,%rsi,4), %rax\n";
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
  vm.addMnemonicCB("SHLX64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHLX64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  state->rax = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0x91a2b3c091a2b3c0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHLX64rm_EVEX") {
  if (!checkFeature("bmi2")) {
    return;
  }
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} shlxq %rcx, 0x11(%rbx,%rsi,4), %rax\n";
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
  vm.addMnemonicCB("SHLX64rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHLX64rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  state->rax = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0x91a2b3c091a2b3c0);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHRX64rm") {
  if (!checkFeature("bmi2")) {
    return;
  }
  const char source[] = "shrxq %rcx, 0x11(%rbx,%rsi,4), %rax\n";
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
  vm.addMnemonicCB("SHRX64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHRX64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  state->rax = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0x02468acf02468acf);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHRX64rm_EVEX") {
  if (!checkFeature("bmi2")) {
    return;
  }
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} shrxq %rcx, 0x11(%rbx,%rsi,4), %rax\n";
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
  vm.addMnemonicCB("SHRX64rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHRX64rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  state->rax = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0x02468acf02468acf);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SARX64rm") {
  if (!checkFeature("bmi2")) {
    return;
  }
  const char source[] = "sarxq %rcx, 0x11(%rbx,%rsi,4), %rax\n";
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
  vm.addMnemonicCB("SARX64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SARX64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  state->rax = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0x02468acf02468acf);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SARX64rm_EVEX") {
  if (!checkFeature("bmi2")) {
    return;
  }
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} sarxq %rcx, 0x11(%rbx,%rsi,4), %rax\n";
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
  vm.addMnemonicCB("SARX64rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SARX64rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rcx = 0x3;
  state->rax = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0x02468acf02468acf);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-RORX64mi") {
  if (!checkFeature("bmi2")) {
    return;
  }
  const char source[] = "rorxq $0x5, 0x11(%rbx,%rsi,4), %rax\n";
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
  vm.addMnemonicCB("RORX64mi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RORX64mi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0xc091a2b3c091a2b3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-RORX64mi_EVEX") {
  if (!checkFeature("bmi2")) {
    return;
  }
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} rorxq $0x5, 0x11(%rbx,%rsi,4), %rax\n";
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
  vm.addMnemonicCB("RORX64mi_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RORX64mi_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0xc091a2b3c091a2b3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}
