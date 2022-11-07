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

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrt") {

  const char source[] = "ldrt r1, [r0, #4]\n";

  QBDI::rword v = 0x747f4b7e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRT", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 4;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v) - 4);
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strt") {

  const char source[] = "strt r1, [r0, #4]\n";

  QBDI::rword v = 0x747f4b7e;
  QBDI::rword dest = 0;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&dest, v, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRT", QBDI::POSTINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&dest) - 4;
  state->r1 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&dest) - 4);
  CHECK(dest == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrbt") {

  const char source[] = "ldrbt r1, [r0, #4]\n";

  QBDI::rword v = 0x74;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRBT", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 4;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v) - 4);
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strbt") {

  const char source[] = "strbt r1, [r0, #4]\n";

  QBDI::rword v = 0x7e;
  QBDI::rword dest = 0;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&dest, v, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRBT", QBDI::POSTINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&dest) - 4;
  state->r1 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&dest) - 4);
  CHECK(dest == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrb_post") {

  const char source[] = "ldrb r1, [r0], #1\n";

  QBDI::rword v = 0x7e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRB_POST", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v);
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v) + 1);
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strb_post") {

  const char source[] = "strb r1, [r0], #1\n";

  QBDI::rword v = 0x7e;
  QBDI::rword dest = 0;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&dest, v, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRB_POST", QBDI::POSTINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&dest);
  state->r1 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&dest) + 1);
  CHECK(dest == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrb_pre1") {

  const char source[] = "ldrb r1, [r0, #5]!\n";

  QBDI::rword v = 0x7e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRB_PRE", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 5;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v));
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrb_pre2") {

  const char source[] = "ldrb r1, [r0, #-25]!\n";

  QBDI::rword v = 0x7e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRB_PRE", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 25;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v));
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strb_pre1") {

  const char source[] = "strb r1, [r0, #5]!\n";

  QBDI::rword v = 0x7e;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRB_PRE", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 5;
  state->r1 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1));
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strb_pre2") {

  const char source[] = "strb r1, [r0, #-25]!\n";

  QBDI::rword v = 0x7e;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRB_PRE", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 25;
  state->r1 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1));
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrb_imm1") {

  const char source[] = "ldrb r1, [r0, #5]\n";

  QBDI::rword v = 0x7e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tLDRBi", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 5;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrb_imm2") {

  const char source[] = "ldrb.w r1, [r0, #5]\n";

  QBDI::rword v = 0x7e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRBi12", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 5;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrb_imm3") {

  const char source[] = "ldrb r1, [r0, #-25]\n";

  QBDI::rword v = 0x7e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRBi8", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 25;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrb_imm4") {

  const char source[] = "ldrb r1, [pc, #-1]\n";
  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb);
  REQUIRE(codeAddr % 4 == 1);

  QBDI::rword v = 0x10;
  ExpectedMemoryAccesses expectedPre = {{
      {(codeAddr & (~1)) + 3, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRBpci", QBDI::PREINST, checkAccess, &expectedPre);

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

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrb_imm5") {

  const char source[] = "nop\n ldrb r1, [pc, #-1]\n";
  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb);
  REQUIRE(codeAddr % 4 == 1);

  QBDI::rword v = 0xf8;
  ExpectedMemoryAccesses expectedPre = {{
      {(codeAddr & (~1)) + 3, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRBpci", QBDI::PREINST, checkAccess, &expectedPre);

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

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strb_imm1") {

  const char source[] = "strb r1, [r0, #524]\n";

  QBDI::rword v = 0x7e;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRBi12", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 524;
  state->r1 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strb_imm2") {

  const char source[] = "strb r1, [r0, #-57]\n";

  QBDI::rword v = 0x7e;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRBi8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 57;
  state->r1 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strb_imm3") {

  const char source[] = "strb r1, [r0, #5]\n";

  QBDI::rword v = 0x7e;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tSTRBi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 5;
  state->r1 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrb_reg1") {

  const char source[] = "ldrb r1, [r0, r2]\n";

  QBDI::rword v = 0x7e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tLDRBr", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 25;
  state->r1 = 0;
  state->r2 = -25;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v) + 25);
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrb_reg2") {

  const char source[] = "ldrb.w r1, [r0, r2]\n";

  QBDI::rword v = 0x7e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRBs", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 25;
  state->r1 = 0;
  state->r2 = -25;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v) + 25);
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrb_reg3") {

  const char source[] = "ldrb.w r1, [r0, r2, lsl #2]\n";

  QBDI::rword v = 0x7e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRBs", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 24;
  state->r1 = 0;
  state->r2 = -6;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v) + 24);
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strb_reg1") {

  const char source[] = "strb r1, [r0, r2]\n";

  QBDI::rword v = 0x7e;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tSTRBr", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 25;
  state->r1 = v;
  state->r2 = -25;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1) + 25);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strb_reg2") {

  const char source[] = "strb.w r1, [r0, r2]\n";

  QBDI::rword v = 0x7e;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRBs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 25;
  state->r1 = v;
  state->r2 = -25;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1) + 25);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strb_reg3") {

  const char source[] = "strb.w r1, [r0, r2, lsl #2]\n";

  QBDI::rword v = 0x7e;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRBs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 24;
  state->r1 = v;
  state->r2 = -6;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1) + 24);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrd_imm1") {

  const char source[] = "ldrd r2, r3, [r0, #4]\n";

  QBDI::rword v[] = {0x719ca3c1, 0xd7b24369};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], v[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], v[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRDi8", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 4;
  state->r2 = 0;
  state->r3 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v[0]);
  CHECK(state->r3 == v[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrd_imm2") {

  const char source[] = "ldrd r2, r3, [r0, #-24]\n";

  QBDI::rword v[] = {0x719ca3c1, 0xd7b24369};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], v[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], v[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRDi8", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 24;
  state->r2 = 0;
  state->r3 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v[0]);
  CHECK(state->r3 == v[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrd_imm3") {

  const char source[] = "ldrd r2, r3, [pc, #-4];\n bx lr;\n";
  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb);

  ExpectedMemoryAccesses expectedPre = {{
      {(codeAddr & (~1)), ((QBDI::rword *)(codeAddr & (~1)))[0], 4,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(codeAddr & (~1)) + 4, ((QBDI::rword *)(codeAddr & (~1)))[1], 4,
       QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRDi8", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  CHECK(state->r2 == expectedPre.accesses[0].value);
  CHECK(state->r3 == expectedPre.accesses[1].value);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strd_imm1") {

  const char source[] = "strd r2, r3, [r0, #56]\n";

  QBDI::rword v[] = {0x719ca3c1, 0xd7b24369};
  QBDI::rword v1[] = {0, 0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRDi8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 56;
  state->r2 = v[0];
  state->r3 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1[0] == v[0]);
  CHECK(v1[1] == v[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strd_imm2") {

  const char source[] = "strd r2, r3, [r0, #-24]\n";

  QBDI::rword v[] = {0x719ca3c1, 0xd7b24369};
  QBDI::rword v1[] = {0, 0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRDi8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 24;
  state->r2 = v[0];
  state->r3 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1[0] == v[0]);
  CHECK(v1[1] == v[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrd_pre1") {

  const char source[] = "ldrd r2, r3, [r0, #4]!\n";

  QBDI::rword v[] = {0x719ca3c1, 0xd7b24369};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], v[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], v[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRD_PRE", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 4;
  state->r2 = 0;
  state->r3 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v));
  CHECK(state->r2 == v[0]);
  CHECK(state->r3 == v[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrd_pre2") {

  const char source[] = "ldrd r2, r3, [r0, #-24]!\n";

  QBDI::rword v[] = {0x719ca3c1, 0xd7b24369};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], v[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], v[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRD_PRE", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 24;
  state->r2 = 0;
  state->r3 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v));
  CHECK(state->r2 == v[0]);
  CHECK(state->r3 == v[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strd_pre1") {

  const char source[] = "strd r2, r3, [r0, #56]!\n";

  QBDI::rword v[] = {0x719ca3c1, 0xd7b24369};
  QBDI::rword v1[] = {0, 0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRD_PRE", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 56;
  state->r2 = v[0];
  state->r3 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1));
  CHECK(v1[0] == v[0]);
  CHECK(v1[1] == v[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strd_pre2") {

  const char source[] = "strd r2, r3, [r0, #-24]!\n";

  QBDI::rword v[] = {0x719ca3c1, 0xd7b24369};
  QBDI::rword v1[] = {0, 0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRD_PRE", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 24;
  state->r2 = v[0];
  state->r3 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1));
  CHECK(v1[0] == v[0]);
  CHECK(v1[1] == v[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrd_post1") {

  const char source[] = "ldrd r2, r3, [r0], #4\n";

  QBDI::rword v[] = {0x719ca3c1, 0xd7b24369};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], v[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], v[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRD_POST", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v);
  state->r2 = 0;
  state->r3 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v) + 4);
  CHECK(state->r2 == v[0]);
  CHECK(state->r3 == v[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrd_post2") {

  const char source[] = "ldrd r2, r3, [r0], #-24\n";

  QBDI::rword v[] = {0x719ca3c1, 0xd7b24369};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], v[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], v[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRD_POST", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v);
  state->r2 = 0;
  state->r3 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v) - 24);
  CHECK(state->r2 == v[0]);
  CHECK(state->r3 == v[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strd_post1") {

  const char source[] = "strd r2, r3, [r0], #56\n";

  QBDI::rword v[] = {0x719ca3c1, 0xd7b24369};
  QBDI::rword v1[] = {0, 0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRD_POST", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r2 = v[0];
  state->r3 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1) + 56);
  CHECK(v1[0] == v[0]);
  CHECK(v1[1] == v[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strd_post2") {

  const char source[] = "strd r2, r3, [r0], #-24\n";

  QBDI::rword v[] = {0x719ca3c1, 0xd7b24369};
  QBDI::rword v1[] = {0, 0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRD_POST", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r2 = v[0];
  state->r3 = v[1];
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1) - 24);
  CHECK(v1[0] == v[0]);
  CHECK(v1[1] == v[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrh_post") {

  const char source[] = "ldrh r1, [r0], #45\n";

  QBDI::rword v = 0x7e95;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRH_POST", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v);
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v) + 45);
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strh_post") {

  const char source[] = "strh r1, [r0], #25\n";

  QBDI::rword v = 0x7e75;
  QBDI::rword dest = 0;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&dest, v, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRH_POST", QBDI::POSTINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&dest);
  state->r1 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&dest) + 25);
  CHECK(dest == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrh_pre1") {

  const char source[] = "ldrh r1, [r0, #5]!\n";

  QBDI::rword v = 0x7e13;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRH_PRE", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 5;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v));
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrh_pre2") {

  const char source[] = "ldrh r1, [r0, #-25]!\n";

  QBDI::rword v = 0x7e11;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRH_PRE", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 25;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v));
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strh_pre1") {

  const char source[] = "strh r1, [r0, #5]!\n";

  QBDI::rword v = 0x7e74;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRH_PRE", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 5;
  state->r1 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1));
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strh_pre2") {

  const char source[] = "strh r1, [r0, #-25]!\n";

  QBDI::rword v = 0x7e75;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRH_PRE", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 25;
  state->r1 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1));
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrh_imm1") {

  const char source[] = "ldrh r1, [r0, #6]\n";

  QBDI::rword v = 0x7e54;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tLDRHi", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 6;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrh_imm2") {

  const char source[] = "ldrh.w r1, [r0, #5]\n";

  QBDI::rword v = 0x76fe;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRHi12", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 5;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrh_imm3") {

  const char source[] = "ldrh r1, [r0, #-25]\n";

  QBDI::rword v = 0xda7e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRHi8", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 25;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrh_imm4") {

  const char source[] = "ldrh r1, [pc, #-1]\n bx lr\n";
  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb);
  REQUIRE(codeAddr % 4 == 1);

  QBDI::rword v = 0x7010;
  ExpectedMemoryAccesses expectedPre = {{
      {(codeAddr & (~1)) + 3, v, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRHpci", QBDI::PREINST, checkAccess, &expectedPre);

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

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrh_imm5") {

  const char source[] = "nop\n ldrh r1, [pc, #-1]\n";
  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb);
  REQUIRE(codeAddr % 4 == 1);

  QBDI::rword v = 0x01f8;
  ExpectedMemoryAccesses expectedPre = {{
      {(codeAddr & (~1)) + 3, v, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRHpci", QBDI::PREINST, checkAccess, &expectedPre);

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

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strh_imm1") {

  const char source[] = "strh r1, [r0, #524]\n";

  QBDI::rword v = 0xb86e;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRHi12", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 524;
  state->r1 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strh_imm2") {

  const char source[] = "strh r1, [r0, #-57]\n";

  QBDI::rword v = 0x138f;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRHi8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 57;
  state->r1 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strh_imm3") {

  const char source[] = "strh r1, [r0, #6]\n";

  QBDI::rword v = 0x74d2;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tSTRHi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 6;
  state->r1 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrh_reg1") {

  const char source[] = "ldrh r1, [r0, r2]\n";

  QBDI::rword v = 0x7e43;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tLDRHr", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 25;
  state->r1 = 0;
  state->r2 = -25;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v) + 25);
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrh_reg2") {

  const char source[] = "ldrh.w r1, [r0, r2]\n";

  QBDI::rword v = 0x437e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRHs", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 25;
  state->r1 = 0;
  state->r2 = -25;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v) + 25);
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrh_reg3") {

  const char source[] = "ldrh.w r1, [r0, r2, lsl #2]\n";

  QBDI::rword v = 0x7e2e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRHs", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 24;
  state->r1 = 0;
  state->r2 = -6;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v) + 24);
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strh_reg1") {

  const char source[] = "strh r1, [r0, r2]\n";

  QBDI::rword v = 0x746e;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tSTRHr", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 25;
  state->r1 = v;
  state->r2 = -25;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1) + 25);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strh_reg2") {

  const char source[] = "strh.w r1, [r0, r2]\n";

  QBDI::rword v = 0xf254;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRHs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 25;
  state->r1 = v;
  state->r2 = -25;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1) + 25);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strh_reg3") {

  const char source[] = "strh.w r1, [r0, r2, lsl #2]\n";

  QBDI::rword v = 0xa68b;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRHs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 24;
  state->r1 = v;
  state->r2 = -6;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v1) + 24);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrsb_post") {

  const char source[] = "ldrsb r1, [r0], #1\n";

  QBDI::rword v = 0x8e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRSB_POST", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v);
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v) + 1);
  CHECK(((QBDI::sword)state->r1) == ((int8_t)v));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrsb_pre1") {

  const char source[] = "ldrsb r1, [r0, #5]!\n";

  QBDI::rword v = 0x8e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRSB_PRE", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 5;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v));
  CHECK(((QBDI::sword)state->r1) == ((int8_t)v));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrsb_pre2") {

  const char source[] = "ldrsb r1, [r0, #-25]!\n";

  QBDI::rword v = 0x8e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRSB_PRE", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 25;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v));
  CHECK(((QBDI::sword)state->r1) == ((int8_t)v));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrsb_imm1") {

  const char source[] = "ldrsb.w r1, [r0, #5]\n";

  QBDI::rword v = 0x8e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRSBi12", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 5;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(((QBDI::sword)state->r1) == ((int8_t)v));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrsb_imm2") {

  const char source[] = "ldrsb r1, [r0, #-25]\n";

  QBDI::rword v = 0x8e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRSBi8", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 25;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(((QBDI::sword)state->r1) == ((int8_t)v));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrsb_imm3") {

  const char source[] = "ldrsb r1, [pc, #-1]\n";
  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb);
  REQUIRE(codeAddr % 4 == 1);

  QBDI::rword v = 0x10;
  ExpectedMemoryAccesses expectedPre = {{
      {(codeAddr & (~1)) + 3, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRSBpci", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(((QBDI::sword)state->r1) == ((int8_t)v));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrsb_imm4") {

  const char source[] = "nop\n ldrsb r1, [pc, #-1]\n";
  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb);
  REQUIRE(codeAddr % 4 == 1);

  QBDI::rword v = 0xf9;
  ExpectedMemoryAccesses expectedPre = {{
      {(codeAddr & (~1)) + 3, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRSBpci", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(((QBDI::sword)state->r1) == ((int8_t)v));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrsb_reg1") {

  const char source[] = "ldrsb r1, [r0, r2]\n";

  QBDI::rword v = 0x8e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tLDRSBr", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 25;
  state->r1 = 0;
  state->r2 = -25;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v) + 25);
  CHECK(((QBDI::sword)state->r1) == ((int8_t)v));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrsb_reg2") {

  const char source[] = "ldrsb.w r1, [r0, r2]\n";

  QBDI::rword v = 0x8e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRSBs", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 25;
  state->r1 = 0;
  state->r2 = -25;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v) + 25);
  CHECK(((QBDI::sword)state->r1) == ((int8_t)v));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrsb_reg3") {

  const char source[] = "ldrsb.w r1, [r0, r2, lsl #2]\n";

  QBDI::rword v = 0x8e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRSBs", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 24;
  state->r1 = 0;
  state->r2 = -6;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v) + 24);
  CHECK(((QBDI::sword)state->r1) == ((int8_t)v));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrsh_post") {

  const char source[] = "ldrsh r1, [r0], #45\n";

  QBDI::rword v = 0x9e95;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRSH_POST", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v);
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v) + 45);
  CHECK(((QBDI::sword)state->r1) == ((int16_t)v));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrsh_pre1") {

  const char source[] = "ldrsh r1, [r0, #5]!\n";

  QBDI::rword v = 0x9e13;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRSH_PRE", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 5;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v));
  CHECK(((QBDI::sword)state->r1) == ((int16_t)v));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrsh_pre2") {

  const char source[] = "ldrsh r1, [r0, #-25]!\n";

  QBDI::rword v = 0x9e11;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRSH_PRE", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 25;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v));
  CHECK(((QBDI::sword)state->r1) == ((int16_t)v));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrsh_imm1") {

  const char source[] = "ldrsh.w r1, [r0, #5]\n";

  QBDI::rword v = 0xa6fe;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRSHi12", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 5;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(((QBDI::sword)state->r1) == ((int16_t)v));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrsh_imm2") {

  const char source[] = "ldrsh r1, [r0, #-25]\n";

  QBDI::rword v = 0xda7e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRSHi8", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 25;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(((QBDI::sword)state->r1) == ((int16_t)v));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrsh_imm3") {

  const char source[] = "ldrsh r1, [pc, #-1]\n bx lr\n";
  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb);
  REQUIRE(codeAddr % 4 == 1);

  QBDI::rword v = 0x7010;
  ExpectedMemoryAccesses expectedPre = {{
      {(codeAddr & (~1)) + 3, v, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRSHpci", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(((QBDI::sword)state->r1) == ((int16_t)v));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrsh_imm4") {

  const char source[] = "nop\n ldrsh r1, [pc, #-1]\n";
  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb);
  REQUIRE(codeAddr % 4 == 1);

  QBDI::rword v = 0x01f9;
  ExpectedMemoryAccesses expectedPre = {{
      {(codeAddr & (~1)) + 3, v, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRSHpci", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(((QBDI::sword)state->r1) == ((int16_t)v));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrsh_reg1") {

  const char source[] = "ldrsh r1, [r0, r2]\n";

  QBDI::rword v = 0xfe43;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tLDRSHr", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 25;
  state->r1 = 0;
  state->r2 = -25;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v) + 25);
  CHECK(((QBDI::sword)state->r1) == ((int16_t)v));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrsh_reg2") {

  const char source[] = "ldrsh.w r1, [r0, r2]\n";

  QBDI::rword v = 0x937e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRSHs", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 25;
  state->r1 = 0;
  state->r2 = -25;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v) + 25);
  CHECK(((QBDI::sword)state->r1) == ((int16_t)v));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrsh_reg3") {

  const char source[] = "ldrsh.w r1, [r0, r2, lsl #2]\n";

  QBDI::rword v = 0xce2e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRSHs", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 24;
  state->r1 = 0;
  state->r2 = -6;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r0 == reinterpret_cast<QBDI::rword>(&v) + 24);
  CHECK(((QBDI::sword)state->r1) == ((int16_t)v));
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldr_imm1") {

  const char source[] = "ldr r2, [r0, #4]\n";

  QBDI::rword v = 0xb25e4516;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tLDRi", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 4;
  state->r2 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldr_imm2") {

  const char source[] = "mov sp, r0\n ldr r2, [sp, #4]\n";

  QBDI::rword v = 0xb25e4516;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tLDRspi", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 4;
  state->r2 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldr_imm3") {

  const char source[] = "ldr.w r2, [r0, #5]\n";

  QBDI::rword v = 0xb25e4516;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRi12", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 5;
  state->r2 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldr_imm4") {

  const char source[] = "ldr r2, [r0, #-25]\n";

  QBDI::rword v = 0xb25e4516;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRi8", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) + 25;
  state->r2 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldr_imm5") {

  const char source[] =
      "ldr r1, [pc, #4]\n bx lr\n.long 0xfe125498\n.long 0x5eb6a7ce";
  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb);
  REQUIRE(codeAddr % 4 == 1);

  QBDI::rword v = 0x5eb6a7ce;
  ExpectedMemoryAccesses expectedPre = {{
      {(codeAddr & (~1)) + 8, v, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tLDRpci", QBDI::PREINST, checkAccess, &expectedPre);

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

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldr_imm6") {

  const char source[] =
      "nop\nldr r1, [pc, #4]\n bx lr\n.long 0xfe125498\n.long 0x5eb6a7ce";
  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb);
  REQUIRE(codeAddr % 4 == 1);

  QBDI::rword v = 0xa7cefe12;
  ExpectedMemoryAccesses expectedPre = {{
      {(codeAddr & (~1)) + 8, v, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tLDRpci", QBDI::PREINST, checkAccess, &expectedPre);

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

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldr_imm7") {

  const char source[] = "ldr r1, [pc, #-4]\n";
  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb);
  REQUIRE(codeAddr % 4 == 1);

  QBDI::rword v = 0x1004f85f;
  ExpectedMemoryAccesses expectedPre = {{
      {(codeAddr & (~1)), v, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRpci", QBDI::PREINST, checkAccess, &expectedPre);

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

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldr_imm8") {

  const char source[] = "nop\n ldr r1, [pc, #-4]\n";
  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb);
  REQUIRE(codeAddr % 4 == 1);

  QBDI::rword v = 0xf85fbf00;
  ExpectedMemoryAccesses expectedPre = {{
      {(codeAddr & (~1)), v, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRpci", QBDI::PREINST, checkAccess, &expectedPre);

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

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-str_imm1") {

  const char source[] = "str r2, [r0, #4]\n";

  QBDI::rword v = 0xb25e4516;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tSTRi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 4;
  state->r2 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-str_imm2") {

  const char source[] = "mov sp, r0\n str r2, [sp, #8]\n";

  QBDI::rword v = 0xb25e4516;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tSTRspi", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 8;
  state->r2 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-str_imm3") {

  const char source[] = "str.w r2, [r0, #35]\n";

  QBDI::rword v = 0xb25e4516;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRi12", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 35;
  state->r2 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-str_imm4") {

  const char source[] = "str r2, [r0, #-43]\n";

  QBDI::rword v = 0xb25e4516;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRi8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 43;
  state->r2 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldr_reg1") {

  const char source[] = "ldr r2, [r0, r1]\n";

  QBDI::rword v = 0xb25e4516;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tLDRr", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 5;
  state->r1 = 5;
  state->r2 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldr_reg2") {

  const char source[] = "ldr.w r2, [r0, r1]\n";

  QBDI::rword v = 0xb25e4516;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRs", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 5;
  state->r1 = 5;
  state->r2 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldr_reg3") {

  const char source[] = "ldr r2, [r0, r1, lsl #3]\n";

  QBDI::rword v = 0xb25e4516;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRs", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 24;
  state->r1 = 3;
  state->r2 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-str_reg1") {

  const char source[] = "str r2, [r0, r1]\n";

  QBDI::rword v = 0xb25e4516;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tSTRr", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 25;
  state->r1 = 25;
  state->r2 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-str_reg2") {

  const char source[] = "str.w r2, [r0, r1]\n";

  QBDI::rword v = 0xb25e4516;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) + 25;
  state->r1 = -25;
  state->r2 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-str_reg3") {

  const char source[] = "str r2, [r0, r1, lsl #3]\n";

  QBDI::rword v = 0xb25e4516;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STRs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 24;
  state->r1 = 3;
  state->r2 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldr_pre_imm1") {

  const char source[] = "ldr r2, [r0, #5]!\n";

  QBDI::rword v = 0xb25e4516;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDR_PRE", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 5;
  state->r2 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-str_pre_imm1") {

  const char source[] = "str r2, [r0, #5]!\n";

  QBDI::rword v = 0xb25e4516;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STR_PRE", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 5;
  state->r2 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldr_post_imm1") {

  const char source[] = "ldr r2, [r0], 25\n";

  QBDI::rword v = 0xb25e4516;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDR_POST", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v);
  state->r2 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r2 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-str_post_imm1") {

  const char source[] = "str r2, [r0], 25\n";

  QBDI::rword v = 0xb25e4516;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STR_POST", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r2 = v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrexb") {

  const char source[] = "ldrexb r1, [r0]\n";

  QBDI::rword v = 0x7e;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDREXB", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v);
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrexd") {

  const char source[] = "ldrexd r1, r2, [r0]\n";

  alignas(16) QBDI::rword v[] = {0x13eb7e46, 0x4a56ebc9};
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v[0], v[0], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v[1], v[1], 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDREXD", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v);
  state->r1 = 0;
  state->r2 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v[0]);
  CHECK(state->r2 == v[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrexh") {

  const char source[] = "ldrexh r1, [r0]\n";

  QBDI::rword v = 0x7e46;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDREXH", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v);
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrex1") {

  const char source[] = "ldrex r1, [r0]\n";

  QBDI::rword v = 0x7e4e5686;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDREX", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v);
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrex2") {

  const char source[] = "ldrex r1, [r0, #8]\n";

  QBDI::rword v = 0x7e4e5686;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDREX", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 8;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-ldrex3") {

  const char source[] = "ldrex r1, [r0, #1020]\n";

  QBDI::rword v = 0x7e4e5686;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, v, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDREX", QBDI::PREINST, checkAccess, &expectedPre);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v) - 1020;
  state->r1 = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);

  state = vm.getGPRState();
  CHECK(state->r1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strexb1") {

  const char source[] = "strexb r10, r1, [r0]\n";

  QBDI::rword v = 0x7e;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, 0, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STREXB", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = v;
  state->localMonitor.enable = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strexb2") {

  const char source[] = "strexb r10, r1, [r0]\n";

  QBDI::rword v = 0x7e;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STREXB", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = v;
  state->localMonitor.enable = 1;
  state->localMonitor.addr = (QBDI::rword)&v1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strexd1") {

  const char source[] = "strexd r10, r1, r2, [r0]\n";

  alignas(16) QBDI::rword v[] = {0x7112a12e, 0xb0e356d9};
  alignas(16) QBDI::rword v1[] = {0, 0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], 0, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], 0, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STREXD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = v[0];
  state->r2 = v[1];
  state->localMonitor.enable = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1[0] == 0);
  CHECK(v1[1] == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strexd2") {

  const char source[] = "strexd r10, r1, r2, [r0]\n";

  alignas(16) QBDI::rword v[] = {0x7112a12e, 0xb0e356d9};
  alignas(16) QBDI::rword v1[] = {0, 0};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1[0], v[0], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v1[1], v[1], 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STREXD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = v[0];
  state->r2 = v[1];
  state->localMonitor.enable = 8;
  state->localMonitor.addr = (QBDI::rword)&v1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1[0] == v[0]);
  CHECK(v1[1] == v[1]);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strexh1") {

  const char source[] = "strexh r10, r1, [r0]\n";

  QBDI::rword v = 0x7e2e;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, 0, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STREXH", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = v;
  state->localMonitor.enable = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strexh2") {

  const char source[] = "strexh r10, r1, [r0]\n";

  QBDI::rword v = 0x7e13;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STREXH", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = v;
  state->localMonitor.enable = 2;
  state->localMonitor.addr = (QBDI::rword)&v1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strex1") {

  const char source[] = "strex r10, r1, [r0]\n";

  QBDI::rword v = 0x7802d13e;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, 0, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STREX", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = v;
  state->localMonitor.enable = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strex2") {

  const char source[] = "strex r10, r1, [r0]\n";

  QBDI::rword v = 0x7e40e213;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STREX", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1);
  state->r1 = v;
  state->localMonitor.enable = 4;
  state->localMonitor.addr = (QBDI::rword)&v1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strex3") {

  const char source[] = "strex r10, r1, [r0, #8]\n";

  QBDI::rword v = 0x7802d13e;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, 0, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STREX", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 8;
  state->r1 = v;
  state->localMonitor.enable = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strex4") {

  const char source[] = "strex r10, r1, [r0, #8]\n";

  QBDI::rword v = 0x7e40e213;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STREX", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 8;
  state->r1 = v;
  state->localMonitor.enable = 4;
  state->localMonitor.addr = (QBDI::rword)&v1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strex5") {

  const char source[] = "strex r10, r1, [r0, #1020]\n";

  QBDI::rword v = 0x7802d13e;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, 0, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STREX", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 1020;
  state->r1 = v;
  state->localMonitor.enable = 0;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == 0);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-strex6") {

  const char source[] = "strex r10, r1, [r0, #1020]\n";

  QBDI::rword v = 0x7e40e213;
  QBDI::rword v1 = 0;
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v1, v, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2STREX", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = reinterpret_cast<QBDI::rword>(&v1) - 1020;
  state->r1 = v;
  state->localMonitor.enable = 4;
  state->localMonitor.addr = (QBDI::rword)&v1;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
  CHECK(v1 == v);
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-tbb1") {

  const char source[] =
      "tbb [pc, r0]\n.word 0x05040504\n.word 0x05040504\nbx lr\nbx lr";
  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb);

  ExpectedMemoryAccesses expectedPre = {{
      {0, 0, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2TBB", QBDI::PREINST, checkAccess, &expectedPre);

  for (int i = 0; i < 8; i++) {
    QBDI::GPRState *state = vm.getGPRState();
    state->r0 = i;
    vm.setGPRState(state);

    expectedPre.accesses[0].see = false;
    expectedPre.accesses[0].address = (codeAddr & (~1)) + 4 + i;
    expectedPre.accesses[0].value = ((i & 1) == 1) ? 0x5 : 0x4;

    QBDI::rword retval;
    bool ran = vm.call(&retval, codeAddr, {});

    CHECK(ran);
    for (auto &e : expectedPre.accesses)
      CHECK(e.see);
  }
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-tbb2") {

  const char source[] = "tbb [r0, r1]\nbx lr\nbx lr\nbx lr\nbx lr\nbx lr";
  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb);

  uint8_t v[] = {0, 3, 2, 3, 1, 2, 4, 0};
  ExpectedMemoryAccesses expectedPre = {{
      {0, 0, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2TBB", QBDI::PREINST, checkAccess, &expectedPre);

  for (unsigned i = 0; i < sizeof(v); i++) {
    QBDI::GPRState *state = vm.getGPRState();
    state->r0 = (QBDI::rword)&v;
    state->r1 = i;
    vm.setGPRState(state);

    expectedPre.accesses[0].see = false;
    expectedPre.accesses[0].address = (QBDI::rword)&v[i];
    expectedPre.accesses[0].value = v[i];

    QBDI::rword retval;
    bool ran = vm.call(&retval, codeAddr, {});

    CHECK(ran);
    for (auto &e : expectedPre.accesses)
      CHECK(e.see);
  }
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-tbh1") {

  const char source[] =
      "tbh [pc, r0, lsl #1]\n.word 0x090008\n.word 0x090008\n.word "
      "0x090008\n.word 0x090008\nbx lr\nbx lr";
  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb);

  ExpectedMemoryAccesses expectedPre = {{
      {0, 0, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2TBH", QBDI::PREINST, checkAccess, &expectedPre);

  for (int i = 0; i < 8; i++) {
    QBDI::GPRState *state = vm.getGPRState();
    state->r0 = i;
    vm.setGPRState(state);

    expectedPre.accesses[0].see = false;
    expectedPre.accesses[0].address = (codeAddr & (~1)) + 4 + i * 2;
    expectedPre.accesses[0].value = ((i & 1) == 1) ? 0x9 : 0x8;

    QBDI::rword retval;
    bool ran = vm.call(&retval, codeAddr, {});

    CHECK(ran);
    for (auto &e : expectedPre.accesses)
      CHECK(e.see);
  }
}

TEST_CASE_METHOD(APITest, "MemoryAccessTest_Thumb-tbh2") {

  const char source[] =
      "tbh [r0, r1, lsl #1]\nbx lr\nbx lr\nbx lr\nbx lr\nbx lr";
  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb);

  uint16_t v[] = {0, 3, 2, 3, 1, 2, 4, 0};
  ExpectedMemoryAccesses expectedPre = {{
      {0, 0, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2TBH", QBDI::PREINST, checkAccess, &expectedPre);

  for (unsigned i = 0; i < sizeof(v) / sizeof(v[0]); i++) {
    QBDI::GPRState *state = vm.getGPRState();
    state->r0 = (QBDI::rword)&v;
    state->r1 = i;
    vm.setGPRState(state);

    expectedPre.accesses[0].see = false;
    expectedPre.accesses[0].address = (QBDI::rword)&v[i];
    expectedPre.accesses[0].value = v[i];

    QBDI::rword retval;
    bool ran = vm.call(&retval, codeAddr, {});

    CHECK(ran);
    for (auto &e : expectedPre.accesses)
      CHECK(e.see);
  }
}
