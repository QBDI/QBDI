/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2023 Quarkslab
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
#include "API/APITest.h"

#include <algorithm>
#include <sstream>
#include <string>
#include "inttypes.h"

#include "QBDI/Memory.hpp"
#include "QBDI/Platform.h"

struct ExpectedInstAnalysis {
  std::string mnemonic;
  QBDI::rword address;
  QBDI::CPUMode cpuMode;
  uint32_t instSize;
  bool affectControlFlow;
  bool isBranch;
  bool isCall;
  bool isReturn;
  bool isCompare;
  bool isPredicable;
  bool mayLoad;
  bool mayStore;
  uint32_t loadSize;
  uint32_t storeSize;
  QBDI::ConditionType condition;
};

[[maybe_unused]] static void debugOperand(const QBDI::InstAnalysis *ana) {
  if ((ana->analysisType & QBDI::ANALYSIS_OPERANDS) ==
      QBDI::ANALYSIS_OPERANDS) {
    for (int i = 0; i < ana->numOperands; i++) {
      const QBDI::OperandAnalysis &op = ana->operands[i];
      WARN("- type: " << op.type << ", flag: " << op.flag
                      << ", value: " << op.value << ", size: " << (int)op.size
                      << ", regOff : " << (int)op.regOff
                      << ", regCtxIdx: " << op.regCtxIdx << ", regName: "
                      << (op.regName == nullptr ? "nullptr" : op.regName)
                      << ", regAccess: "
                      << (op.regAccess & QBDI::REGISTER_READ ? "r" : "-")
                      << (op.regAccess & QBDI::REGISTER_WRITE ? "w" : "-"));
    }
  }
}

static void checkOperand(const QBDI::InstAnalysis *ana,
                         const std::vector<QBDI::OperandAnalysis> expecteds,
                         QBDI::RegisterAccessType flagsAccess) {

  CHECKED_IF((ana->analysisType & QBDI::ANALYSIS_OPERANDS) ==
             QBDI::ANALYSIS_OPERANDS) {
    CHECK(flagsAccess == ana->flagsAccess);
    CHECK(expecteds.size() == ana->numOperands);
    for (unsigned i = 0;
         i < std::min<unsigned>(ana->numOperands, expecteds.size()); i++) {
      const QBDI::OperandAnalysis &expect = expecteds[i];
      const QBDI::OperandAnalysis &op = ana->operands[i];
      INFO("For operand " << i);

      CHECK(expect.type == op.type);
      CHECK(expect.flag == op.flag);
      if (op.type == QBDI::OPERAND_IMM || expect.value != 0) {
        CHECK(expect.value == op.value);
      }
      CHECK(expect.size == op.size);
      CHECK(expect.regOff == op.regOff);
      CHECK(expect.regCtxIdx == op.regCtxIdx);
      CHECK(expect.regAccess == op.regAccess);

      const std::string expectedRegName(
          (expect.regName != nullptr) ? expect.regName : "");
      const std::string foundRegName((op.regName != nullptr) ? op.regName : "");
      CHECK(expectedRegName == foundRegName);

      if (expect.regName == nullptr || op.regName == nullptr) {
        CHECK(expect.regName == op.regName);
      }
    }
  }
}

static void checkInst(const QBDI::InstAnalysis *ana,
                      const ExpectedInstAnalysis expected) {

  CHECKED_IF((ana->analysisType & QBDI::ANALYSIS_INSTRUCTION) ==
             QBDI::ANALYSIS_INSTRUCTION) {
    CHECK(expected.mnemonic == ana->mnemonic);
    CHECK(expected.address == ana->address);
    CHECK(expected.instSize == ana->instSize);
    CHECK(expected.cpuMode == ana->cpuMode);
    CHECK(expected.affectControlFlow == ana->affectControlFlow);
    CHECK(expected.isBranch == ana->isBranch);
    CHECK(expected.isCall == ana->isCall);
    CHECK(expected.isReturn == ana->isReturn);
    CHECK(expected.isCompare == ana->isCompare);
    CHECK(expected.isPredicable == ana->isPredicable);
    CHECK(expected.mayLoad == ana->mayLoad);
    CHECK(expected.mayStore == ana->mayStore);
    CHECK(expected.loadSize == ana->loadSize);
    CHECK(expected.storeSize == ana->storeSize);
    CHECK(expected.condition == ana->condition);
  }
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_Thumb-CachedInst") {

  QBDI::rword addr = genASM("bic.W r0, r0, 0xff\n", QBDI::CPUMode::Thumb);

  CHECK(vm.getCachedInstAnalysis(addr) != nullptr);

  vm.clearAllCache();

  CHECK(vm.getCachedInstAnalysis(addr) == nullptr);

  vm.precacheBasicBlock(addr);

  CHECK(vm.getCachedInstAnalysis(addr) != nullptr);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_Thumb-bx_lr") {

  QBDI::rword addr = genASM("bx lr\n", QBDI::CPUMode::Thumb);

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "tBX", addr & (~1), QBDI::CPUMode::Thumb,
          /* instSize */ 2, /* affectControlFlow */ true, /* isBranch */ true,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 14,
                    "LR", QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_Thumb-bx_reg") {

  QBDI::rword addr = genASM("bx r0\n", QBDI::CPUMode::Thumb);

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "tBX", addr & (~1), QBDI::CPUMode::Thumb,
          /* instSize */ 2, /* affectControlFlow */ true, /* isBranch */ true,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 0, "R0",
                    QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_Thumb-bxgt") {

  QBDI::rword addr = genASM("it gt\nbxgt r0\n", QBDI::CPUMode::Thumb);

  checkInst(
      vm.getCachedInstAnalysis(addr + 2, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "tBX", (addr + 2) & (~1), QBDI::CPUMode::Thumb,
          /* instSize */ 2, /* affectControlFlow */ true, /* isBranch */ true,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_GREAT});
  checkOperand(vm.getCachedInstAnalysis(addr + 2, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 0, "R0",
                    QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_READ);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_Thumb-bxal") {

  QBDI::rword addr = genASM("it al\nbxal r0\n", QBDI::CPUMode::Thumb) + 2;

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "tBX", addr & (~1), QBDI::CPUMode::Thumb,
          /* instSize */ 2, /* affectControlFlow */ true, /* isBranch */ true,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 0, "R0",
                    QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_Thumb-beq") {

  QBDI::rword addr =
      genASM("beq label\nnop\nnop\nnop\nlabel: nop\n", QBDI::CPUMode::Thumb);

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "tBcc", addr & (~1), QBDI::CPUMode::Thumb,
          /* instSize */ 2, /* affectControlFlow */ true, /* isBranch */ true,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_EQUALS});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_PCREL, 4, 2, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_READ);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_Thumb-add") {

  QBDI::rword addr = genASM("add r1, r2\n", QBDI::CPUMode::Thumb);

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "tADDhirr", addr & (~1), QBDI::CPUMode::Thumb,
          /* instSize */ 2, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 1, "R1",
                    QBDI::REGISTER_READ_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 2, "R2",
                    QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_Thumb-adds") {

  QBDI::rword addr = genASM("adds r1, r1, r2\n", QBDI::CPUMode::Thumb);

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "tADDrr", addr & (~1), QBDI::CPUMode::Thumb,
          /* instSize */ 2, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 1, "R1",
                    QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 1, "R1",
                    QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 2, "R2",
                    QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_WRITE);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_Thumb-addal") {

  QBDI::rword addr =
      genASM("it al\naddal r1, r1, r2\n", QBDI::CPUMode::Thumb) + 2;

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "tADDrr", addr & (~1), QBDI::CPUMode::Thumb,
          /* instSize */ 2, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 1, "R1",
                    QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 1, "R1",
                    QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 2, "R2",
                    QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_Thumb-addeq") {

  QBDI::rword addr =
      genASM("it eq\naddeq r1, r1, r2\n", QBDI::CPUMode::Thumb) + 2;

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "tADDrr", addr & (~1), QBDI::CPUMode::Thumb,
          /* instSize */ 2, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_EQUALS});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 1, "R1",
                    QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 1, "R1",
                    QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 2, "R2",
                    QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_READ);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_Thumb-ital") {

  QBDI::rword addr = genASM("it al\naddal r1, r1, r2\n", QBDI::CPUMode::Thumb);

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "t2IT", addr & (~1), QBDI::CPUMode::Thumb,
          /* instSize */ 2, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
      {
          {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_UNDEFINED_EFFECT,
           QBDI::CONDITION_ALWAYS, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
          {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 8, 4, 0, -1,
           nullptr, QBDI::REGISTER_UNUSED},
          {QBDI::OPERAND_SEG, QBDI::OPERANDFLAG_IMPLICIT, 12, 1, 0, -1,
           "ITSTATE", QBDI::REGISTER_WRITE},
      },
      QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_Thumb-iteq") {

  QBDI::rword addr = genASM("it eq\naddeq r1, r1, r2\n", QBDI::CPUMode::Thumb);

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "t2IT", addr & (~1), QBDI::CPUMode::Thumb,
          /* instSize */ 2, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
      {
          {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_UNDEFINED_EFFECT,
           QBDI::CONDITION_EQUALS, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
          {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 8, 4, 0, -1,
           nullptr, QBDI::REGISTER_UNUSED},
          {QBDI::OPERAND_SEG, QBDI::OPERANDFLAG_IMPLICIT, 12, 1, 0, -1,
           "ITSTATE", QBDI::REGISTER_WRITE},
      },
      QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_Thumb-ldrex") {

  QBDI::rword addr = genASM("ldrex r0, [r10, #16]\n", QBDI::CPUMode::Thumb);

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "t2LDREX", addr & (~1), QBDI::CPUMode::Thumb,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 4, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 0, "R0",
                    QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 10,
                    "R10", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 16, 4, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_Thumb-ldrexb") {

  QBDI::rword addr = genASM("ldrexb r0, [r10]\n", QBDI::CPUMode::Thumb);

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "t2LDREXB", addr & (~1), QBDI::CPUMode::Thumb,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 1, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 0, "R0",
                    QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 10,
                    "R10", QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_Thumb-ldrexd") {

  QBDI::rword addr = genASM("ldrexd r0, r1, [r10]\n", QBDI::CPUMode::Thumb);

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "t2LDREXD", addr & (~1), QBDI::CPUMode::Thumb,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 8, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 0, "R0",
                    QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 1, "R1",
                    QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 10,
                    "R10", QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_Thumb-strex") {

  QBDI::rword addr = genASM("strex r5, r0, [r10, #16]\n", QBDI::CPUMode::Thumb);

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "t2STREX", addr & (~1), QBDI::CPUMode::Thumb,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ true,
          /* loadSize */ 0, /* storeSize */ 4,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 5, "R5",
                    QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 0, "R0",
                    QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 10,
                    "R10", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 16, 4, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_Thumb-strexb") {

  QBDI::rword addr = genASM("strexb r5, r0, [r10]\n", QBDI::CPUMode::Thumb);

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "t2STREXB", addr & (~1), QBDI::CPUMode::Thumb,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ true,
          /* loadSize */ 0, /* storeSize */ 1,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 5, "R5",
                    QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 0, "R0",
                    QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 10,
                    "R10", QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_Thumb-strexd") {

  QBDI::rword addr = genASM("strexd r5, r0, r1, [r10]\n", QBDI::CPUMode::Thumb);

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "t2STREXD", addr & (~1), QBDI::CPUMode::Thumb,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ true,
          /* loadSize */ 0, /* storeSize */ 8,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 5, "R5",
                    QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 0, "R0",
                    QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 1, "R1",
                    QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 10,
                    "R10", QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_Thumb-cmp") {

  QBDI::rword addr = genASM("cmp r5, #16\n", QBDI::CPUMode::Thumb);

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "tCMPi8", addr & (~1), QBDI::CPUMode::Thumb,
          /* instSize */ 2, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ true,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 5, "R5",
                    QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 16,
                    4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_WRITE);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_Thumb-tst") {

  QBDI::rword addr = genASM("tst r5, #16\n", QBDI::CPUMode::Thumb);

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "t2TSTri", addr & (~1), QBDI::CPUMode::Thumb,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ true,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 5, "R5",
                    QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 16,
                    4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_WRITE);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_Thumb-tldrh") {

  QBDI::rword addr = genASM("ldrh r2, [r0, #4]\n", QBDI::CPUMode::Thumb);

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "tLDRHi", addr & (~1), QBDI::CPUMode::Thumb,
          /* instSize */ 2, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 2, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 2, "R2",
                    QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 0, "R0",
                    QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 4, 4, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_Thumb-tldr") {

  QBDI::rword addr = genASM("ldr r2, [r0, #4]\n", QBDI::CPUMode::Thumb);

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "tLDRi", addr & (~1), QBDI::CPUMode::Thumb,
          /* instSize */ 2, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 4, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 2, "R2",
                    QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 0, "R0",
                    QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 4, 4, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_Thumb-tstrh") {

  QBDI::rword addr = genASM("strh r2, [r0, #4]\n", QBDI::CPUMode::Thumb);

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "tSTRHi", addr & (~1), QBDI::CPUMode::Thumb,
          /* instSize */ 2, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ true,
          /* loadSize */ 0, /* storeSize */ 2,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 2, "R2",
                    QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 0, "R0",
                    QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 4, 4, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_Thumb-tstr") {

  QBDI::rword addr = genASM("str r2, [r0, #4]\n", QBDI::CPUMode::Thumb);

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "tSTRi", addr & (~1), QBDI::CPUMode::Thumb,
          /* instSize */ 2, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ true,
          /* loadSize */ 0, /* storeSize */ 4,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 2, "R2",
                    QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 0, "R0",
                    QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 4, 4, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_Thumb-tldrsp") {

  QBDI::rword addr = genASM("ldr r2, [sp, #4]\n", QBDI::CPUMode::Thumb);

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "tLDRspi", addr & (~1), QBDI::CPUMode::Thumb,
          /* instSize */ 2, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 4, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 2, "R2",
                    QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 13,
                    "SP", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 4, 4, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_Thumb-tstrsp") {

  QBDI::rword addr = genASM("str	r2, [sp, #0x8]", QBDI::CPUMode::Thumb);

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "tSTRspi", addr & (~1), QBDI::CPUMode::Thumb,
          /* instSize */ 2, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ true,
          /* loadSize */ 0, /* storeSize */ 4,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 2, "R2",
                    QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 13,
                    "SP", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 8, 4, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_Thumb-t2ldrd") {

  QBDI::rword addr =
      genASM("ldrd r2, r12, [r0, #-16]!\n", QBDI::CPUMode::Thumb);

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "t2LDRD_PRE", addr & (~1), QBDI::CPUMode::Thumb,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 8, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 2, "R2",
                    QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 12,
                    "R12", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 0, "R0",
                    QBDI::REGISTER_READ_WRITE},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, -16, 4, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_Thumb-t2ldrdsp") {

  QBDI::rword addr =
      genASM("ldrd r2, r12, [sp, #-16]!\n", QBDI::CPUMode::Thumb);

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "t2LDRD_PRE", addr & (~1), QBDI::CPUMode::Thumb,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 8, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 2, "R2",
                    QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 12,
                    "R12", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 13,
                    "SP", QBDI::REGISTER_READ_WRITE},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, -16, 4, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_UNUSED);
}
