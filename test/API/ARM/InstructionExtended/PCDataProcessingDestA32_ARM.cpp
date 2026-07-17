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

static constexpr QBDI::rword R1 = 0x3000;
static constexpr QBDI::rword R2 = 0x3100;

static void checkPcDestRedirect(APITest &fixture, const char *mnemonic,
                                const char *source, QBDI::rword expectedPc,
                                bool expectMemAccess) {
  bool seenPre = false;
  bool seen = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB(mnemonic, QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             if (!expectMemAccess) {
                               CHECK(vmi->getInstMemoryAccess().empty());
                             }
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB(
      mnemonic, QBDI::POSTINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        if (!expectMemAccess) {
          CHECK(vmi->getInstMemoryAccess().empty());
        }
        CHECK(gprState->pc == expectedPc);
        const QBDI::InstAnalysis *ia =
            vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
        gprState->pc = ia->address + ia->instSize;
        fixture.vm.setGPRState(gprState);
        seen = true;
        return QBDI::VMAction::BREAK_TO_VM;
      });

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r1 = R1;
  state->r2 = R2;
  fixture.vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seen);
  CHECK(retval == 0x1111);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldr_pre_imm_pc") {
  const char source[] =
      "adr r0, ldrpreimm_data\n"
      "ldr pc, [r0, #4]!\n"
      "mov r0, #0x1111\n"
      "b ldrpreimm_end\n"
      "ldrpreimm_landed:\n"
      "mov r0, #0x2222\n"
      "b ldrpreimm_end\n"
      "ldrpreimm_data:\n"
      ".word 0x12345678\n"
      ".word ldrpreimm_landed\n"
      "ldrpreimm_end:\n";

  bool seenPre = false;
  bool seen = false;
  QBDI::rword loadedPcValue = 0;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDR_PRE_IMM", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(accesses[0].accessAddress == gprState->r0 + 4);
                     loadedPcValue = accesses[0].value;
                     seenPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("LDR_PRE_IMM", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].value == gprState->pc);
                     CHECK(accesses[0].value == loadedPcValue);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(gprState->r0 == accesses[0].accessAddress);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seen);
  CHECK(retval == 0x2222);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldr_post_imm_pc") {
  const char source[] =
      "adr r0, ldrpostimm_data\n"
      "ldr pc, [r0], #4\n"
      "mov r0, #0x1111\n"
      "b ldrpostimm_end\n"
      "ldrpostimm_landed:\n"
      "mov r0, #0x2222\n"
      "b ldrpostimm_end\n"
      "ldrpostimm_data:\n"
      ".word ldrpostimm_landed\n"
      ".word 0x12345678\n"
      "ldrpostimm_end:\n";

  bool seenPre = false;
  bool seen = false;
  QBDI::rword loadedPcValue = 0;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDR_POST_IMM", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(accesses[0].accessAddress == gprState->r0);
                     loadedPcValue = accesses[0].value;
                     seenPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("LDR_POST_IMM", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].value == gprState->pc);
                     CHECK(accesses[0].value == loadedPcValue);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(gprState->r0 == accesses[0].accessAddress + 4);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seen);
  CHECK(retval == 0x2222);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-mvni_pc") {
  checkPcDestRedirect(*this, "MVNi",
                      "mvn pc, #0x1000\n"
                      "mov r0, #0x1111\n",
                      (QBDI::rword) ~(QBDI::rword)0x1000, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-movi_pc") {
  checkPcDestRedirect(*this, "MOVi",
                      "mov pc, #0x1000\n"
                      "eor r0, r0, r0\n"
                      "orr r0, r0, #0x1100\n"
                      "orr r0, r0, #0x11\n",
                      0x1000, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-movi16_pc") {
  checkPcDestRedirect(*this, "MOVi16",
                      "movw pc, #0x1234\n"
                      "eor r0, r0, r0\n"
                      "orr r0, r0, #0x1100\n"
                      "orr r0, r0, #0x11\n",
                      0x1234, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-adcri_pc") {
  checkPcDestRedirect(*this, "ADCri",
                      "cmp r3, r3\n"
                      "adc pc, r1, #4\n"
                      "mov r0, #0x1111\n",
                      R1 + 4 + 1, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-addri_pc") {
  checkPcDestRedirect(*this, "ADDri",
                      "add pc, r1, #4\n"
                      "mov r0, #0x1111\n",
                      R1 + 4, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-andri_pc") {
  checkPcDestRedirect(*this, "ANDri",
                      "and pc, r1, #0xff\n"
                      "mov r0, #0x1111\n",
                      R1 & 0xffu, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-bicri_pc") {
  checkPcDestRedirect(*this, "BICri",
                      "bic pc, r1, #0xff\n"
                      "mov r0, #0x1111\n",
                      R1 & ~0xffu, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-eorri_pc") {
  checkPcDestRedirect(*this, "EORri",
                      "eor pc, r1, #0xff\n"
                      "mov r0, #0x1111\n",
                      R1 ^ 0xffu, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldri12_pc") {
  const char source[] =
      "adr r0, ldri12_data\n"
      "ldr pc, [r0, #4]\n"
      "mov r0, #0x1111\n"
      "b ldri12_end\n"
      "ldri12_landed:\n"
      "mov r0, #0x2222\n"
      "b ldri12_end\n"
      "ldri12_data:\n"
      ".word 0x12345678\n"
      ".word ldri12_landed\n"
      "ldri12_end:\n";

  bool seenPre = false;
  bool seen = false;
  QBDI::rword loadedPcValue = 0;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRi12", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(accesses[0].accessAddress == gprState->r0 + 4);
                     loadedPcValue = accesses[0].value;
                     seenPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("LDRi12", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].value == gprState->pc);
                     CHECK(accesses[0].value == loadedPcValue);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(accesses[0].accessAddress == gprState->r0 + 4);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seen);
  CHECK(retval == 0x2222);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrrs_pc") {
  const char source[] =
      "adr r0, ldrrs_data\n"
      "mov r1, #4\n"
      "ldr pc, [r0, r1]\n"
      "mov r0, #0x1111\n"
      "b ldrrs_end\n"
      "ldrrs_landed:\n"
      "mov r0, #0x2222\n"
      "b ldrrs_end\n"
      "ldrrs_data:\n"
      ".word 0x12345678\n"
      ".word ldrrs_landed\n"
      "ldrrs_end:\n";

  bool seenPre = false;
  bool seen = false;
  QBDI::rword loadedPcValue = 0;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDRrs", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(accesses[0].accessAddress == gprState->r0 + 4);
                     loadedPcValue = accesses[0].value;
                     seenPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("LDRrs", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].value == gprState->pc);
                     CHECK(accesses[0].value == loadedPcValue);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(accesses[0].accessAddress == gprState->r0 + 4);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seen);
  CHECK(retval == 0x2222);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-movsi_pc") {
  checkPcDestRedirect(*this, "MOVsi",
                      "mov pc, r1, lsl #2\n"
                      "mov r0, #0x1111\n",
                      R1 << 2, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-mvnr_pc") {
  checkPcDestRedirect(*this, "MVNr",
                      "mvn pc, r1\n"
                      "mov r0, #0x1111\n",
                      ~R1, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-orrri_pc") {
  checkPcDestRedirect(*this, "ORRri",
                      "orr pc, r1, #0xff\n"
                      "mov r0, #0x1111\n",
                      R1 | 0xffu, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-rsbri_pc") {
  checkPcDestRedirect(*this, "RSBri",
                      "rsb pc, r1, #0x4000\n"
                      "mov r0, #0x1111\n",
                      0x4000 - R1, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-rscri_pc") {
  checkPcDestRedirect(*this, "RSCri",
                      "cmp r3, r3\n"
                      "rsc pc, r1, #0x4000\n"
                      "mov r0, #0x1111\n",
                      0x4000 - R1, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-sbcri_pc") {
  checkPcDestRedirect(*this, "SBCri",
                      "cmp r3, r3\n"
                      "sbc pc, r1, #4\n"
                      "mov r0, #0x1111\n",
                      R1 - 4, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-subri_pc") {
  checkPcDestRedirect(*this, "SUBri",
                      "sub pc, r1, #4\n"
                      "mov r0, #0x1111\n",
                      R1 - 4, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-adcrr_pc") {
  checkPcDestRedirect(*this, "ADCrr",
                      "cmp r3, r3\n"
                      "adc pc, r1, r2\n"
                      "mov r0, #0x1111\n",
                      R1 + R2 + 1, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-adcrsi_pc") {
  checkPcDestRedirect(*this, "ADCrsi",
                      "cmp r3, r3\n"
                      "adc pc, r1, r2, lsl #2\n"
                      "mov r0, #0x1111\n",
                      R1 + (R2 << 2) + 1, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-addrsi_pc") {
  checkPcDestRedirect(*this, "ADDrsi",
                      "add pc, r1, r2, lsl #2\n"
                      "mov r0, #0x1111\n",
                      R1 + (R2 << 2), false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-andrr_pc") {
  checkPcDestRedirect(*this, "ANDrr",
                      "and pc, r1, r2\n"
                      "mov r0, #0x1111\n",
                      R1 & R2, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-andrsi_pc") {
  checkPcDestRedirect(*this, "ANDrsi",
                      "and pc, r1, r2, lsl #2\n"
                      "mov r0, #0x1111\n",
                      R1 & (R2 << 2), false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-bicrr_pc") {
  checkPcDestRedirect(*this, "BICrr",
                      "bic pc, r1, r2\n"
                      "mov r0, #0x1111\n",
                      R1 & ~R2, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-bicrsi_pc") {
  checkPcDestRedirect(*this, "BICrsi",
                      "bic pc, r1, r2, lsl #2\n"
                      "mov r0, #0x1111\n",
                      R1 & ~(R2 << 2), false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-eorrr_pc") {
  checkPcDestRedirect(*this, "EORrr",
                      "eor pc, r1, r2\n"
                      "mov r0, #0x1111\n",
                      R1 ^ R2, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-eorrsi_pc") {
  checkPcDestRedirect(*this, "EORrsi",
                      "eor pc, r1, r2, lsl #2\n"
                      "mov r0, #0x1111\n",
                      R1 ^ (R2 << 2), false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-orrrr_pc") {
  checkPcDestRedirect(*this, "ORRrr",
                      "orr pc, r1, r2\n"
                      "mov r0, #0x1111\n",
                      R1 | R2, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-orrrsi_pc") {
  checkPcDestRedirect(*this, "ORRrsi",
                      "orr pc, r1, r2, lsl #2\n"
                      "mov r0, #0x1111\n",
                      R1 | (R2 << 2), false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-rsbrr_pc") {
  checkPcDestRedirect(*this, "RSBrr",
                      "rsb pc, r1, r2\n"
                      "mov r0, #0x1111\n",
                      R2 - R1, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-rsbrsi_pc") {
  checkPcDestRedirect(*this, "RSBrsi",
                      "rsb pc, r1, r2, lsl #2\n"
                      "mov r0, #0x1111\n",
                      (R2 << 2) - R1, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-rscrr_pc") {
  checkPcDestRedirect(*this, "RSCrr",
                      "cmp r3, r3\n"
                      "rsc pc, r1, r2\n"
                      "mov r0, #0x1111\n",
                      R2 - R1, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-rscrsi_pc") {
  checkPcDestRedirect(*this, "RSCrsi",
                      "cmp r3, r3\n"
                      "rsc pc, r1, r2, lsl #2\n"
                      "mov r0, #0x1111\n",
                      (R2 << 2) - R1, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-sbcrr_pc") {
  checkPcDestRedirect(*this, "SBCrr",
                      "cmp r3, r3\n"
                      "sbc pc, r1, r2\n"
                      "mov r0, #0x1111\n",
                      R1 - R2, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-sbcrsi_pc") {
  checkPcDestRedirect(*this, "SBCrsi",
                      "cmp r3, r3\n"
                      "sbc pc, r1, r2, lsl #2\n"
                      "mov r0, #0x1111\n",
                      R1 - (R2 << 2), false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-subrr_pc") {
  checkPcDestRedirect(*this, "SUBrr",
                      "sub pc, r1, r2\n"
                      "mov r0, #0x1111\n",
                      R1 - R2, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-subrsi_pc") {
  checkPcDestRedirect(*this, "SUBrsi",
                      "sub pc, r1, r2, lsl #2\n"
                      "mov r0, #0x1111\n",
                      R1 - (R2 << 2), false);
}
