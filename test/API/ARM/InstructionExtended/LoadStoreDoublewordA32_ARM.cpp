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
#include "MemAccessTestUtils_ARM.h"

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrd") {
  const char source[] = "ldrd r2, r3, [r0, #8]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expected = {{
      {(QBDI::rword)&buf[8], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("LDRD", QBDI::PREINST, checkAccess, &expected);
  vm.addMnemonicCB("LDRD", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->r2 == 0x44332211);
                     CHECK(gprState->r3 == 0x88776655);
                     CHECK(gprState->r0 == (QBDI::rword)&buf[0]);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expected.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrd_pre") {
  const char source[] = "ldrd r2, r3, [r0, #8]!\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expected = {{
      {(QBDI::rword)&buf[8], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("LDRD_PRE", QBDI::PREINST, checkAccess, &expected);
  vm.addMnemonicCB("LDRD_PRE", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->r2 == 0x44332211);
                     CHECK(gprState->r3 == 0x88776655);
                     CHECK(gprState->r0 == (QBDI::rword)&buf[8]);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expected.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrd_post") {
  const char source[] = "ldrd r2, r3, [r0], #8\n";

  uint8_t buf[16] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expected = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("LDRD_POST", QBDI::PREINST, checkAccess, &expected);
  vm.addMnemonicCB("LDRD_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->r2 == 0x44332211);
                     CHECK(gprState->r3 == 0x88776655);
                     CHECK(gprState->r0 == (QBDI::rword)&buf[8]);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expected.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strd") {
  const char source[] = "strd r2, r3, [r0, #8]\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[8], 0x44332211, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x88776655, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 0x44332211;
  state->r3 = 0x88776655;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[8] == 0x44332211);
  CHECK(*(uint32_t *)&buf[12] == 0x88776655);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strd_pre") {
  const char source[] = "strd r2, r3, [r0, #8]!\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[8], 0x44332211, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[12], 0x88776655, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRD_PRE", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRD_PRE", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STRD_PRE", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->r0 == (QBDI::rword)&buf[8]);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 0x44332211;
  state->r3 = 0x88776655;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[8] == 0x44332211);
  CHECK(*(uint32_t *)&buf[12] == 0x88776655);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strd_post") {
  const char source[] = "strd r2, r3, [r0], #8\n";

  uint8_t buf[16] = {0};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&buf[0], 0x44332211, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&buf[4], 0x88776655, 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};
  vm.addMnemonicCB("STRD_POST", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRD_POST", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("STRD_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->r0 == (QBDI::rword)&buf[8]);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  state->r2 = 0x44332211;
  state->r3 = 0x88776655;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[0] == 0x44332211);
  CHECK(*(uint32_t *)&buf[4] == 0x88776655);
}
