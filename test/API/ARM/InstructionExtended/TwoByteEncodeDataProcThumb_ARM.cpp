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

#include "QBDI/InstAnalysis.h"

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tadc") {
  QBDI::rword addr = genASM("adcs r0, r1\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tADC");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-taddi3") {
  QBDI::rword addr = genASM("adds r0, r1, #3\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tADDi3");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-taddi8") {
  QBDI::rword addr = genASM("adds r0, #8\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tADDi8");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-taddrsp") {
  QBDI::rword addr = genASM("add r0, sp, r0\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tADDrSP");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-taddrspi") {
  QBDI::rword addr = genASM("add r0, sp, #4\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tADDrSPi");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-taddspi") {
  QBDI::rword addr = genASM("add sp, #4\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tADDspi");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-taddspr") {
  QBDI::rword addr = genASM("add sp, r0\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tADDspr");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tand") {
  QBDI::rword addr = genASM("ands r0, r1\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tAND");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tasrri") {
  QBDI::rword addr = genASM("asrs r0, r1, #4\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tASRri");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tasrrr") {
  QBDI::rword addr = genASM("asrs r0, r1\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tASRrr");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tbic") {
  QBDI::rword addr = genASM("bics r0, r1\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tBIC");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tcmnz") {
  QBDI::rword addr = genASM("cmn r0, r1\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tCMNz");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tcmphir") {
  QBDI::rword addr = genASM("cmp r0, r8\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tCMPhir");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tcmpr") {
  QBDI::rword addr = genASM("cmp r0, r1\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tCMPr");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-teor") {
  QBDI::rword addr = genASM("eors r0, r1\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tEOR");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tlslri") {
  QBDI::rword addr = genASM("lsls r0, r1, #4\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tLSLri");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tlslrr") {
  QBDI::rword addr = genASM("lsls r0, r1\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tLSLrr");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tlsrri") {
  QBDI::rword addr = genASM("lsrs r0, r1, #4\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tLSRri");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tlsrrr") {
  QBDI::rword addr = genASM("lsrs r0, r1\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tLSRrr");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tmovsr") {
  QBDI::rword addr = genASM("movs r0, r1\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tMOVSr");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tmovi8") {
  QBDI::rword addr = genASM("movs r0, #8\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tMOVi8");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tmovr") {
  QBDI::rword addr = genASM("mov r0, r1\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tMOVr");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tmul") {
  QBDI::rword addr = genASM("muls r0, r1\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tMUL");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tmvn") {
  QBDI::rword addr = genASM("mvns r0, r1\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tMVN");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-torr") {
  QBDI::rword addr = genASM("orrs r0, r1\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tORR");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-trev") {
  QBDI::rword addr = genASM("rev r0, r1\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tREV");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-trev16") {
  QBDI::rword addr = genASM("rev16 r0, r1\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tREV16");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-trevsh") {
  QBDI::rword addr = genASM("revsh r0, r1\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tREVSH");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tror") {
  QBDI::rword addr = genASM("rors r0, r1\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tROR");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-trsb") {
  QBDI::rword addr = genASM("rsbs r0, r1, #0\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tRSB");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tsbc") {
  QBDI::rword addr = genASM("sbcs r0, r1\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tSBC");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tsubi3") {
  QBDI::rword addr = genASM("subs r0, r1, #3\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tSUBi3");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tsubi8") {
  QBDI::rword addr = genASM("subs r0, #8\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tSUBi8");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tsubrr") {
  QBDI::rword addr = genASM("subs r0, r1, r2\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tSUBrr");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tsubspi") {
  QBDI::rword addr = genASM("sub sp, #4\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tSUBspi");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tsxtb") {
  QBDI::rword addr = genASM("sxtb r0, r1\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tSXTB");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tsxth") {
  QBDI::rword addr = genASM("sxth r0, r1\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tSXTH");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-ttst") {
  QBDI::rword addr = genASM("tst r0, r1\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tTST");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tuxtb") {
  QBDI::rword addr = genASM("uxtb r0, r1\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tUXTB");
  CHECK(ana->instSize == 2);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_Thumb-tuxth") {
  QBDI::rword addr = genASM("uxth r0, r1\n", QBDI::CPUMode::Thumb);
  const QBDI::InstAnalysis *ana =
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION);
  REQUIRE(ana != nullptr);
  CHECK(std::string(ana->mnemonic) == "tUXTH");
  CHECK(ana->instSize == 2);
}
