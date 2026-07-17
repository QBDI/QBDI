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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfadd") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfadd s0, s1, [x2]\n";

  constexpr uint32_t oldval = 0x3fc00000;
  constexpr uint32_t operand = 0x40000000;
  constexpr uint32_t newval = 0x40600000;
  uint32_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDFADDS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFADDS", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFADDS", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v1 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x2 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = operand;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsfe"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfadda") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfadda s0, s1, [x2]\n";

  constexpr uint32_t oldval = 0x3fc00000;
  constexpr uint32_t operand = 0x40000000;
  constexpr uint32_t newval = 0x40600000;
  uint32_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDFADDAS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFADDAS", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFADDAS", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v1 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x2 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = operand;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsfe"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfaddl") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfaddl s0, s1, [x2]\n";

  constexpr uint32_t oldval = 0x3fc00000;
  constexpr uint32_t operand = 0x40000000;
  constexpr uint32_t newval = 0x40600000;
  uint32_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDFADDLS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFADDLS", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFADDLS", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v1 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x2 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = operand;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsfe"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfaddal") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfaddal s0, s1, [x2]\n";

  constexpr uint32_t oldval = 0x3fc00000;
  constexpr uint32_t operand = 0x40000000;
  constexpr uint32_t newval = 0x40600000;
  uint32_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDFADDALS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFADDALS", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFADDALS", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v1 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x2 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = operand;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsfe"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfmax") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfmax s0, s1, [x2]\n";

  constexpr uint32_t oldval = 0xc0800000;
  constexpr uint32_t operand = 0x40000000;
  constexpr uint32_t newval = 0x40000000;
  uint32_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDFMAXS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMAXS", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMAXS", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v1 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x2 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = operand;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsfe"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfmaxa") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfmaxa s0, s1, [x2]\n";

  constexpr uint32_t oldval = 0xc0800000;
  constexpr uint32_t operand = 0x40000000;
  constexpr uint32_t newval = 0x40000000;
  uint32_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDFMAXAS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMAXAS", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMAXAS", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v1 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x2 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = operand;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsfe"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfmaxl") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfmaxl s0, s1, [x2]\n";

  constexpr uint32_t oldval = 0xc0800000;
  constexpr uint32_t operand = 0x40000000;
  constexpr uint32_t newval = 0x40000000;
  uint32_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDFMAXLS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMAXLS", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMAXLS", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v1 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x2 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = operand;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsfe"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfmaxal") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfmaxal s0, s1, [x2]\n";

  constexpr uint32_t oldval = 0xc0800000;
  constexpr uint32_t operand = 0x40000000;
  constexpr uint32_t newval = 0x40000000;
  uint32_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDFMAXALS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMAXALS", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMAXALS", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v1 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x2 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = operand;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsfe"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfmaxnm") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfmaxnm s0, s1, [x2]\n";

  constexpr uint32_t oldval = 0xc0800000;
  constexpr uint32_t operand = 0x40000000;
  constexpr uint32_t newval = 0x40000000;
  uint32_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDFMAXNMS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMAXNMS", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMAXNMS", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v1 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x2 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = operand;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsfe"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfmaxnma") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfmaxnma s0, s1, [x2]\n";

  constexpr uint32_t oldval = 0xc0800000;
  constexpr uint32_t operand = 0x40000000;
  constexpr uint32_t newval = 0x40000000;
  uint32_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDFMAXNMAS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMAXNMAS", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMAXNMAS", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v1 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x2 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = operand;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsfe"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfmaxnml") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfmaxnml s0, s1, [x2]\n";

  constexpr uint32_t oldval = 0xc0800000;
  constexpr uint32_t operand = 0x40000000;
  constexpr uint32_t newval = 0x40000000;
  uint32_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDFMAXNMLS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMAXNMLS", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMAXNMLS", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v1 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x2 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = operand;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsfe"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfmaxnmal") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfmaxnmal s0, s1, [x2]\n";

  constexpr uint32_t oldval = 0xc0800000;
  constexpr uint32_t operand = 0x40000000;
  constexpr uint32_t newval = 0x40000000;
  uint32_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDFMAXNMALS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMAXNMALS", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMAXNMALS", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v1 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x2 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = operand;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsfe"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfmin") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfmin s0, s1, [x2]\n";

  constexpr uint32_t oldval = 0xc0800000;
  constexpr uint32_t operand = 0x40000000;
  constexpr uint32_t newval = 0xc0800000;
  uint32_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDFMINS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMINS", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMINS", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v1 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x2 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = operand;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsfe"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfmina") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfmina s0, s1, [x2]\n";

  constexpr uint32_t oldval = 0xc0800000;
  constexpr uint32_t operand = 0x40000000;
  constexpr uint32_t newval = 0xc0800000;
  uint32_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDFMINAS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMINAS", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMINAS", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v1 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x2 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = operand;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsfe"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfminl") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfminl s0, s1, [x2]\n";

  constexpr uint32_t oldval = 0xc0800000;
  constexpr uint32_t operand = 0x40000000;
  constexpr uint32_t newval = 0xc0800000;
  uint32_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDFMINLS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMINLS", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMINLS", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v1 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x2 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = operand;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsfe"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfminal") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfminal s0, s1, [x2]\n";

  constexpr uint32_t oldval = 0xc0800000;
  constexpr uint32_t operand = 0x40000000;
  constexpr uint32_t newval = 0xc0800000;
  uint32_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDFMINALS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMINALS", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMINALS", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v1 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x2 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = operand;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsfe"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfminnm") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfminnm s0, s1, [x2]\n";

  constexpr uint32_t oldval = 0xc0800000;
  constexpr uint32_t operand = 0x40000000;
  constexpr uint32_t newval = 0xc0800000;
  uint32_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDFMINNMS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMINNMS", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMINNMS", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v1 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x2 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = operand;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsfe"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfminnma") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfminnma s0, s1, [x2]\n";

  constexpr uint32_t oldval = 0xc0800000;
  constexpr uint32_t operand = 0x40000000;
  constexpr uint32_t newval = 0xc0800000;
  uint32_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDFMINNMAS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMINNMAS", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMINNMAS", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v1 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x2 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = operand;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsfe"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfminnml") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfminnml s0, s1, [x2]\n";

  constexpr uint32_t oldval = 0xc0800000;
  constexpr uint32_t operand = 0x40000000;
  constexpr uint32_t newval = 0xc0800000;
  uint32_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDFMINNMLS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMINNMLS", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMINNMLS", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v1 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x2 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = operand;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsfe"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfminnmal") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfminnmal s0, s1, [x2]\n";

  constexpr uint32_t oldval = 0xc0800000;
  constexpr uint32_t operand = 0x40000000;
  constexpr uint32_t newval = 0xc0800000;
  uint32_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDFMINNMALS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMINNMALS", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMINNMALS", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK((uint32_t)fprState->v1 == oldval);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x2 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = operand;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsfe"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stfadd") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "stfadd s0, [x2]\n";

  constexpr uint32_t oldval = 0x3fc00000;
  constexpr uint32_t operand = 0x40000000;
  constexpr uint32_t newval = 0x40600000;
  uint32_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STFADDS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STFADDS", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x2 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = operand;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsfe"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stfaddl") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "stfaddl s0, [x2]\n";

  constexpr uint32_t oldval = 0x3fc00000;
  constexpr uint32_t operand = 0x40000000;
  constexpr uint32_t newval = 0x40600000;
  uint32_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STFADDLS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STFADDLS", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x2 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = operand;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsfe"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stfmax") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "stfmax s0, [x2]\n";

  constexpr uint32_t oldval = 0xc0800000;
  constexpr uint32_t operand = 0x40000000;
  constexpr uint32_t newval = 0x40000000;
  uint32_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STFMAXS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STFMAXS", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x2 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = operand;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsfe"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stfmaxl") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "stfmaxl s0, [x2]\n";

  constexpr uint32_t oldval = 0xc0800000;
  constexpr uint32_t operand = 0x40000000;
  constexpr uint32_t newval = 0x40000000;
  uint32_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STFMAXLS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STFMAXLS", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x2 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = operand;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsfe"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stfmaxnm") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "stfmaxnm s0, [x2]\n";

  constexpr uint32_t oldval = 0xc0800000;
  constexpr uint32_t operand = 0x40000000;
  constexpr uint32_t newval = 0x40000000;
  uint32_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STFMAXNMS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STFMAXNMS", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x2 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = operand;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsfe"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stfmaxnml") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "stfmaxnml s0, [x2]\n";

  constexpr uint32_t oldval = 0xc0800000;
  constexpr uint32_t operand = 0x40000000;
  constexpr uint32_t newval = 0x40000000;
  uint32_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STFMAXNMLS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STFMAXNMLS", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x2 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = operand;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsfe"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stfmin") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "stfmin s0, [x2]\n";

  constexpr uint32_t oldval = 0xc0800000;
  constexpr uint32_t operand = 0x40000000;
  constexpr uint32_t newval = 0xc0800000;
  uint32_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STFMINS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STFMINS", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x2 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = operand;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsfe"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stfminl") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "stfminl s0, [x2]\n";

  constexpr uint32_t oldval = 0xc0800000;
  constexpr uint32_t operand = 0x40000000;
  constexpr uint32_t newval = 0xc0800000;
  uint32_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STFMINLS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STFMINLS", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x2 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = operand;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsfe"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stfminnm") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "stfminnm s0, [x2]\n";

  constexpr uint32_t oldval = 0xc0800000;
  constexpr uint32_t operand = 0x40000000;
  constexpr uint32_t newval = 0xc0800000;
  uint32_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STFMINNMS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STFMINNMS", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x2 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = operand;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsfe"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stfminnml") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "stfminnml s0, [x2]\n";

  constexpr uint32_t oldval = 0xc0800000;
  constexpr uint32_t operand = 0x40000000;
  constexpr uint32_t newval = 0xc0800000;
  uint32_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STFMINNMLS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STFMINNMLS", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x2 = (QBDI::rword)&v;
  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v0 = operand;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsfe"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(v == newval);
}
