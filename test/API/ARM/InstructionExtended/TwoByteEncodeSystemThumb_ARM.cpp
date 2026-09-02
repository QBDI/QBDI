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
#include "MemAccessTestUtils_ARM.h"

#include "QBDI/InstAnalysis.h"

using QBDITestBatch2::checkFeature;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t2setpan") {
  if (!checkFeature("v8.1a")) {
    return;
  }
  QBDI::rword addr = genASM("setpan #1\n", QBDI::CPUMode::Thumb, {"v8.1a"});
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "t2SETPAN");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tbkpt") {
  QBDI::rword addr = genASM("bkpt #0\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tBKPT");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tblxnsr") {
  if (!checkFeature("8msecext")) {
    return;
  }
  QBDI::rword addr = genASM("blxns r0\n", QBDI::CPUMode::Thumb, {"8msecext"});
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tBLXNSr");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tbxns") {
  if (!checkFeature("8msecext")) {
    return;
  }
  QBDI::rword addr = genASM("bxns r0\n", QBDI::CPUMode::Thumb, {"8msecext"});
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tBXNS");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tcps") {
  QBDI::rword addr = genASM("cpsie i\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tCPS");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-thint") {
  QBDI::rword addr = genASM("hint #0\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tHINT");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-thlt") {
  if (!checkFeature("v8")) {
    return;
  }
  QBDI::rword addr = genASM("hlt #0\n", QBDI::CPUMode::Thumb, {"v8"});
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tHLT");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tsvc") {
  QBDI::rword addr = genASM("svc #0\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tSVC");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-ttrap") {
  QBDI::rword addr = genASM("trap\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tTRAP");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tudf") {
  QBDI::rword addr = genASM("udf #0\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tUDF");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-t__brkdiv0") {
  QBDI::rword addr = genASM("__brkdiv0\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "t__brkdiv0");
  CHECK(ana->instSize == 2);
}
