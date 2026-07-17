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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrdl") {
  const char source[] =
      "ldr d0, 1f\n"
      "b 2f\n"
      "1:\n"
      ".xword 0x123456789abcdef0\n"
      "2:\n";

  // literal sits right after 2 fixed-4-byte instructions ("ldr d0, 1f" + "b
  // 2f")
  QBDI::rword addr = genASM(source);
  QBDI::rword literalAddr = addr + 2 * 4;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {literalAddr, 0x123456789abcdef0ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {literalAddr, 0x123456789abcdef0ULL, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRDl", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRDl", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRDl", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v0 == 0x123456789abcdef0ULL);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = vm.call(&retval, addr);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrxpost") {
  const char source[] = "ldr x1, [x0], #8\n";

  constexpr uint64_t expected = 0x13579bdf22446688ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], expected, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], expected, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRXpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRXpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRXpost", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == expected);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 8);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrdpost") {
  const char source[] = "ldr d0, [x0], #8\n";

  constexpr uint64_t expected = 0x2468ace013579bdfULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], expected, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], expected, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRDpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRDpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRDpost", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v0 == expected);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 8);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrdpre") {
  const char source[] = "ldr d0, [x0, #8]!\n";

  constexpr uint64_t expected = 0x369cf25811223344ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[8] = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[8], expected, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[8], expected, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRDpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRDpre", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRDpre", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v0 == expected);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 8);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strdui") {
  const char source[] = "str d0, [x0, #16]\n";

  constexpr uint64_t newval = 0x48c159d99887766ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], newval, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRDui", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRDui", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];

  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = newval;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[16] == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldtrx") {
  const char source[] = "ldtr x1, [x0, #10]\n";

  constexpr uint64_t expected = 0x1133557799aabbccULL;
  uint8_t buf[40] = {0};
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
  vm.addMnemonicCB("LDTRXi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDTRXi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDTRXi", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldurxi") {
  const char source[] = "ldur x1, [x0, #10]\n";

  constexpr uint64_t expected = 0x2244668811335577ULL;
  uint8_t buf[40] = {0};
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
  vm.addMnemonicCB("LDURXi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDURXi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDURXi", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldurdi") {
  const char source[] = "ldur d0, [x0, #10]\n";

  constexpr uint64_t expected = 0x99aabbcc11223344ULL;
  uint8_t buf[40] = {0};
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
  vm.addMnemonicCB("LDURDi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDURDi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDURDi", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrdui") {
  const char source[] = "ldr d0, [x0, #16]\n";

  constexpr uint64_t expected = 0x9900112233445566ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[16] = expected;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[16], expected, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], expected, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRDui", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRDui", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRDui", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strxpost") {
  const char source[] = "str x1, [x0], #8\n";

  constexpr uint64_t newval = 0xaabbccdd11223344ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], newval, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRXpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRXpost", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x1 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strxpre") {
  const char source[] = "str x1, [x0, #8]!\n";

  constexpr uint64_t newval = 0xbbccddee22334455ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[8], newval, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRXpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRXpre", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x1 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[8] == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strxui") {
  const char source[] = "str x1, [x0, #16]\n";

  constexpr uint64_t newval = 0xccddeeff33445566ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], newval, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRXui", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRXui", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x1 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[16] == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strdpost") {
  const char source[] = "str d0, [x0], #8\n";

  constexpr uint64_t newval = 0xddeeff0044556677ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], newval, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRDpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRDpost", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];

  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = newval;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[0] == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strdpre") {
  const char source[] = "str d0, [x0, #8]!\n";

  constexpr uint64_t newval = 0xeeff001155667788ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[8], newval, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRDpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRDpre", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];

  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = newval;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[8] == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-sttrxi") {
  const char source[] = "sttr x1, [x0, #10]\n";

  constexpr uint64_t newval = 0xff00112266778899ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[10], newval, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STTRXi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STTRXi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x1 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[10] == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-sturxi") {
  const char source[] = "stur x1, [x0, #10]\n";

  constexpr uint64_t newval = 0x11223377889900ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[10], newval, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STURXi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STURXi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x1 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[10] == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-sturdi") {
  const char source[] = "stur d0, [x0, #10]\n";

  constexpr uint64_t newval = 0x1122334488990011ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[10], newval, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STURDi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STURDi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];

  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = newval;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[10] == newval);
}
