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
#include <cstring>
#include <string>

#include "ConditionTestUtils_ARM.h"
#include "MemAccessTestUtils_ARM.h"

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::checkFeature;
using QBDITestBatch2::conditionForIndex;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;
using QBDITestBatch2::setConditionCPSR;

static void checkVldrhCond(APITest &fixture, bool taken) {
  if (!checkFeature("fullfp16")) {
    return;
  }
  const auto &cond = conditionForIndex(0);
  std::string source = std::string("vldr") + cond.suffix + ".16 s0, [r0, #4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {
        {(QBDI::rword)&buf[8], 0x8988, 2, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
    };
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("VLDRH", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("VLDRH", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::FPRState *fpr = fixture.vm.getFPRState();
  fpr->vreg.s[0] = 0.0f;
  fixture.vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::ARM,
                              {"fullfp16"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::FPRState *finalFpr = fixture.vm.getFPRState();
  if (taken) {
    CHECK(finalFpr->vreg.q[0][0] == 0x88);
    CHECK(finalFpr->vreg.q[0][1] == 0x89);
  } else {
    CHECK(finalFpr->vreg.s[0] == 0.0f);
  }
}

static void checkVstrhCond(APITest &fixture, bool taken) {
  if (!checkFeature("fullfp16")) {
    return;
  }
  const auto &cond = conditionForIndex(1);
  std::string source = std::string("vstr") + cond.suffix + ".16 s0, [r0, #4]\n";

  uint8_t buf[16] = {0};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {
        {(QBDI::rword)&buf[4], 0xabcd, 2, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
    };
  }
  fixture.vm.addMnemonicCB("VSTRH", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("VSTRH", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::FPRState *fpr = fixture.vm.getFPRState();
  fpr->vreg.q[0][0] = 0xcd;
  fpr->vreg.q[0][1] = 0xab;
  fixture.vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::ARM,
                              {"fullfp16"});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(buf[4] == (taken ? 0xcd : 0x00));
  CHECK(buf[5] == (taken ? 0xab : 0x00));
}

static void checkVldrsCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(2);
  std::string source = std::string("vldr") + cond.suffix + " s0, [r0, #4]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {
        {(QBDI::rword)&buf[8], 0x8b8a8988, 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
    };
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("VLDRS", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("VLDRS", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[4];
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::FPRState *fpr = fixture.vm.getFPRState();
  fpr->vreg.q[0][0] = 0xde;
  fpr->vreg.q[0][1] = 0xad;
  fpr->vreg.q[0][2] = 0xbe;
  fpr->vreg.q[0][3] = 0xef;
  fixture.vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::FPRState *finalFpr = fixture.vm.getFPRState();
  if (taken) {
    CHECK(finalFpr->vreg.q[0][0] == 0x88);
    CHECK(finalFpr->vreg.q[0][1] == 0x89);
    CHECK(finalFpr->vreg.q[0][2] == 0x8a);
    CHECK(finalFpr->vreg.q[0][3] == 0x8b);
  } else {
    CHECK(finalFpr->vreg.q[0][0] == 0xde);
    CHECK(finalFpr->vreg.q[0][1] == 0xad);
    CHECK(finalFpr->vreg.q[0][2] == 0xbe);
    CHECK(finalFpr->vreg.q[0][3] == 0xef);
  }
}

static void checkVstrsCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(3);
  std::string source = std::string("vstr") + cond.suffix + " s0, [r0, #4]\n";

  uint8_t buf[16] = {0};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {
        {(QBDI::rword)&buf[4], 0xaabbccdd, 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
    };
  }
  fixture.vm.addMnemonicCB("VSTRS", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("VSTRS", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::FPRState *fpr = fixture.vm.getFPRState();
  fpr->vreg.q[0][0] = 0xdd;
  fpr->vreg.q[0][1] = 0xcc;
  fpr->vreg.q[0][2] = 0xbb;
  fpr->vreg.q[0][3] = 0xaa;
  fixture.vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[4] == (taken ? 0xaabbccddu : 0x00000000u));
}

static void checkVldrdCond(APITest &fixture, bool taken) {
  if (!checkFeature("vfp2")) {
    return;
  }
  const auto &cond = conditionForIndex(4);
  std::string source = std::string("vldr") + cond.suffix + " d0, [r0, #8]\n";

  uint8_t buf[16] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                     0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {
        {(QBDI::rword)&buf[8], 0x44332211, 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
        {(QBDI::rword)&buf[12], 0x88776655, 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
    };
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("VLDRD", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("VLDRD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::FPRState *fpr = fixture.vm.getFPRState();
  for (int i = 0; i < 8; i++) {
    fpr->vreg.q[0][i] = 0xa0 + i;
  }
  fixture.vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::FPRState *finalFpr = fixture.vm.getFPRState();
  for (int i = 0; i < 4; i++) {
    if (taken) {
      CHECK(finalFpr->vreg.q[0][i] == buf[8 + i]);
      CHECK(finalFpr->vreg.q[0][4 + i] == buf[12 + i]);
    } else {
      CHECK(finalFpr->vreg.q[0][i] == (uint8_t)(0xa0 + i));
      CHECK(finalFpr->vreg.q[0][4 + i] == (uint8_t)(0xa0 + 4 + i));
    }
  }
}

static void checkVstrdCond(APITest &fixture, bool taken) {
  if (!checkFeature("vfp2")) {
    return;
  }
  const auto &cond = conditionForIndex(5);
  std::string source = std::string("vstr") + cond.suffix + " d0, [r0, #8]\n";

  uint8_t buf[16] = {0};
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPost.accesses = {
        {(QBDI::rword)&buf[8], 0x44332211, 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
        {(QBDI::rword)&buf[12], 0x88776655, 4, QBDI::MEMORY_WRITE,
         QBDI::MEMORY_NO_FLAGS},
    };
  }
  fixture.vm.addMnemonicCB("VSTRD", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB("VSTRD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = (QBDI::rword)&buf[0];
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::FPRState *fpr = fixture.vm.getFPRState();
  fpr->vreg.q[0][0] = 0x11;
  fpr->vreg.q[0][1] = 0x22;
  fpr->vreg.q[0][2] = 0x33;
  fpr->vreg.q[0][3] = 0x44;
  fpr->vreg.q[0][4] = 0x55;
  fpr->vreg.q[0][5] = 0x66;
  fpr->vreg.q[0][6] = 0x77;
  fpr->vreg.q[0][7] = 0x88;
  fixture.vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(*(uint32_t *)&buf[8] == (taken ? 0x44332211u : 0x00000000u));
  CHECK(*(uint32_t *)&buf[12] == (taken ? 0x88776655u : 0x00000000u));
}

static void checkVldmddbUpdCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(6);
  std::string source = std::string("vldmdb") + cond.suffix + " r0!, {d0, d1}\n";

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
  fixture.vm.addMnemonicCB("VLDMDDB_UPD", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("VLDMDDB_UPD", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1[4]);
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
                            : reinterpret_cast<QBDI::rword>(&v1[4])));
}

static void checkVldmsdbUpdCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(7);
  std::string source = std::string("vldmdb") + cond.suffix + " r0!, {s0-s2}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0xd7b24369};
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
    };
  }

  ExpectedMemoryAccesses expectedPost = expectedPre;
  fixture.vm.addMnemonicCB("VLDMSDB_UPD", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("VLDMSDB_UPD", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1[3]);
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::FPRState *fpr = fixture.vm.getFPRState();
  QBDI::rword poison[3] = {0xdead0000, 0xdead0001, 0xdead0002};
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
                            : reinterpret_cast<QBDI::rword>(&v1[3])));
}

static void checkVstmddbUpdCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(8);
  std::string source = std::string("vstmdb") + cond.suffix + " r0!, {d0, d1}\n";

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
  fixture.vm.addMnemonicCB("VSTMDDB_UPD", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("VSTMDDB_UPD", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1[4]);
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
                            : reinterpret_cast<QBDI::rword>(&v1[4])));
}

static void checkVstmsdbUpdCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(9);
  std::string source = std::string("vstmdb") + cond.suffix + " r0!, {s0-s2}\n";

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e, 0xd7b24369};
  QBDI::rword v1[] = {0, 0, 0};
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
    };
  }

  ExpectedMemoryAccesses expectedPre{};
  fixture.vm.addMnemonicCB("VSTMSDB_UPD", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("VSTMSDB_UPD", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1[3]);
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
    QBDI::rword zero[3] = {0, 0, 0};
    CHECK(memcmp(&v1, zero, sizeof(v1)) == 0);
  }

  state = fixture.vm.getGPRState();
  CHECK(state->r0 == (taken ? reinterpret_cast<QBDI::rword>(&v1[0])
                            : reinterpret_cast<QBDI::rword>(&v1[3])));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vldrh_cond_taken") {
  checkVldrhCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vldrh_cond_not_taken") {
  checkVldrhCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vstrh_cond_taken") {
  checkVstrhCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vstrh_cond_not_taken") {
  checkVstrhCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vldrs_cond_taken") {
  checkVldrsCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vldrs_cond_not_taken") {
  checkVldrsCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vstrs_cond_taken") {
  checkVstrsCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vstrs_cond_not_taken") {
  checkVstrsCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vldrd_cond_taken") {
  checkVldrdCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vldrd_cond_not_taken") {
  checkVldrdCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vstrd_cond_taken") {
  checkVstrdCond(*this, true);
}
TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vstrd_cond_not_taken") {
  checkVstrdCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-vldmddb_upd_cond_taken") {
  checkVldmddbUpdCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-vldmddb_upd_cond_not_taken") {
  checkVldmddbUpdCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-vldmsdb_upd_cond_taken") {
  checkVldmsdbUpdCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-vldmsdb_upd_cond_not_taken") {
  checkVldmsdbUpdCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-vstmddb_upd_cond_taken") {
  checkVstmddbUpdCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-vstmddb_upd_cond_not_taken") {
  checkVstmddbUpdCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-vstmsdb_upd_cond_taken") {
  checkVstmsdbUpdCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-vstmsdb_upd_cond_not_taken") {
  checkVstmsdbUpdCond(*this, false);
}
