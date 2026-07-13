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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ADC64rm") {
  const char source[] = "adcq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0505050505050505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADC64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADC64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1010101010101010;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0x1515151515151515);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ADC64rm_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} adcq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0505050505050505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADC64rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADC64rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1010101010101010;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0x1515151515151515);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ADC64rm_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "adcq 0x11(%rbx,%rsi,4), %rax, %r8\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0505050505050505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADC64rm_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADC64rm_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1010101010101010;
  state->r8 = 0;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x1515151515151515);
  CHECK(finalState->rax == 0x1010101010101010);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ADD64rm") {
  const char source[] = "addq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0505050505050505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADD64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADD64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1010101010101010;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0x1515151515151515);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ADD64rm_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} addq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0505050505050505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADD64rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADD64rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1010101010101010;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0x1515151515151515);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ADD64rm_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "addq 0x11(%rbx,%rsi,4), %rax, %r8\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0505050505050505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADD64rm_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADD64rm_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1010101010101010;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x1515151515151515);
  CHECK(finalState->rax == 0x1010101010101010);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ADD64rm_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} addq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0505050505050505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADD64rm_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADD64rm_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1010101010101010;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0x1515151515151515);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ADD64rm_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} addq 0x11(%rbx,%rsi,4), %rax, %r8\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0505050505050505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADD64rm_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADD64rm_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1010101010101010;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x1515151515151515);
  CHECK(finalState->rax == 0x1010101010101010);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-AND64rm") {
  const char source[] = "andq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0f0f0f0f0f0f0f0f;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0f0f0f0f0f0f0f0f, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0f0f0f0f0f0f0f0f, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("AND64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("AND64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0xf3f3f3f3f3f3f3f3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0x0303030303030303);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-AND64rm_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} andq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0f0f0f0f0f0f0f0f;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0f0f0f0f0f0f0f0f, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0f0f0f0f0f0f0f0f, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("AND64rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("AND64rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0xf3f3f3f3f3f3f3f3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0x0303030303030303);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-AND64rm_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "andq 0x11(%rbx,%rsi,4), %rax, %r8\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0f0f0f0f0f0f0f0f;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0f0f0f0f0f0f0f0f, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0f0f0f0f0f0f0f0f, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("AND64rm_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("AND64rm_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0xf3f3f3f3f3f3f3f3;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x0303030303030303);
  CHECK(finalState->rax == 0xf3f3f3f3f3f3f3f3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-AND64rm_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} andq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0f0f0f0f0f0f0f0f;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0f0f0f0f0f0f0f0f, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0f0f0f0f0f0f0f0f, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("AND64rm_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("AND64rm_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0xf3f3f3f3f3f3f3f3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0x0303030303030303);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-AND64rm_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} andq 0x11(%rbx,%rsi,4), %rax, %r8\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0f0f0f0f0f0f0f0f;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0f0f0f0f0f0f0f0f, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0f0f0f0f0f0f0f0f, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("AND64rm_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("AND64rm_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0xf3f3f3f3f3f3f3f3;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x0303030303030303);
  CHECK(finalState->rax == 0xf3f3f3f3f3f3f3f3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-OR64rm") {
  const char source[] = "orq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0505050505050505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("OR64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("OR64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1010101010101010;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0x1515151515151515);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-OR64rm_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} orq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0505050505050505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("OR64rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("OR64rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1010101010101010;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0x1515151515151515);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-OR64rm_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "orq 0x11(%rbx,%rsi,4), %rax, %r8\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0505050505050505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("OR64rm_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("OR64rm_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1010101010101010;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x1515151515151515);
  CHECK(finalState->rax == 0x1010101010101010);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-OR64rm_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} orq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0505050505050505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("OR64rm_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("OR64rm_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1010101010101010;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0x1515151515151515);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-OR64rm_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} orq 0x11(%rbx,%rsi,4), %rax, %r8\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0505050505050505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("OR64rm_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("OR64rm_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1010101010101010;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x1515151515151515);
  CHECK(finalState->rax == 0x1010101010101010);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SBB64rm") {
  const char source[] = "sbbq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0505050505050505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SBB64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SBB64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x3030303030303030;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0x2b2b2b2b2b2b2b2b);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SBB64rm_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} sbbq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0505050505050505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SBB64rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SBB64rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x3030303030303030;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0x2b2b2b2b2b2b2b2b);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SBB64rm_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "sbbq 0x11(%rbx,%rsi,4), %rax, %r8\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0505050505050505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SBB64rm_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SBB64rm_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x3030303030303030;
  state->r8 = 0;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x2b2b2b2b2b2b2b2b);
  CHECK(finalState->rax == 0x3030303030303030);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SUB64rm") {
  const char source[] = "subq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0505050505050505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SUB64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SUB64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x3030303030303030;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0x2b2b2b2b2b2b2b2b);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SUB64rm_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} subq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0505050505050505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SUB64rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SUB64rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x3030303030303030;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0x2b2b2b2b2b2b2b2b);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SUB64rm_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "subq 0x11(%rbx,%rsi,4), %rax, %r8\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0505050505050505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SUB64rm_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SUB64rm_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x3030303030303030;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x2b2b2b2b2b2b2b2b);
  CHECK(finalState->rax == 0x3030303030303030);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SUB64rm_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} subq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0505050505050505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SUB64rm_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SUB64rm_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x3030303030303030;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0x2b2b2b2b2b2b2b2b);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SUB64rm_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} subq 0x11(%rbx,%rsi,4), %rax, %r8\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0505050505050505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SUB64rm_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SUB64rm_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x3030303030303030;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x2b2b2b2b2b2b2b2b);
  CHECK(finalState->rax == 0x3030303030303030);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-XOR64rm") {
  const char source[] = "xorq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0f0f0f0f0f0f0f0f;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0f0f0f0f0f0f0f0f, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0f0f0f0f0f0f0f0f, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("XOR64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("XOR64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x3333333333333333;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0x3c3c3c3c3c3c3c3c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-XOR64rm_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} xorq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0f0f0f0f0f0f0f0f;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0f0f0f0f0f0f0f0f, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0f0f0f0f0f0f0f0f, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("XOR64rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("XOR64rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x3333333333333333;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0x3c3c3c3c3c3c3c3c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-XOR64rm_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "xorq 0x11(%rbx,%rsi,4), %rax, %r8\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0f0f0f0f0f0f0f0f;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0f0f0f0f0f0f0f0f, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0f0f0f0f0f0f0f0f, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("XOR64rm_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("XOR64rm_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x3333333333333333;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x3c3c3c3c3c3c3c3c);
  CHECK(finalState->rax == 0x3333333333333333);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-XOR64rm_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} xorq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0f0f0f0f0f0f0f0f;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0f0f0f0f0f0f0f0f, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0f0f0f0f0f0f0f0f, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("XOR64rm_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("XOR64rm_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x3333333333333333;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0x3c3c3c3c3c3c3c3c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-XOR64rm_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} xorq 0x11(%rbx,%rsi,4), %rax, %r8\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0f0f0f0f0f0f0f0f;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0f0f0f0f0f0f0f0f, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0f0f0f0f0f0f0f0f, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("XOR64rm_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("XOR64rm_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x3333333333333333;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r8 == 0x3c3c3c3c3c3c3c3c);
  CHECK(finalState->rax == 0x3333333333333333);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}
