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
#include <cstdio>
#include <string>
#include <vector>

#include "MemAccessTestUtils_AARCH64.h"

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::checkedSnprintf;
using QBDITestBatch2::checkFeature;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

namespace {

bool checkFeatures(const std::vector<std::string> &feats) {
  for (const std::string &f : feats) {
    if (!checkFeature(f.c_str())) {
      return false;
    }
  }
  return true;
}

void testMopsCpyPrologue(APITest &t, const char *asmMnemonic,
                         const char *upperMnemonic) {
  if (!checkFeature("mops")) {
    return;
  }

  char source[64];
  checkedSnprintf(source, "%s [x0]!, [x1]!, x2!\n", asmMnemonic);

  char dst[512];
  char src[512];

  t.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)src, 0x100, 0x100, QBDI::MEMORY_READ,
       QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)src, 0x100, 0x100, QBDI::MEMORY_READ,
       QBDI::MEMORY_UNKNOWN_VALUE},
      {(QBDI::rword)dst, 0x100, 0x100, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  t.vm.addMnemonicCB(upperMnemonic, QBDI::PREINST, checkAccess, &expectedPre);
  t.vm.addMnemonicCB(upperMnemonic, QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = t.vm.getGPRState();
  state->x0 = (QBDI::rword)dst;
  state->x1 = (QBDI::rword)src;
  state->x2 = 0x100;
  t.vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"mops"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

void testMopsSetPrologueWithValue(APITest &t, const char *asmMnemonic,
                                  const char *upperMnemonic,
                                  const std::vector<std::string> &feats) {
  if (!checkFeatures(feats)) {
    return;
  }

  char source[64];
  checkedSnprintf(source, "%s [x0]!, x1!, x2\n", asmMnemonic);

  alignas(16) char dst[1024];

  t.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)dst, 0x100, 0x100, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  t.vm.addMnemonicCB(upperMnemonic, QBDI::PREINST, checkAccess, &expectedPre);
  t.vm.addMnemonicCB(upperMnemonic, QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = t.vm.getGPRState();
  state->x0 = (QBDI::rword)dst;
  state->x1 = 0x100;
  state->x2 = 0x41;
  t.vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, feats);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

void testMopsSetPrologueNoValue(APITest &t, const char *asmMnemonic,
                                const char *upperMnemonic,
                                const std::vector<std::string> &feats) {
  if (!checkFeatures(feats)) {
    return;
  }

  char source[64];
  checkedSnprintf(source, "%s [x0]!, x1!\n", asmMnemonic);

  alignas(16) char dst[1024];

  t.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)dst, 0x100, 0x100, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  t.vm.addMnemonicCB(upperMnemonic, QBDI::PREINST, checkAccess, &expectedPre);
  t.vm.addMnemonicCB(upperMnemonic, QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = t.vm.getGPRState();
  state->x0 = (QBDI::rword)dst;
  state->x1 = 0x100;
  t.vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, feats);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

void testMopsCpyUnsupported(APITest &t, const char *source,
                            const char *upperMnemonicUnderTest,
                            const std::vector<std::string> &feats) {
  if (!checkFeatures(feats)) {
    return;
  }

  char dst[512];
  char src[512];

  t.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{}};
  t.vm.addMnemonicCB(upperMnemonicUnderTest, QBDI::PREINST, checkAccess,
                     &expectedPre);
  t.vm.addMnemonicCB(upperMnemonicUnderTest, QBDI::POSTINST, checkAccess,
                     &expectedPost);

  QBDI::GPRState *state = t.vm.getGPRState();
  state->x0 = (QBDI::rword)dst;
  state->x1 = (QBDI::rword)src;
  state->x2 = 0x100;
  t.vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, feats);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

void testMopsSetUnsupportedWithValue(APITest &t, const char *source,
                                     const char *upperMnemonicUnderTest,
                                     const std::vector<std::string> &feats) {
  if (!checkFeatures(feats)) {
    return;
  }

  alignas(16) char dst[1024];

  t.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{}};
  t.vm.addMnemonicCB(upperMnemonicUnderTest, QBDI::PREINST, checkAccess,
                     &expectedPre);
  t.vm.addMnemonicCB(upperMnemonicUnderTest, QBDI::POSTINST, checkAccess,
                     &expectedPost);

  QBDI::GPRState *state = t.vm.getGPRState();
  state->x0 = (QBDI::rword)dst;
  state->x1 = 0x100;
  state->x2 = 0x41;
  t.vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, feats);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

void testMopsSetUnsupportedNoValue(APITest &t, const char *source,
                                   const char *upperMnemonicUnderTest,
                                   const std::vector<std::string> &feats) {
  if (!checkFeatures(feats)) {
    return;
  }

  alignas(16) char dst[1024];

  t.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{}};
  t.vm.addMnemonicCB(upperMnemonicUnderTest, QBDI::PREINST, checkAccess,
                     &expectedPre);
  t.vm.addMnemonicCB(upperMnemonicUnderTest, QBDI::POSTINST, checkAccess,
                     &expectedPost);

  QBDI::GPRState *state = t.vm.getGPRState();
  state->x0 = (QBDI::rword)dst;
  state->x1 = 0x100;
  t.vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = t.runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, feats);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

} // namespace

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfp") {
  testMopsCpyPrologue(*this, "cpyfp", "CPYFP");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfpn") {
  testMopsCpyPrologue(*this, "cpyfpn", "CPYFPN");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfprn") {
  testMopsCpyPrologue(*this, "cpyfprn", "CPYFPRN");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfprt") {
  testMopsCpyPrologue(*this, "cpyfprt", "CPYFPRT");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfprtn") {
  testMopsCpyPrologue(*this, "cpyfprtn", "CPYFPRTN");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfprtrn") {
  testMopsCpyPrologue(*this, "cpyfprtrn", "CPYFPRTRN");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfprtwn") {
  testMopsCpyPrologue(*this, "cpyfprtwn", "CPYFPRTWN");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfpt") {
  testMopsCpyPrologue(*this, "cpyfpt", "CPYFPT");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfptn") {
  testMopsCpyPrologue(*this, "cpyfptn", "CPYFPTN");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfptrn") {
  testMopsCpyPrologue(*this, "cpyfptrn", "CPYFPTRN");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfptwn") {
  testMopsCpyPrologue(*this, "cpyfptwn", "CPYFPTWN");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfpwn") {
  testMopsCpyPrologue(*this, "cpyfpwn", "CPYFPWN");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfpwt") {
  testMopsCpyPrologue(*this, "cpyfpwt", "CPYFPWT");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfpwtn") {
  testMopsCpyPrologue(*this, "cpyfpwtn", "CPYFPWTN");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfpwtrn") {
  testMopsCpyPrologue(*this, "cpyfpwtrn", "CPYFPWTRN");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfpwtwn") {
  testMopsCpyPrologue(*this, "cpyfpwtwn", "CPYFPWTWN");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyp") {
  testMopsCpyPrologue(*this, "cpyp", "CPYP");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpypn") {
  testMopsCpyPrologue(*this, "cpypn", "CPYPN");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyprn") {
  testMopsCpyPrologue(*this, "cpyprn", "CPYPRN");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyprt") {
  testMopsCpyPrologue(*this, "cpyprt", "CPYPRT");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyprtn") {
  testMopsCpyPrologue(*this, "cpyprtn", "CPYPRTN");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyprtrn") {
  testMopsCpyPrologue(*this, "cpyprtrn", "CPYPRTRN");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyprtwn") {
  testMopsCpyPrologue(*this, "cpyprtwn", "CPYPRTWN");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpypt") {
  testMopsCpyPrologue(*this, "cpypt", "CPYPT");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyptn") {
  testMopsCpyPrologue(*this, "cpyptn", "CPYPTN");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyptrn") {
  testMopsCpyPrologue(*this, "cpyptrn", "CPYPTRN");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyptwn") {
  testMopsCpyPrologue(*this, "cpyptwn", "CPYPTWN");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpypwn") {
  testMopsCpyPrologue(*this, "cpypwn", "CPYPWN");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpypwt") {
  testMopsCpyPrologue(*this, "cpypwt", "CPYPWT");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpypwtn") {
  testMopsCpyPrologue(*this, "cpypwtn", "CPYPWTN");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpypwtrn") {
  testMopsCpyPrologue(*this, "cpypwtrn", "CPYPWTRN");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpypwtwn") {
  testMopsCpyPrologue(*this, "cpypwtwn", "CPYPWTWN");
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-setp") {
  testMopsSetPrologueWithValue(*this, "setp", "SETP", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-setpn") {
  testMopsSetPrologueWithValue(*this, "setpn", "SETPN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-setpt") {
  testMopsSetPrologueWithValue(*this, "setpt", "SETPT", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-setptn") {
  testMopsSetPrologueWithValue(*this, "setptn", "SETPTN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-setgp") {
  testMopsSetPrologueWithValue(*this, "setgp", "SETGP", {"mops", "mte"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-setgpn") {
  testMopsSetPrologueWithValue(*this, "setgpn", "SETGPN", {"mops", "mte"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-setgpt") {
  testMopsSetPrologueWithValue(*this, "setgpt", "SETGPT", {"mops", "mte"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-setgptn") {
  testMopsSetPrologueWithValue(*this, "setgptn", "SETGPTN", {"mops", "mte"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-setgop") {
  testMopsSetPrologueNoValue(*this, "setgop", "SETGOP",
                             {"mops", "mte", "mops-go"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-setgopn") {
  testMopsSetPrologueNoValue(*this, "setgopn", "SETGOPN",
                             {"mops", "mte", "mops-go"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-setgopt") {
  testMopsSetPrologueNoValue(*this, "setgopt", "SETGOPT",
                             {"mops", "mte", "mops-go"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-setgoptn") {
  testMopsSetPrologueNoValue(*this, "setgoptn", "SETGOPTN",
                             {"mops", "mte", "mops-go"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfm") {
  const char source[] =
      "cpyfp [x0]!, [x1]!, x2!\n"
      "cpyfm [x0]!, [x1]!, x2!\n"
      "cpyfe [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFM", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfe") {
  const char source[] =
      "cpyfp [x0]!, [x1]!, x2!\n"
      "cpyfm [x0]!, [x1]!, x2!\n"
      "cpyfe [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFE", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfmn") {
  const char source[] =
      "cpyfpn [x0]!, [x1]!, x2!\n"
      "cpyfmn [x0]!, [x1]!, x2!\n"
      "cpyfen [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFMN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfen") {
  const char source[] =
      "cpyfpn [x0]!, [x1]!, x2!\n"
      "cpyfmn [x0]!, [x1]!, x2!\n"
      "cpyfen [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFEN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfmrn") {
  const char source[] =
      "cpyfprn [x0]!, [x1]!, x2!\n"
      "cpyfmrn [x0]!, [x1]!, x2!\n"
      "cpyfern [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFMRN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfern") {
  const char source[] =
      "cpyfprn [x0]!, [x1]!, x2!\n"
      "cpyfmrn [x0]!, [x1]!, x2!\n"
      "cpyfern [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFERN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfmrt") {
  const char source[] =
      "cpyfprt [x0]!, [x1]!, x2!\n"
      "cpyfmrt [x0]!, [x1]!, x2!\n"
      "cpyfert [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFMRT", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfert") {
  const char source[] =
      "cpyfprt [x0]!, [x1]!, x2!\n"
      "cpyfmrt [x0]!, [x1]!, x2!\n"
      "cpyfert [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFERT", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfmrtn") {
  const char source[] =
      "cpyfprtn [x0]!, [x1]!, x2!\n"
      "cpyfmrtn [x0]!, [x1]!, x2!\n"
      "cpyfertn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFMRTN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfertn") {
  const char source[] =
      "cpyfprtn [x0]!, [x1]!, x2!\n"
      "cpyfmrtn [x0]!, [x1]!, x2!\n"
      "cpyfertn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFERTN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfmrtrn") {
  const char source[] =
      "cpyfprtrn [x0]!, [x1]!, x2!\n"
      "cpyfmrtrn [x0]!, [x1]!, x2!\n"
      "cpyfertrn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFMRTRN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfertrn") {
  const char source[] =
      "cpyfprtrn [x0]!, [x1]!, x2!\n"
      "cpyfmrtrn [x0]!, [x1]!, x2!\n"
      "cpyfertrn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFERTRN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfmrtwn") {
  const char source[] =
      "cpyfprtwn [x0]!, [x1]!, x2!\n"
      "cpyfmrtwn [x0]!, [x1]!, x2!\n"
      "cpyfertwn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFMRTWN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfertwn") {
  const char source[] =
      "cpyfprtwn [x0]!, [x1]!, x2!\n"
      "cpyfmrtwn [x0]!, [x1]!, x2!\n"
      "cpyfertwn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFERTWN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfmt") {
  const char source[] =
      "cpyfpt [x0]!, [x1]!, x2!\n"
      "cpyfmt [x0]!, [x1]!, x2!\n"
      "cpyfet [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFMT", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfet") {
  const char source[] =
      "cpyfpt [x0]!, [x1]!, x2!\n"
      "cpyfmt [x0]!, [x1]!, x2!\n"
      "cpyfet [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFET", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfmtn") {
  const char source[] =
      "cpyfptn [x0]!, [x1]!, x2!\n"
      "cpyfmtn [x0]!, [x1]!, x2!\n"
      "cpyfetn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFMTN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfetn") {
  const char source[] =
      "cpyfptn [x0]!, [x1]!, x2!\n"
      "cpyfmtn [x0]!, [x1]!, x2!\n"
      "cpyfetn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFETN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfmtrn") {
  const char source[] =
      "cpyfptrn [x0]!, [x1]!, x2!\n"
      "cpyfmtrn [x0]!, [x1]!, x2!\n"
      "cpyfetrn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFMTRN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfetrn") {
  const char source[] =
      "cpyfptrn [x0]!, [x1]!, x2!\n"
      "cpyfmtrn [x0]!, [x1]!, x2!\n"
      "cpyfetrn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFETRN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfmtwn") {
  const char source[] =
      "cpyfptwn [x0]!, [x1]!, x2!\n"
      "cpyfmtwn [x0]!, [x1]!, x2!\n"
      "cpyfetwn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFMTWN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfetwn") {
  const char source[] =
      "cpyfptwn [x0]!, [x1]!, x2!\n"
      "cpyfmtwn [x0]!, [x1]!, x2!\n"
      "cpyfetwn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFETWN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfmwn") {
  const char source[] =
      "cpyfpwn [x0]!, [x1]!, x2!\n"
      "cpyfmwn [x0]!, [x1]!, x2!\n"
      "cpyfewn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFMWN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfewn") {
  const char source[] =
      "cpyfpwn [x0]!, [x1]!, x2!\n"
      "cpyfmwn [x0]!, [x1]!, x2!\n"
      "cpyfewn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFEWN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfmwt") {
  const char source[] =
      "cpyfpwt [x0]!, [x1]!, x2!\n"
      "cpyfmwt [x0]!, [x1]!, x2!\n"
      "cpyfewt [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFMWT", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfewt") {
  const char source[] =
      "cpyfpwt [x0]!, [x1]!, x2!\n"
      "cpyfmwt [x0]!, [x1]!, x2!\n"
      "cpyfewt [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFEWT", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfmwtn") {
  const char source[] =
      "cpyfpwtn [x0]!, [x1]!, x2!\n"
      "cpyfmwtn [x0]!, [x1]!, x2!\n"
      "cpyfewtn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFMWTN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfewtn") {
  const char source[] =
      "cpyfpwtn [x0]!, [x1]!, x2!\n"
      "cpyfmwtn [x0]!, [x1]!, x2!\n"
      "cpyfewtn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFEWTN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfmwtrn") {
  const char source[] =
      "cpyfpwtrn [x0]!, [x1]!, x2!\n"
      "cpyfmwtrn [x0]!, [x1]!, x2!\n"
      "cpyfewtrn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFMWTRN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfewtrn") {
  const char source[] =
      "cpyfpwtrn [x0]!, [x1]!, x2!\n"
      "cpyfmwtrn [x0]!, [x1]!, x2!\n"
      "cpyfewtrn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFEWTRN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfmwtwn") {
  const char source[] =
      "cpyfpwtwn [x0]!, [x1]!, x2!\n"
      "cpyfmwtwn [x0]!, [x1]!, x2!\n"
      "cpyfewtwn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFMWTWN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyfewtwn") {
  const char source[] =
      "cpyfpwtwn [x0]!, [x1]!, x2!\n"
      "cpyfmwtwn [x0]!, [x1]!, x2!\n"
      "cpyfewtwn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYFEWTWN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpym") {
  const char source[] =
      "cpyp [x0]!, [x1]!, x2!\n"
      "cpym [x0]!, [x1]!, x2!\n"
      "cpye [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYM", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpye") {
  const char source[] =
      "cpyp [x0]!, [x1]!, x2!\n"
      "cpym [x0]!, [x1]!, x2!\n"
      "cpye [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYE", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpymn") {
  const char source[] =
      "cpypn [x0]!, [x1]!, x2!\n"
      "cpymn [x0]!, [x1]!, x2!\n"
      "cpyen [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYMN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyen") {
  const char source[] =
      "cpypn [x0]!, [x1]!, x2!\n"
      "cpymn [x0]!, [x1]!, x2!\n"
      "cpyen [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYEN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpymrn") {
  const char source[] =
      "cpyprn [x0]!, [x1]!, x2!\n"
      "cpymrn [x0]!, [x1]!, x2!\n"
      "cpyern [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYMRN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyern") {
  const char source[] =
      "cpyprn [x0]!, [x1]!, x2!\n"
      "cpymrn [x0]!, [x1]!, x2!\n"
      "cpyern [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYERN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpymrt") {
  const char source[] =
      "cpyprt [x0]!, [x1]!, x2!\n"
      "cpymrt [x0]!, [x1]!, x2!\n"
      "cpyert [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYMRT", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyert") {
  const char source[] =
      "cpyprt [x0]!, [x1]!, x2!\n"
      "cpymrt [x0]!, [x1]!, x2!\n"
      "cpyert [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYERT", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpymrtn") {
  const char source[] =
      "cpyprtn [x0]!, [x1]!, x2!\n"
      "cpymrtn [x0]!, [x1]!, x2!\n"
      "cpyertn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYMRTN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyertn") {
  const char source[] =
      "cpyprtn [x0]!, [x1]!, x2!\n"
      "cpymrtn [x0]!, [x1]!, x2!\n"
      "cpyertn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYERTN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpymrtrn") {
  const char source[] =
      "cpyprtrn [x0]!, [x1]!, x2!\n"
      "cpymrtrn [x0]!, [x1]!, x2!\n"
      "cpyertrn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYMRTRN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyertrn") {
  const char source[] =
      "cpyprtrn [x0]!, [x1]!, x2!\n"
      "cpymrtrn [x0]!, [x1]!, x2!\n"
      "cpyertrn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYERTRN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpymrtwn") {
  const char source[] =
      "cpyprtwn [x0]!, [x1]!, x2!\n"
      "cpymrtwn [x0]!, [x1]!, x2!\n"
      "cpyertwn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYMRTWN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyertwn") {
  const char source[] =
      "cpyprtwn [x0]!, [x1]!, x2!\n"
      "cpymrtwn [x0]!, [x1]!, x2!\n"
      "cpyertwn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYERTWN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpymt") {
  const char source[] =
      "cpypt [x0]!, [x1]!, x2!\n"
      "cpymt [x0]!, [x1]!, x2!\n"
      "cpyet [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYMT", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyet") {
  const char source[] =
      "cpypt [x0]!, [x1]!, x2!\n"
      "cpymt [x0]!, [x1]!, x2!\n"
      "cpyet [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYET", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpymtn") {
  const char source[] =
      "cpyptn [x0]!, [x1]!, x2!\n"
      "cpymtn [x0]!, [x1]!, x2!\n"
      "cpyetn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYMTN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyetn") {
  const char source[] =
      "cpyptn [x0]!, [x1]!, x2!\n"
      "cpymtn [x0]!, [x1]!, x2!\n"
      "cpyetn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYETN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpymtrn") {
  const char source[] =
      "cpyptrn [x0]!, [x1]!, x2!\n"
      "cpymtrn [x0]!, [x1]!, x2!\n"
      "cpyetrn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYMTRN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyetrn") {
  const char source[] =
      "cpyptrn [x0]!, [x1]!, x2!\n"
      "cpymtrn [x0]!, [x1]!, x2!\n"
      "cpyetrn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYETRN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpymtwn") {
  const char source[] =
      "cpyptwn [x0]!, [x1]!, x2!\n"
      "cpymtwn [x0]!, [x1]!, x2!\n"
      "cpyetwn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYMTWN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyetwn") {
  const char source[] =
      "cpyptwn [x0]!, [x1]!, x2!\n"
      "cpymtwn [x0]!, [x1]!, x2!\n"
      "cpyetwn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYETWN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpymwn") {
  const char source[] =
      "cpypwn [x0]!, [x1]!, x2!\n"
      "cpymwn [x0]!, [x1]!, x2!\n"
      "cpyewn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYMWN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyewn") {
  const char source[] =
      "cpypwn [x0]!, [x1]!, x2!\n"
      "cpymwn [x0]!, [x1]!, x2!\n"
      "cpyewn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYEWN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpymwt") {
  const char source[] =
      "cpypwt [x0]!, [x1]!, x2!\n"
      "cpymwt [x0]!, [x1]!, x2!\n"
      "cpyewt [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYMWT", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyewt") {
  const char source[] =
      "cpypwt [x0]!, [x1]!, x2!\n"
      "cpymwt [x0]!, [x1]!, x2!\n"
      "cpyewt [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYEWT", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpymwtn") {
  const char source[] =
      "cpypwtn [x0]!, [x1]!, x2!\n"
      "cpymwtn [x0]!, [x1]!, x2!\n"
      "cpyewtn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYMWTN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyewtn") {
  const char source[] =
      "cpypwtn [x0]!, [x1]!, x2!\n"
      "cpymwtn [x0]!, [x1]!, x2!\n"
      "cpyewtn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYEWTN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpymwtrn") {
  const char source[] =
      "cpypwtrn [x0]!, [x1]!, x2!\n"
      "cpymwtrn [x0]!, [x1]!, x2!\n"
      "cpyewtrn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYMWTRN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyewtrn") {
  const char source[] =
      "cpypwtrn [x0]!, [x1]!, x2!\n"
      "cpymwtrn [x0]!, [x1]!, x2!\n"
      "cpyewtrn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYEWTRN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpymwtwn") {
  const char source[] =
      "cpypwtwn [x0]!, [x1]!, x2!\n"
      "cpymwtwn [x0]!, [x1]!, x2!\n"
      "cpyewtwn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYMWTWN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cpyewtwn") {
  const char source[] =
      "cpypwtwn [x0]!, [x1]!, x2!\n"
      "cpymwtwn [x0]!, [x1]!, x2!\n"
      "cpyewtwn [x0]!, [x1]!, x2!\n";
  testMopsCpyUnsupported(*this, source, "CPYEWTWN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-setm") {
  const char source[] =
      "setp [x0]!, x1!, x2\n"
      "setm [x0]!, x1!, x2\n"
      "sete [x0]!, x1!, x2\n";
  testMopsSetUnsupportedWithValue(*this, source, "SETM", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-sete") {
  const char source[] =
      "setp [x0]!, x1!, x2\n"
      "setm [x0]!, x1!, x2\n"
      "sete [x0]!, x1!, x2\n";
  testMopsSetUnsupportedWithValue(*this, source, "SETE", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-setgm") {
  const char source[] =
      "setgp [x0]!, x1!, x2\n"
      "setgm [x0]!, x1!, x2\n"
      "setge [x0]!, x1!, x2\n";
  testMopsSetUnsupportedWithValue(*this, source, "SETGM", {"mops", "mte"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-mopssetge") {
  const char source[] =
      "setgp [x0]!, x1!, x2\n"
      "setgm [x0]!, x1!, x2\n"
      "setge [x0]!, x1!, x2\n";
  testMopsSetUnsupportedWithValue(*this, source, "MOPSSETGE", {"mops", "mte"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-setmn") {
  const char source[] =
      "setpn [x0]!, x1!, x2\n"
      "setmn [x0]!, x1!, x2\n"
      "seten [x0]!, x1!, x2\n";
  testMopsSetUnsupportedWithValue(*this, source, "SETMN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-seten") {
  const char source[] =
      "setpn [x0]!, x1!, x2\n"
      "setmn [x0]!, x1!, x2\n"
      "seten [x0]!, x1!, x2\n";
  testMopsSetUnsupportedWithValue(*this, source, "SETEN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-setgmn") {
  const char source[] =
      "setgpn [x0]!, x1!, x2\n"
      "setgmn [x0]!, x1!, x2\n"
      "setgen [x0]!, x1!, x2\n";
  testMopsSetUnsupportedWithValue(*this, source, "SETGMN", {"mops", "mte"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-mopssetgen") {
  const char source[] =
      "setgpn [x0]!, x1!, x2\n"
      "setgmn [x0]!, x1!, x2\n"
      "setgen [x0]!, x1!, x2\n";
  testMopsSetUnsupportedWithValue(*this, source, "MOPSSETGEN", {"mops", "mte"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-setmt") {
  const char source[] =
      "setpt [x0]!, x1!, x2\n"
      "setmt [x0]!, x1!, x2\n"
      "setet [x0]!, x1!, x2\n";
  testMopsSetUnsupportedWithValue(*this, source, "SETMT", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-setet") {
  const char source[] =
      "setpt [x0]!, x1!, x2\n"
      "setmt [x0]!, x1!, x2\n"
      "setet [x0]!, x1!, x2\n";
  testMopsSetUnsupportedWithValue(*this, source, "SETET", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-setgmt") {
  const char source[] =
      "setgpt [x0]!, x1!, x2\n"
      "setgmt [x0]!, x1!, x2\n"
      "setget [x0]!, x1!, x2\n";
  testMopsSetUnsupportedWithValue(*this, source, "SETGMT", {"mops", "mte"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-mopssetget") {
  const char source[] =
      "setgpt [x0]!, x1!, x2\n"
      "setgmt [x0]!, x1!, x2\n"
      "setget [x0]!, x1!, x2\n";
  testMopsSetUnsupportedWithValue(*this, source, "MOPSSETGET", {"mops", "mte"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-setmtn") {
  const char source[] =
      "setptn [x0]!, x1!, x2\n"
      "setmtn [x0]!, x1!, x2\n"
      "setetn [x0]!, x1!, x2\n";
  testMopsSetUnsupportedWithValue(*this, source, "SETMTN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-setetn") {
  const char source[] =
      "setptn [x0]!, x1!, x2\n"
      "setmtn [x0]!, x1!, x2\n"
      "setetn [x0]!, x1!, x2\n";
  testMopsSetUnsupportedWithValue(*this, source, "SETETN", {"mops"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-setgmtn") {
  const char source[] =
      "setgptn [x0]!, x1!, x2\n"
      "setgmtn [x0]!, x1!, x2\n"
      "setgetn [x0]!, x1!, x2\n";
  testMopsSetUnsupportedWithValue(*this, source, "SETGMTN", {"mops", "mte"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-mopssetgetn") {
  const char source[] =
      "setgptn [x0]!, x1!, x2\n"
      "setgmtn [x0]!, x1!, x2\n"
      "setgetn [x0]!, x1!, x2\n";
  testMopsSetUnsupportedWithValue(*this, source, "MOPSSETGETN",
                                  {"mops", "mte"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-setgom") {
  const char source[] =
      "setgop [x0]!, x1!\n"
      "setgom [x0]!, x1!\n"
      "setgoe [x0]!, x1!\n";
  testMopsSetUnsupportedNoValue(*this, source, "SETGOM",
                                {"mops", "mte", "mops-go"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-setgoe") {
  const char source[] =
      "setgop [x0]!, x1!\n"
      "setgom [x0]!, x1!\n"
      "setgoe [x0]!, x1!\n";
  testMopsSetUnsupportedNoValue(*this, source, "SETGOE",
                                {"mops", "mte", "mops-go"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-setgomn") {
  const char source[] =
      "setgopn [x0]!, x1!\n"
      "setgomn [x0]!, x1!\n"
      "setgoen [x0]!, x1!\n";
  testMopsSetUnsupportedNoValue(*this, source, "SETGOMN",
                                {"mops", "mte", "mops-go"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-setgoen") {
  const char source[] =
      "setgopn [x0]!, x1!\n"
      "setgomn [x0]!, x1!\n"
      "setgoen [x0]!, x1!\n";
  testMopsSetUnsupportedNoValue(*this, source, "SETGOEN",
                                {"mops", "mte", "mops-go"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-setgomt") {
  const char source[] =
      "setgopt [x0]!, x1!\n"
      "setgomt [x0]!, x1!\n"
      "setgoet [x0]!, x1!\n";
  testMopsSetUnsupportedNoValue(*this, source, "SETGOMT",
                                {"mops", "mte", "mops-go"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-setgoet") {
  const char source[] =
      "setgopt [x0]!, x1!\n"
      "setgomt [x0]!, x1!\n"
      "setgoet [x0]!, x1!\n";
  testMopsSetUnsupportedNoValue(*this, source, "SETGOET",
                                {"mops", "mte", "mops-go"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-setgomtn") {
  const char source[] =
      "setgoptn [x0]!, x1!\n"
      "setgomtn [x0]!, x1!\n"
      "setgoetn [x0]!, x1!\n";
  testMopsSetUnsupportedNoValue(*this, source, "SETGOMTN",
                                {"mops", "mte", "mops-go"});
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-setgoetn") {
  const char source[] =
      "setgoptn [x0]!, x1!\n"
      "setgomtn [x0]!, x1!\n"
      "setgoetn [x0]!, x1!\n";
  testMopsSetUnsupportedNoValue(*this, source, "SETGOETN",
                                {"mops", "mte", "mops-go"});
}
