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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tldrbi") {
  const char source[] = "ldrb r1, [r0, #4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("tLDRBi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("tLDRBi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x88);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tldrbr") {
  const char source[] = "ldrb r1, [r0, r2]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("tLDRBr", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("tLDRBr", QBDI::POSTINST, checkAccess, &expectedPost);

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
  CHECK(finalState->r1 == 0x88);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tldrsb") {
  const char source[] = "ldrsb r1, [r0, r2]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("tLDRSB", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("tLDRSB", QBDI::POSTINST, checkAccess, &expectedPost);

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
  CHECK(finalState->r1 == 0xffffff88);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tstrbi") {
  const char source[] = "strb r1, [r0, #4]\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xab, 1, QBDI::MEMORY_WRITE,
                            QBDI::MEMORY_NO_FLAGS}};
  vm.addMnemonicCB("tSTRBi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("tSTRBi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xab;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == 0xab);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tstrbr") {
  const char source[] = "strb r1, [r0, r2]\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xab, 1, QBDI::MEMORY_WRITE,
                            QBDI::MEMORY_NO_FLAGS}};
  vm.addMnemonicCB("tSTRBr", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("tSTRBr", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xab;
  state->r2 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == 0xab);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrbt") {
  const char source[] = "ldrbt r1, [r0, #4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("t2LDRBT", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRBT", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x88);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrb_post") {
  const char source[] = "ldrb.w r1, [r0], #4\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x84, 1, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("t2LDRB_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRB_POST", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x84);
  CHECK(finalState->r0 == (QBDI::rword)&buf[8]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrb_pre") {
  const char source[] = "ldrb.w r1, [r0, #4]!\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("t2LDRB_PRE", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRB_PRE", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x88);
  CHECK(finalState->r0 == (QBDI::rword)&buf[8]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrbi12") {
  const char source[] = "ldrb.w r1, [r0, #4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("t2LDRBi12", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRBi12", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x88);
  CHECK(finalState->r0 == (QBDI::rword)&buf[4]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrbi8") {
  const char source[] = "ldrb.w r1, [r0, #-4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x84, 1, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("t2LDRBi8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRBi8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[8];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x84);
  CHECK(finalState->r0 == (QBDI::rword)&buf[8]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrbpci") {
  const char source[] =
      "b 1f\n"
      "label:\n"
      "  .byte 0x9b, 0x00\n"
      "1:\n"
      "  ldrb.w r1, label\n";

  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb);
  QBDI::rword labelAddr = (codeAddr & ~(QBDI::rword)1) + 2;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {
      {labelAddr, 0x9b, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("t2LDRBpci", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRBpci", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x9b);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrbs") {
  const char source[] = "ldrb.w r1, [r0, r2]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("t2LDRBs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRBs", QBDI::POSTINST, checkAccess, &expectedPost);

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
  CHECK(finalState->r1 == 0x88);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrbs_lsl1") {
  const char source[] = "ldrb.w r1, [r0, r2, lsl #1]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("t2LDRBs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRBs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0x88);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrbs_lsl2") {
  const char source[] = "ldrb.w r1, [r0, r2, lsl #2]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("t2LDRBs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRBs", QBDI::POSTINST, checkAccess, &expectedPost);

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
  CHECK(finalState->r1 == 0x88);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrbs_lsl3") {
  const char source[] = "ldrb.w r1, [r0, r2, lsl #3]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("t2LDRBs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRBs", QBDI::POSTINST, checkAccess, &expectedPost);

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
  CHECK(finalState->r1 == 0x88);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrsbt") {
  const char source[] = "ldrsbt r1, [r0, #4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("t2LDRSBT", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRSBT", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0xffffff88);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrsb_post") {
  const char source[] = "ldrsb.w r1, [r0], #4\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x84, 1, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("t2LDRSB_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRSB_POST", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0xffffff84);
  CHECK(finalState->r0 == (QBDI::rword)&buf[8]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrsb_pre") {
  const char source[] = "ldrsb.w r1, [r0, #4]!\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("t2LDRSB_PRE", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRSB_PRE", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0xffffff88);
  CHECK(finalState->r0 == (QBDI::rword)&buf[8]);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrsbi12") {
  const char source[] = "ldrsb.w r1, [r0, #4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("t2LDRSBi12", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRSBi12", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0xffffff88);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrsbi8") {
  const char source[] = "ldrsb.w r1, [r0, #-4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[4], 0x84, 1, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("t2LDRSBi8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRSBi8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[8];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0xffffff84);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrsbpci") {
  const char source[] =
      "b 1f\n"
      "label:\n"
      "  .byte 0x9b, 0x00\n"
      "1:\n"
      "  ldrsb.w r1, label\n";

  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb);
  QBDI::rword labelAddr = (codeAddr & ~(QBDI::rword)1) + 2;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {
      {labelAddr, 0x9b, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("t2LDRSBpci", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRSBpci", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0xffffff9b);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrsbs") {
  const char source[] = "ldrsb.w r1, [r0, r2]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("t2LDRSBs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRSBs", QBDI::POSTINST, checkAccess, &expectedPost);

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
  CHECK(finalState->r1 == 0xffffff88);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrsbs_lsl1") {
  const char source[] = "ldrsb.w r1, [r0, r2, lsl #1]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("t2LDRSBs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRSBs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r1 == 0xffffff88);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrsbs_lsl2") {
  const char source[] = "ldrsb.w r1, [r0, r2, lsl #2]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("t2LDRSBs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRSBs", QBDI::POSTINST, checkAccess, &expectedPost);

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
  CHECK(finalState->r1 == 0xffffff88);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrsbs_lsl3") {
  const char source[] = "ldrsb.w r1, [r0, r2, lsl #3]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {{(QBDI::rword)&buf[8], 0x88, 1, QBDI::MEMORY_READ,
                           QBDI::MEMORY_NO_FLAGS}};
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("t2LDRSBs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRSBs", QBDI::POSTINST, checkAccess, &expectedPost);

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
  CHECK(finalState->r1 == 0xffffff88);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2strbt") {
  const char source[] = "strbt r1, [r0, #4]\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xab, 1, QBDI::MEMORY_WRITE,
                            QBDI::MEMORY_NO_FLAGS}};
  vm.addMnemonicCB("t2STRBT", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2STRBT", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xab;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == 0xab);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2strb_post") {
  const char source[] = "strb.w r1, [r0], #4\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[0], 0xab, 1, QBDI::MEMORY_WRITE,
                            QBDI::MEMORY_NO_FLAGS}};
  vm.addMnemonicCB("t2STRB_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2STRB_POST", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xab;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[4]);
  CHECK(buf[0] == 0xab);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2strb_pre") {
  const char source[] = "strb.w r1, [r0, #4]!\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xab, 1, QBDI::MEMORY_WRITE,
                            QBDI::MEMORY_NO_FLAGS}};
  vm.addMnemonicCB("t2STRB_PRE", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2STRB_PRE", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xab;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)&buf[4]);
  CHECK(buf[4] == 0xab);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2strbi12") {
  const char source[] = "strb.w r1, [r0, #4]\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xab, 1, QBDI::MEMORY_WRITE,
                            QBDI::MEMORY_NO_FLAGS}};
  vm.addMnemonicCB("t2STRBi12", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2STRBi12", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xab;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == 0xab);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2strbi8") {
  const char source[] = "strb.w r1, [r0, #-4]\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xab, 1, QBDI::MEMORY_WRITE,
                            QBDI::MEMORY_NO_FLAGS}};
  vm.addMnemonicCB("t2STRBi8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2STRBi8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[8];
  state->r1 = 0xab;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == 0xab);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2strbs") {
  const char source[] = "strb.w r1, [r0, r2]\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[4], 0xab, 1, QBDI::MEMORY_WRITE,
                            QBDI::MEMORY_NO_FLAGS}};
  vm.addMnemonicCB("t2STRBs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2STRBs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xab;
  state->r2 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == 0xab);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2strbs_lsl1") {
  const char source[] = "strb.w r1, [r0, r2, lsl #1]\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[8], 0xab, 1, QBDI::MEMORY_WRITE,
                            QBDI::MEMORY_NO_FLAGS}};
  vm.addMnemonicCB("t2STRBs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2STRBs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xab;
  state->r2 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[8] == 0xab);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2strbs_lsl2") {
  const char source[] = "strb.w r1, [r0, r2, lsl #2]\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[8], 0xab, 1, QBDI::MEMORY_WRITE,
                            QBDI::MEMORY_NO_FLAGS}};
  vm.addMnemonicCB("t2STRBs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2STRBs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xab;
  state->r2 = 2;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[8] == 0xab);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2strbs_lsl3") {
  const char source[] = "strb.w r1, [r0, r2, lsl #3]\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPost.accesses = {{(QBDI::rword)&buf[8], 0xab, 1, QBDI::MEMORY_WRITE,
                            QBDI::MEMORY_NO_FLAGS}};
  vm.addMnemonicCB("t2STRBs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2STRBs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r1 = 0xab;
  state->r2 = 1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[8] == 0xab);
}
