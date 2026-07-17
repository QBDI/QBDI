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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrxrox_lsl_s0") {
  const char source[] = "ldr x1, [x0, x3, lsl #0]\n";

  constexpr uint64_t expected = 0x13579bdf22446688ULL;
  uint8_t buf[24] = {0};
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
  vm.addMnemonicCB("LDRXroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRXroX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRXroX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 8;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrxrox_lsl_s3") {
  const char source[] = "ldr x1, [x0, x3, lsl #3]\n";

  constexpr uint64_t expected = 0x13579bdf22446688ULL;
  uint8_t buf[24] = {0};
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
  vm.addMnemonicCB("LDRXroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRXroX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRXroX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrxrox_sxtx_s0") {
  const char source[] = "ldr x1, [x0, x3, sxtx]\n";

  constexpr uint64_t expected = 0x13579bdf22446688ULL;
  uint8_t buf[24] = {0};
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
  vm.addMnemonicCB("LDRXroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRXroX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRXroX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 8;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrxrox_sxtx_s3") {
  const char source[] = "ldr x1, [x0, x3, sxtx #3]\n";

  constexpr uint64_t expected = 0x13579bdf22446688ULL;
  uint8_t buf[24] = {0};
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
  vm.addMnemonicCB("LDRXroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRXroX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRXroX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrdrow_uxtw_s0") {
  const char source[] = "ldr d1, [x0, w3, uxtw]\n";

  constexpr uint64_t expected = 0x13579bdf22446688ULL;
  uint8_t buf[24] = {0};
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
  vm.addMnemonicCB("LDRDroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRDroW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRDroW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 8;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrdrow_uxtw_s3") {
  const char source[] = "ldr d1, [x0, w3, uxtw #3]\n";

  constexpr uint64_t expected = 0x13579bdf22446688ULL;
  uint8_t buf[24] = {0};
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
  vm.addMnemonicCB("LDRDroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRDroW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRDroW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrdrow_sxtw_s0") {
  const char source[] = "ldr d1, [x0, w3, sxtw]\n";

  constexpr uint64_t expected = 0x13579bdf22446688ULL;
  uint8_t buf[24] = {0};
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
  vm.addMnemonicCB("LDRDroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRDroW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRDroW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 8;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrdrow_sxtw_s3") {
  const char source[] = "ldr d1, [x0, w3, sxtw #3]\n";

  constexpr uint64_t expected = 0x13579bdf22446688ULL;
  uint8_t buf[24] = {0};
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
  vm.addMnemonicCB("LDRDroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRDroW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRDroW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrdrox_lsl_s0") {
  const char source[] = "ldr d1, [x0, x3, lsl #0]\n";

  constexpr uint64_t expected = 0x13579bdf22446688ULL;
  uint8_t buf[24] = {0};
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
  vm.addMnemonicCB("LDRDroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRDroX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRDroX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 8;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrdrox_lsl_s3") {
  const char source[] = "ldr d1, [x0, x3, lsl #3]\n";

  constexpr uint64_t expected = 0x13579bdf22446688ULL;
  uint8_t buf[24] = {0};
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
  vm.addMnemonicCB("LDRDroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRDroX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRDroX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrdrox_sxtx_s0") {
  const char source[] = "ldr d1, [x0, x3, sxtx]\n";

  constexpr uint64_t expected = 0x13579bdf22446688ULL;
  uint8_t buf[24] = {0};
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
  vm.addMnemonicCB("LDRDroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRDroX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRDroX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 8;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrdrox_sxtx_s3") {
  const char source[] = "ldr d1, [x0, x3, sxtx #3]\n";

  constexpr uint64_t expected = 0x13579bdf22446688ULL;
  uint8_t buf[24] = {0};
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
  vm.addMnemonicCB("LDRDroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRDroX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRDroX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strxrow_uxtw_s0") {
  const char source[] = "str x1, [x0, w3, uxtw]\n";

  constexpr uint64_t newval = 0xaabbccdd11223344ULL;
  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[8], newval, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRXroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRXroW", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 8;
  state->x1 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[8] == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strxrow_uxtw_s3") {
  const char source[] = "str x1, [x0, w3, uxtw #3]\n";

  constexpr uint64_t newval = 0xaabbccdd11223344ULL;
  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[8], newval, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRXroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRXroW", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 1;
  state->x1 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[8] == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strxrow_sxtw_s0") {
  const char source[] = "str x1, [x0, w3, sxtw]\n";

  constexpr uint64_t newval = 0xaabbccdd11223344ULL;
  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[8], newval, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRXroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRXroW", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 8;
  state->x1 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[8] == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strxrow_sxtw_s3") {
  const char source[] = "str x1, [x0, w3, sxtw #3]\n";

  constexpr uint64_t newval = 0xaabbccdd11223344ULL;
  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[8], newval, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRXroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRXroW", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 1;
  state->x1 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[8] == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strxrox_lsl_s0") {
  const char source[] = "str x1, [x0, x3, lsl #0]\n";

  constexpr uint64_t newval = 0xaabbccdd11223344ULL;
  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[8], newval, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRXroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRXroX", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 8;
  state->x1 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[8] == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strxrox_lsl_s3") {
  const char source[] = "str x1, [x0, x3, lsl #3]\n";

  constexpr uint64_t newval = 0xaabbccdd11223344ULL;
  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[8], newval, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRXroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRXroX", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 1;
  state->x1 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[8] == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strxrox_sxtx_s0") {
  const char source[] = "str x1, [x0, x3, sxtx]\n";

  constexpr uint64_t newval = 0xaabbccdd11223344ULL;
  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[8], newval, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRXroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRXroX", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 8;
  state->x1 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[8] == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strxrox_sxtx_s3") {
  const char source[] = "str x1, [x0, x3, sxtx #3]\n";

  constexpr uint64_t newval = 0xaabbccdd11223344ULL;
  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[8], newval, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRXroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRXroX", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 1;
  state->x1 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[8] == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strdrow_uxtw_s0") {
  const char source[] = "str d1, [x0, w3, uxtw]\n";

  constexpr uint64_t newval = 0xaabbccdd11223344ULL;
  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[8], newval, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRDroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRDroW", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 8;

  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = newval;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[8] == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strdrow_uxtw_s3") {
  const char source[] = "str d1, [x0, w3, uxtw #3]\n";

  constexpr uint64_t newval = 0xaabbccdd11223344ULL;
  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[8], newval, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRDroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRDroW", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 1;

  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = newval;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[8] == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strdrow_sxtw_s0") {
  const char source[] = "str d1, [x0, w3, sxtw]\n";

  constexpr uint64_t newval = 0xaabbccdd11223344ULL;
  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[8], newval, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRDroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRDroW", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 8;

  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = newval;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[8] == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strdrow_sxtw_s3") {
  const char source[] = "str d1, [x0, w3, sxtw #3]\n";

  constexpr uint64_t newval = 0xaabbccdd11223344ULL;
  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[8], newval, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRDroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRDroW", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 1;

  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = newval;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[8] == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strdrox_lsl_s0") {
  const char source[] = "str d1, [x0, x3, lsl #0]\n";

  constexpr uint64_t newval = 0xaabbccdd11223344ULL;
  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[8], newval, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRDroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRDroX", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 8;

  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = newval;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[8] == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strdrox_lsl_s3") {
  const char source[] = "str d1, [x0, x3, lsl #3]\n";

  constexpr uint64_t newval = 0xaabbccdd11223344ULL;
  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[8], newval, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRDroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRDroX", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 1;

  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = newval;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[8] == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strdrox_sxtx_s0") {
  const char source[] = "str d1, [x0, x3, sxtx]\n";

  constexpr uint64_t newval = 0xaabbccdd11223344ULL;
  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[8], newval, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRDroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRDroX", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 8;

  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = newval;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[8] == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strdrox_sxtx_s3") {
  const char source[] = "str d1, [x0, x3, sxtx #3]\n";

  constexpr uint64_t newval = 0xaabbccdd11223344ULL;
  uint8_t buf[24] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[8], newval, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRDroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRDroX", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x3 = 1;

  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = newval;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[8] == newval);
}
