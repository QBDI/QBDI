/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2025 Quarkslab
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

TEST_CASE_METHOD(APITest, "InstAnalysisTest_AARCH64-CachedInst") {

  QBDI::rword addr = genASM("stp x0, x1, [sp]\n");

  CHECK(vm.getCachedInstAnalysis(addr) != nullptr);

  vm.clearAllCache();

  CHECK(vm.getCachedInstAnalysis(addr) == nullptr);

  vm.precacheBasicBlock(addr);

  CHECK(vm.getCachedInstAnalysis(addr) != nullptr);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_AARCH64-ret") {

  QBDI::rword addr = genASM("ret\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "RET", addr,
          /* instSize */ 4, /* affectControlFlow */ true, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ true, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 30,
                    "LR", QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_AARCH64-br") {

  QBDI::rword addr = genASM("br x0\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "BR", addr,
          /* instSize */ 4, /* affectControlFlow */ true, /* isBranch */ true,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 0, "X0",
                    QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_AARCH64-blr") {

  QBDI::rword addr = genASM("blr x10\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "BLR", addr,
          /* instSize */ 4, /* affectControlFlow */ true, /* isBranch */ false,
          /* isCall */ true, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 10,
                    "X10", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 8, 0, 31,
                    "SP", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 8, 0, 30,
                    "LR", QBDI::REGISTER_WRITE},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_AARCH64-b") {

  QBDI::rword addr = genASM("b label\nnop\nnop\nnop\nlabel: nop\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "B", addr,
          /* instSize */ 4, /* affectControlFlow */ true, /* isBranch */ true,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_PCREL, 4, 2, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_AARCH64-bl") {

  QBDI::rword addr = genASM("bl label\nnop\nnop\nnop\nlabel: nop\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "BL", addr,
          /* instSize */ 4, /* affectControlFlow */ true, /* isBranch */ false,
          /* isCall */ true, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_PCREL, 4, 2, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 8, 0, 31,
                    "SP", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 8, 0, 30,
                    "LR", QBDI::REGISTER_WRITE},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_AARCH64-addi") {

  QBDI::rword addr = genASM("add x17, X28, #258\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "ADDXri", addr,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 17,
                    "X17", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 28,
                    "X28", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 258,
                    8, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 8,
                    0, -1, nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_AARCH64-addr") {

  QBDI::rword addr = genASM("add W17, W28, W8, LSR #8\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "ADDWrs", addr,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 17,
                    "W17", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 28,
                    "W28", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 8, "W8", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0x48,
                    8, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_AARCH64-subs") {

  QBDI::rword addr = genASM("subs X17, X28, X8\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "SUBSXrs", addr,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ true,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 17,
                    "X17", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 28,
                    "X28", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 8,
                    0, 8, "X8", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 8,
                    0, -1, nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_WRITE);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_AARCH64-cmp") {

  QBDI::rword addr = genASM("cmp X28, X8\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "SUBSXrs", addr,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ true,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_SEG, QBDI::OPERANDFLAG_NONE, 0, 8, 0, -1,
                    "XZR", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 28,
                    "X28", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 8,
                    0, 8, "X8", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 8,
                    0, -1, nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_WRITE);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_AARCH64-adr") {

  QBDI::rword addr = genASM("adr X28, #127\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "ADR", addr,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 28,
                    "X28", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_PCREL, 127, 2, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_AARCH64-cbz") {

  QBDI::rword addr = genASM("cbz x28, label\nnop\nnop\nnop\nlabel: nop\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "CBZX", addr,
          /* instSize */ 4, /* affectControlFlow */ true, /* isBranch */ true,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 28,
                    "X28", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_PCREL, 4, 2, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_AARCH64-bcc") {

  QBDI::rword addr = genASM("b.le label\nnop\nnop\nnop\nlabel: nop\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "Bcc", addr,
          /* instSize */ 4, /* affectControlFlow */ true, /* isBranch */ true,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_LESS_EQUALS});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_PCREL, 4, 2, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_READ);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_AARCH64-ldp") {

  QBDI::rword addr = genASM("ldp x0, x1, [x2, #8]\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "LDPXi", addr,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 16, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 0, "X0",
                    QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 1, "X1",
                    QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 2, "X2",
                    QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 1, 8,
                    0, -1, nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_AARCH64-ldp_pre") {

  QBDI::rword addr = genASM("ldp x0, x1, [x2, #8]!\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "LDPXpre", addr,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 16, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 0, "X0",
                    QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 1, "X1",
                    QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 2, "X2",
                    QBDI::REGISTER_READ_WRITE},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 1, 8,
                    0, -1, nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_AARCH64-ldp_post") {

  QBDI::rword addr = genASM("ldp x0, x1, [x2], #8\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "LDPXpost", addr,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 16, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 0, "X0",
                    QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 1, "X1",
                    QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 2, "X2",
                    QBDI::REGISTER_READ_WRITE},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 1, 8,
                    0, -1, nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_AARCH64-st1Three") {

  QBDI::rword addr = genASM("st1  { v0.8b, v1.8b, v2.8b }, [x0], #24\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "ST1Threev8b_POST", addr,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ true,
          /* loadSize */ 0, /* storeSize */ 24,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 0, "D0",
                    QBDI::REGISTER_READ},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 16,
                    "D1", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 32,
                    "D2", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 0, "X0",
                    QBDI::REGISTER_READ_WRITE},
                   {QBDI::OPERAND_SEG, QBDI::OPERANDFLAG_NONE, 0, 8, 0, -1,
                    "XZR", QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_AARCH64-ld4") {

  QBDI::rword addr = genASM("ld4  { v0.b, v1.b, v2.b, v3.b }[3], [x0], #4\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "LD4i8_POST", addr,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 4, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_NONE, 0, 16, 0, 0,
                    "Q0", QBDI::REGISTER_READ_WRITE},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_NONE, 0, 16, 0, 16,
                    "Q1", QBDI::REGISTER_READ_WRITE},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_NONE, 0, 16, 0, 32,
                    "Q2", QBDI::REGISTER_READ_WRITE},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_NONE, 0, 16, 0, 48,
                    "Q3", QBDI::REGISTER_READ_WRITE},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 3, 8,
                    0, -1, nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 0, "X0",
                    QBDI::REGISTER_READ_WRITE},
                   {QBDI::OPERAND_SEG, QBDI::OPERANDFLAG_NONE, 0, 8, 0, -1,
                    "XZR", QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_UNUSED);
}
