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

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-CachedInst") {

  QBDI::rword addr = genASM("leaq (%rax), %rbx\n");

  CHECK(vm.getCachedInstAnalysis(addr) != nullptr);

  vm.clearAllCache();

  CHECK(vm.getCachedInstAnalysis(addr) == nullptr);

  vm.precacheBasicBlock(addr);

  CHECK(vm.getCachedInstAnalysis(addr) != nullptr);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-lea") {

  QBDI::rword addr = genASM("leaq (%rax), %rbx\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "LEA64r", addr,
          /* instSize */ 3, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 1,
                    "RBX", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 8,
                    0, 0, "RAX", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 1, 8,
                    0, -1, nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_UNDEFINED_EFFECT,
                    0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 8,
                    0, -1, nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_UNDEFINED_EFFECT,
                    0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-lea-same-reg") {

  QBDI::rword addr = genASM("leaq (%rax,%rax), %rax\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "LEA64r", addr,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 0,
                    "RAX", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 8,
                    0, 0, "RAX", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 1, 8,
                    0, -1, nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 8,
                    0, 0, "RAX", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 8,
                    0, -1, nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_UNDEFINED_EFFECT,
                    0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-movrm") {

  QBDI::rword addr = genASM("movq 0x45(%rax,%rdx,4), %rbx\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "MOV64rm", addr,
          /* instSize */ 5, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 8, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 1,
                    "RBX", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 0,
                    "RAX", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 4, 8, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 3,
                    "RDX", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0x45, 8, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-movrm-seg") {

  QBDI::rword addr = genASM("movq %gs:0x45(%rax,%rdx,4), %rbx\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "MOV64rm", addr,
          /* instSize */ 6, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 8, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 1,
                    "RBX", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 0,
                    "RAX", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 4, 8, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 3,
                    "RDX", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0x45, 8, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_SEG, QBDI::OPERANDFLAG_ADDR, 0, 2, 0, -1,
                    "GS", QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-addmi") {

  QBDI::rword addr = genASM("addq	$0x4157, (%rcx)\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "ADD64mi32", addr,
          /* instSize */ 7, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ true,
          /* loadSize */ 8, /* storeSize */ 8,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 2,
                    "RCX", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 1, 8, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_NONE, 0x4157, 4, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_WRITE);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-movrr") {

  QBDI::rword addr = genASM("mov %rcx, %rbx\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "MOV64rr", addr,
          /* instSize */ 3, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 1,
                    "RBX", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 2,
                    "RCX", QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-movrr8") {

  QBDI::rword addr = genASM("mov %ch, %bl\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "MOV8rr", addr,
          /* instSize */ 2, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 1, 0, 1, "BL",
                    QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 1, 8, 2, "CH",
                    QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-xchgrr") {

  QBDI::rword addr = genASM("xchg %rcx, %rbx\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "XCHG64rr", addr,
          /* instSize */ 3, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 1,
                    "RBX", QBDI::REGISTER_READ_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 2,
                    "RCX", QBDI::REGISTER_READ_WRITE},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-addrr") {

  QBDI::rword addr = genASM("add %rcx, %rbx\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "ADD64rr", addr,
          /* instSize */ 3, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 1,
                    "RBX", QBDI::REGISTER_READ_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 2,
                    "RCX", QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_WRITE);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-movsb") {

  QBDI::rword addr = genASM("movsb\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "MOVSB", addr,
          /* instSize */ 1, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ true,
          /* loadSize */ 1, /* storeSize */ 1,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 5,
                    "RDI", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 4,
                    "RSI", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 4, 0, 5,
                    "EDI", QBDI::REGISTER_READ_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 4, 0, 4,
                    "ESI", QBDI::REGISTER_READ_WRITE},
               },
               QBDI::REGISTER_READ);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-cmpsb") {

  QBDI::rword addr = genASM("cmpsb\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "CMPSB", addr,
          /* instSize */ 1, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 1, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 5,
                    "RDI", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 4,
                    "RSI", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 4, 0, 5,
                    "EDI", QBDI::REGISTER_READ_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 4, 0, 4,
                    "ESI", QBDI::REGISTER_READ_WRITE},
               },
               QBDI::REGISTER_READ_WRITE);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-cmpmr") {

  QBDI::rword addr = genASM("cmpq %rcx, (%rax,%rdx)\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "CMP64mr", addr,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ true,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 8, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 0,
                    "RAX", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 1, 8, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 3,
                    "RDX", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 2,
                    "RCX", QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_WRITE);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-cmprm") {

  QBDI::rword addr = genASM("cmpq (%rax,%rdx), %rcx\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "CMP64rm", addr,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ true,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 8, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 2,
                    "RCX", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 0,
                    "RAX", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 1, 8, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 3,
                    "RDX", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_WRITE);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-ret") {

  QBDI::rword addr = genASM("retq\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "RET64", addr,
          /* instSize */ 1, /* affectControlFlow */ true, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ true, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 8, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 8, 0, 15,
                    "RSP", QBDI::REGISTER_READ_WRITE},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-call") {

  QBDI::rword addr = genASM("call test_custom_call\ntest_custom_call:\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "CALL64pcrel32", addr,
          /* instSize */ 5, /* affectControlFlow */ true, /* isBranch */ false,
          /* isCall */ true, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ true,
          /* loadSize */ 0, /* storeSize */ 8,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_PCREL, 0, 4, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 8, 0, 15,
                    "RSP", QBDI::REGISTER_READ_WRITE},
                   {QBDI::OPERAND_SEG, QBDI::OPERANDFLAG_IMPLICIT, 0, 8, 0, -1,
                    "SSP", QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-callr") {

  QBDI::rword addr = genASM("callq *%rax\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "CALL64r", addr,
          /* instSize */ 2, /* affectControlFlow */ true, /* isBranch */ false,
          /* isCall */ true, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ true,
          /* loadSize */ 0, /* storeSize */ 8,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 0,
                    "RAX", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 8, 0, 15,
                    "RSP", QBDI::REGISTER_READ_WRITE},
                   {QBDI::OPERAND_SEG, QBDI::OPERANDFLAG_IMPLICIT, 0, 8, 0, -1,
                    "SSP", QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-callm") {

  QBDI::rword addr = genASM("callq *0xa(%rax)\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "CALL64m", addr,
          /* instSize */ 3, /* affectControlFlow */ true, /* isBranch */ false,
          /* isCall */ true, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ true,
          /* loadSize */ 8, /* storeSize */ 8,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 0,
                    "RAX", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 1, 8, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0xa, 8, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 8, 0, 15,
                    "RSP", QBDI::REGISTER_READ_WRITE},
                   {QBDI::OPERAND_SEG, QBDI::OPERANDFLAG_IMPLICIT, 0, 8, 0, -1,
                    "SSP", QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-jmpi") {

  QBDI::rword addr = genASM("jmp test_jmp\ntest_jmp:\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "JMP_4", addr,
          /* instSize */ 5, /* affectControlFlow */ true, /* isBranch */ true,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_PCREL, 0, 4, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-je") {

  QBDI::rword addr = genASM("je test_jmp\ntest_jmp:\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "JCC_4", addr,
          /* instSize */ 6, /* affectControlFlow */ true, /* isBranch */ true,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_EQUALS});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_PCREL, 0, 4, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_READ);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-jmpm") {

  QBDI::rword addr = genASM("jmpq *0xa(%rax)\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "JMP64m", addr,
          /* instSize */ 3, /* affectControlFlow */ true, /* isBranch */ true,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 8, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 0,
                    "RAX", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 1, 8, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0xa, 8, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-fldl") {

  QBDI::rword addr = genASM("fldl (%rax)\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "LD_F64m", addr,
          /* instSize */ 2, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 8, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 0,
                    "RAX", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 1, 8, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0x0, 8, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 2, 0, 0,
                    "FPCW", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 2, 0, 2,
                    "FPSW", QBDI::REGISTER_WRITE},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-fstps") {

  QBDI::rword addr = genASM("fstps (%rax)\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "ST_FP32m", addr,
          /* instSize */ 2, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ true,
          /* loadSize */ 0, /* storeSize */ 4,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 0,
                    "RAX", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 1, 8, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0x0, 8, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 2, 0, 0,
                    "FPCW", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 2, 0, 2,
                    "FPSW", QBDI::REGISTER_WRITE},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-movapd") {

  QBDI::rword addr = genASM("movapd (%rax), %xmm1\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "MOVAPDrm", addr,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 16, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
      {
          {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_NONE, 0, 16, 0,
           offsetof(QBDI::FPRState, xmm1), "XMM1", QBDI::REGISTER_WRITE},
          {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 0, "RAX",
           QBDI::REGISTER_READ},
          {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 1, 8, 0, -1, nullptr,
           QBDI::REGISTER_UNUSED},
          {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr,
           QBDI::REGISTER_UNUSED},
          {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0x0, 8, 0, -1, nullptr,
           QBDI::REGISTER_UNUSED},
          {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr,
           QBDI::REGISTER_UNUSED},
      },
      QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-paddb") {

  QBDI::rword addr = genASM("paddb %mm1, %mm0\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "MMX_PADDBrr", addr,
          /* instSize */ 3, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 32,
                    "MM0", QBDI::REGISTER_READ_WRITE},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 48,
                    "MM1", QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-vpaddb") {

  QBDI::rword addr = genASM("vpaddb %xmm2, %xmm1, %xmm0\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "VPADDBrr", addr,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_NONE, 0, 16, 0, 160,
                    "XMM0", QBDI::REGISTER_WRITE},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_NONE, 0, 16, 0, 176,
                    "XMM1", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_NONE, 0, 16, 0, 192,
                    "XMM2", QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-xlatb") {

  QBDI::rword addr = genASM("xlatb\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "XLAT", addr,
          /* instSize */ 1, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 1, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 1, 0, 0,
                    "AL", QBDI::REGISTER_READ_WRITE},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 4, 0, 1,
                    "EBX", QBDI::REGISTER_READ},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-movdir64b") {

  QBDI::rword addr = genASM("movdir64b 0xc(%rax), %rcx\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "MOVDIR64B64", addr,
          /* instSize */ 6, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ true,
          /* loadSize */ 512, /* storeSize */ 512,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 2,
                    "RCX", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 0,
                    "RAX", QBDI::REGISTER_READ},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 1, 8, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0xc, 8, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-loop") {

  QBDI::rword addr = genASM("target:\n    loop target\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "LOOP", addr,
          /* instSize */ 2, /* affectControlFlow */ true, /* isBranch */ true,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NONE});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_PCREL, -2, 1, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 8, 0, 2,
                    "RCX", QBDI::REGISTER_READ_WRITE},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-loope") {

  QBDI::rword addr = genASM("target:\n    loope target\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "LOOPE", addr,
          /* instSize */ 2, /* affectControlFlow */ true, /* isBranch */ true,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_EQUALS});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_PCREL, -2, 1, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 8, 0, 2,
                    "RCX", QBDI::REGISTER_READ_WRITE},
               },
               QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64-loopne") {

  QBDI::rword addr = genASM("target:\n    loopne target\n");

  checkInst(
      vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
      ExpectedInstAnalysis{
          "LOOPNE", addr,
          /* instSize */ 2, /* affectControlFlow */ true, /* isBranch */ true,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0,
          /* condition */ QBDI::CONDITION_NOT_EQUALS});
  checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
               {
                   {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_PCREL, -2, 1, 0, -1,
                    nullptr, QBDI::REGISTER_UNUSED},
                   {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 8, 0, 2,
                    "RCX", QBDI::REGISTER_READ_WRITE},
               },
               QBDI::REGISTER_UNUSED);
}

// TEST_CASE_METHOD(APITest, "InstAnalysisTest_X86_64_tmp") {
//
//   QBDI::rword addr = genASM(getenv("TMP_INST"));
//
//   debugOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS));
// }
