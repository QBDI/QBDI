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

#include "MemAccessTestUtils_X86_64.h"

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::checkFeature;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PACKSSDWrm") {
  if (!checkFeature("sse2")) {
    return;
  }
  const char source[] = "packssdw 0x20(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[80] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PACKSSDWrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PACKSSDWrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PACKSSWBrm") {
  if (!checkFeature("sse2")) {
    return;
  }
  const char source[] = "packsswb 0x20(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[80] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PACKSSWBrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PACKSSWBrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PACKUSDWrm") {
  if (!checkFeature("sse4.1")) {
    return;
  }
  const char source[] = "packusdw 0x20(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[80] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PACKUSDWrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PACKUSDWrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PACKUSWBrm") {
  if (!checkFeature("sse2")) {
    return;
  }
  const char source[] = "packuswb 0x20(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[80] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PACKUSWBrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PACKUSWBrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PALIGNRrmi") {
  if (!checkFeature("ssse3")) {
    return;
  }
  const char source[] = "palignr $0x1, 0x20(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[80] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PALIGNRrmi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PALIGNRrmi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PSHUFBrm") {
  if (!checkFeature("ssse3")) {
    return;
  }
  const char source[] = "pshufb 0x20(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[80] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PSHUFBrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PSHUFBrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PSHUFDmi") {
  if (!checkFeature("sse2")) {
    return;
  }
  const char source[] = "pshufd $0x1, 0x20(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[80] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PSHUFDmi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PSHUFDmi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PSHUFHWmi") {
  if (!checkFeature("sse2")) {
    return;
  }
  const char source[] = "pshufhw $0x1, 0x20(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[80] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PSHUFHWmi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PSHUFHWmi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PSHUFLWmi") {
  if (!checkFeature("sse2")) {
    return;
  }
  const char source[] = "pshuflw $0x1, 0x20(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[80] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PSHUFLWmi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PSHUFLWmi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PUNPCKHBWrm") {
  if (!checkFeature("sse2")) {
    return;
  }
  const char source[] = "punpckhbw 0x20(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[80] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PUNPCKHBWrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PUNPCKHBWrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PUNPCKHDQrm") {
  if (!checkFeature("sse2")) {
    return;
  }
  const char source[] = "punpckhdq 0x20(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[80] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PUNPCKHDQrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PUNPCKHDQrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PUNPCKHQDQrm") {
  if (!checkFeature("sse2")) {
    return;
  }
  const char source[] = "punpckhqdq 0x20(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[80] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PUNPCKHQDQrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PUNPCKHQDQrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PUNPCKHWDrm") {
  if (!checkFeature("sse2")) {
    return;
  }
  const char source[] = "punpckhwd 0x20(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[80] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PUNPCKHWDrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PUNPCKHWDrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PUNPCKLBWrm") {
  if (!checkFeature("sse2")) {
    return;
  }
  const char source[] = "punpcklbw 0x20(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[80] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PUNPCKLBWrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PUNPCKLBWrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PUNPCKLDQrm") {
  if (!checkFeature("sse2")) {
    return;
  }
  const char source[] = "punpckldq 0x20(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[80] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PUNPCKLDQrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PUNPCKLDQrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PUNPCKLQDQrm") {
  if (!checkFeature("sse2")) {
    return;
  }
  const char source[] = "punpcklqdq 0x20(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[80] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PUNPCKLQDQrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PUNPCKLQDQrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PUNPCKLWDrm") {
  if (!checkFeature("sse2")) {
    return;
  }
  const char source[] = "punpcklwd 0x20(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[80] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PUNPCKLWDrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PUNPCKLWDrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHUFPDrmi") {
  if (!checkFeature("sse2")) {
    return;
  }
  const char source[] = "shufpd $0x1, 0x20(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[80] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHUFPDrmi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHUFPDrmi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-SHUFPSrmi") {
  const char source[] = "shufps $0x1, 0x20(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[80] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHUFPSrmi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHUFPSrmi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-UNPCKHPDrm") {
  if (!checkFeature("sse2")) {
    return;
  }
  const char source[] = "unpckhpd 0x20(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[80] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("UNPCKHPDrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("UNPCKHPDrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-UNPCKHPSrm") {
  const char source[] = "unpckhps 0x20(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[80] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("UNPCKHPSrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("UNPCKHPSrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-UNPCKLPDrm") {
  if (!checkFeature("sse2")) {
    return;
  }
  const char source[] = "unpcklpd 0x20(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[80] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("UNPCKLPDrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("UNPCKLPDrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-UNPCKLPSrm") {
  const char source[] = "unpcklps 0x20(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[80] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("UNPCKLPSrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("UNPCKLPSrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-MPSADBWrmi") {
  if (!checkFeature("sse4.1")) {
    return;
  }
  const char source[] = "mpsadbw $0x1, 0x20(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[80] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MPSADBWrmi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MPSADBWrmi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-PBLENDWrmi") {
  if (!checkFeature("sse4.1")) {
    return;
  }
  const char source[] = "pblendw $0x1, 0x20(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[80] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PBLENDWrmi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PBLENDWrmi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-DPPDrmi") {
  if (!checkFeature("sse4.1")) {
    return;
  }
  const char source[] = "dppd $0x1, 0x20(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[80] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("DPPDrmi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("DPPDrmi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-DPPSrmi") {
  if (!checkFeature("sse4.1")) {
    return;
  }
  const char source[] = "dpps $0x1, 0x20(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[80] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("DPPSrmi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("DPPSrmi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROUNDPDmi") {
  if (!checkFeature("sse4.1")) {
    return;
  }
  const char source[] = "roundpd $0x1, 0x20(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[80] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ROUNDPDmi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROUNDPDmi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-ROUNDPSmi") {
  if (!checkFeature("sse4.1")) {
    return;
  }
  const char source[] = "roundps $0x1, 0x20(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[80] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[48];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ROUNDPSmi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ROUNDPSmi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 4;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VPACKSSDWrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vpackssdw 0x11(%rbx,%rsi,4), %xmm1, %xmm0\n";
  alignas(16) uint8_t buffer[48] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VPACKSSDWrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VPACKSSDWrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VPACKSSWBrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vpacksswb 0x11(%rbx,%rsi,4), %xmm1, %xmm0\n";
  alignas(16) uint8_t buffer[48] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VPACKSSWBrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VPACKSSWBrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VPACKUSDWrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vpackusdw 0x11(%rbx,%rsi,4), %xmm1, %xmm0\n";
  alignas(16) uint8_t buffer[48] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VPACKUSDWrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VPACKUSDWrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VPACKUSWBrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vpackuswb 0x11(%rbx,%rsi,4), %xmm1, %xmm0\n";
  alignas(16) uint8_t buffer[48] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VPACKUSWBrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VPACKUSWBrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VPALIGNRrmi") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vpalignr $0x1, 0x11(%rbx,%rsi,4), %xmm1, %xmm0\n";
  alignas(16) uint8_t buffer[48] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VPALIGNRrmi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VPALIGNRrmi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VPSHUFBrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vpshufb 0x11(%rbx,%rsi,4), %xmm1, %xmm0\n";
  alignas(16) uint8_t buffer[48] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VPSHUFBrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VPSHUFBrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VPSHUFDmi") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vpshufd $0x1, 0x11(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[48] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VPSHUFDmi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VPSHUFDmi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VPSHUFHWmi") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vpshufhw $0x1, 0x11(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[48] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VPSHUFHWmi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VPSHUFHWmi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VPSHUFLWmi") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vpshuflw $0x1, 0x11(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[48] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VPSHUFLWmi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VPSHUFLWmi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VPUNPCKHBWrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vpunpckhbw 0x11(%rbx,%rsi,4), %xmm1, %xmm0\n";
  alignas(16) uint8_t buffer[48] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VPUNPCKHBWrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VPUNPCKHBWrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VPUNPCKHDQrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vpunpckhdq 0x11(%rbx,%rsi,4), %xmm1, %xmm0\n";
  alignas(16) uint8_t buffer[48] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VPUNPCKHDQrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VPUNPCKHDQrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VPUNPCKHQDQrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vpunpckhqdq 0x11(%rbx,%rsi,4), %xmm1, %xmm0\n";
  alignas(16) uint8_t buffer[48] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VPUNPCKHQDQrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VPUNPCKHQDQrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VPUNPCKHWDrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vpunpckhwd 0x11(%rbx,%rsi,4), %xmm1, %xmm0\n";
  alignas(16) uint8_t buffer[48] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VPUNPCKHWDrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VPUNPCKHWDrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VPUNPCKLBWrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vpunpcklbw 0x11(%rbx,%rsi,4), %xmm1, %xmm0\n";
  alignas(16) uint8_t buffer[48] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VPUNPCKLBWrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VPUNPCKLBWrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VPUNPCKLDQrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vpunpckldq 0x11(%rbx,%rsi,4), %xmm1, %xmm0\n";
  alignas(16) uint8_t buffer[48] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VPUNPCKLDQrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VPUNPCKLDQrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VPUNPCKLQDQrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vpunpcklqdq 0x11(%rbx,%rsi,4), %xmm1, %xmm0\n";
  alignas(16) uint8_t buffer[48] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VPUNPCKLQDQrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VPUNPCKLQDQrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VPUNPCKLWDrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vpunpcklwd 0x11(%rbx,%rsi,4), %xmm1, %xmm0\n";
  alignas(16) uint8_t buffer[48] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VPUNPCKLWDrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VPUNPCKLWDrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VSHUFPDrmi") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vshufpd $0x1, 0x11(%rbx,%rsi,4), %xmm1, %xmm0\n";
  alignas(16) uint8_t buffer[48] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VSHUFPDrmi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VSHUFPDrmi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VSHUFPSrmi") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vshufps $0x1, 0x11(%rbx,%rsi,4), %xmm1, %xmm0\n";
  alignas(16) uint8_t buffer[48] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VSHUFPSrmi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VSHUFPSrmi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VUNPCKHPDrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vunpckhpd 0x11(%rbx,%rsi,4), %xmm1, %xmm0\n";
  alignas(16) uint8_t buffer[48] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VUNPCKHPDrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VUNPCKHPDrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VUNPCKHPSrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vunpckhps 0x11(%rbx,%rsi,4), %xmm1, %xmm0\n";
  alignas(16) uint8_t buffer[48] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VUNPCKHPSrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VUNPCKHPSrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VUNPCKLPDrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vunpcklpd 0x11(%rbx,%rsi,4), %xmm1, %xmm0\n";
  alignas(16) uint8_t buffer[48] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VUNPCKLPDrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VUNPCKLPDrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VUNPCKLPSrm") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vunpcklps 0x11(%rbx,%rsi,4), %xmm1, %xmm0\n";
  alignas(16) uint8_t buffer[48] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VUNPCKLPSrm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VUNPCKLPSrm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VMPSADBWrmi") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vmpsadbw $0x1, 0x11(%rbx,%rsi,4), %xmm1, %xmm0\n";
  alignas(16) uint8_t buffer[48] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VMPSADBWrmi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VMPSADBWrmi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VDPPDrmi") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vdppd $0x1, 0x11(%rbx,%rsi,4), %xmm1, %xmm0\n";
  alignas(16) uint8_t buffer[48] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VDPPDrmi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VDPPDrmi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VDPPSrmi") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vdpps $0x1, 0x11(%rbx,%rsi,4), %xmm1, %xmm0\n";
  alignas(16) uint8_t buffer[48] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VDPPSrmi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VDPPSrmi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VROUNDPDmi") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vroundpd $0x1, 0x11(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[48] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VROUNDPDmi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VROUNDPDmi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-VROUNDPSmi") {
  if (!checkFeature("avx")) {
    return;
  }
  const char source[] = "vroundps $0x1, 0x11(%rbx,%rsi,4), %xmm0\n";
  alignas(16) uint8_t buffer[48] = {0};
  QBDI::rword targetAddr = (QBDI::rword)&buffer[21];
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VROUNDPSmi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VROUNDPSmi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->rbx = (QBDI::rword)&buffer[0];
  state->rsi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}
