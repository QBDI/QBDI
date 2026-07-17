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
#include "MemAccessTestUtils_AARCH64.h"

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::checkFeature;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldxrw") {
  const char source[] = "ldxr w1, [x0]\n";

  constexpr uint32_t expected = 0xa5b3c1d2;
  uint32_t v = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, expected, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, expected, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDXRW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDXRW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDXRW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)gprState->x1 == expected);
                     CHECK(gprState->localMonitor.enable == 4);
                     CHECK(gprState->localMonitor.addr == (QBDI::rword)&v);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldaxrw") {
  const char source[] = "ldaxr w1, [x0]\n";

  constexpr uint32_t expected = 0x5a3d8e91;
  uint32_t v = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, expected, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, expected, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDAXRW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDAXRW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDAXRW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)gprState->x1 == expected);
                     CHECK(gprState->localMonitor.enable == 4);
                     CHECK(gprState->localMonitor.addr == (QBDI::rword)&v);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldarw") {
  const char source[] = "ldar w1, [x0]\n";

  constexpr uint32_t expected = 0x3c4d9f10;
  uint32_t v = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, expected, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, expected, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDARW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDARW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDARW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)gprState->x1 == expected);
                     CHECK(gprState->localMonitor.enable == 0);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldlarw") {
  if (!checkFeature("lor")) {
    return;
  }

  const char source[] = "ldlar w1, [x0]\n";

  constexpr uint32_t expected = 0xc3d47a2b;
  uint32_t v = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, expected, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, expected, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDLARW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDLARW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDLARW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)gprState->x1 == expected);
                     CHECK(gprState->localMonitor.enable == 0);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lor"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stxrw") {
  const char source[] =
      "ldaxr w2, [x0]\n"
      "stxr w3, w1, [x0]\n";

  uint32_t v = 0x10203040;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, 0xcd41ef53, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STXRW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STXRW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STXRW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)gprState->x3 == 0);
                     CHECK(gprState->localMonitor.enable == 0);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = 0xcd41ef53;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == 0xcd41ef53);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stlxrw") {
  const char source[] =
      "ldaxr w2, [x0]\n"
      "stlxr w3, w1, [x0]\n";

  uint32_t v = 0x10203040;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, 0xef51ab34, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STLXRW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STLXRW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STLXRW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)gprState->x3 == 0);
                     CHECK(gprState->localMonitor.enable == 0);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = 0xef51ab34;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == 0xef51ab34);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stlrw") {
  const char source[] = "stlr w1, [x0]\n";

  uint32_t v = 0x50607080;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, 0x99112233, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STLRW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STLRW", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = 0x99112233;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == 0x99112233);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stllrw") {
  if (!checkFeature("lor")) {
    return;
  }

  const char source[] = "stllr w1, [x0]\n";

  uint32_t v = 0x50607080;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, 0x77129abc, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STLLRW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STLLRW", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = 0x77129abc;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lor"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == 0x77129abc);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldxpw") {
  const char source[] = "ldxp w1, w2, [x0]\n";

  constexpr uint32_t old0 = 0x11111111;
  constexpr uint32_t old1 = 0x22222222;
  constexpr uint64_t expected = ((uint64_t)old1 << 32) | old0;
  uint32_t v[2] = {old0, old1};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], expected, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], expected, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDXPW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDXPW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDXPW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)gprState->x1 == old0);
                     CHECK((uint32_t)gprState->x2 == old1);
                     CHECK(gprState->localMonitor.enable == 0x800);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldaxpw") {
  const char source[] = "ldaxp w1, w2, [x0]\n";

  constexpr uint32_t old0 = 0x11111111;
  constexpr uint32_t old1 = 0x22222222;
  constexpr uint64_t expected = ((uint64_t)old1 << 32) | old0;
  uint32_t v[2] = {old0, old1};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], expected, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], expected, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDAXPW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDAXPW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDAXPW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)gprState->x1 == old0);
                     CHECK((uint32_t)gprState->x2 == old1);
                     CHECK(gprState->localMonitor.enable == 0x800);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stxpw") {
  const char source[] =
      "ldxp w4, w5, [x0]\n"
      "stxp w6, w1, w2, [x0]\n";

  constexpr uint32_t new0 = 0x33333333;
  constexpr uint32_t new1 = 0x44444444;
  constexpr uint64_t newval = ((uint64_t)new1 << 32) | new0;
  uint32_t v[2] = {0x11111111, 0x22222222};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], newval, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STXPW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STXPW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STXPW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)gprState->x6 == 0);
                     CHECK(gprState->localMonitor.enable == 0);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = new0;
  state->x2 = new1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stlxpw") {
  const char source[] =
      "ldaxp w4, w5, [x0]\n"
      "stlxp w6, w1, w2, [x0]\n";

  constexpr uint32_t new0 = 0x33333333;
  constexpr uint32_t new1 = 0x44444444;
  constexpr uint64_t newval = ((uint64_t)new1 << 32) | new0;
  uint32_t v[2] = {0x11111111, 0x22222222};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], newval, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STLXPW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STLXPW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STLXPW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)gprState->x6 == 0);
                     CHECK(gprState->localMonitor.enable == 0);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = new0;
  state->x2 = new1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}
