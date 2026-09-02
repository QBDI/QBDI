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

#include <cstring>
#include <vector>

#include "MemAccessTestUtils_ARM.h"

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::checkFeature;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

static QBDI::rword copySnippetToWritableBuffer(QBDI::VM &vm,
                                               QBDI::rword snippetAddr,
                                               std::vector<uint8_t> &buf) {
  memcpy(buf.data(), (void *)snippetAddr, buf.size());
  QBDI::rword bufAddr = (QBDI::rword)buf.data();
  vm.addInstrumentedRange(bufAddr, bufAddr + buf.size());
  vm.clearCache(bufAddr, bufAddr + buf.size());
  return bufAddr;
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strbi12_pc_base") {
  const char source[] =
      "strb r1, [pc, #4]\n"
      "bx lr\n"
      ".word 0\n"
      ".word 0\n";
  QBDI::rword snippetAddr = genASM(source);
  std::vector<uint8_t> codeBuf(32, 0);
  QBDI::rword bufAddr = copySnippetToWritableBuffer(vm, snippetAddr, codeBuf);

  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost = {{
      {bufAddr + 12, 0xab, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STRBi12", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRBi12", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r1 = 0xab;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = vm.call(&retval, bufAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(codeBuf[12] == 0xab);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-stri12_pc_base") {
  const char source[] =
      "str r1, [pc, #4]\n"
      "bx lr\n"
      ".word 0\n"
      ".word 0\n";
  QBDI::rword snippetAddr = genASM(source);
  std::vector<uint8_t> codeBuf(32, 0);
  QBDI::rword bufAddr = copySnippetToWritableBuffer(vm, snippetAddr, codeBuf);

  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost = {{
      {bufAddr + 12, 0xabcdef01, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STRi12", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRi12", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r1 = 0xabcdef01;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = vm.call(&retval, bufAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&codeBuf[12] == 0xabcdef01);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strh_pc_base") {
  const char source[] =
      "strh r1, [pc, #4]\n"
      "bx lr\n"
      ".word 0\n"
      ".word 0\n";
  QBDI::rword snippetAddr = genASM(source);
  std::vector<uint8_t> codeBuf(32, 0);
  QBDI::rword bufAddr = copySnippetToWritableBuffer(vm, snippetAddr, codeBuf);

  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost = {{
      {bufAddr + 12, 0xabcd, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STRH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRH", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r1 = 0xabcd;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = vm.call(&retval, bufAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint16_t *)&codeBuf[12] == 0xabcd);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vstmdia_pc_base") {
  if (!checkFeature("vfp2")) {
    return;
  }
  const char source[] =
      "vstmia pc, {d0}\n"
      "bx lr\n"
      ".word 0\n"
      ".word 0\n";
  QBDI::rword snippetAddr = genASM(source);
  std::vector<uint8_t> codeBuf(32, 0);
  QBDI::rword bufAddr = copySnippetToWritableBuffer(vm, snippetAddr, codeBuf);

  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost = {{
      {bufAddr + 8, 0x44332211, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {bufAddr + 12, 0x88776655, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VSTMDIA", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VSTMDIA", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->vreg.q[0][0] = 0x11;
  fpr->vreg.q[0][1] = 0x22;
  fpr->vreg.q[0][2] = 0x33;
  fpr->vreg.q[0][3] = 0x44;
  fpr->vreg.q[0][4] = 0x55;
  fpr->vreg.q[0][5] = 0x66;
  fpr->vreg.q[0][6] = 0x77;
  fpr->vreg.q[0][7] = 0x88;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = vm.call(&retval, bufAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&codeBuf[8] == 0x44332211);
  CHECK(*(uint32_t *)&codeBuf[12] == 0x88776655);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-fstmxia_pc_base") {
  if (!checkFeature("vfp2")) {
    return;
  }
  const char source[] =
      "fstmiax pc, {d0}\n"
      "bx lr\n"
      ".word 0\n"
      ".word 0\n";
  QBDI::rword snippetAddr = genASM(source);
  std::vector<uint8_t> codeBuf(32, 0);
  QBDI::rword bufAddr = copySnippetToWritableBuffer(vm, snippetAddr, codeBuf);

  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost = {{
      {bufAddr + 8, 0x44332211, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {bufAddr + 12, 0x88776655, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("FSTMXIA", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("FSTMXIA", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->vreg.q[0][0] = 0x11;
  fpr->vreg.q[0][1] = 0x22;
  fpr->vreg.q[0][2] = 0x33;
  fpr->vreg.q[0][3] = 0x44;
  fpr->vreg.q[0][4] = 0x55;
  fpr->vreg.q[0][5] = 0x66;
  fpr->vreg.q[0][6] = 0x77;
  fpr->vreg.q[0][7] = 0x88;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = vm.call(&retval, bufAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&codeBuf[8] == 0x44332211);
  CHECK(*(uint32_t *)&codeBuf[12] == 0x88776655);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vstmsia_pc_base") {
  if (!checkFeature("vfp2")) {
    return;
  }
  const char source[] =
      "vstmia pc, {s0}\n"
      "bx lr\n"
      ".word 0\n";
  QBDI::rword snippetAddr = genASM(source);
  std::vector<uint8_t> codeBuf(32, 0);
  QBDI::rword bufAddr = copySnippetToWritableBuffer(vm, snippetAddr, codeBuf);

  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost = {{
      {bufAddr + 8, 0x44332211, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VSTMSIA", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VSTMSIA", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->vreg.q[0][0] = 0x11;
  fpr->vreg.q[0][1] = 0x22;
  fpr->vreg.q[0][2] = 0x33;
  fpr->vreg.q[0][3] = 0x44;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = vm.call(&retval, bufAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&codeBuf[8] == 0x44332211);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vstrd_pc_base") {
  if (!checkFeature("vfp2")) {
    return;
  }
  const char source[] =
      "vstr d0, [pc, #8]\n"
      "bx lr\n"
      ".word 0\n"
      ".word 0\n"
      ".word 0\n"
      ".word 0\n";
  QBDI::rword snippetAddr = genASM(source);
  std::vector<uint8_t> codeBuf(32, 0);
  QBDI::rword bufAddr = copySnippetToWritableBuffer(vm, snippetAddr, codeBuf);

  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost = {{
      {bufAddr + 16, 0x44332211, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {bufAddr + 20, 0x88776655, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VSTRD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VSTRD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->vreg.q[0][0] = 0x11;
  fpr->vreg.q[0][1] = 0x22;
  fpr->vreg.q[0][2] = 0x33;
  fpr->vreg.q[0][3] = 0x44;
  fpr->vreg.q[0][4] = 0x55;
  fpr->vreg.q[0][5] = 0x66;
  fpr->vreg.q[0][6] = 0x77;
  fpr->vreg.q[0][7] = 0x88;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = vm.call(&retval, bufAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&codeBuf[16] == 0x44332211);
  CHECK(*(uint32_t *)&codeBuf[20] == 0x88776655);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vstrs_pc_base") {
  if (!checkFeature("vfp2")) {
    return;
  }
  const char source[] =
      "vstr s0, [pc, #8]\n"
      "bx lr\n"
      ".word 0\n"
      ".word 0\n"
      ".word 0\n";
  QBDI::rword snippetAddr = genASM(source);
  std::vector<uint8_t> codeBuf(32, 0);
  QBDI::rword bufAddr = copySnippetToWritableBuffer(vm, snippetAddr, codeBuf);

  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost = {{
      {bufAddr + 16, 0x44332211, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VSTRS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VSTRS", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->vreg.q[0][0] = 0x11;
  fpr->vreg.q[0][1] = 0x22;
  fpr->vreg.q[0][2] = 0x33;
  fpr->vreg.q[0][3] = 0x44;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = vm.call(&retval, bufAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&codeBuf[16] == 0x44332211);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vstrh_pc_base") {
  if (!checkFeature("fullfp16")) {
    return;
  }
  const char source[] =
      "vstr.16 s0, [pc, #8]\n"
      "bx lr\n"
      ".word 0\n"
      ".word 0\n"
      ".word 0\n";
  QBDI::rword snippetAddr = genASM(source, QBDI::CPUMode::ARM, {"fullfp16"});
  std::vector<uint8_t> codeBuf(32, 0);
  QBDI::rword bufAddr = copySnippetToWritableBuffer(vm, snippetAddr, codeBuf);

  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost = {{
      {bufAddr + 16, 0x1234, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VSTRH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VSTRH", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->vreg.q[0][0] = 0x34;
  fpr->vreg.q[0][1] = 0x12;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = vm.call(&retval, bufAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint16_t *)&codeBuf[16] == 0x1234);
}
