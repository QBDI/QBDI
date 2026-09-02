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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1rv2s_post") {
  const char source[] = "ld1r {v0.2s}, [x0], #4\n";

  constexpr uint32_t expected = 0x51005678;
  uint32_t buf[1] = {expected};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], expected, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], expected, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Rv2s_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Rv2s_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Rv2s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v0 == expected);
                     CHECK((uint32_t)(fprState->v0 >> 32) == expected);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 4);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1rv4s") {
  const char source[] = "ld1r {v0.4s}, [x0]\n";

  constexpr uint32_t expected = 0x51009abc;
  uint32_t buf[1] = {expected};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], expected, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], expected, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Rv4s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Rv4s", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Rv4s", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v0 == expected);
                     CHECK((uint32_t)(fprState->v0 >> 32) == expected);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1rv4s_post") {
  const char source[] = "ld1r {v0.4s}, [x0], #4\n";

  constexpr uint32_t expected = 0x5100cdef;
  uint32_t buf[1] = {expected};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], expected, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], expected, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Rv4s_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Rv4s_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Rv4s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v0 == expected);
                     CHECK((uint32_t)(fprState->v0 >> 32) == expected);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 4);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2rv2s") {
  const char source[] = "ld2r {v0.2s, v1.2s}, [x0]\n";

  constexpr uint32_t val0 = 0x11112222;
  constexpr uint32_t val1 = 0x33334444;
  uint32_t buf[2] = {val0, val1};
  constexpr uint64_t expected = ((uint64_t)val1 << 32) | val0;
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
  vm.addMnemonicCB("LD2Rv2s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2Rv2s", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD2Rv2s", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v0 == val0);
                     CHECK((uint32_t)fprState->v1 == val1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2rv2s_post") {
  const char source[] = "ld2r {v0.2s, v1.2s}, [x0], #8\n";

  constexpr uint32_t val0 = 0x12121212;
  constexpr uint32_t val1 = 0x34343434;
  uint32_t buf[2] = {val0, val1};
  constexpr uint64_t expected = ((uint64_t)val1 << 32) | val0;
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
  vm.addMnemonicCB("LD2Rv2s_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2Rv2s_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD2Rv2s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v0 == val0);
                     CHECK((uint32_t)fprState->v1 == val1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2rv4s") {
  const char source[] = "ld2r {v0.4s, v1.4s}, [x0]\n";

  constexpr uint32_t val0 = 0x21212121;
  constexpr uint32_t val1 = 0x43434343;
  uint32_t buf[2] = {val0, val1};
  constexpr uint64_t expected = ((uint64_t)val1 << 32) | val0;
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
  vm.addMnemonicCB("LD2Rv4s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2Rv4s", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD2Rv4s", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v0 == val0);
                     CHECK((uint32_t)fprState->v1 == val1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2rv4s_post") {
  const char source[] = "ld2r {v0.4s, v1.4s}, [x0], #8\n";

  constexpr uint32_t val0 = 0x56565656;
  constexpr uint32_t val1 = 0x78787878;
  uint32_t buf[2] = {val0, val1};
  constexpr uint64_t expected = ((uint64_t)val1 << 32) | val0;
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
  vm.addMnemonicCB("LD2Rv4s_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2Rv4s_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD2Rv4s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v0 == val0);
                     CHECK((uint32_t)fprState->v1 == val1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3rv2s") {
  const char source[] = "ld3r {v0.2s, v1.2s, v2.2s}, [x0]\n";

  constexpr uint32_t val0 = 0x11111111;
  constexpr uint32_t val1 = 0x22222222;
  constexpr uint32_t val2 = 0x33333333;
  constexpr uint64_t combined01 = ((uint64_t)val1 << 32) | val0;
  uint32_t buf[3] = {val0, val1, val2};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], val2, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], val2, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3Rv2s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD3Rv2s", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD3Rv2s", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v0 == val0);
                     CHECK((uint32_t)fprState->v1 == val1);
                     CHECK((uint32_t)fprState->v2 == val2);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3rv2s_post") {
  const char source[] = "ld3r {v0.2s, v1.2s, v2.2s}, [x0], #12\n";

  constexpr uint32_t val0 = 0x44444444;
  constexpr uint32_t val1 = 0x55555555;
  constexpr uint32_t val2 = 0x66666666;
  constexpr uint64_t combined01 = ((uint64_t)val1 << 32) | val0;
  uint32_t buf[3] = {val0, val1, val2};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], val2, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], val2, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3Rv2s_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD3Rv2s_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD3Rv2s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v0 == val0);
                     CHECK((uint32_t)fprState->v1 == val1);
                     CHECK((uint32_t)fprState->v2 == val2);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 12);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3rv4s_post") {
  const char source[] = "ld3r {v0.4s, v1.4s, v2.4s}, [x0], #12\n";

  constexpr uint32_t val0 = 0x77777777;
  constexpr uint32_t val1 = 0x88888888;
  constexpr uint32_t val2 = 0x99999999;
  constexpr uint64_t combined01 = ((uint64_t)val1 << 32) | val0;
  uint32_t buf[3] = {val0, val1, val2};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], val2, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], val2, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3Rv4s_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD3Rv4s_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD3Rv4s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v0 == val0);
                     CHECK((uint32_t)fprState->v1 == val1);
                     CHECK((uint32_t)fprState->v2 == val2);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 12);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4rv2s") {
  const char source[] = "ld4r {v0.2s, v1.2s, v2.2s, v3.2s}, [x0]\n";

  constexpr uint32_t val0 = 0x11111111;
  constexpr uint32_t val1 = 0x22222222;
  constexpr uint32_t val2 = 0x33333333;
  constexpr uint32_t val3 = 0x44444444;
  constexpr uint64_t combined01 = ((uint64_t)val1 << 32) | val0;
  constexpr uint64_t combined23 = ((uint64_t)val3 << 32) | val2;
  uint32_t buf[4] = {val0, val1, val2, val3};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], combined23, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], combined23, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4Rv2s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4Rv2s", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD4Rv2s", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v0 == val0);
                     CHECK((uint32_t)fprState->v1 == val1);
                     CHECK((uint32_t)fprState->v2 == val2);
                     CHECK((uint32_t)fprState->v3 == val3);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4rv2s_post") {
  const char source[] = "ld4r {v0.2s, v1.2s, v2.2s, v3.2s}, [x0], #16\n";

  constexpr uint32_t val0 = 0x55555555;
  constexpr uint32_t val1 = 0x66666666;
  constexpr uint32_t val2 = 0x77777777;
  constexpr uint32_t val3 = 0x88888888;
  constexpr uint64_t combined01 = ((uint64_t)val1 << 32) | val0;
  constexpr uint64_t combined23 = ((uint64_t)val3 << 32) | val2;
  uint32_t buf[4] = {val0, val1, val2, val3};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], combined23, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], combined23, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4Rv2s_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4Rv2s_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD4Rv2s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v0 == val0);
                     CHECK((uint32_t)fprState->v1 == val1);
                     CHECK((uint32_t)fprState->v2 == val2);
                     CHECK((uint32_t)fprState->v3 == val3);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 16);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4rv4s") {
  const char source[] = "ld4r {v0.4s, v1.4s, v2.4s, v3.4s}, [x0]\n";

  constexpr uint32_t val0 = 0x99999999;
  constexpr uint32_t val1 = 0xaaaaaaaa;
  constexpr uint32_t val2 = 0xbbbbbbbb;
  constexpr uint32_t val3 = 0xcccccccc;
  constexpr uint64_t combined01 = ((uint64_t)val1 << 32) | val0;
  constexpr uint64_t combined23 = ((uint64_t)val3 << 32) | val2;
  uint32_t buf[4] = {val0, val1, val2, val3};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], combined23, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], combined23, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4Rv4s", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4Rv4s", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD4Rv4s", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v0 == val0);
                     CHECK((uint32_t)fprState->v1 == val1);
                     CHECK((uint32_t)fprState->v2 == val2);
                     CHECK((uint32_t)fprState->v3 == val3);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4rv4s_post") {
  const char source[] = "ld4r {v0.4s, v1.4s, v2.4s, v3.4s}, [x0], #16\n";

  constexpr uint32_t val0 = 0xdddddddd;
  constexpr uint32_t val1 = 0xeeeeeeee;
  constexpr uint32_t val2 = 0xffffffff;
  constexpr uint32_t val3 = 0x11223344;
  constexpr uint64_t combined01 = ((uint64_t)val1 << 32) | val0;
  constexpr uint64_t combined23 = ((uint64_t)val3 << 32) | val2;
  uint32_t buf[4] = {val0, val1, val2, val3};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], combined23, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], combined23, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4Rv4s_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4Rv4s_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD4Rv4s_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v0 == val0);
                     CHECK((uint32_t)fprState->v1 == val1);
                     CHECK((uint32_t)fprState->v2 == val2);
                     CHECK((uint32_t)fprState->v3 == val3);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 16);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1i32") {
  const char source[] = "ld1 {v0.s}[0], [x0]\n";

  constexpr uint32_t val0 = 0x12345678;

  uint32_t buf[1] = {val0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], val0, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], val0, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1i32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1i32", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1i32", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v0 == val0);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1i32_post") {
  const char source[] = "ld1 {v0.s}[0], [x0], #4\n";

  constexpr uint32_t val0 = 0x23456789;

  uint32_t buf[1] = {val0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], val0, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], val0, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1i32_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1i32_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1i32_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v0 == val0);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 4);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2i32") {
  const char source[] = "ld2 {v0.s, v1.s}[0], [x0]\n";

  constexpr uint32_t val0 = 0x11111111;
  constexpr uint32_t val1 = 0x22222222;
  constexpr uint64_t combined01 = ((uint64_t)val1 << 32) | val0;
  uint32_t buf[2] = {val0, val1};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2i32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2i32", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD2i32", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v0 == val0);
                     CHECK((uint32_t)fprState->v1 == val1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2i32_post") {
  const char source[] = "ld2 {v0.s, v1.s}[0], [x0], #8\n";

  constexpr uint32_t val0 = 0x33333333;
  constexpr uint32_t val1 = 0x44444444;
  constexpr uint64_t combined01 = ((uint64_t)val1 << 32) | val0;
  uint32_t buf[2] = {val0, val1};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2i32_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2i32_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD2i32_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v0 == val0);
                     CHECK((uint32_t)fprState->v1 == val1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3i32") {
  const char source[] = "ld3 {v0.s, v1.s, v2.s}[0], [x0]\n";

  constexpr uint32_t val0 = 0x11111111;
  constexpr uint32_t val1 = 0x22222222;
  constexpr uint32_t val2 = 0x33333333;
  constexpr uint64_t combined01 = ((uint64_t)val1 << 32) | val0;
  uint32_t buf[3] = {val0, val1, val2};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], val2, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], val2, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3i32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD3i32", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD3i32", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v0 == val0);
                     CHECK((uint32_t)fprState->v1 == val1);
                     CHECK((uint32_t)fprState->v2 == val2);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3i32_post") {
  const char source[] = "ld3 {v0.s, v1.s, v2.s}[0], [x0], #12\n";

  constexpr uint32_t val0 = 0x44444444;
  constexpr uint32_t val1 = 0x55555555;
  constexpr uint32_t val2 = 0x66666666;
  constexpr uint64_t combined01 = ((uint64_t)val1 << 32) | val0;
  uint32_t buf[3] = {val0, val1, val2};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], val2, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], val2, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3i32_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD3i32_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD3i32_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v0 == val0);
                     CHECK((uint32_t)fprState->v1 == val1);
                     CHECK((uint32_t)fprState->v2 == val2);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 12);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4i32") {
  const char source[] = "ld4 {v0.s, v1.s, v2.s, v3.s}[0], [x0]\n";

  constexpr uint32_t val0 = 0x11111111;
  constexpr uint32_t val1 = 0x22222222;
  constexpr uint32_t val2 = 0x33333333;
  constexpr uint32_t val3 = 0x44444444;
  constexpr uint64_t combined01 = ((uint64_t)val1 << 32) | val0;
  constexpr uint64_t combined23 = ((uint64_t)val3 << 32) | val2;
  uint32_t buf[4] = {val0, val1, val2, val3};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], combined23, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], combined23, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4i32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4i32", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD4i32", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v0 == val0);
                     CHECK((uint32_t)fprState->v1 == val1);
                     CHECK((uint32_t)fprState->v2 == val2);
                     CHECK((uint32_t)fprState->v3 == val3);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4i32_post") {
  const char source[] = "ld4 {v0.s, v1.s, v2.s, v3.s}[0], [x0], #16\n";

  constexpr uint32_t val0 = 0x55555555;
  constexpr uint32_t val1 = 0x66666666;
  constexpr uint32_t val2 = 0x77777777;
  constexpr uint32_t val3 = 0x88888888;
  constexpr uint64_t combined01 = ((uint64_t)val1 << 32) | val0;
  constexpr uint64_t combined23 = ((uint64_t)val3 << 32) | val2;
  uint32_t buf[4] = {val0, val1, val2, val3};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], combined23, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], combined23, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4i32_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4i32_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD4i32_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v0 == val0);
                     CHECK((uint32_t)fprState->v1 == val1);
                     CHECK((uint32_t)fprState->v2 == val2);
                     CHECK((uint32_t)fprState->v3 == val3);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 16);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1i32") {
  const char source[] = "st1 {v0.s}[0], [x0]\n";

  constexpr uint32_t val0 = 0x12345678;

  uint32_t buf[1] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], val0, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1i32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1i32", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = val0;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == val0);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1i32_post") {
  const char source[] = "st1 {v0.s}[0], [x0], #4\n";

  constexpr uint32_t val0 = 0x23456789;

  uint32_t buf[1] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], val0, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1i32_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1i32_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("ST1i32_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 4);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = val0;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(buf[0] == val0);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st2i32") {
  const char source[] = "st2 {v0.s, v1.s}[0], [x0]\n";

  constexpr uint32_t val0 = 0x11111111;
  constexpr uint32_t val1 = 0x22222222;
  constexpr uint64_t combined01 = ((uint64_t)val1 << 32) | val0;
  uint32_t buf[2] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST2i32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST2i32", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = val0;
  fpr->v1 = val1;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == val0);
  CHECK(buf[1] == val1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st2i32_post") {
  const char source[] = "st2 {v0.s, v1.s}[0], [x0], #8\n";

  constexpr uint32_t val0 = 0x33333333;
  constexpr uint32_t val1 = 0x44444444;
  constexpr uint64_t combined01 = ((uint64_t)val1 << 32) | val0;
  uint32_t buf[2] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST2i32_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST2i32_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("ST2i32_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 8);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = val0;
  fpr->v1 = val1;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(buf[0] == val0);
  CHECK(buf[1] == val1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st3i32") {
  const char source[] = "st3 {v0.s, v1.s, v2.s}[0], [x0]\n";

  constexpr uint32_t val0 = 0x11111111;
  constexpr uint32_t val1 = 0x22222222;
  constexpr uint32_t val2 = 0x33333333;
  constexpr uint64_t combined01 = ((uint64_t)val1 << 32) | val0;
  uint32_t buf[3] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], val2, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST3i32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST3i32", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = val0;
  fpr->v1 = val1;
  fpr->v2 = val2;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == val0);
  CHECK(buf[1] == val1);
  CHECK(buf[2] == val2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st3i32_post") {
  const char source[] = "st3 {v0.s, v1.s, v2.s}[0], [x0], #12\n";

  constexpr uint32_t val0 = 0x44444444;
  constexpr uint32_t val1 = 0x55555555;
  constexpr uint32_t val2 = 0x66666666;
  constexpr uint64_t combined01 = ((uint64_t)val1 << 32) | val0;
  uint32_t buf[3] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], val2, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST3i32_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST3i32_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("ST3i32_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 12);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = val0;
  fpr->v1 = val1;
  fpr->v2 = val2;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(buf[0] == val0);
  CHECK(buf[1] == val1);
  CHECK(buf[2] == val2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st4i32") {
  const char source[] = "st4 {v0.s, v1.s, v2.s, v3.s}[0], [x0]\n";

  constexpr uint32_t val0 = 0x11111111;
  constexpr uint32_t val1 = 0x22222222;
  constexpr uint32_t val2 = 0x33333333;
  constexpr uint32_t val3 = 0x44444444;
  constexpr uint64_t combined01 = ((uint64_t)val1 << 32) | val0;
  constexpr uint64_t combined23 = ((uint64_t)val3 << 32) | val2;
  uint32_t buf[4] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], combined23, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST4i32", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST4i32", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = val0;
  fpr->v1 = val1;
  fpr->v2 = val2;
  fpr->v3 = val3;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == val0);
  CHECK(buf[1] == val1);
  CHECK(buf[2] == val2);
  CHECK(buf[3] == val3);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st4i32_post") {
  const char source[] = "st4 {v0.s, v1.s, v2.s, v3.s}[0], [x0], #16\n";

  constexpr uint32_t val0 = 0x55555555;
  constexpr uint32_t val1 = 0x66666666;
  constexpr uint32_t val2 = 0x77777777;
  constexpr uint32_t val3 = 0x88888888;
  constexpr uint64_t combined01 = ((uint64_t)val1 << 32) | val0;
  constexpr uint64_t combined23 = ((uint64_t)val3 << 32) | val2;
  uint32_t buf[4] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], combined01, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[2], combined23, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST4i32_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST4i32_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("ST4i32_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 16);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = val0;
  fpr->v1 = val1;
  fpr->v2 = val2;
  fpr->v3 = val3;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(buf[0] == val0);
  CHECK(buf[1] == val1);
  CHECK(buf[2] == val2);
  CHECK(buf[3] == val3);
}
