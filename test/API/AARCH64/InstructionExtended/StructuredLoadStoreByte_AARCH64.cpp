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

static const unsigned __int128 ONES =
    ((unsigned __int128)0x0101010101010101ULL << 64) | 0x0101010101010101ULL;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1r_v8b") {
  const char source[] = "ld1r {v0.8b}, [x0]\n";

  uint8_t buf[1] = {(uint8_t)(0x51 + 0)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], (((uint32_t)buf[0] << 0)), 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], (((uint32_t)buf[0] << 0)), 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Rv8b", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Rv8b", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Rv8b", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == (uint64_t)(ONES * buf[0]));
                     CHECK((uint64_t)(fprState->v0 >> 64) == 0);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1r_v8b_post") {
  const char source[] = "ld1r {v0.8b}, [x0], #1\n";

  uint8_t buf[1] = {(uint8_t)(0x51 + 0)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], (((uint32_t)buf[0] << 0)), 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], (((uint32_t)buf[0] << 0)), 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Rv8b_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Rv8b_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Rv8b_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == (uint64_t)(ONES * buf[0]));
                     CHECK((uint64_t)(fprState->v0 >> 64) == 0);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1r_v16b") {
  const char source[] = "ld1r {v0.16b}, [x0]\n";

  uint8_t buf[1] = {(uint8_t)(0x51 + 0)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], (((uint32_t)buf[0] << 0)), 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], (((uint32_t)buf[0] << 0)), 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Rv16b", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Rv16b", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Rv16b", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v0 == ONES * buf[0]);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1r_v16b_post") {
  const char source[] = "ld1r {v0.16b}, [x0], #1\n";

  uint8_t buf[1] = {(uint8_t)(0x51 + 0)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], (((uint32_t)buf[0] << 0)), 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], (((uint32_t)buf[0] << 0)), 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Rv16b_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Rv16b_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Rv16b_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v0 == ONES * buf[0]);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2r_v8b") {
  const char source[] = "ld2r {v0.8b, v1.8b}, [x0]\n";

  uint8_t buf[2] = {(uint8_t)(0x51 + 0), (uint8_t)(0x51 + 1)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8)), 2,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8)), 2,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2Rv8b", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2Rv8b", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD2Rv8b", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == (uint64_t)(ONES * buf[0]));
                     CHECK((uint64_t)(fprState->v0 >> 64) == 0);
                     CHECK((uint64_t)fprState->v1 == (uint64_t)(ONES * buf[1]));
                     CHECK((uint64_t)(fprState->v1 >> 64) == 0);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2r_v8b_post") {
  const char source[] = "ld2r {v0.8b, v1.8b}, [x0], #2\n";

  uint8_t buf[2] = {(uint8_t)(0x51 + 0), (uint8_t)(0x51 + 1)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8)), 2,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8)), 2,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2Rv8b_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2Rv8b_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD2Rv8b_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == (uint64_t)(ONES * buf[0]));
                     CHECK((uint64_t)(fprState->v0 >> 64) == 0);
                     CHECK((uint64_t)fprState->v1 == (uint64_t)(ONES * buf[1]));
                     CHECK((uint64_t)(fprState->v1 >> 64) == 0);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 2);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2r_v16b") {
  const char source[] = "ld2r {v0.16b, v1.16b}, [x0]\n";

  uint8_t buf[2] = {(uint8_t)(0x51 + 0), (uint8_t)(0x51 + 1)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8)), 2,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8)), 2,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2Rv16b", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2Rv16b", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD2Rv16b", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v0 == ONES * buf[0]);
                     CHECK(fprState->v1 == ONES * buf[1]);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2r_v16b_post") {
  const char source[] = "ld2r {v0.16b, v1.16b}, [x0], #2\n";

  uint8_t buf[2] = {(uint8_t)(0x51 + 0), (uint8_t)(0x51 + 1)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8)), 2,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8)), 2,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2Rv16b_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2Rv16b_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD2Rv16b_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v0 == ONES * buf[0]);
                     CHECK(fprState->v1 == ONES * buf[1]);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 2);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3r_v8b") {
  const char source[] = "ld3r {v0.8b, v1.8b, v2.8b}, [x0]\n";

  uint8_t buf[3] = {(uint8_t)(0x51 + 0), (uint8_t)(0x51 + 1),
                    (uint8_t)(0x51 + 2)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8) |
        ((uint32_t)buf[2] << 16)),
       3, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8) |
        ((uint32_t)buf[2] << 16)),
       3, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3Rv8b", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD3Rv8b", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD3Rv8b", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == (uint64_t)(ONES * buf[0]));
                     CHECK((uint64_t)(fprState->v0 >> 64) == 0);
                     CHECK((uint64_t)fprState->v1 == (uint64_t)(ONES * buf[1]));
                     CHECK((uint64_t)(fprState->v1 >> 64) == 0);
                     CHECK((uint64_t)fprState->v2 == (uint64_t)(ONES * buf[2]));
                     CHECK((uint64_t)(fprState->v2 >> 64) == 0);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3r_v8b_post") {
  const char source[] = "ld3r {v0.8b, v1.8b, v2.8b}, [x0], #3\n";

  uint8_t buf[3] = {(uint8_t)(0x51 + 0), (uint8_t)(0x51 + 1),
                    (uint8_t)(0x51 + 2)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8) |
        ((uint32_t)buf[2] << 16)),
       3, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8) |
        ((uint32_t)buf[2] << 16)),
       3, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3Rv8b_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD3Rv8b_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD3Rv8b_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == (uint64_t)(ONES * buf[0]));
                     CHECK((uint64_t)(fprState->v0 >> 64) == 0);
                     CHECK((uint64_t)fprState->v1 == (uint64_t)(ONES * buf[1]));
                     CHECK((uint64_t)(fprState->v1 >> 64) == 0);
                     CHECK((uint64_t)fprState->v2 == (uint64_t)(ONES * buf[2]));
                     CHECK((uint64_t)(fprState->v2 >> 64) == 0);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 3);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3r_v16b") {
  const char source[] = "ld3r {v0.16b, v1.16b, v2.16b}, [x0]\n";

  uint8_t buf[3] = {(uint8_t)(0x51 + 0), (uint8_t)(0x51 + 1),
                    (uint8_t)(0x51 + 2)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8) |
        ((uint32_t)buf[2] << 16)),
       3, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8) |
        ((uint32_t)buf[2] << 16)),
       3, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3Rv16b", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD3Rv16b", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD3Rv16b", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v0 == ONES * buf[0]);
                     CHECK(fprState->v1 == ONES * buf[1]);
                     CHECK(fprState->v2 == ONES * buf[2]);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3r_v16b_post") {
  const char source[] = "ld3r {v0.16b, v1.16b, v2.16b}, [x0], #3\n";

  uint8_t buf[3] = {(uint8_t)(0x51 + 0), (uint8_t)(0x51 + 1),
                    (uint8_t)(0x51 + 2)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8) |
        ((uint32_t)buf[2] << 16)),
       3, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8) |
        ((uint32_t)buf[2] << 16)),
       3, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3Rv16b_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD3Rv16b_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD3Rv16b_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v0 == ONES * buf[0]);
                     CHECK(fprState->v1 == ONES * buf[1]);
                     CHECK(fprState->v2 == ONES * buf[2]);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 3);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4r_v8b") {
  const char source[] = "ld4r {v0.8b, v1.8b, v2.8b, v3.8b}, [x0]\n";

  uint8_t buf[4] = {(uint8_t)(0x51 + 0), (uint8_t)(0x51 + 1),
                    (uint8_t)(0x51 + 2), (uint8_t)(0x51 + 3)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8) |
        ((uint32_t)buf[2] << 16) | ((uint32_t)buf[3] << 24)),
       4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8) |
        ((uint32_t)buf[2] << 16) | ((uint32_t)buf[3] << 24)),
       4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4Rv8b", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4Rv8b", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD4Rv8b", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == (uint64_t)(ONES * buf[0]));
                     CHECK((uint64_t)(fprState->v0 >> 64) == 0);
                     CHECK((uint64_t)fprState->v1 == (uint64_t)(ONES * buf[1]));
                     CHECK((uint64_t)(fprState->v1 >> 64) == 0);
                     CHECK((uint64_t)fprState->v2 == (uint64_t)(ONES * buf[2]));
                     CHECK((uint64_t)(fprState->v2 >> 64) == 0);
                     CHECK((uint64_t)fprState->v3 == (uint64_t)(ONES * buf[3]));
                     CHECK((uint64_t)(fprState->v3 >> 64) == 0);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4r_v8b_post") {
  const char source[] = "ld4r {v0.8b, v1.8b, v2.8b, v3.8b}, [x0], #4\n";

  uint8_t buf[4] = {(uint8_t)(0x51 + 0), (uint8_t)(0x51 + 1),
                    (uint8_t)(0x51 + 2), (uint8_t)(0x51 + 3)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8) |
        ((uint32_t)buf[2] << 16) | ((uint32_t)buf[3] << 24)),
       4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8) |
        ((uint32_t)buf[2] << 16) | ((uint32_t)buf[3] << 24)),
       4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4Rv8b_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4Rv8b_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD4Rv8b_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 == (uint64_t)(ONES * buf[0]));
                     CHECK((uint64_t)(fprState->v0 >> 64) == 0);
                     CHECK((uint64_t)fprState->v1 == (uint64_t)(ONES * buf[1]));
                     CHECK((uint64_t)(fprState->v1 >> 64) == 0);
                     CHECK((uint64_t)fprState->v2 == (uint64_t)(ONES * buf[2]));
                     CHECK((uint64_t)(fprState->v2 >> 64) == 0);
                     CHECK((uint64_t)fprState->v3 == (uint64_t)(ONES * buf[3]));
                     CHECK((uint64_t)(fprState->v3 >> 64) == 0);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4r_v16b") {
  const char source[] = "ld4r {v0.16b, v1.16b, v2.16b, v3.16b}, [x0]\n";

  uint8_t buf[4] = {(uint8_t)(0x51 + 0), (uint8_t)(0x51 + 1),
                    (uint8_t)(0x51 + 2), (uint8_t)(0x51 + 3)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8) |
        ((uint32_t)buf[2] << 16) | ((uint32_t)buf[3] << 24)),
       4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8) |
        ((uint32_t)buf[2] << 16) | ((uint32_t)buf[3] << 24)),
       4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4Rv16b", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4Rv16b", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD4Rv16b", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v0 == ONES * buf[0]);
                     CHECK(fprState->v1 == ONES * buf[1]);
                     CHECK(fprState->v2 == ONES * buf[2]);
                     CHECK(fprState->v3 == ONES * buf[3]);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4r_v16b_post") {
  const char source[] = "ld4r {v0.16b, v1.16b, v2.16b, v3.16b}, [x0], #4\n";

  uint8_t buf[4] = {(uint8_t)(0x51 + 0), (uint8_t)(0x51 + 1),
                    (uint8_t)(0x51 + 2), (uint8_t)(0x51 + 3)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8) |
        ((uint32_t)buf[2] << 16) | ((uint32_t)buf[3] << 24)),
       4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8) |
        ((uint32_t)buf[2] << 16) | ((uint32_t)buf[3] << 24)),
       4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4Rv16b_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4Rv16b_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD4Rv16b_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v0 == ONES * buf[0]);
                     CHECK(fprState->v1 == ONES * buf[1]);
                     CHECK(fprState->v2 == ONES * buf[2]);
                     CHECK(fprState->v3 == ONES * buf[3]);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1_i8") {
  const char source[] = "ld1 {v0.b}[5], [x0]\n";

  uint8_t buf[1] = {(uint8_t)(0x61 + 0)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], (((uint32_t)buf[0] << 0)), 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], (((uint32_t)buf[0] << 0)), 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1i8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1i8", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1i8", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint8_t)(fprState->v0 >> (5 * 8)) == buf[0]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = 0;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1_i8_post") {
  const char source[] = "ld1 {v0.b}[5], [x0], #1\n";

  uint8_t buf[1] = {(uint8_t)(0x61 + 0)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], (((uint32_t)buf[0] << 0)), 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], (((uint32_t)buf[0] << 0)), 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1i8_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1i8_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1i8_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint8_t)(fprState->v0 >> (5 * 8)) == buf[0]);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = 0;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2_i8") {
  const char source[] = "ld2 {v0.b, v1.b}[5], [x0]\n";

  uint8_t buf[2] = {(uint8_t)(0x61 + 0), (uint8_t)(0x61 + 1)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8)), 2,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8)), 2,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2i8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2i8", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD2i8", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint8_t)(fprState->v0 >> (5 * 8)) == buf[0]);
                     CHECK((uint8_t)(fprState->v1 >> (5 * 8)) == buf[1]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = 0;
  fpr->v1 = 0;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2_i8_post") {
  const char source[] = "ld2 {v0.b, v1.b}[5], [x0], #2\n";

  uint8_t buf[2] = {(uint8_t)(0x61 + 0), (uint8_t)(0x61 + 1)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8)), 2,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8)), 2,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2i8_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2i8_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD2i8_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint8_t)(fprState->v0 >> (5 * 8)) == buf[0]);
                     CHECK((uint8_t)(fprState->v1 >> (5 * 8)) == buf[1]);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 2);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = 0;
  fpr->v1 = 0;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3_i8") {
  const char source[] = "ld3 {v0.b, v1.b, v2.b}[5], [x0]\n";

  uint8_t buf[3] = {(uint8_t)(0x61 + 0), (uint8_t)(0x61 + 1),
                    (uint8_t)(0x61 + 2)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8) |
        ((uint32_t)buf[2] << 16)),
       3, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8) |
        ((uint32_t)buf[2] << 16)),
       3, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3i8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD3i8", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD3i8", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint8_t)(fprState->v0 >> (5 * 8)) == buf[0]);
                     CHECK((uint8_t)(fprState->v1 >> (5 * 8)) == buf[1]);
                     CHECK((uint8_t)(fprState->v2 >> (5 * 8)) == buf[2]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = 0;
  fpr->v1 = 0;
  fpr->v2 = 0;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3_i8_post") {
  const char source[] = "ld3 {v0.b, v1.b, v2.b}[5], [x0], #3\n";

  uint8_t buf[3] = {(uint8_t)(0x61 + 0), (uint8_t)(0x61 + 1),
                    (uint8_t)(0x61 + 2)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8) |
        ((uint32_t)buf[2] << 16)),
       3, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8) |
        ((uint32_t)buf[2] << 16)),
       3, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3i8_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD3i8_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD3i8_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint8_t)(fprState->v0 >> (5 * 8)) == buf[0]);
                     CHECK((uint8_t)(fprState->v1 >> (5 * 8)) == buf[1]);
                     CHECK((uint8_t)(fprState->v2 >> (5 * 8)) == buf[2]);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 3);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = 0;
  fpr->v1 = 0;
  fpr->v2 = 0;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4_i8") {
  const char source[] = "ld4 {v0.b, v1.b, v2.b, v3.b}[5], [x0]\n";

  uint8_t buf[4] = {(uint8_t)(0x61 + 0), (uint8_t)(0x61 + 1),
                    (uint8_t)(0x61 + 2), (uint8_t)(0x61 + 3)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8) |
        ((uint32_t)buf[2] << 16) | ((uint32_t)buf[3] << 24)),
       4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8) |
        ((uint32_t)buf[2] << 16) | ((uint32_t)buf[3] << 24)),
       4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4i8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4i8", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD4i8", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint8_t)(fprState->v0 >> (5 * 8)) == buf[0]);
                     CHECK((uint8_t)(fprState->v1 >> (5 * 8)) == buf[1]);
                     CHECK((uint8_t)(fprState->v2 >> (5 * 8)) == buf[2]);
                     CHECK((uint8_t)(fprState->v3 >> (5 * 8)) == buf[3]);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = 0;
  fpr->v1 = 0;
  fpr->v2 = 0;
  fpr->v3 = 0;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4_i8_post") {
  const char source[] = "ld4 {v0.b, v1.b, v2.b, v3.b}[5], [x0], #4\n";

  uint8_t buf[4] = {(uint8_t)(0x61 + 0), (uint8_t)(0x61 + 1),
                    (uint8_t)(0x61 + 2), (uint8_t)(0x61 + 3)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8) |
        ((uint32_t)buf[2] << 16) | ((uint32_t)buf[3] << 24)),
       4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       (((uint32_t)buf[0] << 0) | ((uint32_t)buf[1] << 8) |
        ((uint32_t)buf[2] << 16) | ((uint32_t)buf[3] << 24)),
       4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4i8_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4i8_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD4i8_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint8_t)(fprState->v0 >> (5 * 8)) == buf[0]);
                     CHECK((uint8_t)(fprState->v1 >> (5 * 8)) == buf[1]);
                     CHECK((uint8_t)(fprState->v2 >> (5 * 8)) == buf[2]);
                     CHECK((uint8_t)(fprState->v3 >> (5 * 8)) == buf[3]);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 4);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = 0;
  fpr->v1 = 0;
  fpr->v2 = 0;
  fpr->v3 = 0;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1_i8") {
  const char source[] = "st1 {v0.b}[5], [x0]\n";

  uint8_t buf[1] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], (uint8_t)(0x71 + 0), 1, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1i8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1i8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((unsigned __int128)(0x71 + 0)) << (5 * 8);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == (uint8_t)(0x71 + 0));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1_i8_post") {
  const char source[] = "st1 {v0.b}[5], [x0], #1\n";

  uint8_t buf[1] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], (uint8_t)(0x71 + 0), 1, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1i8_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1i8_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("ST1i8_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((unsigned __int128)(0x71 + 0)) << (5 * 8);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(buf[0] == (uint8_t)(0x71 + 0));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st2_i8") {
  const char source[] = "st2 {v0.b, v1.b}[5], [x0]\n";

  uint8_t buf[2] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       (uint32_t)((uint8_t)(0x71 + 0)) | ((uint32_t)((uint8_t)(0x71 + 1)) << 8),
       2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST2i8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST2i8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((unsigned __int128)(0x71 + 0)) << (5 * 8);
  fpr->v1 = ((unsigned __int128)(0x71 + 1)) << (5 * 8);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == (uint8_t)(0x71 + 0));
  CHECK(buf[1] == (uint8_t)(0x71 + 1));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st2_i8_post") {
  const char source[] = "st2 {v0.b, v1.b}[5], [x0], #2\n";

  uint8_t buf[2] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       (uint32_t)((uint8_t)(0x71 + 0)) | ((uint32_t)((uint8_t)(0x71 + 1)) << 8),
       2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST2i8_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST2i8_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("ST2i8_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 2);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((unsigned __int128)(0x71 + 0)) << (5 * 8);
  fpr->v1 = ((unsigned __int128)(0x71 + 1)) << (5 * 8);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(buf[0] == (uint8_t)(0x71 + 0));
  CHECK(buf[1] == (uint8_t)(0x71 + 1));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st3_i8") {
  const char source[] = "st3 {v0.b, v1.b, v2.b}[5], [x0]\n";

  uint8_t buf[3] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       (uint32_t)((uint8_t)(0x71 + 0)) |
           ((uint32_t)((uint8_t)(0x71 + 1)) << 8) |
           ((uint32_t)((uint8_t)(0x71 + 2)) << 16),
       3, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST3i8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST3i8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((unsigned __int128)(0x71 + 0)) << (5 * 8);
  fpr->v1 = ((unsigned __int128)(0x71 + 1)) << (5 * 8);
  fpr->v2 = ((unsigned __int128)(0x71 + 2)) << (5 * 8);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == (uint8_t)(0x71 + 0));
  CHECK(buf[1] == (uint8_t)(0x71 + 1));
  CHECK(buf[2] == (uint8_t)(0x71 + 2));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st3_i8_post") {
  const char source[] = "st3 {v0.b, v1.b, v2.b}[5], [x0], #3\n";

  uint8_t buf[3] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       (uint32_t)((uint8_t)(0x71 + 0)) |
           ((uint32_t)((uint8_t)(0x71 + 1)) << 8) |
           ((uint32_t)((uint8_t)(0x71 + 2)) << 16),
       3, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST3i8_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST3i8_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("ST3i8_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 3);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((unsigned __int128)(0x71 + 0)) << (5 * 8);
  fpr->v1 = ((unsigned __int128)(0x71 + 1)) << (5 * 8);
  fpr->v2 = ((unsigned __int128)(0x71 + 2)) << (5 * 8);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(buf[0] == (uint8_t)(0x71 + 0));
  CHECK(buf[1] == (uint8_t)(0x71 + 1));
  CHECK(buf[2] == (uint8_t)(0x71 + 2));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st4_i8") {
  const char source[] = "st4 {v0.b, v1.b, v2.b, v3.b}[5], [x0]\n";

  uint8_t buf[4] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       (uint32_t)((uint8_t)(0x71 + 0)) |
           ((uint32_t)((uint8_t)(0x71 + 1)) << 8) |
           ((uint32_t)((uint8_t)(0x71 + 2)) << 16) |
           ((uint32_t)((uint8_t)(0x71 + 3)) << 24),
       4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST4i8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST4i8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((unsigned __int128)(0x71 + 0)) << (5 * 8);
  fpr->v1 = ((unsigned __int128)(0x71 + 1)) << (5 * 8);
  fpr->v2 = ((unsigned __int128)(0x71 + 2)) << (5 * 8);
  fpr->v3 = ((unsigned __int128)(0x71 + 3)) << (5 * 8);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == (uint8_t)(0x71 + 0));
  CHECK(buf[1] == (uint8_t)(0x71 + 1));
  CHECK(buf[2] == (uint8_t)(0x71 + 2));
  CHECK(buf[3] == (uint8_t)(0x71 + 3));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st4_i8_post") {
  const char source[] = "st4 {v0.b, v1.b, v2.b, v3.b}[5], [x0], #4\n";

  uint8_t buf[4] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       (uint32_t)((uint8_t)(0x71 + 0)) |
           ((uint32_t)((uint8_t)(0x71 + 1)) << 8) |
           ((uint32_t)((uint8_t)(0x71 + 2)) << 16) |
           ((uint32_t)((uint8_t)(0x71 + 3)) << 24),
       4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST4i8_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST4i8_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("ST4i8_POST", QBDI::POSTINST,
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
  fpr->v0 = ((unsigned __int128)(0x71 + 0)) << (5 * 8);
  fpr->v1 = ((unsigned __int128)(0x71 + 1)) << (5 * 8);
  fpr->v2 = ((unsigned __int128)(0x71 + 2)) << (5 * 8);
  fpr->v3 = ((unsigned __int128)(0x71 + 3)) << (5 * 8);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(buf[0] == (uint8_t)(0x71 + 0));
  CHECK(buf[1] == (uint8_t)(0x71 + 1));
  CHECK(buf[2] == (uint8_t)(0x71 + 2));
  CHECK(buf[3] == (uint8_t)(0x71 + 3));
}
