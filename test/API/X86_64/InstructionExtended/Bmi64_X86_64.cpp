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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ANDN64rm") {
  if (!checkFeature("bmi")) {
    return;
  }
  const char source[] = "andnq 0x11(%rbx,%rsi,4), %rax, %rcx\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x000000000000000f;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x000000000000000f, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x000000000000000f, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ANDN64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ANDN64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x00000000000000f0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x000000000000000f);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ANDN64rm_EVEX") {
  if (!checkFeature("bmi")) {
    return;
  }
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} andnq 0x11(%rbx,%rsi,4), %rax, %rcx\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x000000000000000f;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x000000000000000f, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x000000000000000f, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ANDN64rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ANDN64rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x00000000000000f0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x000000000000000f);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-BEXTR64rm") {
  if (!checkFeature("bmi")) {
    return;
  }
  const char source[] = "bextrq %rax, 0x11(%rbx,%rsi,4), %rcx\n";
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
  vm.addMnemonicCB("BEXTR64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BEXTR64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0000000000000408;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-BEXTR64rm_EVEX") {
  if (!checkFeature("bmi")) {
    return;
  }
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} bextrq %rax, 0x11(%rbx,%rsi,4), %rcx\n";
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
  vm.addMnemonicCB("BEXTR64rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BEXTR64rm_EVEX", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0000000000000408;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-BEXTR64rm_NF") {
  if (!checkFeature("bmi")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} bextrq %rax, 0x11(%rbx,%rsi,4), %rcx\n";
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
  vm.addMnemonicCB("BEXTR64rm_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BEXTR64rm_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0000000000000408;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-BEXTRI64mi") {
  if (!checkFeature("tbm")) {
    return;
  }
  const char source[] = "bextrq $0x0408, 0x11(%rbx,%rsi,4), %rax\n";
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
  vm.addMnemonicCB("BEXTRI64mi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BEXTRI64mi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-BSF64rm") {
  const char source[] = "bsfq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000008;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000008, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000008, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BSF64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BSF64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000008);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-BSR64rm") {
  const char source[] = "bsrq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000008;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000008, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000008, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BSR64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BSR64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000008);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-BZHI64rm") {
  if (!checkFeature("bmi2")) {
    return;
  }
  const char source[] = "bzhiq %rax, 0x11(%rbx,%rsi,4), %rcx\n";
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
  vm.addMnemonicCB("BZHI64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BZHI64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0000000000000010;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-BZHI64rm_EVEX") {
  if (!checkFeature("bmi2")) {
    return;
  }
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} bzhiq %rax, 0x11(%rbx,%rsi,4), %rcx\n";
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
  vm.addMnemonicCB("BZHI64rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BZHI64rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0000000000000010;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-BZHI64rm_NF") {
  if (!checkFeature("bmi2")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} bzhiq %rax, 0x11(%rbx,%rsi,4), %rcx\n";
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
  vm.addMnemonicCB("BZHI64rm_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BZHI64rm_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0000000000000010;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-LZCNT64rm") {
  if (!checkFeature("lzcnt")) {
    return;
  }
  const char source[] = "lzcntq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000008;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000008, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000008, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LZCNT64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LZCNT64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000008);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-LZCNT64rm_EVEX") {
  if (!checkFeature("lzcnt")) {
    return;
  }
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} lzcntq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000008;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000008, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000008, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LZCNT64rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LZCNT64rm_EVEX", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000008);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-LZCNT64rm_NF") {
  if (!checkFeature("lzcnt")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} lzcntq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000008;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000008, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000008, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LZCNT64rm_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LZCNT64rm_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000008);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-MULX64rm") {
  if (!checkFeature("bmi2")) {
    return;
  }
  const char source[] = "mulxq 0x11(%rbx,%rsi,4), %rcx, %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x000000000000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MULX64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MULX64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rdx = 0x0000000000000003;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x000000000000000c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-MULX64rm_EVEX") {
  if (!checkFeature("bmi2")) {
    return;
  }
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} mulxq 0x11(%rbx,%rsi,4), %rcx, %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x000000000000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MULX64rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MULX64rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rdx = 0x0000000000000003;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x000000000000000c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PDEP64rm") {
  if (!checkFeature("bmi2")) {
    return;
  }
  const char source[] = "pdepq 0x11(%rbx,%rsi,4), %rax, %rcx\n";
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
  vm.addMnemonicCB("PDEP64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PDEP64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0f0f0f0f0f0f0f0f;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PDEP64rm_EVEX") {
  if (!checkFeature("bmi2")) {
    return;
  }
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} pdepq 0x11(%rbx,%rsi,4), %rax, %rcx\n";
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
  vm.addMnemonicCB("PDEP64rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PDEP64rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0f0f0f0f0f0f0f0f;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PEXT64rm") {
  if (!checkFeature("bmi2")) {
    return;
  }
  const char source[] = "pextq 0x11(%rbx,%rsi,4), %rax, %rcx\n";
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
  vm.addMnemonicCB("PEXT64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PEXT64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0f0f0f0f0f0f0f0f;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PEXT64rm_EVEX") {
  if (!checkFeature("bmi2")) {
    return;
  }
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} pextq 0x11(%rbx,%rsi,4), %rax, %rcx\n";
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
  vm.addMnemonicCB("PEXT64rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PEXT64rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x0f0f0f0f0f0f0f0f;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x1234567812345678);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-POPCNT64rm") {
  if (!checkFeature("popcnt")) {
    return;
  }
  const char source[] = "popcntq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000008;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000008, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000008, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("POPCNT64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("POPCNT64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000008);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-POPCNT64rm_EVEX") {
  if (!checkFeature("popcnt")) {
    return;
  }
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} popcntq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000008;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000008, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000008, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("POPCNT64rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("POPCNT64rm_EVEX", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000008);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-POPCNT64rm_NF") {
  if (!checkFeature("popcnt")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} popcntq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000008;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000008, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000008, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("POPCNT64rm_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("POPCNT64rm_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000008);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-T1MSKC64rm") {
  if (!checkFeature("tbm")) {
    return;
  }
  const char source[] = "t1mskcq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x000000000000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("T1MSKC64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("T1MSKC64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x000000000000000c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-TZCNT64rm") {
  if (!checkFeature("bmi")) {
    return;
  }
  const char source[] = "tzcntq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000008;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000008, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000008, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("TZCNT64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("TZCNT64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000008);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-TZCNT64rm_EVEX") {
  if (!checkFeature("bmi")) {
    return;
  }
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} tzcntq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000008;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000008, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000008, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("TZCNT64rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("TZCNT64rm_EVEX", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000008);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-TZCNT64rm_NF") {
  if (!checkFeature("bmi")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} tzcntq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000008;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000008, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000008, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("TZCNT64rm_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("TZCNT64rm_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000008);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-TZMSK64rm") {
  if (!checkFeature("tbm")) {
    return;
  }
  const char source[] = "tzmskq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x000000000000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("TZMSK64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("TZMSK64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x000000000000000c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-BLCFILL64rm") {
  if (!checkFeature("tbm")) {
    return;
  }
  const char source[] = "blcfillq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x000000000000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BLCFILL64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BLCFILL64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x000000000000000c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-BLCI64rm") {
  if (!checkFeature("tbm")) {
    return;
  }
  const char source[] = "blciq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x000000000000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BLCI64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BLCI64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x000000000000000c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-BLCIC64rm") {
  if (!checkFeature("tbm")) {
    return;
  }
  const char source[] = "blcicq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x000000000000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BLCIC64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BLCIC64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x000000000000000c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-BLCMSK64rm") {
  if (!checkFeature("tbm")) {
    return;
  }
  const char source[] = "blcmskq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x000000000000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BLCMSK64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BLCMSK64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x000000000000000c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-BLCS64rm") {
  if (!checkFeature("tbm")) {
    return;
  }
  const char source[] = "blcsq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x000000000000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BLCS64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BLCS64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x000000000000000c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-BLSFILL64rm") {
  if (!checkFeature("tbm")) {
    return;
  }
  const char source[] = "blsfillq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x000000000000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BLSFILL64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BLSFILL64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x000000000000000c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-BLSIC64rm") {
  if (!checkFeature("tbm")) {
    return;
  }
  const char source[] = "blsicq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x000000000000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BLSIC64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BLSIC64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x000000000000000c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-BLSI64rm") {
  if (!checkFeature("bmi")) {
    return;
  }
  const char source[] = "blsiq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x000000000000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BLSI64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BLSI64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x000000000000000c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-BLSI64rm_EVEX") {
  if (!checkFeature("bmi")) {
    return;
  }
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} blsiq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x000000000000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BLSI64rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BLSI64rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x000000000000000c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-BLSI64rm_NF") {
  if (!checkFeature("bmi")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} blsiq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x000000000000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BLSI64rm_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BLSI64rm_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x000000000000000c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-BLSMSK64rm") {
  if (!checkFeature("bmi")) {
    return;
  }
  const char source[] = "blsmskq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x000000000000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BLSMSK64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BLSMSK64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x000000000000000c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-BLSMSK64rm_EVEX") {
  if (!checkFeature("bmi")) {
    return;
  }
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} blsmskq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x000000000000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BLSMSK64rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BLSMSK64rm_EVEX", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x000000000000000c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-BLSMSK64rm_NF") {
  if (!checkFeature("bmi")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} blsmskq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x000000000000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BLSMSK64rm_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BLSMSK64rm_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x000000000000000c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-BLSR64rm") {
  if (!checkFeature("bmi")) {
    return;
  }
  const char source[] = "blsrq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x000000000000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BLSR64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BLSR64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x000000000000000c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-BLSR64rm_EVEX") {
  if (!checkFeature("bmi")) {
    return;
  }
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} blsrq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x000000000000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BLSR64rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BLSR64rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x000000000000000c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-BLSR64rm_NF") {
  if (!checkFeature("bmi")) {
    return;
  }
  if (!checkFeature("nf")) {
    return;
  }
  const char source[] = "{nf} blsrq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x000000000000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x000000000000000c, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BLSR64rm_NF", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BLSR64rm_NF", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x000000000000000c);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ADCX64rm") {
  if (!checkFeature("adx")) {
    return;
  }
  const char source[] = "adcxq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000005;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000005, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000005, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADCX64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADCX64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x000000000000000a;
  state->eflags &= ~0x901;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000005);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax) == 0x000000000000000f);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ADCX64rm_EVEX") {
  if (!checkFeature("adx")) {
    return;
  }
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} adcxq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000005;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000005, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000005, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADCX64rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADCX64rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x000000000000000a;
  state->eflags &= ~0x901;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000005);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax) == 0x000000000000000f);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ADCX64rm_ND") {
  if (!checkFeature("adx")) {
    return;
  }
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "adcxq 0x11(%rbx,%rsi,4), %rax, %r8\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000005;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000005, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000005, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADCX64rm_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADCX64rm_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x000000000000000a;
  state->r8 = 0;
  state->eflags &= ~0x901;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000005);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8) == 0x000000000000000f);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ADOX64rm") {
  if (!checkFeature("adx")) {
    return;
  }
  const char source[] = "adoxq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000005;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000005, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000005, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADOX64rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADOX64rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x000000000000000a;
  state->eflags &= ~0x901;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000005);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax) == 0x000000000000000f);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ADOX64rm_EVEX") {
  if (!checkFeature("adx")) {
    return;
  }
  if (!checkFeature("egpr")) {
    return;
  }
  const char source[] = "{evex} adoxq 0x11(%rbx,%rsi,4), %rax\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000005;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000005, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000005, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADOX64rm_EVEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADOX64rm_EVEX", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x000000000000000a;
  state->eflags &= ~0x901;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000005);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->rax) == 0x000000000000000f);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ADOX64rm_ND") {
  if (!checkFeature("adx")) {
    return;
  }
  if (!checkFeature("ndd")) {
    return;
  }
  const char source[] = "adoxq 0x11(%rbx,%rsi,4), %rax, %r8\n";
  uint8_t buffer[48] = {0};
  uint64_t *target = reinterpret_cast<uint64_t *>(&buffer[21]);
  *target = 0x0000000000000005;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000000000005, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000000000005, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADOX64rm_ND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADOX64rm_ND", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  state->rax = 0x000000000000000a;
  state->r8 = 0;
  state->eflags &= ~0x901;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000000000005);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->r8) == 0x000000000000000f);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}
