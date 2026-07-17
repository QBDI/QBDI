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
#include "Patch/ARM/Patch_Test_Thumb.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tGPRSave") {
  seed_random();
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tGPRSave_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tGPRShuffle") {
  seed_random();
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tGPRShuffle_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tRelativeAddressing") {
  seed_random();
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tRelativeAddressing_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tFibonacciRecursion") {
  seed_random();
  QBDI::Context inputState;

  initContext(inputState);
  inputState.gprState.r0 = (rand() % 20) + 2;
  comparedExec(tFibonacciRecursion_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tBranchCondTest") {
  seed_random();
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tBranchCondTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tBranchLinkCondTest") {
  seed_random();
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tBranchLinkCondTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tBranchRegisterCondTest") {
  seed_random();
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tBranchRegisterCondTest_s, inputState, 4096);

  CHECK(inputState.gprState.r10 == 0);
  CHECK(inputState.gprState.r11 == 0);
  CHECK(inputState.gprState.r12 == 0);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tPushPopTest") {
  seed_random();
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tPushPopTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tLdmiaStmdbWbackTest") {
  seed_random();
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tLdmiaStmdbWbackTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tLdmdbStmiaWbackTest") {
  seed_random();
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tLdmdbStmiaWbackTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tLdmiaStmdbTest") {
  seed_random();
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tLdmiaStmdbTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tLdmdbStmiaTest") {
  seed_random();
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tLdmdbStmiaTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tLdrPCTest") {
  seed_random();
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tLdrPCTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tLdrbPCTest") {
  seed_random();
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tLdrbPCTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tLdrdPCTest") {
  seed_random();
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tLdrdPCTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tLdrhPCTest") {
  seed_random();
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tLdrhPCTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tLdrsbPCTest") {
  seed_random();
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tLdrsbPCTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tLdrshPCTest") {
  seed_random();
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tLdrshPCTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tMovPCTest") {
  seed_random();
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tMovPCTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tTBBTest") {
  seed_random();
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tTBBTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tTBHTest") {
  seed_random();
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tTBHTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tITCondTest") {
  seed_random();
  QBDI::Context inputState;

  initContext(inputState);
  comparedExec(tITCondTest_s, inputState, 4096);
}

TEST_CASE_METHOD(Patch_Test_Thumb, "Patch_Test-tldrexTest") {
  seed_random();
  QBDI::Context inputState;
  uint8_t buffer[4096] = {0};

  initContext(inputState);
  inputState.gprState.r11 = (QBDI::rword)&buffer;
  comparedExec(tldrexTest_s, inputState, 4096);
}
