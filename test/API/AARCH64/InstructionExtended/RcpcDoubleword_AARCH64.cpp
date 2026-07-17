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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldapr_x") {
  if (!checkFeature("rcpc")) {
    return;
  }

  const char source[] = "ldapr x1, [x0]\n";

  constexpr uint64_t expected = 0x6d3c8f21789abcdeULL;
  uint64_t v = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, expected, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, expected, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDAPRX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDAPRX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDAPRX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == expected);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldapr_x_post") {
  if (!checkFeature("rcpc3")) {
    return;
  }

  const char source[] = "ldapr x1, [x0], #8\n";

  constexpr uint64_t expected = 0x71928f3a90a0b0c0ULL;
  uint64_t v = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, expected, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, expected, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDAPRXpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDAPRXpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDAPRXpost", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == expected);
                     CHECK(gprState->x0 == (QBDI::rword)&v + 8);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"rcpc3"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldapur_x") {
  if (!checkFeature("rcpc-immo")) {
    return;
  }

  const char source[] = "ldapur x1, [x0, #10]\n";

  constexpr uint64_t expected = 0x71928a3d80901234ULL;
  uint8_t buf[24] = {0};
  *(uint64_t *)&buf[10] = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[10], expected, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[10], expected, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDAPURXi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDAPURXi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDAPURXi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == expected);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldapur_d") {
  if (!checkFeature("rcpc3")) {
    return;
  }

  const char source[] = "ldapur d0, [x0, #10]\n";

  constexpr uint64_t expected = 0x2f4a91bc12345678ULL;
  uint8_t buf[24] = {0};
  *(uint64_t *)&buf[10] = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[10], expected, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[10], expected, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDAPURdi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDAPURdi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDAPURdi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v0 == expected);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stlur_d") {
  if (!checkFeature("rcpc3")) {
    return;
  }

  const char source[] = "stlur d0, [x0, #10]\n";

  constexpr uint64_t newval = 0x64218acd13572468ULL;
  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[10], newval, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STLURdi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STLURdi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = newval;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"rcpc3"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[10] == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stlur_x") {
  if (!checkFeature("rcpc-immo")) {
    return;
  }

  const char source[] = "stlur x1, [x0, #10]\n";

  constexpr uint64_t newval = 0x93458bcd11223344ULL;
  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[10], newval, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STLURXi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STLURXi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x1 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"rcpc-immo"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[10] == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldiapp_x") {
  if (!checkFeature("rcpc3")) {
    return;
  }

  const char source[] = "ldiapp x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1122334455667788ULL;
  constexpr uint64_t old1 = 0x99aabbccddeeff00ULL;
  uint64_t v[2] = {old0, old1};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], old0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], old1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], old0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], old1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDIAPPX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDIAPPX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDIAPPX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldiapp_x_post") {
  if (!checkFeature("rcpc3")) {
    return;
  }

  const char source[] = "ldiapp x1, x2, [x0], #16\n";

  constexpr uint64_t old0 = 0x2233445566778899ULL;
  constexpr uint64_t old1 = 0xaabbccddeeff0011ULL;
  uint64_t v[2] = {old0, old1};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], old0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], old1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], old0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], old1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDIAPPXpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDIAPPXpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDIAPPXpost", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     CHECK(gprState->x0 == (QBDI::rword)&v[0] + 16);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stilp_x") {
  if (!checkFeature("rcpc3")) {
    return;
  }

  const char source[] = "stilp x1, x2, [x0]\n";

  constexpr uint64_t new0 = 0x99aabbccddeeff00ULL;
  constexpr uint64_t new1 = 0x1122334455667788ULL;
  uint64_t v[2] = {0, 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], new0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], new1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STILPX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STILPX", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = new0;
  state->x2 = new1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"rcpc3"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldap") {
  if (!checkFeature("lscp")) {
    return;
  }

  const char source[] = "ldap x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  uint64_t v[2] = {old0, old1};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], old0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], old1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], old0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], old1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDAPi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDAPi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDAPi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lscp"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldapp") {
  if (!checkFeature("lscp")) {
    return;
  }

  const char source[] = "ldapp x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x3333333333333333ULL;
  constexpr uint64_t old1 = 0x4444444444444444ULL;
  uint64_t v[2] = {old0, old1};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], old0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], old1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], old0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], old1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDAPPi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDAPPi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDAPPi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lscp"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stlp") {
  if (!checkFeature("lscp")) {
    return;
  }

  const char source[] = "stlp x1, x2, [x0]\n";

  constexpr uint64_t new0 = 0x5555555555555555ULL;
  constexpr uint64_t new1 = 0x6666666666666666ULL;
  uint64_t v[2] = {0, 0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], new0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], new1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STLPi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STLPi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = new0;
  state->x2 = new1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lscp"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldap1") {
  if (!checkFeature("rcpc3")) {
    return;
  }

  const char source[] = "ldap1 {v0.d}[0], [x0]\n";

  constexpr uint64_t expected = 0x123456789abcdef0ULL;
  uint64_t v = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, expected, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, expected, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDAP1", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDAP1", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDAP1", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v0 == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"rcpc3"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stl1") {
  if (!checkFeature("rcpc3")) {
    return;
  }

  const char source[] = "stl1 {v0.d}[0], [x0]\n";

  constexpr uint64_t newval = 0xfedcba9876543210ULL;
  uint64_t v = 0;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STL1", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STL1", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = newval;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"rcpc3"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == newval);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stlr_x_pre") {
  if (!checkFeature("rcpc3")) {
    return;
  }

  const char source[] = "stlr x1, [x0, #-8]!\n";

  constexpr uint64_t newval = 0x1928374655463728ULL;
  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[8], newval, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STLRXpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STLRXpre", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STLRXpre", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[8]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[16];
  state->x1 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"rcpc3"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[8] == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stilp_x_pre") {
  if (!checkFeature("rcpc3")) {
    return;
  }

  const char source[] = "stilp x1, x2, [x0, #-16]!\n";

  constexpr uint64_t new0 = 0x99aabbccddeeff00ULL;
  constexpr uint64_t new1 = 0x1122334455667788ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], new0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], new1, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STILPXpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STILPXpre", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STILPXpre", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[16]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[32];
  state->x1 = new0;
  state->x2 = new1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"rcpc3"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[16] == new0);
  CHECK(*(uint64_t *)&buf[24] == new1);
}
