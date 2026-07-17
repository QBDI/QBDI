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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ld64b") {
  if (!checkFeature("ls64")) {
    return;
  }

  const char source[] = "ld64b x0, [x8]\n";

  constexpr uint64_t e0 = 0x1111111111111111ULL;
  constexpr uint64_t e1 = 0x2222222222222222ULL;
  constexpr uint64_t e2 = 0x3333333333333333ULL;
  constexpr uint64_t e3 = 0x4444444444444444ULL;
  constexpr uint64_t e4 = 0x5555555555555555ULL;
  constexpr uint64_t e5 = 0x6666666666666666ULL;
  constexpr uint64_t e6 = 0x7777777777777777ULL;
  constexpr uint64_t e7 = 0x8888888888888888ULL;
  uint64_t v[8] = {e0, e1, e2, e3, e4, e5, e6, e7};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], v[0], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], v[1], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[2], v[2], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[3], v[3], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[4], v[4], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[5], v[5], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[6], v[6], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[7], v[7], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], v[0], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], v[1], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[2], v[2], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[3], v[3], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[4], v[4], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[5], v[5], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[6], v[6], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[7], v[7], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LD64B", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD64B", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LD64B", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == e0);
                     CHECK(gprState->x1 == e1);
                     CHECK(gprState->x2 == e2);
                     CHECK(gprState->x3 == e3);
                     CHECK(gprState->x4 == e4);
                     CHECK(gprState->x5 == e5);
                     CHECK(gprState->x6 == e6);
                     CHECK(gprState->x7 == e7);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x8 = (QBDI::rword)&v[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"ls64"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st64b") {
  if (!checkFeature("ls64")) {
    return;
  }

  const char source[] = "st64b x0, [x8]\n";

  constexpr uint64_t e0 = 0x9999999999999999ULL;
  constexpr uint64_t e1 = 0xaaaaaaaaaaaaaaaaULL;
  constexpr uint64_t e2 = 0xbbbbbbbbbbbbbbbbULL;
  constexpr uint64_t e3 = 0xccccccccccccccccULL;
  constexpr uint64_t e4 = 0xddddddddddddddddULL;
  constexpr uint64_t e5 = 0xeeeeeeeeeeeeeeeeULL;
  constexpr uint64_t e6 = 0xffffffffffffffffULL;
  constexpr uint64_t e7 = 0x0123456789abcdefULL;
  uint64_t v[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[2], e2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[3], e3, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[4], e4, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[5], e5, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[6], e6, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[7], e7, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("ST64B", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST64B", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x8 = (QBDI::rword)&v[0];
  state->x0 = e0;
  state->x1 = e1;
  state->x2 = e2;
  state->x3 = e3;
  state->x4 = e4;
  state->x5 = e5;
  state->x6 = e6;
  state->x7 = e7;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"ls64"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v[0] == e0);
  CHECK(v[1] == e1);
  CHECK(v[2] == e2);
  CHECK(v[3] == e3);
  CHECK(v[4] == e4);
  CHECK(v[5] == e5);
  CHECK(v[6] == e6);
  CHECK(v[7] == e7);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st64bv") {
  if (!checkFeature("ls64")) {
    return;
  }

  const char source[] = "st64bv x9, x0, [x8]\n";

  constexpr uint64_t e0 = 0x1010101010101010ULL;
  constexpr uint64_t e1 = 0x2020202020202020ULL;
  constexpr uint64_t e2 = 0x3030303030303030ULL;
  constexpr uint64_t e3 = 0x4040404040404040ULL;
  constexpr uint64_t e4 = 0x5050505050505050ULL;
  constexpr uint64_t e5 = 0x6060606060606060ULL;
  constexpr uint64_t e6 = 0x7070707070707070ULL;
  constexpr uint64_t e7 = 0x8080808080808080ULL;
  uint64_t v[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[2], e2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[3], e3, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[4], e4, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[5], e5, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[6], e6, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[7], e7, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST64BV", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST64BV", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("ST64BV", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x9 == 0);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x8 = (QBDI::rword)&v[0];
  state->x0 = e0;
  state->x1 = e1;
  state->x2 = e2;
  state->x3 = e3;
  state->x4 = e4;
  state->x5 = e5;
  state->x6 = e6;
  state->x7 = e7;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"ls64"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-st64bv0") {
  if (!checkFeature("ls64")) {
    return;
  }

  const char source[] = "st64bv0 x9, x0, [x8]\n";

  constexpr uint64_t e0 = 0x0101010101010101ULL;
  constexpr uint64_t e1 = 0x0202020202020202ULL;
  constexpr uint64_t e2 = 0x0303030303030303ULL;
  constexpr uint64_t e3 = 0x0404040404040404ULL;
  constexpr uint64_t e4 = 0x0505050505050505ULL;
  constexpr uint64_t e5 = 0x0606060606060606ULL;
  constexpr uint64_t e6 = 0x0707070707070707ULL;
  constexpr uint64_t e7 = 0x0808080808080808ULL;
  uint64_t v[8] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], e0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], e1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[2], e2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[3], e3, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[4], e4, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[5], e5, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[6], e6, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[7], e7, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("ST64BV0", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST64BV0", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("ST64BV0", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x9 == 0);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x8 = (QBDI::rword)&v[0];
  state->x0 = e0;
  state->x1 = e1;
  state->x2 = e2;
  state->x3 = e3;
  state->x4 = e4;
  state->x5 = e5;
  state->x6 = e6;
  state->x7 = e7;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"ls64"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}
