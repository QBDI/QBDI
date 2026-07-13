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

#include "MemAccessTestUtils_X86.h"

using QBDITestBatch2::avx512OpmaskSaveRestoreUnsupported;
using QBDITestBatch2::checkAccess;
using QBDITestBatch2::checkFeature;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-CRC32r32m16") {
  if (!checkFeature("sse4.2")) {
    return;
  }
  const char source[] = "crc32w 0x11(%ebx,%esi,4), %eax\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x3535;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x3535, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x3535, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("CRC32r32m16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("CRC32r32m16", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x3535);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-KMOVWkm") {
  if (!checkFeature("avx512f")) {
    return;
  }
  const char source[] = "kmovw 0x11(%ebx,%esi,4), %k1\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x2aaa;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x2aaa, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x2aaa, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("KMOVWkm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("KMOVWkm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-KMOVWmk") {
  if (!checkFeature("avx512f")) {
    return;
  }
  if (avx512OpmaskSaveRestoreUnsupported()) {
    return;
  }
  const char source[] =
      "kmovw %eax, %k1\n"
      "kmovw %k1, 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x2bbb, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("KMOVWmk", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("KMOVWmk", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x2bbb;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x2bbb);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-MMX_PINSRWrmi") {
  if (!checkFeature("mmx")) {
    return;
  }
  if (!checkFeature("sse")) {
    return;
  }
  const char source[] = "pinsrw $0, 0x11(%ebx,%esi,4), %mm0\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x3232;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x3232, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x3232, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MMX_PINSRWrmi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MMX_PINSRWrmi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-PEXTRWmri") {
  if (!checkFeature("sse4.1")) {
    return;
  }
  const char source[] =
      "movd %eax, %xmm0\n"
      "pextrw $0, %xmm0, 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x2d2d, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PEXTRWmri", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PEXTRWmri", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x2d2d;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x2d2d);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-PINSRWrmi") {
  if (!checkFeature("sse2")) {
    return;
  }
  const char source[] = "pinsrw $0, 0x11(%ebx,%esi,4), %xmm0\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x2c2c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x2c2c, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x2c2c, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PINSRWrmi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PINSRWrmi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-PMOVSXBQrm") {
  if (!checkFeature("sse4.1")) {
    return;
  }
  const char source[] = "pmovsxbq 0x11(%ebx,%esi,4), %xmm0\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x3333;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x3333, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x3333, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PMOVSXBQrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PMOVSXBQrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-PMOVZXBQrm") {
  if (!checkFeature("sse4.1")) {
    return;
  }
  const char source[] = "pmovzxbq 0x11(%ebx,%esi,4), %xmm0\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x3333;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x3333, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x3333, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PMOVZXBQrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PMOVZXBQrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-VPBROADCASTWrm") {
  if (!checkFeature("avx2")) {
    return;
  }
  const char source[] = "vpbroadcastw 0x11(%ebx,%esi,4), %xmm0\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x3131;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x3131, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x3131, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VPBROADCASTWrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VPBROADCASTWrm", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-VPBROADCASTWYrm") {
  if (!checkFeature("avx2")) {
    return;
  }
  const char source[] = "vpbroadcastw 0x11(%ebx,%esi,4), %ymm0\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x3131;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x3131, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x3131, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VPBROADCASTWYrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VPBROADCASTWYrm", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-VPEXTRWmri") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] =
      "vmovd %eax, %xmm0\n"
      "vpextrw $0, %xmm0, 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x2f2f, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VPEXTRWmri", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VPEXTRWmri", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x2f2f;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x2f2f);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-VPINSRWrmi") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vpinsrw $0, 0x11(%ebx,%esi,4), %xmm1, %xmm0\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x2e2e;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x2e2e, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x2e2e, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VPINSRWrmi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VPINSRWrmi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-VPMOVSXBQrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vpmovsxbq 0x11(%ebx,%esi,4), %xmm0\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x3434;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x3434, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x3434, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VPMOVSXBQrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VPMOVSXBQrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-VPMOVZXBQrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vpmovzxbq 0x11(%ebx,%esi,4), %xmm0\n";
  uint8_t buffer[40] = {0};
  uint16_t *target = reinterpret_cast<uint16_t *>(&buffer[21]);
  *target = 0x3434;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x3434, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x3434, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VPMOVZXBQrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VPMOVZXBQrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}
