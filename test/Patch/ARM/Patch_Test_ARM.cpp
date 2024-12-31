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
#include <catch2/catch_test_macros.hpp>
#include "Patch/Patch_Test.h"

TEST_CASE_METHOD(Patch_Test, "Patch_Test-STLDMIA") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(STLDMIA_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test, "Patch_Test-STLDMIB") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(STLDMIB_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test, "Patch_Test-STLDMDA") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(STLDMDA_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test, "Patch_Test-STLDMDB") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(STLDMDB_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test, "Patch_Test-STMDB_LDMIA_post") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;
  initContext(inputState);

  comparedExec(STMDB_LDMIA_post_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test, "Patch_Test-STMDA_LDMIB_post") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;
  initContext(inputState);

  comparedExec(STMDA_LDMIB_post_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test, "Patch_Test-STMIB_LDMDA_post") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;
  initContext(inputState);

  comparedExec(STMIB_LDMDA_post_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test, "Patch_Test-STMIA_LDMDB_post") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;
  initContext(inputState);

  comparedExec(STMIA_LDMDB_post_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test, "Patch_Test-LDREXTest") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;
  uint8_t buffer[4096] = {0};

  initContext(inputState);
  inputState.gprState.r11 = (QBDI::rword)&buffer;
  comparedExec(LDREXTest_s, inputState, 4096);
}
