/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2022 Quarkslab
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
#include <catch2/catch.hpp>
#include "API/APITest.h"

#include "inttypes.h"

#include "QBDI/Memory.hpp"
#include "QBDI/Platform.h"
#include "QBDI/Range.h"

#include "Utility/System.h"

static bool checkFeature(const char *f) {
  if (!QBDI::isHostCPUFeaturePresent(f)) {
    WARN("Host doesn't support " << f << " feature: SKIP");
    return false;
  }
  return true;
}

[[maybe_unused]] static QBDI::VMAction debugCB(QBDI::VMInstanceRef vm,
                                               QBDI::GPRState *gprState,
                                               QBDI::FPRState *fprState,
                                               void *data) {
  const QBDI::InstAnalysis *instAnalysis = vm->getInstAnalysis();
  printf("0x%lx (%10s): %s\n", instAnalysis->address, instAnalysis->mnemonic,
         instAnalysis->disassembly);

  for (auto &a : vm->getInstMemoryAccess()) {
    printf(
        " - inst: 0x%lx, addr: 0x%lx, size: %d, type: %c%c, "
        "value: 0x%lx, flags : 0x %x\n",
        a.instAddress, a.accessAddress, a.size,
        ((a.type & QBDI::MEMORY_READ) != 0) ? 'r' : '-',
        ((a.type & QBDI::MEMORY_WRITE) != 0) ? 'w' : '-', a.value, a.flags);
  }
  return QBDI::VMAction::CONTINUE;
}

struct ExpectedMemoryAccess {
  QBDI::rword address;
  QBDI::rword value;
  uint16_t size;
  QBDI::MemoryAccessType type;
  QBDI::MemoryAccessFlags flags;
  bool see = false;
};

struct ExpectedMemoryAccesses {
  std::vector<ExpectedMemoryAccess> accesses;
};

static QBDI::VMAction checkAccess(QBDI::VMInstanceRef vm,
                                  QBDI::GPRState *gprState,
                                  QBDI::FPRState *fprState, void *data) {

  ExpectedMemoryAccesses *info = static_cast<ExpectedMemoryAccesses *>(data);
  if (std::all_of(info->accesses.begin(), info->accesses.end(),
                  [](ExpectedMemoryAccess &a) { return a.see; }))
    return QBDI::VMAction::CONTINUE;

  std::vector<QBDI::MemoryAccess> memaccesses = vm->getInstMemoryAccess();

  CHECKED_IF(memaccesses.size() == info->accesses.size()) {
    for (size_t i = 0; i < info->accesses.size(); i++) {
      auto &memaccess = memaccesses[i];
      auto &expect = info->accesses[i];
      INFO("Expected Access n°" << i);
      INFO("Value 0x" << std::hex << memaccess.value << " expect 0x" << std::hex
                      << expect.value);
      CHECKED_IF(memaccess.accessAddress == expect.address)
      CHECKED_IF((memaccess.value == expect.value || expect.value == 0))
      CHECKED_IF(memaccess.size == expect.size)
      CHECKED_IF(memaccess.type == expect.type)
      CHECKED_IF(memaccess.flags == expect.flags)
      expect.see = true;
    }
  }
  return QBDI::VMAction::CONTINUE;
}

void setFPR(QBDI::FPRState *fpr, size_t index, QBDI::rword hvalue,
            QBDI::rword lvalue) {
  reinterpret_cast<QBDI::rword *>(fpr)[index * 2] = lvalue;
  reinterpret_cast<QBDI::rword *>(fpr)[index * 2 + 1] = hvalue;
}

void checkFullFPR(QBDI::FPRState *fpr, size_t index, QBDI::rword hvalue,
                  QBDI::rword lvalue) {
  INFO("v" << index);
  CHECK(reinterpret_cast<QBDI::rword *>(fpr)[index * 2] == lvalue);
  CHECK(reinterpret_cast<QBDI::rword *>(fpr)[index * 2 + 1] == hvalue);
}

void checkLowFPR(QBDI::FPRState *fpr, size_t index, QBDI::rword lvalue) {
  INFO("v" << index);
  CHECK(reinterpret_cast<QBDI::rword *>(fpr)[index * 2] == lvalue);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ldpx") {

  const char source[] = "ldp x0, x1, [x2]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0x8e060b1505409a1bull};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDPXi", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = 0;
  state->x1 = 0;
  state->x2 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->x0 == v1[0]);
  CHECK(state->x1 == v1[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ldpx_2") {

  const char source[] = "ldp x0, x1, [x2, #8]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0x8e060b1505409a1bull,
                      0x616d087f1e054a7cull};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[1], v1[1], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDPXi", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = 0;
  state->x1 = 0;
  state->x2 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->x0 == v1[1]);
  CHECK(state->x1 == v1[2]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ldpx_pre") {

  const char source[] = "ldp x0, x1, [x2, #8]!\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0x8e060b1505409a1bull,
                      0x616d087f1e054a7cull};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[1], v1[1], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDPXpre", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = 0;
  state->x1 = 0;
  state->x2 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->x0 == v1[1]);
  CHECK(state->x1 == v1[2]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ldpx_post") {

  const char source[] = "ldp x0, x1, [x2], #8\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0x8e060b1505409a1bull,
                      0x616d087f1e054a7cull};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDPXpost", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = 0;
  state->x1 = 0;
  state->x2 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->x0 == v1[0]);
  CHECK(state->x1 == v1[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ldpw") {

  const char source[] = "ldp w0, w1, [x2]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0x8e060b1505409a1bull};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDPWi", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = 0;
  state->x1 = 0;
  state->x2 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->x0 == (v1[0] & 0xffffffff));
  CHECK(state->x1 == (v1[0] >> 32));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ldpw_2") {

  const char source[] = "ldp w0, w1, [x2, #8]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0x8e060b1505409a1bul};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[1], v1[1], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDPWi", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = 0;
  state->x1 = 0;
  state->x2 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->x0 == (v1[1] & 0xffffffff));
  CHECK(state->x1 == (v1[1] >> 32));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ldpw_pre") {

  const char source[] = "ldp w0, w1, [x2, #8]!\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0x8e060b1505409a1bul};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[1], v1[1], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDPWpre", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = 0;
  state->x1 = 0;
  state->x2 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->x0 == (v1[1] & 0xffffffff));
  CHECK(state->x1 == (v1[1] >> 32));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ldrb_1") {

  const char source[] = "ldrb w0, [x1, x2]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0x8e060b1505409a1bull};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[1], v1[1] & 0xff, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRBBroX", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = 0;
  state->x1 = reinterpret_cast<QBDI::rword>(&v1);
  state->x2 = 8;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->x0 == (v1[1] & 0xff));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ldrb_2") {

  const char source[] = "ldrb w0, [x1, x2]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0x8e060b1505409a1bull};
  ExpectedMemoryAccesses expectedPre = {{
      {((QBDI::rword)&v1[0]) + 4, (v1[0] >> 32) & 0xff, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRBBroX", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = 0;
  state->x1 = reinterpret_cast<QBDI::rword>(&v1) + 8;
  state->x2 = -4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->x0 == ((v1[0] >> 32) & 0xff));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ldrb_3") {

  const char source[] = "ldrb w0, [x1, w2, SXTW]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0x8e060b1505409a1bull};
  ExpectedMemoryAccesses expectedPre = {{
      {((QBDI::rword)&v1[0]) + 4, (v1[0] >> 32) & 0xff, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRBBroW", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = 0;
  state->x1 = reinterpret_cast<QBDI::rword>(&v1) + 8;
  state->x2 = -4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->x0 == ((v1[0] >> 32) & 0xff));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ldrb_4") {

  const char source[] = "ldrb w0, [x1, w2, UXTW]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0x8e060b1505409a1bull};
  ExpectedMemoryAccesses expectedPre = {{
      {((QBDI::rword)&v1[0]) + 3, (v1[0] >> 24) & 0xff, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRBBroW", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = 0;
  state->x1 = reinterpret_cast<QBDI::rword>(&v1);
  state->x2 = 3;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->x0 == ((v1[0] >> 24) & 0xff));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ldrh_1") {

  const char source[] = "ldrh w0, [x1, x2, LSL #0]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0x8e060b1505409a1bull};
  ExpectedMemoryAccesses expectedPre = {{
      {((QBDI::rword)&v1[0]) + 5, (v1[0] >> 40) & 0xffff, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRHHroX", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = 0;
  state->x1 = reinterpret_cast<QBDI::rword>(&v1);
  state->x2 = 5;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->x0 == ((v1[0] >> 40) & 0xffff));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ldrh_2") {

  const char source[] = "ldrh w0, [x1, x2, LSL #1]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0x8e060b1505409a1bull};
  ExpectedMemoryAccesses expectedPre = {{
      {((QBDI::rword)&v1[1]) + 2, (v1[1] >> 16) & 0xffff, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRHHroX", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = 0;
  state->x1 = reinterpret_cast<QBDI::rword>(&v1);
  state->x2 = 5;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->x0 == ((v1[1] >> 16) & 0xffff));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ldrh_3") {

  const char source[] = "ldrh w0, [x1, x2, SXTX #0]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0x8e060b1505409a1bull};
  ExpectedMemoryAccesses expectedPre = {{
      {((QBDI::rword)&v1[0]) + 4, (v1[0] >> 32) & 0xffff, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRHHroX", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = 0;
  state->x1 = reinterpret_cast<QBDI::rword>(&v1) + 8;
  state->x2 = -4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->x0 == ((v1[0] >> 32) & 0xffff));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ldrh_4") {

  const char source[] = "ldrh w0, [x1, x2, SXTX #1]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0x8e060b1505409a1bull};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0] & 0xffff, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRHHroX", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = 0;
  state->x1 = reinterpret_cast<QBDI::rword>(&v1) + 8;
  state->x2 = -4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->x0 == (v1[0] & 0xffff));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ldrh_5") {

  const char source[] = "ldrh w0, [x1, w2, UXTW #0]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0x8e060b1505409a1bull};
  ExpectedMemoryAccesses expectedPre = {{
      {((QBDI::rword)&v1[0]) + 5, (v1[0] >> 40) & 0xffff, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRHHroW", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = 0;
  state->x1 = reinterpret_cast<QBDI::rword>(&v1);
  state->x2 = 5;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->x0 == ((v1[0] >> 40) & 0xffff));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ldrh_6") {

  const char source[] = "ldrh w0, [x1, w2, UXTW #1]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0x8e060b1505409a1bull};
  ExpectedMemoryAccesses expectedPre = {{
      {((QBDI::rword)&v1[1]) + 2, (v1[1] >> 16) & 0xffff, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRHHroW", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = 0;
  state->x1 = reinterpret_cast<QBDI::rword>(&v1);
  state->x2 = 5;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->x0 == ((v1[1] >> 16) & 0xffff));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ldrh_7") {

  const char source[] = "ldrh w0, [x1, w2, SXTW #0]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0x8e060b1505409a1bull};
  ExpectedMemoryAccesses expectedPre = {{
      {((QBDI::rword)&v1[0]) + 4, (v1[0] >> 32) & 0xffff, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRHHroW", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = 0;
  state->x1 = reinterpret_cast<QBDI::rword>(&v1) + 8;
  state->x2 = -4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->x0 == ((v1[0] >> 32) & 0xffff));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ldrh_8") {

  const char source[] = "ldrh w0, [x1, w2, SXTW #1]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0x8e060b1505409a1bull};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0] & 0xffff, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRHHroW", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = 0;
  state->x1 = reinterpret_cast<QBDI::rword>(&v1) + 8;
  state->x2 = -4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->x0 == (v1[0] & 0xffff));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ldrw_1") {

  const char source[] = "ldr w0, [x1, x2, LSL #0]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0x8e060b1505409a1bull};
  ExpectedMemoryAccesses expectedPre = {{
      {((QBDI::rword)&v1[0]) + 3, (v1[0] >> 24) & 0xffffffff, 4,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRWroX", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = 0;
  state->x1 = reinterpret_cast<QBDI::rword>(&v1);
  state->x2 = 3;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->x0 == ((v1[0] >> 24) & 0xffffffff));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ldrw_2") {

  const char source[] = "ldr w0, [x1, x2, LSL #2]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0x8e060b1505409a1bull,
                      0x2c9f7f51fb7d40ceull};
  ExpectedMemoryAccesses expectedPre = {{
      {((QBDI::rword)&v1[2]) + 4, (v1[2] >> 32) & 0xffffffff, 4,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRWroX", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = 0;
  state->x1 = reinterpret_cast<QBDI::rword>(&v1);
  state->x2 = 5;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->x0 == ((v1[2] >> 32) & 0xffffffff));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ldrw_3") {

  const char source[] = "ldr w0, [x1, x2, SXTX #0]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0x8e060b1505409a1bull};
  ExpectedMemoryAccesses expectedPre = {{
      {((QBDI::rword)&v1[0]) + 4, (v1[0] >> 32) & 0xffffffff, 4,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRWroX", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = 0;
  state->x1 = reinterpret_cast<QBDI::rword>(&v1) + 8;
  state->x2 = -4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->x0 == ((v1[0] >> 32) & 0xffffffff));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ldrw_4") {

  const char source[] = "ldr w0, [x1, x2, SXTX #2]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0x8e060b1505409a1bull};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0] & 0xffffffff, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRWroX", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = 0;
  state->x1 = reinterpret_cast<QBDI::rword>(&v1) + 16;
  state->x2 = -4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->x0 == (v1[0] & 0xffffffff));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ldrxui") {

  const char source[] = "ldr x0, [x1, #8]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0x8e060b1505409a1bull};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[1], v1[1], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRXui", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = 0;
  state->x1 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->x0 == v1[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ldrxpre") {

  const char source[] = "ldr x0, [x1, #8]!\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0x8e060b1505409a1bull};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[1], v1[1], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRXpre", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = 0;
  state->x1 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->x0 == v1[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ldrwui") {

  const char source[] = "ldr w0, [x1, #4]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0x8e060b1505409a1bull};
  ExpectedMemoryAccesses expectedPre = {{
      {((QBDI::rword)&v1[0]) + 4, (v1[0] >> 32) & 0xffffffff, 4,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRWui", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = 0;
  state->x1 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->x0 == ((v1[0] >> 32) & 0xffffffff));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ldrwpre") {

  const char source[] = "ldr w0, [x1, #4]!\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0x8e060b1505409a1bull};
  ExpectedMemoryAccesses expectedPre = {{
      {((QBDI::rword)&v1[0]) + 4, (v1[0] >> 32) & 0xffffffff, 4,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRWpre", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = 0;
  state->x1 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->x0 == ((v1[0] >> 32) & 0xffffffff));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ldrxl") {

  const char source[] =
      "  ldr x0, label\n"
      "  b target\n"
      "label:\n"
      "  brk 65535\n"
      "  brk 65535\n"
      "target:\n"
      "  ret\n";

  QBDI::rword addr = genASM(source);

  ExpectedMemoryAccesses expectedPre = {{
      {addr + 8, 0xD43FFFE0D43FFFE0ull, 8, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRXl", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::rword retval;
  bool ran = vm.call(&retval, addr);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->x0 == 0xD43FFFE0D43FFFE0ull);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ld1rv1d") {

  const char source[] = "ld1r  { v0.1d }, [x0]\n";

  QBDI::rword v1 = 0xab3672016bef61aeull;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1, v1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LD1Rv1d", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::FPRState *fstate = vm.getFPRState();
  setFPR(fstate, 0, 0x946fcddeb64b95d0ull, 0x426ca4a93f5b418bull);
  vm.setFPRState(fstate);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  checkLowFPR(fstate, 0, v1);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ld1rv2d") {

  const char source[] = "ld1r  { v0.2d }, [x0]\n";

  QBDI::rword v1 = 0xab3672016bef61aeull;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1, v1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LD1Rv2d", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::FPRState *fstate = vm.getFPRState();
  setFPR(fstate, 0, 0x946fcddeb64b95d0ull, 0x426ca4a93f5b418bull);
  vm.setFPRState(fstate);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  checkFullFPR(fstate, 0, v1, v1);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ld1rv2s") {

  const char source[] = "ld1r  { v0.2s }, [x0]\n";

  QBDI::rword v1 = 0xab3672016bef61aeull;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1, v1 & 0xffffffff, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LD1Rv2s", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::FPRState *fstate = vm.getFPRState();
  setFPR(fstate, 0, 0x946fcddeb64b95d0ull, 0x426ca4a93f5b418bull);
  vm.setFPRState(fstate);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  checkLowFPR(fstate, 0, (v1 & 0xffffffff) | ((v1 & 0xffffffff) << 32));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ld1rv4h") {

  const char source[] = "ld1r  { v0.4h }, [x0]\n";

  QBDI::rword v1 = 0xab3672016bef61aeull;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1, v1 & 0xffff, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LD1Rv4h", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::FPRState *fstate = vm.getFPRState();
  setFPR(fstate, 0, 0x946fcddeb64b95d0ull, 0x426ca4a93f5b418bull);
  vm.setFPRState(fstate);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  QBDI::rword ev = v1 & 0xffff;
  ev |= (ev << 16);
  ev |= (ev << 32);
  checkLowFPR(fstate, 0, ev);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ld1rv8b") {

  const char source[] = "ld1r  { v0.8b }, [x0]\n";

  QBDI::rword v1 = 0xab3672016bef61aeull;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1, v1 & 0xff, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LD1Rv8b", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::FPRState *fstate = vm.getFPRState();
  setFPR(fstate, 0, 0x946fcddeb64b95d0ull, 0x426ca4a93f5b418bull);
  vm.setFPRState(fstate);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  QBDI::rword ev = v1 & 0xff;
  ev |= (ev << 8);
  ev |= (ev << 16);
  ev |= (ev << 32);
  checkLowFPR(fstate, 0, ev);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ld2rv16b") {

  const char source[] = "ld2r  { v0.16b, v1.16b }, [x0]\n";

  QBDI::rword v1 = 0xab3672016bef61aeull;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1, v1 & 0xffff, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LD2Rv16b", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::FPRState *fstate = vm.getFPRState();
  setFPR(fstate, 0, 0x946fcddeb64b95d0ull, 0x426ca4a93f5b418bull);
  setFPR(fstate, 1, 0xac0b75912b4f4f06ull, 0xdf8f31ec3dd54a56ull);
  vm.setFPRState(fstate);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  QBDI::rword ev1 = v1 & 0xff;
  ev1 |= (ev1 << 8);
  ev1 |= (ev1 << 16);
  ev1 |= (ev1 << 32);
  checkFullFPR(fstate, 0, ev1, ev1);
  QBDI::rword ev2 = (v1 >> 8) & 0xff;
  ev2 |= (ev2 << 8);
  ev2 |= (ev2 << 16);
  ev2 |= (ev2 << 32);
  checkFullFPR(fstate, 1, ev2, ev2);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ld3rv16b") {

  const char source[] = "ld3r  { v0.16b, v1.16b, v2.16b }, [x0]\n";

  QBDI::rword v1 = 0xab3672016bef61aeull;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1, v1 & 0xffffff, 3, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LD3Rv16b", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::FPRState *fstate = vm.getFPRState();
  setFPR(fstate, 0, 0x946fcddeb64b95d0ull, 0x426ca4a93f5b418bull);
  setFPR(fstate, 1, 0xac0b75912b4f4f06ull, 0xdf8f31ec3dd54a56ull);
  setFPR(fstate, 2, 0x9e635973ed71406cull, 0x0f50a5e4aaee4ed0ull);
  vm.setFPRState(fstate);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  QBDI::rword ev1 = v1 & 0xff;
  ev1 |= (ev1 << 8);
  ev1 |= (ev1 << 16);
  ev1 |= (ev1 << 32);
  checkFullFPR(fstate, 0, ev1, ev1);
  QBDI::rword ev2 = (v1 >> 8) & 0xff;
  ev2 |= (ev2 << 8);
  ev2 |= (ev2 << 16);
  ev2 |= (ev2 << 32);
  checkFullFPR(fstate, 1, ev2, ev2);
  QBDI::rword ev3 = (v1 >> 16) & 0xff;
  ev3 |= (ev3 << 8);
  ev3 |= (ev3 << 16);
  ev3 |= (ev3 << 32);
  checkFullFPR(fstate, 2, ev3, ev3);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ld3rv8h") {

  const char source[] = "ld3r  { v0.8h, v1.8h, v2.8h }, [x0]\n";

  QBDI::rword v1 = 0xab3672016bef61aeull;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1, v1 & 0xffffffffffff, 6, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LD3Rv8h", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::FPRState *fstate = vm.getFPRState();
  setFPR(fstate, 0, 0x946fcddeb64b95d0ull, 0x426ca4a93f5b418bull);
  setFPR(fstate, 1, 0xac0b75912b4f4f06ull, 0xdf8f31ec3dd54a56ull);
  setFPR(fstate, 2, 0x9e635973ed71406cull, 0x0f50a5e4aaee4ed0ull);
  vm.setFPRState(fstate);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  QBDI::rword ev1 = v1 & 0xffff;
  ev1 |= (ev1 << 16);
  ev1 |= (ev1 << 32);
  checkFullFPR(fstate, 0, ev1, ev1);
  QBDI::rword ev2 = (v1 >> 16) & 0xffff;
  ev2 |= (ev2 << 16);
  ev2 |= (ev2 << 32);
  checkFullFPR(fstate, 1, ev2, ev2);
  QBDI::rword ev3 = (v1 >> 32) & 0xffff;
  ev3 |= (ev3 << 16);
  ev3 |= (ev3 << 32);
  checkFullFPR(fstate, 2, ev3, ev3);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ld3rv4s") {

  const char source[] = "ld3r  { v0.4s, v1.4s, v2.4s }, [x0]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0x0a18ce5402b84b8aull};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1] & 0xffffffff, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LD3Rv4s", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::FPRState *fstate = vm.getFPRState();
  setFPR(fstate, 0, 0x946fcddeb64b95d0ull, 0x426ca4a93f5b418bull);
  setFPR(fstate, 1, 0xac0b75912b4f4f06ull, 0xdf8f31ec3dd54a56ull);
  setFPR(fstate, 2, 0x9e635973ed71406cull, 0x0f50a5e4aaee4ed0ull);
  vm.setFPRState(fstate);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  QBDI::rword ev1 = v1[0] & 0xffffffff;
  ev1 |= (ev1 << 32);
  checkFullFPR(fstate, 0, ev1, ev1);
  QBDI::rword ev2 = (v1[0] >> 32) & 0xffffffff;
  ev2 |= (ev2 << 32);
  checkFullFPR(fstate, 1, ev2, ev2);
  QBDI::rword ev3 = v1[1] & 0xffffffff;
  ev3 |= (ev3 << 32);
  checkFullFPR(fstate, 2, ev3, ev3);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ld3rv2d") {

  const char source[] = "ld3r  { v0.2d, v1.2d, v2.2d }, [x0]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0x0a18ce5402b84b8aull,
                      0x78385dce3a634b5eull};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LD3Rv2d", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::FPRState *fstate = vm.getFPRState();
  setFPR(fstate, 0, 0x946fcddeb64b95d0ull, 0x426ca4a93f5b418bull);
  setFPR(fstate, 1, 0xac0b75912b4f4f06ull, 0xdf8f31ec3dd54a56ull);
  setFPR(fstate, 2, 0x9e635973ed71406cull, 0x0f50a5e4aaee4ed0ull);
  vm.setFPRState(fstate);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  checkFullFPR(fstate, 0, v1[0], v1[0]);
  checkFullFPR(fstate, 1, v1[1], v1[1]);
  checkFullFPR(fstate, 2, v1[2], v1[2]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ld1Onev8b") {

  const char source[] = "ld1  { v0.8b }, [x0]\n";

  QBDI::rword v1 = 0xab3672016bef61aeull;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1, v1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LD1Onev8b", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::FPRState *fstate = vm.getFPRState();
  setFPR(fstate, 0, 0x946fcddeb64b95d0ull, 0x426ca4a93f5b418bull);
  vm.setFPRState(fstate);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  checkLowFPR(fstate, 0, v1);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ld1Twov8b") {

  const char source[] = "ld1  { v0.8b, v1.8b }, [x0]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0xa5949fc25fcfe5fdull};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LD1Twov8b", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::FPRState *fstate = vm.getFPRState();
  setFPR(fstate, 0, 0x946fcddeb64b95d0ull, 0x426ca4a93f5b418bull);
  setFPR(fstate, 1, 0xac0b75912b4f4f06ull, 0xdf8f31ec3dd54a56ull);
  vm.setFPRState(fstate);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  checkLowFPR(fstate, 0, v1[0]);
  checkLowFPR(fstate, 1, v1[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ld2Twov8b") {

  const char source[] = "ld2  { v0.8b, v1.8b }, [x0]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0xa5949fc25fcfe5fdull};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LD2Twov8b", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::FPRState *fstate = vm.getFPRState();
  setFPR(fstate, 0, 0x946fcddeb64b95d0ull, 0x426ca4a93f5b418bull);
  setFPR(fstate, 1, 0xac0b75912b4f4f06ull, 0xdf8f31ec3dd54a56ull);
  vm.setFPRState(fstate);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  QBDI::rword ev[2] = {0ull, 0ull};
  for (int j = 0; j < 8 * 2; j++) {
    ev[j % 2] |= ((v1[j / 8] >> ((j % 8) * 8)) & 0xff) << ((j / 2) * 8);
  }
  checkLowFPR(fstate, 0, ev[0]);
  checkLowFPR(fstate, 1, ev[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ld1Threev8b") {

  const char source[] = "ld1  { v0.8b, v1.8b, v2.8b }, [x0]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0xa5949fc25fcfe5fdull,
                      0x9265fd6758aa9205ull};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LD1Threev8b", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::FPRState *fstate = vm.getFPRState();
  setFPR(fstate, 0, 0x946fcddeb64b95d0ull, 0x426ca4a93f5b418bull);
  setFPR(fstate, 1, 0xac0b75912b4f4f06ull, 0xdf8f31ec3dd54a56ull);
  setFPR(fstate, 2, 0x9e635973ed71406cull, 0x0f50a5e4aaee4ed0ull);
  vm.setFPRState(fstate);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  checkLowFPR(fstate, 0, v1[0]);
  checkLowFPR(fstate, 1, v1[1]);
  checkLowFPR(fstate, 2, v1[2]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ld3Threev8b") {

  const char source[] = "ld3  { v0.8b, v1.8b, v2.8b }, [x0]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0xa5949fc25fcfe5fdull,
                      0x9265fd6758aa9205ull};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LD3Threev8b", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::FPRState *fstate = vm.getFPRState();
  setFPR(fstate, 0, 0x946fcddeb64b95d0ull, 0x426ca4a93f5b418bull);
  setFPR(fstate, 1, 0xac0b75912b4f4f06ull, 0xdf8f31ec3dd54a56ull);
  setFPR(fstate, 2, 0x9e635973ed71406cull, 0x0f50a5e4aaee4ed0ull);
  vm.setFPRState(fstate);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  QBDI::rword ev[3] = {0ull, 0ull, 0ull};
  for (int j = 0; j < 8 * 3; j++) {
    ev[j % 3] |= ((v1[j / 8] >> ((j % 8) * 8)) & 0xff) << ((j / 3) * 8);
  }
  checkLowFPR(fstate, 0, ev[0]);
  checkLowFPR(fstate, 1, ev[1]);
  checkLowFPR(fstate, 2, ev[2]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ld1Fourv8b") {

  const char source[] = "ld1  { v0.8b, v1.8b, v2.8b, v3.8b }, [x0]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0xa5949fc25fcfe5fdull,
                      0x9265fd6758aa9205ull, 0xb9986f96bec2bbd6ull};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[3], v1[3], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LD1Fourv8b", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::FPRState *fstate = vm.getFPRState();
  setFPR(fstate, 0, 0x946fcddeb64b95d0ull, 0x426ca4a93f5b418bull);
  setFPR(fstate, 1, 0xac0b75912b4f4f06ull, 0xdf8f31ec3dd54a56ull);
  setFPR(fstate, 2, 0x9e635973ed71406cull, 0x0f50a5e4aaee4ed0ull);
  setFPR(fstate, 3, 0xd8a0052563bc4ccaull, 0x1717581f6a684b0full);
  vm.setFPRState(fstate);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  checkLowFPR(fstate, 0, v1[0]);
  checkLowFPR(fstate, 1, v1[1]);
  checkLowFPR(fstate, 2, v1[2]);
  checkLowFPR(fstate, 3, v1[3]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ld4Fourv8b") {

  const char source[] = "ld4  { v0.8b, v1.8b, v2.8b, v3.8b }, [x0]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0xa5949fc25fcfe5fdull,
                      0x9265fd6758aa9205ull, 0xb9986f96bec2bbd6ull};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[3], v1[3], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LD4Fourv8b", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::FPRState *fstate = vm.getFPRState();
  setFPR(fstate, 0, 0x946fcddeb64b95d0ull, 0x426ca4a93f5b418bull);
  setFPR(fstate, 1, 0xac0b75912b4f4f06ull, 0xdf8f31ec3dd54a56ull);
  setFPR(fstate, 2, 0x9e635973ed71406cull, 0x0f50a5e4aaee4ed0ull);
  setFPR(fstate, 3, 0xd8a0052563bc4ccaull, 0x1717581f6a684b0full);
  vm.setFPRState(fstate);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  QBDI::rword ev[4] = {0ull, 0ull, 0ull, 0ull};
  for (int j = 0; j < 8 * 4; j++) {
    ev[j % 4] |= ((v1[j / 8] >> ((j % 8) * 8)) & 0xff) << ((j / 4) * 8);
  }
  checkLowFPR(fstate, 0, ev[0]);
  checkLowFPR(fstate, 1, ev[1]);
  checkLowFPR(fstate, 2, ev[2]);
  checkLowFPR(fstate, 3, ev[3]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ld1Fourv16b") {

  const char source[] = "ld1  { v0.16b, v1.16b, v2.16b, v3.16b }, [x0]\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0xa5949fc25fcfe5fdull,
                      0x9265fd6758aa9205ull, 0xb9986f96bec2bbd6ull,
                      0xf1410141b7604984ull, 0xfede641155b54c9dull,
                      0x0a91424a2ff449b4ull, 0x6c9e77ad310f47abull};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[3], v1[3], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[4], v1[4], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[5], v1[5], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[6], v1[6], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[7], v1[7], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LD1Fourv16b", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::FPRState *fstate = vm.getFPRState();
  setFPR(fstate, 0, 0x946fcddeb64b95d0ull, 0x426ca4a93f5b418bull);
  setFPR(fstate, 1, 0xac0b75912b4f4f06ull, 0xdf8f31ec3dd54a56ull);
  setFPR(fstate, 2, 0x9e635973ed71406cull, 0x0f50a5e4aaee4ed0ull);
  setFPR(fstate, 3, 0xd8a0052563bc4ccaull, 0x1717581f6a684b0full);
  vm.setFPRState(fstate);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  checkFullFPR(fstate, 0, v1[1], v1[0]);
  checkFullFPR(fstate, 1, v1[3], v1[2]);
  checkFullFPR(fstate, 2, v1[5], v1[4]);
  checkFullFPR(fstate, 3, v1[7], v1[6]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ld1Onev8b_POST") {

  const char source[] = "ld1  { v0.8b }, [x0], #8\n";

  QBDI::rword v1 = 0xab3672016bef61aeull;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1, v1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LD1Onev8b_POST", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::FPRState *fstate = vm.getFPRState();
  setFPR(fstate, 0, 0x946fcddeb64b95d0ull, 0x426ca4a93f5b418bull);
  vm.setFPRState(fstate);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  checkLowFPR(fstate, 0, v1);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ld1Fourv8b_POST") {

  const char source[] = "ld1  { v0.8b, v1.8b, v2.8b, v3.8b }, [x0], #32\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0xa5949fc25fcfe5fdull,
                      0x9265fd6758aa9205ull, 0xb9986f96bec2bbd6ull};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[3], v1[3], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LD1Fourv8b_POST", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::FPRState *fstate = vm.getFPRState();
  setFPR(fstate, 0, 0x946fcddeb64b95d0ull, 0x426ca4a93f5b418bull);
  setFPR(fstate, 1, 0xac0b75912b4f4f06ull, 0xdf8f31ec3dd54a56ull);
  setFPR(fstate, 2, 0x9e635973ed71406cull, 0x0f50a5e4aaee4ed0ull);
  setFPR(fstate, 3, 0xd8a0052563bc4ccaull, 0x1717581f6a684b0full);
  vm.setFPRState(fstate);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  checkLowFPR(fstate, 0, v1[0]);
  checkLowFPR(fstate, 1, v1[1]);
  checkLowFPR(fstate, 2, v1[2]);
  checkLowFPR(fstate, 3, v1[3]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ld1Fourv1d_POST") {

  const char source[] = "ld1  { v0.1d, v1.1d, v2.1d, v3.1d }, [x0], #32\n";

  QBDI::rword v1[] = {0xab3672016bef61aeull, 0xa5949fc25fcfe5fdull,
                      0x9265fd6758aa9205ull, 0xb9986f96bec2bbd6ull};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[3], v1[3], 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LD1Fourv1d_POST", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::FPRState *fstate = vm.getFPRState();
  setFPR(fstate, 0, 0x946fcddeb64b95d0ull, 0x426ca4a93f5b418bull);
  setFPR(fstate, 1, 0xac0b75912b4f4f06ull, 0xdf8f31ec3dd54a56ull);
  setFPR(fstate, 2, 0x9e635973ed71406cull, 0x0f50a5e4aaee4ed0ull);
  setFPR(fstate, 3, 0xd8a0052563bc4ccaull, 0x1717581f6a684b0full);
  vm.setFPRState(fstate);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  checkLowFPR(fstate, 0, v1[0]);
  checkLowFPR(fstate, 1, v1[1]);
  checkLowFPR(fstate, 2, v1[2]);
  checkLowFPR(fstate, 3, v1[3]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ld1i64_POST") {

  const char source[] = "ld1  { v0.d }[0], [x0], #8\n";

  QBDI::rword v1 = 0xab3672016bef61aeull;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1, v1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LD1i64_POST", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::FPRState *fstate = vm.getFPRState();
  setFPR(fstate, 0, 0x946fcddeb64b95d0ull, 0x426ca4a93f5b418bull);
  vm.setFPRState(fstate);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  checkFullFPR(fstate, 0, 0x946fcddeb64b95d0ull, v1);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ld1i64_POST_2") {

  const char source[] = "ld1  { v0.d }[1], [x0], #8\n";

  QBDI::rword v1 = 0xab3672016bef61aeull;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1, v1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LD1i64_POST", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::FPRState *fstate = vm.getFPRState();
  setFPR(fstate, 0, 0x946fcddeb64b95d0ull, 0x426ca4a93f5b418bull);
  vm.setFPRState(fstate);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  checkFullFPR(fstate, 0, v1, 0x426ca4a93f5b418bull);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ld1i8_POST") {

  const char source[] = "ld1  { v0.b }[3], [x0], #1\n";

  QBDI::rword v1 = 0xab3672016bef61aeull;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1, v1 & 0xff, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LD1i8_POST", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::FPRState *fstate = vm.getFPRState();
  setFPR(fstate, 0, 0x946fcddeb64b95d0ull, 0x426ca4a93f5b418bull);
  vm.setFPRState(fstate);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  checkFullFPR(fstate, 0, 0x946fcddeb64b95d0ull,
               0x426ca4a9005b418bull | ((v1 & 0xff) << (3 * 8)));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ld4i8_POST") {

  const char source[] = "ld4  { v0.b, v1.b, v2.b, v3.b }[3], [x0], #4\n";

  QBDI::rword v1 = 0xab3672016bef61aeull;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1, v1 & 0xffffffff, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LD4i8_POST", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::FPRState *fstate = vm.getFPRState();
  setFPR(fstate, 0, 0x946fcddeb64b95d0ull, 0x426ca4a93f5b418bull);
  setFPR(fstate, 1, 0xac0b75912b4f4f06ull, 0xdf8f31ec3dd54a56ull);
  setFPR(fstate, 2, 0x9e635973ed71406cull, 0x0f50a5e4aaee4ed0ull);
  setFPR(fstate, 3, 0xd8a0052563bc4ccaull, 0x1717581f6a684b0full);
  vm.setFPRState(fstate);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  checkFullFPR(fstate, 0, 0x946fcddeb64b95d0ull,
               0x426ca4a9005b418bull | ((v1 & 0xff) << (3 * 8)));
  checkFullFPR(fstate, 1, 0xac0b75912b4f4f06ull,
               0xdf8f31ec00d54a56ull | (((v1 >> 8) & 0xff) << (3 * 8)));
  checkFullFPR(fstate, 2, 0x9e635973ed71406cull,
               0x0f50a5e400ee4ed0ull | (((v1 >> 16) & 0xff) << (3 * 8)));
  checkFullFPR(fstate, 3, 0xd8a0052563bc4ccaull,
               0x1717581f00684b0full | (((v1 >> 24) & 0xff) << (3 * 8)));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-st1Threev8b") {

  const char source[] = "st1  { v0.8b, v1.8b, v2.8b }, [x0]\n";

  QBDI::rword v[] = {0xab3672016bef61aeull, 0xa5949fc25fcfe5fdull,
                     0x9265fd6758aa9205ull};
  QBDI::rword dest[] = {0ull, 0ull, 0ull};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&dest[0], v[0], 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&dest[1], v[1], 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&dest[2], v[2], 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ST1Threev8b", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = reinterpret_cast<QBDI::rword>(&dest);
  vm.setGPRState(state);

  QBDI::FPRState *fstate = vm.getFPRState();
  setFPR(fstate, 0, 0x946fcddeb64b95d0ull, v[0]);
  setFPR(fstate, 1, 0xac0b75912b4f4f06ull, v[1]);
  setFPR(fstate, 2, 0x9e635973ed71406cull, v[2]);
  vm.setFPRState(fstate);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v[0] == dest[0]);
  CHECK(v[1] == dest[1]);
  CHECK(v[2] == dest[2]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-st1Threev8b_POST") {

  const char source[] = "st1  { v0.8b, v1.8b, v2.8b }, [x0], #24\n";

  QBDI::rword v[] = {0xab3672016bef61aeull, 0xa5949fc25fcfe5fdull,
                     0x9265fd6758aa9205ull};
  QBDI::rword dest[] = {0ull, 0ull, 0ull};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&dest[0], v[0], 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&dest[1], v[1], 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&dest[2], v[2], 8, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ST1Threev8b_POST", QBDI::POSTINST, checkAccess,
                   &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = reinterpret_cast<QBDI::rword>(&dest);
  vm.setGPRState(state);

  QBDI::FPRState *fstate = vm.getFPRState();
  setFPR(fstate, 0, 0x946fcddeb64b95d0ull, v[0]);
  setFPR(fstate, 1, 0xac0b75912b4f4f06ull, v[1]);
  setFPR(fstate, 2, 0x9e635973ed71406cull, v[2]);
  vm.setFPRState(fstate);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v[0] == dest[0]);
  CHECK(v[1] == dest[1]);
  CHECK(v[2] == dest[2]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-swp") {

  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "swp x0, x1, [x2]\n";

  QBDI::rword v1 = 0xab3672016bef61aeull;
  QBDI::rword v2 = 0x8e060b1505409a1bull;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1, v1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1, v2, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SWPX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SWPX", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = v2;
  state->x1 = 0;
  state->x2 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_AARCH64-ldaddb") {

  if (!checkFeature("lse")) {
    return;
  }

  const char source[] = "ldaddb w0, w1, [x2]\n";

  QBDI::rword v1 = 0xab3672016bef61aeull;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1, v1 & 0xff, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v1 & 0xff, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1, ((v1 & 0xff) + 3) % 256, 1, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDADDB", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LDADDB", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->x0 = 3;
  state->x1 = 0;
  state->x2 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::DEFAULT, {"lse"});

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}
