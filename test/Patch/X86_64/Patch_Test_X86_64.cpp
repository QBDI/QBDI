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
#include <catch2/catch.hpp>
#include "Patch/Patch_Test.h"

TEST_CASE_METHOD(Patch_Test, "Patch_Test-UnalignedCodeForward") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(UnalignedCodeForward_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test, "Patch_Test-UnalignedCodeBackward") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(UnalignedCodeBackward_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test, "Patch_Test-LoopCode") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(LoopCode_s, inputState, 4096);
}
