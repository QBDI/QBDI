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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ADC8rm") {
  const char source[] = "adcb 0x11(%rbx,%rsi,4), %al\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x05;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADC8rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADC8rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x10;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x05);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xff) == 0x15);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ADC8rm_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} adcb 0x11(%rbx,%rsi,4), %al\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x05;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADC8rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADC8rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x10;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x05);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xff) == 0x15);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ADC8rm_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "adcb 0x11(%rbx,%rsi,4), %al, %r8b\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x05;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADC8rm_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADC8rm_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x10;
  state->r8 = 0;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x05);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xff) == 0x15);
  CHECK((finalState->rax & 0xff) == 0x10);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ADD8rm") {
  const char source[] = "addb 0x11(%rbx,%rsi,4), %al\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x05;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADD8rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADD8rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x10;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x05);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xff) == 0x15);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ADD8rm_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} addb 0x11(%rbx,%rsi,4), %al\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x05;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADD8rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADD8rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x10;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x05);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xff) == 0x15);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ADD8rm_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "addb 0x11(%rbx,%rsi,4), %al, %r8b\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x05;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADD8rm_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADD8rm_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x10;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x05);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xff) == 0x15);
  CHECK((finalState->rax & 0xff) == 0x10);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ADD8rm_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} addb 0x11(%rbx,%rsi,4), %al\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x05;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADD8rm_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADD8rm_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x10;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x05);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xff) == 0x15);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ADD8rm_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} addb 0x11(%rbx,%rsi,4), %al, %r8b\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x05;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADD8rm_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADD8rm_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x10;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x05);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xff) == 0x15);
  CHECK((finalState->rax & 0xff) == 0x10);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-AND8rm") {
  const char source[] = "andb 0x11(%rbx,%rsi,4), %al\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x0f;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0f, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0f, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("AND8rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("AND8rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0xf3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x0f);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xff) == 0x03);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-AND8rm_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} andb 0x11(%rbx,%rsi,4), %al\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x0f;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0f, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0f, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("AND8rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("AND8rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0xf3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x0f);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xff) == 0x03);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-AND8rm_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "andb 0x11(%rbx,%rsi,4), %al, %r8b\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x0f;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0f, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0f, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("AND8rm_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("AND8rm_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0xf3;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x0f);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xff) == 0x03);
  CHECK((finalState->rax & 0xff) == 0xf3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-AND8rm_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} andb 0x11(%rbx,%rsi,4), %al\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x0f;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0f, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0f, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("AND8rm_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("AND8rm_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0xf3;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x0f);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xff) == 0x03);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-AND8rm_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} andb 0x11(%rbx,%rsi,4), %al, %r8b\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x0f;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0f, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0f, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("AND8rm_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("AND8rm_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0xf3;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x0f);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xff) == 0x03);
  CHECK((finalState->rax & 0xff) == 0xf3);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-OR8rm") {
  const char source[] = "orb 0x11(%rbx,%rsi,4), %al\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x05;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("OR8rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("OR8rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x10;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x05);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xff) == 0x15);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-OR8rm_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} orb 0x11(%rbx,%rsi,4), %al\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x05;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("OR8rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("OR8rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x10;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x05);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xff) == 0x15);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-OR8rm_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "orb 0x11(%rbx,%rsi,4), %al, %r8b\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x05;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("OR8rm_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("OR8rm_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x10;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x05);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xff) == 0x15);
  CHECK((finalState->rax & 0xff) == 0x10);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-OR8rm_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} orb 0x11(%rbx,%rsi,4), %al\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x05;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("OR8rm_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("OR8rm_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x10;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x05);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xff) == 0x15);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-OR8rm_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} orb 0x11(%rbx,%rsi,4), %al, %r8b\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x05;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("OR8rm_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("OR8rm_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x10;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x05);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xff) == 0x15);
  CHECK((finalState->rax & 0xff) == 0x10);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SBB8rm") {
  const char source[] = "sbbb 0x11(%rbx,%rsi,4), %al\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x05;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SBB8rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SBB8rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x30;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x05);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xff) == 0x2b);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SBB8rm_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} sbbb 0x11(%rbx,%rsi,4), %al\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x05;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SBB8rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SBB8rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x30;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x05);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xff) == 0x2b);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SBB8rm_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "sbbb 0x11(%rbx,%rsi,4), %al, %r8b\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x05;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SBB8rm_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SBB8rm_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x30;
  state->r8 = 0;
  state->eflags &= ~1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x05);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xff) == 0x2b);
  CHECK((finalState->rax & 0xff) == 0x30);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SUB8rm") {
  const char source[] = "subb 0x11(%rbx,%rsi,4), %al\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x05;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SUB8rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SUB8rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x30;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x05);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xff) == 0x2b);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SUB8rm_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} subb 0x11(%rbx,%rsi,4), %al\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x05;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SUB8rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SUB8rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x30;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x05);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xff) == 0x2b);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SUB8rm_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "subb 0x11(%rbx,%rsi,4), %al, %r8b\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x05;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SUB8rm_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SUB8rm_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x30;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x05);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xff) == 0x2b);
  CHECK((finalState->rax & 0xff) == 0x30);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SUB8rm_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} subb 0x11(%rbx,%rsi,4), %al\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x05;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SUB8rm_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SUB8rm_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x30;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x05);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xff) == 0x2b);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SUB8rm_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} subb 0x11(%rbx,%rsi,4), %al, %r8b\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x05;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x05, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SUB8rm_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SUB8rm_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x30;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x05);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xff) == 0x2b);
  CHECK((finalState->rax & 0xff) == 0x30);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-XOR8rm") {
  const char source[] = "xorb 0x11(%rbx,%rsi,4), %al\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x0f;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0f, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0f, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("XOR8rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("XOR8rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x33;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x0f);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xff) == 0x3c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-XOR8rm_EVEX") {
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} xorb 0x11(%rbx,%rsi,4), %al\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x0f;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0f, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0f, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("XOR8rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("XOR8rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x33;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x0f);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xff) == 0x3c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-XOR8rm_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "xorb 0x11(%rbx,%rsi,4), %al, %r8b\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x0f;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0f, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0f, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("XOR8rm_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("XOR8rm_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x33;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x0f);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xff) == 0x3c);
  CHECK((finalState->rax & 0xff) == 0x33);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-XOR8rm_NF") {
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} xorb 0x11(%rbx,%rsi,4), %al\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x0f;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0f, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0f, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("XOR8rm_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("XOR8rm_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x33;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x0f);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax & 0xff) == 0x3c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-XOR8rm_NF_ND") {
  if (!checkFeature("ndd")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} xorb 0x11(%rbx,%rsi,4), %al, %r8b\n";
  uint8_t buffer[40] = {0};
  buffer[21] = 0x0f;
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0f, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0f, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("XOR8rm_NF_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("XOR8rm_NF_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x33;
  state->r8 = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(buffer[21] == 0x0f);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8 & 0xff) == 0x3c);
  CHECK((finalState->rax & 0xff) == 0x33);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}
