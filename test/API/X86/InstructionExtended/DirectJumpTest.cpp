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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-jmp_short") {
  const char source[] =
      "jmp jmp_short_forward\n"
      "jmp_short_back:\n"
      "movl $0x2222, %eax\n"
      "jmp jmp_short_end\n"
      "jmp_short_forward:\n"
      "jmp jmp_short_back\n"
      "jmp_short_end:\n";

  size_t seen = 0;
  size_t seenPost = 0;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("JMP_1", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seen++;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("JMP_1", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seenPost++;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen == 3);
  CHECK(seenPost == 3);
  CHECK(retval == 0x2222);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-jmp_near") {
  const char source[] =
      "jmp jmp_near_target\n"
      ".fill 200, 1, 0x90\n"
      "jmp_near_target:\n"
      "movl $0x4444, %eax\n";

  bool seen = false;
  bool seenPost = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("JMP_4", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("JMP_4", QBDI::POSTINST,
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
  CHECK(retval == 0x4444);
}
