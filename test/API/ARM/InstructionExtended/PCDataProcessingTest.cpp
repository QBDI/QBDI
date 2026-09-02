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

static void setZ(APITest &fixture, bool taken) {
  QBDI::GPRState *state = fixture.vm.getGPRState();
  if (taken) {
    state->cpsr |= CPSR_Z;
  } else {
    state->cpsr &= ~CPSR_Z;
  }
  fixture.vm.setGPRState(state);
}

static void checkMovPcRCond(APITest &fixture, bool taken) {
  const char source[] =
      "adr r1, movpccond_landed\n"
      "moveq pc, r1\n"
      "mov r0, #0x1111\n"
      "b movpccond_end\n"
      "movpccond_landed:\n"
      "mov r0, #0x2222\n"
      "movpccond_end:\n";

  bool seenPre = false, seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("MOVr", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             if (!seenPre) {
                               CHECK(vmi->getInstMemoryAccess().empty());
                               seenPre = true;
                             }
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("MOVr", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             if (!seenPost) {
                               CHECK(vmi->getInstMemoryAccess().empty());
                               seenPost = true;
                             }
                             return QBDI::VMAction::CONTINUE;
                           });

  setZ(fixture, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkAddPcPcRCond(APITest &fixture, bool taken) {
  const char source[] =
      "mov r0, #4\n"
      "addeq pc, pc, r0\n"
      "mov r0, #0x1111\n"
      "b addpccond_end\n"
      "addpccond_landed:\n"
      "mov r0, #0x2222\n"
      "addpccond_end:\n";

  bool seenPre = false, seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("ADDrr", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("ADDrr", QBDI::POSTINST,
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
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkLdmPcCond(APITest &fixture, bool taken) {
  const char source[] =
      "adr r1, ldmcond_data\n"
      "ldmiaeq r1, {r0, pc}\n"
      "mov r0, #0x1111\n"
      "b ldmcond_end\n"
      "ldmcond_landed:\n"
      "mov r0, #0x2222\n"
      "b ldmcond_end\n"
      "ldmcond_data:\n"
      ".word 0x12345678\n"
      ".word ldmcond_landed\n"
      "ldmcond_end:\n";

  bool seenPost = false;
  QBDI::rword baseAddr = 0;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("LDMIA", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             baseAddr = gprState->r1;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("LDMIA", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             auto accesses = vmi->getInstMemoryAccess();
                             if (taken) {
                               REQUIRE(accesses.size() == 2);
                               CHECK(accesses[0].accessAddress == baseAddr);
                               CHECK(accesses[0].value == 0x12345678);
                               CHECK(accesses[0].size == 4);
                               CHECK(accesses[0].type == QBDI::MEMORY_READ);
                               CHECK(accesses[1].accessAddress == baseAddr + 4);
                               CHECK(accesses[1].value == gprState->pc);
                               CHECK(accesses[1].size == 4);
                               CHECK(accesses[1].type == QBDI::MEMORY_READ);
                             } else {
                               CHECK(accesses.empty());
                             }
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  setZ(fixture, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkTadrCond(APITest &fixture, bool taken) {
  const char source[] =
      "it eq\n"
      "adreq r0, tadrcond_target\n"
      "tadrcond_target:\n"
      "mov r1, #0x2222\n";

  bool seenPost = false;
  QBDI::rword targetAddr = 0;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB(
      "tADR", QBDI::PREINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        const QBDI::InstAnalysis *ia =
            vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
        targetAddr = ia->address + ia->instSize;
        return QBDI::VMAction::CONTINUE;
      });
  fixture.vm.addMnemonicCB(
      "tADR", QBDI::POSTINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        CHECK(vmi->getInstMemoryAccess().empty());
        CHECK(gprState->r0 == (taken ? targetAddr : 0x11111111));
        seenPost = true;
        return QBDI::VMAction::CONTINUE;
      });

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0x11111111;
  fixture.vm.setGPRState(state);
  setZ(fixture, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seenPost);
}

static void checkItBlock(APITest &fixture, bool taken) {
  const char source[] =
      "it eq\n"
      "moveq r0, #0x2222\n";

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0x1111;
  fixture.vm.setGPRState(state);
  setZ(fixture, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-mov_pc_r_cond_taken") {
  checkMovPcRCond(*this, true);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-mov_pc_r_cond_not_taken") {
  checkMovPcRCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-add_pc_pc_r_cond_taken") {
  checkAddPcPcRCond(*this, true);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-add_pc_pc_r_cond_not_taken") {
  checkAddPcPcRCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldm_pc_cond_taken") {
  checkLdmPcCond(*this, true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldm_pc_cond_not_taken") {
  checkLdmPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tadr_cond_taken") {
  checkTadrCond(*this, true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tadr_cond_not_taken") {
  checkTadrCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-it_block_taken") {
  checkItBlock(*this, true);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-it_block_not_taken") {
  checkItBlock(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-mov_pc_r") {
  const char source[] =
      "adr r1, movpc_landed\n"
      "mov pc, r1\n"
      "mov r0, #0x1111\n"
      "b movpc_end\n"
      "movpc_landed:\n"
      "mov r0, #0x2222\n"
      "movpc_end:\n";

  bool seen = false, seenPost = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOVr", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     if (!seen) {
                       CHECK(vmi->getInstMemoryAccess().empty());
                       seen = true;
                     }
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("MOVr", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     if (!seenPost) {
                       CHECK(vmi->getInstMemoryAccess().empty());
                       seenPost = true;
                     }
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
  CHECK(seenPost);
  CHECK(retval == 0x2222);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-mov_r_pc") {
  const char source[] = "mov r1, pc\n";

  bool seenPre = false, seen = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MOVr", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seenPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("MOVr", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     CHECK(gprState->r1 == ia->address + 8);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seen);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-add_pc_pc_r") {
  const char source[] =
      "mov r0, #4\n"
      "add pc, pc, r0\n"
      "mov r0, #0x1111\n"
      "b addpc_end\n"
      "addpc_landed:\n"
      "mov r0, #0x2222\n"
      "addpc_end:\n";

  bool seen = false, seenPost = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADDrr", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("ADDrr", QBDI::POSTINST,
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldm_pc") {
  const char source[] =
      "adr r1, ldm_data\n"
      "ldmia r1, {r0, pc}\n"
      "mov r0, #0x1111\n"
      "b ldm_end\n"
      "ldm_landed:\n"
      "mov r0, #0x2222\n"
      "b ldm_end\n"
      "ldm_data:\n"
      ".word 0x12345678\n"
      ".word ldm_landed\n"
      "ldm_end:\n";

  bool seen = false, seenPost = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LDMIA", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 2);
                     CHECK(accesses[0].value == 0x12345678);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(accesses[1].size == 4);
                     CHECK(accesses[1].type == QBDI::MEMORY_READ);
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("LDMIA", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     auto accesses = vmi->getInstMemoryAccess();
                     REQUIRE(accesses.size() == 2);
                     CHECK(accesses[0].value == 0x12345678);
                     CHECK(accesses[0].size == 4);
                     CHECK(accesses[0].type == QBDI::MEMORY_READ);
                     CHECK(accesses[1].size == 4);
                     CHECK(accesses[1].type == QBDI::MEMORY_READ);
                     CHECK(accesses[1].value == gprState->pc);
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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tadr") {
  const char source[] =
      "adr r0, tadr_target\n"
      "tadr_target:\n"
      "mov r1, #0x2222\n";

  bool seenPre = false, seen = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2ADR", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seenPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("t2ADR", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seen);
}
