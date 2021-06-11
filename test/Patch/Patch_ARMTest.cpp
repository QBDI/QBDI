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
#include "Patch/Patch_ARMTest.h"
#include <catch2/catch.hpp>

TEST_CASE_METHOD(Patch_ARMTest, "Patch_ARMTest-GPRSave") {
  QBDI::Context inputState;

  memset(&inputState, 0, sizeof(QBDI::Context));
  comparedExec(GPRSave_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_ARMTest, "Patch_ARMTest-GPRShuffle") {
  QBDI::Context inputState;

  memset(&inputState, 0, sizeof(QBDI::Context));
  for (uint32_t i = 0; i < QBDI::AVAILABLE_GPR; i++)
    QBDI_GPR_SET(&inputState.gprState, i, i);
  comparedExec(GPRShuffle_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_ARMTest, "Patch_ARMTest-RelativeAddressing") {
  QBDI::Context inputState;

  memset(&inputState, 0, sizeof(QBDI::Context));
  inputState.gprState.r0 = rand();
  inputState.gprState.r1 = rand();
  comparedExec(RelativeAddressing_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_ARMTest, "Patch_ARMTest-ConditionalBranching") {
  QBDI::Context inputState;

  memset(&inputState, 0, sizeof(QBDI::Context));
  inputState.gprState.r0 = rand();
  inputState.gprState.r1 = rand();
  inputState.gprState.r2 = rand();
  inputState.gprState.r3 = rand();
  comparedExec(ConditionalBranching_s, inputState, 4096);
}
