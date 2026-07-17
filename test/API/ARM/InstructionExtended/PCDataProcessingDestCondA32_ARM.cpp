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
#include <string>

#include <catch2/catch_test_macros.hpp>
#include "API/APITest.h"

#include "ConditionTestUtils_ARM.h"

static bool carryForIndex(size_t idx, bool taken) {
  const auto &cond = QBDITestBatch2::conditionForIndex(idx);
  return (QBDITestBatch2::nzcvForCondition(cond.cond, taken) &
          QBDITestBatch2::APSR_C) != 0;
}

static void checkLdri12PcCond(APITest &fixture, bool taken) {
  const auto &cond = QBDITestBatch2::conditionForIndex(0);

  std::string source = std::string("adr r0, ldri12cond_data\n") + "ldr" +
                       cond.suffix +
                       " pc, [r0, #4]\n"
                       "mov r0, #0x1111\n"
                       "b ldri12cond_end\n"
                       "ldri12cond_landed:\n"
                       "mov r0, #0x2222\n"
                       "b ldri12cond_end\n"
                       "ldri12cond_data:\n"
                       ".word 0x12345678\n"
                       ".word ldri12cond_landed\n"
                       "ldri12cond_end:\n";

  bool seenPost = false;
  QBDI::rword baseR0 = 0;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("LDRi12", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             baseR0 = gprState->r0;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("LDRi12", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             auto accesses = vmi->getInstMemoryAccess();
                             if (taken) {
                               REQUIRE(accesses.size() == 1);
                               CHECK(accesses[0].accessAddress == baseR0 + 4);
                               CHECK(accesses[0].value == gprState->pc);
                               CHECK(accesses[0].size == 4);
                               CHECK(accesses[0].type == QBDI::MEMORY_READ);
                             } else {
                               CHECK(accesses.empty());
                             }
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkLdrPostImmPcCond(APITest &fixture, bool taken) {
  const auto &cond = QBDITestBatch2::conditionForIndex(1);

  std::string source = std::string("adr r0, ldrpostimmcond_data\n") + "ldr" +
                       cond.suffix +
                       " pc, [r0], #4\n"
                       "mov r0, #0x1111\n"
                       "b ldrpostimmcond_end\n"
                       "ldrpostimmcond_landed:\n"
                       "mov r0, #0x2222\n"
                       "b ldrpostimmcond_end\n"
                       "ldrpostimmcond_data:\n"
                       ".word ldrpostimmcond_landed\n"
                       ".word 0x12345678\n"
                       "ldrpostimmcond_end:\n";

  bool seenPost = false;
  QBDI::rword baseR0 = 0;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("LDR_POST_IMM", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             baseR0 = gprState->r0;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("LDR_POST_IMM", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             auto accesses = vmi->getInstMemoryAccess();
                             if (taken) {
                               REQUIRE(accesses.size() == 1);
                               CHECK(accesses[0].accessAddress == baseR0);
                               CHECK(accesses[0].value == gprState->pc);
                               CHECK(accesses[0].size == 4);
                               CHECK(accesses[0].type == QBDI::MEMORY_READ);
                               CHECK(gprState->r0 == baseR0 + 4);
                             } else {
                               CHECK(accesses.empty());
                             }
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkLdrPreImmPcCond(APITest &fixture, bool taken) {
  const auto &cond = QBDITestBatch2::conditionForIndex(2);

  std::string source = std::string("adr r0, ldrpreimmcond_data\n") + "ldr" +
                       cond.suffix +
                       " pc, [r0, #4]!\n"
                       "mov r0, #0x1111\n"
                       "b ldrpreimmcond_end\n"
                       "ldrpreimmcond_landed:\n"
                       "mov r0, #0x2222\n"
                       "b ldrpreimmcond_end\n"
                       "ldrpreimmcond_data:\n"
                       ".word 0x12345678\n"
                       ".word ldrpreimmcond_landed\n"
                       "ldrpreimmcond_end:\n";

  bool seenPost = false;
  QBDI::rword baseR0 = 0;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("LDR_PRE_IMM", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             baseR0 = gprState->r0;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("LDR_PRE_IMM", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             auto accesses = vmi->getInstMemoryAccess();
                             if (taken) {
                               REQUIRE(accesses.size() == 1);
                               CHECK(accesses[0].accessAddress == baseR0 + 4);
                               CHECK(accesses[0].value == gprState->pc);
                               CHECK(accesses[0].size == 4);
                               CHECK(accesses[0].type == QBDI::MEMORY_READ);
                               CHECK(gprState->r0 == baseR0 + 4);
                             } else {
                               CHECK(accesses.empty());
                             }
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkLdrrsPcCond(APITest &fixture, bool taken) {
  const auto &cond = QBDITestBatch2::conditionForIndex(3);

  std::string source = std::string("adr r0, ldrrscond_data\n") +
                       "mov r1, #4\n" + "ldr" + cond.suffix +
                       " pc, [r0, r1]\n"
                       "mov r0, #0x1111\n"
                       "b ldrrscond_end\n"
                       "ldrrscond_landed:\n"
                       "mov r0, #0x2222\n"
                       "b ldrrscond_end\n"
                       "ldrrscond_data:\n"
                       ".word 0x12345678\n"
                       ".word ldrrscond_landed\n"
                       "ldrrscond_end:\n";

  bool seenPost = false;
  QBDI::rword baseR0 = 0;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("LDRrs", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             baseR0 = gprState->r0;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("LDRrs", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             auto accesses = vmi->getInstMemoryAccess();
                             if (taken) {
                               REQUIRE(accesses.size() == 1);
                               CHECK(accesses[0].accessAddress == baseR0 + 4);
                               CHECK(accesses[0].value == gprState->pc);
                               CHECK(accesses[0].size == 4);
                               CHECK(accesses[0].type == QBDI::MEMORY_READ);
                             } else {
                               CHECK(accesses.empty());
                             }
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkMvniPcCond(APITest &fixture, bool taken) {
  const auto &cond = QBDITestBatch2::conditionForIndex(4);
  constexpr QBDI::rword expectedPc = (QBDI::rword) ~(QBDI::rword)0x1000;

  std::string source = std::string("mvn") + cond.suffix +
                       " pc, #0x1000\n"
                       "mov r0, #0x1111\n"
                       "b mvnicond_end\n"
                       "mvnicond_landed:\n"
                       "mov r0, #0x2222\n"
                       "mvnicond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("MVNi", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB(
      "MVNi", QBDI::POSTINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        CHECK(vmi->getInstMemoryAccess().empty());
        const QBDI::InstAnalysis *ia =
            vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
        if (taken) {
          CHECK(gprState->pc == expectedPc);
          gprState->pc = ia->address + 12;
          fixture.vm.setGPRState(gprState);
          seenPost = true;
          return QBDI::VMAction::BREAK_TO_VM;
        }
        CHECK(gprState->pc == ia->address + ia->instSize);
        seenPost = true;
        return QBDI::VMAction::CONTINUE;
      });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkMoviPcCond(APITest &fixture, bool taken) {
  const auto &cond = QBDITestBatch2::conditionForIndex(36);
  constexpr QBDI::rword expectedPc = 0x1000;

  std::string source = std::string("mov") + cond.suffix +
                       " pc, #0x1000\n"
                       "eor r0, r0, r0\n"
                       "orr r0, r0, #0x1100\n"
                       "orr r0, r0, #0x11\n"
                       "b movicond_end\n"
                       "movicond_landed:\n"
                       "eor r0, r0, r0\n"
                       "orr r0, r0, #0x2200\n"
                       "orr r0, r0, #0x22\n"
                       "movicond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("MOVi", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB(
      "MOVi", QBDI::POSTINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        CHECK(vmi->getInstMemoryAccess().empty());
        const QBDI::InstAnalysis *ia =
            vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
        if (taken) {
          CHECK(gprState->pc == expectedPc);
          gprState->pc = ia->address + 20;
          fixture.vm.setGPRState(gprState);
          seenPost = true;
          return QBDI::VMAction::BREAK_TO_VM;
        }
        CHECK(gprState->pc == ia->address + ia->instSize);
        seenPost = true;
        return QBDI::VMAction::CONTINUE;
      });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkMovi16PcCond(APITest &fixture, bool taken) {
  const auto &cond = QBDITestBatch2::conditionForIndex(37);
  constexpr QBDI::rword expectedPc = 0x1234;

  std::string source = std::string("movw") + cond.suffix +
                       " pc, #0x1234\n"
                       "eor r0, r0, r0\n"
                       "orr r0, r0, #0x1100\n"
                       "orr r0, r0, #0x11\n"
                       "b movi16cond_end\n"
                       "movi16cond_landed:\n"
                       "eor r0, r0, r0\n"
                       "orr r0, r0, #0x2200\n"
                       "orr r0, r0, #0x22\n"
                       "movi16cond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("MOVi16", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB(
      "MOVi16", QBDI::POSTINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        CHECK(vmi->getInstMemoryAccess().empty());
        const QBDI::InstAnalysis *ia =
            vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
        if (taken) {
          CHECK(gprState->pc == expectedPc);
          gprState->pc = ia->address + 20;
          fixture.vm.setGPRState(gprState);
          seenPost = true;
          return QBDI::VMAction::BREAK_TO_VM;
        }
        CHECK(gprState->pc == ia->address + ia->instSize);
        seenPost = true;
        return QBDI::VMAction::CONTINUE;
      });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkAdcriPcCond(APITest &fixture, bool taken) {
  bool carry = carryForIndex(5, taken);
  const auto &cond = QBDITestBatch2::conditionForIndex(5);

  std::string source = std::string("adr r1, adcricond_landed\n") +
                       (carry ? "sub r1, r1, #1\n" : "") + "adc" + cond.suffix +
                       " pc, r1, #0\n"
                       "mov r0, #0x1111\n"
                       "b adcricond_end\n"
                       "adcricond_landed:\n"
                       "mov r0, #0x2222\n"
                       "adcricond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("ADCri", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("ADCri", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkAddriPcCond(APITest &fixture, bool taken) {
  const auto &cond = QBDITestBatch2::conditionForIndex(6);

  std::string source = std::string("adr r1, addricond_landed\n") + "add" +
                       cond.suffix +
                       " pc, r1, #0\n"
                       "mov r0, #0x1111\n"
                       "b addricond_end\n"
                       "addricond_landed:\n"
                       "mov r0, #0x2222\n"
                       "addricond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("ADDri", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("ADDri", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkAndriPcCond(APITest &fixture, bool taken) {
  const auto &cond = QBDITestBatch2::conditionForIndex(7);
  constexpr QBDI::rword R1 = 0x3000;
  constexpr QBDI::rword expectedPc = R1 & 0xffu;

  std::string source = std::string("and") + cond.suffix +
                       " pc, r1, #0xff\n"
                       "mov r0, #0x1111\n"
                       "b andricond_end\n"
                       "andricond_landed:\n"
                       "mov r0, #0x2222\n"
                       "andricond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("ANDri", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB(
      "ANDri", QBDI::POSTINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        CHECK(vmi->getInstMemoryAccess().empty());
        const QBDI::InstAnalysis *ia =
            vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
        if (taken) {
          CHECK(gprState->pc == expectedPc);
          gprState->pc = ia->address + 12;
          fixture.vm.setGPRState(gprState);
          seenPost = true;
          return QBDI::VMAction::BREAK_TO_VM;
        }
        CHECK(gprState->pc == ia->address + ia->instSize);
        seenPost = true;
        return QBDI::VMAction::CONTINUE;
      });

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r1 = R1;
  fixture.vm.setGPRState(state);
  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkBicriPcCond(APITest &fixture, bool taken) {
  const auto &cond = QBDITestBatch2::conditionForIndex(8);

  std::string source = std::string("adr r1, bicricond_landed\n") + "bic" +
                       cond.suffix +
                       " pc, r1, #0\n"
                       "mov r0, #0x1111\n"
                       "b bicricond_end\n"
                       "bicricond_landed:\n"
                       "mov r0, #0x2222\n"
                       "bicricond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("BICri", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("BICri", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkEorriPcCond(APITest &fixture, bool taken) {
  const auto &cond = QBDITestBatch2::conditionForIndex(9);

  std::string source = std::string("adr r1, eorricond_landed\n") + "eor" +
                       cond.suffix +
                       " pc, r1, #0\n"
                       "mov r0, #0x1111\n"
                       "b eorricond_end\n"
                       "eorricond_landed:\n"
                       "mov r0, #0x2222\n"
                       "eorricond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("EORri", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("EORri", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkMovsiPcCond(APITest &fixture, bool taken) {
  const auto &cond = QBDITestBatch2::conditionForIndex(10);

  std::string source = std::string("adr r1, movsicond_landed\n") +
                       "lsr r1, r1, #2\n" + "mov" + cond.suffix +
                       " pc, r1, lsl #2\n"
                       "mov r0, #0x1111\n"
                       "b movsicond_end\n"
                       "movsicond_landed:\n"
                       "mov r0, #0x2222\n"
                       "movsicond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("MOVsi", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("MOVsi", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkMvnrPcCond(APITest &fixture, bool taken) {
  const auto &cond = QBDITestBatch2::conditionForIndex(11);

  std::string source = std::string("adr r1, mvnrcond_landed\n") +
                       "mvn r1, r1\n" + "mvn" + cond.suffix +
                       " pc, r1\n"
                       "mov r0, #0x1111\n"
                       "b mvnrcond_end\n"
                       "mvnrcond_landed:\n"
                       "mov r0, #0x2222\n"
                       "mvnrcond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("MVNr", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("MVNr", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkOrriPcCond(APITest &fixture, bool taken) {
  const auto &cond = QBDITestBatch2::conditionForIndex(12);

  std::string source = std::string("adr r1, orrricond_landed\n") + "orr" +
                       cond.suffix +
                       " pc, r1, #0\n"
                       "mov r0, #0x1111\n"
                       "b orrricond_end\n"
                       "orrricond_landed:\n"
                       "mov r0, #0x2222\n"
                       "orrricond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("ORRri", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("ORRri", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkRsbriPcCond(APITest &fixture, bool taken) {
  const auto &cond = QBDITestBatch2::conditionForIndex(13);

  std::string source = std::string("adr r1, rsbricond_landed\n") +
                       "rsb r1, r1, #0\n" + "rsb" + cond.suffix +
                       " pc, r1, #0\n"
                       "mov r0, #0x1111\n"
                       "b rsbricond_end\n"
                       "rsbricond_landed:\n"
                       "mov r0, #0x2222\n"
                       "rsbricond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("RSBri", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("RSBri", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkRscriPcCond(APITest &fixture, bool taken) {
  bool carry = carryForIndex(14, taken);
  const auto &cond = QBDITestBatch2::conditionForIndex(14);

  std::string source = std::string("adr r1, rscricond_landed\n") +
                       "rsb r1, r1, #0\n" + (carry ? "" : "sub r1, r1, #1\n") +
                       "rsc" + cond.suffix +
                       " pc, r1, #0\n"
                       "mov r0, #0x1111\n"
                       "b rscricond_end\n"
                       "rscricond_landed:\n"
                       "mov r0, #0x2222\n"
                       "rscricond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("RSCri", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("RSCri", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkSbcriPcCond(APITest &fixture, bool taken) {
  bool carry = carryForIndex(15, taken);
  const auto &cond = QBDITestBatch2::conditionForIndex(15);

  std::string source = std::string("adr r1, sbcricond_landed\n") +
                       (carry ? "" : "add r1, r1, #1\n") + "sbc" + cond.suffix +
                       " pc, r1, #0\n"
                       "mov r0, #0x1111\n"
                       "b sbcricond_end\n"
                       "sbcricond_landed:\n"
                       "mov r0, #0x2222\n"
                       "sbcricond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("SBCri", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("SBCri", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkSubriPcCond(APITest &fixture, bool taken) {
  const auto &cond = QBDITestBatch2::conditionForIndex(16);

  std::string source = std::string("adr r1, subricond_landed\n") + "sub" +
                       cond.suffix +
                       " pc, r1, #0\n"
                       "mov r0, #0x1111\n"
                       "b subricond_end\n"
                       "subricond_landed:\n"
                       "mov r0, #0x2222\n"
                       "subricond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("SUBri", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("SUBri", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkAdcrrPcCond(APITest &fixture, bool taken) {
  bool carry = carryForIndex(17, taken);
  const auto &cond = QBDITestBatch2::conditionForIndex(17);

  std::string source = std::string("adr r1, adcrrcond_landed\n") +
                       (carry ? "sub r1, r1, #1\n" : "") + "mov r2, #0\n" +
                       "adc" + cond.suffix +
                       " pc, r1, r2\n"
                       "mov r0, #0x1111\n"
                       "b adcrrcond_end\n"
                       "adcrrcond_landed:\n"
                       "mov r0, #0x2222\n"
                       "adcrrcond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("ADCrr", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("ADCrr", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkAdcrsiPcCond(APITest &fixture, bool taken) {
  bool carry = carryForIndex(18, taken);
  const auto &cond = QBDITestBatch2::conditionForIndex(18);

  std::string source = std::string("adr r1, adcrsicond_landed\n") +
                       (carry ? "sub r1, r1, #1\n" : "") + "mov r2, #0\n" +
                       "adc" + cond.suffix +
                       " pc, r1, r2, lsl #2\n"
                       "mov r0, #0x1111\n"
                       "b adcrsicond_end\n"
                       "adcrsicond_landed:\n"
                       "mov r0, #0x2222\n"
                       "adcrsicond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("ADCrsi", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("ADCrsi", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkAddrsiPcCond(APITest &fixture, bool taken) {
  const auto &cond = QBDITestBatch2::conditionForIndex(19);

  std::string source = std::string("adr r1, addrsicond_landed\n") +
                       "mov r2, #0\n" + "add" + cond.suffix +
                       " pc, r1, r2, lsl #2\n"
                       "mov r0, #0x1111\n"
                       "b addrsicond_end\n"
                       "addrsicond_landed:\n"
                       "mov r0, #0x2222\n"
                       "addrsicond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("ADDrsi", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("ADDrsi", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkAndrrPcCond(APITest &fixture, bool taken) {
  const auto &cond = QBDITestBatch2::conditionForIndex(20);

  std::string source = std::string("adr r2, andrrcond_landed\n") +
                       "mvn r1, #0\n" + "and" + cond.suffix +
                       " pc, r1, r2\n"
                       "mov r0, #0x1111\n"
                       "b andrrcond_end\n"
                       "andrrcond_landed:\n"
                       "mov r0, #0x2222\n"
                       "andrrcond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("ANDrr", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("ANDrr", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkAndrsiPcCond(APITest &fixture, bool taken) {
  const auto &cond = QBDITestBatch2::conditionForIndex(21);

  std::string source = std::string("adr r2, andrsicond_landed\n") +
                       "lsr r2, r2, #2\n" + "mvn r1, #0\n" + "and" +
                       cond.suffix +
                       " pc, r1, r2, lsl #2\n"
                       "mov r0, #0x1111\n"
                       "b andrsicond_end\n"
                       "andrsicond_landed:\n"
                       "mov r0, #0x2222\n"
                       "andrsicond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("ANDrsi", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("ANDrsi", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkBicrrPcCond(APITest &fixture, bool taken) {
  const auto &cond = QBDITestBatch2::conditionForIndex(22);

  std::string source = std::string("adr r1, bicrrcond_landed\n") +
                       "mov r2, #0\n" + "bic" + cond.suffix +
                       " pc, r1, r2\n"
                       "mov r0, #0x1111\n"
                       "b bicrrcond_end\n"
                       "bicrrcond_landed:\n"
                       "mov r0, #0x2222\n"
                       "bicrrcond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("BICrr", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("BICrr", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkBicrsiPcCond(APITest &fixture, bool taken) {
  const auto &cond = QBDITestBatch2::conditionForIndex(23);

  std::string source = std::string("adr r1, bicrsicond_landed\n") +
                       "mov r2, #0\n" + "bic" + cond.suffix +
                       " pc, r1, r2, lsl #2\n"
                       "mov r0, #0x1111\n"
                       "b bicrsicond_end\n"
                       "bicrsicond_landed:\n"
                       "mov r0, #0x2222\n"
                       "bicrsicond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("BICrsi", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("BICrsi", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkEorrrPcCond(APITest &fixture, bool taken) {
  const auto &cond = QBDITestBatch2::conditionForIndex(24);

  std::string source = std::string("adr r1, eorrrcond_landed\n") +
                       "mov r2, #0\n" + "eor" + cond.suffix +
                       " pc, r1, r2\n"
                       "mov r0, #0x1111\n"
                       "b eorrrcond_end\n"
                       "eorrrcond_landed:\n"
                       "mov r0, #0x2222\n"
                       "eorrrcond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("EORrr", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("EORrr", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkEorrsiPcCond(APITest &fixture, bool taken) {
  const auto &cond = QBDITestBatch2::conditionForIndex(25);

  std::string source = std::string("adr r1, eorrsicond_landed\n") +
                       "mov r2, #0\n" + "eor" + cond.suffix +
                       " pc, r1, r2, lsl #2\n"
                       "mov r0, #0x1111\n"
                       "b eorrsicond_end\n"
                       "eorrsicond_landed:\n"
                       "mov r0, #0x2222\n"
                       "eorrsicond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("EORrsi", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("EORrsi", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkOrrrrPcCond(APITest &fixture, bool taken) {
  const auto &cond = QBDITestBatch2::conditionForIndex(26);

  std::string source = std::string("adr r1, orrrrcond_landed\n") +
                       "mov r2, #0\n" + "orr" + cond.suffix +
                       " pc, r1, r2\n"
                       "mov r0, #0x1111\n"
                       "b orrrrcond_end\n"
                       "orrrrcond_landed:\n"
                       "mov r0, #0x2222\n"
                       "orrrrcond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("ORRrr", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("ORRrr", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkOrrrsiPcCond(APITest &fixture, bool taken) {
  const auto &cond = QBDITestBatch2::conditionForIndex(27);

  std::string source = std::string("adr r1, orrrsicond_landed\n") +
                       "mov r2, #0\n" + "orr" + cond.suffix +
                       " pc, r1, r2, lsl #2\n"
                       "mov r0, #0x1111\n"
                       "b orrrsicond_end\n"
                       "orrrsicond_landed:\n"
                       "mov r0, #0x2222\n"
                       "orrrsicond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("ORRrsi", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("ORRrsi", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkRsbrrPcCond(APITest &fixture, bool taken) {
  const auto &cond = QBDITestBatch2::conditionForIndex(28);

  std::string source = std::string("adr r2, rsbrrcond_landed\n") +
                       "mov r1, #0\n" + "rsb" + cond.suffix +
                       " pc, r1, r2\n"
                       "mov r0, #0x1111\n"
                       "b rsbrrcond_end\n"
                       "rsbrrcond_landed:\n"
                       "mov r0, #0x2222\n"
                       "rsbrrcond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("RSBrr", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("RSBrr", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkRsbrsiPcCond(APITest &fixture, bool taken) {
  const auto &cond = QBDITestBatch2::conditionForIndex(29);

  std::string source = std::string("adr r2, rsbrsicond_landed\n") +
                       "lsr r2, r2, #2\n" + "mov r1, #0\n" + "rsb" +
                       cond.suffix +
                       " pc, r1, r2, lsl #2\n"
                       "mov r0, #0x1111\n"
                       "b rsbrsicond_end\n"
                       "rsbrsicond_landed:\n"
                       "mov r0, #0x2222\n"
                       "rsbrsicond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("RSBrsi", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("RSBrsi", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkRscrrPcCond(APITest &fixture, bool taken) {
  bool carry = carryForIndex(30, taken);
  const auto &cond = QBDITestBatch2::conditionForIndex(30);

  std::string source = std::string("adr r2, rscrrcond_landed\n") +
                       "mov r1, #0\n" + (carry ? "" : "sub r1, r1, #1\n") +
                       "rsc" + cond.suffix +
                       " pc, r1, r2\n"
                       "mov r0, #0x1111\n"
                       "b rscrrcond_end\n"
                       "rscrrcond_landed:\n"
                       "mov r0, #0x2222\n"
                       "rscrrcond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("RSCrr", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("RSCrr", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkRscrsiPcCond(APITest &fixture, bool taken) {
  bool carry = carryForIndex(31, taken);
  const auto &cond = QBDITestBatch2::conditionForIndex(31);

  std::string source = std::string("adr r2, rscrsicond_landed\n") +
                       "lsr r2, r2, #2\n" + "mov r1, #0\n" +
                       (carry ? "" : "sub r1, r1, #1\n") + "rsc" + cond.suffix +
                       " pc, r1, r2, lsl #2\n"
                       "mov r0, #0x1111\n"
                       "b rscrsicond_end\n"
                       "rscrsicond_landed:\n"
                       "mov r0, #0x2222\n"
                       "rscrsicond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("RSCrsi", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("RSCrsi", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkSbcrrPcCond(APITest &fixture, bool taken) {
  bool carry = carryForIndex(32, taken);
  const auto &cond = QBDITestBatch2::conditionForIndex(32);

  std::string source = std::string("adr r1, sbcrrcond_landed\n") +
                       (carry ? "" : "add r1, r1, #1\n") + "mov r2, #0\n" +
                       "sbc" + cond.suffix +
                       " pc, r1, r2\n"
                       "mov r0, #0x1111\n"
                       "b sbcrrcond_end\n"
                       "sbcrrcond_landed:\n"
                       "mov r0, #0x2222\n"
                       "sbcrrcond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("SBCrr", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("SBCrr", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkSbcrsiPcCond(APITest &fixture, bool taken) {
  bool carry = carryForIndex(33, taken);
  const auto &cond = QBDITestBatch2::conditionForIndex(33);

  std::string source = std::string("adr r1, sbcrsicond_landed\n") +
                       (carry ? "" : "add r1, r1, #1\n") + "mov r2, #0\n" +
                       "sbc" + cond.suffix +
                       " pc, r1, r2, lsl #2\n"
                       "mov r0, #0x1111\n"
                       "b sbcrsicond_end\n"
                       "sbcrsicond_landed:\n"
                       "mov r0, #0x2222\n"
                       "sbcrsicond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("SBCrsi", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("SBCrsi", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkSubrrPcCond(APITest &fixture, bool taken) {
  const auto &cond = QBDITestBatch2::conditionForIndex(34);

  std::string source = std::string("adr r1, subrrcond_landed\n") +
                       "mov r2, #0\n" + "sub" + cond.suffix +
                       " pc, r1, r2\n"
                       "mov r0, #0x1111\n"
                       "b subrrcond_end\n"
                       "subrrcond_landed:\n"
                       "mov r0, #0x2222\n"
                       "subrrcond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("SUBrr", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("SUBrr", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkSubrsiPcCond(APITest &fixture, bool taken) {
  const auto &cond = QBDITestBatch2::conditionForIndex(35);

  std::string source = std::string("adr r1, subrsicond_landed\n") +
                       "mov r2, #0\n" + "sub" + cond.suffix +
                       " pc, r1, r2, lsl #2\n"
                       "mov r0, #0x1111\n"
                       "b subrsicond_end\n"
                       "subrsicond_landed:\n"
                       "mov r0, #0x2222\n"
                       "subrsicond_end:\n";

  bool seenPre = false;
  bool seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("SUBrsi", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("SUBrsi", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  QBDITestBatch2::setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.runOnASM(&retval, source.c_str());

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldri12_pc_cond_taken") {
  checkLdri12PcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldri12_pc_cond_not_taken") {
  checkLdri12PcCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldr_post_imm_pc_cond_taken") {
  checkLdrPostImmPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldr_post_imm_pc_cond_not_taken") {
  checkLdrPostImmPcCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldr_pre_imm_pc_cond_taken") {
  checkLdrPreImmPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldr_pre_imm_pc_cond_not_taken") {
  checkLdrPreImmPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-ldrrs_pc_cond_taken") {
  checkLdrrsPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-ldrrs_pc_cond_not_taken") {
  checkLdrrsPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-mvni_pc_cond_taken") {
  checkMvniPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-mvni_pc_cond_not_taken") {
  checkMvniPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-movi_pc_cond_taken") {
  checkMoviPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-movi_pc_cond_not_taken") {
  checkMoviPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-movi16_pc_cond_taken") {
  checkMovi16PcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-movi16_pc_cond_not_taken") {
  checkMovi16PcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-adcri_pc_cond_taken") {
  checkAdcriPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-adcri_pc_cond_not_taken") {
  checkAdcriPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-addri_pc_cond_taken") {
  checkAddriPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-addri_pc_cond_not_taken") {
  checkAddriPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-andri_pc_cond_taken") {
  checkAndriPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-andri_pc_cond_not_taken") {
  checkAndriPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-bicri_pc_cond_taken") {
  checkBicriPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-bicri_pc_cond_not_taken") {
  checkBicriPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-eorri_pc_cond_taken") {
  checkEorriPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-eorri_pc_cond_not_taken") {
  checkEorriPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-movsi_pc_cond_taken") {
  checkMovsiPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-movsi_pc_cond_not_taken") {
  checkMovsiPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-mvnr_pc_cond_taken") {
  checkMvnrPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-mvnr_pc_cond_not_taken") {
  checkMvnrPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-orrri_pc_cond_taken") {
  checkOrriPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-orrri_pc_cond_not_taken") {
  checkOrriPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-rsbri_pc_cond_taken") {
  checkRsbriPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-rsbri_pc_cond_not_taken") {
  checkRsbriPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-rscri_pc_cond_taken") {
  checkRscriPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-rscri_pc_cond_not_taken") {
  checkRscriPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-sbcri_pc_cond_taken") {
  checkSbcriPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-sbcri_pc_cond_not_taken") {
  checkSbcriPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-subri_pc_cond_taken") {
  checkSubriPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-subri_pc_cond_not_taken") {
  checkSubriPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-adcrr_pc_cond_taken") {
  checkAdcrrPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-adcrr_pc_cond_not_taken") {
  checkAdcrrPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-adcrsi_pc_cond_taken") {
  checkAdcrsiPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-adcrsi_pc_cond_not_taken") {
  checkAdcrsiPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-addrsi_pc_cond_taken") {
  checkAddrsiPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-addrsi_pc_cond_not_taken") {
  checkAddrsiPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-andrr_pc_cond_taken") {
  checkAndrrPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-andrr_pc_cond_not_taken") {
  checkAndrrPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-andrsi_pc_cond_taken") {
  checkAndrsiPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-andrsi_pc_cond_not_taken") {
  checkAndrsiPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-bicrr_pc_cond_taken") {
  checkBicrrPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-bicrr_pc_cond_not_taken") {
  checkBicrrPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-bicrsi_pc_cond_taken") {
  checkBicrsiPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-bicrsi_pc_cond_not_taken") {
  checkBicrsiPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-eorrr_pc_cond_taken") {
  checkEorrrPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-eorrr_pc_cond_not_taken") {
  checkEorrrPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-eorrsi_pc_cond_taken") {
  checkEorrsiPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-eorrsi_pc_cond_not_taken") {
  checkEorrsiPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-orrrr_pc_cond_taken") {
  checkOrrrrPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-orrrr_pc_cond_not_taken") {
  checkOrrrrPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-orrrsi_pc_cond_taken") {
  checkOrrrsiPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-orrrsi_pc_cond_not_taken") {
  checkOrrrsiPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-rsbrr_pc_cond_taken") {
  checkRsbrrPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-rsbrr_pc_cond_not_taken") {
  checkRsbrrPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-rsbrsi_pc_cond_taken") {
  checkRsbrsiPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-rsbrsi_pc_cond_not_taken") {
  checkRsbrsiPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-rscrr_pc_cond_taken") {
  checkRscrrPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-rscrr_pc_cond_not_taken") {
  checkRscrrPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-rscrsi_pc_cond_taken") {
  checkRscrsiPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-rscrsi_pc_cond_not_taken") {
  checkRscrsiPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-sbcrr_pc_cond_taken") {
  checkSbcrrPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-sbcrr_pc_cond_not_taken") {
  checkSbcrrPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-sbcrsi_pc_cond_taken") {
  checkSbcrsiPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-sbcrsi_pc_cond_not_taken") {
  checkSbcrsiPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-subrr_pc_cond_taken") {
  checkSubrrPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-subrr_pc_cond_not_taken") {
  checkSubrrPcCond(*this, false);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-subrsi_pc_cond_taken") {
  checkSubrsiPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-subrsi_pc_cond_not_taken") {
  checkSubrsiPcCond(*this, false);
}
