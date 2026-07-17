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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldadd_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldadd x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0x4b20a920ffffffffULL;
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
  vm.addMnemonicCB("LDADDX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDADDX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDADDX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldadda_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldadda x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0x4b20a920ffffffffULL;
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
  vm.addMnemonicCB("LDADDAX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDADDAX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDADDAX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldaddl_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldaddl x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0x4b20a920ffffffffULL;
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
  vm.addMnemonicCB("LDADDLX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDADDLX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDADDLX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldaddal_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldaddal x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0x4b20a920ffffffffULL;
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
  vm.addMnemonicCB("LDADDALX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDADDALX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDADDALX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldclr_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldclr x1, x2, [x0]\n";

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
  vm.addMnemonicCB("LDCLRX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDCLRX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDCLRX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldclra_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldclra x1, x2, [x0]\n";

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
  vm.addMnemonicCB("LDCLRAX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDCLRAX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDCLRAX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldclrl_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldclrl x1, x2, [x0]\n";

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
  vm.addMnemonicCB("LDCLRLX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDCLRLX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDCLRLX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldclral_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldclral x1, x2, [x0]\n";

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
  vm.addMnemonicCB("LDCLRALX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDCLRALX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDCLRALX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldeor_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldeor x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0x331e951effffffffULL;
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
  vm.addMnemonicCB("LDEORX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDEORX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDEORX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldeora_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldeora x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0x331e951effffffffULL;
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
  vm.addMnemonicCB("LDEORAX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDEORAX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDEORAX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldeorl_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldeorl x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0x331e951effffffffULL;
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
  vm.addMnemonicCB("LDEORLX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDEORLX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDEORLX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldeoral_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldeoral x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x3c119a11f0f0f0f0ULL;
  constexpr uint64_t operand = 0xf0f0f0f0f0f0f0fULL;
  constexpr uint64_t newval = 0x331e951effffffffULL;
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
  vm.addMnemonicCB("LDEORALX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDEORALX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDEORALX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldset_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldset x1, x2, [x0]\n";

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
  vm.addMnemonicCB("LDSETX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDSETX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDSETX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldseta_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldseta x1, x2, [x0]\n";

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
  vm.addMnemonicCB("LDSETAX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDSETAX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDSETAX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldsetl_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldsetl x1, x2, [x0]\n";

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
  vm.addMnemonicCB("LDSETLX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDSETLX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDSETLX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldsetal_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldsetal x1, x2, [x0]\n";

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
  vm.addMnemonicCB("LDSETALX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDSETALX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDSETALX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldsmax_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldsmax x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x8100000000000000ULL;
  constexpr uint64_t operand = 0x2ULL;
  constexpr uint64_t newval = 0x2ULL;
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
  vm.addMnemonicCB("LDSMAXX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDSMAXX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDSMAXX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldsmaxa_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldsmaxa x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x8100000000000000ULL;
  constexpr uint64_t operand = 0x2ULL;
  constexpr uint64_t newval = 0x2ULL;
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
  vm.addMnemonicCB("LDSMAXAX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDSMAXAX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDSMAXAX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldsmaxl_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldsmaxl x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x8100000000000000ULL;
  constexpr uint64_t operand = 0x2ULL;
  constexpr uint64_t newval = 0x2ULL;
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
  vm.addMnemonicCB("LDSMAXLX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDSMAXLX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDSMAXLX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldsmaxal_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldsmaxal x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x8100000000000000ULL;
  constexpr uint64_t operand = 0x2ULL;
  constexpr uint64_t newval = 0x2ULL;
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
  vm.addMnemonicCB("LDSMAXALX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDSMAXALX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDSMAXALX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldsmin_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldsmin x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x8100000000000000ULL;
  constexpr uint64_t operand = 0x2ULL;
  constexpr uint64_t newval = 0x8100000000000000ULL;
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
  vm.addMnemonicCB("LDSMINX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDSMINX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDSMINX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldsmina_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldsmina x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x8100000000000000ULL;
  constexpr uint64_t operand = 0x2ULL;
  constexpr uint64_t newval = 0x8100000000000000ULL;
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
  vm.addMnemonicCB("LDSMINAX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDSMINAX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDSMINAX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldsminl_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldsminl x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x8100000000000000ULL;
  constexpr uint64_t operand = 0x2ULL;
  constexpr uint64_t newval = 0x8100000000000000ULL;
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
  vm.addMnemonicCB("LDSMINLX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDSMINLX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDSMINLX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldsminal_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldsminal x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x8100000000000000ULL;
  constexpr uint64_t operand = 0x2ULL;
  constexpr uint64_t newval = 0x8100000000000000ULL;
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
  vm.addMnemonicCB("LDSMINALX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDSMINALX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDSMINALX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldumax_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldumax x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x100ULL;
  constexpr uint64_t operand = 0x8000000000000002ULL;
  constexpr uint64_t newval = 0x8000000000000002ULL;
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
  vm.addMnemonicCB("LDUMAXX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDUMAXX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDUMAXX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldumaxa_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldumaxa x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x100ULL;
  constexpr uint64_t operand = 0x8000000000000002ULL;
  constexpr uint64_t newval = 0x8000000000000002ULL;
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
  vm.addMnemonicCB("LDUMAXAX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDUMAXAX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDUMAXAX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldumaxl_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldumaxl x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x100ULL;
  constexpr uint64_t operand = 0x8000000000000002ULL;
  constexpr uint64_t newval = 0x8000000000000002ULL;
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
  vm.addMnemonicCB("LDUMAXLX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDUMAXLX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDUMAXLX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldumaxal_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldumaxal x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x100ULL;
  constexpr uint64_t operand = 0x8000000000000002ULL;
  constexpr uint64_t newval = 0x8000000000000002ULL;
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
  vm.addMnemonicCB("LDUMAXALX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDUMAXALX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDUMAXALX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldumin_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldumin x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x8000000000000002ULL;
  constexpr uint64_t operand = 0x100ULL;
  constexpr uint64_t newval = 0x100ULL;
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
  vm.addMnemonicCB("LDUMINX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDUMINX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDUMINX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldumina_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldumina x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x8000000000000002ULL;
  constexpr uint64_t operand = 0x100ULL;
  constexpr uint64_t newval = 0x100ULL;
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
  vm.addMnemonicCB("LDUMINAX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDUMINAX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDUMINAX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-lduminl_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "lduminl x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x8000000000000002ULL;
  constexpr uint64_t operand = 0x100ULL;
  constexpr uint64_t newval = 0x100ULL;
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
  vm.addMnemonicCB("LDUMINLX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDUMINLX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDUMINLX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-lduminal_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "lduminal x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x8000000000000002ULL;
  constexpr uint64_t operand = 0x100ULL;
  constexpr uint64_t newval = 0x100ULL;
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
  vm.addMnemonicCB("LDUMINALX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDUMINALX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDUMINALX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-swpa_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "swpa x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x123456789abcdef0ULL;
  constexpr uint64_t newval = 0x876543211a2b3c4dULL;
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
  vm.addMnemonicCB("SWPAX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SWPAX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("SWPAX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == oldval);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-swpl_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "swpl x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x123456789abcdef0ULL;
  constexpr uint64_t newval = 0x876543211a2b3c4dULL;
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
  vm.addMnemonicCB("SWPLX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SWPLX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("SWPLX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == oldval);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-swpal_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "swpal x1, x2, [x0]\n";

  constexpr uint64_t oldval = 0x123456789abcdef0ULL;
  constexpr uint64_t newval = 0x876543211a2b3c4dULL;
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
  vm.addMnemonicCB("SWPALX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SWPALX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("SWPALX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x2 == oldval);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-cas_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "cas x1, x2, [x0]\n";

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
  vm.addMnemonicCB("CASX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("CASX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("CASX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-casa_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "casa x1, x2, [x0]\n";

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
  vm.addMnemonicCB("CASAX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("CASAX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("CASAX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-casl_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "casl x1, x2, [x0]\n";

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
  vm.addMnemonicCB("CASLX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("CASLX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("CASLX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-casal_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "casal x1, x2, [x0]\n";

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
  vm.addMnemonicCB("CASALX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("CASALX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("CASALX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v == newval);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-casp_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "casp x2, x3, x4, x5, [x0]\n";

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
  vm.addMnemonicCB("CASPX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("CASPX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("CASPX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-caspa_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "caspa x2, x3, x4, x5, [x0]\n";

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
  vm.addMnemonicCB("CASPAX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("CASPAX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("CASPAX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-caspl_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "caspl x2, x3, x4, x5, [x0]\n";

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
  vm.addMnemonicCB("CASPLX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("CASPLX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("CASPLX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-caspal_x") {
  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "caspal x2, x3, x4, x5, [x0]\n";

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
  vm.addMnemonicCB("CASPALX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("CASPALX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("CASPALX", QBDI::POSTINST,
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
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(v[0] == new0);
  CHECK(v[1] == new1);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-swpp") {
  if (!checkFeature("lse128")) {
    return;
  }

  const char source[] = "swpp x1, x2, [x0]\n";

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
  vm.addMnemonicCB("SWPP", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SWPP", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("SWPP", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = new0;
  state->x2 = new1;
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-swppa") {
  if (!checkFeature("lse128")) {
    return;
  }

  const char source[] = "swppa x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x5555555555555555ULL;
  constexpr uint64_t old1 = 0x6666666666666666ULL;
  constexpr uint64_t new0 = 0x7777777777777777ULL;
  constexpr uint64_t new1 = 0x8888888888888888ULL;
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
  vm.addMnemonicCB("SWPPA", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SWPPA", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("SWPPA", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = new0;
  state->x2 = new1;
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-swppl") {
  if (!checkFeature("lse128")) {
    return;
  }

  const char source[] = "swppl x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0x9999999999999999ULL;
  constexpr uint64_t old1 = 0xaaaaaaaaaaaaaaaaULL;
  constexpr uint64_t new0 = 0xbbbbbbbbbbbbbbbbULL;
  constexpr uint64_t new1 = 0xccccccccccccccccULL;
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
  vm.addMnemonicCB("SWPPL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SWPPL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("SWPPL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = new0;
  state->x2 = new1;
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-swppal") {
  if (!checkFeature("lse128")) {
    return;
  }

  const char source[] = "swppal x1, x2, [x0]\n";

  constexpr uint64_t old0 = 0xddddddddddddddddULL;
  constexpr uint64_t old1 = 0xeeeeeeeeeeeeeeeeULL;
  constexpr uint64_t new0 = 0xffffffffffffffffULL;
  constexpr uint64_t new1 = 0x0123456789abcdefULL;
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
  vm.addMnemonicCB("SWPPAL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SWPPAL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("SWPPAL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == old0);
                     CHECK(gprState->x2 == old1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&v[0];
  state->x1 = new0;
  state->x2 = new1;
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
