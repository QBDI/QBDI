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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-AADD64mr") {
  if (!checkFeature("raoint")) {
    return;
  }
  const char source[] = "aaddq %rax, 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x1010101010101010;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x1010101010101010, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x1010101010101010, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x1515151515151515, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("AADD64mr", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("AADD64mr", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0505050505050505;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1515151515151515);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-AADD64mr_EVEX") {
  if (!checkFeature("raoint")) {
    return;
  }
  const char source[] = "{evex} aaddq %rax, 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x1010101010101010;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x1010101010101010, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x1010101010101010, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x1515151515151515, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("AADD64mr_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("AADD64mr_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0505050505050505;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1515151515151515);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-AAND64mr") {
  if (!checkFeature("raoint")) {
    return;
  }
  const char source[] = "aandq %rax, 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0xf3f3f3f3f3f3f3f3;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0xf3f3f3f3f3f3f3f3, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0xf3f3f3f3f3f3f3f3, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x0303030303030303, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("AAND64mr", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("AAND64mr", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0f0f0f0f0f0f0f0f;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0303030303030303);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-AAND64mr_EVEX") {
  if (!checkFeature("raoint")) {
    return;
  }
  const char source[] = "{evex} aandq %rax, 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0xf3f3f3f3f3f3f3f3;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0xf3f3f3f3f3f3f3f3, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0xf3f3f3f3f3f3f3f3, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x0303030303030303, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("AAND64mr_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("AAND64mr_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0f0f0f0f0f0f0f0f;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0303030303030303);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-AOR64mr") {
  if (!checkFeature("raoint")) {
    return;
  }
  const char source[] = "aorq %rax, 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x1010101010101010;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x1010101010101010, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x1010101010101010, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x1515151515151515, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("AOR64mr", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("AOR64mr", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0505050505050505;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1515151515151515);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-AOR64mr_EVEX") {
  if (!checkFeature("raoint")) {
    return;
  }
  const char source[] = "{evex} aorq %rax, 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x1010101010101010;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x1010101010101010, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x1010101010101010, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x1515151515151515, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("AOR64mr_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("AOR64mr_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0505050505050505;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1515151515151515);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-AXOR64mr") {
  if (!checkFeature("raoint")) {
    return;
  }
  const char source[] = "axorq %rax, 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x3333333333333333;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x3333333333333333, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x3333333333333333, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x3c3c3c3c3c3c3c3c, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("AXOR64mr", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("AXOR64mr", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0f0f0f0f0f0f0f0f;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x3c3c3c3c3c3c3c3c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-AXOR64mr_EVEX") {
  if (!checkFeature("raoint")) {
    return;
  }
  const char source[] = "{evex} axorq %rax, 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x3333333333333333;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x3333333333333333, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x3333333333333333, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x3c3c3c3c3c3c3c3c, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("AXOR64mr_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("AXOR64mr_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0f0f0f0f0f0f0f0f;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x3c3c3c3c3c3c3c3c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-XADD64rm") {
  const char source[] = "xaddq %rax, 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x1010101010101010;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x1010101010101010, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x1010101010101010, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x1515151515151515, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("XADD64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("XADD64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0505050505050505;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1515151515151515);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0x1010101010101010);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-XCHG64rm") {
  const char source[] = "xchgq %rax, 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x1010101010101010;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x1010101010101010, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x1010101010101010, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("XCHG64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("XCHG64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0505050505050505;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0505050505050505);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0x1010101010101010);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-CMPXCHG64rm") {
  const char source[] = "cmpxchgq %rcx, 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x1010101010101010;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x1010101010101010, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x1010101010101010, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("CMPXCHG64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("CMPXCHG64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1010101010101010;
  state->rcx = 0x0505050505050505;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0505050505050505);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0x1010101010101010);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-CMPCCXADDmr64") {
  if (!checkFeature("cmpccxadd")) {
    return;
  }
  const char source[] = "cmpexadd %rcx, %rax, 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x1010101010101010;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x1010101010101010, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x1010101010101010, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x1515151515151515, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("CMPCCXADDmr64", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("CMPCCXADDmr64", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1010101010101010;
  state->rcx = 0x0505050505050505;
  state->eflags |= 0x40;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1515151515151515);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0x1010101010101010);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-CMPCCXADDmr64_EVEX") {
  if (!checkFeature("cmpccxadd")) {
    return;
  }
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} cmpexadd %rcx, %rax, 0x11(%rbx,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x1010101010101010;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x1010101010101010, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x1010101010101010, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x1515151515151515, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("CMPCCXADDmr64_EVEX", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("CMPCCXADDmr64_EVEX", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1010101010101010;
  state->rcx = 0x0505050505050505;
  state->eflags |= 0x40;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1515151515151515);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->rax == 0x1010101010101010);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-CMPXCHG8B") {
  if (!checkFeature("cx8")) {
    return;
  }
  const char source[] = "cmpxchg8b 0x11(%rdi,%rsi,4)\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x1010101010101010;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x1010101010101010, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x1010101010101010, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {targetAddr, 0x0505050505050505, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("CMPXCHG8B", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("CMPXCHG8B", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rdi = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x10101010;
  state->rdx = 0x10101010;
  state->rbx = 0x05050505;
  state->rcx = 0x05050505;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0505050505050505);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}
