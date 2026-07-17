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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwcas") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwcas x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x2222222222222222ULL;
  constexpr uint64_t expected = 0x2222222222222222ULL;
  constexpr uint64_t newval = 0x9999999999999999ULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWCAS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWCAS", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWCAS", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = expected;
  state->x2 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwcasa") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwcasa x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x2222222222222222ULL;
  constexpr uint64_t expected = 0x2222222222222222ULL;
  constexpr uint64_t newval = 0x9999999999999999ULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWCASA", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWCASA", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWCASA", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = expected;
  state->x2 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwcasl") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwcasl x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x2222222222222222ULL;
  constexpr uint64_t expected = 0x2222222222222222ULL;
  constexpr uint64_t newval = 0x9999999999999999ULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWCASL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWCASL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWCASL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = expected;
  state->x2 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwcasal") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwcasal x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x2222222222222222ULL;
  constexpr uint64_t expected = 0x2222222222222222ULL;
  constexpr uint64_t newval = 0x9999999999999999ULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWCASAL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWCASAL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWCASAL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = expected;
  state->x2 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwscas") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwscas x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x2222222222222222ULL;
  constexpr uint64_t expected = 0x2222222222222222ULL;
  constexpr uint64_t newval = 0x9999999999999999ULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWSCAS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSCAS", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSCAS", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = expected;
  state->x2 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwscasa") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwscasa x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x2222222222222222ULL;
  constexpr uint64_t expected = 0x2222222222222222ULL;
  constexpr uint64_t newval = 0x9999999999999999ULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWSCASA", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSCASA", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSCASA", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = expected;
  state->x2 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwscasl") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwscasl x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x2222222222222222ULL;
  constexpr uint64_t expected = 0x2222222222222222ULL;
  constexpr uint64_t newval = 0x9999999999999999ULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWSCASL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSCASL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSCASL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = expected;
  state->x2 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwscasal") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwscasal x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x2222222222222222ULL;
  constexpr uint64_t expected = 0x2222222222222222ULL;
  constexpr uint64_t newval = 0x9999999999999999ULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWSCASAL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSCASAL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSCASAL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = expected;
  state->x2 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwcasp") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwcasp x2, x3, x4, x5, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWCASP", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWCASP", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWCASP", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == old0);
                     CHECK(gprState->x3 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x2 = old0;
  state->x3 = old1;
  state->x4 = new0;
  state->x5 = new1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwcaspa") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwcaspa x2, x3, x4, x5, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWCASPA", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWCASPA", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWCASPA", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == old0);
                     CHECK(gprState->x3 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x2 = old0;
  state->x3 = old1;
  state->x4 = new0;
  state->x5 = new1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwcaspl") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwcaspl x2, x3, x4, x5, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWCASPL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWCASPL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWCASPL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == old0);
                     CHECK(gprState->x3 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x2 = old0;
  state->x3 = old1;
  state->x4 = new0;
  state->x5 = new1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwcaspal") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwcaspal x2, x3, x4, x5, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWCASPAL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWCASPAL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWCASPAL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == old0);
                     CHECK(gprState->x3 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x2 = old0;
  state->x3 = old1;
  state->x4 = new0;
  state->x5 = new1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwscasp") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwscasp x2, x3, x4, x5, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWSCASP", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSCASP", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSCASP", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == old0);
                     CHECK(gprState->x3 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x2 = old0;
  state->x3 = old1;
  state->x4 = new0;
  state->x5 = new1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwscaspa") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwscaspa x2, x3, x4, x5, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWSCASPA", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSCASPA", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSCASPA", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == old0);
                     CHECK(gprState->x3 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x2 = old0;
  state->x3 = old1;
  state->x4 = new0;
  state->x5 = new1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwscaspl") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwscaspl x2, x3, x4, x5, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWSCASPL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSCASPL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSCASPL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == old0);
                     CHECK(gprState->x3 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x2 = old0;
  state->x3 = old1;
  state->x4 = new0;
  state->x5 = new1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwscaspal") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwscaspal x2, x3, x4, x5, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWSCASPAL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSCASPAL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSCASPAL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == old0);
                     CHECK(gprState->x3 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x2 = old0;
  state->x3 = old1;
  state->x4 = new0;
  state->x5 = new1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwclr") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwclr x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0x30109010f0f0f0f0ULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWCLR", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWCLR", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWCLR", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwclra") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwclra x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0x30109010f0f0f0f0ULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWCLRA", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWCLRA", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWCLRA", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwclrl") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwclrl x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0x30109010f0f0f0f0ULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWCLRL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWCLRL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWCLRL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwclral") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwclral x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0x30109010f0f0f0f0ULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWCLRAL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWCLRAL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWCLRAL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwsclr") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwsclr x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0x30109010f0f0f0f0ULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWCLRS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWCLRS", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWCLRS", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwsclra") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwsclra x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0x30109010f0f0f0f0ULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWCLRSA", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWCLRSA", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWCLRSA", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwsclrl") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwsclrl x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0x30109010f0f0f0f0ULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWCLRSL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWCLRSL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWCLRSL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwsclral") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwsclral x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0x30109010f0f0f0f0ULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWCLRSAL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWCLRSAL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWCLRSAL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwset") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwset x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0x3f1f9f1fffffffffULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWSET", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSET", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSET", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwseta") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwseta x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0x3f1f9f1fffffffffULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWSETA", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSETA", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSETA", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwsetl") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwsetl x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0x3f1f9f1fffffffffULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWSETL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSETL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSETL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwsetal") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwsetal x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0x3f1f9f1fffffffffULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWSETAL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSETAL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSETAL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwsset") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwsset x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0x3f1f9f1fffffffffULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWSETS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSETS", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSETS", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwsseta") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwsseta x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0x3f1f9f1fffffffffULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWSETSA", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSETSA", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSETSA", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwssetl") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwssetl x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0x3f1f9f1fffffffffULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWSETSL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSETSL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSETSL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwssetal") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwssetal x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0x3f1f9f1fffffffffULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWSETSAL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSETSAL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSETSAL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwswp") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwswp x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0xf0f0f0f0f0f0f0fULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWSWP", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSWP", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSWP", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwswpa") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwswpa x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0xf0f0f0f0f0f0f0fULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWSWPA", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSWPA", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSWPA", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwswpl") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwswpl x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0xf0f0f0f0f0f0f0fULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWSWPL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSWPL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSWPL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwswpal") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwswpal x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0xf0f0f0f0f0f0f0fULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWSWPAL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSWPAL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSWPAL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwsswp") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwsswp x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0xf0f0f0f0f0f0f0fULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWSWPS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSWPS", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSWPS", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwsswpa") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwsswpa x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0xf0f0f0f0f0f0f0fULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWSWPSA", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSWPSA", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSWPSA", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwsswpl") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwsswpl x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0xf0f0f0f0f0f0f0fULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWSWPSL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSWPSL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSWPSL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwsswpal") {
  if (!checkFeature("the")) {
    return;
  }

  const char source[] = "rcwsswpal x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0xf0f0f0f0f0f0f0fULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("RCWSWPSAL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSWPSAL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSWPSAL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwclrp") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwclrp x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWCLRP", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWCLRP", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWCLRP", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = old0;
  state->x2 = old1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwclrpa") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwclrpa x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWCLRPA", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWCLRPA", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWCLRPA", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = old0;
  state->x2 = old1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwclrpl") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwclrpl x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWCLRPL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWCLRPL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWCLRPL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = old0;
  state->x2 = old1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwclrpal") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwclrpal x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWCLRPAL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWCLRPAL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWCLRPAL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = old0;
  state->x2 = old1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwsclrp") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwsclrp x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWCLRSP", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWCLRSP", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWCLRSP", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = old0;
  state->x2 = old1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwsclrpa") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwsclrpa x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWCLRSPA", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWCLRSPA", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWCLRSPA", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = old0;
  state->x2 = old1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwsclrpl") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwsclrpl x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWCLRSPL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWCLRSPL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWCLRSPL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = old0;
  state->x2 = old1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwsclrpal") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwsclrpal x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWCLRSPAL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWCLRSPAL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWCLRSPAL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = old0;
  state->x2 = old1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwsetp") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwsetp x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWSETP", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSETP", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSETP", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = old0;
  state->x2 = old1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwsetpa") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwsetpa x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWSETPA", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSETPA", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSETPA", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = old0;
  state->x2 = old1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwsetpl") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwsetpl x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWSETPL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSETPL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSETPL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = old0;
  state->x2 = old1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwsetpal") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwsetpal x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWSETPAL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSETPAL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSETPAL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = old0;
  state->x2 = old1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwssetp") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwssetp x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWSETSP", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSETSP", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSETSP", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = old0;
  state->x2 = old1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwssetpa") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwssetpa x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWSETSPA", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSETSPA", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSETSPA", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = old0;
  state->x2 = old1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwssetpl") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwssetpl x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWSETSPL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSETSPL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSETSPL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = old0;
  state->x2 = old1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwssetpal") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwssetpal x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWSETSPAL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSETSPAL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSETSPAL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = old0;
  state->x2 = old1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwswpp") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwswpp x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWSWPP", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSWPP", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSWPP", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = old0;
  state->x2 = old1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwswppa") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwswppa x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWSWPPA", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSWPPA", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSWPPA", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = old0;
  state->x2 = old1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwswppl") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwswppl x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWSWPPL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSWPPL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSWPPL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = old0;
  state->x2 = old1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwswppal") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwswppal x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWSWPPAL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSWPPAL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSWPPAL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = old0;
  state->x2 = old1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwsswpp") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwsswpp x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWSWPSP", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSWPSP", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSWPSP", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = old0;
  state->x2 = old1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwsswppa") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwsswppa x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWSWPSPA", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSWPSPA", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSWPSPA", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = old0;
  state->x2 = old1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwsswppl") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwsswppl x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWSWPSPL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSWPSPL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSWPSPL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = old0;
  state->x2 = old1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-rcwsswppal") {
  if (!checkFeature("the") || !checkFeature("d128")) {
    return;
  }

  const char source[] = "rcwsswppal x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x1111111111111111ULL;
  constexpr uint64_t old1 = 0x2222222222222222ULL;
  constexpr uint64_t new0 = 0x3333333333333333ULL;
  constexpr uint64_t new1 = 0x4444444444444444ULL;
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
  vm.addMnemonicCB("RCWSWPSPAL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RCWSWPSPAL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("RCWSWPSPAL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = old0;
  state->x2 = old1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran =
      runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"the", "d128"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}
