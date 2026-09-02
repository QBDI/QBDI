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
#include "MemAccessTestUtils_ARM.h"

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::conditionForIndex;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;
using QBDITestBatch2::setConditionCPSR;

static void checkT2Ldri8PcCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(0);

  std::string source = std::string("adr r0, t2ldri8cond_data_after\n") + "it " +
                       cond.suffix + "\n" + "ldr" + cond.suffix +
                       " pc, [r0, #-4]\n"
                       "mov r0, #0x1111\n"
                       "b t2ldri8cond_end\n"
                       "t2ldri8cond_landed:\n"
                       "mov r0, #0x2222\n"
                       "b t2ldri8cond_end\n"
                       ".p2align 2\n"
                       "t2ldri8cond_data:\n"
                       ".word t2ldri8cond_landed + 1\n"
                       "t2ldri8cond_data_after:\n"
                       "t2ldri8cond_end:\n";

  bool seenPost = false;
  QBDI::rword baseR0 = 0;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("t2LDRi8", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             baseR0 = gprState->r0;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("t2LDRi8", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             auto accesses = vmi->getInstMemoryAccess();
                             if (taken) {
                               REQUIRE(accesses.size() == 1);
                               CHECK(accesses[0].accessAddress == baseR0 - 4);
                               CHECK(accesses[0].value == gprState->pc);
                               CHECK(accesses[0].size == 4);
                               CHECK(accesses[0].type == QBDI::MEMORY_READ);
                             } else {
                               CHECK(accesses.empty());
                             }
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkT2LdrPrePcCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(1);

  std::string source = std::string("adr r0, t2ldrprecond_data\n") + "it " +
                       cond.suffix + "\n" + "ldr" + cond.suffix +
                       " pc, [r0, #4]!\n"
                       "mov r0, #0x1111\n"
                       "b t2ldrprecond_end\n"
                       "t2ldrprecond_landed:\n"
                       "mov r0, #0x2222\n"
                       "b t2ldrprecond_end\n"
                       ".p2align 2\n"
                       "t2ldrprecond_data:\n"
                       ".word 0x12345678\n"
                       ".word t2ldrprecond_landed + 1\n"
                       "t2ldrprecond_end:\n";

  bool seenPost = false;
  QBDI::rword baseR0 = 0;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("t2LDR_PRE", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             baseR0 = gprState->r0;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("t2LDR_PRE", QBDI::POSTINST,
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
                               CHECK(gprState->r0 == baseR0);
                             }
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkT2LdrPostPcCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(2);

  std::string source = std::string("adr r0, t2ldrpostcond_data\n") + "it " +
                       cond.suffix + "\n" + "ldr" + cond.suffix +
                       " pc, [r0], #4\n"
                       "mov r0, #0x1111\n"
                       "b t2ldrpostcond_end\n"
                       "t2ldrpostcond_landed:\n"
                       "mov r0, #0x2222\n"
                       "b t2ldrpostcond_end\n"
                       ".p2align 2\n"
                       "t2ldrpostcond_data:\n"
                       ".word t2ldrpostcond_landed + 1\n"
                       ".word 0x12345678\n"
                       "t2ldrpostcond_end:\n";

  bool seenPost = false;
  QBDI::rword baseR0 = 0;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("t2LDR_POST", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             baseR0 = gprState->r0;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("t2LDR_POST", QBDI::POSTINST,
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
                               CHECK(gprState->r0 == baseR0);
                             }
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkT2Ldri12PcCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(3);

  std::string source = std::string("adr r0, t2ldri12cond_data\n") + "it " +
                       cond.suffix + "\n" + "ldr" + cond.suffix +
                       " pc, [r0, #4]\n"
                       "mov r0, #0x1111\n"
                       "b t2ldri12cond_end\n"
                       "t2ldri12cond_landed:\n"
                       "mov r0, #0x2222\n"
                       "b t2ldri12cond_end\n"
                       ".p2align 2\n"
                       "t2ldri12cond_data:\n"
                       ".word 0x12345678\n"
                       ".word t2ldri12cond_landed + 1\n"
                       "t2ldri12cond_end:\n";

  bool seenPost = false;
  QBDI::rword baseR0 = 0;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("t2LDRi12", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             baseR0 = gprState->r0;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("t2LDRi12", QBDI::POSTINST,
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

  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkT2LdrsPcCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(4);

  std::string source = std::string("adr r0, t2ldrscond_data\n") +
                       "mov r1, #4\n" + "it " + cond.suffix + "\n" + "ldr" +
                       cond.suffix +
                       " pc, [r0, r1, lsl #2]\n"
                       "mov r0, #0x1111\n"
                       "b t2ldrscond_end\n"
                       "t2ldrscond_landed:\n"
                       "mov r0, #0x2222\n"
                       "b t2ldrscond_end\n"
                       ".p2align 2\n"
                       "t2ldrscond_data:\n"
                       ".word 0x11111111\n"
                       ".word 0x22222222\n"
                       ".word 0x33333333\n"
                       ".word 0x44444444\n"
                       ".word t2ldrscond_landed + 1\n"
                       "t2ldrscond_end:\n";

  bool seenPost = false;
  QBDI::rword baseR0 = 0;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("t2LDRs", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             baseR0 = gprState->r0;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("t2LDRs", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             auto accesses = vmi->getInstMemoryAccess();
                             if (taken) {
                               REQUIRE(accesses.size() == 1);
                               CHECK(accesses[0].accessAddress == baseR0 + 16);
                               CHECK(accesses[0].value == gprState->pc);
                               CHECK(accesses[0].size == 4);
                               CHECK(accesses[0].type == QBDI::MEMORY_READ);
                             } else {
                               CHECK(accesses.empty());
                             }
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkTmovrPcDestCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(5);

  std::string source = std::string("adr r1, tmovrcond_landed\n") + "it " +
                       cond.suffix + "\n" + "mov" + cond.suffix +
                       " pc, r1\n"
                       "mov r0, #0x1111\n"
                       "b tmovrcond_end\n"
                       "tmovrcond_landed:\n"
                       "mov r0, #0x2222\n"
                       "tmovrcond_end:\n";

  bool seenPre = false, seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("tMOVr", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("tMOVr", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkTmovrPcSrcCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(6);

  std::string source = std::string("it ") + cond.suffix + "\n" + "mov" +
                       cond.suffix + " r1, pc\n";

  bool seenPost = false;
  QBDI::rword expected = 0;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB(
      "tMOVr", QBDI::PREINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        const QBDI::InstAnalysis *ia =
            vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
        expected = ia->address + 4;
        return QBDI::VMAction::CONTINUE;
      });
  fixture.vm.addMnemonicCB(
      "tMOVr", QBDI::POSTINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        CHECK(vmi->getInstMemoryAccess().empty());
        CHECK(gprState->r1 == (taken ? expected : 0x11111111u));
        seenPost = true;
        return QBDI::VMAction::CONTINUE;
      });

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r1 = 0x11111111;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seenPost);
}

static void checkTaddrspPcCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(7);

  std::string source =
      std::string("mov r4, sp\n") + "adr r2, taddrspcond_landed\n" +
      "adr r3, taddrspcond_add_lbl\n" + "sub r2, r2, r3\n" +
      "sub r2, r2, #4\n" + "mov sp, r2\n" + "it " + cond.suffix + "\n" +
      "taddrspcond_add_lbl:\n" + "add" + cond.suffix +
      " pc, sp, pc\n"
      "mov sp, r4\n"
      "mov r0, #0x1111\n"
      "b taddrspcond_end\n"
      "taddrspcond_landed:\n"
      "mov sp, r4\n"
      "mov r0, #0x2222\n"
      "taddrspcond_end:\n";

  bool seenPre = false, seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("tADDrSP", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("tADDrSP", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkTaddhirrPcDestCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(8);

  std::string source = std::string("adr r2, taddhirrcond_landed\n") +
                       "adr r3, taddhirrcond_add_lbl\n" + "sub r2, r2, r3\n" +
                       "sub r1, r2, #4\n" + "it " + cond.suffix + "\n" +
                       "taddhirrcond_add_lbl:\n" + "add" + cond.suffix +
                       " pc, r1\n"
                       "mov r0, #0x1111\n"
                       "b taddhirrcond_end\n"
                       "taddhirrcond_landed:\n"
                       "mov r0, #0x2222\n"
                       "taddhirrcond_end:\n";

  bool seenPre = false, seenPost = false;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB("tADDhirr", QBDI::PREINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPre = true;
                             return QBDI::VMAction::CONTINUE;
                           });
  fixture.vm.addMnemonicCB("tADDhirr", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == (taken ? 0x2222 : 0x1111));
}

static void checkTaddhirrPcSrcCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(9);

  std::string source = std::string("mov r0, #0x1000\n") + "it " + cond.suffix +
                       "\n" + "add" + cond.suffix + " r0, pc\n";

  bool seenPost = false;
  QBDI::rword expected = 0;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB(
      "tADDhirr", QBDI::PREINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        const QBDI::InstAnalysis *ia =
            vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
        expected = 0x1000u + (ia->address + 4);
        return QBDI::VMAction::CONTINUE;
      });
  fixture.vm.addMnemonicCB(
      "tADDhirr", QBDI::POSTINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        CHECK(vmi->getInstMemoryAccess().empty());
        CHECK(gprState->r0 == (taken ? expected : 0x1000u));
        seenPost = true;
        return QBDI::VMAction::CONTINUE;
      });

  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seenPost);
}

static void checkTaddsprPcCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(10);

  std::string source = std::string("mov r4, sp\n") + "it " + cond.suffix +
                       "\n" + "add" + cond.suffix +
                       " sp, pc\n"
                       "mov sp, r4\n";

  bool seenPost = false;
  QBDI::rword origSp = 0, expected = 0;
  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  fixture.vm.addMnemonicCB(
      "tADDspr", QBDI::PREINST,
      [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
          QBDI::FPRState *fprState) -> QBDI::VMAction {
        const QBDI::InstAnalysis *ia =
            vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
        origSp = gprState->sp;
        expected = origSp + (ia->address + 4);
        return QBDI::VMAction::CONTINUE;
      });
  fixture.vm.addMnemonicCB("tADDspr", QBDI::POSTINST,
                           [&](QBDI::VMInstanceRef vmi,
                               QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState) -> QBDI::VMAction {
                             CHECK(vmi->getInstMemoryAccess().empty());
                             CHECK(gprState->sp == (taken ? expected : origSp));
                             seenPost = true;
                             return QBDI::VMAction::CONTINUE;
                           });

  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran =
      fixture.runOnASM(&retval, source.c_str(), {}, QBDI::CPUMode::Thumb);

  CHECK(ran);
  CHECK(seenPost);
}

static void checkT2Ldrdi8PcCond(APITest &fixture, bool taken) {
  const auto &cond = conditionForIndex(11);

  std::string source = std::string("nop\n") + "it " + cond.suffix + "\n" +
                       "ldrd" + cond.suffix +
                       " r0, r1, [pc, #4]\n"
                       "bx lr\n"
                       ".short 0\n"
                       ".word 0x11223344\n"
                       ".word 0x55667788\n";
  QBDI::rword codeAddr = fixture.genASM(source.c_str(), QBDI::CPUMode::Thumb);
  QBDI::rword baseAddr = codeAddr & ~(QBDI::rword)1;

  fixture.vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);

  ExpectedMemoryAccesses expectedPre{}, expectedPost{};
  if (taken) {
    expectedPre.accesses = {
        {baseAddr + 12, 0x11223344, 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
        {baseAddr + 16, 0x55667788, 4, QBDI::MEMORY_READ,
         QBDI::MEMORY_NO_FLAGS},
    };
    expectedPost.accesses = expectedPre.accesses;
  }
  fixture.vm.addMnemonicCB("t2LDRDi8", QBDI::PREINST, checkAccess,
                           &expectedPre);
  fixture.vm.addMnemonicCB("t2LDRDi8", QBDI::POSTINST, checkAccess,
                           &expectedPost);

  QBDI::GPRState *state = fixture.vm.getGPRState();
  state->r0 = 0xdead0000;
  state->r1 = 0xdead0001;
  fixture.vm.setGPRState(state);
  setConditionCPSR(fixture, cond.cond, taken);

  QBDI::rword retval;
  bool ran = fixture.vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  QBDI::GPRState *finalState = fixture.vm.getGPRState();
  CHECK(finalState->r0 == (taken ? 0x11223344u : 0xdead0000u));
  CHECK(finalState->r1 == (taken ? 0x55667788u : 0xdead0001u));
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldri8_pc_cond_taken") {
  checkT2Ldri8PcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldri8_pc_cond_not_taken") {
  checkT2Ldri8PcCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldr_pre_pc_cond_taken") {
  checkT2LdrPrePcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldr_pre_pc_cond_not_taken") {
  checkT2LdrPrePcCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldr_post_pc_cond_taken") {
  checkT2LdrPostPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldr_post_pc_cond_not_taken") {
  checkT2LdrPostPcCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldri12_pc_cond_taken") {
  checkT2Ldri12PcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldri12_pc_cond_not_taken") {
  checkT2Ldri12PcCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrs_pc_cond_taken") {
  checkT2LdrsPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrs_pc_cond_not_taken") {
  checkT2LdrsPcCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tmovr_pc_dest_cond_taken") {
  checkTmovrPcDestCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tmovr_pc_dest_cond_not_taken") {
  checkTmovrPcDestCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tmovr_pc_src_cond_taken") {
  checkTmovrPcSrcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-tmovr_pc_src_cond_not_taken") {
  checkTmovrPcSrcCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-taddrsp_pc_cond_taken") {
  checkTaddrspPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-taddrsp_pc_cond_not_taken") {
  checkTaddrspPcCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-taddhirr_pc_dest_cond_taken") {
  checkTaddhirrPcDestCond(*this, true);
}
TEST_CASE_METHOD(
    APITest, "InstructionExtendedTest_Thumb-taddhirr_pc_dest_cond_not_taken") {
  checkTaddhirrPcDestCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-taddhirr_pc_src_cond_taken") {
  checkTaddhirrPcSrcCond(*this, true);
}
TEST_CASE_METHOD(
    APITest, "InstructionExtendedTest_Thumb-taddhirr_pc_src_cond_not_taken") {
  checkTaddhirrPcSrcCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-taddspr_pc_cond_taken") {
  checkTaddsprPcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-taddspr_pc_cond_not_taken") {
  checkTaddsprPcCond(*this, false);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrdi8_pc_cond_taken") {
  checkT2Ldrdi8PcCond(*this, true);
}
TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_Thumb-t2ldrdi8_pc_cond_not_taken") {
  checkT2Ldrdi8PcCond(*this, false);
}
