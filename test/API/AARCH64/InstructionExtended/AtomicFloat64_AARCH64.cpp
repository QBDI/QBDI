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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfadd_d") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfadd d0, d1, [x2]\n";

  constexpr uint64_t oldval = 0x3ff8000000000000ULL;
  constexpr uint64_t operand = 0x4000000000000000ULL;
  constexpr uint64_t newval = 0x400c000000000000ULL;
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
  vm.addMnemonicCB("LDFADDD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFADDD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFADDD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == oldval);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfadda_d") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfadda d0, d1, [x2]\n";

  constexpr uint64_t oldval = 0x3ff8000000000000ULL;
  constexpr uint64_t operand = 0x4000000000000000ULL;
  constexpr uint64_t newval = 0x400c000000000000ULL;
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
  vm.addMnemonicCB("LDFADDAD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFADDAD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFADDAD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == oldval);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfaddl_d") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfaddl d0, d1, [x2]\n";

  constexpr uint64_t oldval = 0x3ff8000000000000ULL;
  constexpr uint64_t operand = 0x4000000000000000ULL;
  constexpr uint64_t newval = 0x400c000000000000ULL;
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
  vm.addMnemonicCB("LDFADDLD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFADDLD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFADDLD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == oldval);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfaddal_d") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfaddal d0, d1, [x2]\n";

  constexpr uint64_t oldval = 0x3ff8000000000000ULL;
  constexpr uint64_t operand = 0x4000000000000000ULL;
  constexpr uint64_t newval = 0x400c000000000000ULL;
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
  vm.addMnemonicCB("LDFADDALD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFADDALD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFADDALD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == oldval);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfmax_d") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfmax d0, d1, [x2]\n";

  constexpr uint64_t oldval = 0xc010000000000000ULL;
  constexpr uint64_t operand = 0x4000000000000000ULL;
  constexpr uint64_t newval = 0x4000000000000000ULL;
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
  vm.addMnemonicCB("LDFMAXD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMAXD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMAXD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == oldval);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfmaxa_d") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfmaxa d0, d1, [x2]\n";

  constexpr uint64_t oldval = 0xc010000000000000ULL;
  constexpr uint64_t operand = 0x4000000000000000ULL;
  constexpr uint64_t newval = 0x4000000000000000ULL;
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
  vm.addMnemonicCB("LDFMAXAD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMAXAD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMAXAD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == oldval);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfmaxl_d") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfmaxl d0, d1, [x2]\n";

  constexpr uint64_t oldval = 0xc010000000000000ULL;
  constexpr uint64_t operand = 0x4000000000000000ULL;
  constexpr uint64_t newval = 0x4000000000000000ULL;
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
  vm.addMnemonicCB("LDFMAXLD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMAXLD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMAXLD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == oldval);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfmaxal_d") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfmaxal d0, d1, [x2]\n";

  constexpr uint64_t oldval = 0xc010000000000000ULL;
  constexpr uint64_t operand = 0x4000000000000000ULL;
  constexpr uint64_t newval = 0x4000000000000000ULL;
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
  vm.addMnemonicCB("LDFMAXALD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMAXALD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMAXALD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == oldval);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfmaxnm_d") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfmaxnm d0, d1, [x2]\n";

  constexpr uint64_t oldval = 0xc010000000000000ULL;
  constexpr uint64_t operand = 0x4000000000000000ULL;
  constexpr uint64_t newval = 0x4000000000000000ULL;
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
  vm.addMnemonicCB("LDFMAXNMD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMAXNMD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMAXNMD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == oldval);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfmaxnma_d") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfmaxnma d0, d1, [x2]\n";

  constexpr uint64_t oldval = 0xc010000000000000ULL;
  constexpr uint64_t operand = 0x4000000000000000ULL;
  constexpr uint64_t newval = 0x4000000000000000ULL;
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
  vm.addMnemonicCB("LDFMAXNMAD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMAXNMAD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMAXNMAD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == oldval);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfmaxnml_d") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfmaxnml d0, d1, [x2]\n";

  constexpr uint64_t oldval = 0xc010000000000000ULL;
  constexpr uint64_t operand = 0x4000000000000000ULL;
  constexpr uint64_t newval = 0x4000000000000000ULL;
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
  vm.addMnemonicCB("LDFMAXNMLD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMAXNMLD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMAXNMLD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == oldval);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfmaxnmal_d") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfmaxnmal d0, d1, [x2]\n";

  constexpr uint64_t oldval = 0xc010000000000000ULL;
  constexpr uint64_t operand = 0x4000000000000000ULL;
  constexpr uint64_t newval = 0x4000000000000000ULL;
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
  vm.addMnemonicCB("LDFMAXNMALD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMAXNMALD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMAXNMALD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == oldval);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfmin_d") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfmin d0, d1, [x2]\n";

  constexpr uint64_t oldval = 0xc010000000000000ULL;
  constexpr uint64_t operand = 0x4000000000000000ULL;
  constexpr uint64_t newval = 0xc010000000000000ULL;
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
  vm.addMnemonicCB("LDFMIND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMIND", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMIND", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == oldval);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfmina_d") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfmina d0, d1, [x2]\n";

  constexpr uint64_t oldval = 0xc010000000000000ULL;
  constexpr uint64_t operand = 0x4000000000000000ULL;
  constexpr uint64_t newval = 0xc010000000000000ULL;
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
  vm.addMnemonicCB("LDFMINAD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMINAD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMINAD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == oldval);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfminl_d") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfminl d0, d1, [x2]\n";

  constexpr uint64_t oldval = 0xc010000000000000ULL;
  constexpr uint64_t operand = 0x4000000000000000ULL;
  constexpr uint64_t newval = 0xc010000000000000ULL;
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
  vm.addMnemonicCB("LDFMINLD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMINLD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMINLD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == oldval);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfminal_d") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfminal d0, d1, [x2]\n";

  constexpr uint64_t oldval = 0xc010000000000000ULL;
  constexpr uint64_t operand = 0x4000000000000000ULL;
  constexpr uint64_t newval = 0xc010000000000000ULL;
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
  vm.addMnemonicCB("LDFMINALD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMINALD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMINALD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == oldval);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfminnm_d") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfminnm d0, d1, [x2]\n";

  constexpr uint64_t oldval = 0xc010000000000000ULL;
  constexpr uint64_t operand = 0x4000000000000000ULL;
  constexpr uint64_t newval = 0xc010000000000000ULL;
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
  vm.addMnemonicCB("LDFMINNMD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMINNMD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMINNMD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == oldval);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfminnma_d") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfminnma d0, d1, [x2]\n";

  constexpr uint64_t oldval = 0xc010000000000000ULL;
  constexpr uint64_t operand = 0x4000000000000000ULL;
  constexpr uint64_t newval = 0xc010000000000000ULL;
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
  vm.addMnemonicCB("LDFMINNMAD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMINNMAD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMINNMAD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == oldval);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfminnml_d") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfminnml d0, d1, [x2]\n";

  constexpr uint64_t oldval = 0xc010000000000000ULL;
  constexpr uint64_t operand = 0x4000000000000000ULL;
  constexpr uint64_t newval = 0xc010000000000000ULL;
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
  vm.addMnemonicCB("LDFMINNMLD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMINNMLD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMINNMLD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == oldval);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldfminnmal_d") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "ldfminnmal d0, d1, [x2]\n";

  constexpr uint64_t oldval = 0xc010000000000000ULL;
  constexpr uint64_t operand = 0x4000000000000000ULL;
  constexpr uint64_t newval = 0xc010000000000000ULL;
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
  vm.addMnemonicCB("LDFMINNMALD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDFMINNMALD", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDFMINNMALD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == oldval);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stfadd_d") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "stfadd d0, [x2]\n";

  constexpr uint64_t oldval = 0x3ff8000000000000ULL;
  constexpr uint64_t operand = 0x4000000000000000ULL;
  constexpr uint64_t newval = 0x400c000000000000ULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STFADDD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STFADDD", QBDI::POSTINST, checkAccess, &expectedPost);

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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stfaddl_d") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "stfaddl d0, [x2]\n";

  constexpr uint64_t oldval = 0x3ff8000000000000ULL;
  constexpr uint64_t operand = 0x4000000000000000ULL;
  constexpr uint64_t newval = 0x400c000000000000ULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STFADDLD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STFADDLD", QBDI::POSTINST, checkAccess, &expectedPost);

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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stfmax_d") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "stfmax d0, [x2]\n";

  constexpr uint64_t oldval = 0xc010000000000000ULL;
  constexpr uint64_t operand = 0x4000000000000000ULL;
  constexpr uint64_t newval = 0x4000000000000000ULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STFMAXD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STFMAXD", QBDI::POSTINST, checkAccess, &expectedPost);

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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stfmaxl_d") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "stfmaxl d0, [x2]\n";

  constexpr uint64_t oldval = 0xc010000000000000ULL;
  constexpr uint64_t operand = 0x4000000000000000ULL;
  constexpr uint64_t newval = 0x4000000000000000ULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STFMAXLD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STFMAXLD", QBDI::POSTINST, checkAccess, &expectedPost);

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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stfmaxnm_d") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "stfmaxnm d0, [x2]\n";

  constexpr uint64_t oldval = 0xc010000000000000ULL;
  constexpr uint64_t operand = 0x4000000000000000ULL;
  constexpr uint64_t newval = 0x4000000000000000ULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STFMAXNMD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STFMAXNMD", QBDI::POSTINST, checkAccess, &expectedPost);

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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stfmaxnml_d") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "stfmaxnml d0, [x2]\n";

  constexpr uint64_t oldval = 0xc010000000000000ULL;
  constexpr uint64_t operand = 0x4000000000000000ULL;
  constexpr uint64_t newval = 0x4000000000000000ULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STFMAXNMLD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STFMAXNMLD", QBDI::POSTINST, checkAccess, &expectedPost);

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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stfmin_d") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "stfmin d0, [x2]\n";

  constexpr uint64_t oldval = 0xc010000000000000ULL;
  constexpr uint64_t operand = 0x4000000000000000ULL;
  constexpr uint64_t newval = 0xc010000000000000ULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STFMIND", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STFMIND", QBDI::POSTINST, checkAccess, &expectedPost);

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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stfminl_d") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "stfminl d0, [x2]\n";

  constexpr uint64_t oldval = 0xc010000000000000ULL;
  constexpr uint64_t operand = 0x4000000000000000ULL;
  constexpr uint64_t newval = 0xc010000000000000ULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STFMINLD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STFMINLD", QBDI::POSTINST, checkAccess, &expectedPost);

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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stfminnm_d") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "stfminnm d0, [x2]\n";

  constexpr uint64_t oldval = 0xc010000000000000ULL;
  constexpr uint64_t operand = 0x4000000000000000ULL;
  constexpr uint64_t newval = 0xc010000000000000ULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STFMINNMD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STFMINNMD", QBDI::POSTINST, checkAccess, &expectedPost);

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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stfminnml_d") {
  if (!checkFeature("lsfe")) {
    return;
  }

  const char source[] = "stfminnml d0, [x2]\n";

  constexpr uint64_t oldval = 0xc010000000000000ULL;
  constexpr uint64_t operand = 0x4000000000000000ULL;
  constexpr uint64_t newval = 0xc010000000000000ULL;
  uint64_t v = oldval;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, oldval, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, newval, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STFMINNMLD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STFMINNMLD", QBDI::POSTINST, checkAccess, &expectedPost);

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
