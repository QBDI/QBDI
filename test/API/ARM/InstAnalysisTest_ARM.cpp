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

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-CachedInst") {

  QBDI::rword addr = genASM("bic r0, r0, 0xff\n");

  CHECK(vm.getCachedInstAnalysis(addr) != nullptr);

  vm.clearAllCache();

  CHECK(vm.getCachedInstAnalysis(addr) == nullptr);

  vm.precacheBasicBlock(addr);

  CHECK(vm.getCachedInstAnalysis(addr) != nullptr);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-bx_lr") {

  QBDI::rword addr = genASM("bx lr\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "BX_RET", addr, QBDI::CPUMode::ARM,
          /* instSize */ 4, /* affectControlFlow */ true, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ true, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 4, 0, 14,
                    "LR", QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-beq") {

  QBDI::rword addr = genASM("beq label\nnop\nnop\nnop\nlabel: nop\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "Bcc", addr, QBDI::CPUMode::ARM,
          /* instSize */ 4, /* affectControlFlow */ true, /* isBranch */ true,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_EQUALS});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_PCREL, 8, 2, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_READ);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-add") {

  QBDI::rword addr = genASM("add r1, r2, r1\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "ADDrr", addr, QBDI::CPUMode::ARM,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 1, "R1",
                    QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 2, "R2",
                    QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 1, "R1",
                    QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-addsne") {

  QBDI::rword addr = genASM("addsne r1, r2, r1\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "ADDrr", addr, QBDI::CPUMode::ARM,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NOT_EQUALS});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 1, "R1",
                    QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 2, "R2",
                    QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 1, "R1",
                    QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_READ_WRITE);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-pop1") {

  QBDI::rword addr = genASM("pop {r0-r4}\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "LDMIA_UPD", addr, QBDI::CPUMode::ARM,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 20, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 13,
                    "SP", QBDI::REGISTER_READ_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 0, "R0", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 1, "R1", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 2, "R2", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 3, "R3", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 4, "R4", QBDI::REGISTER_WRITE},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-pop2") {

  QBDI::rword addr = genASM("pop {r0-r4,r8-r12}\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "LDMIA_UPD", addr, QBDI::CPUMode::ARM,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 40, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 13,
                    "SP", QBDI::REGISTER_READ_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 0, "R0", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 1, "R1", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 2, "R2", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 3, "R3", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 4, "R4", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 8, "R8", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 9, "R9", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 10, "R10", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 11, "R11", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 12, "R12", QBDI::REGISTER_WRITE},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-pop3") {

  QBDI::rword addr = genASM("pop {r0-r12,lr,pc}\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "LDMIA_UPD", addr, QBDI::CPUMode::ARM,
          /* instSize */ 4, /* affectControlFlow */ true, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 60, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 13,
                    "SP", QBDI::REGISTER_READ_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 0, "R0", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 1, "R1", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 2, "R2", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 3, "R3", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 4, "R4", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 5, "R5", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 6, "R6", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 7, "R7", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 8, "R8", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 9, "R9", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 10, "R10", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 11, "R11", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 12, "R12", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 14, "LR", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 15, "PC", QBDI::REGISTER_WRITE},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-push1") {

  QBDI::rword addr = genASM("push {r0-r4}\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "STMDB_UPD", addr, QBDI::CPUMode::ARM,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ true,
          /* loadSize */ 0, /* storeSize */ 20,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 13,
                    "SP", QBDI::REGISTER_READ_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 0, "R0", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 1, "R1", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 2, "R2", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 3, "R3", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 4, "R4", QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-push2") {

  QBDI::rword addr = genASM("push {r0-r4,r8-r12}\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "STMDB_UPD", addr, QBDI::CPUMode::ARM,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ true,
          /* loadSize */ 0, /* storeSize */ 40,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 13,
                    "SP", QBDI::REGISTER_READ_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 0, "R0", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 1, "R1", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 2, "R2", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 3, "R3", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 4, "R4", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 8, "R8", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 9, "R9", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 10, "R10", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 11, "R11", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 12, "R12", QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-push3") {

  QBDI::rword addr = genASM("push {r0-r12,lr,pc}\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "STMDB_UPD", addr, QBDI::CPUMode::ARM,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ true,
          /* loadSize */ 0, /* storeSize */ 60,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 13,
                    "SP", QBDI::REGISTER_READ_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 0, "R0", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 1, "R1", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 2, "R2", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 3, "R3", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 4, "R4", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 5, "R5", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 6, "R6", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 7, "R7", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 8, "R8", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 9, "R9", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 10, "R10", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 11, "R11", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 12, "R12", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 14, "LR", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 15, "PC", QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-sadd8") {

  QBDI::rword addr = genASM("sadd8 r0, r1, r2\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "SADD8", addr, QBDI::CPUMode::ARM,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 0, "R0",
                    QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 1, "R1",
                    QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 2, "R2",
                    QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_WRITE);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-mrs") {

  QBDI::rword addr = genASM("mrs r0, APSR\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "MRS", addr, QBDI::CPUMode::ARM,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 0, "R0",
                    QBDI::REGISTER_WRITE},
               },
               QBDI::REGISTER_READ);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-msr") {

  QBDI::rword addr = genASM("msr APSR_nzcvqg, r0\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "MSR", addr, QBDI::CPUMode::ARM,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 12,
                    4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 0, "R0",
                    QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_WRITE);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-msri") {

  QBDI::rword addr = genASM("msr APSR_nzcvqg, #0\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "MSRi", addr, QBDI::CPUMode::ARM,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 12,
                    4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, -1, nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_WRITE);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-vldm1") {

  QBDI::rword addr = genASM("vldmia	r0, {s0, s1, s2, s3}\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "VLDMSIA", addr, QBDI::CPUMode::ARM,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 16, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 0, "R0",
                    QBDI::REGISTER_READ},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 0, "S0", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 4, "S1", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 8, "S2", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 12, "S3", QBDI::REGISTER_WRITE},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-vldm2") {

  QBDI::rword addr = genASM("vldmia	r0, {d0, d1, d2, d3}\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "VLDMDIA", addr, QBDI::CPUMode::ARM,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 32, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 0, "R0",
                    QBDI::REGISTER_READ},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 8,
                    0, 0, "D0", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 8,
                    0, 8, "D1", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 8,
                    0, 16, "D2", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 8,
                    0, 24, "D3", QBDI::REGISTER_WRITE},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-vldm3") {

  QBDI::rword addr = genASM("vldmia	r0!, {d0, d1, d2, d3}\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "VLDMDIA_UPD", addr, QBDI::CPUMode::ARM,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 32, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 0, "R0",
                    QBDI::REGISTER_READ_WRITE},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 8,
                    0, 0, "D0", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 8,
                    0, 8, "D1", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 8,
                    0, 16, "D2", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 8,
                    0, 24, "D3", QBDI::REGISTER_WRITE},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-vstm1") {

  QBDI::rword addr = genASM("vstmia	r0, {s0, s1, s2, s3}\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "VSTMSIA", addr, QBDI::CPUMode::ARM,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ true,
          /* loadSize */ 0, /* storeSize */ 16,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 0, "R0",
                    QBDI::REGISTER_READ},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 0, "S0", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 4, "S1", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 8, "S2", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 4,
                    0, 12, "S3", QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-vstm2") {

  QBDI::rword addr = genASM("vstmia	r0, {d0, d1, d2, d3}\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "VSTMDIA", addr, QBDI::CPUMode::ARM,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ true,
          /* loadSize */ 0, /* storeSize */ 32,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 0, "R0",
                    QBDI::REGISTER_READ},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 8,
                    0, 0, "D0", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 8,
                    0, 8, "D1", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 8,
                    0, 16, "D2", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 8,
                    0, 24, "D3", QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-vstm3") {

  QBDI::rword addr = genASM("vstmia	r0!, {d0, d1, d2, d3}\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "VSTMDIA_UPD", addr, QBDI::CPUMode::ARM,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ false, /* mayStore */ true,
          /* loadSize */ 0, /* storeSize */ 32,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 0, "R0",
                    QBDI::REGISTER_READ_WRITE},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 8,
                    0, 0, "D0", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 8,
                    0, 8, "D1", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 8,
                    0, 16, "D2", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 8,
                    0, 24, "D3", QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-ldrbt_imm") {

  QBDI::rword addr = genASM("ldrbt r0, [r10], #-0x40\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "LDRBT_POST_IMM", addr, QBDI::CPUMode::ARM,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 1, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 0, "R0",
                    QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 10,
                    "R10", QBDI::REGISTER_READ_WRITE},
                   {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, -0x40, 4, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-ldrbt_reg1") {

  QBDI::rword addr = genASM("ldrbt r0, [r10], -r9, ror #3\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "LDRBT_POST_REG", addr, QBDI::CPUMode::ARM,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 1, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
      {
          {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 0, "R0",
           QBDI::REGISTER_WRITE},
          {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 10, "R10",
           QBDI::REGISTER_READ_WRITE},
          {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 9, "R9",
           QBDI::REGISTER_READ},
          {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR,
           (/* IndexModePost */ 2 << 16) | (/* ShiftOpc::ror */ 4 << 13) |
               (/* sub */ 1 << 12) | (/* shift */ 3),
           4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
      },
      QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-ldrbt_reg2") {

  QBDI::rword addr = genASM("ldrbt r0, [r10], r9, ror #3\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "LDRBT_POST_REG", addr, QBDI::CPUMode::ARM,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ true, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 1, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
      {
          {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 0, "R0",
           QBDI::REGISTER_WRITE},
          {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 10, "R10",
           QBDI::REGISTER_READ_WRITE},
          {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 9, "R9",
           QBDI::REGISTER_READ},
          {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR,
           (/* IndexModePost */ 2 << 16) | (/* ShiftOpc::ror */ 4 << 13) |
               (/* sub */ 0 << 12) | (/* shift */ 3),
           4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
      },
      QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-ldrex") {

  QBDI::rword addr = genASM("ldrex r0, [r10]\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "LDREX", addr, QBDI::CPUMode::ARM,
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
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-ldrexb") {

  QBDI::rword addr = genASM("ldrexb r0, [r10]\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "LDREXB", addr, QBDI::CPUMode::ARM,
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

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-ldrexd") {

  QBDI::rword addr = genASM("ldrexd r0, r1, [r10]\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "LDREXD", addr, QBDI::CPUMode::ARM,
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

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-strex") {

  QBDI::rword addr = genASM("strex r5, r0, [r10]\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "STREX", addr, QBDI::CPUMode::ARM,
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
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-strexb") {

  QBDI::rword addr = genASM("strexb r5, r0, [r10]\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "STREXB", addr, QBDI::CPUMode::ARM,
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

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-strexd") {

  QBDI::rword addr = genASM("strexd r5, r0, r1, [r10]\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "STREXD", addr, QBDI::CPUMode::ARM,
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

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-cmp") {

  QBDI::rword addr = genASM("cmp r5, #16\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "CMPri", addr, QBDI::CPUMode::ARM,
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

TEST_CASE_METHOD(APITest, "InstAnalysisTest_ARM-tst") {

  QBDI::rword addr = genASM("tst r5, #16\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "TSTri", addr, QBDI::CPUMode::ARM,
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
