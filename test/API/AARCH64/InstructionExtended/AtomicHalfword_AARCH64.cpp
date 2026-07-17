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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldaddh") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldaddh w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x3c11;
  constexpr uint16_t operand = 0xf0f;
  constexpr uint16_t newval = 0x4b20;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDADDH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDADDH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDADDH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldaddah") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldaddah w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x3c11;
  constexpr uint16_t operand = 0xf0f;
  constexpr uint16_t newval = 0x4b20;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDADDAH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDADDAH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDADDAH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldaddlh") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldaddlh w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x3c11;
  constexpr uint16_t operand = 0xf0f;
  constexpr uint16_t newval = 0x4b20;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDADDLH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDADDLH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDADDLH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldaddalh") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldaddalh w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x3c11;
  constexpr uint16_t operand = 0xf0f;
  constexpr uint16_t newval = 0x4b20;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDADDALH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDADDALH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDADDALH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldclrh") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldclrh w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x3c11;
  constexpr uint16_t operand = 0xf0f;
  constexpr uint16_t newval = 0x3010;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDCLRH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDCLRH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDCLRH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldclrah") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldclrah w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x3c11;
  constexpr uint16_t operand = 0xf0f;
  constexpr uint16_t newval = 0x3010;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDCLRAH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDCLRAH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDCLRAH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldclrlh") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldclrlh w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x3c11;
  constexpr uint16_t operand = 0xf0f;
  constexpr uint16_t newval = 0x3010;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDCLRLH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDCLRLH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDCLRLH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldclralh") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldclralh w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x3c11;
  constexpr uint16_t operand = 0xf0f;
  constexpr uint16_t newval = 0x3010;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDCLRALH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDCLRALH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDCLRALH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldeorh") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldeorh w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x3c11;
  constexpr uint16_t operand = 0xf0f;
  constexpr uint16_t newval = 0x331e;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDEORH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDEORH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDEORH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldeorah") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldeorah w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x3c11;
  constexpr uint16_t operand = 0xf0f;
  constexpr uint16_t newval = 0x331e;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDEORAH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDEORAH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDEORAH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldeorlh") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldeorlh w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x3c11;
  constexpr uint16_t operand = 0xf0f;
  constexpr uint16_t newval = 0x331e;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDEORLH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDEORLH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDEORLH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldeoralh") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldeoralh w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x3c11;
  constexpr uint16_t operand = 0xf0f;
  constexpr uint16_t newval = 0x331e;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDEORALH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDEORALH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDEORALH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldseth") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldseth w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x3c11;
  constexpr uint16_t operand = 0xf0f;
  constexpr uint16_t newval = 0x3f1f;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDSETH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDSETH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDSETH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldsetah") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldsetah w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x3c11;
  constexpr uint16_t operand = 0xf0f;
  constexpr uint16_t newval = 0x3f1f;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDSETAH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDSETAH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDSETAH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldsetlh") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldsetlh w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x3c11;
  constexpr uint16_t operand = 0xf0f;
  constexpr uint16_t newval = 0x3f1f;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDSETLH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDSETLH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDSETLH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldsetalh") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldsetalh w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x3c11;
  constexpr uint16_t operand = 0xf0f;
  constexpr uint16_t newval = 0x3f1f;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDSETALH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDSETALH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDSETALH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldsmaxh") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldsmaxh w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x8100;
  constexpr uint16_t operand = 0x2;
  constexpr uint16_t newval = 0x2;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDSMAXH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDSMAXH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDSMAXH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldsmaxah") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldsmaxah w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x8100;
  constexpr uint16_t operand = 0x2;
  constexpr uint16_t newval = 0x2;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDSMAXAH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDSMAXAH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDSMAXAH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldsmaxlh") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldsmaxlh w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x8100;
  constexpr uint16_t operand = 0x2;
  constexpr uint16_t newval = 0x2;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDSMAXLH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDSMAXLH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDSMAXLH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldsmaxalh") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldsmaxalh w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x8100;
  constexpr uint16_t operand = 0x2;
  constexpr uint16_t newval = 0x2;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDSMAXALH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDSMAXALH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDSMAXALH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldsminh") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldsminh w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x8100;
  constexpr uint16_t operand = 0x2;
  constexpr uint16_t newval = 0x8100;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDSMINH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDSMINH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDSMINH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldsminah") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldsminah w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x8100;
  constexpr uint16_t operand = 0x2;
  constexpr uint16_t newval = 0x8100;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDSMINAH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDSMINAH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDSMINAH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldsminlh") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldsminlh w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x8100;
  constexpr uint16_t operand = 0x2;
  constexpr uint16_t newval = 0x8100;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDSMINLH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDSMINLH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDSMINLH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldsminalh") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldsminalh w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x8100;
  constexpr uint16_t operand = 0x2;
  constexpr uint16_t newval = 0x8100;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDSMINALH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDSMINALH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDSMINALH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldumaxh") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldumaxh w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x100;
  constexpr uint16_t operand = 0x8002;
  constexpr uint16_t newval = 0x8002;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDUMAXH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDUMAXH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDUMAXH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldumaxah") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldumaxah w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x100;
  constexpr uint16_t operand = 0x8002;
  constexpr uint16_t newval = 0x8002;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDUMAXAH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDUMAXAH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDUMAXAH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldumaxlh") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldumaxlh w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x100;
  constexpr uint16_t operand = 0x8002;
  constexpr uint16_t newval = 0x8002;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDUMAXLH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDUMAXLH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDUMAXLH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldumaxalh") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldumaxalh w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x100;
  constexpr uint16_t operand = 0x8002;
  constexpr uint16_t newval = 0x8002;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDUMAXALH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDUMAXALH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDUMAXALH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-lduminh") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "lduminh w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x8002;
  constexpr uint16_t operand = 0x100;
  constexpr uint16_t newval = 0x100;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDUMINH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDUMINH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDUMINH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-lduminah") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "lduminah w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x8002;
  constexpr uint16_t operand = 0x100;
  constexpr uint16_t newval = 0x100;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDUMINAH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDUMINAH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDUMINAH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-lduminlh") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "lduminlh w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x8002;
  constexpr uint16_t operand = 0x100;
  constexpr uint16_t newval = 0x100;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDUMINLH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDUMINLH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDUMINLH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-lduminalh") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "lduminalh w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x8002;
  constexpr uint16_t operand = 0x100;
  constexpr uint16_t newval = 0x100;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDUMINALH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDUMINALH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDUMINALH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = operand;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-swph") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "swph w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x1234;
  constexpr uint16_t newval = 0x5678;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("SWPH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SWPH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("SWPH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-swpah") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "swpah w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x1234;
  constexpr uint16_t newval = 0x5678;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("SWPAH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SWPAH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("SWPAH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-swplh") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "swplh w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x1234;
  constexpr uint16_t newval = 0x5678;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("SWPLH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SWPLH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("SWPLH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-swpalh") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "swpalh w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x1234;
  constexpr uint16_t newval = 0x5678;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("SWPALH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SWPALH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("SWPALH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x2 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cash") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "cash w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x2222;
  constexpr uint16_t expected = 0x2222;
  constexpr uint16_t newval = 0x9999;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("CASH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("CASH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("CASH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = expected;
  state->x2 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-casah") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "casah w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x2222;
  constexpr uint16_t expected = 0x2222;
  constexpr uint16_t newval = 0x9999;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("CASAH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("CASAH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("CASAH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = expected;
  state->x2 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-caslh") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "caslh w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x2222;
  constexpr uint16_t expected = 0x2222;
  constexpr uint16_t newval = 0x9999;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("CASLH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("CASLH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("CASLH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = expected;
  state->x2 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-casalh") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "casalh w1, w2, [x0]\n";

  constexpr uint16_t oldval = 0x2222;
  constexpr uint16_t expected = 0x2222;
  constexpr uint16_t newval = 0x9999;
  uint16_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("CASALH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("CASALH", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("CASALH", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((gprState->x1 & 0xffff) == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v;
  state->x1 = expected;
  state->x2 = newval;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}
