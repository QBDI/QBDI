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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldnpdi") {
  const char source[] = "ldnp d1, d2, [x0, #16]\n";

  constexpr uint64_t val0 = 0x1122334455667788ULL;
  constexpr uint64_t val1 = 0x99aabbccddeeff00ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[16] = val0;
  *(uint64_t *)&buf[24] = val1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[16], val0, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], val1, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], val0, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], val1, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDNPDi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDNPDi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDNPDi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == val0);
                     CHECK(fprState->v2 == val1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldnpxi") {
  const char source[] = "ldnp x1, x2, [x0, #16]\n";

  constexpr uint64_t val0 = 0x1122334455667788ULL;
  constexpr uint64_t val1 = 0x99aabbccddeeff00ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[16] = val0;
  *(uint64_t *)&buf[24] = val1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[16], val0, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], val1, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], val0, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], val1, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDNPXi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDNPXi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDNPXi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == val0);
                     CHECK(gprState->x2 == val1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldpdi") {
  const char source[] = "ldp d1, d2, [x0, #16]\n";

  constexpr uint64_t val0 = 0x1122334455667788ULL;
  constexpr uint64_t val1 = 0x99aabbccddeeff00ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[16] = val0;
  *(uint64_t *)&buf[24] = val1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[16], val0, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], val1, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], val0, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], val1, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDPDi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDPDi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDPDi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == val0);
                     CHECK(fprState->v2 == val1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldpdpost") {
  const char source[] = "ldp d1, d2, [x0], #16\n";

  constexpr uint64_t val0 = 0x1122334455667788ULL;
  constexpr uint64_t val1 = 0x99aabbccddeeff00ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = val0;
  *(uint64_t *)&buf[8] = val1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], val0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], val1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], val0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], val1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDPDpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDPDpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDPDpost", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == val0);
                     CHECK(fprState->v2 == val1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldpdpre") {
  const char source[] = "ldp d1, d2, [x0, #16]!\n";

  constexpr uint64_t val0 = 0x1122334455667788ULL;
  constexpr uint64_t val1 = 0x99aabbccddeeff00ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[16] = val0;
  *(uint64_t *)&buf[24] = val1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[16], val0, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], val1, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], val0, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], val1, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDPDpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDPDpre", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDPDpre", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(fprState->v1 == val0);
                     CHECK(fprState->v2 == val1);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldtpi") {
  if (!checkFeature("lsui")) {
    return;
  }

  const char source[] = "ldtp x1, x2, [x0, #16]\n";

  constexpr uint64_t val0 = 0x1122334455667788ULL;
  constexpr uint64_t val1 = 0x99aabbccddeeff00ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[16] = val0;
  *(uint64_t *)&buf[24] = val1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[16], val0, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], val1, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], val0, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], val1, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDTPi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDTPi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDTPi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == val0);
                     CHECK(gprState->x2 == val1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsui"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldtppost") {
  if (!checkFeature("lsui")) {
    return;
  }

  const char source[] = "ldtp x1, x2, [x0], #16\n";

  constexpr uint64_t val0 = 0x1122334455667788ULL;
  constexpr uint64_t val1 = 0x99aabbccddeeff00ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[0] = val0;
  *(uint64_t *)&buf[8] = val1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[0], val0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], val1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], val0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], val1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDTPpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDTPpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDTPpost", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == val0);
                     CHECK(gprState->x2 == val1);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 16);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsui"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldtppre") {
  if (!checkFeature("lsui")) {
    return;
  }

  const char source[] = "ldtp x1, x2, [x0, #16]!\n";

  constexpr uint64_t val0 = 0x1122334455667788ULL;
  constexpr uint64_t val1 = 0x99aabbccddeeff00ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[16] = val0;
  *(uint64_t *)&buf[24] = val1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[16], val0, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], val1, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], val0, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], val1, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDTPpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDTPpre", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDTPpre", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == val0);
                     CHECK(gprState->x2 == val1);
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 16);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsui"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stnpdi") {
  const char source[] = "stnp d1, d2, [x0, #16]\n";

  constexpr uint64_t val0 = 0x1122334455667788ULL;
  constexpr uint64_t val1 = 0x99aabbccddeeff00ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], val0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], val1, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STNPDi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STNPDi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];

  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = val0;
  fpr->v2 = val1;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[16] == val0);
  CHECK(*(uint64_t *)&buf[24] == val1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stnpxi") {
  const char source[] = "stnp x1, x2, [x0, #16]\n";

  constexpr uint64_t val0 = 0x1122334455667788ULL;
  constexpr uint64_t val1 = 0x99aabbccddeeff00ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], val0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], val1, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STNPXi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STNPXi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x1 = val0;
  state->x2 = val1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[16] == val0);
  CHECK(*(uint64_t *)&buf[24] == val1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stpdi") {
  const char source[] = "stp d1, d2, [x0, #16]\n";

  constexpr uint64_t val0 = 0x1122334455667788ULL;
  constexpr uint64_t val1 = 0x99aabbccddeeff00ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], val0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], val1, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STPDi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STPDi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];

  vm.setGPRState(state);
  QBDI::FPRState *fpr = vm.getFPRState();
  fpr->v1 = val0;
  fpr->v2 = val1;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[16] == val0);
  CHECK(*(uint64_t *)&buf[24] == val1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stpdpost") {
  const char source[] = "stp d1, d2, [x0], #16\n";

  constexpr uint64_t val0 = 0x1122334455667788ULL;
  constexpr uint64_t val1 = 0x99aabbccddeeff00ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], val0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], val1, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STPDpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STPDpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STPDpost", QBDI::POSTINST,
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
  fpr->v1 = val0;
  fpr->v2 = val1;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == val0);
  CHECK(*(uint64_t *)&buf[8] == val1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stpdpre") {
  const char source[] = "stp d1, d2, [x0, #16]!\n";

  constexpr uint64_t val0 = 0x1122334455667788ULL;
  constexpr uint64_t val1 = 0x99aabbccddeeff00ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], val0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], val1, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STPDpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STPDpre", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STPDpre", QBDI::POSTINST,
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
  fpr->v1 = val0;
  fpr->v2 = val1;
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[16] == val0);
  CHECK(*(uint64_t *)&buf[24] == val1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stpxi") {
  const char source[] = "stp x1, x2, [x0, #16]\n";

  constexpr uint64_t val0 = 0x1122334455667788ULL;
  constexpr uint64_t val1 = 0x99aabbccddeeff00ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], val0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], val1, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STPXi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STPXi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x1 = val0;
  state->x2 = val1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[16] == val0);
  CHECK(*(uint64_t *)&buf[24] == val1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stpxpost") {
  const char source[] = "stp x1, x2, [x0], #16\n";

  constexpr uint64_t val0 = 0x1122334455667788ULL;
  constexpr uint64_t val1 = 0x99aabbccddeeff00ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], val0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], val1, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STPXpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STPXpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STPXpost", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 16);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x1 = val0;
  state->x2 = val1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == val0);
  CHECK(*(uint64_t *)&buf[8] == val1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-stpxpre") {
  const char source[] = "stp x1, x2, [x0, #16]!\n";

  constexpr uint64_t val0 = 0x1122334455667788ULL;
  constexpr uint64_t val1 = 0x99aabbccddeeff00ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], val0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], val1, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STPXpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STPXpre", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STPXpre", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 16);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x1 = val0;
  state->x2 = val1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[16] == val0);
  CHECK(*(uint64_t *)&buf[24] == val1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-sttpi") {
  if (!checkFeature("lsui")) {
    return;
  }

  const char source[] = "sttp x1, x2, [x0, #16]\n";

  constexpr uint64_t val0 = 0x1122334455667788ULL;
  constexpr uint64_t val1 = 0x99aabbccddeeff00ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], val0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], val1, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STTPi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STTPi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x1 = val0;
  state->x2 = val1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsui"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[16] == val0);
  CHECK(*(uint64_t *)&buf[24] == val1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-sttppost") {
  if (!checkFeature("lsui")) {
    return;
  }

  const char source[] = "sttp x1, x2, [x0], #16\n";

  constexpr uint64_t val0 = 0x1122334455667788ULL;
  constexpr uint64_t val1 = 0x99aabbccddeeff00ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], val0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[8], val1, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STTPpost", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STTPpost", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STTPpost", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 16);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x1 = val0;
  state->x2 = val1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsui"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[0] == val0);
  CHECK(*(uint64_t *)&buf[8] == val1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-sttppre") {
  if (!checkFeature("lsui")) {
    return;
  }

  const char source[] = "sttp x1, x2, [x0, #16]!\n";

  constexpr uint64_t val0 = 0x1122334455667788ULL;
  constexpr uint64_t val1 = 0x99aabbccddeeff00ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], val0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], val1, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("STTPpre", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STTPpre", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STTPpre", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x0 == (QBDI::rword)&buf[0] + 16);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x1 = val0;
  state->x2 = val1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsui"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
  CHECK(*(uint64_t *)&buf[16] == val0);
  CHECK(*(uint64_t *)&buf[24] == val1);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-ldtnpxi") {
  if (!checkFeature("lsui")) {
    return;
  }

  const char source[] = "ldtnp x1, x2, [x0, #16]\n";

  constexpr uint64_t val0 = 0x1122334455667788ULL;
  constexpr uint64_t val1 = 0x99aabbccddeeff00ULL;
  uint8_t buf[40] = {0};
  *(uint64_t *)&buf[16] = val0;
  *(uint64_t *)&buf[24] = val1;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&buf[16], val0, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], val1, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], val0, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], val1, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  bool seenPost = false;
  vm.addMnemonicCB("LDTNPXi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDTNPXi", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("LDTNPXi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->x1 == val0);
                     CHECK(gprState->x2 == val1);
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsui"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(seenPost);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_AARCH64-sttnpxi") {
  if (!checkFeature("lsui")) {
    return;
  }

  const char source[] = "sttnp x1, x2, [x0, #16]\n";

  constexpr uint64_t val0 = 0x2233445566778899ULL;
  constexpr uint64_t val1 = 0xaabbccddeeff0011ULL;
  uint8_t buf[40] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[16], val0, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[24], val1, 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STTNPXi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STTNPXi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = (QBDI::rword)&buf[0];
  state->x1 = val0;
  state->x2 = val1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lsui"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint64_t *)&buf[16] == val0);
  CHECK(*(uint64_t *)&buf[24] == val1);
}
