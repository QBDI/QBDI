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
#include <cstddef>
#include <cstdio>

#include "MemAccessTestUtils_AARCH64.h"

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::checkedSnprintf;
using QBDITestBatch2::checkFeature;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

namespace {

struct ExpectedRegValue {
  size_t offset;
  QBDI::rword value;
  bool see = false;
};

QBDI::VMAction checkRegValue(QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                             QBDI::FPRState *fprState, void *data) {
  ExpectedRegValue *info = static_cast<ExpectedRegValue *>(data);
  REQUIRE_FALSE(info->see);
  info->see = true;
  QBDI::rword actual = *reinterpret_cast<QBDI::rword *>(
      reinterpret_cast<char *>(gprState) + info->offset);
  CHECK(actual == info->value);
  return QBDI::VMAction::CONTINUE;
}

struct ExpectedReturnAddr {
  QBDI::rword expected = 0;
  bool see = false;
};

QBDI::VMAction checkLinkReg(QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                            QBDI::FPRState *fprState, void *data) {
  ExpectedReturnAddr *info = static_cast<ExpectedReturnAddr *>(data);
  REQUIRE_FALSE(info->see);
  info->see = true;
  CHECK(vmi->getInstMemoryAccess().empty());
  const QBDI::InstAnalysis *ia =
      vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
  info->expected = ia->address + ia->instSize;
  CHECK(gprState->lr == info->expected);
  return QBDI::VMAction::CONTINUE;
}

void testAuthTwoReg(APITest &t, const char *signMnemonic,
                    const char *authMnemonic, const char *authUpper) {
  if (!checkFeature("pauth")) {
    return;
  }

  char source[128];
  checkedSnprintf(source,
                  "movz x0, #0x1234\n"
                  "movz x1, #0x5678\n"
                  "%s x0, x1\n"
                  "%s x0, x1\n",
                  signMnemonic, authMnemonic);

  t.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{}};
  t.vm.addMnemonicCB(authUpper, QBDI::PREINST, checkAccess, &expectedPre);
  t.vm.addMnemonicCB(authUpper, QBDI::POSTINST, checkAccess, &expectedPost);
  ExpectedRegValue expectedReg = {offsetof(QBDI::GPRState, x0), 0x1234};
  t.vm.addMnemonicCB(authUpper, QBDI::POSTINST, checkRegValue, &expectedReg);

  QBDI::rword retval;
  bool ran = t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"pauth"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(expectedReg.see);
}

void testAuthZero(APITest &t, const char *signMnemonic,
                  const char *authMnemonic, const char *authUpper) {
  if (!checkFeature("pauth")) {
    return;
  }

  char source[128];
  checkedSnprintf(source,
                  "movz x0, #0x1234\n"
                  "%s x0\n"
                  "%s x0\n",
                  signMnemonic, authMnemonic);

  t.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{}};
  t.vm.addMnemonicCB(authUpper, QBDI::PREINST, checkAccess, &expectedPre);
  t.vm.addMnemonicCB(authUpper, QBDI::POSTINST, checkAccess, &expectedPost);
  ExpectedRegValue expectedReg = {offsetof(QBDI::GPRState, x0), 0x1234};
  t.vm.addMnemonicCB(authUpper, QBDI::POSTINST, checkRegValue, &expectedReg);

  QBDI::rword retval;
  bool ran = t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"pauth"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(expectedReg.see);
}

void testAuth1716(APITest &t, const char *signMnemonic,
                  const char *authMnemonic, const char *authUpper) {
  if (!checkFeature("pauth")) {
    return;
  }

  char source[128];
  checkedSnprintf(source,
                  "movz x17, #0x1234\n"
                  "movz x16, #0x5678\n"
                  "%s\n"
                  "%s\n",
                  signMnemonic, authMnemonic);

  t.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{}};
  t.vm.addMnemonicCB(authUpper, QBDI::PREINST, checkAccess, &expectedPre);
  t.vm.addMnemonicCB(authUpper, QBDI::POSTINST, checkAccess, &expectedPost);
  ExpectedRegValue expectedReg = {offsetof(QBDI::GPRState, x17), 0x1234};
  t.vm.addMnemonicCB(authUpper, QBDI::POSTINST, checkRegValue, &expectedReg);

  QBDI::rword retval;
  bool ran = t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"pauth"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(expectedReg.see);
}

void testAuthImplicitLR(APITest &t, const char *signMnemonic,
                        const char *authMnemonic, const char *authUpper) {
  if (!checkFeature("pauth")) {
    return;
  }

  char source[128];
  checkedSnprintf(source,
                  "mov x2, lr\n"
                  "movz x30, #0x1234\n"
                  "%s\n"
                  "%s\n"
                  "mov lr, x2\n",
                  signMnemonic, authMnemonic);

  t.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{}};
  t.vm.addMnemonicCB(authUpper, QBDI::PREINST, checkAccess, &expectedPre);
  t.vm.addMnemonicCB(authUpper, QBDI::POSTINST, checkAccess, &expectedPost);
  ExpectedRegValue expectedReg = {offsetof(QBDI::GPRState, lr), 0x1234};
  t.vm.addMnemonicCB(authUpper, QBDI::POSTINST, checkRegValue, &expectedReg);

  QBDI::rword retval;
  bool ran = t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"pauth"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(expectedReg.see);
}

void testBraaLike(APITest &t, const char *signMnemonic,
                  const char *branchMnemonic, const char *branchUpper) {
  if (!checkFeature("pauth")) {
    return;
  }

  char source[256];
  checkedSnprintf(source,
                  "adr x0, pauth_braa_landed\n"
                  "movz x1, #0x5678\n"
                  "%s x0, x1\n"
                  "%s x0, x1\n"
                  "movz x0, #0x1111\n"
                  "b pauth_braa_end\n"
                  "pauth_braa_landed:\n"
                  "movz x0, #0x2222\n"
                  "pauth_braa_end:\n",
                  signMnemonic, branchMnemonic);

  t.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{}};
  t.vm.addMnemonicCB(branchUpper, QBDI::PREINST, checkAccess, &expectedPre);
  t.vm.addMnemonicCB(branchUpper, QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::rword retval;
  bool ran = t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"pauth"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(retval == 0x2222);
}

void testBraazLike(APITest &t, const char *signMnemonic,
                   const char *branchMnemonic, const char *branchUpper) {
  if (!checkFeature("pauth")) {
    return;
  }

  char source[256];
  checkedSnprintf(source,
                  "adr x0, pauth_braaz_landed\n"
                  "%s x0\n"
                  "%s x0\n"
                  "movz x0, #0x1111\n"
                  "b pauth_braaz_end\n"
                  "pauth_braaz_landed:\n"
                  "movz x0, #0x2222\n"
                  "pauth_braaz_end:\n",
                  signMnemonic, branchMnemonic);

  t.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{}};
  t.vm.addMnemonicCB(branchUpper, QBDI::PREINST, checkAccess, &expectedPre);
  t.vm.addMnemonicCB(branchUpper, QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::rword retval;
  bool ran = t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"pauth"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(retval == 0x2222);
}

void testRetaaLike(APITest &t, const char *signMnemonic,
                   const char *retMnemonic, const char *retUpper) {
  if (!checkFeature("pauth")) {
    return;
  }

  char source[256];
  checkedSnprintf(source,
                  "mov x1, lr\n"
                  "adr x0, pauth_retaa_landed\n"
                  "mov lr, x0\n"
                  "%s\n"
                  "%s\n"
                  "movz x0, #0x1111\n"
                  "b pauth_retaa_end\n"
                  "pauth_retaa_landed:\n"
                  "movz x0, #0x2222\n"
                  "mov lr, x1\n"
                  "pauth_retaa_end:\n",
                  signMnemonic, retMnemonic);

  t.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{}};
  t.vm.addMnemonicCB(retUpper, QBDI::PREINST, checkAccess, &expectedPre);
  t.vm.addMnemonicCB(retUpper, QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::rword retval;
  bool ran = t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"pauth"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(retval == 0x2222);
}

void testRetaaSppcLike(APITest &t, const char *signMnemonic,
                       const char *retMnemonic, const char *retUpper) {
  if (!checkFeature("pauth-lr")) {
    return;
  }

  char source[384];
  checkedSnprintf(source,
                  "mov x2, lr\n"
                  "adr x0, pauth_sppc_landed\n"
                  "mov lr, x0\n"
                  "pauth_sppc_label:\n"
                  "%s\n"
                  "%s pauth_sppc_label\n"
                  "movz x0, #0x1111\n"
                  "b pauth_sppc_end\n"
                  "pauth_sppc_landed:\n"
                  "movz x0, #0x2222\n"
                  "mov lr, x2\n"
                  "pauth_sppc_end:\n",
                  signMnemonic, retMnemonic);

  t.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{}};
  t.vm.addMnemonicCB(retUpper, QBDI::PREINST, checkAccess, &expectedPre);
  t.vm.addMnemonicCB(retUpper, QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::rword retval;
  bool ran =
      t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"pauth-lr"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(retval == 0x2222);
}

void testRetaaSppcRegLike(APITest &t, const char *signMnemonic,
                          const char *retMnemonic, const char *retUpper) {
  if (!checkFeature("pauth-lr")) {
    return;
  }

  char source[384];
  checkedSnprintf(source,
                  "mov x2, lr\n"
                  "adr x0, pauth_sppcr_landed\n"
                  "mov lr, x0\n"
                  "pauth_sppcr_label:\n"
                  "%s\n"
                  "adr x9, pauth_sppcr_label\n"
                  "%s x9\n"
                  "movz x0, #0x1111\n"
                  "b pauth_sppcr_end\n"
                  "pauth_sppcr_landed:\n"
                  "movz x0, #0x2222\n"
                  "mov lr, x2\n"
                  "pauth_sppcr_end:\n",
                  signMnemonic, retMnemonic);

  t.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{}};
  t.vm.addMnemonicCB(retUpper, QBDI::PREINST, checkAccess, &expectedPre);
  t.vm.addMnemonicCB(retUpper, QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::rword retval;
  bool ran =
      t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"pauth-lr"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(retval == 0x2222);
}

QBDI::VMAction checkAuthSppcResult(QBDI::VMInstanceRef vmi,
                                   QBDI::GPRState *gprState,
                                   QBDI::FPRState *fprState, void *data) {
  bool *seen = static_cast<bool *>(data);
  REQUIRE_FALSE(*seen);
  *seen = true;
  CHECK(gprState->x4 == gprState->x3);
  return QBDI::VMAction::CONTINUE;
}

void testAuthSppcLike(APITest &t, const char *signMnemonic,
                      const char *authMnemonic, const char *authUpper) {
  if (!checkFeature("pauth-lr")) {
    return;
  }

  char source[384];
  checkedSnprintf(source,
                  "mov x2, lr\n"
                  "adr x0, pauth_auth_sppc_target\n"
                  "mov x3, x0\n"
                  "mov lr, x0\n"
                  "pauth_auth_sppc_label:\n"
                  "%s\n"
                  "%s pauth_auth_sppc_label\n"
                  "mov x4, lr\n"
                  "mov lr, x2\n"
                  "b pauth_auth_sppc_end\n"
                  "pauth_auth_sppc_target:\n"
                  "pauth_auth_sppc_end:\n",
                  signMnemonic, authMnemonic);

  t.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{}};
  t.vm.addMnemonicCB(authUpper, QBDI::PREINST, checkAccess, &expectedPre);
  t.vm.addMnemonicCB(authUpper, QBDI::POSTINST, checkAccess, &expectedPost);
  bool seenResult = false;
  t.vm.addMnemonicCB(authUpper, QBDI::POSTINST, checkAuthSppcResult,
                     &seenResult);

  QBDI::rword retval;
  bool ran =
      t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"pauth-lr"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenResult);
}

void testBlraaLike(APITest &t, const char *signMnemonic,
                   const char *branchMnemonic, const char *branchUpper) {
  if (!checkFeature("pauth")) {
    return;
  }

  char source[256];
  checkedSnprintf(source,
                  "mov x2, lr\n"
                  "adr x0, pauth_blraa_callee\n"
                  "movz x1, #0x5678\n"
                  "%s x0, x1\n"
                  "%s x0, x1\n"
                  "mov lr, x2\n"
                  "b pauth_blraa_end\n"
                  "pauth_blraa_callee:\n"
                  "movz x0, #0x3333\n"
                  "ret\n"
                  "pauth_blraa_end:\n",
                  signMnemonic, branchMnemonic);

  t.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  t.vm.addMnemonicCB(branchUpper, QBDI::PREINST, checkAccess, &expectedPre);
  ExpectedReturnAddr expectedPost;
  t.vm.addMnemonicCB(branchUpper, QBDI::POSTINST, checkLinkReg, &expectedPost);

  QBDI::rword retval;
  bool ran = t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"pauth"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(expectedPost.expected != 0);
  CHECK(retval == 0x3333);
}

void testBlraazLike(APITest &t, const char *signMnemonic,
                    const char *branchMnemonic, const char *branchUpper) {
  if (!checkFeature("pauth")) {
    return;
  }

  char source[256];
  checkedSnprintf(source,
                  "mov x2, lr\n"
                  "adr x0, pauth_blraaz_callee\n"
                  "%s x0\n"
                  "%s x0\n"
                  "mov lr, x2\n"
                  "b pauth_blraaz_end\n"
                  "pauth_blraaz_callee:\n"
                  "movz x0, #0x3333\n"
                  "ret\n"
                  "pauth_blraaz_end:\n",
                  signMnemonic, branchMnemonic);

  t.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  t.vm.addMnemonicCB(branchUpper, QBDI::PREINST, checkAccess, &expectedPre);
  ExpectedReturnAddr expectedPost;
  t.vm.addMnemonicCB(branchUpper, QBDI::POSTINST, checkLinkReg, &expectedPost);

  QBDI::rword retval;
  bool ran = t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"pauth"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(expectedPost.expected != 0);
  CHECK(retval == 0x3333);
}

void testLdraaIndexed(APITest &t, const char *signMnemonic,
                      const char *ldrMnemonic, const char *ldrUpper) {
  if (!checkFeature("pauth")) {
    return;
  }

  char source[128];
  checkedSnprintf(source,
                  "%s x2\n"
                  "%s x0, [x2, #8]\n",
                  signMnemonic, ldrMnemonic);

  alignas(8)
      QBDI::rword mem[4] = {0x11111111, 0x22222222, 0x33333333, 0x44444444};

  t.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&mem[1], 0x22222222, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&mem[1], 0x22222222, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  t.vm.addMnemonicCB(ldrUpper, QBDI::PREINST, checkAccess, &expectedPre);
  t.vm.addMnemonicCB(ldrUpper, QBDI::POSTINST, checkAccess, &expectedPost);
  ExpectedRegValue expectedReg = {offsetof(QBDI::GPRState, x0), 0x22222222};
  t.vm.addMnemonicCB(ldrUpper, QBDI::POSTINST, checkRegValue, &expectedReg);

  QBDI::GPRState *state = t.vm.getGPRState();
  state->x2 = (QBDI::rword)&mem[0];
  t.vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"pauth"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(expectedReg.see);
}

void testLdraaWriteback(APITest &t, const char *signMnemonic,
                        const char *ldrMnemonic, const char *ldrUpper) {
  if (!checkFeature("pauth")) {
    return;
  }

  char source[128];
  checkedSnprintf(source,
                  "%s x2\n"
                  "%s x0, [x2, #8]!\n",
                  signMnemonic, ldrMnemonic);

  alignas(8)
      QBDI::rword mem[4] = {0x11111111, 0x22222222, 0x33333333, 0x44444444};

  t.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&mem[1], 0x22222222, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&mem[1], 0x22222222, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  t.vm.addMnemonicCB(ldrUpper, QBDI::PREINST, checkAccess, &expectedPre);
  t.vm.addMnemonicCB(ldrUpper, QBDI::POSTINST, checkAccess, &expectedPost);
  ExpectedRegValue expectedX0 = {offsetof(QBDI::GPRState, x0), 0x22222222};
  t.vm.addMnemonicCB(ldrUpper, QBDI::POSTINST, checkRegValue, &expectedX0);
  ExpectedRegValue expectedX2 = {offsetof(QBDI::GPRState, x2),
                                 (QBDI::rword)&mem[1]};
  t.vm.addMnemonicCB(ldrUpper, QBDI::POSTINST, checkRegValue, &expectedX2);

  QBDI::GPRState *state = t.vm.getGPRState();
  state->x2 = (QBDI::rword)&mem[0];
  t.vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"pauth"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(expectedX0.see);
  CHECK(expectedX2.see);
}

} // namespace

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autda") {
  testAuthTwoReg(*this, "pacda", "autda", "AUTDA");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autdb") {
  testAuthTwoReg(*this, "pacdb", "autdb", "AUTDB");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autia") {
  testAuthTwoReg(*this, "pacia", "autia", "AUTIA");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autib") {
  testAuthTwoReg(*this, "pacib", "autib", "AUTIB");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autdza") {
  testAuthZero(*this, "pacdza", "autdza", "AUTDZA");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autdzb") {
  testAuthZero(*this, "pacdzb", "autdzb", "AUTDZB");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autiza") {
  testAuthZero(*this, "paciza", "autiza", "AUTIZA");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autizb") {
  testAuthZero(*this, "pacizb", "autizb", "AUTIZB");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autia1716") {
  testAuth1716(*this, "pacia1716", "autia1716", "AUTIA1716");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autib1716") {
  testAuth1716(*this, "pacib1716", "autib1716", "AUTIB1716");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autiasp") {
  testAuthImplicitLR(*this, "paciasp", "autiasp", "AUTIASP");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autiaz") {
  testAuthImplicitLR(*this, "paciaz", "autiaz", "AUTIAZ");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autibsp") {
  testAuthImplicitLR(*this, "pacibsp", "autibsp", "AUTIBSP");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autibz") {
  testAuthImplicitLR(*this, "pacibz", "autibz", "AUTIBZ");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-braa") {
  testBraaLike(*this, "pacia", "braa", "BRAA");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-brab") {
  testBraaLike(*this, "pacib", "brab", "BRAB");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-braaz") {
  testBraazLike(*this, "paciza", "braaz", "BRAAZ");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-brabz") {
  testBraazLike(*this, "pacizb", "brabz", "BRABZ");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-retaa") {
  testRetaaLike(*this, "paciasp", "retaa", "RETAA");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-retab") {
  testRetaaLike(*this, "pacibsp", "retab", "RETAB");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-retaasppci") {
  testRetaaSppcLike(*this, "paciasppc", "retaasppc", "RETAASPPCi");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-retabsppci") {
  testRetaaSppcLike(*this, "pacibsppc", "retabsppc", "RETABSPPCi");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-retaasppcr") {
  testRetaaSppcRegLike(*this, "paciasppc", "retaasppcr", "RETAASPPCr");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-retabsppcr") {
  testRetaaSppcRegLike(*this, "pacibsppc", "retabsppcr", "RETABSPPCr");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autiasppci") {
  testAuthSppcLike(*this, "paciasppc", "autiasppc", "AUTIASPPCi");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autibsppci") {
  testAuthSppcLike(*this, "pacibsppc", "autibsppc", "AUTIBSPPCi");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-blraa") {
  testBlraaLike(*this, "pacia", "blraa", "BLRAA");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-blrab") {
  testBlraaLike(*this, "pacib", "blrab", "BLRAB");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-blraaz") {
  testBlraazLike(*this, "paciza", "blraaz", "BLRAAZ");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-blrabz") {
  testBlraazLike(*this, "pacizb", "blrabz", "BLRABZ");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldraaindexed") {
  testLdraaIndexed(*this, "pacdza", "ldraa", "LDRAAindexed");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrabindexed") {
  testLdraaIndexed(*this, "pacdzb", "ldrab", "LDRABindexed");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldraawriteback") {
  testLdraaWriteback(*this, "pacdza", "ldraa", "LDRAAwriteback");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrabwriteback") {
  testLdraaWriteback(*this, "pacdzb", "ldrab", "LDRABwriteback");
}
