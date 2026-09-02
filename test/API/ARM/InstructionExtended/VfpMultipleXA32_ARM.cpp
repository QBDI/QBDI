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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-fldmiax_small") {

  const char source[] = "fldmiax r0, {d0, d1}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0xd7b24369, 0xb25e4516};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[3], v1[3], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("FLDMXIA", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("FLDMXIA", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1[0]);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  CHECK(memcmp(fpr, &v1, sizeof(v1)) == 0);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1[0]));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-fldmiax_large") {

  const char source[] = "fldmiax r0, {d0-d15}\n";

  QBDI::rword v1[32] = {
      0x5de254a1, 0x747f4b7e, 0x31eed260, 0xf21a4416, 0x319b8e1b, 0x215f4510,
      0x927e556e, 0xa8a0e729, 0x719ca3c1, 0xd7b24369, 0xb25e4516, 0x235b2fc3,
      0xc2708a8b, 0x2d624053, 0xaad33b87, 0x33562724, 0x215f4510, 0x927e556e,
      0xa8a0e729, 0x235b2fc3, 0xc2708a8b, 0x2d624053, 0xaad33b87, 0x33562724,
      0x719ca3c1, 0xd7b24369, 0xb25e4516, 0x319b8e1b, 0x31eed260, 0x747f4b7e,
      0xc2708a8b, 0xf21a4416};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], 0, 128, QBDI::MEMORY_READ,
       QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("FLDMXIA", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("FLDMXIA", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1[0]);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  CHECK(memcmp(fpr, &v1, sizeof(v1)) == 0);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1[0]));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-fldmiax_upd_small") {

  const char source[] = "fldmiax r0!, {d0, d1}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0xd7b24369, 0xb25e4516};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[3], v1[3], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("FLDMXIA_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("FLDMXIA_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1[0]);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  CHECK(memcmp(fpr, &v1, sizeof(v1)) == 0);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1[4]) + 4);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-fldmiax_upd_large") {

  const char source[] = "fldmiax r0!, {d0-d15}\n";

  QBDI::rword v1[32] = {
      0x5de254a1, 0x747f4b7e, 0x31eed260, 0xf21a4416, 0x319b8e1b, 0x215f4510,
      0x927e556e, 0xa8a0e729, 0x719ca3c1, 0xd7b24369, 0xb25e4516, 0x235b2fc3,
      0xc2708a8b, 0x2d624053, 0xaad33b87, 0x33562724, 0x215f4510, 0x927e556e,
      0xa8a0e729, 0x235b2fc3, 0xc2708a8b, 0x2d624053, 0xaad33b87, 0x33562724,
      0x719ca3c1, 0xd7b24369, 0xb25e4516, 0x319b8e1b, 0x31eed260, 0x747f4b7e,
      0xc2708a8b, 0xf21a4416};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], 0, 128, QBDI::MEMORY_READ,
       QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("FLDMXIA_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("FLDMXIA_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1[0]);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  CHECK(memcmp(fpr, &v1, sizeof(v1)) == 0);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1[32]) + 4);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-fldmdbx_upd_small") {

  const char source[] = "fldmdbx r0!, {d0, d1}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0xd7b24369, 0xb25e4516};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[3], v1[3], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("FLDMXDB_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("FLDMXDB_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1[4]) + 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  CHECK(memcmp(fpr, &v1, sizeof(v1)) == 0);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1[0]));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-fldmdbx_upd_large") {

  const char source[] = "fldmdbx r0!, {d0-d15}\n";

  QBDI::rword v1[32] = {0};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], 0, 128, QBDI::MEMORY_READ,
       QBDI::MEMORY_UNKNOWN_VALUE},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("FLDMXDB_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("FLDMXDB_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1[32]) + 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  CHECK(memcmp(fpr, &v1, sizeof(v1)) == 0);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1[0]));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-fstmiax_small") {

  const char source[] = "fstmiax r0, {d0, d1}\n";

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e, 0xd7b24369, 0xb25e4516};
  QBDI::rword v1[] = {0, 0, 0, 0};
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v[2], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[3], v[3], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("FSTMXIA", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("FSTMXIA", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1[0]);
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  memcpy(fpr, v, sizeof(v));
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  CHECK(memcmp(&v, &v1, sizeof(v1)) == 0);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1[0]));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-fstmiax_large") {

  const char source[] = "fstmiax r0, {d0-d15}\n";

  QBDI::rword v[32] = {
      0x5de254a1, 0x747f4b7e, 0x31eed260, 0xf21a4416, 0x319b8e1b, 0x215f4510,
      0x927e556e, 0xa8a0e729, 0x719ca3c1, 0xd7b24369, 0xb25e4516, 0x235b2fc3,
      0xc2708a8b, 0x2d624053, 0xaad33b87, 0x33562724, 0x215f4510, 0x927e556e,
      0xa8a0e729, 0x235b2fc3, 0xc2708a8b, 0x2d624053, 0xaad33b87, 0x33562724,
      0x719ca3c1, 0xd7b24369, 0xb25e4516, 0x319b8e1b, 0x31eed260, 0x747f4b7e,
      0xc2708a8b, 0xf21a4416};
  QBDI::rword v1[32] = {0};
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], 0, 128, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_UNKNOWN_VALUE},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("FSTMXIA", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("FSTMXIA", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1[0]);
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  memcpy(fpr, v, sizeof(v));
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  CHECK(memcmp(&v, &v1, sizeof(v1)) == 0);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1[0]));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-fstmiax_upd_small") {

  const char source[] = "fstmiax r0!, {d0, d1}\n";

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e, 0xd7b24369, 0xb25e4516};
  QBDI::rword v1[] = {0, 0, 0, 0};
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v[2], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[3], v[3], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("FSTMXIA_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("FSTMXIA_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1[0]);
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  memcpy(fpr, v, sizeof(v));
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  CHECK(memcmp(&v, &v1, sizeof(v1)) == 0);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1[4]) + 4);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-fstmiax_upd_large") {

  const char source[] = "fstmiax r0!, {d0-d15}\n";

  QBDI::rword v[32] = {
      0x5de254a1, 0x747f4b7e, 0x31eed260, 0xf21a4416, 0x319b8e1b, 0x215f4510,
      0x927e556e, 0xa8a0e729, 0x719ca3c1, 0xd7b24369, 0xb25e4516, 0x235b2fc3,
      0xc2708a8b, 0x2d624053, 0xaad33b87, 0x33562724, 0x215f4510, 0x927e556e,
      0xa8a0e729, 0x235b2fc3, 0xc2708a8b, 0x2d624053, 0xaad33b87, 0x33562724,
      0x719ca3c1, 0xd7b24369, 0xb25e4516, 0x319b8e1b, 0x31eed260, 0x747f4b7e,
      0xc2708a8b, 0xf21a4416};
  QBDI::rword v1[32] = {0};
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], 0, 128, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_UNKNOWN_VALUE},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("FSTMXIA_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("FSTMXIA_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1[0]);
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  memcpy(fpr, v, sizeof(v));
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  CHECK(memcmp(&v, &v1, sizeof(v1)) == 0);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1[32]) + 4);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-fstmdbx_upd_small") {

  const char source[] = "fstmdbx r0!, {d0, d1}\n";

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e, 0xd7b24369, 0xb25e4516};
  QBDI::rword v1[] = {0, 0, 0, 0};
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v[2], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[3], v[3], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("FSTMXDB_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("FSTMXDB_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1[4]) + 4;
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  memcpy(fpr, v, sizeof(v));
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  CHECK(memcmp(&v, &v1, sizeof(v1)) == 0);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1[0]));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-fstmdbx_upd_large") {

  const char source[] = "fstmdbx r0!, {d0-d15}\n";

  QBDI::rword v[32] = {
      0x5de254a1, 0x747f4b7e, 0x31eed260, 0xf21a4416, 0x319b8e1b, 0x215f4510,
      0x927e556e, 0xa8a0e729, 0x719ca3c1, 0xd7b24369, 0xb25e4516, 0x235b2fc3,
      0xc2708a8b, 0x2d624053, 0xaad33b87, 0x33562724, 0x215f4510, 0x927e556e,
      0xa8a0e729, 0x235b2fc3, 0xc2708a8b, 0x2d624053, 0xaad33b87, 0x33562724,
      0x719ca3c1, 0xd7b24369, 0xb25e4516, 0x319b8e1b, 0x31eed260, 0x747f4b7e,
      0xc2708a8b, 0xf21a4416};
  QBDI::rword v1[32] = {0};
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], 0, 128, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_UNKNOWN_VALUE},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("FSTMXDB_UPD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("FSTMXDB_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1[32]) + 4;
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  memcpy(fpr, v, sizeof(v));
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  CHECK(memcmp(&v, &v1, sizeof(v1)) == 0);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1[0]));
}
