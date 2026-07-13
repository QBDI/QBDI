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

static constexpr QBDI::rword CPSR_Z = (1u << 30);

static constexpr char TCBZ_SRC[] =
    "cbz r0, tcbz_landed\n"
    "mov r0, #0x1111\n"
    "b tcbz_end\n"
    "tcbz_landed:\n"
    "mov r0, #0x2222\n"
    "tcbz_end:\n";

static constexpr char TCBNZ_SRC[] =
    "cbnz r0, tcbnz_landed\n"
    "mov r0, #0x1111\n"
    "b tcbnz_end\n"
    "tcbnz_landed:\n"
    "mov r0, #0x2222\n"
    "tcbnz_end:\n";

static void setZ(APITest &fixture, bool taken) {
  QBDI::GPRState *state = fixture.vm.getGPRState();
  if (taken) {
    state->cpsr |= CPSR_Z;
  } else {
    state->cpsr &= ~CPSR_Z;
  }
  fixture.vm.setGPRState(state);
}

static void checkTbxCond(APITest &fixture, bool taken) {
  const char source[] =
      "adr r2, tbxcond_landed\n"
      "orr r2, r2, #1\n"
      "it eq\n"
      "bxeq r2\n"
      "mov r0, #0x1111\n"
      "b tbxcond_end\n"
      "tbxcond_landed:\n"
      "mov r0, #0x2222\n"
      "tbxcond_end:\n";

  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("tBX", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  setZ(fixture, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkTblCond(APITest &fixture, bool taken) {
  const char source[] =
      "mov r1, lr\n"
      "mov r0, #0x1111\n"
      "it eq\n"
      "bleq tblcond_callee\n"
      "mov lr, r1\n"
      "b tblcond_end\n"
      "tblcond_callee:\n"
      "mov r0, #0x4444\n"
      "bx lr\n"
      "tblcond_end:\n";

  QBDI::rword expectedReturnAddr = 0;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB(
      "tBL", QBDI::PREINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        const QBDI::InstAnalysis *ia =
            vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
        expectedReturnAddr = ia->address + ia->instSize;
        return QBDI::VMAction::CONTINUE;
      });
  fixture.vm.addMnemonicCB("tBL", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             if (taken) {
                               CHECK(gprState->lr == (expectedReturnAddr | 1));
                             }
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  setZ(fixture, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x4444 : 0x1111));
}

static void checkTbcc(APITest &fixture, bool taken) {
  const char source[] =
      "beq tbcc_landed\n"
      "mov r0, #0x1111\n"
      "b tbcc_end\n"
      "tbcc_landed:\n"
      "mov r0, #0x2222\n"
      "tbcc_end:\n";

  bool seen = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("tBcc", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seen = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("tBcc", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  setZ(fixture, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seen);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkRegBranchThumb(APITest &fixture, const char *mnemonic,
                                const char *source, QBDI::rword r0Taken,
                                QBDI::rword r0NotTaken, bool taken) {
  bool seen = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB(mnemonic, QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seen = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB(mnemonic, QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = taken ? r0Taken : r0NotTaken;
  fixture.vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seen);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tbx") {
  const char source[] =
      "adr r2, tbx_landed\n"
      "orr r2, r2, #1\n"
      "bx r2\n"
      "mov r0, #0x1111\n"
      "b tbx_end\n"
      "tbx_landed:\n"
      "mov r0, #0x2222\n"
      "tbx_end:\n";

  bool seen = false;
  bool seenPost = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tBX", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("tBX", QBDI::POSTINST,
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tbl") {
  const char source[] =
      "mov r1, lr\n"
      "bl tbl_callee\n"
      "mov lr, r1\n"
      "b tbl_end\n"
      "tbl_callee:\n"
      "mov r0, #0x4444\n"
      "bx lr\n"
      "tbl_end:\n";

  QBDI::rword expectedReturnAddr = 0;
  bool seen = false, seenPost = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tBL", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     expectedReturnAddr = ia->address + ia->instSize;
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("tBL", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     CHECK(gprState->lr == (expectedReturnAddr | 1));
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seen);
  CHECK(seenPost);
  CHECK(retval == 0x4444);
  CHECK(expectedReturnAddr != 0);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tbx_cond_taken") {
  checkTbxCond(*this, true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tbx_cond_not_taken") {
  checkTbxCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tbl_cond_taken") {
  checkTblCond(*this, true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tbl_cond_not_taken") {
  checkTblCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tbcc_taken") {
  checkTbcc(*this, true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tbcc_not_taken") {
  checkTbcc(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tcbz_taken") {
  checkRegBranchThumb(*this, "tCBZ", TCBZ_SRC, 0, 5, true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tcbz_not_taken") {
  checkRegBranchThumb(*this, "tCBZ", TCBZ_SRC, 0, 5, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tcbnz_taken") {
  checkRegBranchThumb(*this, "tCBNZ", TCBNZ_SRC, 5, 0, true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tcbnz_not_taken") {
  checkRegBranchThumb(*this, "tCBNZ", TCBNZ_SRC, 5, 0, false);
}
