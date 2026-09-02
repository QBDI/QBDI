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

static void checkPcDestRedirectThumb(APITest &fixture, const char *mnemonic,
                                     const char *source,
                                     QBDI::rword expectedPc) {
  bool seenPre = false, seen = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB(mnemonic, QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB(
      mnemonic, QBDI::POSTINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        CHECK(vmi->getInstMemoryAccess().empty());
        CHECK(gprState->pc == (expectedPc | 1));
        const QBDI::InstAnalysis *ia =
            vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
        gprState->pc = (ia->address + ia->instSize) | 1;
        fixture.vm.setGPRState(gprState);
        seen = true;
        return QBDI::VMAction::BREAK_TO_VM;
      });

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seen);
  CHECK(retval == 0x1111);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldri8_pc") {
  const char source[] =
      "adr r0, t2ldri8_data_after\n"
      "ldr pc, [r0, #-4]\n"
      "mov r0, #0x1111\n"
      "b t2ldri8_end\n"
      "t2ldri8_landed:\n"
      "mov r0, #0x2222\n"
      "b t2ldri8_end\n"
      ".p2align 2\n"
      "t2ldri8_data:\n"
      ".word t2ldri8_landed + 1\n"
      "t2ldri8_data_after:\n"
      "t2ldri8_end:\n";

  bool seenPre = false, seen = false;
  QBDI::rword preValue = 0;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRi8", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(accesses[0].accessAddress == gprState->r0 - 4);
                     preValue = accesses[0].value;
                     seenPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("t2LDRi8", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].value == gprState->pc);
                     CHECK(accesses[0].value == preValue);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(accesses[0].accessAddress == gprState->r0 - 4);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seen);
  CHECK(retval == 0x2222);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldr_pre_pc") {
  const char source[] =
      "adr r0, t2ldrpre_data\n"
      "ldr pc, [r0, #4]!\n"
      "mov r0, #0x1111\n"
      "b t2ldrpre_end\n"
      "t2ldrpre_landed:\n"
      "mov r0, #0x2222\n"
      "b t2ldrpre_end\n"
      ".p2align 2\n"
      "t2ldrpre_data:\n"
      ".word 0x12345678\n"
      ".word t2ldrpre_landed + 1\n"
      "t2ldrpre_end:\n";

  bool seenPre = false, seen = false;
  QBDI::rword preValue = 0;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDR_PRE", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(accesses[0].accessAddress == gprState->r0 + 4);
                     preValue = accesses[0].value;
                     seenPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("t2LDR_PRE", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].value == gprState->pc);
                     CHECK(accesses[0].value == preValue);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(gprState->r0 == accesses[0].accessAddress);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seen);
  CHECK(retval == 0x2222);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldr_post_pc") {
  const char source[] =
      "adr r0, t2ldrpost_data\n"
      "ldr pc, [r0], #4\n"
      "mov r0, #0x1111\n"
      "b t2ldrpost_end\n"
      "t2ldrpost_landed:\n"
      "mov r0, #0x2222\n"
      "b t2ldrpost_end\n"
      ".p2align 2\n"
      "t2ldrpost_data:\n"
      ".word t2ldrpost_landed + 1\n"
      ".word 0x12345678\n"
      "t2ldrpost_end:\n";

  bool seenPre = false, seen = false;
  QBDI::rword preValue = 0;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDR_POST", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(accesses[0].accessAddress == gprState->r0);
                     preValue = accesses[0].value;
                     seenPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("t2LDR_POST", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].value == gprState->pc);
                     CHECK(accesses[0].value == preValue);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(gprState->r0 == accesses[0].accessAddress + 4);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seen);
  CHECK(retval == 0x2222);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldri12_pc") {
  const char source[] =
      "adr r0, t2ldri12_data\n"
      "ldr pc, [r0, #4]\n"
      "mov r0, #0x1111\n"
      "b t2ldri12_end\n"
      "t2ldri12_landed:\n"
      "mov r0, #0x2222\n"
      "b t2ldri12_end\n"
      ".p2align 2\n"
      "t2ldri12_data:\n"
      ".word 0x12345678\n"
      ".word t2ldri12_landed + 1\n"
      "t2ldri12_end:\n";

  bool seenPre = false, seen = false;
  QBDI::rword preValue = 0;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRi12", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(accesses[0].accessAddress == gprState->r0 + 4);
                     preValue = accesses[0].value;
                     seenPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("t2LDRi12", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].value == gprState->pc);
                     CHECK(accesses[0].value == preValue);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(accesses[0].accessAddress == gprState->r0 + 4);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seen);
  CHECK(retval == 0x2222);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrs_pc") {
  const char source[] =
      "adr r0, t2ldrs_data\n"
      "mov r1, #4\n"
      "ldr pc, [r0, r1, lsl #2]\n"
      "mov r0, #0x1111\n"
      "b t2ldrs_end\n"
      "t2ldrs_landed:\n"
      "mov r0, #0x2222\n"
      "b t2ldrs_end\n"
      ".p2align 2\n"
      "t2ldrs_data:\n"
      ".word 0x11111111\n"
      ".word 0x22222222\n"
      ".word 0x33333333\n"
      ".word 0x44444444\n"
      ".word t2ldrs_landed + 1\n"
      "t2ldrs_end:\n";

  bool seenPre = false, seen = false;
  QBDI::rword preValue = 0;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRs", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(accesses[0].accessAddress == gprState->r0 + 16);
                     preValue = accesses[0].value;
                     seenPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("t2LDRs", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 1);
                     CHECK(accesses[0].value == gprState->pc);
                     CHECK(accesses[0].value == preValue);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(accesses[0].accessAddress == gprState->r0 + 16);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seen);
  CHECK(retval == 0x2222);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tmovr_pc_dest") {
  const char source[] =
      "mov r1, #0x2000\n"
      "mov pc, r1\n"
      "mov r0, #0x1111\n";

  checkPcDestRedirectThumb(*this, "tMOVr", source, 0x2000);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tmovr_pc_src") {
  const char source[] = "mov r1, pc\n";

  bool seenPre = false, seen = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tMOVr", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seenPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("tMOVr", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     CHECK(gprState->r1 == ia->address + 4);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seen);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-taddrsp_pc") {
  const char source[] =
      "add pc, sp, pc\n"
      "mov r0, #0x1111\n";

  QBDI::rword expectedPc = 0;
  bool seen = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tADDrSP", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     expectedPc = (gprState->sp + (ia->address + 4)) | 1;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("tADDrSP", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     CHECK(gprState->pc == expectedPc);
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     gprState->pc = (ia->address + ia->instSize) | 1;
                     vm.setGPRState(gprState);
                     seen = true;
                     return QBDI::VMAction::BREAK_TO_VM;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seen);
  CHECK(retval == 0x1111);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-taddhirr_pc_dest") {
  const char source[] =
      "mov r1, #0x1000\n"
      "add pc, r1\n"
      "mov r0, #0x1111\n";

  QBDI::rword expectedPc = 0;
  bool seen = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tADDhirr", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     expectedPc = ((ia->address + 4) + gprState->r1) | 1;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("tADDhirr", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     CHECK(gprState->pc == expectedPc);
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     gprState->pc = (ia->address + ia->instSize) | 1;
                     vm.setGPRState(gprState);
                     seen = true;
                     return QBDI::VMAction::BREAK_TO_VM;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seen);
  CHECK(retval == 0x1111);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-taddhirr_pc_src") {
  const char source[] =
      "mov r0, #0x1000\n"
      "add r0, pc\n";

  bool seenPre = false, seen = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tADDhirr", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seenPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("tADDhirr", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     CHECK(gprState->r0 == 0x1000u + (ia->address + 4));
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seen);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-taddspr_pc") {
  const char source[] = "add sp, pc\n";

  QBDI::rword origSp = 0, expectedSp = 0;
  bool seen = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("tADDspr", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     origSp = gprState->sp;
                     expectedSp = origSp + (ia->address + 4);
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("tADDspr", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     CHECK(gprState->sp == expectedSp);
                     gprState->sp = origSp;
                     vm.setGPRState(gprState);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seen);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2ldrdi8_pc") {
  const char source[] =
      "ldrd r0, r1, [pc, #4]\n"
      "bx lr\n"
      ".short 0\n"
      ".word 0x11223344\n"
      ".word 0x55667788\n";
  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb);
  QBDI::rword baseAddr = codeAddr & ~(QBDI::rword)1;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  expectedPre.accesses = {
      {baseAddr + 8, 0x11223344, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
      {baseAddr + 12, 0x55667788, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  };
  expectedPost.accesses = expectedPre.accesses;
  vm.addMnemonicCB("t2LDRDi8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRDi8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == 0x11223344u);
  CHECK(finalState->r1 == 0x55667788u);
}
