/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2024 Quarkslab
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
#include "Patch/ARM/Instr_Test_Thumb.h"
#include <catch2/catch_test_macros.hpp>

QBDI::VMAction incrementThumbPre(QBDI::VMInstanceRef vm,
                                 QBDI::GPRState *gprState,
                                 QBDI::FPRState *fprState, void *data) {
  *((uint64_t *)data) = *((uint64_t *)data) + 1;

  // verify instAnalysis
  const QBDI::InstAnalysis *ana =
      vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
  CHECK(((gprState->pc & 1) == 1) == (ana->cpuMode == QBDI::CPUMode::Thumb));
  return QBDI::VMAction::CONTINUE;
}

QBDI::VMAction incrementThumbPost(QBDI::VMInstanceRef vm,
                                  QBDI::GPRState *gprState,
                                  QBDI::FPRState *fprState, void *data) {
  *((uint64_t *)data) = *((uint64_t *)data) + 1;

  // verify instAnalysis
  const QBDI::InstAnalysis *ana =
      vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
  if (not ana->affectControlFlow) {
    CHECK(((gprState->pc & 1) == 1) == (ana->cpuMode == QBDI::CPUMode::Thumb));
  }

  return QBDI::VMAction::CONTINUE;
}

TEST_CASE_METHOD(Instr_Test_Thumb, "Instr_Test-tGPRSave") {
  INFO("TEST_SEED=" << seed_random());
  uint64_t count1 = 0;
  uint64_t count2 = 0;

  QBDI::Context inputState;
  initContext(inputState);

  vm.deleteAllInstrumentations();
  vm.addCodeCB(QBDI::PREINST, incrementThumbPre, (void *)&count1);
  vm.addCodeCB(QBDI::POSTINST, incrementThumbPost, (void *)&count2);

  comparedExec(tGPRSave_s, inputState, 4096);

  REQUIRE((uint64_t)0 < count1);
  REQUIRE(count1 == count2);

  INFO("Took " << count1 << " instructions\n");
}

TEST_CASE_METHOD(Instr_Test_Thumb, "Instr_Test-tGPRShuffle") {
  INFO("TEST_SEED=" << seed_random());
  uint64_t count1 = 0;
  uint64_t count2 = 0;

  QBDI::Context inputState;
  initContext(inputState);

  vm.deleteAllInstrumentations();
  vm.addCodeCB(QBDI::PREINST, incrementThumbPre, (void *)&count1);
  vm.addCodeCB(QBDI::POSTINST, incrementThumbPost, (void *)&count2);

  comparedExec(tGPRShuffle_s, inputState, 4096);

  REQUIRE((uint64_t)0 < count1);
  REQUIRE(count1 == count2);

  INFO("Took " << count1 << " instructions\n");
}

TEST_CASE_METHOD(Instr_Test_Thumb, "Instr_Test-tFibonacciRecursion") {
  INFO("TEST_SEED=" << seed_random());
  uint64_t count1 = 0;
  uint64_t count2 = 0;

  QBDI::Context inputState;
  initContext(inputState);
  inputState.gprState.r0 = (rand() % 20) + 2;

  vm.deleteAllInstrumentations();
  vm.addCodeCB(QBDI::PREINST, incrementThumbPre, (void *)&count1);
  vm.addCodeCB(QBDI::POSTINST, incrementThumbPost, (void *)&count2);

  comparedExec(tFibonacciRecursion_s, inputState, 4096);

  REQUIRE((uint64_t)0 < count1);
  REQUIRE(count1 == count2);

  INFO("Took " << count1 << " instructions\n");
}

TEST_CASE_METHOD(Instr_Test_Thumb, "Instr_Test-tRelativeAddressing") {
  INFO("TEST_SEED=" << seed_random());
  uint64_t count1 = 0;
  uint64_t count2 = 0;

  QBDI::Context inputState;
  initContext(inputState);

  vm.deleteAllInstrumentations();
  vm.addCodeCB(QBDI::PREINST, incrementThumbPre, (void *)&count1);
  vm.addCodeCB(QBDI::POSTINST, incrementThumbPost, (void *)&count2);

  comparedExec(tRelativeAddressing_s, inputState, 4096);

  REQUIRE((uint64_t)0 < count1);
  REQUIRE(count1 == count2);

  INFO("Took " << count1 << " instructions\n");
}

TEST_CASE_METHOD(Instr_Test_Thumb, "Instr_Test-tBranchCondTest") {
  INFO("TEST_SEED=" << seed_random());
  uint64_t count1 = 0;
  uint64_t count2 = 0;

  QBDI::Context inputState;
  initContext(inputState);

  vm.deleteAllInstrumentations();
  vm.addCodeCB(QBDI::PREINST, incrementThumbPre, (void *)&count1);
  vm.addCodeCB(QBDI::POSTINST, incrementThumbPost, (void *)&count2);

  comparedExec(tBranchCondTest_s, inputState, 4096);

  REQUIRE((uint64_t)0 < count1);
  REQUIRE(count1 == count2);

  INFO("Took " << count1 << " instructions\n");
}

TEST_CASE_METHOD(Instr_Test_Thumb, "Instr_Test-tBranchLinkCondTest") {
  INFO("TEST_SEED=" << seed_random());
  uint64_t count1 = 0;
  uint64_t count2 = 0;

  QBDI::Context inputState;
  initContext(inputState);

  vm.deleteAllInstrumentations();
  vm.addCodeCB(QBDI::PREINST, incrementThumbPre, (void *)&count1);
  vm.addCodeCB(QBDI::POSTINST, incrementThumbPost, (void *)&count2);

  comparedExec(tBranchLinkCondTest_s, inputState, 4096);

  REQUIRE((uint64_t)0 < count1);
  REQUIRE(count1 == count2);

  INFO("Took " << count1 << " instructions\n");
}

TEST_CASE_METHOD(Instr_Test_Thumb, "Instr_Test-tBranchRegisterCondTest") {
  INFO("TEST_SEED=" << seed_random());
  uint64_t count1 = 0;
  uint64_t count2 = 0;

  QBDI::Context inputState;
  initContext(inputState);

  vm.deleteAllInstrumentations();
  vm.addCodeCB(QBDI::PREINST, incrementThumbPre, (void *)&count1);
  vm.addCodeCB(QBDI::POSTINST, incrementThumbPost, (void *)&count2);

  comparedExec(tBranchRegisterCondTest_s, inputState, 4096);

  REQUIRE((uint64_t)0 < count1);
  REQUIRE(count1 == count2);

  CHECK(inputState.gprState.r10 == 0);
  CHECK(inputState.gprState.r11 == 0);
  CHECK(inputState.gprState.r12 == 0);

  INFO("Took " << count1 << " instructions\n");
}

TEST_CASE_METHOD(Instr_Test_Thumb, "Instr_Test-tPushPopTest") {
  INFO("TEST_SEED=" << seed_random());
  uint64_t count1 = 0;
  uint64_t count2 = 0;

  QBDI::Context inputState;
  initContext(inputState);

  vm.deleteAllInstrumentations();
  vm.addCodeCB(QBDI::PREINST, incrementThumbPre, (void *)&count1);
  vm.addCodeCB(QBDI::POSTINST, incrementThumbPost, (void *)&count2);

  comparedExec(tPushPopTest_s, inputState, 4096);

  REQUIRE((uint64_t)0 < count1);
  REQUIRE(count1 == count2);

  INFO("Took " << count1 << " instructions\n");
}

TEST_CASE_METHOD(Instr_Test_Thumb, "Instr_Test-tLdmiaStmdbWbackTest") {
  INFO("TEST_SEED=" << seed_random());
  uint64_t count1 = 0;
  uint64_t count2 = 0;

  QBDI::Context inputState;
  initContext(inputState);

  vm.deleteAllInstrumentations();
  vm.addCodeCB(QBDI::PREINST, incrementThumbPre, (void *)&count1);
  vm.addCodeCB(QBDI::POSTINST, incrementThumbPost, (void *)&count2);

  comparedExec(tLdmiaStmdbWbackTest_s, inputState, 4096);

  REQUIRE((uint64_t)0 < count1);
  REQUIRE(count1 == count2);

  INFO("Took " << count1 << " instructions\n");
}

TEST_CASE_METHOD(Instr_Test_Thumb, "Instr_Test-tLdmdbStmiaWbackTest") {
  INFO("TEST_SEED=" << seed_random());
  uint64_t count1 = 0;
  uint64_t count2 = 0;

  QBDI::Context inputState;
  initContext(inputState);

  vm.deleteAllInstrumentations();
  vm.addCodeCB(QBDI::PREINST, incrementThumbPre, (void *)&count1);
  vm.addCodeCB(QBDI::POSTINST, incrementThumbPost, (void *)&count2);

  comparedExec(tLdmdbStmiaWbackTest_s, inputState, 4096);

  REQUIRE((uint64_t)0 < count1);
  REQUIRE(count1 == count2);

  INFO("Took " << count1 << " instructions\n");
}

TEST_CASE_METHOD(Instr_Test_Thumb, "Instr_Test-tLdmiaStmdbTest") {
  INFO("TEST_SEED=" << seed_random());
  uint64_t count1 = 0;
  uint64_t count2 = 0;

  QBDI::Context inputState;
  initContext(inputState);

  vm.deleteAllInstrumentations();
  vm.addCodeCB(QBDI::PREINST, incrementThumbPre, (void *)&count1);
  vm.addCodeCB(QBDI::POSTINST, incrementThumbPost, (void *)&count2);

  comparedExec(tLdmiaStmdbTest_s, inputState, 4096);

  REQUIRE((uint64_t)0 < count1);
  REQUIRE(count1 == count2);

  INFO("Took " << count1 << " instructions\n");
}

TEST_CASE_METHOD(Instr_Test_Thumb, "Instr_Test-tLdmdbStmiaTest") {
  INFO("TEST_SEED=" << seed_random());
  uint64_t count1 = 0;
  uint64_t count2 = 0;

  QBDI::Context inputState;
  initContext(inputState);

  vm.deleteAllInstrumentations();
  vm.addCodeCB(QBDI::PREINST, incrementThumbPre, (void *)&count1);
  vm.addCodeCB(QBDI::POSTINST, incrementThumbPost, (void *)&count2);

  comparedExec(tLdmdbStmiaTest_s, inputState, 4096);

  REQUIRE((uint64_t)0 < count1);
  REQUIRE(count1 == count2);

  INFO("Took " << count1 << " instructions\n");
}

TEST_CASE_METHOD(Instr_Test_Thumb, "Instr_Test-tLdrPCTest") {
  INFO("TEST_SEED=" << seed_random());
  uint64_t count1 = 0;
  uint64_t count2 = 0;

  QBDI::Context inputState;
  initContext(inputState);

  vm.deleteAllInstrumentations();
  vm.addCodeCB(QBDI::PREINST, incrementThumbPre, (void *)&count1);
  vm.addCodeCB(QBDI::POSTINST, incrementThumbPost, (void *)&count2);

  comparedExec(tLdrPCTest_s, inputState, 4096);

  REQUIRE((uint64_t)0 < count1);
  REQUIRE(count1 == count2);

  INFO("Took " << count1 << " instructions\n");
}

TEST_CASE_METHOD(Instr_Test_Thumb, "Instr_Test-tLdrbPCTest") {
  INFO("TEST_SEED=" << seed_random());
  uint64_t count1 = 0;
  uint64_t count2 = 0;

  QBDI::Context inputState;
  initContext(inputState);

  vm.deleteAllInstrumentations();
  vm.addCodeCB(QBDI::PREINST, incrementThumbPre, (void *)&count1);
  vm.addCodeCB(QBDI::POSTINST, incrementThumbPost, (void *)&count2);

  comparedExec(tLdrbPCTest_s, inputState, 4096);

  REQUIRE((uint64_t)0 < count1);
  REQUIRE(count1 == count2);

  INFO("Took " << count1 << " instructions\n");
}

TEST_CASE_METHOD(Instr_Test_Thumb, "Instr_Test-tLdrdPCTest") {
  INFO("TEST_SEED=" << seed_random());
  uint64_t count1 = 0;
  uint64_t count2 = 0;

  QBDI::Context inputState;
  initContext(inputState);

  vm.deleteAllInstrumentations();
  vm.addCodeCB(QBDI::PREINST, incrementThumbPre, (void *)&count1);
  vm.addCodeCB(QBDI::POSTINST, incrementThumbPost, (void *)&count2);

  comparedExec(tLdrdPCTest_s, inputState, 4096);

  REQUIRE((uint64_t)0 < count1);
  REQUIRE(count1 == count2);

  INFO("Took " << count1 << " instructions\n");
}

TEST_CASE_METHOD(Instr_Test_Thumb, "Instr_Test-tLdrhPCTest") {
  INFO("TEST_SEED=" << seed_random());
  uint64_t count1 = 0;
  uint64_t count2 = 0;

  QBDI::Context inputState;
  initContext(inputState);

  vm.deleteAllInstrumentations();
  vm.addCodeCB(QBDI::PREINST, incrementThumbPre, (void *)&count1);
  vm.addCodeCB(QBDI::POSTINST, incrementThumbPost, (void *)&count2);

  comparedExec(tLdrhPCTest_s, inputState, 4096);

  REQUIRE((uint64_t)0 < count1);
  REQUIRE(count1 == count2);

  INFO("Took " << count1 << " instructions\n");
}

TEST_CASE_METHOD(Instr_Test_Thumb, "Instr_Test-tLdrsbPCTest") {
  INFO("TEST_SEED=" << seed_random());
  uint64_t count1 = 0;
  uint64_t count2 = 0;

  QBDI::Context inputState;
  initContext(inputState);

  vm.deleteAllInstrumentations();
  vm.addCodeCB(QBDI::PREINST, incrementThumbPre, (void *)&count1);
  vm.addCodeCB(QBDI::POSTINST, incrementThumbPost, (void *)&count2);

  comparedExec(tLdrsbPCTest_s, inputState, 4096);

  REQUIRE((uint64_t)0 < count1);
  REQUIRE(count1 == count2);

  INFO("Took " << count1 << " instructions\n");
}

TEST_CASE_METHOD(Instr_Test_Thumb, "Instr_Test-tLdrshPCTest") {
  INFO("TEST_SEED=" << seed_random());
  uint64_t count1 = 0;
  uint64_t count2 = 0;

  QBDI::Context inputState;
  initContext(inputState);

  vm.deleteAllInstrumentations();
  vm.addCodeCB(QBDI::PREINST, incrementThumbPre, (void *)&count1);
  vm.addCodeCB(QBDI::POSTINST, incrementThumbPost, (void *)&count2);

  comparedExec(tLdrshPCTest_s, inputState, 4096);

  REQUIRE((uint64_t)0 < count1);
  REQUIRE(count1 == count2);

  INFO("Took " << count1 << " instructions\n");
}

TEST_CASE_METHOD(Instr_Test_Thumb, "Instr_Test-tMovPCTest") {
  INFO("TEST_SEED=" << seed_random());
  uint64_t count1 = 0;
  uint64_t count2 = 0;

  QBDI::Context inputState;
  initContext(inputState);

  vm.deleteAllInstrumentations();
  vm.addCodeCB(QBDI::PREINST, incrementThumbPre, (void *)&count1);
  vm.addCodeCB(QBDI::POSTINST, incrementThumbPost, (void *)&count2);

  comparedExec(tMovPCTest_s, inputState, 4096);

  REQUIRE((uint64_t)0 < count1);
  REQUIRE(count1 == count2);

  INFO("Took " << count1 << " instructions\n");
}

TEST_CASE_METHOD(Instr_Test_Thumb, "Instr_Test-tTBBTest") {
  INFO("TEST_SEED=" << seed_random());
  uint64_t count1 = 0;
  uint64_t count2 = 0;

  QBDI::Context inputState;
  initContext(inputState);

  vm.deleteAllInstrumentations();
  vm.addCodeCB(QBDI::PREINST, incrementThumbPre, (void *)&count1);
  vm.addCodeCB(QBDI::POSTINST, incrementThumbPost, (void *)&count2);

  comparedExec(tTBBTest_s, inputState, 4096);

  REQUIRE((uint64_t)0 < count1);
  REQUIRE(count1 == count2);

  INFO("Took " << count1 << " instructions\n");
}

TEST_CASE_METHOD(Instr_Test_Thumb, "Instr_Test-tTBHTest") {
  INFO("TEST_SEED=" << seed_random());
  uint64_t count1 = 0;
  uint64_t count2 = 0;

  QBDI::Context inputState;
  initContext(inputState);

  vm.deleteAllInstrumentations();
  vm.addCodeCB(QBDI::PREINST, incrementThumbPre, (void *)&count1);
  vm.addCodeCB(QBDI::POSTINST, incrementThumbPost, (void *)&count2);

  comparedExec(tTBHTest_s, inputState, 4096);

  REQUIRE((uint64_t)0 < count1);
  REQUIRE(count1 == count2);

  INFO("Took " << count1 << " instructions\n");
}

TEST_CASE_METHOD(Instr_Test_Thumb, "Instr_Test-tITCondTest") {
  INFO("TEST_SEED=" << seed_random());
  uint64_t count1 = 0;
  uint64_t count2 = 0;

  QBDI::Context inputState;
  initContext(inputState);

  vm.deleteAllInstrumentations();
  vm.addCodeCB(QBDI::PREINST, incrementThumbPre, (void *)&count1);
  vm.addCodeCB(QBDI::POSTINST, incrementThumbPost, (void *)&count2);

  comparedExec(tITCondTest_s, inputState, 4096);

  REQUIRE((uint64_t)0 < count1);
  REQUIRE(count1 == count2);

  INFO("Took " << count1 << " instructions\n");
}

TEST_CASE_METHOD(Instr_Test_Thumb, "Instr_Test-tldrexTest") {
  INFO("TEST_SEED=" << seed_random());
  uint64_t count1 = 0;
  uint64_t count2 = 0;
  uint8_t buffer[4096] = {0};

  QBDI::Context inputState;
  initContext(inputState);
  inputState.gprState.r11 = (QBDI::rword)&buffer;

  vm.deleteAllInstrumentations();
  vm.addCodeCB(QBDI::PREINST, incrementThumbPre, (void *)&count1);
  vm.addCodeCB(QBDI::POSTINST, incrementThumbPost, (void *)&count2);

  comparedExec(tldrexTest_s, inputState, 4096);

  REQUIRE((uint64_t)0 < count1);
  REQUIRE(count1 == count2);

  INFO("Took " << count1 << " instructions\n");
}
