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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldaprh") {
  if (!checkFeature("rcpc")) {
    return;
  }

  const char source[] = "ldaprh w1, [x0]\n";

  constexpr uint16_t expected = 0x6d3c;
  uint16_t v = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, expected, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, expected, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDAPRH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDAPRH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDAPRH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffff) == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"rcpc"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldapurh") {
  if (!checkFeature("rcpc-immo")) {
    return;
  }

  const char source[] = "ldapurh w1, [x0, #10]\n";

  constexpr uint16_t expected = 0x7192;
  uint8_t buf[16] = {0};
  *(uint16_t *)&buf[10] = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[10], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[10], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDAPURHi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDAPURHi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDAPURHi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffff) == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"rcpc-immo"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldapursh_w") {
  if (!checkFeature("rcpc-immo")) {
    return;
  }

  const char source[] = "ldapursh w1, [x0, #10]\n";

  constexpr uint16_t expected = 0x8090;
  uint8_t buf[16] = {0};
  *(uint16_t *)&buf[10] = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[10], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[10], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDAPURSHWi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDAPURSHWi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDAPURSHWi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffffffff) == 0xffff8090);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"rcpc-immo"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldapursh_x") {
  if (!checkFeature("rcpc-immo")) {
    return;
  }

  const char source[] = "ldapursh x1, [x0, #10]\n";

  constexpr uint16_t expected = 0x8091;
  uint8_t buf[16] = {0};
  *(uint16_t *)&buf[10] = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[10], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[10], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDAPURSHXi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDAPURSHXi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDAPURSHXi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == 0xffffffffffff8091ULL);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"rcpc-immo"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldapur_h") {
  if (!checkFeature("rcpc3")) {
    return;
  }

  const char source[] = "ldapur h0, [x0, #10]\n";

  constexpr uint16_t expected = 0x2f4a;
  uint8_t buf[16] = {0};
  *(uint16_t *)&buf[10] = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[10], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[10], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDAPURhi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDAPURhi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDAPURhi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint16_t)fprState->v0 == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"rcpc3"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stlurh") {
  if (!checkFeature("rcpc-immo")) {
    return;
  }

  const char source[] = "stlurh w1, [x0, #10]\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[10], 0x9345, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STLURHi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STLURHi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x1 = 0x9345;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"rcpc-immo"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint16_t *)&buf[10] == 0x9345);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stlur_h") {
  if (!checkFeature("rcpc3")) {
    return;
  }

  const char source[] = "stlur h0, [x0, #10]\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[10], 0x6421, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STLURhi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STLURhi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = 0x6421;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"rcpc3"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint16_t *)&buf[10] == 0x6421);
}
