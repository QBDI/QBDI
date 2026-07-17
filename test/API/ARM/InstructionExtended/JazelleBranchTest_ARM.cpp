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
#include <cerrno>
#include <csignal>
#include <cstdint>
#include <sys/wait.h>
#include <unistd.h>

#include "API/APITest.h"
#include "MemAccessTestUtils_ARM.h"

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

static constexpr QBDI::rword CPSR_Z = (1u << 30);

static void setZ(APITest &fixture, bool taken) {
  QBDI::GPRState *state = fixture.vm.getGPRState();
  if (taken) {
    state->cpsr |= CPSR_Z;
  } else {
    state->cpsr &= ~CPSR_Z;
  }
  fixture.vm.setGPRState(state);
}

namespace {

bool isJazelleTrivialImplementation() {
  static const bool trivial = [] {
    pid_t pid = fork();
    if (pid < 0) {
      return false;
    }
    if (pid == 0) {
      struct sigaction newAction{};
      newAction.sa_handler = [](int) { _exit(1); };
      sigemptyset(&newAction.sa_mask);
      newAction.sa_flags = 0;

      if (sigaction(SIGILL, &newAction, nullptr) != 0) {
        _exit(1);
      }

      uint32_t jidr = 0;
      asm volatile("mrc p14, 7, %0, c0, c0, 0" : "=r"(jidr));

      constexpr uint32_t implementerAndSubarchMask = 0x0FFFF000;
      _exit((jidr & implementerAndSubarchMask) == 0 ? 0 : 1);
    }

    int status = 0;
    pid_t waited;
    do {
      waited = waitpid(pid, &status, 0);
    } while (waited == -1 and errno == EINTR);

    if (waited != pid) {
      return false;
    }

    return WIFEXITED(status) and WEXITSTATUS(status) == 0;
  }();
  return trivial;
}

} // namespace

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-bxj") {
  if (not isJazelleTrivialImplementation()) {
    return;
  }

  const char source[] =
      "adr r2, bxj_landed\n"
      "bxj r2\n"
      "mov r0, #0x1111\n"
      "b bxj_end\n"
      "bxj_landed:\n"
      "mov r0, #0x2222\n"
      "bxj_end:\n";

  bool seen = false;
  bool seenPost = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BXJ", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("BXJ", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
  CHECK(seenPost);
  CHECK(retval == 0x2222);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2bxj") {
  if (not isJazelleTrivialImplementation()) {
    return;
  }

  const char source[] =
      "adr r2, t2bxj_landed\n"
      "orr r2, r2, #1\n"
      "bxj r2\n"
      "mov r0, #0x1111\n"
      "b t2bxj_end\n"
      "t2bxj_landed:\n"
      "mov r0, #0x2222\n"
      "t2bxj_end:\n";

  bool seen = false;
  bool seenPost = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2BXJ", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("t2BXJ", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seen);
  CHECK(seenPost);
  CHECK(retval == 0x2222);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-bxj_cond_taken") {
  if (not isJazelleTrivialImplementation()) {
    return;
  }

  const char source[] =
      "adr r2, bxjcond_landed\n"
      "bxjeq r2\n"
      "mov r0, #0x1111\n"
      "b bxjcond_end\n"
      "bxjcond_landed:\n"
      "mov r0, #0x2222\n"
      "bxjcond_end:\n";

  bool seen = false;
  bool seenPost = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BXJ", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("BXJ", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  setZ(*this, true);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
  CHECK(seenPost);
  CHECK(retval == 0x2222);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-bxj_cond_not_taken") {
  if (not isJazelleTrivialImplementation()) {
    return;
  }

  const char source[] =
      "adr r2, bxjcond2_landed\n"
      "bxjeq r2\n"
      "mov r0, #0x1111\n"
      "b bxjcond2_end\n"
      "bxjcond2_landed:\n"
      "mov r0, #0x2222\n"
      "bxjcond2_end:\n";

  bool seen = false;
  bool seenPost = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BXJ", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("BXJ", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  setZ(*this, false);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
  CHECK(seenPost);
  CHECK(retval == 0x1111);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2bxj_cond_taken") {
  if (not isJazelleTrivialImplementation()) {
    return;
  }

  const char source[] =
      "adr r2, t2bxjcond_landed\n"
      "orr r2, r2, #1\n"
      "it eq\n"
      "bxjeq r2\n"
      "mov r0, #0x1111\n"
      "b t2bxjcond_end\n"
      "t2bxjcond_landed:\n"
      "mov r0, #0x2222\n"
      "t2bxjcond_end:\n";

  bool seen = false;
  bool seenPost = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2BXJ", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("t2BXJ", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  setZ(*this, true);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seen);
  CHECK(seenPost);
  CHECK(retval == 0x2222);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2bxj_cond_not_taken") {
  if (not isJazelleTrivialImplementation()) {
    return;
  }

  const char source[] =
      "adr r2, t2bxjcond2_landed\n"
      "orr r2, r2, #1\n"
      "it eq\n"
      "bxjeq r2\n"
      "mov r0, #0x1111\n"
      "b t2bxjcond2_end\n"
      "t2bxjcond2_landed:\n"
      "mov r0, #0x2222\n"
      "t2bxjcond2_end:\n";

  bool seen = false;
  bool seenPost = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2BXJ", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("t2BXJ", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  setZ(*this, false);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seen);
  CHECK(seenPost);
  CHECK(retval == 0x1111);
}
