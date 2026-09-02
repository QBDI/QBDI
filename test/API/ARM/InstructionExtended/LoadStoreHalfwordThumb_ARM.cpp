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

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::checkFeature;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tldrhi") {
  const char source[] = "ldrh r1, [r0, #4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8988, 2, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tLDRHi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("tLDRHi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x8988);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tldrhr") {
  const char source[] = "ldrh r1, [r0, r2]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8988, 2, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tLDRHr", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("tLDRHr", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r2 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x8988);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tldrsh") {
  const char source[] = "ldrsh r1, [r0, r2]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8988, 2, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tLDRSH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("tLDRSH", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r2 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0xffff8988);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tstrhi") {
  const char source[] = "strh r1, [r0, #4]\n";

  uint8_t buf[16] = {0};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xabcd, 2, QBDI::MEMORY_WRITE,
                            QBDI::MEMORY_NO_FLAGS}};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tSTRHi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("tSTRHi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xabcd;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == 0xcd);
  CHECK(buf[5] == 0xab);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tstrhr") {
  const char source[] = "strh r1, [r0, r2]\n";

  uint8_t buf[16] = {0};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xabcd, 2, QBDI::MEMORY_WRITE,
                            QBDI::MEMORY_NO_FLAGS}};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tSTRHr", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("tSTRHr", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xabcd;
  state->r2 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == 0xcd);
  CHECK(buf[5] == 0xab);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrhi12") {
  const char source[] = "ldrh.w r1, [r0, #4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8988, 2, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRHi12", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRHi12", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x8988);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrhi8") {
  const char source[] = "ldrh.w r1, [r0, #-4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x8584, 2, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRHi8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRHi8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[8];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x8584);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrhpci") {
  const char source[] =
      "b 1f\n"
      "label:\n"
      "  .byte 0x34, 0x12\n"
      "1:\n"
      "  ldrh.w r1, label\n";

  bool seen = false, seenPost = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRHpci", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].value == 0x1234);
                     CHECK(accesses[0].size == 2);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("t2LDRHpci", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].value == 0x1234);
                     CHECK(accesses[0].size == 2);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(gprState->r1 == 0x1234);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seen);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrh_post") {
  const char source[] = "ldrh.w r1, [r0], #4\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x8584, 2, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRH_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRH_POST", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x8584);
  CHECK(finalState->r0 == (QBDI::rword)&buf[8]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrh_pre") {
  const char source[] = "ldrh.w r1, [r0, #4]!\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8988, 2, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRH_PRE", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRH_PRE", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x8988);
  CHECK(finalState->r0 == (QBDI::rword)&buf[8]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrht") {
  const char source[] = "ldrht r1, [r0, #4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8988, 2, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRHT", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRHT", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x8988);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrhs") {
  const char source[] = "ldrh.w r1, [r0, r2]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8988, 2, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRHs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRHs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r2 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x8988);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrhs_lsl1") {
  const char source[] = "ldrh.w r1, [r0, r2, lsl #1]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x8584, 2, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRHs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRHs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 2;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x8584);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrhs_lsl2") {
  const char source[] = "ldrh.w r1, [r0, r2, lsl #2]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x8584, 2, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRHs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRHs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x8584);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrhs_lsl3") {
  const char source[] = "ldrh.w r1, [r0, r2, lsl #3]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8988, 2, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRHs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRHs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x8988);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrshi12") {
  const char source[] = "ldrsh.w r1, [r0, #4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8988, 2, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRSHi12", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRSHi12", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0xffff8988);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrshi8") {
  const char source[] = "ldrsh.w r1, [r0, #-4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x8584, 2, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRSHi8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRSHi8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[8];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0xffff8584);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrshpci") {
  const char source[] =
      "b 1f\n"
      "label:\n"
      "  .byte 0x34, 0x12\n"
      "1:\n"
      "  ldrsh.w r1, label\n";

  bool seen = false, seenPost = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRSHpci", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].value == 0x1234);
                     CHECK(accesses[0].size == 2);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("t2LDRSHpci", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].value == 0x1234);
                     CHECK(accesses[0].size == 2);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(gprState->r1 == 0x1234);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seen);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrsh_post") {
  const char source[] = "ldrsh.w r1, [r0], #4\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x8584, 2, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRSH_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRSH_POST", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0xffff8584);
  CHECK(finalState->r0 == (QBDI::rword)&buf[8]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrsh_pre") {
  const char source[] = "ldrsh.w r1, [r0, #4]!\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8988, 2, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRSH_PRE", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRSH_PRE", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0xffff8988);
  CHECK(finalState->r0 == (QBDI::rword)&buf[8]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrsht") {
  const char source[] = "ldrsht r1, [r0, #4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8988, 2, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRSHT", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRSHT", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0xffff8988);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrshs") {
  const char source[] = "ldrsh.w r1, [r0, r2]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8988, 2, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRSHs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRSHs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  state->r2 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0xffff8988);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrshs_lsl1") {
  const char source[] = "ldrsh.w r1, [r0, r2, lsl #1]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x8584, 2, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRSHs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRSHs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 2;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0xffff8584);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrshs_lsl2") {
  const char source[] = "ldrsh.w r1, [r0, r2, lsl #2]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x8584, 2, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRSHs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRSHs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0xffff8584);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrshs_lsl3") {
  const char source[] = "ldrsh.w r1, [r0, r2, lsl #3]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x8988, 2, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRSHs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRSHs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0xffff8988);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2strhi12") {
  const char source[] = "strh.w r1, [r0, #4]\n";

  uint8_t buf[16] = {0};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xabcd, 2, QBDI::MEMORY_WRITE,
                            QBDI::MEMORY_NO_FLAGS}};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRHi12", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2STRHi12", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xabcd;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == 0xcd);
  CHECK(buf[5] == 0xab);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2strhi8") {
  const char source[] = "strh.w r1, [r0, #-4]\n";

  uint8_t buf[16] = {0};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xabcd, 2, QBDI::MEMORY_WRITE,
                            QBDI::MEMORY_NO_FLAGS}};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRHi8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2STRHi8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[8];
  state->r1 = 0xabcd;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == 0xcd);
  CHECK(buf[5] == 0xab);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2strh_post") {
  const char source[] = "strh.w r1, [r0], #4\n";

  uint8_t buf[16] = {0};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[0], 0xabcd, 2, QBDI::MEMORY_WRITE,
                            QBDI::MEMORY_NO_FLAGS}};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRH_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2STRH_POST", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xabcd;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[4]);
  CHECK(buf[0] == 0xcd);
  CHECK(buf[1] == 0xab);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2strh_pre") {
  const char source[] = "strh.w r1, [r0, #4]!\n";

  uint8_t buf[16] = {0};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xabcd, 2, QBDI::MEMORY_WRITE,
                            QBDI::MEMORY_NO_FLAGS}};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRH_PRE", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2STRH_PRE", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xabcd;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[4]);
  CHECK(buf[4] == 0xcd);
  CHECK(buf[5] == 0xab);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2strht") {
  const char source[] = "strht r1, [r0, #4]\n";

  uint8_t buf[16] = {0};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xabcd, 2, QBDI::MEMORY_WRITE,
                            QBDI::MEMORY_NO_FLAGS}};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRHT", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2STRHT", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xabcd;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == 0xcd);
  CHECK(buf[5] == 0xab);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2strhs") {
  const char source[] = "strh.w r1, [r0, r2]\n";

  uint8_t buf[16] = {0};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xabcd, 2, QBDI::MEMORY_WRITE,
                            QBDI::MEMORY_NO_FLAGS}};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRHs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2STRHs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xabcd;
  state->r2 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == 0xcd);
  CHECK(buf[5] == 0xab);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2strhs_lsl1") {
  const char source[] = "strh.w r1, [r0, r2, lsl #1]\n";

  uint8_t buf[16] = {0};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xabcd, 2, QBDI::MEMORY_WRITE,
                            QBDI::MEMORY_NO_FLAGS}};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRHs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2STRHs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xabcd;
  state->r2 = 2;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == 0xcd);
  CHECK(buf[5] == 0xab);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2strhs_lsl2") {
  const char source[] = "strh.w r1, [r0, r2, lsl #2]\n";

  uint8_t buf[16] = {0};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xabcd, 2, QBDI::MEMORY_WRITE,
                            QBDI::MEMORY_NO_FLAGS}};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRHs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2STRHs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xabcd;
  state->r2 = 1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == 0xcd);
  CHECK(buf[5] == 0xab);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2strhs_lsl3") {
  const char source[] = "strh.w r1, [r0, r2, lsl #3]\n";

  uint8_t buf[16] = {0};
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[8], 0xabcd, 2, QBDI::MEMORY_WRITE,
                            QBDI::MEMORY_NO_FLAGS}};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRHs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2STRHs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xabcd;
  state->r2 = 1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[8] == 0xcd);
  CHECK(buf[9] == 0xab);
}
