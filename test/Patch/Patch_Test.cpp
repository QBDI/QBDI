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
#include "Patch/Patch_Test.h"
#include <catch2/catch.hpp>

TEST_CASE_METHOD(Patch_Test, "Patch_Test-EmptyFunction") {
  QBDI::Context inputState;

  memset(&inputState, 0, sizeof(QBDI::Context));
  comparedExec("", inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test, "Patch_Test-GPRSave") {
  QBDI::Context inputState;

  memset(&inputState, 0, sizeof(QBDI::Context));
  comparedExec(GPRSave_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test, "Patch_Test-GPRShuffle") {
  QBDI::Context inputState;

  memset(&inputState, 0, sizeof(QBDI::Context));
  for (uint32_t i = 0; i < QBDI::AVAILABLE_GPR; i++)
    QBDI_GPR_SET(&inputState.gprState, i, i);
  comparedExec(GPRShuffle_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test, "Patch_Test-RelativeAddressing") {
  QBDI::Context inputState;

  memset(&inputState, 0, sizeof(QBDI::Context));
  QBDI_GPR_SET(&inputState.gprState, 0, get_random());
  QBDI_GPR_SET(&inputState.gprState, 1, get_random());
  comparedExec(RelativeAddressing_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test, "Patch_Test-ConditionalBranching") {
  QBDI::Context inputState;

  memset(&inputState, 0, sizeof(QBDI::Context));
  QBDI_GPR_SET(&inputState.gprState, 0, get_random());
  QBDI_GPR_SET(&inputState.gprState, 1, get_random());
  QBDI_GPR_SET(&inputState.gprState, 2, get_random());
  QBDI_GPR_SET(&inputState.gprState, 3, get_random());
  comparedExec(ConditionalBranching_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test, "Patch_Test-FibonacciRecursion") {
  QBDI::Context inputState;

  memset(&inputState, 0, sizeof(QBDI::Context));
  QBDI_GPR_SET(&inputState.gprState, 0, (rand() % 20) + 2);
  comparedExec(FibonacciRecursion_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test, "Patch_Test-StackTricks") {
  QBDI::Context inputState;

  memset(&inputState, 0, sizeof(QBDI::Context));
  QBDI_GPR_SET(&inputState.gprState, 0, (rand() % 20) + 2);
  comparedExec(StackTricks_s, inputState, 4096);
}
