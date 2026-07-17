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
#include <catch2/catch_test_macros.hpp>
#include "API/APITest.h"
#include "MemAccessTestUtils_ARM.h"

#include "Engine/LLVMCPU.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/MC/MCInst.h"

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::checkFeature;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

static bool isSwpFamilyDecodable() {
  QBDI::LLVMCPU probe("", "arm", {});
  llvm::MCInst inst;
  uint64_t size;
  const uint8_t bytes[] = {0x91, 0x20, 0x00, 0xe1}; // swp r2, r1, [r0]
  return probe.getInstruction(inst, size, llvm::ArrayRef<uint8_t>(bytes, 4), 0);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-swp") {
  if (!isSwpFamilyDecodable()) {
    return;
  }
  const char source[] = "swp r2, r1, [r0]\n";

  uint8_t buf[4] = {0x11, 0x22, 0x33, 0x44};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[0], 0x44332211, 4,
                           QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = {{(QBDI::rword)&buf[0], 0x44332211, 4,
                            QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
                           {(QBDI::rword)&buf[0], 0xaabbccdd, 4,
                            QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  vm.addMnemonicCB("SWP", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SWP", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xaabbccdd;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r2 == 0x44332211);
  CHECK(*(uint32_t *)&buf[0] == 0xaabbccdd);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldri12") {
  const char source[] = "ldr r1, [r0, #4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8b8a8988, 4,
                           QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("LDRi12", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRi12", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x8b8a8988);
  CHECK(finalState->r0 == (QBDI::rword)&buf[4]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrrs") {
  const char source[] = "ldr r1, [r0, r2]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8b8a8988, 4,
                           QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("LDRrs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRrs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r2 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x8b8a8988);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrrs_lsl2") {
  const char source[] = "ldr r1, [r0, r2, lsl #2]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8b8a8988, 4,
                           QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("LDRrs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRrs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 2;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x8b8a8988);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrrs_lsr2") {
  const char source[] = "ldr r1, [r0, r2, lsr #2]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8b8a8988, 4,
                           QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("LDRrs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRrs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 32;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x8b8a8988);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrrs_asr2") {
  const char source[] = "ldr r1, [r0, r2, asr #2]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8b8a8988, 4,
                           QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("LDRrs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRrs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 32;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x8b8a8988);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrrs_ror2") {
  const char source[] = "ldr r1, [r0, r2, ror #2]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8b8a8988, 4,
                           QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("LDRrs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRrs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 32;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x8b8a8988);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldr_pre_imm") {
  const char source[] = "ldr r1, [r0, #4]!\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8b8a8988, 4,
                           QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("LDR_PRE_IMM", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDR_PRE_IMM", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x8b8a8988);
  CHECK(finalState->r0 == (QBDI::rword)&buf[8]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldr_pre_reg") {
  const char source[] = "ldr r1, [r0, r2]!\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8b8a8988, 4,
                           QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("LDR_PRE_REG", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDR_PRE_REG", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r2 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x8b8a8988);
  CHECK(finalState->r0 == (QBDI::rword)&buf[8]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldr_post_imm") {
  const char source[] = "ldr r1, [r0], #4\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x87868584, 4,
                           QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("LDR_POST_IMM", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDR_POST_IMM", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x87868584);
  CHECK(finalState->r0 == (QBDI::rword)&buf[8]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldr_post_reg") {
  const char source[] = "ldr r1, [r0], r2\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x87868584, 4,
                           QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("LDR_POST_REG", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDR_POST_REG", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r2 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x87868584);
  CHECK(finalState->r0 == (QBDI::rword)&buf[8]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrt_post_imm") {
  const char source[] = "ldrt r1, [r0], #4\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x87868584, 4,
                           QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("LDRT_POST_IMM", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRT_POST_IMM", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x87868584);
  CHECK(finalState->r0 == (QBDI::rword)&buf[8]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrt_post_reg") {
  const char source[] = "ldrt r1, [r0], r2\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x87868584, 4,
                           QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("LDRT_POST_REG", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRT_POST_REG", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r2 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x87868584);
  CHECK(finalState->r0 == (QBDI::rword)&buf[8]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-stri12") {
  const char source[] = "str r1, [r0, #4]\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xaabbccdd, 4,
                            QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  vm.addMnemonicCB("STRi12", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRi12", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xaabbccdd;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[0]);
  CHECK(*(uint32_t *)&buf[4] == 0xaabbccdd);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strrs") {
  const char source[] = "str r1, [r0, r2]\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xaabbccdd, 4,
                            QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  vm.addMnemonicCB("STRrs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRrs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xaabbccdd;
  state->r2 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[4] == 0xaabbccdd);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strrs_lsl2") {
  const char source[] = "str r1, [r0, r2, lsl #2]\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[8], 0xaabbccdd, 4,
                            QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  vm.addMnemonicCB("STRrs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRrs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xaabbccdd;
  state->r2 = 2;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[8] == 0xaabbccdd);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strrs_lsr2") {
  const char source[] = "str r1, [r0, r2, lsr #2]\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[8], 0xaabbccdd, 4,
                            QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  vm.addMnemonicCB("STRrs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRrs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xaabbccdd;
  state->r2 = 32;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[8] == 0xaabbccdd);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strrs_asr2") {
  const char source[] = "str r1, [r0, r2, asr #2]\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[8], 0xaabbccdd, 4,
                            QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  vm.addMnemonicCB("STRrs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRrs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xaabbccdd;
  state->r2 = 32;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[8] == 0xaabbccdd);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strrs_ror2") {
  const char source[] = "str r1, [r0, r2, ror #2]\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[8], 0xaabbccdd, 4,
                            QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  vm.addMnemonicCB("STRrs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRrs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xaabbccdd;
  state->r2 = 32;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[8] == 0xaabbccdd);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-str_pre_imm") {
  const char source[] = "str r1, [r0, #4]!\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xaabbccdd, 4,
                            QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  vm.addMnemonicCB("STR_PRE_IMM", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STR_PRE_IMM", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xaabbccdd;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[4]);
  CHECK(*(uint32_t *)&buf[4] == 0xaabbccdd);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-str_pre_reg") {
  const char source[] = "str r1, [r0, r2]!\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xaabbccdd, 4,
                            QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  vm.addMnemonicCB("STR_PRE_REG", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STR_PRE_REG", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xaabbccdd;
  state->r2 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[4]);
  CHECK(*(uint32_t *)&buf[4] == 0xaabbccdd);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-str_post_imm") {
  const char source[] = "str r1, [r0], #4\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[0], 0xaabbccdd, 4,
                            QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  vm.addMnemonicCB("STR_POST_IMM", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STR_POST_IMM", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xaabbccdd;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[4]);
  CHECK(*(uint32_t *)&buf[0] == 0xaabbccdd);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-str_post_reg") {
  const char source[] = "str r1, [r0], r2\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[0], 0xaabbccdd, 4,
                            QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  vm.addMnemonicCB("STR_POST_REG", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STR_POST_REG", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xaabbccdd;
  state->r2 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[4]);
  CHECK(*(uint32_t *)&buf[0] == 0xaabbccdd);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strt_post_imm") {
  const char source[] = "strt r1, [r0], #4\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[0], 0xaabbccdd, 4,
                            QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  vm.addMnemonicCB("STRT_POST_IMM", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRT_POST_IMM", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xaabbccdd;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[4]);
  CHECK(*(uint32_t *)&buf[0] == 0xaabbccdd);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strt_post_reg") {
  const char source[] = "strt r1, [r0], r2\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[0], 0xaabbccdd, 4,
                            QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  vm.addMnemonicCB("STRT_POST_REG", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRT_POST_REG", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xaabbccdd;
  state->r2 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[4]);
  CHECK(*(uint32_t *)&buf[0] == 0xaabbccdd);
}
