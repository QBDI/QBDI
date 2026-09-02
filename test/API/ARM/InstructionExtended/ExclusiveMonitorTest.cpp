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
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-clrex") {
  const char source[] = "clrex\n";

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost{};
  vm.addMnemonicCB("CLREX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("CLREX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("CLREX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->localMonitor.enable == 0);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-clrex_clears_monitor") {
  const char source[] =
      "ldrex r1, [r0]\n"
      "clrex\n";

  uint32_t v = 0x12345678;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost{};
  vm.addMnemonicCB("CLREX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("CLREX", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->localMonitor.enable == 4);
                     CHECK(gprState->localMonitor.addr == (QBDI::rword)&v);
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("CLREX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("CLREX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->localMonitor.enable == 0);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrex") {
  const char source[] = "ldrex r1, [r0]\n";

  constexpr uint32_t expected = 0x12345678;
  uint32_t v = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{
      {{(QBDI::rword)&v, expected, 4, QBDI::MEMORY_READ,
        QBDI::MEMORY_NO_FLAGS}}};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("LDREX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDREX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDREX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrexb") {
  const char source[] = "ldrexb r1, [r0]\n";

  constexpr uint8_t expected = 0x42;
  uint8_t v = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{
      {{(QBDI::rword)&v, expected, 1, QBDI::MEMORY_READ,
        QBDI::MEMORY_NO_FLAGS}}};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("LDREXB", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDREXB", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDREXB", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrexh") {
  const char source[] = "ldrexh r1, [r0]\n";

  constexpr uint16_t expected = 0x1234;
  uint16_t v = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{
      {{(QBDI::rword)&v, expected, 2, QBDI::MEMORY_READ,
        QBDI::MEMORY_NO_FLAGS}}};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("LDREXH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDREXH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDREXH", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrexd") {
  const char source[] = "ldrexd r2, r3, [r0]\n";

  constexpr uint64_t expected = 0x1122334455667788ULL;
  uint64_t v = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{
      {{(QBDI::rword)&v, (uint32_t)expected, 4, QBDI::MEMORY_READ,
        QBDI::MEMORY_NO_FLAGS},
       {(QBDI::rword)&v + 4, (uint32_t)(expected >> 32), 4, QBDI::MEMORY_READ,
        QBDI::MEMORY_NO_FLAGS}}};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("LDREXD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDREXD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDREXD", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrexd_novalue") {
  const char source[] = "ldrexd r2, r3, [r0]\n";

  constexpr uint64_t expected = 0x1122334455667788ULL;
  uint64_t v = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.setOptions(vm.getOptions() |
                QBDI::Options::OPT_DISABLE_MEMORYACCESS_VALUE);
  ExpectedMemoryAccesses expectedPre{
      {{(QBDI::rword)&v, 0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE}}};
  ExpectedMemoryAccesses expectedPost = expectedPre;
  vm.addMnemonicCB("LDREXD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDREXD", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK((accesses[0].flags & QBDI::MEMORY_UNKNOWN_VALUE) !=
                           0);
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("LDREXD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDREXD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK((accesses[0].flags & QBDI::MEMORY_UNKNOWN_VALUE) !=
                           0);
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
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strex") {
  const char source[] =
      "ldrex r2, [r0]\n"
      "strex r3, r1, [r0]\n";

  uint32_t v = 0x10101010;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost{
      {{(QBDI::rword)&v, 0xcafebabe, 4, QBDI::MEMORY_WRITE,
        QBDI::MEMORY_NO_FLAGS}}};
  vm.addMnemonicCB("STREX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STREX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STREX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == 0xcafebabe);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strexb") {
  const char source[] =
      "ldrexb r2, [r0]\n"
      "strexb r3, r1, [r0]\n";

  uint8_t v = 0x10;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost{
      {{(QBDI::rword)&v, 0xca, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}}};
  vm.addMnemonicCB("STREXB", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STREXB", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STREXB", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == 0xca);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strexh") {
  const char source[] =
      "ldrexh r2, [r0]\n"
      "strexh r3, r1, [r0]\n";

  uint16_t v = 0x1010;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost{
      {{(QBDI::rword)&v, 0xcafe, 2, QBDI::MEMORY_WRITE,
        QBDI::MEMORY_NO_FLAGS}}};
  vm.addMnemonicCB("STREXH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STREXH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STREXH", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == 0xcafe);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strexd") {
  const char source[] =
      "ldrexd r2, r3, [r0]\n"
      "strexd r5, r6, r7, [r0]\n";

  uint64_t v = 0x1010101010101010ULL;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost{
      {{(QBDI::rword)&v, 0xcafebabe, 4, QBDI::MEMORY_WRITE,
        QBDI::MEMORY_NO_FLAGS},
       {(QBDI::rword)&v + 4, 0xdeadbeef, 4, QBDI::MEMORY_WRITE,
        QBDI::MEMORY_NO_FLAGS}}};
  vm.addMnemonicCB("STREXD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STREXD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STREXD", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == 0xdeadbeefcafebabeULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strexd_novalue") {
  const char source[] =
      "ldrexd r2, r3, [r0]\n"
      "strexd r5, r6, r7, [r0]\n";

  uint64_t v = 0x1010101010101010ULL;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.setOptions(vm.getOptions() |
                QBDI::Options::OPT_DISABLE_MEMORYACCESS_VALUE);
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost{
      {{(QBDI::rword)&v, 0, 8, QBDI::MEMORY_WRITE,
        QBDI::MEMORY_UNKNOWN_VALUE}}};
  vm.addMnemonicCB("STREXD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STREXD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STREXD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK((accesses[0].flags & QBDI::MEMORY_UNKNOWN_VALUE) !=
                           0);
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
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == 0xdeadbeefcafebabeULL);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strex_fail") {
  const char source[] = "strex r3, r1, [r0]\n";

  constexpr uint32_t expected = 0x10101010;
  uint32_t v = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost{};
  vm.addMnemonicCB("STREX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STREX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STREX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == expected);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strexb_fail") {
  const char source[] = "strexb r3, r1, [r0]\n";

  constexpr uint8_t expected = 0x10;
  uint8_t v = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost{};
  vm.addMnemonicCB("STREXB", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STREXB", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STREXB", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->r3 == 1);
                     CHECK(gprState->localMonitor.enable == 0);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r1 = 0xca;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == expected);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strexh_fail") {
  const char source[] = "strexh r3, r1, [r0]\n";

  constexpr uint16_t expected = 0x1010;
  uint16_t v = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost{};
  vm.addMnemonicCB("STREXH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STREXH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STREXH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->r3 == 1);
                     CHECK(gprState->localMonitor.enable == 0);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r1 = 0xcafe;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == expected);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strexd_fail") {
  const char source[] = "strexd r5, r6, r7, [r0]\n";

  constexpr uint32_t expectedLo = 0x10101010;
  constexpr uint32_t expectedHi = 0x10101010;
  uint64_t v = ((uint64_t)expectedHi << 32) | expectedLo;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost{};
  vm.addMnemonicCB("STREXD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STREXD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STREXD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->r5 == 1);
                     CHECK(gprState->localMonitor.enable == 0);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r6 = 0xcafebabe;
  state->r7 = 0xdeadbeef;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == 0x1010101010101010ULL);
}

static constexpr QBDI::rword CPSR_Z = (1u << 30);

static void checkLdrexCond(APITest &fixture, bool taken) {
  const char source[] = "ldrexeq r1, [r0]\n";

  constexpr uint32_t expected = 0x12345678;
  uint32_t v = expected;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost{};
  if (taken) {
    expectedPre.accesses = {{(QBDI::rword)&v, expected, 4, QBDI::MEMORY_READ,
                             QBDI::MEMORY_NO_FLAGS}};
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("LDREX", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("LDREX", QBDI::POSTINST, checkAccess, &expectedPost);
  fixture.vm.addMnemonicCB(
      "LDREX", QBDI::POSTINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        if (taken) {
          CHECK(gprState->r1 == expected);
          CHECK(gprState->localMonitor.enable == 4);
          CHECK(gprState->localMonitor.addr == (QBDI::rword)&v);
        } else {
          CHECK(gprState->r1 == 0x11111111);
          CHECK(gprState->localMonitor.enable == 0);
        }
        return QBDI::VMAction::CONTINUE;
      });

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r1 = taken ? 0 : 0x11111111;
  if (taken) {
    state->cpsr |= CPSR_Z;
  } else {
    state->cpsr &= ~CPSR_Z;
  }
  fixture.vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

static void checkStrexCond(APITest &fixture, bool taken) {
  const char source[] =
      "ldrex r2, [r0]\n"
      "strexeq r3, r1, [r0]\n";

  uint32_t v = 0x10101010;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost{};
  if (taken) {
    expectedPost.accesses = {{(QBDI::rword)&v, 0xcafebabe, 4,
                              QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS}};
  }
  fixture.vm.addMnemonicCB("STREX", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("STREX", QBDI::POSTINST, checkAccess, &expectedPost);
  fixture.vm.addMnemonicCB(
      "STREX", QBDI::POSTINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        if (taken) {
          CHECK(gprState->r3 == 0);
          CHECK(gprState->localMonitor.enable == 0);
        } else {
          CHECK(gprState->r3 == 0x99999999);
          CHECK(gprState->localMonitor.enable == 4);
          CHECK(gprState->localMonitor.addr == (QBDI::rword)&v);
        }
        return QBDI::VMAction::CONTINUE;
      });

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&v;
  state->r1 = 0xcafebabe;
  state->r3 = taken ? 0 : 0x99999999;
  if (taken) {
    state->cpsr |= CPSR_Z;
  } else {
    state->cpsr &= ~CPSR_Z;
  }
  fixture.vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == (taken ? 0xcafebabe : 0x10101010));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrex_cond_taken") {
  checkLdrexCond(*this, true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrex_cond_not_taken") {
  checkLdrexCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strex_cond_taken") {
  checkStrexCond(*this, true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strex_cond_not_taken") {
  checkStrexCond(*this, false);
}
