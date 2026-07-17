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

void testAuthTwoRegBypass(APITest &t, const char *authMnemonic,
                          const char *authUpper) {
  if (!checkFeature("pauth")) {
    return;
  }
  t.vm.setOptions(t.vm.getOptions() | QBDI::Options::OPT_BYPASS_PAUTH);

  char source[128];
  checkedSnprintf(source,
                  "movz x0, #0x1234\n"
                  "movz x1, #0x5678\n"
                  "%s x0, x1\n",
                  authMnemonic);

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

void testAuthZeroBypass(APITest &t, const char *authMnemonic,
                        const char *authUpper) {
  if (!checkFeature("pauth")) {
    return;
  }
  t.vm.setOptions(t.vm.getOptions() | QBDI::Options::OPT_BYPASS_PAUTH);

  char source[128];
  checkedSnprintf(source,
                  "movz x0, #0x1234\n"
                  "%s x0\n",
                  authMnemonic);

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

void testAuth1716Bypass(APITest &t, const char *authMnemonic,
                        const char *authUpper) {
  if (!checkFeature("pauth")) {
    return;
  }
  t.vm.setOptions(t.vm.getOptions() | QBDI::Options::OPT_BYPASS_PAUTH);

  char source[128];
  checkedSnprintf(source,
                  "movz x17, #0x1234\n"
                  "movz x16, #0x5678\n"
                  "%s\n",
                  authMnemonic);

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

void testAuth171615Bypass(APITest &t, const char *authMnemonic,
                          const char *authUpper) {
  if (!checkFeature("pauth-lr")) {
    return;
  }
  t.vm.setOptions(t.vm.getOptions() | QBDI::Options::OPT_BYPASS_PAUTH);

  char source[128];
  checkedSnprintf(source,
                  "movz x17, #0x1234\n"
                  "movz x16, #0x5678\n"
                  "movz x15, #0x9abc\n"
                  "%s\n",
                  authMnemonic);

  t.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{}};
  t.vm.addMnemonicCB(authUpper, QBDI::PREINST, checkAccess, &expectedPre);
  t.vm.addMnemonicCB(authUpper, QBDI::POSTINST, checkAccess, &expectedPost);
  ExpectedRegValue expectedReg = {offsetof(QBDI::GPRState, x17), 0x1234};
  t.vm.addMnemonicCB(authUpper, QBDI::POSTINST, checkRegValue, &expectedReg);

  QBDI::rword retval;
  bool ran =
      t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"pauth-lr"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(expectedReg.see);
}

void testAuthSppcImmBypass(APITest &t, const char *authMnemonic,
                           const char *authUpper) {
  if (!checkFeature("pauth-lr")) {
    return;
  }
  t.vm.setOptions(t.vm.getOptions() | QBDI::Options::OPT_BYPASS_PAUTH);

  char source[256];
  checkedSnprintf(source,
                  "pauth_bypass_sppci_label:\n"
                  "mov x2, lr\n"
                  "movz x30, #0x1234\n"
                  "%s pauth_bypass_sppci_label\n"
                  "mov lr, x2\n",
                  authMnemonic);

  t.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{}};
  t.vm.addMnemonicCB(authUpper, QBDI::PREINST, checkAccess, &expectedPre);
  t.vm.addMnemonicCB(authUpper, QBDI::POSTINST, checkAccess, &expectedPost);
  ExpectedRegValue expectedReg = {offsetof(QBDI::GPRState, lr), 0x1234};
  t.vm.addMnemonicCB(authUpper, QBDI::POSTINST, checkRegValue, &expectedReg);

  QBDI::rword retval;
  bool ran =
      t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"pauth-lr"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(expectedReg.see);
}

void testAuthSppcRegBypass(APITest &t, const char *authMnemonic,
                           const char *authUpper) {
  if (!checkFeature("pauth-lr")) {
    return;
  }
  t.vm.setOptions(t.vm.getOptions() | QBDI::Options::OPT_BYPASS_PAUTH);

  char source[128];
  checkedSnprintf(source,
                  "mov x2, lr\n"
                  "movz x30, #0x1234\n"
                  "movz x5, #0x9999\n"
                  "%s x5\n"
                  "mov lr, x2\n",
                  authMnemonic);

  t.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{}};
  t.vm.addMnemonicCB(authUpper, QBDI::PREINST, checkAccess, &expectedPre);
  t.vm.addMnemonicCB(authUpper, QBDI::POSTINST, checkAccess, &expectedPost);
  ExpectedRegValue expectedReg = {offsetof(QBDI::GPRState, lr), 0x1234};
  t.vm.addMnemonicCB(authUpper, QBDI::POSTINST, checkRegValue, &expectedReg);

  QBDI::rword retval;
  bool ran =
      t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"pauth-lr"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(expectedReg.see);
}

void testAuthImplicitLRBypass(APITest &t, const char *authMnemonic,
                              const char *authUpper) {
  if (!checkFeature("pauth")) {
    return;
  }
  t.vm.setOptions(t.vm.getOptions() | QBDI::Options::OPT_BYPASS_PAUTH);

  char source[128];
  checkedSnprintf(source,
                  "mov x2, lr\n"
                  "movz x30, #0x1234\n"
                  "%s\n"
                  "mov lr, x2\n",
                  authMnemonic);

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

void testBraaLikeBypass(APITest &t, const char *branchMnemonic,
                        const char *branchUpper) {
  if (!checkFeature("pauth")) {
    return;
  }
  t.vm.setOptions(t.vm.getOptions() | QBDI::Options::OPT_BYPASS_PAUTH);

  char source[256];
  checkedSnprintf(source,
                  "adr x0, pauth_bypass_braa_landed\n"
                  "movz x1, #0xdead\n"
                  "%s x0, x1\n"
                  "movz x0, #0x1111\n"
                  "b pauth_bypass_braa_end\n"
                  "pauth_bypass_braa_landed:\n"
                  "movz x0, #0x2222\n"
                  "pauth_bypass_braa_end:\n",
                  branchMnemonic);

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

void testBraazLikeBypass(APITest &t, const char *branchMnemonic,
                         const char *branchUpper) {
  if (!checkFeature("pauth")) {
    return;
  }
  t.vm.setOptions(t.vm.getOptions() | QBDI::Options::OPT_BYPASS_PAUTH);

  char source[256];
  checkedSnprintf(source,
                  "adr x0, pauth_bypass_braaz_landed\n"
                  "%s x0\n"
                  "movz x0, #0x1111\n"
                  "b pauth_bypass_braaz_end\n"
                  "pauth_bypass_braaz_landed:\n"
                  "movz x0, #0x2222\n"
                  "pauth_bypass_braaz_end:\n",
                  branchMnemonic);

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

void testRetaaLikeBypass(APITest &t, const char *retMnemonic,
                         const char *retUpper) {
  if (!checkFeature("pauth")) {
    return;
  }
  t.vm.setOptions(t.vm.getOptions() | QBDI::Options::OPT_BYPASS_PAUTH);

  char source[256];
  checkedSnprintf(source,
                  "mov x1, lr\n"
                  "adr x0, pauth_bypass_retaa_landed\n"
                  "mov lr, x0\n"
                  "%s\n"
                  "movz x0, #0x1111\n"
                  "b pauth_bypass_retaa_end\n"
                  "pauth_bypass_retaa_landed:\n"
                  "movz x0, #0x2222\n"
                  "mov lr, x1\n"
                  "pauth_bypass_retaa_end:\n",
                  retMnemonic);

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

void testRetaaSppcLikeBypass(APITest &t, const char *retMnemonic,
                             const char *retUpper) {
  if (!checkFeature("pauth-lr")) {
    return;
  }
  t.vm.setOptions(t.vm.getOptions() | QBDI::Options::OPT_BYPASS_PAUTH);

  char source[256];
  checkedSnprintf(source,
                  "mov x1, lr\n"
                  "adr x0, pauth_bypass_sppc_landed\n"
                  "mov lr, x0\n"
                  "pauth_bypass_sppc_label:\n"
                  "%s pauth_bypass_sppc_label\n"
                  "movz x0, #0x1111\n"
                  "b pauth_bypass_sppc_end\n"
                  "pauth_bypass_sppc_landed:\n"
                  "movz x0, #0x2222\n"
                  "mov lr, x1\n"
                  "pauth_bypass_sppc_end:\n",
                  retMnemonic);

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

void testRetaaSppcRegLikeBypass(APITest &t, const char *retMnemonic,
                                const char *retUpper) {
  if (!checkFeature("pauth-lr")) {
    return;
  }
  t.vm.setOptions(t.vm.getOptions() | QBDI::Options::OPT_BYPASS_PAUTH);

  char source[256];
  checkedSnprintf(source,
                  "mov x1, lr\n"
                  "adr x0, pauth_bypass_sppcr_landed\n"
                  "mov lr, x0\n"
                  "movz x9, #0x9999\n"
                  "%s x9\n"
                  "movz x0, #0x1111\n"
                  "b pauth_bypass_sppcr_end\n"
                  "pauth_bypass_sppcr_landed:\n"
                  "movz x0, #0x2222\n"
                  "mov lr, x1\n"
                  "pauth_bypass_sppcr_end:\n",
                  retMnemonic);

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

void testBlraaLikeBypass(APITest &t, const char *branchMnemonic,
                         const char *branchUpper) {
  if (!checkFeature("pauth")) {
    return;
  }
  t.vm.setOptions(t.vm.getOptions() | QBDI::Options::OPT_BYPASS_PAUTH);

  char source[256];
  checkedSnprintf(source,
                  "mov x2, lr\n"
                  "adr x0, pauth_bypass_blraa_callee\n"
                  "movz x1, #0xdead\n"
                  "%s x0, x1\n"
                  "mov lr, x2\n"
                  "b pauth_bypass_blraa_end\n"
                  "pauth_bypass_blraa_callee:\n"
                  "movz x0, #0x3333\n"
                  "ret\n"
                  "pauth_bypass_blraa_end:\n",
                  branchMnemonic);

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

void testBlraazLikeBypass(APITest &t, const char *branchMnemonic,
                          const char *branchUpper) {
  if (!checkFeature("pauth")) {
    return;
  }
  t.vm.setOptions(t.vm.getOptions() | QBDI::Options::OPT_BYPASS_PAUTH);

  char source[256];
  checkedSnprintf(source,
                  "mov x2, lr\n"
                  "adr x0, pauth_bypass_blraaz_callee\n"
                  "%s x0\n"
                  "mov lr, x2\n"
                  "b pauth_bypass_blraaz_end\n"
                  "pauth_bypass_blraaz_callee:\n"
                  "movz x0, #0x3333\n"
                  "ret\n"
                  "pauth_bypass_blraaz_end:\n",
                  branchMnemonic);

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

void testLdraaIndexedBypass(APITest &t, const char *ldrMnemonic,
                            const char *ldrUpper) {
  if (!checkFeature("pauth")) {
    return;
  }
  t.vm.setOptions(t.vm.getOptions() | QBDI::Options::OPT_BYPASS_PAUTH);

  char source[128];
  checkedSnprintf(source, "%s x0, [x2, #8]\n", ldrMnemonic);

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

void testLdraaWritebackBypass(APITest &t, const char *ldrMnemonic,
                              const char *ldrUpper) {
  if (!checkFeature("pauth")) {
    return;
  }
  t.vm.setOptions(t.vm.getOptions() | QBDI::Options::OPT_BYPASS_PAUTH);

  char source[128];
  checkedSnprintf(source, "%s x0, [x2, #8]!\n", ldrMnemonic);

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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autda_bypass") {
  testAuthTwoRegBypass(*this, "autda", "AUTDA");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autdb_bypass") {
  testAuthTwoRegBypass(*this, "autdb", "AUTDB");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autia_bypass") {
  testAuthTwoRegBypass(*this, "autia", "AUTIA");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autib_bypass") {
  testAuthTwoRegBypass(*this, "autib", "AUTIB");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autdza_bypass") {
  testAuthZeroBypass(*this, "autdza", "AUTDZA");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autdzb_bypass") {
  testAuthZeroBypass(*this, "autdzb", "AUTDZB");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autiza_bypass") {
  testAuthZeroBypass(*this, "autiza", "AUTIZA");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autizb_bypass") {
  testAuthZeroBypass(*this, "autizb", "AUTIZB");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autia1716_bypass") {
  testAuth1716Bypass(*this, "autia1716", "AUTIA1716");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autib1716_bypass") {
  testAuth1716Bypass(*this, "autib1716", "AUTIB1716");
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-autia171615_bypass") {
  testAuth171615Bypass(*this, "autia171615", "AUTIA171615");
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-autib171615_bypass") {
  testAuth171615Bypass(*this, "autib171615", "AUTIB171615");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autiasp_bypass") {
  testAuthImplicitLRBypass(*this, "autiasp", "AUTIASP");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autiaz_bypass") {
  testAuthImplicitLRBypass(*this, "autiaz", "AUTIAZ");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autibsp_bypass") {
  testAuthImplicitLRBypass(*this, "autibsp", "AUTIBSP");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autibz_bypass") {
  testAuthImplicitLRBypass(*this, "autibz", "AUTIBZ");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-braa_bypass") {
  testBraaLikeBypass(*this, "braa", "BRAA");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-brab_bypass") {
  testBraaLikeBypass(*this, "brab", "BRAB");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-braaz_bypass") {
  testBraazLikeBypass(*this, "braaz", "BRAAZ");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-brabz_bypass") {
  testBraazLikeBypass(*this, "brabz", "BRABZ");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-retaa_bypass") {
  testRetaaLikeBypass(*this, "retaa", "RETAA");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-retab_bypass") {
  testRetaaLikeBypass(*this, "retab", "RETAB");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-retaasppci_bypass") {
  testRetaaSppcLikeBypass(*this, "retaasppc", "RETAASPPCi");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-retabsppci_bypass") {
  testRetaaSppcLikeBypass(*this, "retabsppc", "RETABSPPCi");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-retaasppcr_bypass") {
  testRetaaSppcRegLikeBypass(*this, "retaasppcr", "RETAASPPCr");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-retabsppcr_bypass") {
  testRetaaSppcRegLikeBypass(*this, "retabsppcr", "RETABSPPCr");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-blraa_bypass") {
  testBlraaLikeBypass(*this, "blraa", "BLRAA");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-blrab_bypass") {
  testBlraaLikeBypass(*this, "blrab", "BLRAB");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-blraaz_bypass") {
  testBlraazLikeBypass(*this, "blraaz", "BLRAAZ");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-blrabz_bypass") {
  testBlraazLikeBypass(*this, "blrabz", "BLRABZ");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autiasppci_bypass") {
  testAuthSppcImmBypass(*this, "autiasppc", "AUTIASPPCi");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autibsppci_bypass") {
  testAuthSppcImmBypass(*this, "autibsppc", "AUTIBSPPCi");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autiasppcr_bypass") {
  testAuthSppcRegBypass(*this, "autiasppcr", "AUTIASPPCr");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-autibsppcr_bypass") {
  testAuthSppcRegBypass(*this, "autibsppcr", "AUTIBSPPCr");
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-ldraaindexed_bypass") {
  testLdraaIndexedBypass(*this, "ldraa", "LDRAAindexed");
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-ldrabindexed_bypass") {
  testLdraaIndexedBypass(*this, "ldrab", "LDRABindexed");
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-ldraawriteback_bypass") {
  testLdraaWritebackBypass(*this, "ldraa", "LDRAAwriteback");
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-ldrabwriteback_bypass") {
  testLdraaWritebackBypass(*this, "ldrab", "LDRABwriteback");
}
