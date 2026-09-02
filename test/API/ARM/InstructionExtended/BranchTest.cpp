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

static void checkBcc(APITest &fixture, bool taken) {
  const char source[] =
      "beq bcc_landed\n"
      "mov r0, #0x1111\n"
      "b bcc_end\n"
      "bcc_landed:\n"
      "mov r0, #0x2222\n"
      "bcc_end:\n";

  bool seen = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("Bcc", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seen = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("Bcc", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  setZ(fixture, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkBxRetCond(APITest &fixture, bool taken) {
  const char source[] =
      "mov r1, lr\n"
      "adr r0, retcond_landed\n"
      "mov lr, r0\n"
      "bxeq lr\n"
      "mov r0, #0x1111\n"
      "mov lr, r1\n"
      "b retcond_end\n"
      "retcond_landed:\n"
      "mov r0, #0x2222\n"
      "mov lr, r1\n"
      "retcond_end:\n";

  bool seen = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("BX_RET", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seen = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("BX_RET", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  setZ(fixture, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkBxCond(APITest &fixture, bool taken) {
  const char source[] =
      "adr r2, bxcond_landed\n"
      "bxeq r2\n"
      "mov r0, #0x1111\n"
      "b bxcond_end\n"
      "bxcond_landed:\n"
      "mov r0, #0x2222\n"
      "bxcond_end:\n";

  bool seen = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("BX", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seen = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("BX", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  setZ(fixture, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkBlxCond(APITest &fixture, bool taken) {
  const char source[] =
      "mov r1, lr\n"
      "adr r2, blxcond_callee\n"
      "mov r0, #0x1111\n"
      "blxeq r2\n"
      "mov lr, r1\n"
      "b blxcond_end\n"
      "blxcond_callee:\n"
      "mov r0, #0x3333\n"
      "bx lr\n"
      "blxcond_end:\n";

  QBDI::rword expectedReturnAddr = 0;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("BLX", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB(
      "BLX", QBDI::PREINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        const QBDI::InstAnalysis *ia =
            vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
        expectedReturnAddr = ia->address + ia->instSize;
        return QBDI::VMAction::CONTINUE;
      });
  fixture.vm.addMnemonicCB("BLX", QBDI::POSTINST, checkAccess, &expectedPost);
  fixture.vm.addMnemonicCB("BLX", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             if (taken) {
                               CHECK(gprState->lr == expectedReturnAddr);
                             }
                             return QBDI::VMAction::CONTINUE;
                           });

  setZ(fixture, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(retval == (taken ? 0x3333 : 0x1111));
}

static void checkBlCond(APITest &fixture, bool taken) {
  const char source[] =
      "mov r1, lr\n"
      "mov r0, #0x1111\n"
      "bleq blcond_callee\n"
      "mov lr, r1\n"
      "b blcond_end\n"
      "blcond_callee:\n"
      "mov r0, #0x4444\n"
      "bx lr\n"
      "blcond_end:\n";

  QBDI::rword expectedReturnAddr = 0;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  fixture.vm.addMnemonicCB("BL", QBDI::PREINST, checkAccess, &expectedPre);
  fixture.vm.addMnemonicCB(
      "BL", QBDI::PREINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        const QBDI::InstAnalysis *ia =
            vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
        expectedReturnAddr = ia->address + ia->instSize;
        return QBDI::VMAction::CONTINUE;
      });
  fixture.vm.addMnemonicCB("BL", QBDI::POSTINST, checkAccess, &expectedPost);
  fixture.vm.addMnemonicCB("BL", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             if (taken) {
                               CHECK(gprState->lr == expectedReturnAddr);
                             }
                             return QBDI::VMAction::CONTINUE;
                           });

  setZ(fixture, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(retval == (taken ? 0x4444 : 0x1111));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-bx_ret") {
  const char source[] =
      "mov r1, lr\n"
      "adr r0, ret_landed\n"
      "mov lr, r0\n"
      "bx lr\n"
      "mov r0, #0x1111\n"
      "b ret_end\n"
      "ret_landed:\n"
      "mov r0, #0x2222\n"
      "mov lr, r1\n"
      "ret_end:\n";

  bool seen = false;
  bool seenPost = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BX_RET", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("BX_RET", QBDI::POSTINST,
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-bx") {
  const char source[] =
      "adr r2, bx_landed\n"
      "bx r2\n"
      "mov r0, #0x1111\n"
      "b bx_end\n"
      "bx_landed:\n"
      "mov r0, #0x2222\n"
      "bx_end:\n";

  bool seen = false;
  bool seenPost = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BX", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("BX", QBDI::POSTINST,
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-blx") {
  const char source[] =
      "mov r1, lr\n"
      "adr r0, blx_callee\n"
      "blx r0\n"
      "mov lr, r1\n"
      "b blx_end\n"
      "blx_callee:\n"
      "mov r0, #0x3333\n"
      "bx lr\n"
      "blx_end:\n";

  QBDI::rword expectedReturnAddr = 0;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  vm.addMnemonicCB("BLX", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BLX", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     expectedReturnAddr = ia->address + ia->instSize;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("BLX", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("BLX", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->lr == expectedReturnAddr);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(retval == 0x3333);
  CHECK(expectedReturnAddr != 0);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-bl") {
  const char source[] =
      "mov r1, lr\n"
      "bl bl_callee\n"
      "mov lr, r1\n"
      "b bl_end\n"
      "bl_callee:\n"
      "mov r0, #0x4444\n"
      "bx lr\n"
      "bl_end:\n";

  QBDI::rword expectedReturnAddr = 0;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  vm.addMnemonicCB("BL", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BL", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     expectedReturnAddr = ia->address + ia->instSize;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("BL", QBDI::POSTINST, checkAccess, &expectedPost);
  vm.addMnemonicCB("BL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(gprState->lr == expectedReturnAddr);
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(retval == 0x4444);
  CHECK(expectedReturnAddr != 0);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-bcc_taken") {
  checkBcc(*this, true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-bcc_not_taken") {
  checkBcc(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-bx_ret_cond_taken") {
  checkBxRetCond(*this, true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-bx_ret_cond_not_taken") {
  checkBxRetCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-bx_cond_taken") {
  checkBxCond(*this, true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-bx_cond_not_taken") {
  checkBxCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-blx_cond_taken") {
  checkBlxCond(*this, true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-blx_cond_not_taken") {
  checkBlxCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-bl_cond_taken") {
  checkBlCond(*this, true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-bl_cond_not_taken") {
  checkBlCond(*this, false);
}
