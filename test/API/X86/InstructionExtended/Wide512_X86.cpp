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

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::checkFeature;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-AESDEC256KL") {
  if (!checkFeature("kl")) {
    return;
  }
  const char source[] = "aesdec256kl 0x20(%ebx,%esi,4), %xmm0\n";
  alignas(16) uint8_t buffer[128] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("AESDEC256KL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("AESDEC256KL", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-AESENC256KL") {
  if (!checkFeature("kl")) {
    return;
  }
  const char source[] = "aesenc256kl 0x20(%ebx,%esi,4), %xmm0\n";
  alignas(16) uint8_t buffer[128] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("AESENC256KL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("AESENC256KL", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-AESDECWIDE256KL") {
  if (!checkFeature("widekl")) {
    return;
  }
  const char source[] = "aesdecwide256kl 0x20(%ebx,%esi,4)\n";
  alignas(16) uint8_t buffer[128] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("AESDECWIDE256KL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("AESDECWIDE256KL", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-AESENCWIDE256KL") {
  if (!checkFeature("widekl")) {
    return;
  }
  const char source[] = "aesencwide256kl 0x20(%ebx,%esi,4)\n";
  alignas(16) uint8_t buffer[128] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("AESENCWIDE256KL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("AESENCWIDE256KL", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-VAESDECZrm") {
  if (!checkFeature("avx512f") || !checkFeature("vaes")) {
    return;
  }
  const char source[] = "vaesdec 0x20(%ebx,%esi,4), %zmm2, %zmm0\n";
  alignas(64) uint8_t buffer[128] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VAESDECZrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VAESDECZrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-VAESDECLASTZrm") {
  if (!checkFeature("avx512f") || !checkFeature("vaes")) {
    return;
  }
  const char source[] = "vaesdeclast 0x20(%ebx,%esi,4), %zmm2, %zmm0\n";
  alignas(64) uint8_t buffer[128] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VAESDECLASTZrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VAESDECLASTZrm", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-VAESENCZrm") {
  if (!checkFeature("avx512f") || !checkFeature("vaes")) {
    return;
  }
  const char source[] = "vaesenc 0x20(%ebx,%esi,4), %zmm2, %zmm0\n";
  alignas(64) uint8_t buffer[128] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VAESENCZrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VAESENCZrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-VAESENCLASTZrm") {
  if (!checkFeature("avx512f") || !checkFeature("vaes")) {
    return;
  }
  const char source[] = "vaesenclast 0x20(%ebx,%esi,4), %zmm2, %zmm0\n";
  alignas(64) uint8_t buffer[128] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VAESENCLASTZrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VAESENCLASTZrm", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-VP2INTERSECTDZrm") {
  if (!checkFeature("avx512vp2intersect")) {
    return;
  }
  const char source[] = "vp2intersectd 0x20(%ebx,%esi,4), %zmm1, %k0\n";
  alignas(64) uint8_t buffer[128] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VP2INTERSECTDZrm", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VP2INTERSECTDZrm", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-VP2INTERSECTQZrm") {
  if (!checkFeature("avx512vp2intersect")) {
    return;
  }
  const char source[] = "vp2intersectq 0x20(%ebx,%esi,4), %zmm1, %k0\n";
  alignas(64) uint8_t buffer[128] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VP2INTERSECTQZrm", QBDI::PREINST, checkAccess,
                   &expectedPre);
  vm.addMnemonicCB("VP2INTERSECTQZrm", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-VPCLMULQDQZrmi") {
  if (!checkFeature("avx512f") || !checkFeature("vpclmulqdq")) {
    return;
  }
  const char source[] = "vpclmulqdq $1, 0x20(%ebx,%esi,4), %zmm1, %zmm0\n";
  alignas(64) uint8_t buffer[128] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VPCLMULQDQZrmi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VPCLMULQDQZrmi", QBDI::POSTINST, checkAccess,
                   &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-MOVDIR64B32") {
  if (!checkFeature("movdir64b")) {
    return;
  }
  const char source[] = "movdir64b 0x20(%ebx,%esi,4), %eax\n";
  uint8_t src[112] = {0};
  alignas(64) uint8_t dst[128];
  for (size_t i = 0; i < sizeof(dst); i++) {
    dst[i] = 0xcc;
  }
  QBDI::rword srcAddr = (QBDI::rword)&src[48];
  QBDI::rword dstAddr = (QBDI::rword)&dst[0];
  ExpectedMemoryAccesses expectedPre = {{
      {srcAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {srcAddr, 0, 64, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
      {dstAddr, 0, 64, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOVDIR64B32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MOVDIR64B32", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->eax = dstAddr;
  state->ebx = (QBDI::rword)&src[0];
  state->esi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  for (size_t i = 64; i < sizeof(dst); i++) {
    CHECK(dst[i] == 0xcc);
  }
}
