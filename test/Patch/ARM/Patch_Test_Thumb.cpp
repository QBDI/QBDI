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
#include "Patch/ARM/Patch_Test_Thumb.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tGPRSave") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tGPRSave_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tGPRShuffle") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tGPRShuffle_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tRelativeAddressing") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tRelativeAddressing_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tFibonacciRecursion") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  inputState.gprState.r0 = (rand() % 20) + 2;
  comparedExec(tFibonacciRecursion_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tBranchCondTest") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tBranchCondTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tBranchLinkCondTest") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tBranchLinkCondTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tBranchRegisterCondTest") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tBranchRegisterCondTest_s, inputState, 4096);

  CHECK(inputState.gprState.r10 == 0);
  CHECK(inputState.gprState.r11 == 0);
  CHECK(inputState.gprState.r12 == 0);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tPushPopTest") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tPushPopTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tLdmiaStmdbWbackTest") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tLdmiaStmdbWbackTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tLdmdbStmiaWbackTest") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tLdmdbStmiaWbackTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tLdmiaStmdbTest") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tLdmiaStmdbTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tLdmdbStmiaTest") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tLdmdbStmiaTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tLdrPCTest") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tLdrPCTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tLdrbPCTest") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tLdrbPCTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tLdrdPCTest") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tLdrdPCTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tLdrhPCTest") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tLdrhPCTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tLdrsbPCTest") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tLdrsbPCTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tLdrshPCTest") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tLdrshPCTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tMovPCTest") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tMovPCTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tTBBTest") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tTBBTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tTBHTest") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tTBHTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tITCondTest") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tITCondTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tldrexTest") {
  INFO("TEST_SEED=" << seed_random());
  QBDI::Context inputState;
  uint8_t buffer[4096] = {0};

  initContext(inputState);
  inputState.gprState.r11 = (QBDI::rword)&buffer;
  comparedExec(tldrexTest_s, inputState, 4096);
}
