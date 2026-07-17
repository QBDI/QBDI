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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrhh_row_uxtw_s0") {
  const char source[] = "ldrh w1, [x0, w3, uxtw]\n";

  constexpr uint16_t expected = 0x7a3b;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x7a3b >> 0) & 0xff),
                   (uint8_t)((0x7a3b >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRHHroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRHHroW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRHHroW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffff) == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrhh_row_uxtw_s1") {
  const char source[] = "ldrh w1, [x0, w3, uxtw #1]\n";

  constexpr uint16_t expected = 0x7a3b;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x7a3b >> 0) & 0xff),
                   (uint8_t)((0x7a3b >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRHHroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRHHroW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRHHroW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffff) == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrhh_row_sxtw_s0") {
  const char source[] = "ldrh w1, [x0, w3, sxtw]\n";

  constexpr uint16_t expected = 0x7a3b;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x7a3b >> 0) & 0xff),
                   (uint8_t)((0x7a3b >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRHHroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRHHroW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRHHroW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffff) == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrhh_row_sxtw_s1") {
  const char source[] = "ldrh w1, [x0, w3, sxtw #1]\n";

  constexpr uint16_t expected = 0x7a3b;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x7a3b >> 0) & 0xff),
                   (uint8_t)((0x7a3b >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRHHroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRHHroW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRHHroW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffff) == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrhh_rox_lsl_s0") {
  const char source[] = "ldrh w1, [x0, x3, lsl #0]\n";

  constexpr uint16_t expected = 0x7a3b;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x7a3b >> 0) & 0xff),
                   (uint8_t)((0x7a3b >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRHHroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRHHroX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRHHroX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffff) == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrhh_rox_lsl_s1") {
  const char source[] = "ldrh w1, [x0, x3, lsl #1]\n";

  constexpr uint16_t expected = 0x7a3b;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x7a3b >> 0) & 0xff),
                   (uint8_t)((0x7a3b >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRHHroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRHHroX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRHHroX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffff) == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrhh_rox_sxtx_s0") {
  const char source[] = "ldrh w1, [x0, x3, sxtx]\n";

  constexpr uint16_t expected = 0x7a3b;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x7a3b >> 0) & 0xff),
                   (uint8_t)((0x7a3b >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRHHroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRHHroX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRHHroX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffff) == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldrhh_rox_sxtx_s1") {
  const char source[] = "ldrh w1, [x0, x3, sxtx #1]\n";

  constexpr uint16_t expected = 0x7a3b;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x7a3b >> 0) & 0xff),
                   (uint8_t)((0x7a3b >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRHHroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRHHroX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRHHroX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffff) == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-ldrsh_w_row_uxtw_s0") {
  const char source[] = "ldrsh w1, [x0, w3, uxtw]\n";

  constexpr uint16_t expected = 0x8091;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x8091 >> 0) & 0xff),
                   (uint8_t)((0x8091 >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRSHWroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRSHWroW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRSHWroW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffffffff) == 0xffff8091);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-ldrsh_w_row_uxtw_s1") {
  const char source[] = "ldrsh w1, [x0, w3, uxtw #1]\n";

  constexpr uint16_t expected = 0x8091;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x8091 >> 0) & 0xff),
                   (uint8_t)((0x8091 >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRSHWroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRSHWroW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRSHWroW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffffffff) == 0xffff8091);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-ldrsh_w_row_sxtw_s0") {
  const char source[] = "ldrsh w1, [x0, w3, sxtw]\n";

  constexpr uint16_t expected = 0x8091;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x8091 >> 0) & 0xff),
                   (uint8_t)((0x8091 >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRSHWroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRSHWroW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRSHWroW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffffffff) == 0xffff8091);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-ldrsh_w_row_sxtw_s1") {
  const char source[] = "ldrsh w1, [x0, w3, sxtw #1]\n";

  constexpr uint16_t expected = 0x8091;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x8091 >> 0) & 0xff),
                   (uint8_t)((0x8091 >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRSHWroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRSHWroW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRSHWroW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffffffff) == 0xffff8091);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-ldrsh_w_rox_lsl_s0") {
  const char source[] = "ldrsh w1, [x0, x3, lsl #0]\n";

  constexpr uint16_t expected = 0x8091;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x8091 >> 0) & 0xff),
                   (uint8_t)((0x8091 >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRSHWroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRSHWroX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRSHWroX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffffffff) == 0xffff8091);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-ldrsh_w_rox_lsl_s1") {
  const char source[] = "ldrsh w1, [x0, x3, lsl #1]\n";

  constexpr uint16_t expected = 0x8091;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x8091 >> 0) & 0xff),
                   (uint8_t)((0x8091 >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRSHWroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRSHWroX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRSHWroX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffffffff) == 0xffff8091);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-ldrsh_w_rox_sxtx_s0") {
  const char source[] = "ldrsh w1, [x0, x3, sxtx]\n";

  constexpr uint16_t expected = 0x8091;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x8091 >> 0) & 0xff),
                   (uint8_t)((0x8091 >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRSHWroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRSHWroX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRSHWroX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffffffff) == 0xffff8091);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-ldrsh_w_rox_sxtx_s1") {
  const char source[] = "ldrsh w1, [x0, x3, sxtx #1]\n";

  constexpr uint16_t expected = 0x8091;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x8091 >> 0) & 0xff),
                   (uint8_t)((0x8091 >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRSHWroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRSHWroX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRSHWroX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffffffff) == 0xffff8091);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-ldrsh_x_row_uxtw_s0") {
  const char source[] = "ldrsh x1, [x0, w3, uxtw]\n";

  constexpr uint16_t expected = 0x8092;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x8092 >> 0) & 0xff),
                   (uint8_t)((0x8092 >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRSHXroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRSHXroW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRSHXroW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == 0xffffffffffff8092ULL);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-ldrsh_x_row_uxtw_s1") {
  const char source[] = "ldrsh x1, [x0, w3, uxtw #1]\n";

  constexpr uint16_t expected = 0x8092;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x8092 >> 0) & 0xff),
                   (uint8_t)((0x8092 >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRSHXroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRSHXroW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRSHXroW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == 0xffffffffffff8092ULL);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-ldrsh_x_row_sxtw_s0") {
  const char source[] = "ldrsh x1, [x0, w3, sxtw]\n";

  constexpr uint16_t expected = 0x8092;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x8092 >> 0) & 0xff),
                   (uint8_t)((0x8092 >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRSHXroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRSHXroW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRSHXroW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == 0xffffffffffff8092ULL);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-ldrsh_x_row_sxtw_s1") {
  const char source[] = "ldrsh x1, [x0, w3, sxtw #1]\n";

  constexpr uint16_t expected = 0x8092;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x8092 >> 0) & 0xff),
                   (uint8_t)((0x8092 >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRSHXroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRSHXroW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRSHXroW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == 0xffffffffffff8092ULL);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-ldrsh_x_rox_lsl_s0") {
  const char source[] = "ldrsh x1, [x0, x3, lsl #0]\n";

  constexpr uint16_t expected = 0x8092;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x8092 >> 0) & 0xff),
                   (uint8_t)((0x8092 >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRSHXroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRSHXroX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRSHXroX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == 0xffffffffffff8092ULL);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-ldrsh_x_rox_lsl_s1") {
  const char source[] = "ldrsh x1, [x0, x3, lsl #1]\n";

  constexpr uint16_t expected = 0x8092;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x8092 >> 0) & 0xff),
                   (uint8_t)((0x8092 >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRSHXroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRSHXroX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRSHXroX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == 0xffffffffffff8092ULL);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-ldrsh_x_rox_sxtx_s0") {
  const char source[] = "ldrsh x1, [x0, x3, sxtx]\n";

  constexpr uint16_t expected = 0x8092;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x8092 >> 0) & 0xff),
                   (uint8_t)((0x8092 >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRSHXroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRSHXroX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRSHXroX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == 0xffffffffffff8092ULL);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-ldrsh_x_rox_sxtx_s1") {
  const char source[] = "ldrsh x1, [x0, x3, sxtx #1]\n";

  constexpr uint16_t expected = 0x8092;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x8092 >> 0) & 0xff),
                   (uint8_t)((0x8092 >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRSHXroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRSHXroX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRSHXroX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == 0xffffffffffff8092ULL);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-ldrh_fpr_row_uxtw_s0") {
  const char source[] = "ldr h1, [x0, w3, uxtw]\n";

  constexpr uint16_t expected = 0x9c3d;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x9c3d >> 0) & 0xff),
                   (uint8_t)((0x9c3d >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRHroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRHroW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRHroW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint16_t)fprState->v1 == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-ldrh_fpr_row_uxtw_s1") {
  const char source[] = "ldr h1, [x0, w3, uxtw #1]\n";

  constexpr uint16_t expected = 0x9c3d;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x9c3d >> 0) & 0xff),
                   (uint8_t)((0x9c3d >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRHroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRHroW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRHroW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint16_t)fprState->v1 == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-ldrh_fpr_row_sxtw_s0") {
  const char source[] = "ldr h1, [x0, w3, sxtw]\n";

  constexpr uint16_t expected = 0x9c3d;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x9c3d >> 0) & 0xff),
                   (uint8_t)((0x9c3d >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRHroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRHroW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRHroW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint16_t)fprState->v1 == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-ldrh_fpr_row_sxtw_s1") {
  const char source[] = "ldr h1, [x0, w3, sxtw #1]\n";

  constexpr uint16_t expected = 0x9c3d;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x9c3d >> 0) & 0xff),
                   (uint8_t)((0x9c3d >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRHroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRHroW", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRHroW", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint16_t)fprState->v1 == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-ldrh_fpr_rox_lsl_s0") {
  const char source[] = "ldr h1, [x0, x3, lsl #0]\n";

  constexpr uint16_t expected = 0x9c3d;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x9c3d >> 0) & 0xff),
                   (uint8_t)((0x9c3d >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRHroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRHroX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRHroX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint16_t)fprState->v1 == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-ldrh_fpr_rox_lsl_s1") {
  const char source[] = "ldr h1, [x0, x3, lsl #1]\n";

  constexpr uint16_t expected = 0x9c3d;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x9c3d >> 0) & 0xff),
                   (uint8_t)((0x9c3d >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRHroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRHroX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRHroX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint16_t)fprState->v1 == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-ldrh_fpr_rox_sxtx_s0") {
  const char source[] = "ldr h1, [x0, x3, sxtx]\n";

  constexpr uint16_t expected = 0x9c3d;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x9c3d >> 0) & 0xff),
                   (uint8_t)((0x9c3d >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[4], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRHroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRHroX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRHroX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint16_t)fprState->v1 == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-ldrh_fpr_rox_sxtx_s1") {
  const char source[] = "ldr h1, [x0, x3, sxtx #1]\n";

  constexpr uint16_t expected = 0x9c3d;
  uint8_t v[16] = {0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   0,
                   (uint8_t)((0x9c3d >> 0) & 0xff),
                   (uint8_t)((0x9c3d >> 8) & 0xff),
                   0,
                   0,
                   0,
                   0,
                   0,
                   0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[8], expected, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDRHroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDRHroX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDRHroX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint16_t)fprState->v1 == expected);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strhh_row_uxtw_s0") {
  const char source[] = "strh w1, [x0, w3, uxtw]\n";

  uint8_t v[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[4], 0xb4c5, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRHHroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRHHroW", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  state->x1 = 0xb4c5;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint16_t *)&v[4] == 0xb4c5);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strhh_row_uxtw_s1") {
  const char source[] = "strh w1, [x0, w3, uxtw #1]\n";

  uint8_t v[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[8], 0xb4c5, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRHHroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRHHroW", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  state->x1 = 0xb4c5;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint16_t *)&v[8] == 0xb4c5);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strhh_row_sxtw_s0") {
  const char source[] = "strh w1, [x0, w3, sxtw]\n";

  uint8_t v[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[4], 0xb4c5, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRHHroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRHHroW", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  state->x1 = 0xb4c5;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint16_t *)&v[4] == 0xb4c5);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strhh_row_sxtw_s1") {
  const char source[] = "strh w1, [x0, w3, sxtw #1]\n";

  uint8_t v[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[8], 0xb4c5, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRHHroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRHHroW", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  state->x1 = 0xb4c5;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint16_t *)&v[8] == 0xb4c5);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strhh_rox_lsl_s0") {
  const char source[] = "strh w1, [x0, x3, lsl #0]\n";

  uint8_t v[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[4], 0xb4c5, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRHHroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRHHroX", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  state->x1 = 0xb4c5;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint16_t *)&v[4] == 0xb4c5);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strhh_rox_lsl_s1") {
  const char source[] = "strh w1, [x0, x3, lsl #1]\n";

  uint8_t v[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[8], 0xb4c5, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRHHroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRHHroX", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  state->x1 = 0xb4c5;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint16_t *)&v[8] == 0xb4c5);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strhh_rox_sxtx_s0") {
  const char source[] = "strh w1, [x0, x3, sxtx]\n";

  uint8_t v[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[4], 0xb4c5, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRHHroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRHHroX", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  state->x1 = 0xb4c5;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint16_t *)&v[4] == 0xb4c5);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-strhh_rox_sxtx_s1") {
  const char source[] = "strh w1, [x0, x3, sxtx #1]\n";

  uint8_t v[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[8], 0xb4c5, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRHHroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRHHroX", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  state->x1 = 0xb4c5;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint16_t *)&v[8] == 0xb4c5);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-strh_fpr_row_uxtw_s0") {
  const char source[] = "str h1, [x0, w3, uxtw]\n";

  uint8_t v[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[4], 0xd4e5, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRHroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRHroW", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = 0xd4e5;
  vm.setFPRState(fpr);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint16_t *)&v[4] == 0xd4e5);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-strh_fpr_row_uxtw_s1") {
  const char source[] = "str h1, [x0, w3, uxtw #1]\n";

  uint8_t v[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[8], 0xd4e5, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRHroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRHroW", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = 0xd4e5;
  vm.setFPRState(fpr);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint16_t *)&v[8] == 0xd4e5);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-strh_fpr_row_sxtw_s0") {
  const char source[] = "str h1, [x0, w3, sxtw]\n";

  uint8_t v[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[4], 0xd4e5, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRHroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRHroW", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = 0xd4e5;
  vm.setFPRState(fpr);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint16_t *)&v[4] == 0xd4e5);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-strh_fpr_row_sxtw_s1") {
  const char source[] = "str h1, [x0, w3, sxtw #1]\n";

  uint8_t v[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[8], 0xd4e5, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRHroW", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRHroW", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = 0xd4e5;
  vm.setFPRState(fpr);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint16_t *)&v[8] == 0xd4e5);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-strh_fpr_rox_lsl_s0") {
  const char source[] = "str h1, [x0, x3, lsl #0]\n";

  uint8_t v[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[4], 0xd4e5, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRHroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRHroX", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = 0xd4e5;
  vm.setFPRState(fpr);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint16_t *)&v[4] == 0xd4e5);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-strh_fpr_rox_lsl_s1") {
  const char source[] = "str h1, [x0, x3, lsl #1]\n";

  uint8_t v[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[8], 0xd4e5, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRHroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRHroX", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = 0xd4e5;
  vm.setFPRState(fpr);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint16_t *)&v[8] == 0xd4e5);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-strh_fpr_rox_sxtx_s0") {
  const char source[] = "str h1, [x0, x3, sxtx]\n";

  uint8_t v[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[4], 0xd4e5, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRHroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRHroX", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = 0xd4e5;
  vm.setFPRState(fpr);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint16_t *)&v[4] == 0xd4e5);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_AARCH64-strh_fpr_rox_sxtx_s1") {
  const char source[] = "str h1, [x0, x3, sxtx #1]\n";

  uint8_t v[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[8], 0xd4e5, 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRHroX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRHroX", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x3 = 4;
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = 0xd4e5;
  vm.setFPRState(fpr);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint16_t *)&v[8] == 0xd4e5);
}
