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

[[maybe_unused]] static bool checkFeature(const char *f) {
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

// void setFPR(QBDI::FPRState *fpr, size_t index, QBDI::rword hvalue,
//             QBDI::rword lvalue) {
//   reinterpret_cast<QBDI::rword *>(fpr)[index * 2] = lvalue;
//   reinterpret_cast<QBDI::rword *>(fpr)[index * 2 + 1] = hvalue;
// }
//
// void checkFullFPR(QBDI::FPRState *fpr, size_t index, QBDI::rword hvalue,
//                   QBDI::rword lvalue) {
//   INFO("v" << index);
//   CHECK(reinterpret_cast<QBDI::rword *>(fpr)[index * 2] == lvalue);
//   CHECK(reinterpret_cast<QBDI::rword *>(fpr)[index * 2 + 1] == hvalue);
// }
//
// void checkLowFPR(QBDI::FPRState *fpr, size_t index, QBDI::rword lvalue) {
//   INFO("v" << index);
//   CHECK(reinterpret_cast<QBDI::rword *>(fpr)[index * 2] == lvalue);
// }

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrt_post_imm") {

  const char source[] = "ldrt r1, [r0], #4\n";

  QBDI::rword v = 0x747f4b7e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRT_POST_IMM", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v);
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v) + 4);
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-strt_post_imm") {

  const char source[] = "strt r1, [r0], #4\n";

  QBDI::rword v = 0x747f4b7e;
  QBDI::rword dest = 0;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&dest, v, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STRT_POST_IMM", QBDI::POSTINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&dest);
  state->r1 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&dest) + 4);
  CHECK(dest == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrb_post_imm") {

  const char source[] = "ldrb r1, [r0], #1\n";

  QBDI::rword v = 0x7e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRB_POST_IMM", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v);
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v) + 1);
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-strb_post_imm") {

  const char source[] = "strb r1, [r0], #1\n";

  QBDI::rword v = 0x7e;
  QBDI::rword dest = 0;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&dest, v, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STRB_POST_IMM", QBDI::POSTINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&dest);
  state->r1 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&dest) + 1);
  CHECK(dest == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrb_pre_imm1") {

  const char source[] = "ldrb r1, [r0, #5]!\n";

  QBDI::rword v = 0x7e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRB_PRE_IMM", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 5;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v));
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrb_pre_imm2") {

  const char source[] = "ldrb r1, [r0, #-25]!\n";

  QBDI::rword v = 0x7e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRB_PRE_IMM", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 25;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v));
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-strb_pre_imm1") {

  const char source[] = "strb r1, [r0, #5]!\n";

  QBDI::rword v = 0x7e;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STRB_PRE_IMM", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 5;
  state->r1 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1));
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-strb_pre_imm2") {

  const char source[] = "strb r1, [r0, #-25]!\n";

  QBDI::rword v = 0x7e;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STRB_PRE_IMM", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 25;
  state->r1 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1));
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrb_pre_reg1") {

  const char source[] = "ldrb r1, [r0, r2]!\n";

  QBDI::rword v = 0x7e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRB_PRE_REG", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 25;
  state->r1 = 0;
  state->r2 = -25;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v));
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrb_pre_reg2") {

  const char source[] = "ldrb r1, [r0, -r2]!\n";

  QBDI::rword v = 0x7e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRB_PRE_REG", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 25;
  state->r1 = 0;
  state->r2 = 25;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v));
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrb_pre_reg3") {

  const char source[] = "ldrb r1, [r0, r2, lsl #4]!\n";

  QBDI::rword v = 0x7e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRB_PRE_REG", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 16;
  state->r1 = 0;
  state->r2 = -1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v));
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrb_pre_reg4") {

  const char source[] = "ldrb r1, [r0, -r2, lsl #4]!\n";

  QBDI::rword v = 0x7e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRB_PRE_REG", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 16;
  state->r1 = 0;
  state->r2 = 1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v));
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-strb_pre_reg1") {

  const char source[] = "strb r1, [r0, r2]!\n";

  QBDI::rword v = 0x7e;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STRB_PRE_REG", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 25;
  state->r1 = v;
  state->r2 = -25;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1));
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-strb_pre_reg2") {

  const char source[] = "strb r1, [r0, -r2]!\n";

  QBDI::rword v = 0x7e;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STRB_PRE_REG", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 25;
  state->r1 = v;
  state->r2 = 25;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1));
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-strb_pre_reg3") {

  const char source[] = "strb r1, [r0, r2, lsl #4]!\n";

  QBDI::rword v = 0x7e;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STRB_PRE_REG", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 16;
  state->r1 = v;
  state->r2 = -1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1));
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-strb_pre_reg4") {

  const char source[] = "strb r1, [r0, -r2, lsl #4]!\n";

  QBDI::rword v = 0x7e;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STRB_PRE_REG", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 16;
  state->r1 = v;
  state->r2 = 1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1));
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrb_imm1") {

  const char source[] = "ldrb r1, [r0, #5]\n";

  QBDI::rword v = 0x7e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRBi12", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 5;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrb_imm2") {

  const char source[] = "ldrb r1, [r0, #-25]\n";

  QBDI::rword v = 0x7e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRBi12", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 25;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrb_imm3") {

  const char source[] = "ldrb r1, [pc, #-6]\n";
  QBDI::rword codeAddr = genASM(source);

  QBDI::rword v = 0x5f;
  ExpectedMemoryAccesses expectedPre = {{
      {codeAddr + 2, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRBi12", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-strb_imm1") {

  const char source[] = "strb r1, [r0, #5]\n";

  QBDI::rword v = 0x7e;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STRBi12", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 5;
  state->r1 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrb_reg1") {

  const char source[] = "ldrb r1, [r0, r2]\n";

  QBDI::rword v = 0x7e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRBrs", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 25;
  state->r1 = 0;
  state->r2 = -25;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrb_reg2") {

  const char source[] = "ldrb r1, [r0, -r2]\n";

  QBDI::rword v = 0x7e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRBrs", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 25;
  state->r1 = 0;
  state->r2 = 25;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrb_reg3") {

  const char source[] = "ldrb r1, [r0, r2, lsl #4]\n";

  QBDI::rword v = 0x7e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRBrs", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 16;
  state->r1 = 0;
  state->r2 = -1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrb_reg4") {

  const char source[] = "ldrb r1, [r0, -r2, lsl #4]\n";

  QBDI::rword v = 0x7e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRBrs", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 16;
  state->r1 = 0;
  state->r2 = 1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrb_reg5") {

  const char source[] = "ldrb r1, [pc, -r0]\n";
  QBDI::rword codeAddr = genASM(source);

  QBDI::rword v = 0x7e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRBrs", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = codeAddr + 8 - reinterpret_cast<QBDI::rword>(&v);
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-strb_reg1") {

  const char source[] = "strb r1, [r0, -r2, lsl #4]\n";

  QBDI::rword v = 0x7e;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STRBrs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 16;
  state->r1 = v;
  state->r2 = 1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-strb_reg2") {

  const char source[] = "strb r1, [pc, -r0]\n";
  QBDI::rword codeAddr = genASM(source);

  QBDI::rword v = 0x7e;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STRBrs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = codeAddr + 8 - reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrd_imm1") {

  const char source[] = "ldrd r2, r3, [r0, #5]\n";

  QBDI::rword v[] = {0x719ca3c1, 0xd7b24369};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], v[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], v[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRD", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 5;
  state->r2 = 0;
  state->r3 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v[0]);
  CHECK(state->r3 == v[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrd_imm2") {

  const char source[] = "ldrd r2, r3, [r0, #-25]\n";

  QBDI::rword v[] = {0x719ca3c1, 0xd7b24369};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], v[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], v[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRD", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 25;
  state->r2 = 0;
  state->r3 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v[0]);
  CHECK(state->r3 == v[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrd_imm3") {

  const char source[] = "ldrd r2, r3, [pc, #-8];\n bx lr;\n";
  QBDI::rword codeAddr = genASM(source);

  ExpectedMemoryAccesses expectedPre = {{
      {codeAddr, ((QBDI::rword *)codeAddr)[0], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {codeAddr + 4, ((QBDI::rword *)codeAddr)[1], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRD", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-strd_imm1") {

  const char source[] = "strd r2, r3, [r0, #-25]\n";

  QBDI::rword v[] = {0x719ca3c1, 0xd7b24369};
  QBDI::rword v1[] = {0, 0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STRD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 25;
  state->r2 = v[0];
  state->r3 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1[0] == v[0]);
  CHECK(v1[1] == v[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrd_reg1") {

  const char source[] = "ldrd r2, r3, [r0, r1]\n";

  QBDI::rword v[] = {0x719ca3c1, 0xd7b24369};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], v[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], v[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRD", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 5;
  state->r1 = 5;
  state->r2 = 0;
  state->r3 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v[0]);
  CHECK(state->r3 == v[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrd_reg2") {

  const char source[] = "ldrd r2, r3, [r0, -r1]\n";

  QBDI::rword v[] = {0x719ca3c1, 0xd7b24369};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], v[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], v[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRD", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 25;
  state->r1 = 25;
  state->r2 = 0;
  state->r3 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v[0]);
  CHECK(state->r3 == v[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrd_reg3") {

  const char source[] = "ldrd r2, r3, [pc, r0];\n bx lr;\n";
  QBDI::rword codeAddr = genASM(source);

  ExpectedMemoryAccesses expectedPre = {{
      {codeAddr, ((QBDI::rword *)codeAddr)[0], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {codeAddr + 4, ((QBDI::rword *)codeAddr)[1], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRD", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = -8;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrd_reg4") {

  const char source[] = "ldrd r2, r3, [pc, -r0];\n bx lr;\n";
  QBDI::rword codeAddr = genASM(source);

  ExpectedMemoryAccesses expectedPre = {{
      {codeAddr, ((QBDI::rword *)codeAddr)[0], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {codeAddr + 4, ((QBDI::rword *)codeAddr)[1], 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRD", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = 8;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-strd_reg1") {

  const char source[] = "strd r2, r3, [pc, -r0]";
  QBDI::rword codeAddr = genASM(source);

  QBDI::rword v[] = {0x719ca3c1, 0xd7b24369};
  QBDI::rword v1[] = {0, 0};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STRD", QBDI::POSTINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = codeAddr + 8 - ((QBDI::rword)&v1[0]);
  state->r2 = v[0];
  state->r3 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrd_pre_reg1") {

  const char source[] = "ldrd r2, r3, [r0, r1]!\n";

  QBDI::rword v[] = {0x719ca3c1, 0xd7b24369};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], v[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], v[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRD_PRE", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 5;
  state->r1 = 5;
  state->r2 = 0;
  state->r3 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v[0]);
  CHECK(state->r3 == v[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-strd_pre_reg1") {

  const char source[] = "strd r2, r3, [r0, r1]!\n";

  QBDI::rword v[] = {0x719ca3c1, 0xd7b24369};
  QBDI::rword v1[] = {0, 0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STRD_PRE", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 5;
  state->r1 = 5;
  state->r2 = v[0];
  state->r3 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1[0] == v[0]);
  CHECK(v1[1] == v[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrd_post_reg1") {

  const char source[] = "ldrd r2, r3, [r0], r1\n";

  QBDI::rword v[] = {0x719ca3c1, 0xd7b24369};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], v[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], v[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRD_POST", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v);
  state->r1 = 5;
  state->r2 = 0;
  state->r3 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v[0]);
  CHECK(state->r3 == v[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-strd_post_reg1") {

  const char source[] = "strd r2, r3, [r0], r1\n";

  QBDI::rword v[] = {0x719ca3c1, 0xd7b24369};
  QBDI::rword v1[] = {0, 0};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STRD_POST", QBDI::POSTINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = 5;
  state->r2 = v[0];
  state->r3 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  CHECK(v1[0] == v[0]);
  CHECK(v1[1] == v[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrh_imm1") {

  const char source[] = "ldrh r2, [r0, #5]\n";

  QBDI::rword v = 0xa3c1;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRH", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 5;
  state->r2 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-strh_imm1") {

  const char source[] = "strh r2, [r0, #5]\n";

  QBDI::rword v = 0xa3c1;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STRH", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 5;
  state->r2 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrh_reg1") {

  const char source[] = "ldrh r2, [r0, r1]\n";

  QBDI::rword v = 0xa3c1;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRH", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 5;
  state->r1 = 5;
  state->r2 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-strh_reg1") {

  const char source[] = "strh r2, [r0, r1]\n";

  QBDI::rword v = 0xa3c1;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STRH", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 5;
  state->r1 = 5;
  state->r2 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(state->r2 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrh_pre_reg1") {

  const char source[] = "ldrh r2, [r0, r1]!\n";

  QBDI::rword v = 0xa3c1;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRH_PRE", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 5;
  state->r1 = 5;
  state->r2 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-strh_pre_reg1") {

  const char source[] = "strh r2, [r0, r1]!\n";

  QBDI::rword v = 0xa3c1;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STRH_PRE", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 5;
  state->r1 = 5;
  state->r2 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrh_post_reg1") {

  const char source[] = "ldrh r2, [r0], r1\n";

  QBDI::rword v = 0xa3c1;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRH_POST", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v);
  state->r1 = 5;
  state->r2 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-strh_post_reg1") {

  const char source[] = "strh r2, [r0], r1\n";

  QBDI::rword v = 0xa3c1;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STRH_POST", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = 5;
  state->r2 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrsh_reg1") {

  const char source[] = "ldrsh r2, [r0, r1]\n";

  QBDI::rword v = 0x23c1;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRSH", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 5;
  state->r1 = 5;
  state->r2 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrsh_pre_reg1") {

  const char source[] = "ldrsh r2, [r0, r1]!\n";

  QBDI::rword v = 0x23c1;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRSH_PRE", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 5;
  state->r1 = 5;
  state->r2 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrsh_post_reg1") {

  const char source[] = "ldrsh r2, [r0], r1\n";

  QBDI::rword v = 0x23c1;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRSH_POST", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v);
  state->r1 = 5;
  state->r2 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrsb_reg1") {

  const char source[] = "ldrsb r2, [r0, r1]\n";

  QBDI::rword v = 0x23;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRSB", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 5;
  state->r1 = 5;
  state->r2 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrsb_pre_reg1") {

  const char source[] = "ldrsb r2, [r0, r1]!\n";

  QBDI::rword v = 0x23;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRSB_PRE", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 5;
  state->r1 = 5;
  state->r2 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldrsb_post_reg1") {

  const char source[] = "ldrsb r2, [r0], r1\n";

  QBDI::rword v = 0x23;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRSB_POST", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v);
  state->r1 = 5;
  state->r2 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldr_imm1") {

  const char source[] = "ldr r2, [r0, #5]\n";

  QBDI::rword v = 0xb25e4516;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRi12", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 5;
  state->r2 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldr_imm2") {

  const char source[] = "ldr r2, [r0, #-25]\n";

  QBDI::rword v = 0xb25e4516;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRi12", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 25;
  state->r2 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-str_imm1") {

  const char source[] = "str r2, [r0, #5]\n";

  QBDI::rword v = 0xb25e4516;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STRi12", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 5;
  state->r2 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldr_reg1") {

  const char source[] = "ldr r2, [r0, r1]\n";

  QBDI::rword v = 0xb25e4516;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRrs", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 5;
  state->r1 = 5;
  state->r2 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldr_reg2") {

  const char source[] = "ldr r2, [r0, -r1, lsr #4]\n";

  QBDI::rword v = 0xb25e4516;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRrs", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 25;
  state->r1 = (25 << 4);
  state->r2 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-str_reg1") {

  const char source[] = "str r2, [r0, -r1, lsr #4]\n";

  QBDI::rword v = 0xb25e4516;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STRrs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 25;
  state->r1 = (25 << 4);
  state->r2 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldr_pre_imm1") {

  const char source[] = "ldr r2, [r0, #5]!\n";

  QBDI::rword v = 0xb25e4516;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDR_PRE_IMM", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 5;
  state->r2 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-str_pre_imm1") {

  const char source[] = "str r2, [r0, #5]!\n";

  QBDI::rword v = 0xb25e4516;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STR_PRE_IMM", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 5;
  state->r2 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldr_pre_reg1") {

  const char source[] = "ldr r2, [r0, r1]!\n";

  QBDI::rword v = 0xb25e4516;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDR_PRE_REG", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 5;
  state->r1 = 5;
  state->r2 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldr_pre_reg2") {

  const char source[] = "ldr r2, [r0, r1, lsr #4]!\n";

  QBDI::rword v = 0xb25e4516;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDR_PRE_REG", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 5;
  state->r1 = (5 << 4);
  state->r2 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-str_pre_reg1") {

  const char source[] = "str r2, [r0, r1, lsr #4]!\n";

  QBDI::rword v = 0xb25e4516;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STR_PRE_REG", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 5;
  state->r1 = (5 << 4);
  state->r2 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldr_post_imm1") {

  const char source[] = "ldr r2, [r0], 25\n";

  QBDI::rword v = 0xb25e4516;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDR_POST_IMM", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v);
  state->r2 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-str_post_imm1") {

  const char source[] = "str r2, [r0], 25\n";

  QBDI::rword v = 0xb25e4516;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STR_POST_IMM", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r2 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-ldr_post_reg1") {

  const char source[] = "ldr r2, [r0], r1\n";

  QBDI::rword v = 0xb25e4516;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDR_POST_REG", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v);
  state->r1 = 5;
  state->r2 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_ARM-str_post_reg1") {

  const char source[] = "str r2, [r0], r1\n";

  QBDI::rword v = 0xb25e4516;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STR_POST_REG", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = 5;
  state->r2 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}
