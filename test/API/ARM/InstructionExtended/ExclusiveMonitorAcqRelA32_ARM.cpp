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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldaex") {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const char source[] = "ldaex r1, [r0]\n";

  constexpr uint32_t expected = 0x12345678;
  uint32_t v = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{
      {{(QBDI::rword)&v, expected, 4, QBDI::MEMORY_READ,
        QBDI::MEMORY_NO_FLAGS}}};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("LDAEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDAEX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDAEX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->r1 == expected);
                     CHECK(gprState->localMonitor.enable == 4);
                     CHECK(gprState->localMonitor.addr == (QBDI::rword)&v);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldaexb") {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const char source[] = "ldaexb r1, [r0]\n";

  constexpr uint8_t expected = 0x42;
  uint8_t v = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{
      {{(QBDI::rword)&v, expected, 1, QBDI::MEMORY_READ,
        QBDI::MEMORY_NO_FLAGS}}};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("LDAEXB", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDAEXB", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDAEXB", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->r1 & 0xff) == expected);
                     CHECK(gprState->localMonitor.enable == 1);
                     CHECK(gprState->localMonitor.addr == (QBDI::rword)&v);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldaexh") {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const char source[] = "ldaexh r1, [r0]\n";

  constexpr uint16_t expected = 0x1234;
  uint16_t v = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{
      {{(QBDI::rword)&v, expected, 2, QBDI::MEMORY_READ,
        QBDI::MEMORY_NO_FLAGS}}};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("LDAEXH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDAEXH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDAEXH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->r1 & 0xffff) == expected);
                     CHECK(gprState->localMonitor.enable == 2);
                     CHECK(gprState->localMonitor.addr == (QBDI::rword)&v);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldaexd") {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const char source[] = "ldaexd r2, r3, [r0]\n";

  constexpr uint64_t expected = 0x1122334455667788ULL;
  uint64_t v = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{
      {{(QBDI::rword)&v, (uint32_t)expected, 4, QBDI::MEMORY_READ,
        QBDI::MEMORY_NO_FLAGS},
       {(QBDI::rword)&v + 4, (uint32_t)(expected >> 32), 4, QBDI::MEMORY_READ,
        QBDI::MEMORY_NO_FLAGS}}};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("LDAEXD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDAEXD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDAEXD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->r2 == (uint32_t)expected);
                     CHECK(gprState->r3 == (uint32_t)(expected >> 32));
                     CHECK(gprState->localMonitor.enable == 8);
                     CHECK(gprState->localMonitor.addr == (QBDI::rword)&v);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-stlex") {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const char source[] =
      "ldaex r2, [r0]\n"
      "stlex r3, r1, [r0]\n";

  uint32_t v = 0x10101010;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost{
      {{(QBDI::rword)&v, 0xcafebabe, 4, QBDI::MEMORY_WRITE,
        QBDI::MEMORY_NO_FLAGS}}};
  vm.addMnemonicCB("STLEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STLEX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STLEX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->r3 == 0);
                     CHECK(gprState->localMonitor.enable == 0);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r1 = 0xcafebabe;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == 0xcafebabe);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-stlexb") {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const char source[] =
      "ldaexb r2, [r0]\n"
      "stlexb r3, r1, [r0]\n";

  uint8_t v = 0x10;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost{
      {{(QBDI::rword)&v, 0xca, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}}};
  vm.addMnemonicCB("STLEXB", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STLEXB", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STLEXB", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->r3 == 0);
                     CHECK(gprState->localMonitor.enable == 0);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r1 = 0xca;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == 0xca);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-stlexh") {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const char source[] =
      "ldaexh r2, [r0]\n"
      "stlexh r3, r1, [r0]\n";

  uint16_t v = 0x1010;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost{
      {{(QBDI::rword)&v, 0xcafe, 2, QBDI::MEMORY_WRITE,
        QBDI::MEMORY_NO_FLAGS}}};
  vm.addMnemonicCB("STLEXH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STLEXH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STLEXH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->r3 == 0);
                     CHECK(gprState->localMonitor.enable == 0);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r1 = 0xcafe;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == 0xcafe);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-stlexd") {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const char source[] =
      "ldaexd r2, r3, [r0]\n"
      "stlexd r5, r6, r7, [r0]\n";

  uint64_t v = 0x1010101010101010ULL;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost{
      {{(QBDI::rword)&v, 0xcafebabe, 4, QBDI::MEMORY_WRITE,
        QBDI::MEMORY_NO_FLAGS},
       {(QBDI::rword)&v + 4, 0xdeadbeef, 4, QBDI::MEMORY_WRITE,
        QBDI::MEMORY_NO_FLAGS}}};
  vm.addMnemonicCB("STLEXD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STLEXD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STLEXD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->r5 == 0);
                     CHECK(gprState->localMonitor.enable == 0);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r6 = 0xcafebabe;
  state->r7 = 0xdeadbeef;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == 0xdeadbeefcafebabeULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-stlex_fail") {
  if (!checkFeature("acquire-release")) {
    return;
  }
  const char source[] = "stlex r3, r1, [r0]\n";

  constexpr uint32_t expected = 0x10101010;
  uint32_t v = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost{};
  vm.addMnemonicCB("STLEX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STLEX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STLEX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->r3 == 1);
                     CHECK(gprState->localMonitor.enable == 0);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r1 = 0xcafebabe;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"acquire-release"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == expected);
}
