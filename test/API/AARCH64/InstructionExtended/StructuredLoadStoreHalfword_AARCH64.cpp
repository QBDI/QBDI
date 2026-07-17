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

static const unsigned __int128 ONES16 =
    ((unsigned __int128)0x0001000100010001ULL << 64) | 0x0001000100010001ULL;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1r_v4h") {
  const char source[] = "ld1r {v0.4h}, [x0]\n";

  uint16_t buf[1] = {(uint16_t)(0x5100 + 0)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], ((((uint64_t)buf[0]) << 0)), 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], ((((uint64_t)buf[0]) << 0)), 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Rv4h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Rv4h", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Rv4h", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 ==
                           (uint64_t)(ONES16 * buf[0]));
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1r_v4h_post") {
  const char source[] = "ld1r {v0.4h}, [x0], #2\n";

  uint16_t buf[1] = {(uint16_t)(0x5100 + 0)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], ((((uint64_t)buf[0]) << 0)), 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], ((((uint64_t)buf[0]) << 0)), 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Rv4h_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Rv4h_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Rv4h_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint64_t)fprState->v0 ==
                           (uint64_t)(ONES16 * buf[0]));
                     CHECK((uint64_t)(fprState->v0 >> 64) == 0);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1r_v8h") {
  const char source[] = "ld1r {v0.8h}, [x0]\n";

  uint16_t buf[1] = {(uint16_t)(0x5100 + 0)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], ((((uint64_t)buf[0]) << 0)), 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], ((((uint64_t)buf[0]) << 0)), 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Rv8h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Rv8h", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Rv8h", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v0 == ONES16 * buf[0]);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1r_v8h_post") {
  const char source[] = "ld1r {v0.8h}, [x0], #2\n";

  uint16_t buf[1] = {(uint16_t)(0x5100 + 0)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], ((((uint64_t)buf[0]) << 0)), 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], ((((uint64_t)buf[0]) << 0)), 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1Rv8h_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1Rv8h_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1Rv8h_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v0 == ONES16 * buf[0]);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2r_v4h") {
  const char source[] = "ld2r {v0.4h, v1.4h}, [x0]\n";

  uint16_t buf[2] = {(uint16_t)(0x5100 + 0), (uint16_t)(0x5100 + 1)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16)), 4,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16)), 4,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2Rv4h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2Rv4h", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB(
      "LD2Rv4h", QBDI::POSTINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        CHECK((uint64_t)fprState->v0 == (uint64_t)(ONES16 * buf[0]));
        CHECK((uint64_t)(fprState->v0 >> 64) == 0);
        CHECK((uint64_t)fprState->v1 == (uint64_t)(ONES16 * buf[1]));
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2r_v4h_post") {
  const char source[] = "ld2r {v0.4h, v1.4h}, [x0], #4\n";

  uint16_t buf[2] = {(uint16_t)(0x5100 + 0), (uint16_t)(0x5100 + 1)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16)), 4,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16)), 4,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2Rv4h_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2Rv4h_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB(
      "LD2Rv4h_POST", QBDI::POSTINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        CHECK((uint64_t)fprState->v0 == (uint64_t)(ONES16 * buf[0]));
        CHECK((uint64_t)(fprState->v0 >> 64) == 0);
        CHECK((uint64_t)fprState->v1 == (uint64_t)(ONES16 * buf[1]));
        CHECK((uint64_t)(fprState->v1 >> 64) == 0);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2r_v8h") {
  const char source[] = "ld2r {v0.8h, v1.8h}, [x0]\n";

  uint16_t buf[2] = {(uint16_t)(0x5100 + 0), (uint16_t)(0x5100 + 1)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16)), 4,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16)), 4,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2Rv8h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2Rv8h", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD2Rv8h", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v0 == ONES16 * buf[0]);
                     CHECK(fprState->v1 == ONES16 * buf[1]);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2r_v8h_post") {
  const char source[] = "ld2r {v0.8h, v1.8h}, [x0], #4\n";

  uint16_t buf[2] = {(uint16_t)(0x5100 + 0), (uint16_t)(0x5100 + 1)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16)), 4,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16)), 4,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2Rv8h_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2Rv8h_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD2Rv8h_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v0 == ONES16 * buf[0]);
                     CHECK(fprState->v1 == ONES16 * buf[1]);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3r_v4h") {
  const char source[] = "ld3r {v0.4h, v1.4h, v2.4h}, [x0]\n";

  uint16_t buf[3] = {(uint16_t)(0x5100 + 0), (uint16_t)(0x5100 + 1),
                     (uint16_t)(0x5100 + 2)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16) |
        (((uint64_t)buf[2]) << 32)),
       6, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16) |
        (((uint64_t)buf[2]) << 32)),
       6, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3Rv4h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD3Rv4h", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB(
      "LD3Rv4h", QBDI::POSTINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        CHECK((uint64_t)fprState->v0 == (uint64_t)(ONES16 * buf[0]));
        CHECK((uint64_t)(fprState->v0 >> 64) == 0);
        CHECK((uint64_t)fprState->v1 == (uint64_t)(ONES16 * buf[1]));
        CHECK((uint64_t)(fprState->v1 >> 64) == 0);
        CHECK((uint64_t)fprState->v2 == (uint64_t)(ONES16 * buf[2]));
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3r_v4h_post") {
  const char source[] = "ld3r {v0.4h, v1.4h, v2.4h}, [x0], #6\n";

  uint16_t buf[3] = {(uint16_t)(0x5100 + 0), (uint16_t)(0x5100 + 1),
                     (uint16_t)(0x5100 + 2)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16) |
        (((uint64_t)buf[2]) << 32)),
       6, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16) |
        (((uint64_t)buf[2]) << 32)),
       6, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3Rv4h_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD3Rv4h_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB(
      "LD3Rv4h_POST", QBDI::POSTINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        CHECK((uint64_t)fprState->v0 == (uint64_t)(ONES16 * buf[0]));
        CHECK((uint64_t)(fprState->v0 >> 64) == 0);
        CHECK((uint64_t)fprState->v1 == (uint64_t)(ONES16 * buf[1]));
        CHECK((uint64_t)(fprState->v1 >> 64) == 0);
        CHECK((uint64_t)fprState->v2 == (uint64_t)(ONES16 * buf[2]));
        CHECK((uint64_t)(fprState->v2 >> 64) == 0);
        CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 6);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3r_v8h") {
  const char source[] = "ld3r {v0.8h, v1.8h, v2.8h}, [x0]\n";

  uint16_t buf[3] = {(uint16_t)(0x5100 + 0), (uint16_t)(0x5100 + 1),
                     (uint16_t)(0x5100 + 2)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16) |
        (((uint64_t)buf[2]) << 32)),
       6, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16) |
        (((uint64_t)buf[2]) << 32)),
       6, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3Rv8h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD3Rv8h", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD3Rv8h", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v0 == ONES16 * buf[0]);
                     CHECK(fprState->v1 == ONES16 * buf[1]);
                     CHECK(fprState->v2 == ONES16 * buf[2]);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3r_v8h_post") {
  const char source[] = "ld3r {v0.8h, v1.8h, v2.8h}, [x0], #6\n";

  uint16_t buf[3] = {(uint16_t)(0x5100 + 0), (uint16_t)(0x5100 + 1),
                     (uint16_t)(0x5100 + 2)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16) |
        (((uint64_t)buf[2]) << 32)),
       6, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16) |
        (((uint64_t)buf[2]) << 32)),
       6, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3Rv8h_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD3Rv8h_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD3Rv8h_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v0 == ONES16 * buf[0]);
                     CHECK(fprState->v1 == ONES16 * buf[1]);
                     CHECK(fprState->v2 == ONES16 * buf[2]);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 6);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4r_v4h") {
  const char source[] = "ld4r {v0.4h, v1.4h, v2.4h, v3.4h}, [x0]\n";

  uint16_t buf[4] = {(uint16_t)(0x5100 + 0), (uint16_t)(0x5100 + 1),
                     (uint16_t)(0x5100 + 2), (uint16_t)(0x5100 + 3)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16) |
        (((uint64_t)buf[2]) << 32) | (((uint64_t)buf[3]) << 48)),
       8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16) |
        (((uint64_t)buf[2]) << 32) | (((uint64_t)buf[3]) << 48)),
       8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4Rv4h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4Rv4h", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB(
      "LD4Rv4h", QBDI::POSTINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        CHECK((uint64_t)fprState->v0 == (uint64_t)(ONES16 * buf[0]));
        CHECK((uint64_t)(fprState->v0 >> 64) == 0);
        CHECK((uint64_t)fprState->v1 == (uint64_t)(ONES16 * buf[1]));
        CHECK((uint64_t)(fprState->v1 >> 64) == 0);
        CHECK((uint64_t)fprState->v2 == (uint64_t)(ONES16 * buf[2]));
        CHECK((uint64_t)(fprState->v2 >> 64) == 0);
        CHECK((uint64_t)fprState->v3 == (uint64_t)(ONES16 * buf[3]));
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4r_v4h_post") {
  const char source[] = "ld4r {v0.4h, v1.4h, v2.4h, v3.4h}, [x0], #8\n";

  uint16_t buf[4] = {(uint16_t)(0x5100 + 0), (uint16_t)(0x5100 + 1),
                     (uint16_t)(0x5100 + 2), (uint16_t)(0x5100 + 3)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16) |
        (((uint64_t)buf[2]) << 32) | (((uint64_t)buf[3]) << 48)),
       8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16) |
        (((uint64_t)buf[2]) << 32) | (((uint64_t)buf[3]) << 48)),
       8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4Rv4h_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4Rv4h_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB(
      "LD4Rv4h_POST", QBDI::POSTINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        CHECK((uint64_t)fprState->v0 == (uint64_t)(ONES16 * buf[0]));
        CHECK((uint64_t)(fprState->v0 >> 64) == 0);
        CHECK((uint64_t)fprState->v1 == (uint64_t)(ONES16 * buf[1]));
        CHECK((uint64_t)(fprState->v1 >> 64) == 0);
        CHECK((uint64_t)fprState->v2 == (uint64_t)(ONES16 * buf[2]));
        CHECK((uint64_t)(fprState->v2 >> 64) == 0);
        CHECK((uint64_t)fprState->v3 == (uint64_t)(ONES16 * buf[3]));
        CHECK((uint64_t)(fprState->v3 >> 64) == 0);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4r_v8h") {
  const char source[] = "ld4r {v0.8h, v1.8h, v2.8h, v3.8h}, [x0]\n";

  uint16_t buf[4] = {(uint16_t)(0x5100 + 0), (uint16_t)(0x5100 + 1),
                     (uint16_t)(0x5100 + 2), (uint16_t)(0x5100 + 3)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16) |
        (((uint64_t)buf[2]) << 32) | (((uint64_t)buf[3]) << 48)),
       8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16) |
        (((uint64_t)buf[2]) << 32) | (((uint64_t)buf[3]) << 48)),
       8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4Rv8h", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4Rv8h", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD4Rv8h", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v0 == ONES16 * buf[0]);
                     CHECK(fprState->v1 == ONES16 * buf[1]);
                     CHECK(fprState->v2 == ONES16 * buf[2]);
                     CHECK(fprState->v3 == ONES16 * buf[3]);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4r_v8h_post") {
  const char source[] = "ld4r {v0.8h, v1.8h, v2.8h, v3.8h}, [x0], #8\n";

  uint16_t buf[4] = {(uint16_t)(0x5100 + 0), (uint16_t)(0x5100 + 1),
                     (uint16_t)(0x5100 + 2), (uint16_t)(0x5100 + 3)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16) |
        (((uint64_t)buf[2]) << 32) | (((uint64_t)buf[3]) << 48)),
       8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16) |
        (((uint64_t)buf[2]) << 32) | (((uint64_t)buf[3]) << 48)),
       8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4Rv8h_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4Rv8h_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD4Rv8h_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v0 == ONES16 * buf[0]);
                     CHECK(fprState->v1 == ONES16 * buf[1]);
                     CHECK(fprState->v2 == ONES16 * buf[2]);
                     CHECK(fprState->v3 == ONES16 * buf[3]);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1_i16") {
  const char source[] = "ld1 {v0.h}[3], [x0]\n";

  uint16_t buf[1] = {(uint16_t)(0x6100 + 0)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], ((((uint64_t)buf[0]) << 0)), 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], ((((uint64_t)buf[0]) << 0)), 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1i16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1i16", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1i16", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint16_t)(fprState->v0 >> (3 * 16)) == buf[0]);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld1_i16_post") {
  const char source[] = "ld1 {v0.h}[3], [x0], #2\n";

  uint16_t buf[1] = {(uint16_t)(0x6100 + 0)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], ((((uint64_t)buf[0]) << 0)), 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], ((((uint64_t)buf[0]) << 0)), 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD1i16_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD1i16_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD1i16_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint16_t)(fprState->v0 >> (3 * 16)) == buf[0]);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 2);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2_i16") {
  const char source[] = "ld2 {v0.h, v1.h}[3], [x0]\n";

  uint16_t buf[2] = {(uint16_t)(0x6100 + 0), (uint16_t)(0x6100 + 1)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16)), 4,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16)), 4,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2i16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2i16", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD2i16", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint16_t)(fprState->v0 >> (3 * 16)) == buf[0]);
                     CHECK((uint16_t)(fprState->v1 >> (3 * 16)) == buf[1]);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld2_i16_post") {
  const char source[] = "ld2 {v0.h, v1.h}[3], [x0], #4\n";

  uint16_t buf[2] = {(uint16_t)(0x6100 + 0), (uint16_t)(0x6100 + 1)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16)), 4,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16)), 4,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD2i16_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD2i16_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD2i16_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint16_t)(fprState->v0 >> (3 * 16)) == buf[0]);
                     CHECK((uint16_t)(fprState->v1 >> (3 * 16)) == buf[1]);
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
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3_i16") {
  const char source[] = "ld3 {v0.h, v1.h, v2.h}[3], [x0]\n";

  uint16_t buf[3] = {(uint16_t)(0x6100 + 0), (uint16_t)(0x6100 + 1),
                     (uint16_t)(0x6100 + 2)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16) |
        (((uint64_t)buf[2]) << 32)),
       6, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16) |
        (((uint64_t)buf[2]) << 32)),
       6, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3i16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD3i16", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD3i16", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint16_t)(fprState->v0 >> (3 * 16)) == buf[0]);
                     CHECK((uint16_t)(fprState->v1 >> (3 * 16)) == buf[1]);
                     CHECK((uint16_t)(fprState->v2 >> (3 * 16)) == buf[2]);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld3_i16_post") {
  const char source[] = "ld3 {v0.h, v1.h, v2.h}[3], [x0], #6\n";

  uint16_t buf[3] = {(uint16_t)(0x6100 + 0), (uint16_t)(0x6100 + 1),
                     (uint16_t)(0x6100 + 2)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16) |
        (((uint64_t)buf[2]) << 32)),
       6, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16) |
        (((uint64_t)buf[2]) << 32)),
       6, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD3i16_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD3i16_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD3i16_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint16_t)(fprState->v0 >> (3 * 16)) == buf[0]);
                     CHECK((uint16_t)(fprState->v1 >> (3 * 16)) == buf[1]);
                     CHECK((uint16_t)(fprState->v2 >> (3 * 16)) == buf[2]);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 6);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4_i16") {
  const char source[] = "ld4 {v0.h, v1.h, v2.h, v3.h}[3], [x0]\n";

  uint16_t buf[4] = {(uint16_t)(0x6100 + 0), (uint16_t)(0x6100 + 1),
                     (uint16_t)(0x6100 + 2), (uint16_t)(0x6100 + 3)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16) |
        (((uint64_t)buf[2]) << 32) | (((uint64_t)buf[3]) << 48)),
       8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16) |
        (((uint64_t)buf[2]) << 32) | (((uint64_t)buf[3]) << 48)),
       8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4i16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4i16", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD4i16", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint16_t)(fprState->v0 >> (3 * 16)) == buf[0]);
                     CHECK((uint16_t)(fprState->v1 >> (3 * 16)) == buf[1]);
                     CHECK((uint16_t)(fprState->v2 >> (3 * 16)) == buf[2]);
                     CHECK((uint16_t)(fprState->v3 >> (3 * 16)) == buf[3]);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld4_i16_post") {
  const char source[] = "ld4 {v0.h, v1.h, v2.h, v3.h}[3], [x0], #8\n";

  uint16_t buf[4] = {(uint16_t)(0x6100 + 0), (uint16_t)(0x6100 + 1),
                     (uint16_t)(0x6100 + 2), (uint16_t)(0x6100 + 3)};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16) |
        (((uint64_t)buf[2]) << 32) | (((uint64_t)buf[3]) << 48)),
       8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       ((((uint64_t)buf[0]) << 0) | (((uint64_t)buf[1]) << 16) |
        (((uint64_t)buf[2]) << 32) | (((uint64_t)buf[3]) << 48)),
       8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD4i16_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD4i16_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD4i16_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint16_t)(fprState->v0 >> (3 * 16)) == buf[0]);
                     CHECK((uint16_t)(fprState->v1 >> (3 * 16)) == buf[1]);
                     CHECK((uint16_t)(fprState->v2 >> (3 * 16)) == buf[2]);
                     CHECK((uint16_t)(fprState->v3 >> (3 * 16)) == buf[3]);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 8);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1_i16") {
  const char source[] = "st1 {v0.h}[3], [x0]\n";

  uint16_t buf[1] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], (uint16_t)(0x7000 + 0), 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST1i16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1i16", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((unsigned __int128)(0x7000 + 0)) << (3 * 16);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == (uint16_t)(0x7000 + 0));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st1_i16_post") {
  const char source[] = "st1 {v0.h}[3], [x0], #2\n";

  uint16_t buf[1] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], (uint16_t)(0x7000 + 0), 2, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST1i16_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST1i16_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("ST1i16_POST", QBDI::POSTINST,
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
  fpr->v0 = ((unsigned __int128)(0x7000 + 0)) << (3 * 16);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(buf[0] == (uint16_t)(0x7000 + 0));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st2_i16") {
  const char source[] = "st2 {v0.h, v1.h}[3], [x0]\n";

  uint16_t buf[2] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       (uint32_t)((uint16_t)(0x7000 + 0)) |
           ((uint32_t)((uint16_t)(0x7100 + 3)) << 16),
       4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST2i16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST2i16", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((unsigned __int128)(0x7000 + 0)) << (3 * 16);
  fpr->v1 = ((unsigned __int128)(0x7100 + 3)) << (3 * 16);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == (uint16_t)(0x7000 + 0));
  CHECK(buf[1] == (uint16_t)(0x7100 + 3));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st2_i16_post") {
  const char source[] = "st2 {v0.h, v1.h}[3], [x0], #4\n";

  uint16_t buf[2] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       (uint32_t)((uint16_t)(0x7000 + 0)) |
           ((uint32_t)((uint16_t)(0x7100 + 3)) << 16),
       4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST2i16_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST2i16_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("ST2i16_POST", QBDI::POSTINST,
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
  fpr->v0 = ((unsigned __int128)(0x7000 + 0)) << (3 * 16);
  fpr->v1 = ((unsigned __int128)(0x7100 + 3)) << (3 * 16);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(buf[0] == (uint16_t)(0x7000 + 0));
  CHECK(buf[1] == (uint16_t)(0x7100 + 3));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st3_i16") {
  const char source[] = "st3 {v0.h, v1.h, v2.h}[3], [x0]\n";

  uint16_t buf[3] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       (uint64_t)((uint16_t)(0x7000 + 0)) |
           ((uint64_t)((uint16_t)(0x7100 + 3)) << 16) |
           ((uint64_t)((uint16_t)(0x7200 + 6)) << 32),
       6, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST3i16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST3i16", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((unsigned __int128)(0x7000 + 0)) << (3 * 16);
  fpr->v1 = ((unsigned __int128)(0x7100 + 3)) << (3 * 16);
  fpr->v2 = ((unsigned __int128)(0x7200 + 6)) << (3 * 16);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == (uint16_t)(0x7000 + 0));
  CHECK(buf[1] == (uint16_t)(0x7100 + 3));
  CHECK(buf[2] == (uint16_t)(0x7200 + 6));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st3_i16_post") {
  const char source[] = "st3 {v0.h, v1.h, v2.h}[3], [x0], #6\n";

  uint16_t buf[3] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       (uint64_t)((uint16_t)(0x7000 + 0)) |
           ((uint64_t)((uint16_t)(0x7100 + 3)) << 16) |
           ((uint64_t)((uint16_t)(0x7200 + 6)) << 32),
       6, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST3i16_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST3i16_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("ST3i16_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 6);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((unsigned __int128)(0x7000 + 0)) << (3 * 16);
  fpr->v1 = ((unsigned __int128)(0x7100 + 3)) << (3 * 16);
  fpr->v2 = ((unsigned __int128)(0x7200 + 6)) << (3 * 16);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(buf[0] == (uint16_t)(0x7000 + 0));
  CHECK(buf[1] == (uint16_t)(0x7100 + 3));
  CHECK(buf[2] == (uint16_t)(0x7200 + 6));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st4_i16") {
  const char source[] = "st4 {v0.h, v1.h, v2.h, v3.h}[3], [x0]\n";

  uint16_t buf[4] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       (uint64_t)((uint16_t)(0x7000 + 0)) |
           ((uint64_t)((uint16_t)(0x7100 + 3)) << 16) |
           ((uint64_t)((uint16_t)(0x7200 + 6)) << 32) |
           ((uint64_t)((uint16_t)(0x7300 + 9)) << 48),
       8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST4i16", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST4i16", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = ((unsigned __int128)(0x7000 + 0)) << (3 * 16);
  fpr->v1 = ((unsigned __int128)(0x7100 + 3)) << (3 * 16);
  fpr->v2 = ((unsigned __int128)(0x7200 + 6)) << (3 * 16);
  fpr->v3 = ((unsigned __int128)(0x7300 + 9)) << (3 * 16);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[0] == (uint16_t)(0x7000 + 0));
  CHECK(buf[1] == (uint16_t)(0x7100 + 3));
  CHECK(buf[2] == (uint16_t)(0x7200 + 6));
  CHECK(buf[3] == (uint16_t)(0x7300 + 9));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st4_i16_post") {
  const char source[] = "st4 {v0.h, v1.h, v2.h, v3.h}[3], [x0], #8\n";

  uint16_t buf[4] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0],
       (uint64_t)((uint16_t)(0x7000 + 0)) |
           ((uint64_t)((uint16_t)(0x7100 + 3)) << 16) |
           ((uint64_t)((uint16_t)(0x7200 + 6)) << 32) |
           ((uint64_t)((uint16_t)(0x7300 + 9)) << 48),
       8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST4i16_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST4i16_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("ST4i16_POST", QBDI::POSTINST,
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
  fpr->v0 = ((unsigned __int128)(0x7000 + 0)) << (3 * 16);
  fpr->v1 = ((unsigned __int128)(0x7100 + 3)) << (3 * 16);
  fpr->v2 = ((unsigned __int128)(0x7200 + 6)) << (3 * 16);
  fpr->v3 = ((unsigned __int128)(0x7300 + 9)) << (3 * 16);
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(buf[0] == (uint16_t)(0x7000 + 0));
  CHECK(buf[1] == (uint16_t)(0x7100 + 3));
  CHECK(buf[2] == (uint16_t)(0x7200 + 6));
  CHECK(buf[3] == (uint16_t)(0x7300 + 9));
}
