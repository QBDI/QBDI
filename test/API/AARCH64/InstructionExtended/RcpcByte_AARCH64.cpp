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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldaprb") {
  if (!checkFeature("rcpc")) {
    return;
  }

  const char source[] = "ldaprb w1, [x0]\n";

  constexpr uint8_t expected = 0x6d;
  uint8_t v = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, expected, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, expected, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDAPRB", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDAPRB", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDAPRB", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xff) == expected);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldapurb") {
  if (!checkFeature("rcpc-immo")) {
    return;
  }

  const char source[] = "ldapurb w1, [x0, #5]\n";

  constexpr uint8_t expected = 0x71;
  uint8_t v[8] = {0, 0, 0, 0, 0, expected, 0, 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDAPURBi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDAPURBi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDAPURBi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xff) == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"rcpc-immo"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldapursb_w") {
  if (!checkFeature("rcpc-immo")) {
    return;
  }

  const char source[] = "ldapursb w1, [x0, #5]\n";

  constexpr uint8_t expected = 0x80;
  uint8_t v[8] = {0, 0, 0, 0, 0, expected, 0, 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDAPURSBWi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDAPURSBWi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDAPURSBWi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffffffff) == 0xffffff80);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"rcpc-immo"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldapursb_x") {
  if (!checkFeature("rcpc-immo")) {
    return;
  }

  const char source[] = "ldapursb x1, [x0, #5]\n";

  constexpr uint8_t expected = 0x80;
  uint8_t v[8] = {0, 0, 0, 0, 0, expected, 0, 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDAPURSBXi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDAPURSBXi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDAPURSBXi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == 0xffffffffffffff80ULL);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"rcpc-immo"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldapur_b") {
  if (!checkFeature("rcpc3")) {
    return;
  }

  const char source[] = "ldapur b0, [x0, #5]\n";

  constexpr uint8_t expected = 0x2f;
  uint8_t v[8] = {0, 0, 0, 0, 0, expected, 0, 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], expected, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDAPURbi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDAPURbi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDAPURbi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint8_t)fprState->v0 == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"rcpc3"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stlurb") {
  if (!checkFeature("rcpc-immo")) {
    return;
  }

  const char source[] = "stlurb w1, [x0, #5]\n";

  uint8_t v[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], 0x93, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STLURBi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STLURBi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = 0x93;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"rcpc-immo"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v[5] == 0x93);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stlur_b") {
  if (!checkFeature("rcpc3")) {
    return;
  }

  const char source[] = "stlur b0, [x0, #5]\n";

  uint8_t v[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[5], 0x64, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STLURbi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STLURbi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = 0x64;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"rcpc3"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v[5] == 0x64);
}
