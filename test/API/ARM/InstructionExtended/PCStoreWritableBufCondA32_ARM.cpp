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
#include <cstring>
#include <string>
#include <vector>

#include "ConditionTestUtils_ARM.h"
#include "MemAccessTestUtils_ARM.h"

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::checkFeature;
using QBDITestBatch2::conditionForIndex;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;
using QBDITestBatch2::setConditionCPSR;

static QBDI::rword copySnippetToWritableBuffer(QBDI::VM &vm,
                                               QBDI::rword snippetAddr,
                                               std::vector<uint8_t> &buf) {
  memcpy(buf.data(), (void *)snippetAddr, buf.size());
  QBDI::rword bufAddr = (QBDI::rword)buf.data();
  vm.addInstrumentedRange(bufAddr, bufAddr + buf.size());
  vm.clearCache(bufAddr, bufAddr + buf.size());
  return bufAddr;
}

static void checkStrbi12PcCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(8);
  std::string source = std::string("strb") + cond.suffix +
                       " r1, [pc, #4]\n"
                       "bx lr\n"
                       ".word 0\n"
                       ".word 0\n";
  QBDI::rword snippetAddr = fixture.genASM(source.c_str());
  std::vector<uint8_t> codeBuf(32, 0);
  QBDI::rword bufAddr =
      copySnippetToWritableBuffer(fixture.vm, snippetAddr, codeBuf);
  codeBuf[12] = 0xcc;

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {
        {bufAddr + 12, 0xab, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    };
  }
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("STRBi12", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("STRBi12", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r1 = 0xab;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.vm.call(&retval, bufAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(codeBuf[12] == (taken ? 0xab : 0xcc));
}

static void checkStri12PcCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(16);
  std::string source = std::string("str") + cond.suffix +
                       " r1, [pc, #4]\n"
                       "bx lr\n"
                       ".word 0\n"
                       ".word 0\n";
  QBDI::rword snippetAddr = fixture.genASM(source.c_str());
  std::vector<uint8_t> codeBuf(32, 0);
  QBDI::rword bufAddr =
      copySnippetToWritableBuffer(fixture.vm, snippetAddr, codeBuf);
  *(uint32_t *)&codeBuf[12] = 0xcccccccc;

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {
        {bufAddr + 12, 0xabcdef01, 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
    };
  }
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("STRi12", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("STRi12", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r1 = 0xabcdef01;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.vm.call(&retval, bufAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&codeBuf[12] == (taken ? 0xabcdef01 : 0xcccccccc));
}

static void checkStrhPcCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(9);
  std::string source = std::string("strh") + cond.suffix +
                       " r1, [pc, #4]\n"
                       "bx lr\n"
                       ".word 0\n"
                       ".word 0\n";
  QBDI::rword snippetAddr = fixture.genASM(source.c_str());
  std::vector<uint8_t> codeBuf(32, 0);
  QBDI::rword bufAddr =
      copySnippetToWritableBuffer(fixture.vm, snippetAddr, codeBuf);
  codeBuf[12] = 0xcc;
  codeBuf[13] = 0xcc;

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {
        {bufAddr + 12, 0xabcd, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    };
  }
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("STRH", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("STRH", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r1 = 0xabcd;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.vm.call(&retval, bufAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint16_t *)&codeBuf[12] == (taken ? 0xabcd : 0xcccc));
}

static void checkVstmdiaPcCond(APITest &fixture, bool taken) {
  if (!checkFeature("vfp2")) {
    return;
  }
  const auto &cond = conditionForIndex(10);
  std::string source = std::string("vstmia") + cond.suffix +
                       " pc, {d0}\n"
                       "bx lr\n"
                       ".word 0\n"
                       ".word 0\n";
  QBDI::rword snippetAddr = fixture.genASM(source.c_str());
  std::vector<uint8_t> codeBuf(32, 0);
  QBDI::rword bufAddr =
      copySnippetToWritableBuffer(fixture.vm, snippetAddr, codeBuf);
  for (int i = 8; i < 16; i++) {
    codeBuf[i] = 0xcc;
  }

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {
        {bufAddr + 8, 0x44332211, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
        {bufAddr + 12, 0x88776655, 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
    };
  }
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("VSTMDIA", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("VSTMDIA", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::FPRState *fpr = fixture.vm.getFPRState();
  fpr->vreg.q[0][0] = 0x11;
  fpr->vreg.q[0][1] = 0x22;
  fpr->vreg.q[0][2] = 0x33;
  fpr->vreg.q[0][3] = 0x44;
  fpr->vreg.q[0][4] = 0x55;
  fpr->vreg.q[0][5] = 0x66;
  fpr->vreg.q[0][6] = 0x77;
  fpr->vreg.q[0][7] = 0x88;
  fixture.vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = fixture.vm.call(&retval, bufAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  if (taken) {
    CHECK(*(uint32_t *)&codeBuf[8] == 0x44332211);
    CHECK(*(uint32_t *)&codeBuf[12] == 0x88776655);
  } else {
    CHECK(*(uint32_t *)&codeBuf[8] == 0xcccccccc);
    CHECK(*(uint32_t *)&codeBuf[12] == 0xcccccccc);
  }
}

static void checkFstmxiaPcCond(APITest &fixture, bool taken) {
  if (!checkFeature("vfp2")) {
    return;
  }
  const auto &cond = conditionForIndex(15);
  std::string source = std::string("fstmiax") + cond.suffix +
                       " pc, {d0}\n"
                       "bx lr\n"
                       ".word 0\n"
                       ".word 0\n";
  QBDI::rword snippetAddr = fixture.genASM(source.c_str());
  std::vector<uint8_t> codeBuf(32, 0);
  QBDI::rword bufAddr =
      copySnippetToWritableBuffer(fixture.vm, snippetAddr, codeBuf);
  for (int i = 8; i < 16; i++) {
    codeBuf[i] = 0xcc;
  }

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {
        {bufAddr + 8, 0x44332211, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
        {bufAddr + 12, 0x88776655, 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
    };
  }
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("FSTMXIA", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("FSTMXIA", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::FPRState *fpr = fixture.vm.getFPRState();
  fpr->vreg.q[0][0] = 0x11;
  fpr->vreg.q[0][1] = 0x22;
  fpr->vreg.q[0][2] = 0x33;
  fpr->vreg.q[0][3] = 0x44;
  fpr->vreg.q[0][4] = 0x55;
  fpr->vreg.q[0][5] = 0x66;
  fpr->vreg.q[0][6] = 0x77;
  fpr->vreg.q[0][7] = 0x88;
  fixture.vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = fixture.vm.call(&retval, bufAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  if (taken) {
    CHECK(*(uint32_t *)&codeBuf[8] == 0x44332211);
    CHECK(*(uint32_t *)&codeBuf[12] == 0x88776655);
  } else {
    CHECK(*(uint32_t *)&codeBuf[8] == 0xcccccccc);
    CHECK(*(uint32_t *)&codeBuf[12] == 0xcccccccc);
  }
}

static void checkVstmsiaPcCond(APITest &fixture, bool taken) {
  if (!checkFeature("vfp2")) {
    return;
  }
  const auto &cond = conditionForIndex(11);
  std::string source = std::string("vstmia") + cond.suffix +
                       " pc, {s0}\n"
                       "bx lr\n"
                       ".word 0\n";
  QBDI::rword snippetAddr = fixture.genASM(source.c_str());
  std::vector<uint8_t> codeBuf(32, 0);
  QBDI::rword bufAddr =
      copySnippetToWritableBuffer(fixture.vm, snippetAddr, codeBuf);
  for (int i = 8; i < 12; i++) {
    codeBuf[i] = 0xcc;
  }

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {
        {bufAddr + 8, 0x44332211, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    };
  }
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("VSTMSIA", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("VSTMSIA", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::FPRState *fpr = fixture.vm.getFPRState();
  fpr->vreg.q[0][0] = 0x11;
  fpr->vreg.q[0][1] = 0x22;
  fpr->vreg.q[0][2] = 0x33;
  fpr->vreg.q[0][3] = 0x44;
  fixture.vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = fixture.vm.call(&retval, bufAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&codeBuf[8] == (taken ? 0x44332211u : 0xccccccccu));
}

static void checkVstrdPcCond(APITest &fixture, bool taken) {
  if (!checkFeature("vfp2")) {
    return;
  }
  const auto &cond = conditionForIndex(12);
  std::string source = std::string("vstr") + cond.suffix +
                       " d0, [pc, #8]\n"
                       "bx lr\n"
                       ".word 0\n"
                       ".word 0\n"
                       ".word 0\n"
                       ".word 0\n";
  QBDI::rword snippetAddr = fixture.genASM(source.c_str());
  std::vector<uint8_t> codeBuf(32, 0);
  QBDI::rword bufAddr =
      copySnippetToWritableBuffer(fixture.vm, snippetAddr, codeBuf);
  for (int i = 16; i < 24; i++) {
    codeBuf[i] = 0xcc;
  }

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {
        {bufAddr + 16, 0x44332211, 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
        {bufAddr + 20, 0x88776655, 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
    };
  }
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("VSTRD", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("VSTRD", QBDI::POSTINST, checkAccess, &expectedPost);

  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::FPRState *fpr = fixture.vm.getFPRState();
  fpr->vreg.q[0][0] = 0x11;
  fpr->vreg.q[0][1] = 0x22;
  fpr->vreg.q[0][2] = 0x33;
  fpr->vreg.q[0][3] = 0x44;
  fpr->vreg.q[0][4] = 0x55;
  fpr->vreg.q[0][5] = 0x66;
  fpr->vreg.q[0][6] = 0x77;
  fpr->vreg.q[0][7] = 0x88;
  fixture.vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = fixture.vm.call(&retval, bufAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  if (taken) {
    CHECK(*(uint32_t *)&codeBuf[16] == 0x44332211);
    CHECK(*(uint32_t *)&codeBuf[20] == 0x88776655);
  } else {
    CHECK(*(uint32_t *)&codeBuf[16] == 0xcccccccc);
    CHECK(*(uint32_t *)&codeBuf[20] == 0xcccccccc);
  }
}

static void checkVstrsPcCond(APITest &fixture, bool taken) {
  if (!checkFeature("vfp2")) {
    return;
  }
  const auto &cond = conditionForIndex(13);
  std::string source = std::string("vstr") + cond.suffix +
                       " s0, [pc, #8]\n"
                       "bx lr\n"
                       ".word 0\n"
                       ".word 0\n"
                       ".word 0\n";
  QBDI::rword snippetAddr = fixture.genASM(source.c_str());
  std::vector<uint8_t> codeBuf(32, 0);
  QBDI::rword bufAddr =
      copySnippetToWritableBuffer(fixture.vm, snippetAddr, codeBuf);
  for (int i = 16; i < 20; i++) {
    codeBuf[i] = 0xcc;
  }

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {
        {bufAddr + 16, 0x44332211, 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
    };
  }
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("VSTRS", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("VSTRS", QBDI::POSTINST, checkAccess, &expectedPost);

  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::FPRState *fpr = fixture.vm.getFPRState();
  fpr->vreg.q[0][0] = 0x11;
  fpr->vreg.q[0][1] = 0x22;
  fpr->vreg.q[0][2] = 0x33;
  fpr->vreg.q[0][3] = 0x44;
  fixture.vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = fixture.vm.call(&retval, bufAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&codeBuf[16] == (taken ? 0x44332211u : 0xccccccccu));
}

static void checkVstrhPcCond(APITest &fixture, bool taken) {
  if (!checkFeature("fullfp16")) {
    return;
  }
  const auto &cond = conditionForIndex(14);
  std::string source = std::string("vstr") + cond.suffix +
                       ".16 s0, [pc, #8]\n"
                       "bx lr\n"
                       ".word 0\n"
                       ".word 0\n"
                       ".word 0\n";
  QBDI::rword snippetAddr =
      fixture.genASM(source.c_str(), QBDI::CPUMode::ARM, {"fullfp16"});
  std::vector<uint8_t> codeBuf(32, 0);
  QBDI::rword bufAddr =
      copySnippetToWritableBuffer(fixture.vm, snippetAddr, codeBuf);
  codeBuf[16] = 0xcc;
  codeBuf[17] = 0xcc;

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {
        {bufAddr + 16, 0x1234, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    };
  }
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("VSTRH", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("VSTRH", QBDI::POSTINST, checkAccess, &expectedPost);

  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::FPRState *fpr = fixture.vm.getFPRState();
  fpr->vreg.q[0][0] = 0x34;
  fpr->vreg.q[0][1] = 0x12;
  fixture.vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = fixture.vm.call(&retval, bufAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint16_t *)&codeBuf[16] == (taken ? 0x1234 : 0xcccc));
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-strbi12_pc_base_cond_taken") {
  checkStrbi12PcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-strbi12_pc_base_cond_not_taken") {
  checkStrbi12PcCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-stri12_pc_base_cond_taken") {
  checkStri12PcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-stri12_pc_base_cond_not_taken") {
  checkStri12PcCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-strh_pc_base_cond_taken") {
  checkStrhPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-strh_pc_base_cond_not_taken") {
  checkStrhPcCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-vstmdia_pc_base_cond_taken") {
  checkVstmdiaPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-vstmdia_pc_base_cond_not_taken") {
  checkVstmdiaPcCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-fstmxia_pc_base_cond_taken") {
  checkFstmxiaPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-fstmxia_pc_base_cond_not_taken") {
  checkFstmxiaPcCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-vstmsia_pc_base_cond_taken") {
  checkVstmsiaPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-vstmsia_pc_base_cond_not_taken") {
  checkVstmsiaPcCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-vstrd_pc_base_cond_taken") {
  checkVstrdPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-vstrd_pc_base_cond_not_taken") {
  checkVstrdPcCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-vstrs_pc_base_cond_taken") {
  checkVstrsPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-vstrs_pc_base_cond_not_taken") {
  checkVstrsPcCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-vstrh_pc_base_cond_taken") {
  checkVstrhPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-vstrh_pc_base_cond_not_taken") {
  checkVstrhPcCond(*this, false);
}
