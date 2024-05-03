/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2024 Quarkslab
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

// tLDMIA
// ======

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-tldmia1") {

  const char source[] = "ldmia r0, {r0, r1}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tLDMIA", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == v1[0]);
  CHECK(state->r1 == v1[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-tldmia2") {

  const char source[] = "ldmia r0, {r0-r7}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0x31eed260, 0xf21a4416,
                      0x319b8e1b, 0x215f4510, 0x927e556e, 0xa8a0e729};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[3], v1[3], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[4], v1[4], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[5], v1[5], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[6], v1[6], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[7], v1[7], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tLDMIA", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  for (unsigned int i = 0; i < 8; i++) {
    INFO("Offset " << i);
    CHECK(v1[i] == QBDI_GPR_GET(state, i));
  }
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-tldmia_post1") {

  const char source[] = "ldmia r0!, {r1, r2}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tLDMIA", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1) + 8);
  CHECK(state->r1 == v1[0]);
  CHECK(state->r2 == v1[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-tldmia_post2") {

  const char source[] = "ldmia r0!, {r1-r7}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0x31eed260, 0xf21a4416,
                      0x319b8e1b, 0x215f4510, 0x927e556e};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[3], v1[3], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[4], v1[4], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[5], v1[5], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[6], v1[6], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tLDMIA", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1) + 28);
  for (unsigned int i = 0; i < 7; i++) {
    INFO("Offset " << i);
    CHECK(v1[i] == QBDI_GPR_GET(state, i + 1));
  }
}

// t2LDMIA
// =======

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-t2ldmia1") {

  const char source[] = "ldmia.w r0, {r1, r2}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDMIA", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v1[0]);
  CHECK(state->r2 == v1[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-t2ldmia2") {

  const char source[] = "ldmia.w r0, {r0-r12,lr}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0x31eed260, 0xf21a4416,
                      0x319b8e1b, 0x215f4510, 0x927e556e, 0xa8a0e729,
                      0x719ca3c1, 0xd7b24369, 0xb25e4516, 0x235b2fc3,
                      0xc2708a8b, 0x2a};
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
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDMIA", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  for (unsigned int i = 0; i < 13; i++) {
    INFO("Offset " << i);
    CHECK(v1[i] == QBDI_GPR_GET(state, i));
  }
  CHECK(v1[13] == state->lr);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-t2ldmia3") {

  const char source[] = "ldmia.w r0, {r0-r12,pc}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0x31eed260, 0xf21a4416,
                      0x319b8e1b, 0x215f4510, 0x927e556e, 0xa8a0e729,
                      0x719ca3c1, 0xd7b24369, 0xb25e4516, 0x235b2fc3,
                      0xc2708a8b, 0x2a};
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
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDMIA", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  for (unsigned int i = 0; i < 13; i++) {
    INFO("Offset " << i);
    CHECK(v1[i] == QBDI_GPR_GET(state, i));
  }
  CHECK(v1[13] == state->pc);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-t2ldmia4") {

  const char source[] = "cmp r1, #42; it ne; ldmiane.w r0, {r1, r2, pc}\n";

  // with condition reach
  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0x2a};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDMIA", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

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

  ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(not e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == 42);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-t2ldmia_post1") {

  const char source[] = "ldmia.w r0!, {r1, r2}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDMIA_UPD", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v1[0]);
  CHECK(state->r2 == v1[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-t2ldmia_post2") {

  const char source[] = "ldmia.w r0!, {r1-r12,lr}\n";

  QBDI::rword v1[] = {0x747f4b7e, 0x31eed260, 0xf21a4416, 0x319b8e1b,
                      0x215f4510, 0x927e556e, 0xa8a0e729, 0x719ca3c1,
                      0xd7b24369, 0xb25e4516, 0x235b2fc3, 0xc2708a8b,
                      0x2a};
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
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDMIA_UPD", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1) + 4 * 13);
  for (unsigned int i = 0; i < 12; i++) {
    INFO("Offset " << i);
    CHECK(v1[i] == QBDI_GPR_GET(state, i + 1));
  }
  CHECK(v1[12] == state->lr);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-t2ldmia_post3") {

  const char source[] = "ldmia.w r0!, {r1-r12,pc}\n";

  QBDI::rword v1[] = {0x747f4b7e, 0x31eed260, 0xf21a4416, 0x319b8e1b,
                      0x215f4510, 0x927e556e, 0xa8a0e729, 0x719ca3c1,
                      0xd7b24369, 0xb25e4516, 0x235b2fc3, 0xc2708a8b,
                      0x2a};
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
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDMIA_UPD", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1) + 4 * 13);
  for (unsigned int i = 0; i < 12; i++) {
    INFO("Offset " << i);
    CHECK(v1[i] == QBDI_GPR_GET(state, i + 1));
  }
  CHECK(v1[12] == state->pc);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-t2ldmia_post4") {

  const char source[] = "cmp r1, #42; it ne; ldmiane r0!, {r1, r2, pc}\n";

  // with condition reach
  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0x2a};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDMIA_UPD", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

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

  ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(not e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == 42);
}

// t2LDMDB
// =======

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-t2ldmdb1") {

  const char source[] = "ldmdb r0, {r1, r2}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDMDB", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 8;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v1[0]);
  CHECK(state->r2 == v1[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-t2ldmdb2") {

  const char source[] = "ldmdb r0, {r0-r12,lr}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0x31eed260, 0xf21a4416,
                      0x319b8e1b, 0x215f4510, 0x927e556e, 0xa8a0e729,
                      0x719ca3c1, 0xd7b24369, 0xb25e4516, 0x235b2fc3,
                      0xaad33b87, 0x2a};
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
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDMDB", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 4 * 14;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  for (unsigned int i = 0; i < 13; i++) {
    INFO("Offset " << i);
    CHECK(v1[i] == QBDI_GPR_GET(state, i));
  }
  CHECK(v1[13] == state->lr);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-t2ldmdb3") {

  const char source[] = "ldmdb r0, {r0-r12,pc}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0x31eed260, 0xf21a4416,
                      0x319b8e1b, 0x215f4510, 0x927e556e, 0xa8a0e729,
                      0x719ca3c1, 0xd7b24369, 0xb25e4516, 0x235b2fc3,
                      0xaad33b87, 0x2a};
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
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDMDB", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 4 * 14;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  for (unsigned int i = 0; i < 13; i++) {
    INFO("Offset " << i);
    CHECK(v1[i] == QBDI_GPR_GET(state, i));
  }
  CHECK(v1[13] == state->pc);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-t2ldmdb4") {

  const char source[] = "cmp r1, #42; it ne; ldmdbne r0, {r1, r2, pc}\n";

  // with condition reach
  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0x2a};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDMDB", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 12;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

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

  ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(not e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == 42);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-t2ldmdb_post1") {

  const char source[] = "ldmdb r0!, {r1, r2}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDMDB_UPD", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 8;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v1[0]);
  CHECK(state->r2 == v1[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-t2ldmdb_post2") {

  const char source[] = "ldmdb r0!, {r1-r12,lr}\n";

  QBDI::rword v1[] = {0x747f4b7e, 0x31eed260, 0xf21a4416, 0x319b8e1b,
                      0x215f4510, 0x927e556e, 0xa8a0e729, 0x719ca3c1,
                      0xd7b24369, 0xb25e4516, 0x235b2fc3, 0xc2708a8b,
                      0x2a};
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
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDMDB_UPD", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 4 * 13;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  for (unsigned int i = 0; i < 12; i++) {
    INFO("Offset " << i);
    CHECK(v1[i] == QBDI_GPR_GET(state, i + 1));
  }
  CHECK(v1[12] == state->lr);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-t2ldmdb_post3") {

  const char source[] = "ldmdb r0!, {r1-r12,pc}\n";

  QBDI::rword v1[] = {0x747f4b7e, 0x31eed260, 0xf21a4416, 0x319b8e1b,
                      0x215f4510, 0x927e556e, 0xa8a0e729, 0x719ca3c1,
                      0xd7b24369, 0xb25e4516, 0x235b2fc3, 0xc2708a8b,
                      0x2a};
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
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDMDB_UPD", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 4 * 13;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  for (unsigned int i = 0; i < 12; i++) {
    INFO("Offset " << i);
    CHECK(v1[i] == QBDI_GPR_GET(state, i + 1));
  }
  CHECK(v1[12] == state->pc);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-t2ldmdb_post4") {

  const char source[] = "cmp r1, #42; it ne; ldmdbne r0!, {r1, r2, pc}\n";

  // with condition reach
  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0x2a};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDMDB_UPD", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 12;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

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

  ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(not e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == 42);
}

// tPop
// ====

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-tpop1") {

  const char source[] = "mov sp, r12; pop {r0, r1}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tPOP", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r12 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(state->sp == reinterpret_cast<QBDI::rword>(&v1) + 8);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == v1[0]);
  CHECK(state->r1 == v1[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-tpop2") {

  const char source[] = "mov sp, r12; pop {r0-r7}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0x31eed260, 0xf21a4416,
                      0x319b8e1b, 0x215f4510, 0x927e556e, 0xa8a0e729};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v1[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v1[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v1[2], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[3], v1[3], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[4], v1[4], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[5], v1[5], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[6], v1[6], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[7], v1[7], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tPOP", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r12 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->sp == reinterpret_cast<QBDI::rword>(&v1) + 32);
  for (unsigned int i = 0; i < 8; i++) {
    INFO("Offset " << i);
    CHECK(v1[i] == QBDI_GPR_GET(state, i));
  }
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-tpop3") {

  const char source[] = "mov sp, r12; pop {r0-r7,pc}\n";

  QBDI::rword v1[] = {0x5de254a1, 0x747f4b7e, 0x31eed260,
                      0xf21a4416, 0x319b8e1b, 0x215f4510,
                      0x927e556e, 0xa8a0e729, 0x2a};
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
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tPOP", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r12 = reinterpret_cast<QBDI::rword>(&v1);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->sp == reinterpret_cast<QBDI::rword>(&v1) + 36);
  for (unsigned int i = 0; i < 8; i++) {
    INFO("Offset " << i);
    CHECK(v1[i] == QBDI_GPR_GET(state, i));
  }
  CHECK(v1[8] == state->pc);
}

// tSTMIA_UPD
// ==========

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-tstmia1") {

  const char source[] = "stmia r0!, {r1, r2}\n";

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tSTMIA", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = v[0];
  state->r2 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-tstmia2") {

  const char source[] = "stmia r0!, {r0, r1, r2}\n";

  QBDI::rword v[] = {0, 0x5de254a1, 0x747f4b7e};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  v[0] = (QBDI::rword)&v1;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v[2], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tSTMIA", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = v[1];
  state->r2 = v[2];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
}

// t2STMIA
// =======

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-t2stmia1") {

  const char source[] = "stmia.w r0, {r1, r2}\n";

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STMIA", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = v[0];
  state->r2 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-t2stmia2") {

  const char source[] = "stmia.w r0, {r0-r12, lr}\n";

  QBDI::rword v[] = {0,          0x31eed260, 0xf21a4416, 0x319b8e1b, 0x215f4510,
                     0x927e556e, 0xa8a0e729, 0x719ca3c1, 0xd7b24369, 0xb25e4516,
                     0x235b2fc3, 0xc2708a8b, 0x75ef1380, 0x2a};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  v[0] = (QBDI::rword)&v1;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v[2], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[3], v[3], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[4], v[4], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[5], v[5], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[6], v[6], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[7], v[7], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[8], v[8], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[9], v[9], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[10], v[10], 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[11], v[11], 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[12], v[12], 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[13], v[13], 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STMIA", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = v[1];
  state->r2 = v[2];
  state->r3 = v[3];
  state->r4 = v[4];
  state->r5 = v[5];
  state->r6 = v[6];
  state->r7 = v[7];
  state->r8 = v[8];
  state->r9 = v[9];
  state->r10 = v[10];
  state->r11 = v[11];
  state->r12 = v[12];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-t2stmia3") {

  const char source[] = "cmp r4, #42; it ne; stmiane.w r0, {r1, r2, r3}\n";

  // with condition reach
  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e, 0xd7b24369};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v[2], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STMIA", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = v[0];
  state->r2 = v[1];
  state->r3 = v[2];
  state->r4 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);

  // with condition not reach
  expectedPost = {{}};
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = v[0] + 1;
  state->r2 = v[1] + 1;
  state->r3 = v[2] + 1;
  state->r4 = 42;
  vm.setGPRState(state);

  ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(memcmp(v, v1, sizeof(v)) == 0);
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-t2stmia_post1") {

  const char source[] = "stmia.w r0!, {r1, r2}\n";

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STMIA_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = v[0];
  state->r2 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1) + 8);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-t2stmia_post2") {

  const char source[] = "stmia.w r0!, {r1-r12, lr}\n";

  QBDI::rword v[] = {0x31eed260, 0xf21a4416, 0x319b8e1b, 0x215f4510, 0x927e556e,
                     0xa8a0e729, 0x719ca3c1, 0x75ef1380, 0xd7b24369, 0xb25e4516,
                     0x235b2fc3, 0xc2708a8b, 0x2a};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v[2], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[3], v[3], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[4], v[4], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[5], v[5], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[6], v[6], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[7], v[7], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[8], v[8], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[9], v[9], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[10], v[10], 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[11], v[11], 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[12], v[12], 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STMIA_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = v[0];
  state->r2 = v[1];
  state->r3 = v[2];
  state->r4 = v[3];
  state->r5 = v[4];
  state->r6 = v[5];
  state->r7 = v[6];
  state->r8 = v[7];
  state->r9 = v[8];
  state->r10 = v[9];
  state->r11 = v[10];
  state->r12 = v[11];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1) + 52);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-t2stmia_post3") {

  const char source[] = "cmp r4, #42; it ne; stmiane.w r0!, {r1, r2, r3}\n";

  // with condition reach
  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e, 0xd7b24369};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v[2], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STMIA_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = v[0];
  state->r2 = v[1];
  state->r3 = v[2];
  state->r4 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1) + 12);

  // with condition not reach
  expectedPost = {{}};
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = v[0] + 1;
  state->r2 = v[1] + 1;
  state->r3 = v[2] + 1;
  state->r4 = 42;
  vm.setGPRState(state);

  ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(memcmp(v, v1, sizeof(v)) == 0);
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1));
}

// t2STMDB
// =======

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-t2stmdb1") {

  const char source[] = "stmdb.w r0, {r1, r2}\n";

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STMDB", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 8;
  state->r1 = v[0];
  state->r2 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1) + 8);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-t2stmdb2") {

  const char source[] = "stmdb.w r0, {r0-r12, lr}\n";

  QBDI::rword v[] = {0,          0x31eed260, 0xf21a4416, 0x319b8e1b, 0x215f4510,
                     0x927e556e, 0xa8a0e729, 0x719ca3c1, 0xd7b24369, 0xb25e4516,
                     0x235b2fc3, 0xc2708a8b, 0x75ef1380, 0x2a};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  v[0] = (QBDI::rword)&v1 + 56;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v[2], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[3], v[3], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[4], v[4], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[5], v[5], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[6], v[6], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[7], v[7], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[8], v[8], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[9], v[9], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[10], v[10], 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[11], v[11], 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[12], v[12], 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[13], v[13], 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STMDB", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 56;
  state->r1 = v[1];
  state->r2 = v[2];
  state->r3 = v[3];
  state->r4 = v[4];
  state->r5 = v[5];
  state->r6 = v[6];
  state->r7 = v[7];
  state->r8 = v[8];
  state->r9 = v[9];
  state->r10 = v[10];
  state->r11 = v[11];
  state->r12 = v[12];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1) + 56);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-t2stmdb3") {

  const char source[] = "cmp r4, #42; it ne; stmdbne.w r0, {r1, r2, r3}\n";

  // with condition reach
  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e, 0xd7b24369};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v[2], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STMDB", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 12;
  state->r1 = v[0];
  state->r2 = v[1];
  state->r3 = v[2];
  state->r4 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);

  // with condition not reach
  expectedPost = {{}};
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 12;
  state->r1 = v[0] + 1;
  state->r2 = v[1] + 1;
  state->r3 = v[2] + 1;
  state->r4 = 42;
  vm.setGPRState(state);

  ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(memcmp(v, v1, sizeof(v)) == 0);
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1) + 12);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-t2stmdb_post1") {

  const char source[] = "stmdb.w r0!, {r1, r2}\n";

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STMDB_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 8;
  state->r1 = v[0];
  state->r2 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-t2stmdb_post2") {

  const char source[] = "stmdb.w r0!, {r1-r12, lr}\n";

  QBDI::rword v[] = {0x31eed260, 0xf21a4416, 0x319b8e1b, 0x215f4510, 0x927e556e,
                     0xa8a0e729, 0x719ca3c1, 0x75ef1380, 0xd7b24369, 0xb25e4516,
                     0x235b2fc3, 0xc2708a8b, 0x2a};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v[2], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[3], v[3], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[4], v[4], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[5], v[5], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[6], v[6], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[7], v[7], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[8], v[8], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[9], v[9], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[10], v[10], 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[11], v[11], 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[12], v[12], 4, QBDI::MEMORY_WRITE,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STMDB_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 52;
  state->r1 = v[0];
  state->r2 = v[1];
  state->r3 = v[2];
  state->r4 = v[3];
  state->r5 = v[4];
  state->r6 = v[5];
  state->r7 = v[6];
  state->r8 = v[7];
  state->r9 = v[8];
  state->r10 = v[9];
  state->r11 = v[10];
  state->r12 = v[11];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-t2stmdb_post3") {

  const char source[] = "cmp r4, #42; it ne; stmdbne.w r0!, {r1, r2, r3}\n";

  // with condition reach
  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e, 0xd7b24369};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v[2], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STMDB_UPD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 12;
  state->r1 = v[0];
  state->r2 = v[1];
  state->r3 = v[2];
  state->r4 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1));

  // with condition not reach
  expectedPost = {{}};
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 12;
  state->r1 = v[0] + 1;
  state->r2 = v[1] + 1;
  state->r3 = v[2] + 1;
  state->r4 = 42;
  vm.setGPRState(state);

  ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(memcmp(v, v1, sizeof(v)) == 0);
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1) + 12);
}

// tPUSH
// =====

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-tpush1") {

  const char source[] = "mov sp, r0; push {r1, r2}\n";

  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tPUSH", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 8;
  state->r1 = v[0];
  state->r2 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
  CHECK(state->sp == reinterpret_cast<QBDI::rword>(&v1));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-tpush2") {

  const char source[] = "mov sp, r8; push {r0-r7, lr}\n";

  QBDI::rword v[] = {0x31eed260, 0xf21a4416, 0x319b8e1b, 0x215f4510, 0xd7b24369,
                     0xb25e4516, 0x235b2fc3, 0xc2708a8b, 0x2a};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v[2], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[3], v[3], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[4], v[4], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[5], v[5], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[6], v[6], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[7], v[7], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[8], v[8], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tPUSH", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = v[0];
  state->r1 = v[1];
  state->r2 = v[2];
  state->r3 = v[3];
  state->r4 = v[4];
  state->r5 = v[5];
  state->r6 = v[6];
  state->r7 = v[7];
  state->r8 = reinterpret_cast<QBDI::rword>(&v1) + 36;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
  CHECK(state->sp == reinterpret_cast<QBDI::rword>(&v1));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-tpush3") {

  const char source[] = "cmp r4, #42; mov sp, r0; it ne; pushne {r1, r2, r3}\n";

  // with condition reach
  QBDI::rword v[] = {0x5de254a1, 0x747f4b7e, 0xd7b24369};
  QBDI::rword v1[sizeof(v) / sizeof(QBDI::rword)] = {0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[2], v[2], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tPUSH", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  QBDI::rword backupSP = state->sp;
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 12;
  state->r1 = v[0];
  state->r2 = v[1];
  state->r3 = v[2];
  state->r4 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  CHECK(memcmp(v, v1, sizeof(v)) == 0);
  CHECK(state->sp == reinterpret_cast<QBDI::rword>(&v1));

  // with condition not reach
  expectedPost = {{}};
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 12;
  state->r1 = v[0] + 1;
  state->r2 = v[1] + 1;
  state->r3 = v[2] + 1;
  state->r4 = 42;
  state->sp = backupSP;
  vm.setGPRState(state);

  ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(memcmp(v, v1, sizeof(v)) == 0);
  CHECK(state->sp == reinterpret_cast<QBDI::rword>(&v1) + 12);
}
