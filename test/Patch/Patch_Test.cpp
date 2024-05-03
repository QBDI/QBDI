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
#include "Patch/Patch_Test.h"
#include <catch2/catch.hpp>

TEST_CASE_METHOD(Patch_Test, "Patch_Test-EmptyFunction") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec("", inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test, "Patch_Test-GPRSave") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(GPRSave_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test, "Patch_Test-GPRShuffle") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(GPRShuffle_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test, "Patch_Test-RelativeAddressing") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(RelativeAddressing_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test, "Patch_Test-ConditionalBranching") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(ConditionalBranching_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test, "Patch_Test-FibonacciRecursion") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  QBDI_GPR_SET(&inputState.gprState, 0, (rand() % 20) + 2);
  comparedExec(FibonacciRecursion_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test, "Patch_Test-StackTricks") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  QBDI_GPR_SET(&inputState.gprState, 0, (rand() % 20) + 2);
  comparedExec(StackTricks_s, inputState, 4096);
}
