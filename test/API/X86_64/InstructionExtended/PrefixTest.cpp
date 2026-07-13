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

#include <catch2/catch_test_macros.hpp>
#include "API/APITest.h"

#include <cstring>

#include "QBDI/Memory.hpp"
#include "QBDI/Options.h"
#include "QBDI/Platform.h"
#include "QBDI/Range.h"

#include "Utility/System.h"

static bool checkFeature(const char *f) {
  if (!QBDI::isHostCPUFeaturePresent(f)) {
    // WARN("Host doesn't support " << f << " feature: SKIP");
    return false;
  }
  return true;
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
  std::vector<QBDI::MemoryAccess> memaccesses = vm->getInstMemoryAccess();
  REQUIRE(memaccesses.size() == info->accesses.size());
  for (size_t i = 0; i < info->accesses.size(); i++) {
    auto &memaccess = memaccesses[i];
    auto &expect = info->accesses[i];
    CHECKED_IF(memaccess.accessAddress == expect.address)
    CHECKED_IF(memaccess.value == expect.value)
    CHECKED_IF(memaccess.size == expect.size)
    CHECKED_IF(memaccess.type == expect.type)
    expect.see = true;
  }
  return QBDI::VMAction::CONTINUE;
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-lock_add") {
  const char source[] = "lock addl $1, (%rax)\n";

  uint32_t v = 0x41;
  ExpectedMemoryAccesses expectedPre = {{
      {(QBDI::rword)&v, 0x41, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&v, 0x41, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {(QBDI::rword)&v, 0x42, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADD32mi8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADD32mi8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->rax = (QBDI::rword)&v;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(v == 0x42);
  for (auto &e : expectedPre.accesses)
    CHECK(e.see);
  for (auto &e : expectedPost.accesses)
    CHECK(e.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-rep_movsb") {
  const char source[] = "rep movsb\n";

  uint8_t src[4] = {0x11, 0x22, 0x33, 0x44};
  uint8_t dst[4] = {0, 0, 0, 0};
  size_t seenCount = 0;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB(
      "MOVSB", QBDI::POSTINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        std::vector<QBDI::MemoryAccess> accesses = vmi->getInstMemoryAccess();
        REQUIRE(accesses.size() == 2);
        CHECK(accesses[0].accessAddress == (QBDI::rword)&src[0]);
        CHECK(accesses[0].size == sizeof(src));
        CHECK(accesses[0].type == QBDI::MEMORY_READ);
        CHECK((accesses[0].flags & QBDI::MEMORY_UNKNOWN_VALUE) != 0);
        CHECK(accesses[1].accessAddress == (QBDI::rword)&dst[0]);
        CHECK(accesses[1].size == sizeof(dst));
        CHECK(accesses[1].type == QBDI::MEMORY_WRITE);
        CHECK((accesses[1].flags & QBDI::MEMORY_UNKNOWN_VALUE) != 0);
        seenCount++;
        return QBDI::VMAction::CONTINUE;
      });

  QBDI::GPRState *state = vm.getGPRState();
  state->rsi = (QBDI::rword)&src[0];
  state->rdi = (QBDI::rword)&dst[0];
  state->rcx = sizeof(src);
  state->eflags &= ~(1 << 10); // clear DF
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seenCount == 1);
  CHECK(state->rcx == 0);
  CHECK(memcmp(src, dst, sizeof(src)) == 0);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-repne_scasb") {
  const char source[] = "repne scasb\n";

  uint8_t buf[4] = {0x11, 0x22, 0x33, 0x44};
  size_t seenCount = 0;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB(
      "SCASB", QBDI::POSTINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        std::vector<QBDI::MemoryAccess> accesses = vmi->getInstMemoryAccess();
        REQUIRE(accesses.size() == 1);
        CHECK(accesses[0].accessAddress == (QBDI::rword)&buf[0]);
        CHECK(accesses[0].size == 3);
        CHECK(accesses[0].type == QBDI::MEMORY_READ);
        CHECK((accesses[0].flags & QBDI::MEMORY_UNKNOWN_VALUE) != 0);
        seenCount++;
        return QBDI::VMAction::CONTINUE;
      });

  QBDI::GPRState *state = vm.getGPRState();
  state->rdi = (QBDI::rword)&buf[0];
  state->rax = 0x33;
  state->rcx = sizeof(buf);
  state->eflags &= ~(1 << 10);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seenCount == 1);
  CHECK(state->rdi == (QBDI::rword)&buf[3]);
  CHECK(state->rcx == sizeof(buf) - 3);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86_64-data16_mov") {
  const char source[] = "movw $0x1234, %ax\n";

  bool seen = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOV16ri", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     CHECK(gprState->rax == 0xffffffffffff1234ULL);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->rax = 0xffffffffffffffffULL;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_X86_64-segment_override_fs") {
  if (!checkFeature("fsgsbase")) {
    return;
  }

  const char source[] = "movl %fs:(%rax), %ecx\n";

  uint32_t buffer[3] = {0x11111111, 0xdeadbeef, 0x22222222};
  uint32_t *target = &buffer[1];
  QBDI::rword fakeFsBase = sizeof(*target);
  bool seen = false;

  vm.setOptions(vm.getOptions() | QBDI::Options::OPT_ENABLE_FS_GS);

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOV32rm", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     std::vector<QBDI::MemoryAccess> accesses =
                         vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     // reported address is the bare linear operand, not
                     // fsbase+rax
                     CHECK(accesses[0].accessAddress == gprState->rax);
                     CHECK(accesses[0].size == 4);
                     CHECK(gprState->rcx == *target);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->fs = fakeFsBase;
  state->rax = (QBDI::rword)target - fakeFsBase;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
}
