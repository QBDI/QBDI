/*
 * This file is part of QBDI.
 *
 * Copyright 2017 Quarkslab
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
#include "InstAnalysisTest.h"

#include "inttypes.h"
#include <algorithm>
#include <string>
#include <sstream>

#include "Platform.h"
#include "Memory.hpp"

QBDI::rword InstAnalysisTest::writeASM(const char* source) {
    std::ostringstream      finalSource;

    finalSource << source << "\n"
                << "ret\n";

    objects.emplace_back(finalSource.str().c_str());

    return (QBDI::rword) objects.back().getCode().data();
}

struct ExpectedInstAnalysis {
    std::string  mnemonic;
    QBDI::rword  address;
    uint32_t     instSize;
    bool         affectControlFlow;
    bool         isBranch;
    bool         isCall;
    bool         isReturn;
    bool         isCompare;
    bool         isPredicable;
    bool         mayLoad;
    bool         mayStore;
    uint32_t     loadSize;
    uint32_t     storeSize;
};

/*
static void debugOperand(const QBDI::InstAnalysis* ana) {
    if ((ana->analysisType & QBDI::ANALYSIS_OPERANDS) == QBDI::ANALYSIS_OPERANDS) {
        for (int i = 0; i < ana->numOperands; i++) {
            const QBDI::OperandAnalysis& op = ana->operands[i];
            WARN("- type: " << op.type
              << ", flag: " << op.flag
              << ", value: " << op.value
              << ", size: " << (int) op.size
              << ", regOff : " << (int) op.regOff
              << ", regCtxIdx: " << op.regCtxIdx
              << ", regName: " << (op.regName == nullptr ? "nullptr": op.regName)
              << ", regAccess: " << (op.regAccess & QBDI::REGISTER_READ ? "r": "-")
                                 << (op.regAccess & QBDI::REGISTER_WRITE ? "w": "-"));
        }
    }
}// */

static void checkOperand(const QBDI::InstAnalysis* ana, const std::vector<QBDI::OperandAnalysis> expecteds, QBDI::RegisterAccessType flagsAccess) {

    CHECKED_IF((ana->analysisType & QBDI::ANALYSIS_OPERANDS) == QBDI::ANALYSIS_OPERANDS) {
        CHECK(flagsAccess == ana->flagsAccess);
        CHECK(expecteds.size() == ana->numOperands);
        for (int i = 0; i < std::min<unsigned>(ana->numOperands, expecteds.size()); i++) {
            const QBDI::OperandAnalysis& expect = expecteds[i];
            const QBDI::OperandAnalysis& op = ana->operands[i];
            CHECK(expect.type == op.type);
            CHECK(expect.flag == op.flag);
            if (op.type == QBDI::OPERAND_IMM || expect.value != 0)
                CHECK(expect.value == op.value);
            CHECK(expect.size == op.size);
            CHECK(expect.regOff == op.regOff);
            CHECK(expect.regCtxIdx == op.regCtxIdx);
            CHECK(expect.regAccess == op.regAccess);

            if (expect.regName == nullptr) {
                CHECK(((void*) op.regName) == nullptr);
            } else {
                CHECKED_IF(((void*) op.regName) != nullptr)
                    CHECK(strcmp(expect.regName, op.regName) == 0);
            }
        }
    }
}

static void checkInst(const QBDI::InstAnalysis* ana, const ExpectedInstAnalysis expected) {

    CHECKED_IF((ana->analysisType & QBDI::ANALYSIS_INSTRUCTION) == QBDI::ANALYSIS_INSTRUCTION) {
        CHECK(ana->mnemonic          == expected.mnemonic);
        CHECK(ana->address           == expected.address);
        CHECK(ana->instSize          == expected.instSize);
        CHECK(ana->affectControlFlow == expected.affectControlFlow);
        CHECK(ana->isBranch          == expected.isBranch);
        CHECK(ana->isCall            == expected.isCall);
        CHECK(ana->isReturn          == expected.isReturn);
        CHECK(ana->isCompare         == expected.isCompare);
        CHECK(ana->isPredicable      == expected.isPredicable);
        CHECK(ana->mayLoad           == expected.mayLoad);
        CHECK(ana->mayStore          == expected.mayStore);
        CHECK(ana->loadSize          == expected.loadSize);
        CHECK(ana->storeSize         == expected.storeSize);
    }
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86_64-CachedInst") {

    QBDI::rword addr = writeASM("leaq (%rax), %rbx\n");

    CHECK(vm.getCachedInstAnalysis(addr) == nullptr);

    vm.precacheBasicBlock(addr);

    CHECK(vm.getCachedInstAnalysis(addr) != nullptr);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86_64-lea") {

    QBDI::rword addr = writeASM("leaq (%rax), %rbx\n");
    vm.precacheBasicBlock(addr);

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "LEA64r", addr,
          /* instSize */ 3, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 1, "RBX", QBDI::REGISTER_WRITE},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 8, 0, 0, "RAX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 1, 8, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 8, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_UNDEFINED_EFFECT, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
        }, QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86_64-movrm") {

    QBDI::rword addr = writeASM("movq 0x45(%rax,%rdx,4), %rbx\n");
    vm.precacheBasicBlock(addr);

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "MOV64rm", addr,
          /* instSize */ 5, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 8, /* storeSize */ 0});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 1, "RBX", QBDI::REGISTER_WRITE},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 0, "RAX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 4, 8, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 3, "RDX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0x45, 8, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
        }, QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86_64-movrm-seg") {

    QBDI::rword addr = writeASM("movq %gs:0x45(%rax,%rdx,4), %rbx\n");
    vm.precacheBasicBlock(addr);

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "MOV64rm", addr,
          /* instSize */ 6, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 8, /* storeSize */ 0});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 1, "RBX", QBDI::REGISTER_WRITE},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 0, "RAX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 4, 8, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 3, "RDX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0x45, 8, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_SEG, QBDI::OPERANDFLAG_ADDR, 0, 2, 0, -1, "GS", QBDI::REGISTER_READ},
        }, QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86_64-addmi") {

    QBDI::rword addr = writeASM("addq	$0x4157, (%rcx)\n");
    vm.precacheBasicBlock(addr);

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "ADD64mi32", addr,
          /* instSize */ 7, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ true,
          /* loadSize */ 8, /* storeSize */ 8});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 2, "RCX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 1, 8, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_NONE, 0x4157, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
        }, QBDI::REGISTER_WRITE);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86_64-movrr") {

    QBDI::rword addr = writeASM("mov %rcx, %rbx\n");
    vm.precacheBasicBlock(addr);

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "MOV64rr", addr,
          /* instSize */ 3, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 1, "RBX", QBDI::REGISTER_WRITE},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 2, "RCX", QBDI::REGISTER_READ},
        }, QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86_64-movrr8") {

    QBDI::rword addr = writeASM("mov %ch, %bl\n");
    vm.precacheBasicBlock(addr);

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "MOV8rr", addr,
          /* instSize */ 2, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 1, 0, 1, "BL", QBDI::REGISTER_WRITE},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 1, 8, 2, "CH", QBDI::REGISTER_READ},
        }, QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86_64-xchgrr") {

    QBDI::rword addr = writeASM("xchg %rcx, %rbx\n");
    vm.precacheBasicBlock(addr);

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "XCHG64rr", addr,
          /* instSize */ 3, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 1, "RBX", QBDI::REGISTER_WRITE},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 2, "RCX", QBDI::REGISTER_WRITE},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 1, "RBX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 2, "RCX", QBDI::REGISTER_READ},
        }, QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86_64-movsb") {

    QBDI::rword addr = writeASM("movsb\n");
    vm.precacheBasicBlock(addr);

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "MOVSB", addr,
          /* instSize */ 1, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ true,
          /* loadSize */ 1, /* storeSize */ 1});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 5, "RDI", QBDI::REGISTER_READ},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 4, "RSI", QBDI::REGISTER_READ},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 4, 0, 5, "EDI", QBDI::REGISTER_READ_WRITE},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 4, 0, 4, "ESI", QBDI::REGISTER_READ_WRITE},
        }, QBDI::REGISTER_READ);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86_64-cmpmr") {

    QBDI::rword addr = writeASM("cmpq %rcx, (%rax,%rdx)\n");
    vm.precacheBasicBlock(addr);

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "CMP64mr", addr,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ true,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 8, /* storeSize */ 0});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 0, "RAX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 1, 8, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 3, "RDX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 2, "RCX", QBDI::REGISTER_READ},
        }, QBDI::REGISTER_WRITE);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86_64-cmprm") {

    QBDI::rword addr = writeASM("cmpq (%rax,%rdx), %rcx\n");
    vm.precacheBasicBlock(addr);

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "CMP64rm", addr,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ true,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 8, /* storeSize */ 0});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 2, "RCX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 0, "RAX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 1, 8, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 3, "RDX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
        }, QBDI::REGISTER_WRITE);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86_64-ret") {

    QBDI::rword addr = writeASM("retq\n");
    vm.precacheBasicBlock(addr);

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "RETQ", addr,
          /* instSize */ 1, /* affectControlFlow */ true, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ true, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 8, /* storeSize */ 0});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT | QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 15, "RSP", QBDI::REGISTER_READ},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 8, 0, 15, "RSP", QBDI::REGISTER_READ_WRITE},
        }, QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86_64-call") {

    QBDI::rword addr = writeASM("call test_custom_call\ntest_custom_call:\n");
    vm.precacheBasicBlock(addr);

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "CALL64pcrel32", addr,
          /* instSize */ 5, /* affectControlFlow */ true, /* isBranch */ false,
          /* isCall */ true, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ true,
          /* loadSize */ 0, /* storeSize */ 8});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_PCREL, 0, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 8, 0, 15, "RSP", QBDI::REGISTER_READ_WRITE},
            {QBDI::OPERAND_SEG, QBDI::OPERANDFLAG_IMPLICIT, 0, 8, 0, -1, "SSP", QBDI::REGISTER_READ},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT | QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 15, "RSP", QBDI::REGISTER_READ},
        }, QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86_64-callr") {

    QBDI::rword addr = writeASM("callq *%rax\n");
    vm.precacheBasicBlock(addr);

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "CALL64r", addr,
          /* instSize */ 2, /* affectControlFlow */ true, /* isBranch */ false,
          /* isCall */ true, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ true,
          /* loadSize */ 0, /* storeSize */ 8});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 0, "RAX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 8, 0, 15, "RSP", QBDI::REGISTER_READ_WRITE},
            {QBDI::OPERAND_SEG, QBDI::OPERANDFLAG_IMPLICIT, 0, 8, 0, -1, "SSP", QBDI::REGISTER_READ},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT | QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 15, "RSP", QBDI::REGISTER_READ},
        }, QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86_64-callm") {

    QBDI::rword addr = writeASM("callq *0xa(%rax)\n");
    vm.precacheBasicBlock(addr);

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "CALL64m", addr,
          /* instSize */ 3, /* affectControlFlow */ true, /* isBranch */ false,
          /* isCall */ true, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ true,
          /* loadSize */ 8, /* storeSize */ 8});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 0, "RAX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 1, 8, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0xa, 8, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 8, 0, 15, "RSP", QBDI::REGISTER_READ_WRITE},
            {QBDI::OPERAND_SEG, QBDI::OPERANDFLAG_IMPLICIT, 0, 8, 0, -1, "SSP", QBDI::REGISTER_READ},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT | QBDI::OPERANDFLAG_ADDR, 0, 8, 0, 15, "RSP", QBDI::REGISTER_READ},
        }, QBDI::REGISTER_UNUSED);
}
