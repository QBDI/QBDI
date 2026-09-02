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

using QBDITestBatch2::checkedSnprintf;
using QBDITestBatch2::checkFeature;

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

struct PacmFlagCheck {
  QBDI::rword expected;
  int count = 0;
};

QBDI::VMAction checkPacmFlag(QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                             QBDI::FPRState *fprState, void *data) {
  PacmFlagCheck *info = static_cast<PacmFlagCheck *>(data);
  info->count++;
  CHECK(gprState->pacm == info->expected);
  return QBDI::VMAction::CONTINUE;
}

void testPacmRoundTrip1716(APITest &t, const char *signMnemonic,
                           const char *authMnemonic, const char *signUpper,
                           const char *authUpper) {
  char source[256];
  checkedSnprintf(source,
                  "movz x17, #0x1234\n"
                  "movz x16, #0x5678\n"
                  "movz x15, #0x9abc\n"
                  "pacm\n"
                  "%s\n"
                  "pacm\n"
                  "%s\n",
                  signMnemonic, authMnemonic);

  PacmFlagCheck pacmSet = {1};
  t.vm.addMnemonicCB("PACM", QBDI::POSTINST, checkPacmFlag, &pacmSet);
  PacmFlagCheck signCleared = {0};
  t.vm.addMnemonicCB(signUpper, QBDI::POSTINST, checkPacmFlag, &signCleared);
  PacmFlagCheck authCleared = {0};
  t.vm.addMnemonicCB(authUpper, QBDI::POSTINST, checkPacmFlag, &authCleared);
  ExpectedRegValue expectedReg = {offsetof(QBDI::GPRState, x17), 0x1234};
  t.vm.addMnemonicCB(authUpper, QBDI::POSTINST, checkRegValue, &expectedReg);

  QBDI::rword retval;
  bool ran =
      t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"pauth-lr"});

  CHECK(ran);
  CHECK(pacmSet.count == 2);
  CHECK(signCleared.count == 1);
  CHECK(authCleared.count == 1);
  CHECK(expectedReg.see);
}

void testPacmRoundTripSP(APITest &t, const char *signMnemonic,
                         const char *authMnemonic, const char *signUpper,
                         const char *authUpper) {
  char source[384];
  checkedSnprintf(source,
                  "mov x2, lr\n"
                  "movz x30, #0x1234\n"
                  "pacm\n"
                  "pacm_sp_sign:\n"
                  "%s\n"
                  "adr x16, pacm_sp_sign\n"
                  "pacm\n"
                  "%s\n"
                  "mov lr, x2\n",
                  signMnemonic, authMnemonic);

  PacmFlagCheck pacmSet = {1};
  t.vm.addMnemonicCB("PACM", QBDI::POSTINST, checkPacmFlag, &pacmSet);
  PacmFlagCheck signCleared = {0};
  t.vm.addMnemonicCB(signUpper, QBDI::POSTINST, checkPacmFlag, &signCleared);
  PacmFlagCheck authCleared = {0};
  t.vm.addMnemonicCB(authUpper, QBDI::POSTINST, checkPacmFlag, &authCleared);
  ExpectedRegValue expectedReg = {offsetof(QBDI::GPRState, lr), 0x1234};
  t.vm.addMnemonicCB(authUpper, QBDI::POSTINST, checkRegValue, &expectedReg);

  QBDI::rword retval;
  bool ran =
      t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"pauth-lr"});

  CHECK(ran);
  CHECK(pacmSet.count == 2);
  CHECK(signCleared.count == 1);
  CHECK(authCleared.count == 1);
  CHECK(expectedReg.see);
}

void testPacmRoundTripRetaa(APITest &t, const char *signMnemonic,
                            const char *retMnemonic, const char *signUpper,
                            const char *retUpper) {
  if (!checkFeature("pauth")) {
    return;
  }

  char source[512];
  checkedSnprintf(source,
                  "mov x1, lr\n"
                  "adr x0, pacm_retaa_landed\n"
                  "mov lr, x0\n"
                  "pacm\n"
                  "pacm_retaa_sign:\n"
                  "%s\n"
                  "adr x16, pacm_retaa_sign\n"
                  "pacm\n"
                  "%s\n"
                  "movz x0, #0x1111\n"
                  "b pacm_retaa_end\n"
                  "pacm_retaa_landed:\n"
                  "movz x0, #0x2222\n"
                  "mov lr, x1\n"
                  "pacm_retaa_end:\n",
                  signMnemonic, retMnemonic);

  PacmFlagCheck pacmSet = {1};
  t.vm.addMnemonicCB("PACM", QBDI::POSTINST, checkPacmFlag, &pacmSet);
  PacmFlagCheck signCleared = {0};
  t.vm.addMnemonicCB(signUpper, QBDI::POSTINST, checkPacmFlag, &signCleared);
  PacmFlagCheck retCleared = {0};
  t.vm.addMnemonicCB(retUpper, QBDI::POSTINST, checkPacmFlag, &retCleared);

  QBDI::rword retval;
  bool ran = t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT,
                        {"pauth", "pauth-lr"});

  CHECK(ran);
  CHECK(pacmSet.count == 2);
  CHECK(signCleared.count == 1);
  CHECK(retCleared.count == 1);
  CHECK(retval == 0x2222);
}

void testPacmSignSideBypass1716(APITest &t, const char *signMnemonic,
                                const char *signUpper) {
  t.vm.setOptions(t.vm.getOptions() | QBDI::Options::OPT_BYPASS_PAUTH);

  char source[256];
  checkedSnprintf(source,
                  "movz x17, #0x1234\n"
                  "movz x16, #0x5678\n"
                  "movz x15, #0x9abc\n"
                  "pacm\n"
                  "%s\n",
                  signMnemonic);

  PacmFlagCheck pacmSet = {1};
  t.vm.addMnemonicCB("PACM", QBDI::POSTINST, checkPacmFlag, &pacmSet);
  PacmFlagCheck signCleared = {0};
  t.vm.addMnemonicCB(signUpper, QBDI::POSTINST, checkPacmFlag, &signCleared);

  QBDI::rword retval;
  bool ran =
      t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"pauth-lr"});

  CHECK(ran);
  CHECK(pacmSet.count == 1);
  CHECK(signCleared.count == 1);
}

void testPacmSignSideBypassSP(APITest &t, const char *signMnemonic,
                              const char *signUpper) {
  t.vm.setOptions(t.vm.getOptions() | QBDI::Options::OPT_BYPASS_PAUTH);

  char source[128];
  checkedSnprintf(source,
                  "mov x2, lr\n"
                  "movz x30, #0x1234\n"
                  "pacm\n"
                  "%s\n"
                  "mov lr, x2\n",
                  signMnemonic);

  PacmFlagCheck pacmSet = {1};
  t.vm.addMnemonicCB("PACM", QBDI::POSTINST, checkPacmFlag, &pacmSet);
  PacmFlagCheck signCleared = {0};
  t.vm.addMnemonicCB(signUpper, QBDI::POSTINST, checkPacmFlag, &signCleared);

  QBDI::rword retval;
  bool ran =
      t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"pauth-lr"});

  CHECK(ran);
  CHECK(pacmSet.count == 1);
  CHECK(signCleared.count == 1);
}

void testPacmAuthSideBypassDropped1716(APITest &t, const char *authMnemonic,
                                       const char *authUpper) {
  t.vm.setOptions(t.vm.getOptions() | QBDI::Options::OPT_BYPASS_PAUTH);

  char source[128];
  checkedSnprintf(source,
                  "movz x17, #0x1234\n"
                  "movz x16, #0x5678\n"
                  "pacm\n"
                  "%s\n",
                  authMnemonic);

  PacmFlagCheck pacmNotSet = {0};
  t.vm.addMnemonicCB("PACM", QBDI::POSTINST, checkPacmFlag, &pacmNotSet);
  ExpectedRegValue expectedReg = {offsetof(QBDI::GPRState, x17), 0x1234};
  t.vm.addMnemonicCB(authUpper, QBDI::POSTINST, checkRegValue, &expectedReg);

  QBDI::rword retval;
  bool ran =
      t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"pauth-lr"});

  CHECK(ran);
  CHECK(pacmNotSet.count == 1);
  CHECK(expectedReg.see);
}

void testPacmAuthSideBypassDroppedSP(APITest &t, const char *authMnemonic,
                                     const char *authUpper) {
  t.vm.setOptions(t.vm.getOptions() | QBDI::Options::OPT_BYPASS_PAUTH);

  char source[128];
  checkedSnprintf(source,
                  "mov x2, lr\n"
                  "movz x30, #0x1234\n"
                  "pacm\n"
                  "%s\n"
                  "mov lr, x2\n",
                  authMnemonic);

  PacmFlagCheck pacmNotSet = {0};
  t.vm.addMnemonicCB("PACM", QBDI::POSTINST, checkPacmFlag, &pacmNotSet);
  ExpectedRegValue expectedReg = {offsetof(QBDI::GPRState, lr), 0x1234};
  t.vm.addMnemonicCB(authUpper, QBDI::POSTINST, checkRegValue, &expectedReg);

  QBDI::rword retval;
  bool ran =
      t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"pauth-lr"});

  CHECK(ran);
  CHECK(pacmNotSet.count == 1);
  CHECK(expectedReg.see);
}

void testPacmRetaaBypassDropped(APITest &t, const char *retMnemonic,
                                const char *retUpper) {
  if (!checkFeature("pauth")) {
    return;
  }
  t.vm.setOptions(t.vm.getOptions() | QBDI::Options::OPT_BYPASS_PAUTH);

  char source[256];
  checkedSnprintf(source,
                  "mov x1, lr\n"
                  "adr x0, pacm_retaa_bypass_landed\n"
                  "mov lr, x0\n"
                  "pacm\n"
                  "%s\n"
                  "movz x0, #0x1111\n"
                  "b pacm_retaa_bypass_end\n"
                  "pacm_retaa_bypass_landed:\n"
                  "movz x0, #0x2222\n"
                  "mov lr, x1\n"
                  "pacm_retaa_bypass_end:\n",
                  retMnemonic);

  PacmFlagCheck pacmNotSet = {0};
  t.vm.addMnemonicCB("PACM", QBDI::POSTINST, checkPacmFlag, &pacmNotSet);

  QBDI::rword retval;
  bool ran = t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT,
                        {"pauth", "pauth-lr"});

  CHECK(ran);
  CHECK(pacmNotSet.count == 1);
  CHECK(retval == 0x2222);
}

void testPacmSuspicious(APITest &t, bool bypass) {
  if (bypass) {
    t.vm.setOptions(t.vm.getOptions() | QBDI::Options::OPT_BYPASS_PAUTH);
  }

  const char *source =
      "pacm\n"
      "movz x0, #0x1234\n";

  PacmFlagCheck pacmNotSet = {0};
  t.vm.addMnemonicCB("PACM", QBDI::POSTINST, checkPacmFlag, &pacmNotSet);

  QBDI::rword retval;
  bool ran =
      t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"pauth-lr"});

  CHECK(ran);
  CHECK(pacmNotSet.count == 1);
  CHECK(retval == 0x1234);
}

} // namespace

TEST_CASE_METHOD(APITest, "PACMTest_AARCH64-roundtrip_pacia1716") {
  testPacmRoundTrip1716(*this, "pacia1716", "autia1716", "PACIA1716",
                        "AUTIA1716");
}

TEST_CASE_METHOD(APITest, "PACMTest_AARCH64-roundtrip_pacib1716") {
  testPacmRoundTrip1716(*this, "pacib1716", "autib1716", "PACIB1716",
                        "AUTIB1716");
}

TEST_CASE_METHOD(APITest, "PACMTest_AARCH64-roundtrip_paciasp") {
  testPacmRoundTripSP(*this, "paciasp", "autiasp", "PACIASP", "AUTIASP");
}

TEST_CASE_METHOD(APITest, "PACMTest_AARCH64-roundtrip_pacibsp") {
  testPacmRoundTripSP(*this, "pacibsp", "autibsp", "PACIBSP", "AUTIBSP");
}

TEST_CASE_METHOD(APITest, "PACMTest_AARCH64-roundtrip_retaa") {
  testPacmRoundTripRetaa(*this, "paciasp", "retaa", "PACIASP", "RETAA");
}

TEST_CASE_METHOD(APITest, "PACMTest_AARCH64-roundtrip_retab") {
  testPacmRoundTripRetaa(*this, "pacibsp", "retab", "PACIBSP", "RETAB");
}

TEST_CASE_METHOD(APITest, "PACMTest_AARCH64-signside_bypass_pacia1716") {
  testPacmSignSideBypass1716(*this, "pacia1716", "PACIA1716");
}

TEST_CASE_METHOD(APITest, "PACMTest_AARCH64-signside_bypass_pacib1716") {
  testPacmSignSideBypass1716(*this, "pacib1716", "PACIB1716");
}

TEST_CASE_METHOD(APITest, "PACMTest_AARCH64-signside_bypass_paciasp") {
  testPacmSignSideBypassSP(*this, "paciasp", "PACIASP");
}

TEST_CASE_METHOD(APITest, "PACMTest_AARCH64-signside_bypass_pacibsp") {
  testPacmSignSideBypassSP(*this, "pacibsp", "PACIBSP");
}

TEST_CASE_METHOD(APITest,
                 "PACMTest_AARCH64-authside_bypass_dropped_autia1716") {
  testPacmAuthSideBypassDropped1716(*this, "autia1716", "AUTIA1716");
}

TEST_CASE_METHOD(APITest,
                 "PACMTest_AARCH64-authside_bypass_dropped_autib1716") {
  testPacmAuthSideBypassDropped1716(*this, "autib1716", "AUTIB1716");
}

TEST_CASE_METHOD(APITest, "PACMTest_AARCH64-authside_bypass_dropped_autiasp") {
  testPacmAuthSideBypassDroppedSP(*this, "autiasp", "AUTIASP");
}

TEST_CASE_METHOD(APITest, "PACMTest_AARCH64-authside_bypass_dropped_autibsp") {
  testPacmAuthSideBypassDroppedSP(*this, "autibsp", "AUTIBSP");
}

TEST_CASE_METHOD(APITest, "PACMTest_AARCH64-retaa_bypass_dropped") {
  testPacmRetaaBypassDropped(*this, "retaa", "RETAA");
}

TEST_CASE_METHOD(APITest, "PACMTest_AARCH64-retab_bypass_dropped") {
  testPacmRetaaBypassDropped(*this, "retab", "RETAB");
}

TEST_CASE_METHOD(APITest, "PACMTest_AARCH64-suspicious") {
  testPacmSuspicious(*this, false);
}

TEST_CASE_METHOD(APITest, "PACMTest_AARCH64-suspicious_bypass") {
  testPacmSuspicious(*this, true);
}
