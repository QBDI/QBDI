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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldclrp") {
  if (!checkFeature("lse128")) {
    return;
  }

  const char source[] = "ldclrp x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t mask0 = 0x0f0f0f0f0f0f0f0fULL;
  constexpr uint64_t mask1 = 0xf0f0f0f0f0f0f0f0ULL;
  constexpr uint64_t new0 = 0x1010101010101010ULL;
  constexpr uint64_t new1 = 0x0202020202020202ULL;
  uint64_t v[2] = {old0, old1};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], old0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], old1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], old0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], old1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[0], new0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], new1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDCLRP", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDCLRP", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDCLRP", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = mask0;
  state->x2 = mask1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldclrpa") {
  if (!checkFeature("lse128")) {
    return;
  }

  const char source[] = "ldclrpa x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t mask0 = 0x0f0f0f0f0f0f0f0fULL;
  constexpr uint64_t mask1 = 0xf0f0f0f0f0f0f0f0ULL;
  constexpr uint64_t new0 = 0x1010101010101010ULL;
  constexpr uint64_t new1 = 0x0202020202020202ULL;
  uint64_t v[2] = {old0, old1};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], old0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], old1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], old0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], old1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[0], new0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], new1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDCLRPA", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDCLRPA", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDCLRPA", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = mask0;
  state->x2 = mask1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldclrpal") {
  if (!checkFeature("lse128")) {
    return;
  }

  const char source[] = "ldclrpal x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t mask0 = 0x0f0f0f0f0f0f0f0fULL;
  constexpr uint64_t mask1 = 0xf0f0f0f0f0f0f0f0ULL;
  constexpr uint64_t new0 = 0x1010101010101010ULL;
  constexpr uint64_t new1 = 0x0202020202020202ULL;
  uint64_t v[2] = {old0, old1};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], old0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], old1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], old0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], old1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[0], new0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], new1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDCLRPAL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDCLRPAL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDCLRPAL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = mask0;
  state->x2 = mask1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldclrpl") {
  if (!checkFeature("lse128")) {
    return;
  }

  const char source[] = "ldclrpl x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t mask0 = 0x0f0f0f0f0f0f0f0fULL;
  constexpr uint64_t mask1 = 0xf0f0f0f0f0f0f0f0ULL;
  constexpr uint64_t new0 = 0x1010101010101010ULL;
  constexpr uint64_t new1 = 0x0202020202020202ULL;
  uint64_t v[2] = {old0, old1};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], old0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], old1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], old0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], old1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[0], new0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], new1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDCLRPL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDCLRPL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDCLRPL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = mask0;
  state->x2 = mask1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldsetp") {
  if (!checkFeature("lse128")) {
    return;
  }

  const char source[] = "ldsetp x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1010101010101010ULL;
  constexpr uint64_t old1 = 0x2020202020202020ULL;
  constexpr uint64_t mask0 = 0x0f0f0f0f0f0f0f0fULL;
  constexpr uint64_t mask1 = 0xf0f0f0f0f0f0f0f0ULL;
  constexpr uint64_t new0 = 0x1f1f1f1f1f1f1f1fULL;
  constexpr uint64_t new1 = 0xf0f0f0f0f0f0f0f0ULL;
  uint64_t v[2] = {old0, old1};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], old0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], old1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], old0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], old1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[0], new0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], new1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDSETP", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDSETP", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDSETP", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = mask0;
  state->x2 = mask1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldsetpa") {
  if (!checkFeature("lse128")) {
    return;
  }

  const char source[] = "ldsetpa x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1010101010101010ULL;
  constexpr uint64_t old1 = 0x2020202020202020ULL;
  constexpr uint64_t mask0 = 0x0f0f0f0f0f0f0f0fULL;
  constexpr uint64_t mask1 = 0xf0f0f0f0f0f0f0f0ULL;
  constexpr uint64_t new0 = 0x1f1f1f1f1f1f1f1fULL;
  constexpr uint64_t new1 = 0xf0f0f0f0f0f0f0f0ULL;
  uint64_t v[2] = {old0, old1};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], old0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], old1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], old0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], old1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[0], new0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], new1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDSETPA", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDSETPA", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDSETPA", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = mask0;
  state->x2 = mask1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldsetpal") {
  if (!checkFeature("lse128")) {
    return;
  }

  const char source[] = "ldsetpal x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1010101010101010ULL;
  constexpr uint64_t old1 = 0x2020202020202020ULL;
  constexpr uint64_t mask0 = 0x0f0f0f0f0f0f0f0fULL;
  constexpr uint64_t mask1 = 0xf0f0f0f0f0f0f0f0ULL;
  constexpr uint64_t new0 = 0x1f1f1f1f1f1f1f1fULL;
  constexpr uint64_t new1 = 0xf0f0f0f0f0f0f0f0ULL;
  uint64_t v[2] = {old0, old1};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], old0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], old1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], old0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], old1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[0], new0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], new1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDSETPAL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDSETPAL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDSETPAL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = mask0;
  state->x2 = mask1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldsetpl") {
  if (!checkFeature("lse128")) {
    return;
  }

  const char source[] = "ldsetpl x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1010101010101010ULL;
  constexpr uint64_t old1 = 0x2020202020202020ULL;
  constexpr uint64_t mask0 = 0x0f0f0f0f0f0f0f0fULL;
  constexpr uint64_t mask1 = 0xf0f0f0f0f0f0f0f0ULL;
  constexpr uint64_t new0 = 0x1f1f1f1f1f1f1f1fULL;
  constexpr uint64_t new1 = 0xf0f0f0f0f0f0f0f0ULL;
  uint64_t v[2] = {old0, old1};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], old0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], old1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v[0], old0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], old1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[0], new0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], new1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDSETPL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDSETPL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDSETPL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = mask0;
  state->x2 = mask1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}
