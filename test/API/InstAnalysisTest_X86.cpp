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

    QBDI::rword addr = (QBDI::rword) objects.back().getCode().data();
    vm.precacheBasicBlock(addr);

    return addr;
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
    QBDI::ConditionType condition;
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
        for (unsigned i = 0; i < std::min<unsigned>(ana->numOperands, expecteds.size()); i++) {
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

            const std::string expectedRegName ((expect.regName != nullptr) ? expect.regName : "");
            const std::string foundRegName ((op.regName != nullptr) ? op.regName : "");
            CHECK(expectedRegName == foundRegName);

            if (expect.regName == nullptr || op.regName == nullptr)
                CHECK(expect.regName == op.regName);
        }
    }
}

static void checkInst(const QBDI::InstAnalysis* ana, const ExpectedInstAnalysis expected) {

    CHECKED_IF((ana->analysisType & QBDI::ANALYSIS_INSTRUCTION) == QBDI::ANALYSIS_INSTRUCTION) {
        CHECK(expected.mnemonic          == ana->mnemonic);
        CHECK(expected.address           == ana->address);
        CHECK(expected.instSize          == ana->instSize);
        CHECK(expected.affectControlFlow == ana->affectControlFlow);
        CHECK(expected.isBranch          == ana->isBranch);
        CHECK(expected.isCall            == ana->isCall);
        CHECK(expected.isReturn          == ana->isReturn);
        CHECK(expected.isCompare         == ana->isCompare);
        CHECK(expected.isPredicable      == ana->isPredicable);
        CHECK(expected.mayLoad           == ana->mayLoad);
        CHECK(expected.mayStore          == ana->mayStore);
        CHECK(expected.loadSize          == ana->loadSize);
        CHECK(expected.storeSize         == ana->storeSize);
        CHECK(expected.condition         == ana->condition);
    }
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-CachedInst") {

    QBDI::rword addr = writeASM("leal (%eax), %ebx\n");

    CHECK(vm.getCachedInstAnalysis(addr) != nullptr);

    vm.clearAllCache();

    CHECK(vm.getCachedInstAnalysis(addr) == nullptr);

    vm.precacheBasicBlock(addr);

    CHECK(vm.getCachedInstAnalysis(addr) != nullptr);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-lea") {

    QBDI::rword addr = writeASM("leal (%eax), %ebx\n");

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "LEA32r", addr,
          /* instSize */ 2, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0, /* condition */ QBDI::CONDITION_NONE});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 1, "EBX", QBDI::REGISTER_WRITE},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 0, "EAX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 1, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
        }, QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-lea-same-reg") {

    QBDI::rword addr = writeASM("leal (%eax,%eax), %eax\n");

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "LEA32r", addr,
          /* instSize */ 3, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0, /* condition */ QBDI::CONDITION_NONE});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 0, "EAX", QBDI::REGISTER_WRITE},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 0, "EAX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 1, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 0, "EAX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
        }, QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-movrm") {

    QBDI::rword addr = writeASM("movl 0x45(%eax,%edx,4), %ebx\n");

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "MOV32rm", addr,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 4, /* storeSize */ 0, /* condition */ QBDI::CONDITION_NONE});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 1, "EBX", QBDI::REGISTER_WRITE},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 0, "EAX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 4, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 3, "EDX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0x45, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
        }, QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-movrm-seg") {

    QBDI::rword addr = writeASM("movl %gs:0x45(%eax,%edx,4), %ebx\n");

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "MOV32rm", addr,
          /* instSize */ 5, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 4, /* storeSize */ 0, /* condition */ QBDI::CONDITION_NONE});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 1, "EBX", QBDI::REGISTER_WRITE},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 0, "EAX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 4, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 3, "EDX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0x45, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_SEG, QBDI::OPERANDFLAG_ADDR, 0, 2, 0, -1, "GS", QBDI::REGISTER_READ},
        }, QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-addmi") {

    QBDI::rword addr = writeASM("addl	$0x4157, (%ecx)\n");

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "ADD32mi", addr,
          /* instSize */ 6, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ true,
          /* loadSize */ 4, /* storeSize */ 4, /* condition */ QBDI::CONDITION_NONE});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 2, "ECX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 1, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_NONE, 0x4157, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
        }, QBDI::REGISTER_WRITE);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-movrr") {

    QBDI::rword addr = writeASM("mov %ecx, %ebx\n");

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "MOV32rr", addr,
          /* instSize */ 2, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0, /* condition */ QBDI::CONDITION_NONE});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 1, "EBX", QBDI::REGISTER_WRITE},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 2, "ECX", QBDI::REGISTER_READ},
        }, QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-movrr8") {

    QBDI::rword addr = writeASM("mov %ch, %bl\n");

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "MOV8rr", addr,
          /* instSize */ 2, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0, /* condition */ QBDI::CONDITION_NONE});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 1, 0, 1, "BL", QBDI::REGISTER_WRITE},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 1, 8, 2, "CH", QBDI::REGISTER_READ},
        }, QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-xchgrr") {

    QBDI::rword addr = writeASM("xchg %ecx, %ebx\n");

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "XCHG32rr", addr,
          /* instSize */ 2, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0, /* condition */ QBDI::CONDITION_NONE});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 1, "EBX", QBDI::REGISTER_READ_WRITE},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 2, "ECX", QBDI::REGISTER_READ_WRITE},
        }, QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-addrr") {

    QBDI::rword addr = writeASM("add %ecx, %ebx\n");

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "ADD32rr", addr,
          /* instSize */ 2, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0, /* condition */ QBDI::CONDITION_NONE});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 1, "EBX", QBDI::REGISTER_READ_WRITE},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 2, "ECX", QBDI::REGISTER_READ},
        }, QBDI::REGISTER_WRITE);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-movoa") {

    QBDI::rword addr = writeASM("mov %fs:0x0, %eax\n");

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "MOV32ao32", addr,
          /* instSize */ 6, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 4, /* storeSize */ 0, /* condition */ QBDI::CONDITION_NONE});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_SEG, QBDI::OPERANDFLAG_ADDR, 0, 2, 0, -1, "FS", QBDI::REGISTER_READ},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 4, 0, 0, "EAX", QBDI::REGISTER_WRITE},
        }, QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-movsb") {

    QBDI::rword addr = writeASM("movsb\n");

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "MOVSB", addr,
          /* instSize */ 1, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ true,
          /* loadSize */ 1, /* storeSize */ 1, /* condition */ QBDI::CONDITION_NONE});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 5, "EDI", QBDI::REGISTER_READ},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 4, "ESI", QBDI::REGISTER_READ},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 4, 0, 5, "EDI", QBDI::REGISTER_READ_WRITE},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 4, 0, 4, "ESI", QBDI::REGISTER_READ_WRITE},
        }, QBDI::REGISTER_READ);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-cmpsb") {

    QBDI::rword addr = writeASM("cmpsb\n");

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "CMPSB", addr,
          /* instSize */ 1, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 1, /* storeSize */ 0, /* condition */ QBDI::CONDITION_NONE});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 5, "EDI", QBDI::REGISTER_READ},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 4, "ESI", QBDI::REGISTER_READ},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 4, 0, 5, "EDI", QBDI::REGISTER_READ_WRITE},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 4, 0, 4, "ESI", QBDI::REGISTER_READ_WRITE},
        }, QBDI::REGISTER_READ_WRITE);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-cmpmr") {

    QBDI::rword addr = writeASM("cmpl %ecx, (%eax,%edx)\n");

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "CMP32mr", addr,
          /* instSize */ 3, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ true,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 4, /* storeSize */ 0, /* condition */ QBDI::CONDITION_NONE});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 0, "EAX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 1, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 3, "EDX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 2, "ECX", QBDI::REGISTER_READ},
        }, QBDI::REGISTER_WRITE);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-cmprm") {

    QBDI::rword addr = writeASM("cmpl (%eax,%edx), %ecx\n");

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "CMP32rm", addr,
          /* instSize */ 3, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ true,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 4, /* storeSize */ 0, /* condition */ QBDI::CONDITION_NONE});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 2, "ECX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 0, "EAX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 1, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 3, "EDX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
        }, QBDI::REGISTER_WRITE);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-ret") {

    QBDI::rword addr = writeASM("retl\n");

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "RETL", addr,
          /* instSize */ 1, /* affectControlFlow */ true, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ true, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 4, /* storeSize */ 0, /* condition */ QBDI::CONDITION_NONE});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 4, 0, 7, "ESP", QBDI::REGISTER_READ_WRITE},
        }, QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-call") {

    QBDI::rword addr = writeASM("call test_custom_call\ntest_custom_call:\n");

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "CALLpcrel32", addr,
          /* instSize */ 5, /* affectControlFlow */ true, /* isBranch */ false,
          /* isCall */ true, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ true,
          /* loadSize */ 0, /* storeSize */ 4, /* condition */ QBDI::CONDITION_NONE});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_PCREL, 0, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 4, 0, 7, "ESP", QBDI::REGISTER_READ_WRITE},
            {QBDI::OPERAND_SEG, QBDI::OPERANDFLAG_IMPLICIT, 0, 4, 0, -1, "SSP", QBDI::REGISTER_READ},
        }, QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-callr") {

    QBDI::rword addr = writeASM("calll *%eax\n");

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "CALL32r", addr,
          /* instSize */ 2, /* affectControlFlow */ true, /* isBranch */ false,
          /* isCall */ true, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ true,
          /* loadSize */ 0, /* storeSize */ 4, /* condition */ QBDI::CONDITION_NONE});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 0, "EAX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 4, 0, 7, "ESP", QBDI::REGISTER_READ_WRITE},
            {QBDI::OPERAND_SEG, QBDI::OPERANDFLAG_IMPLICIT, 0, 4, 0, -1, "SSP", QBDI::REGISTER_READ},
        }, QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-callm") {

    QBDI::rword addr = writeASM("calll *0xa(%eax)\n");

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "CALL32m", addr,
          /* instSize */ 3, /* affectControlFlow */ true, /* isBranch */ false,
          /* isCall */ true, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ true,
          /* loadSize */ 4, /* storeSize */ 4, /* condition */ QBDI::CONDITION_NONE});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 0, "EAX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 1, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0xa, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 4, 0, 7, "ESP", QBDI::REGISTER_READ_WRITE},
            {QBDI::OPERAND_SEG, QBDI::OPERANDFLAG_IMPLICIT, 0, 4, 0, -1, "SSP", QBDI::REGISTER_READ},
        }, QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-jmpi") {

    QBDI::rword addr = writeASM("jmp test_jmp\ntest_jmp:\n");

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "JMP_4", addr,
          /* instSize */ 5, /* affectControlFlow */ true, /* isBranch */ true,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0, /* condition */ QBDI::CONDITION_NONE});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_PCREL, 0, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
        }, QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-je") {

    QBDI::rword addr = writeASM("je test_jmp\ntest_jmp:\n");

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "JCC_4", addr,
          /* instSize */ 6, /* affectControlFlow */ true, /* isBranch */ true,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0, /* condition */ QBDI::CONDITION_EQUALS});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_PCREL, 0, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
        }, QBDI::REGISTER_READ);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-jmpm") {

    QBDI::rword addr = writeASM("jmpl *0xa(%eax)\n");

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "JMP32m", addr,
          /* instSize */ 3, /* affectControlFlow */ true, /* isBranch */ true,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 4, /* storeSize */ 0, /* condition */ QBDI::CONDITION_NONE});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 0, "EAX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 1, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0xa, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
        }, QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-fldl") {

    QBDI::rword addr = writeASM("fldl (%eax)\n");

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "LD_F64m", addr,
          /* instSize */ 2, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 8, /* storeSize */ 0, /* condition */ QBDI::CONDITION_NONE});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 0, "EAX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 1, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0x0, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 2, 0, 0, "FPCW", QBDI::REGISTER_READ},
            {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 2, 0, 2, "FPSW", QBDI::REGISTER_WRITE},
        }, QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-fstps") {

    QBDI::rword addr = writeASM("fstps (%eax)\n");

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "ST_FP32m", addr,
          /* instSize */ 2, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ true,
          /* loadSize */ 0, /* storeSize */ 4, /* condition */ QBDI::CONDITION_NONE});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 0, "EAX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 1, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0x0, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 2, 0, 0, "FPCW", QBDI::REGISTER_READ},
            {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 2, 0, 2, "FPSW", QBDI::REGISTER_WRITE},
        }, QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-movapd") {

    QBDI::rword addr = writeASM("movapd (%eax), %xmm1\n");

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "MOVAPDrm", addr,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 16, /* storeSize */ 0, /* condition */ QBDI::CONDITION_NONE});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_NONE, 0, 16, 0, offsetof(QBDI::FPRState, xmm1), "XMM1", QBDI::REGISTER_WRITE},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 0, "EAX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 1, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0x0, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
        }, QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-paddb") {

    QBDI::rword addr = writeASM("paddb %mm1, %mm0\n");

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "MMX_PADDBirr", addr,
          /* instSize */ 3, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0, /* condition */ QBDI::CONDITION_NONE});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 32, "MM0", QBDI::REGISTER_READ_WRITE},
            {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 48, "MM1", QBDI::REGISTER_READ},
        }, QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-vpaddb") {

    QBDI::rword addr = writeASM("vpaddb %xmm2, %xmm1, %xmm0\n");

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "VPADDBrr", addr,
          /* instSize */ 4, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ false, /* mayStore */ false,
          /* loadSize */ 0, /* storeSize */ 0, /* condition */ QBDI::CONDITION_NONE});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_NONE, 0, 16, 0, 160, "XMM0", QBDI::REGISTER_WRITE},
            {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_NONE, 0, 16, 0, 176, "XMM1", QBDI::REGISTER_READ},
            {QBDI::OPERAND_FPR, QBDI::OPERANDFLAG_NONE, 0, 16, 0, 192, "XMM2", QBDI::REGISTER_READ},
        }, QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-xlatb") {

    QBDI::rword addr = writeASM("xlatb\n");

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "XLAT", addr,
          /* instSize */ 1, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ false,
          /* loadSize */ 1, /* storeSize */ 0, /* condition */ QBDI::CONDITION_NONE});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 1, 0, 0, "AL", QBDI::REGISTER_READ_WRITE},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_IMPLICIT, 0, 4, 0, 1, "EBX", QBDI::REGISTER_READ},
        }, QBDI::REGISTER_UNUSED);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-movdir64b") {

    QBDI::rword addr = writeASM("movdir64b 0xc(%eax), %ecx\n");

    checkInst(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_INSTRUCTION),
        ExpectedInstAnalysis { "MOVDIR64B32", addr,
          /* instSize */ 6, /* affectControlFlow */ false, /* isBranch */ false,
          /* isCall */ false, /* isReturn */ false, /* isCompare */ false,
          /* isPredicable */ false, /* mayLoad */ true, /* mayStore */ true,
          /* loadSize */ 512, /* storeSize */ 512, /* condition */ QBDI::CONDITION_NONE});
    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 2, "ECX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 0, "EAX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 1, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0xc, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
        }, QBDI::REGISTER_UNUSED);
}
