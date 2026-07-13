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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ADC16rm") {
  const char source[] = "adcw 0x11(%rbx,%rsi,4), %ax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADC16rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADC16rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1010;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x505);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xffff) == 0x1515);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ADC16rm_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} adcw 0x11(%rbx,%rsi,4), %ax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADC16rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADC16rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1010;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x505);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xffff) == 0x1515);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ADC16rm_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "adcw 0x11(%rbx,%rsi,4), %ax, %r8w\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADC16rm_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADC16rm_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1010;
  state->r8 = 0;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x505);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xffff) == 0x1515);
  CHECK((finalState->rax & 0xffff) == 0x1010);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ADD16rm") {
  const char source[] = "addw 0x11(%rbx,%rsi,4), %ax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADD16rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADD16rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1010;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x505);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xffff) == 0x1515);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ADD16rm_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} addw 0x11(%rbx,%rsi,4), %ax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADD16rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADD16rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1010;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x505);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xffff) == 0x1515);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ADD16rm_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "addw 0x11(%rbx,%rsi,4), %ax, %r8w\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADD16rm_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADD16rm_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1010;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x505);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xffff) == 0x1515);
  CHECK((finalState->rax & 0xffff) == 0x1010);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ADD16rm_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} addw 0x11(%rbx,%rsi,4), %ax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADD16rm_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADD16rm_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1010;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x505);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xffff) == 0x1515);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ADD16rm_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} addw 0x11(%rbx,%rsi,4), %ax, %r8w\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADD16rm_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADD16rm_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1010;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x505);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xffff) == 0x1515);
  CHECK((finalState->rax & 0xffff) == 0x1010);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-AND16rm") {
  const char source[] = "andw 0x11(%rbx,%rsi,4), %ax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0xf0f;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0xf0f, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0xf0f, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("AND16rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("AND16rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0xf3f3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0xf0f);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xffff) == 0x303);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-AND16rm_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} andw 0x11(%rbx,%rsi,4), %ax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0xf0f;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0xf0f, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0xf0f, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("AND16rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("AND16rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0xf3f3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0xf0f);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xffff) == 0x303);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-AND16rm_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "andw 0x11(%rbx,%rsi,4), %ax, %r8w\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0xf0f;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0xf0f, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0xf0f, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("AND16rm_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("AND16rm_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0xf3f3;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0xf0f);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xffff) == 0x303);
  CHECK((finalState->rax & 0xffff) == 0xf3f3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-AND16rm_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} andw 0x11(%rbx,%rsi,4), %ax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0xf0f;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0xf0f, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0xf0f, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("AND16rm_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("AND16rm_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0xf3f3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0xf0f);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xffff) == 0x303);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-AND16rm_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} andw 0x11(%rbx,%rsi,4), %ax, %r8w\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0xf0f;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0xf0f, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0xf0f, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("AND16rm_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("AND16rm_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0xf3f3;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0xf0f);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xffff) == 0x303);
  CHECK((finalState->rax & 0xffff) == 0xf3f3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-OR16rm") {
  const char source[] = "orw 0x11(%rbx,%rsi,4), %ax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("OR16rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("OR16rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1010;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x505);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xffff) == 0x1515);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-OR16rm_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} orw 0x11(%rbx,%rsi,4), %ax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("OR16rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("OR16rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1010;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x505);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xffff) == 0x1515);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-OR16rm_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "orw 0x11(%rbx,%rsi,4), %ax, %r8w\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("OR16rm_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("OR16rm_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1010;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x505);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xffff) == 0x1515);
  CHECK((finalState->rax & 0xffff) == 0x1010);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-OR16rm_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} orw 0x11(%rbx,%rsi,4), %ax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("OR16rm_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("OR16rm_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1010;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x505);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xffff) == 0x1515);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-OR16rm_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} orw 0x11(%rbx,%rsi,4), %ax, %r8w\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("OR16rm_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("OR16rm_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x1010;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x505);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xffff) == 0x1515);
  CHECK((finalState->rax & 0xffff) == 0x1010);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SBB16rm") {
  const char source[] = "sbbw 0x11(%rbx,%rsi,4), %ax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SBB16rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SBB16rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x3030;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x505);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xffff) == 0x2b2b);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SBB16rm_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} sbbw 0x11(%rbx,%rsi,4), %ax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SBB16rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SBB16rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x3030;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x505);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xffff) == 0x2b2b);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SBB16rm_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "sbbw 0x11(%rbx,%rsi,4), %ax, %r8w\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SBB16rm_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SBB16rm_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x3030;
  state->r8 = 0;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x505);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xffff) == 0x2b2b);
  CHECK((finalState->rax & 0xffff) == 0x3030);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SUB16rm") {
  const char source[] = "subw 0x11(%rbx,%rsi,4), %ax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SUB16rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SUB16rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x3030;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x505);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xffff) == 0x2b2b);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SUB16rm_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} subw 0x11(%rbx,%rsi,4), %ax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SUB16rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SUB16rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x3030;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x505);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xffff) == 0x2b2b);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SUB16rm_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "subw 0x11(%rbx,%rsi,4), %ax, %r8w\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SUB16rm_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SUB16rm_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x3030;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x505);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xffff) == 0x2b2b);
  CHECK((finalState->rax & 0xffff) == 0x3030);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SUB16rm_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} subw 0x11(%rbx,%rsi,4), %ax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SUB16rm_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SUB16rm_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x3030;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x505);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xffff) == 0x2b2b);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SUB16rm_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} subw 0x11(%rbx,%rsi,4), %ax, %r8w\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x505;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x505, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SUB16rm_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SUB16rm_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x3030;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x505);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xffff) == 0x2b2b);
  CHECK((finalState->rax & 0xffff) == 0x3030);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-XOR16rm") {
  const char source[] = "xorw 0x11(%rbx,%rsi,4), %ax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0xf0f;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0xf0f, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0xf0f, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("XOR16rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("XOR16rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x3333;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0xf0f);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xffff) == 0x3c3c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-XOR16rm_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} xorw 0x11(%rbx,%rsi,4), %ax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0xf0f;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0xf0f, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0xf0f, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("XOR16rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("XOR16rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x3333;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0xf0f);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xffff) == 0x3c3c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-XOR16rm_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "xorw 0x11(%rbx,%rsi,4), %ax, %r8w\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0xf0f;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0xf0f, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0xf0f, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("XOR16rm_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("XOR16rm_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x3333;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0xf0f);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xffff) == 0x3c3c);
  CHECK((finalState->rax & 0xffff) == 0x3333);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-XOR16rm_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} xorw 0x11(%rbx,%rsi,4), %ax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0xf0f;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0xf0f, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0xf0f, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("XOR16rm_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("XOR16rm_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x3333;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0xf0f);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xffff) == 0x3c3c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-XOR16rm_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} xorw 0x11(%rbx,%rsi,4), %ax, %r8w\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0xf0f;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0xf0f, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0xf0f, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("XOR16rm_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("XOR16rm_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x3333;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0xf0f);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xffff) == 0x3c3c);
  CHECK((finalState->rax & 0xffff) == 0x3333);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}
