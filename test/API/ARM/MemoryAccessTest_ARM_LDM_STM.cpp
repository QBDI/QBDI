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

[[maybe_unused]] static QBDI::VMAction debugCB(QBDI::VMInstanceRef vm,
                                               QBDI::GPRState *gprState,
                                               QBDI::FPRState *fprState,
                                               void *data) {
  const QBDI::InstAnalysis *instAnalysis = vm->getInstAnalysis();
  printf("0x%x (%10s): %s\n", instAnalysis->address, instAnalysis->mnemonic,
         instAnalysis->disassembly);

  for (auto &a : vm->getInstMemoryAccess()) {
    printf(
        " - inst: 0x%x, addr: 0x%x, size: %d, type: %c%c, "
        "value: 0x%x, flags : 0x %x\n",
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

// LDMIA
// =====

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldmia1") {

  const char source[] = "ldmia r0, {r1, r2}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDMIA", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v1[0]);
  CHECK(state->r2 == v1[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldmia2") {

  const char source[] = "ldmia r0, {r0-r12,sp,lr,pc}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0x31eed260, 0xf21a4416,
                      0x319b8e1b, 0x215f4510, 0x927e556e, 0xa8a0e729,
                      0x719ca3c1, 0xd7b24369, 0xb25e4516, 0x235b2fc3,
                      0xc2708a8b, 0x2d624053, 0xaad33b87, 0x2a};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[3], v1[3], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[4], v1[4], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[5], v1[5], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[6], v1[6], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[7], v1[7], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[8], v1[8], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[9], v1[9], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[10], v1[10], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[11], v1[11], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[12], v1[12], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[13], v1[13], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[14], v1[14], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[15], v1[15], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDMIA", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  for (int i = 0; i < 16; i++) {
    INFO("Offset " << i);
    CHECK(v1[i] == QBDI_GPR_GET(state, i));
  }
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldmia3") {

  const char source[] = "cmp r1, #42; ldmiane r0, {r1, r2, pc}\n";

  // with condition reach
  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0x2a};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDMIA", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v1[0]);
  CHECK(state->r2 == v1[1]);

  // with condition not reach
  expectedPre = {{}};
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = 42;
  vm.setGPRState(state);

  ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(not e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == 42);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldmia_post1") {

  const char source[] = "ldmia r0!, {r1, r2}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDMIA_UPD", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v1[0]);
  CHECK(state->r2 == v1[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldmia_post2") {

  const char source[] = "ldmia r0!, {r1-r12,sp,lr,pc}\n";

  QBDI::rword v1[] = {0x747f4b7e, 0x31eed260, 0xf21a4416, 0x319b8e1b,
                      0x215f4510, 0x927e556e, 0xa8a0e729, 0x719ca3c1,
                      0xd7b24369, 0xb25e4516, 0x235b2fc3, 0xc2708a8b,
                      0x2d624053, 0xaad33b87, 0x2a};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[3], v1[3], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[4], v1[4], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[5], v1[5], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[6], v1[6], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[7], v1[7], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[8], v1[8], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[9], v1[9], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[10], v1[10], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[11], v1[11], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[12], v1[12], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[13], v1[13], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[14], v1[14], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDMIA_UPD", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  for (int i = 0; i < 15; i++) {
    INFO("Offset " << i);
    CHECK(v1[i] == QBDI_GPR_GET(state, i + 1));
  }
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldmia_post3") {

  const char source[] = "cmp r1, #42; ldmiane r0!, {r1, r2, pc}\n";

  // with condition reach
  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0x2a};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDMIA_UPD", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v1[0]);
  CHECK(state->r2 == v1[1]);

  // with condition not reach
  expectedPre = {{}};
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = 42;
  vm.setGPRState(state);

  ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(not e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == 42);
}

// LDMIB
// =====

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldmib1") {

  const char source[] = "ldmib r0, {r1, r2}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDMIB", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v1[0]);
  CHECK(state->r2 == v1[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldmib2") {

  const char source[] = "ldmib r0, {r0-r12,sp,lr,pc}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0x31eed260, 0xf21a4416,
                      0x319b8e1b, 0x215f4510, 0x927e556e, 0xa8a0e729,
                      0x719ca3c1, 0xd7b24369, 0xb25e4516, 0x235b2fc3,
                      0xc2708a8b, 0x2d624053, 0xaad33b87, 0x2a};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[3], v1[3], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[4], v1[4], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[5], v1[5], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[6], v1[6], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[7], v1[7], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[8], v1[8], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[9], v1[9], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[10], v1[10], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[11], v1[11], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[12], v1[12], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[13], v1[13], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[14], v1[14], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[15], v1[15], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDMIB", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  for (int i = 0; i < 16; i++) {
    INFO("Offset " << i);
    CHECK(v1[i] == QBDI_GPR_GET(state, i));
  }
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldmib3") {

  const char source[] = "cmp r1, #42; ldmibne r0, {r1, r2, pc}\n";

  // with condition reach
  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0x2a};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDMIB", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 4;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v1[0]);
  CHECK(state->r2 == v1[1]);

  // with condition not reach
  expectedPre = {{}};
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 4;
  state->r1 = 42;
  vm.setGPRState(state);

  ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(not e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == 42);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldmib_post1") {

  const char source[] = "ldmib r0!, {r1, r2}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDMIB_UPD", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v1[0]);
  CHECK(state->r2 == v1[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldmib_post2") {

  const char source[] = "ldmib r0!, {r1-r12,sp,lr,pc}\n";

  QBDI::rword v1[] = {0x747f4b7e, 0x31eed260, 0xf21a4416, 0x319b8e1b,
                      0x215f4510, 0x927e556e, 0xa8a0e729, 0x719ca3c1,
                      0xd7b24369, 0xb25e4516, 0x235b2fc3, 0xc2708a8b,
                      0x2d624053, 0xaad33b87, 0x2a};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[3], v1[3], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[4], v1[4], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[5], v1[5], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[6], v1[6], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[7], v1[7], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[8], v1[8], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[9], v1[9], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[10], v1[10], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[11], v1[11], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[12], v1[12], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[13], v1[13], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[14], v1[14], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDMIB_UPD", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  for (int i = 0; i < 15; i++) {
    INFO("Offset " << i);
    CHECK(v1[i] == QBDI_GPR_GET(state, i + 1));
  }
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldmib_post3") {

  const char source[] = "cmp r1, #42; ldmibne r0!, {r1, r2, pc}\n";

  // with condition reach
  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0x2a};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDMIB_UPD", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 4;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v1[0]);
  CHECK(state->r2 == v1[1]);

  // with condition not reach
  expectedPre = {{}};
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 4;
  state->r1 = 42;
  vm.setGPRState(state);

  ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(not e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == 42);
}

// LDMDA
// =====

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldmda1") {

  const char source[] = "ldmda r0, {r1, r2}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDMDA", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v1[0]);
  CHECK(state->r2 == v1[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldmda2") {

  const char source[] = "ldmda r0, {r0-r12,sp,lr,pc}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0x31eed260, 0xf21a4416,
                      0x319b8e1b, 0x215f4510, 0x927e556e, 0xa8a0e729,
                      0x719ca3c1, 0xd7b24369, 0xb25e4516, 0x235b2fc3,
                      0xc2708a8b, 0x2d624053, 0xaad33b87, 0x2a};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[3], v1[3], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[4], v1[4], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[5], v1[5], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[6], v1[6], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[7], v1[7], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[8], v1[8], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[9], v1[9], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[10], v1[10], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[11], v1[11], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[12], v1[12], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[13], v1[13], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[14], v1[14], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[15], v1[15], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDMDA", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 4 * 15;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  for (int i = 0; i < 16; i++) {
    INFO("Offset " << i);
    CHECK(v1[i] == QBDI_GPR_GET(state, i));
  }
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldmda3") {

  const char source[] = "cmp r1, #42; ldmdane r0, {r1, r2, pc}\n";

  // with condition reach
  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0x2a};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDMDA", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 8;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v1[0]);
  CHECK(state->r2 == v1[1]);

  // with condition not reach
  expectedPre = {{}};
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 8;
  state->r1 = 42;
  vm.setGPRState(state);

  ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(not e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == 42);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldmda_post1") {

  const char source[] = "ldmda r0!, {r1, r2}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDMDA_UPD", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v1[0]);
  CHECK(state->r2 == v1[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldmda_post2") {

  const char source[] = "ldmda r0!, {r1-r12,sp,lr,pc}\n";

  QBDI::rword v1[] = {0x747f4b7e, 0x31eed260, 0xf21a4416, 0x319b8e1b,
                      0x215f4510, 0x927e556e, 0xa8a0e729, 0x719ca3c1,
                      0xd7b24369, 0xb25e4516, 0x235b2fc3, 0xc2708a8b,
                      0x2d624053, 0xaad33b87, 0x2a};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[3], v1[3], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[4], v1[4], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[5], v1[5], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[6], v1[6], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[7], v1[7], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[8], v1[8], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[9], v1[9], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[10], v1[10], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[11], v1[11], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[12], v1[12], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[13], v1[13], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[14], v1[14], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDMDA_UPD", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 4 * 14;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  for (int i = 0; i < 15; i++) {
    INFO("Offset " << i);
    CHECK(v1[i] == QBDI_GPR_GET(state, i + 1));
  }
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldmda_post3") {

  const char source[] = "cmp r1, #42; ldmdane r0!, {r1, r2, pc}\n";

  // with condition reach
  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0x2a};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDMDA_UPD", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 8;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v1[0]);
  CHECK(state->r2 == v1[1]);

  // with condition not reach
  expectedPre = {{}};
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 8;
  state->r1 = 42;
  vm.setGPRState(state);

  ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(not e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == 42);
}

// LDMDB
// =====

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldmdb1") {

  const char source[] = "ldmdb r0, {r1, r2}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDMDB", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 8;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v1[0]);
  CHECK(state->r2 == v1[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldmdb2") {

  const char source[] = "ldmdb r0, {r0-r12,sp,lr,pc}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0x31eed260, 0xf21a4416,
                      0x319b8e1b, 0x215f4510, 0x927e556e, 0xa8a0e729,
                      0x719ca3c1, 0xd7b24369, 0xb25e4516, 0x235b2fc3,
                      0xc2708a8b, 0x2d624053, 0xaad33b87, 0x2a};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[3], v1[3], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[4], v1[4], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[5], v1[5], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[6], v1[6], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[7], v1[7], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[8], v1[8], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[9], v1[9], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[10], v1[10], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[11], v1[11], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[12], v1[12], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[13], v1[13], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[14], v1[14], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[15], v1[15], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDMDB", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 4 * 16;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  for (int i = 0; i < 16; i++) {
    INFO("Offset " << i);
    CHECK(v1[i] == QBDI_GPR_GET(state, i));
  }
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldmdb3") {

  const char source[] = "cmp r1, #42; ldmdbne r0, {r1, r2, pc}\n";

  // with condition reach
  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0x2a};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDMDB", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 12;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v1[0]);
  CHECK(state->r2 == v1[1]);

  // with condition not reach
  expectedPre = {{}};
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 12;
  state->r1 = 42;
  vm.setGPRState(state);

  ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(not e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == 42);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldmdb_post1") {

  const char source[] = "ldmdb r0!, {r1, r2}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDMDB_UPD", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 8;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v1[0]);
  CHECK(state->r2 == v1[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldmdb_post2") {

  const char source[] = "ldmdb r0!, {r1-r12,sp,lr,pc}\n";

  QBDI::rword v1[] = {0x747f4b7e, 0x31eed260, 0xf21a4416, 0x319b8e1b,
                      0x215f4510, 0x927e556e, 0xa8a0e729, 0x719ca3c1,
                      0xd7b24369, 0xb25e4516, 0x235b2fc3, 0xc2708a8b,
                      0x2d624053, 0xaad33b87, 0x2a};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[3], v1[3], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[4], v1[4], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[5], v1[5], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[6], v1[6], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[7], v1[7], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[8], v1[8], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[9], v1[9], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[10], v1[10], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[11], v1[11], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[12], v1[12], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[13], v1[13], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[14], v1[14], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDMDB_UPD", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 4 * 15;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  for (int i = 0; i < 15; i++) {
    INFO("Offset " << i);
    CHECK(v1[i] == QBDI_GPR_GET(state, i + 1));
  }
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldmdb_post3") {

  const char source[] = "cmp r1, #42; ldmdbne r0!, {r1, r2, pc}\n";

  // with condition reach
  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0x2a};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDMDB_UPD", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 12;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v1[0]);
  CHECK(state->r2 == v1[1]);

  // with condition not reach
  expectedPre = {{}};
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 12;
  state->r1 = 42;
  vm.setGPRState(state);

  ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(not e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == 42);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-stmia1") {

  const char source[] = "stmia r0, {r1, r2}\n";

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STMIA", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = v[0];
  state->r2 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-stmia2") {

  const char source[] = "stmia r0, {r1, r2, pc}\n";
  QBDI::rword codeAddr = genASM(source);

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e, codeAddr + 8};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v[2], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STMIA", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = v[0];
  state->r2 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-stmia3") {

  const char source[] = "cmp r3, #42; stmiane r0, {r1, r2, pc}\n";
  QBDI::rword codeAddr = genASM(source);

  // with condition reach
  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e, codeAddr + 12};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v[2], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STMIA", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = v[0];
  state->r2 = v[1];
  state->r3 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);

  // with condition not reach
  expectedPost = {{}};
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = v[0] + 1;
  state->r2 = v[1] + 1;
  state->r3 = 42;
  vm.setGPRState(state);

  ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(memcmp(v, v1, sizeof(v)) == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-stmia_post1") {

  const char source[] = "stmia r0!, {r1, r2}\n";

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STMIA_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = v[0];
  state->r2 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-stmia_post2") {

  const char source[] = "stmia r0!, {r1, r2, pc}\n";
  QBDI::rword codeAddr = genASM(source);

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e, codeAddr + 8};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v[2], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STMIA_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = v[0];
  state->r2 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-stmib1") {

  const char source[] = "stmib r0, {r1, r2}\n";

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STMIB", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 4;
  state->r1 = v[0];
  state->r2 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-stmib2") {

  const char source[] = "stmib r0, {r1, r2, pc}\n";
  QBDI::rword codeAddr = genASM(source);

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e, codeAddr + 8};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v[2], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STMIB", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 4;
  state->r1 = v[0];
  state->r2 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-stmib_post1") {

  const char source[] = "stmib r0!, {r1, r2}\n";

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STMIB_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 4;
  state->r1 = v[0];
  state->r2 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-stmib_post2") {

  const char source[] = "stmib r0!, {r1, r2, pc}\n";
  QBDI::rword codeAddr = genASM(source);

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e, codeAddr + 8};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v[2], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STMIB_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 4;
  state->r1 = v[0];
  state->r2 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-stmda1") {

  const char source[] = "stmda r0, {r1, r2}\n";

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STMDA", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 4;
  state->r1 = v[0];
  state->r2 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-stmda2") {

  const char source[] = "stmda r0, {r1, r2, pc}\n";
  QBDI::rword codeAddr = genASM(source);

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e, codeAddr + 8};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v[2], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STMDA", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 8;
  state->r1 = v[0];
  state->r2 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-stmda_post1") {

  const char source[] = "stmda r0!, {r1, r2}\n";

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STMDA_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 4;
  state->r1 = v[0];
  state->r2 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-stmda_post2") {

  const char source[] = "stmda r0!, {r1, r2, pc}\n";
  QBDI::rword codeAddr = genASM(source);

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e, codeAddr + 8};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v[2], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STMDA_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 8;
  state->r1 = v[0];
  state->r2 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-stmdb1") {

  const char source[] = "stmdb r0, {r1, r2}\n";

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STMDB", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 8;
  state->r1 = v[0];
  state->r2 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-stmdb2") {

  const char source[] = "stmdb r0, {r1, r2, pc}\n";
  QBDI::rword codeAddr = genASM(source);

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e, codeAddr + 8};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v[2], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STMDB", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 12;
  state->r1 = v[0];
  state->r2 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-stmdb_post1") {

  const char source[] = "stmdb r0!, {r1, r2}\n";

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STMDB_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 8;
  state->r1 = v[0];
  state->r2 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-stmdb_post2") {

  const char source[] = "stmdb r0!, {r1, r2, pc}\n";
  QBDI::rword codeAddr = genASM(source);

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e, codeAddr + 8};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v[2], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STMDB_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 12;
  state->r1 = v[0];
  state->r2 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-vldmdia1") {

  const char source[] = "vldmia r0, {d0, d1}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0xd7b24369, 0xb25e4516};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[3], v1[3], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VLDMDIA", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  CHECK(memcmp(fpr, &v1, sizeof(v1)) == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-vldmdia2") {

  const char source[] = "vldmia r0!, {d0-d15}\n";

  QBDI::rword v1[32] = {0};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], 0, 128, QBDI::MEMORY_READ,
       QBDI::MEMORY_UNKNOWN_VALUE},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VLDMDIA_UPD", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  CHECK(memcmp(fpr, &v1, sizeof(v1)) == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-vldmsia1") {

  const char source[] = "vldmia r0, {s0-s2}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0xd7b24369};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VLDMSIA", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  CHECK(memcmp(fpr, &v1, sizeof(v1)) == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-vldmsia2") {

  const char source[] = "vldmia r0!, {s0-s31}\n";

  QBDI::rword v1[32] = {0};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], 0, 128, QBDI::MEMORY_READ,
       QBDI::MEMORY_UNKNOWN_VALUE},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VLDMSIA_UPD", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  CHECK(memcmp(fpr, &v1, sizeof(v1)) == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-vstmdia1") {

  const char source[] = "vstmia r0, {d0, d1}\n";

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e, 0xd7b24369, 0xb25e4516};
  QBDI::rword v1[] = {0, 0, 0, 0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v[2], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[3], v[3], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VSTMDIA", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  memcpy(fpr, v, sizeof(v));
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(&v, &v1, sizeof(v1)) == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-vstmdia2") {

  const char source[] = "vstmia r0!, {d0-d15}\n";

  QBDI::rword v[32] = {
      0x5de254a1, 0x747f4b7e, 0x31eed260, 0xf21a4416, 0x319b8e1b, 0x215f4510,
      0x927e556e, 0xa8a0e729, 0x719ca3c1, 0xd7b24369, 0xb25e4516, 0x235b2fc3,
      0xc2708a8b, 0x2d624053, 0xaad33b87, 0x33562724, 0x215f4510, 0x927e556e,
      0xa8a0e729, 0x235b2fc3, 0xc2708a8b, 0x2d624053, 0xaad33b87, 0x33562724,
      0x719ca3c1, 0xd7b24369, 0xb25e4516, 0x319b8e1b, 0x31eed260, 0x747f4b7e,
      0xc2708a8b, 0xf21a4416};
  QBDI::rword v1[32] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], 0, 128, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_UNKNOWN_VALUE},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VSTMDIA_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  memcpy(fpr, v, sizeof(v));
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(&v, &v1, sizeof(v1)) == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-vstmsia1") {

  const char source[] = "vstmia r0, {s0-s2}\n";

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e, 0xd7b24369};
  QBDI::rword v1[] = {0, 0, 0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v[2], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VSTMSIA", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  memcpy(fpr, v, sizeof(v));
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(&v, &v1, sizeof(v1)) == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-vstmsia2") {

  const char source[] = "vstmia r0!, {s0-s31}\n";

  QBDI::rword v[32] = {
      0x5de254a1, 0x747f4b7e, 0x31eed260, 0xf21a4416, 0x319b8e1b, 0x215f4510,
      0x927e556e, 0xa8a0e729, 0x719ca3c1, 0xd7b24369, 0xb25e4516, 0x235b2fc3,
      0xc2708a8b, 0x2d624053, 0xaad33b87, 0x33562724, 0x215f4510, 0x927e556e,
      0xa8a0e729, 0x235b2fc3, 0xc2708a8b, 0x2d624053, 0xaad33b87, 0x33562724,
      0x719ca3c1, 0xd7b24369, 0xb25e4516, 0x319b8e1b, 0x31eed260, 0x747f4b7e,
      0xc2708a8b, 0xf21a4416};
  QBDI::rword v1[32] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], 0, 128, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_UNKNOWN_VALUE},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VSTMSIA_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::FPRState *fpr = vm.getFPRState();
  memcpy(fpr, v, sizeof(v));
  vm.setFPRState(fpr);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(&v, &v1, sizeof(v1)) == 0);
}
