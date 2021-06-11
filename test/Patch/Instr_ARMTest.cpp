/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2021 Quarkslab
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
#include "Instr_ARMTest.h"
#include <catch2/catch.hpp>

QBDI::VMAction increment(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState,
                         QBDI::FPRState *fprState, void *data) {
  *((uint64_t *)data) = *((uint64_t *)data) + 1;
  return QBDI::VMAction::CONTINUE;
}

TEST_CASE_METHOD(Instr_ARMTest, "Instr_ARMTest-GPRSave_IC") {
  uint64_t count1 = 0;
  uint64_t count2 = 0;

  QBDI::Context inputState;
  memset(&inputState, 0, sizeof(QBDI::Context));

  vm.deleteAllInstrumentations();
  vm.addCodeCB(QBDI::PREINST, increment, (void *)&count1);
  vm.addCodeCB(QBDI::POSTINST, increment, (void *)&count2);

  comparedExec(GPRSave_s, inputState, 4096);

  REQUIRE((uint64_t)0 < count1);
  REQUIRE(count1 == count2);

  INFO("Took " << count1 << " instructions");
}

TEST_CASE_METHOD(Instr_ARMTest, "Instr_ARMTest-GPRShuffle_IC") {
  uint64_t count1 = 0;
  uint64_t count2 = 0;

  QBDI::Context inputState;
  memset(&inputState, 0, sizeof(QBDI::Context));
  for (uint32_t i = 0; i < QBDI::AVAILABLE_GPR; i++)
    QBDI_GPR_SET(&inputState.gprState, i, i);

  vm.deleteAllInstrumentations();
  vm.addCodeCB(QBDI::PREINST, increment, (void *)&count1);
  vm.addCodeCB(QBDI::POSTINST, increment, (void *)&count2);

  comparedExec(GPRShuffle_s, inputState, 4096);

  REQUIRE((uint64_t)0 < count1);
  REQUIRE(count1 == count2);

  INFO("Took " << count1 << " instructions");
}

TEST_CASE_METHOD(Instr_ARMTest, "Instr_ARMTest-RelativeAddressing_IC") {
  uint64_t count1 = 0;
  uint64_t count2 = 0;

  QBDI::Context inputState;
  memset(&inputState, 0, sizeof(QBDI::Context));
  inputState.gprState.r0 = rand();
  inputState.gprState.r1 = rand();

  vm.deleteAllInstrumentations();
  vm.addCodeCB(QBDI::PREINST, increment, (void *)&count1);
  vm.addCodeCB(QBDI::POSTINST, increment, (void *)&count2);

  comparedExec(RelativeAddressing_s, inputState, 4096);

  REQUIRE((uint64_t)0 < count1);
  REQUIRE(count1 == count2);

  INFO("Took " << count1 << " instructions");
}

TEST_CASE_METHOD(Instr_ARMTest, "Instr_ARMTest-ConditionalBranching_IC") {
  uint64_t count1 = 0;
  uint64_t count2 = 0;

  QBDI::Context inputState;
  memset(&inputState, 0, sizeof(QBDI::Context));
  inputState.gprState.r0 = rand();
  inputState.gprState.r1 = rand();
  inputState.gprState.r2 = rand();
  inputState.gprState.r3 = rand();

  vm.deleteAllInstrumentations();
  vm.addCodeCB(QBDI::PREINST, increment, (void *)&count1);
  vm.addCodeCB(QBDI::POSTINST, increment, (void *)&count2);

  comparedExec(ConditionalBranching_s, inputState, 4096);

  REQUIRE((uint64_t)0 < count1);
  REQUIRE(count1 == count2);

  INFO("Took " << count1 << " instructions");
}
