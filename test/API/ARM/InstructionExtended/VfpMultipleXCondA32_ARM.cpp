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
#include <string>

#include "ConditionTestUtils_ARM.h"
#include "MemAccessTestUtils_ARM.h"

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::conditionForIndex;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;
using QBDITestBatch2::setConditionCPSR;

static void checkFldmiaxCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(0);
  std::string source = std::string("fldmiax") + cond.suffix + " r0, {d0, d1}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0xd7b24369, 0xb25e4516};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{};
  if (taken) {
    expectedPre.accesses = {
        {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
        {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
        {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
        {(QBDI::rword)&v1[3], v1[3], 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
    };
  }
  ExpectedMemoryAccesses expectedPost = expectedPre;
  fixture.vm.addMnemonicCB("FLDMXIA", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("FLDMXIA", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1[0]);
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::FPRState *fpr = fixture.vm.getFPRState();
  QBDI::rword poison[4] = {0xdead0000, 0xdead0001, 0xdead0002, 0xdead0003};
  memcpy(fpr, poison, sizeof(poison));
  fixture.vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *finalFpr = fixture.vm.getFPRState();
  if (taken) {
    CHECK(memcmp(finalFpr, &v1, sizeof(v1)) == 0);
  } else {
    CHECK(memcmp(finalFpr, poison, sizeof(poison)) == 0);
  }

  state = fixture.vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1[0]));
}

static void checkFldmiaxUpdCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(1);
  std::string source =
      std::string("fldmiax") + cond.suffix + " r0!, {d0, d1}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0xd7b24369, 0xb25e4516};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{};
  if (taken) {
    expectedPre.accesses = {
        {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
        {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
        {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
        {(QBDI::rword)&v1[3], v1[3], 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
    };
  }
  ExpectedMemoryAccesses expectedPost = expectedPre;
  fixture.vm.addMnemonicCB("FLDMXIA_UPD", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("FLDMXIA_UPD", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1[0]);
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::FPRState *fpr = fixture.vm.getFPRState();
  QBDI::rword poison[4] = {0xdead0000, 0xdead0001, 0xdead0002, 0xdead0003};
  memcpy(fpr, poison, sizeof(poison));
  fixture.vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *finalFpr = fixture.vm.getFPRState();
  if (taken) {
    CHECK(memcmp(finalFpr, &v1, sizeof(v1)) == 0);
  } else {
    CHECK(memcmp(finalFpr, poison, sizeof(poison)) == 0);
  }

  state = fixture.vm.getGPRState();
  CHECK(state->r0 == (taken ? reinterpret_cast<QBDI::rword>(&v1[4]) + 4
                            : reinterpret_cast<QBDI::rword>(&v1[0])));
}

static void checkFldmdbxUpdCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(2);
  std::string source =
      std::string("fldmdbx") + cond.suffix + " r0!, {d0, d1}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0xd7b24369, 0xb25e4516};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{};
  if (taken) {
    expectedPre.accesses = {
        {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
        {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
        {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
        {(QBDI::rword)&v1[3], v1[3], 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
    };
  }
  ExpectedMemoryAccesses expectedPost = expectedPre;
  fixture.vm.addMnemonicCB("FLDMXDB_UPD", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("FLDMXDB_UPD", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1[4]) + 4;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::FPRState *fpr = fixture.vm.getFPRState();
  QBDI::rword poison[4] = {0xdead0000, 0xdead0001, 0xdead0002, 0xdead0003};
  memcpy(fpr, poison, sizeof(poison));
  fixture.vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *finalFpr = fixture.vm.getFPRState();
  if (taken) {
    CHECK(memcmp(finalFpr, &v1, sizeof(v1)) == 0);
  } else {
    CHECK(memcmp(finalFpr, poison, sizeof(poison)) == 0);
  }

  state = fixture.vm.getGPRState();
  CHECK(state->r0 == (taken ? reinterpret_cast<QBDI::rword>(&v1[0])
                            : reinterpret_cast<QBDI::rword>(&v1[4]) + 4));
}

static void checkFstmiaxCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(3);
  std::string source = std::string("fstmiax") + cond.suffix + " r0, {d0, d1}\n";

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e, 0xd7b24369, 0xb25e4516};
  QBDI::rword v1[] = {0, 0, 0, 0};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPost{};
  if (taken) {
    expectedPost.accesses = {
        {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
        {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
        {(QBDI::rword)&v1[2], v[2], 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
        {(QBDI::rword)&v1[3], v[3], 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
    };
  }
  ExpectedMemoryAccesses expectedPre{};
  fixture.vm.addMnemonicCB("FSTMXIA", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("FSTMXIA", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1[0]);
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::FPRState *fpr = fixture.vm.getFPRState();
  memcpy(fpr, v, sizeof(v));
  fixture.vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  if (taken) {
    CHECK(memcmp(&v, &v1, sizeof(v1)) == 0);
  } else {
    QBDI::rword zero[4] = {0, 0, 0, 0};
    CHECK(memcmp(&v1, zero, sizeof(v1)) == 0);
  }

  state = fixture.vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1[0]));
}

static void checkFstmiaxUpdCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(4);
  std::string source =
      std::string("fstmiax") + cond.suffix + " r0!, {d0, d1}\n";

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e, 0xd7b24369, 0xb25e4516};
  QBDI::rword v1[] = {0, 0, 0, 0};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPost{};
  if (taken) {
    expectedPost.accesses = {
        {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
        {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
        {(QBDI::rword)&v1[2], v[2], 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
        {(QBDI::rword)&v1[3], v[3], 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
    };
  }
  ExpectedMemoryAccesses expectedPre{};
  fixture.vm.addMnemonicCB("FSTMXIA_UPD", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("FSTMXIA_UPD", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1[0]);
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::FPRState *fpr = fixture.vm.getFPRState();
  memcpy(fpr, v, sizeof(v));
  fixture.vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  if (taken) {
    CHECK(memcmp(&v, &v1, sizeof(v1)) == 0);
  } else {
    QBDI::rword zero[4] = {0, 0, 0, 0};
    CHECK(memcmp(&v1, zero, sizeof(v1)) == 0);
  }

  state = fixture.vm.getGPRState();
  CHECK(state->r0 == (taken ? reinterpret_cast<QBDI::rword>(&v1[4]) + 4
                            : reinterpret_cast<QBDI::rword>(&v1[0])));
}

static void checkFstmdbxUpdCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(5);
  std::string source =
      std::string("fstmdbx") + cond.suffix + " r0!, {d0, d1}\n";

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e, 0xd7b24369, 0xb25e4516};
  QBDI::rword v1[] = {0, 0, 0, 0};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPost{};
  if (taken) {
    expectedPost.accesses = {
        {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
        {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
        {(QBDI::rword)&v1[2], v[2], 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
        {(QBDI::rword)&v1[3], v[3], 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
    };
  }
  ExpectedMemoryAccesses expectedPre{};
  fixture.vm.addMnemonicCB("FSTMXDB_UPD", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("FSTMXDB_UPD", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1[4]) + 4;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::FPRState *fpr = fixture.vm.getFPRState();
  memcpy(fpr, v, sizeof(v));
  fixture.vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  if (taken) {
    CHECK(memcmp(&v, &v1, sizeof(v1)) == 0);
  } else {
    QBDI::rword zero[4] = {0, 0, 0, 0};
    CHECK(memcmp(&v1, zero, sizeof(v1)) == 0);
  }

  state = fixture.vm.getGPRState();
  CHECK(state->r0 == (taken ? reinterpret_cast<QBDI::rword>(&v1[0])
                            : reinterpret_cast<QBDI::rword>(&v1[4]) + 4));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-fldmiax_cond_taken") {
  checkFldmiaxCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-fldmiax_cond_not_taken") {
  checkFldmiaxCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-fldmiax_upd_cond_taken") {
  checkFldmiaxUpdCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-fldmiax_upd_cond_not_taken") {
  checkFldmiaxUpdCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-fldmdbx_upd_cond_taken") {
  checkFldmdbxUpdCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-fldmdbx_upd_cond_not_taken") {
  checkFldmdbxUpdCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-fstmiax_cond_taken") {
  checkFstmiaxCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-fstmiax_cond_not_taken") {
  checkFstmiaxCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-fstmiax_upd_cond_taken") {
  checkFstmiaxUpdCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-fstmiax_upd_cond_not_taken") {
  checkFstmiaxUpdCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-fstmdbx_upd_cond_taken") {
  checkFstmdbxUpdCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-fstmdbx_upd_cond_not_taken") {
  checkFstmdbxUpdCond(*this, false);
}
