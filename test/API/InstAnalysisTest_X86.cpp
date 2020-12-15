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
}*/

static void checkOperand(const QBDI::InstAnalysis* ana, const std::vector<QBDI::OperandAnalysis> expecteds) {

    CHECKED_IF((ana->analysisType & QBDI::ANALYSIS_OPERANDS) == QBDI::ANALYSIS_OPERANDS)
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

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-CachedInst") {

    QBDI::rword addr = writeASM("leal (%eax), %ebx\n");

    CHECK(vm.getCachedInstAnalysis(addr) == nullptr);

    vm.precacheBasicBlock(addr);

    CHECK(vm.getCachedInstAnalysis(addr) != nullptr);
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-lea") {

    QBDI::rword addr = writeASM("leal (%eax), %ebx\n");
    vm.precacheBasicBlock(addr);

    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 1, "EBX", QBDI::REGISTER_WRITE},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 0, "EAX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 1, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
        });
}

TEST_CASE_METHOD(InstAnalysisTest, "InstAnalysisTest_X86-movrm") {

    QBDI::rword addr = writeASM("movl 0x45(%eax,%edx,4), %ebx\n");
    vm.precacheBasicBlock(addr);

    checkOperand(vm.getCachedInstAnalysis(addr, QBDI::ANALYSIS_OPERANDS),
        {
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 1, "EBX", QBDI::REGISTER_WRITE},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 0, "EAX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 4, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, 4, 0, 3, "EDX", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 0x45, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_ADDR, 0, 0, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
        });
}

