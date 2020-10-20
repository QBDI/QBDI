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
#include "MemoryAccessTest.h"

#include "inttypes.h"

#include "Platform.h"
#include "Memory.hpp"
#include "Range.h"

struct ExpectedMemoryAccess {
    QBDI::rword address;
    QBDI::rword value;
    uint16_t size;
    QBDI::MemoryAccessType type;
    QBDI::MemoryAccessFlags flags;
    bool see = false;
};

struct ExpectedMemoryAccesses {
    std::vector<ExpectedMemoryAccess> accesses;
};

static QBDI::VMAction checkAccess(QBDI::VMInstanceRef vm, QBDI::GPRState* gprState, QBDI::FPRState* fprState, void* data) {

    ExpectedMemoryAccesses* info = static_cast<ExpectedMemoryAccesses*>(data);
    std::vector<QBDI::MemoryAccess> memaccesses = vm->getInstMemoryAccess();
    CHECKED_IF(memaccesses.size() == info->accesses.size()) {
        for (size_t i = 0; i < info->accesses.size(); i++) {
            auto &memaccess = memaccesses[i];
            auto &expect = info->accesses[i];
            CHECKED_IF(memaccess.accessAddress == expect.address)
                CHECKED_IF(memaccess.value == expect.value)
                    CHECKED_IF(memaccess.size == expect.size)
                        CHECKED_IF(memaccess.type == expect.type)
                            CHECKED_IF(memaccess.flags == expect.flags)
                                expect.see = true;
        }
    }
    return QBDI::VMAction::CONTINUE;
}

static QBDI::rword test_cpmsb(uint64_t* v1, uint64_t* v2) {
    asm ("mov %0, %%rsi;"
         "mov %1, %%rdi;"
         "cmpsb;"
         :: "r" (v1), "r" (v2)
         : "rsi", "rdi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-cmpsb") {
    uint64_t v1 = 0xaa, v2 = 0x55;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v2, v2, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ);
    vm->addMnemonicCB("CMPSB", QBDI::PREINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_cpmsb, {(QBDI::rword) &v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_cpmsw(uint64_t* v1, uint64_t* v2) {
    asm ("mov %0, %%rsi;"
         "mov %1, %%rdi;"
         "cmpsw;"
         :: "r" (v1), "r" (v2)
         : "rsi", "rdi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-cmpsw") {
    uint64_t v1 = 0x783, v2 = 0xbd7a;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v2, v2, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ);
    vm->addMnemonicCB("CMPSW", QBDI::PREINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_cpmsw, {(QBDI::rword) &v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_cpmsd(uint64_t* v1, uint64_t* v2) {
    asm ("mov %0, %%rsi;"
         "mov %1, %%rdi;"
         "cmpsl;"
         :: "r" (v1), "r" (v2)
         : "rsi", "rdi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-cmpsd") {
    uint64_t v1 = 0x6ef9efbd, v2 = 0xef783b2a;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v2, v2, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ);
    vm->addMnemonicCB("CMPSL", QBDI::PREINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_cpmsd, {(QBDI::rword) &v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_cpmsq(uint64_t* v1, uint64_t* v2) {
    asm ("mov %0, %%rsi;"
         "mov %1, %%rdi;"
         "cmpsq;"
         :: "r" (v1), "r" (v2)
         : "rsi", "rdi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-cmpsq") {
    uint64_t v1 = 0x6723870bdefa, v2 = 0x1234098765efdbac;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v2, v2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ);
    vm->addMnemonicCB("CMPSQ", QBDI::PREINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_cpmsq, {(QBDI::rword) &v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_rep_cpmsb(uint8_t* v1, uint8_t* v2, QBDI::rword size) {
    asm ("mov %0, %%rsi;"
         "mov %1, %%rdi;"
         "mov %2, %%rcx;"
         "cld;"
         "rep cmpsb;"
         :: "r" (v1), "r" (v2), "r" (size)
         : "rsi", "rdi", "rcx");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-rep_cmpsb") {
    uint8_t v1[10] = {0x56, 0x78, 0x89, 0xab, 0xe6, 0xe7, 0x1a, 0xfa, 0xc8, 0x6d};
    uint8_t v2[10] = {0x56, 0x78, 0x89, 0xab, 0xe6, 0xe7, 0x1a, 0xfa, 0xc8, 0x6c};
    ExpectedMemoryAccesses expectedPre = {{
        { (QBDI::rword) &v1, 0, 1, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE | QBDI::MEMORY_UNKNOWN_SIZE},
        { (QBDI::rword) &v2, 0, 1, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE | QBDI::MEMORY_UNKNOWN_SIZE},
    }};
    ExpectedMemoryAccesses expectedPost = {{
        { (QBDI::rword) &v1, 0, 10, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
        { (QBDI::rword) &v2, 0, 10, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ);
    vm->addMnemonicCB("CMPSB", QBDI::PREINST, checkAccess, &expectedPre);
    vm->addMnemonicCB("CMPSB", QBDI::POSTINST, checkAccess, &expectedPost);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_rep_cpmsb, {(QBDI::rword) &v1, (QBDI::rword) &v2, sizeof(v1)});

    REQUIRE(ran);

    for (auto& e: expectedPre.accesses)
        REQUIRE(e.see);

    for (auto& e: expectedPost.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_repne_cpmsb(uint8_t* v1, uint8_t* v2, QBDI::rword size) {
    asm ("mov %0, %%rsi;"
         "mov %1, %%rdi;"
         "mov %2, %%rcx;"
         "cld;"
         "repne cmpsb;"
         :: "r" (v1), "r" (v2), "r" (size)
         : "rsi", "rdi", "rcx");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-repne_cmpsb") {
    uint8_t v1[10] = {0x56, 0x78, 0x89, 0xab, 0xe6, 0xe7, 0x1a, 0xfa, 0xc8, 0x6d};
    uint8_t v2[10] = {0xb1, 0x5, 0x98, 0xae, 0xe2, 0xe6, 0x19, 0xf9, 0xc7, 0x6d};
    ExpectedMemoryAccesses expectedPre = {{
        { (QBDI::rword) &v1, 0, 1, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE | QBDI::MEMORY_UNKNOWN_SIZE},
        { (QBDI::rword) &v2, 0, 1, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE | QBDI::MEMORY_UNKNOWN_SIZE},
    }};
    ExpectedMemoryAccesses expectedPost = {{
        { (QBDI::rword) &v1, 0, 10, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
        { (QBDI::rword) &v2, 0, 10, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ);
    vm->addMnemonicCB("CMPSB", QBDI::PREINST, checkAccess, &expectedPre);
    vm->addMnemonicCB("CMPSB", QBDI::POSTINST, checkAccess, &expectedPost);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_repne_cpmsb, {(QBDI::rword) &v1, (QBDI::rword) &v2, sizeof(v1)});

    REQUIRE(ran);

    for (auto& e: expectedPre.accesses)
        REQUIRE(e.see);

    for (auto& e: expectedPost.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_rep_cpmsb2(uint8_t* v1, uint8_t* v2, QBDI::rword size) {
    asm ("mov %0, %%rsi;"
         "mov %1, %%rdi;"
         "mov %2, %%rcx;"
         "std;"
         "rep cmpsb;"
         "cld;"
         :: "r" (v1), "r" (v2), "r" (size)
         : "rsi", "rdi", "rcx");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-rep_cmpsb2") {
    uint8_t v1[10] = {0x5c, 0x78, 0x89, 0xab, 0xe6, 0xe7, 0x1a, 0xfa, 0xc8, 0x6c};
    uint8_t v2[10] = {0x56, 0x78, 0x89, 0xab, 0xe6, 0xe7, 0x1a, 0xfa, 0xc8, 0x6c};
    ExpectedMemoryAccesses expectedPre = {{
        { (QBDI::rword) &v1[9], 0, 1, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE | QBDI::MEMORY_UNKNOWN_SIZE},
        { (QBDI::rword) &v2[9], 0, 1, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE | QBDI::MEMORY_UNKNOWN_SIZE},
    }};
    ExpectedMemoryAccesses expectedPost = {{
        { (QBDI::rword) &v1, 0, 10, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
        { (QBDI::rword) &v2, 0, 10, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ);
    vm->addMnemonicCB("CMPSB", QBDI::PREINST, checkAccess, &expectedPre);
    vm->addMnemonicCB("CMPSB", QBDI::POSTINST, checkAccess, &expectedPost);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_rep_cpmsb2, {(QBDI::rword) &v1[9], (QBDI::rword) &v2[9], sizeof(v1)});

    REQUIRE(ran);

    for (auto& e: expectedPre.accesses)
        REQUIRE(e.see);

    for (auto& e: expectedPost.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_rep_cpmsw(uint16_t* v1, uint16_t* v2, QBDI::rword size) {
    asm ("mov %0, %%rsi;"
         "mov %1, %%rdi;"
         "mov %2, %%rcx;"
         "cld;"
         "rep cmpsw;"
         :: "r" (v1), "r" (v2), "r" (size)
         : "rsi", "rdi", "rcx");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-rep_cmpsw") {
    uint16_t v1[5] = {0x5c78, 0x89ab, 0xe6e7, 0x1afa, 0xc86c};
    uint16_t v2[5] = {0x5c78, 0x89ab, 0xe6e7, 0x1afa, 0xc86d};
    ExpectedMemoryAccesses expectedPre = {{
        { (QBDI::rword) &v1, 0, 2, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE | QBDI::MEMORY_UNKNOWN_SIZE},
        { (QBDI::rword) &v2, 0, 2, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE | QBDI::MEMORY_UNKNOWN_SIZE},
    }};
    ExpectedMemoryAccesses expectedPost = {{
        { (QBDI::rword) &v1, 0, 10, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
        { (QBDI::rword) &v2, 0, 10, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ);
    vm->addMnemonicCB("CMPSW", QBDI::PREINST, checkAccess, &expectedPre);
    vm->addMnemonicCB("CMPSW", QBDI::POSTINST, checkAccess, &expectedPost);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_rep_cpmsw, {(QBDI::rword) &v1, (QBDI::rword) &v2, sizeof(v1)});

    REQUIRE(ran);

    for (auto& e: expectedPre.accesses)
        REQUIRE(e.see);

    for (auto& e: expectedPost.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_rep_cpmsw2(uint16_t* v1, uint16_t* v2, QBDI::rword size) {
    asm ("mov %0, %%rsi;"
         "mov %1, %%rdi;"
         "mov %2, %%rcx;"
         "std;"
         "rep cmpsw;"
         "cld;"
         :: "r" (v1), "r" (v2), "r" (size)
         : "rsi", "rdi", "rcx");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-rep_cmpsw2") {
    uint16_t v1[5] = {0x5c78, 0x89ab, 0xe6e7, 0x1afa, 0xc86c};
    uint16_t v2[5] = {0x5678, 0x89ab, 0xe6e7, 0x1afa, 0xc86c};
    ExpectedMemoryAccesses expectedPre = {{
        { (QBDI::rword) &v1[4], 0, 2, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE | QBDI::MEMORY_UNKNOWN_SIZE},
        { (QBDI::rword) &v2[4], 0, 2, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE | QBDI::MEMORY_UNKNOWN_SIZE},
    }};
    ExpectedMemoryAccesses expectedPost = {{
        { (QBDI::rword) &v1, 0, 10, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
        { (QBDI::rword) &v2, 0, 10, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ);
    vm->addMnemonicCB("CMPSW", QBDI::PREINST, checkAccess, &expectedPre);
    vm->addMnemonicCB("CMPSW", QBDI::POSTINST, checkAccess, &expectedPost);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_rep_cpmsw2, {(QBDI::rword) &v1[4], (QBDI::rword) &v2[4], sizeof(v1)});

    REQUIRE(ran);

    for (auto& e: expectedPre.accesses)
        REQUIRE(e.see);

    for (auto& e: expectedPost.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_movsb(uint64_t* v1, uint64_t* v2) {
    asm ("mov %0, %%rsi;"
         "mov %1, %%rdi;"
         "cld;"
         "movsb;"
         :: "r" (v1), "r" (v2)
         : "rsi", "rdi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-movsb") {
    uint64_t v1 = 0xbf, v2 = 0x78;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v2, v1, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("MOVSB", QBDI::POSTINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_movsb, {(QBDI::rword) &v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    REQUIRE(v2 == v1);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_movsw(uint64_t* v1, uint64_t* v2) {
    asm ("mov %0, %%rsi;"
         "mov %1, %%rdi;"
         "cld;"
         "movsw;"
         :: "r" (v1), "r" (v2)
         : "rsi", "rdi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-movsw") {
    uint64_t v1 = 0x789f, v2 = 0xbd67;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v2, v1, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("MOVSW", QBDI::POSTINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_movsw, {(QBDI::rword) &v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    REQUIRE(v2 == v1);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_movsl(uint64_t* v1, uint64_t* v2) {
    asm ("mov %0, %%rsi;"
         "mov %1, %%rdi;"
         "cld;"
         "movsl;"
         :: "r" (v1), "r" (v2)
         : "rsi", "rdi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-movsl") {
    uint64_t v1 = 0xa579eb9d, v2 = 0x2389befa;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v2, v1, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("MOVSL", QBDI::POSTINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_movsl, {(QBDI::rword) &v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    REQUIRE(v2 == v1);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_movsq(uint64_t* v1, uint64_t* v2) {
    asm ("mov %0, %%rsi;"
         "mov %1, %%rdi;"
         "cld;"
         "movsq;"
         :: "r" (v1), "r" (v2)
         : "rsi", "rdi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-movsq") {
    uint64_t v1 = 0xb036789eb8ea, v2 = 0xab8e602baef846;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v2, v1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("MOVSQ", QBDI::POSTINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_movsq, {(QBDI::rword) &v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    REQUIRE(v2 == v1);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_movsb2(uint64_t* v1, uint64_t* v2) {
    asm ("mov %0, %%rsi;"
         "mov %1, %%rdi;"
         "std;"
         "movsb;"
         "cld;"
         :: "r" (v1), "r" (v2)
         : "rsi", "rdi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-movsb2") {
    uint64_t v1 = 0x8, v2 = 0x7f;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v2, v1, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("MOVSB", QBDI::POSTINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_movsb2, {(QBDI::rword) &v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    REQUIRE(v2 == v1);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_movsw2(uint64_t* v1, uint64_t* v2) {
    asm ("mov %0, %%rsi;"
         "mov %1, %%rdi;"
         "std;"
         "movsw;"
         "cld;"
         :: "r" (v1), "r" (v2)
         : "rsi", "rdi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-movsw2") {
    uint64_t v1 = 0xad63, v2 = 0x6219;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v2, v1, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("MOVSW", QBDI::POSTINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_movsw2, {(QBDI::rword) &v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    REQUIRE(v2 == v1);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_movsl2(uint64_t* v1, uint64_t* v2) {
    asm ("mov %0, %%rsi;"
         "mov %1, %%rdi;"
         "std;"
         "movsl;"
         "cld;"
         :: "r" (v1), "r" (v2)
         : "rsi", "rdi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-movsl2") {
    uint64_t v1 = 0xefa036db, v2 = 0xefd7137a;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v2, v1, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("MOVSL", QBDI::POSTINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_movsl2, {(QBDI::rword) &v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    REQUIRE(v2 == v1);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_movsq2(uint64_t* v1, uint64_t* v2) {
    asm ("mov %0, %%rsi;"
         "mov %1, %%rdi;"
         "std;"
         "movsq;"
         "cld;"
         :: "r" (v1), "r" (v2)
         : "rsi", "rdi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-movsq2") {
    uint64_t v1 = 0x2360Abed083, v2 = 0xeb0367a801346;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v2, v1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("MOVSQ", QBDI::POSTINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_movsq2, {(QBDI::rword) &v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    REQUIRE(v2 == v1);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_rep_movsl(uint32_t* v1, uint32_t* v2, QBDI::rword size) {
    asm ("mov %0, %%rsi;"
         "mov %1, %%rdi;"
         "mov %2, %%rcx;"
         "cld;"
         "rep movsl;"
         :: "r" (v1), "r" (v2), "r" (size)
         : "rsi", "rdi", "rcx");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-rep_movsl") {
    uint32_t v1[5] = {0xab673, 0xeba9256, 0x638feba8, 0x7182faB, 0x7839021b};
    uint32_t v2[5] = {0};
    ExpectedMemoryAccesses expectedPre = {{
        { (QBDI::rword) &v2, 0, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE | QBDI::MEMORY_UNKNOWN_SIZE},
        { (QBDI::rword) &v1, 0, 4, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE | QBDI::MEMORY_UNKNOWN_SIZE},
    }};
    ExpectedMemoryAccesses expectedPost = {{
        { (QBDI::rword) &v2, 0, sizeof(v1), QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
        { (QBDI::rword) &v1, 0, sizeof(v1), QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("MOVSL", QBDI::PREINST, checkAccess, &expectedPre);
    vm->addMnemonicCB("MOVSL", QBDI::POSTINST, checkAccess, &expectedPost);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_rep_movsl, {(QBDI::rword) &v1, (QBDI::rword) &v2, sizeof(v1) / sizeof(uint32_t)});

    REQUIRE(ran);
    for (size_t i = 0; i < sizeof(v1) / sizeof(uint32_t); i++)
        REQUIRE(v2[i] == v1[i]);
    for (auto& e: expectedPre.accesses)
        REQUIRE(e.see);
    for (auto& e: expectedPost.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_rep_movsl2(uint32_t* v1, uint32_t* v2, QBDI::rword size) {
    asm ("mov %0, %%rsi;"
         "mov %1, %%rdi;"
         "mov %2, %%rcx;"
         "std;"
         "rep movsl;"
         "cld;"
         :: "r" (v1), "r" (v2), "r" (size)
         : "rsi", "rdi", "rcx");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-rep_movsl2") {
    uint32_t v1[5] = {0xab673, 0xeba9256, 0x638feba8, 0x7182faB, 0x7839021b};
    uint32_t v2[5] = {0};
    ExpectedMemoryAccesses expectedPre = {{
        { (QBDI::rword) &v2[4], 0, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE | QBDI::MEMORY_UNKNOWN_SIZE},
        { (QBDI::rword) &v1[4], 0, 4, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE | QBDI::MEMORY_UNKNOWN_SIZE},
    }};
    ExpectedMemoryAccesses expectedPost = {{
        { (QBDI::rword) &v2, 0, sizeof(v1), QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
        { (QBDI::rword) &v1, 0, sizeof(v1), QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("MOVSL", QBDI::PREINST, checkAccess, &expectedPre);
    vm->addMnemonicCB("MOVSL", QBDI::POSTINST, checkAccess, &expectedPost);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_rep_movsl2, {(QBDI::rword) &v1[4], (QBDI::rword) &v2[4], sizeof(v1) / sizeof(uint32_t)});

    REQUIRE(ran);
    for (size_t i = 0; i < sizeof(v1) / sizeof(uint32_t); i++)
        REQUIRE(v2[i] == v1[i]);
    for (auto& e: expectedPre.accesses)
        REQUIRE(e.see);
    for (auto& e: expectedPost.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_scasb(uint64_t v1, uint64_t* v2) {
    asm ("mov %0, %%rax;"
         "mov %1, %%rdi;"
         "scasb;"
         :: "r" (v1), "r" (v2)
         : "rax", "rdi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-scasb") {
    uint64_t v1 = 0x8, v2 = 0x6a;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v2, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("SCASB", QBDI::POSTINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_scasb, {(QBDI::rword) v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_scasw(uint64_t v1, uint64_t* v2) {
    asm ("mov %0, %%rax;"
         "mov %1, %%rdi;"
         "scasw;"
         :: "r" (v1), "r" (v2)
         : "rax", "rdi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-scasw") {
    uint64_t v1 = 0x5ef1, v2 = 0x6789;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v2, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("SCASW", QBDI::POSTINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_scasw, {(QBDI::rword) v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_scasl(uint64_t v1, uint64_t* v2) {
    asm ("mov %0, %%rax;"
         "mov %1, %%rdi;"
         "scasl;"
         :: "r" (v1), "r" (v2)
         : "rax", "rdi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-scasl") {
    uint64_t v1 = 0x629ebf, v2 = 0x1234567;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v2, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("SCASL", QBDI::POSTINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_scasl, {(QBDI::rword) v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_scasq(uint64_t v1, uint64_t* v2) {
    asm ("mov %0, %%rax;"
         "mov %1, %%rdi;"
         "scasq;"
         :: "r" (v1), "r" (v2)
         : "rax", "rdi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-scasq") {
    uint64_t v1 = 0x6efab792eb, v2 = 0xebaf719630145;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("SCASQ", QBDI::POSTINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_scasq, {(QBDI::rword) v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_lodsb(uint64_t* v1, uint64_t* v2) {
    asm ("mov %1, %%rsi;"
         "lodsb;"
         "mov %%rax, %0;"
         : "=r"(*v1)
         : "r" (v2)
         : "rax", "rsi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-lodsb") {
    uint64_t v1 = 0x8, v2 = 0x6a;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v2, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("LODSB", QBDI::POSTINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_lodsb, {(QBDI::rword) &v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    REQUIRE(v1 == v2);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_lodsw(uint64_t* v1, uint64_t* v2) {
    asm ("mov %1, %%rsi;"
         "lodsw;"
         "mov %%rax, %0;"
         : "=r"(*v1)
         : "r" (v2)
         : "rax", "rsi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-lodsw") {
    uint64_t v1 = 0x5ef1, v2 = 0x6789;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v2, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("LODSW", QBDI::POSTINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_lodsw, {(QBDI::rword) &v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    REQUIRE(v1 == v2);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_lodsl(uint64_t* v1, uint64_t* v2) {
    asm ("mov %1, %%rsi;"
         "lodsl;"
         "mov %%rax, %0;"
         : "=r"(*v1)
         : "r" (v2)
         : "rax", "rsi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-lodsl") {
    uint64_t v1 = 0x629ebf, v2 = 0x1234567;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v2, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("LODSL", QBDI::POSTINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_lodsl, {(QBDI::rword) &v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    REQUIRE(v1 == v2);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_lodsq(uint64_t* v1, uint64_t* v2) {
    asm ("mov %1, %%rsi;"
         "lodsq;"
         "mov %%rax, %0;"
         : "=r"(*v1)
         : "r" (v2)
         : "rax", "rsi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-lodsq") {
    uint64_t v1 = 0x6efab792eb, v2 = 0xebaf719630145;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("LODSQ", QBDI::POSTINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_lodsq, {(QBDI::rword) &v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    REQUIRE(v1 == v2);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_stosb(uint64_t v1, uint64_t* v2) {
    asm ("mov %0, %%rax;"
         "mov %1, %%rdi;"
         "cld;"
         "stosb;"
         :: "r"(v1), "r" (v2)
         : "rax", "rdi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-stosb") {
    uint64_t v1 = 0x8, v2 = 0x6a;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("STOSB", QBDI::POSTINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_stosb, {(QBDI::rword) v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    REQUIRE(v1 == v2);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_stosw(uint64_t v1, uint64_t* v2) {
    asm ("mov %0, %%rax;"
         "mov %1, %%rdi;"
         "cld;"
         "stosw;"
         :: "r"(v1), "r" (v2)
         : "rax", "rdi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-stosw") {
    uint64_t v1 = 0x5ef1, v2 = 0x6789;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("STOSW", QBDI::POSTINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_stosw, {(QBDI::rword) v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    REQUIRE(v1 == v2);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_stosl(uint64_t v1, uint64_t* v2) {
    asm ("mov %0, %%rax;"
         "mov %1, %%rdi;"
         "cld;"
         "stosl;"
         :: "r"(v1), "r" (v2)
         : "rax", "rdi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-stosl") {
    uint64_t v1 = 0x629ebf, v2 = 0x1234567;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("STOSL", QBDI::POSTINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_stosl, {(QBDI::rword) v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    REQUIRE(v1 == v2);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_stosq(uint64_t v1, uint64_t* v2) {
    asm ("mov %0, %%rax;"
         "mov %1, %%rdi;"
         "cld;"
         "stosq;"
         :: "r"(v1), "r" (v2)
         : "rax", "rdi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-stosq") {
    uint64_t v1 = 0x6efab792eb, v2 = 0xebaf719630145;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("STOSQ", QBDI::POSTINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_stosq, {(QBDI::rword) v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    REQUIRE(v1 == v2);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_stosb2(uint64_t v1, uint64_t* v2) {
    asm ("mov %0, %%rax;"
         "mov %1, %%rdi;"
         "std;"
         "stosb;"
         "cld;"
         :: "r"(v1), "r" (v2)
         : "rax", "rdi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-stosb2") {
    uint64_t v1 = 0x8, v2 = 0x6a;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("STOSB", QBDI::POSTINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_stosb2, {(QBDI::rword) v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    REQUIRE(v1 == v2);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_stosw2(uint64_t v1, uint64_t* v2) {
    asm ("mov %0, %%rax;"
         "mov %1, %%rdi;"
         "std;"
         "stosw;"
         "cld;"
         :: "r"(v1), "r" (v2)
         : "rax", "rdi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-stosw2") {
    uint64_t v1 = 0x5ef1, v2 = 0x6789;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("STOSW", QBDI::POSTINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_stosw2, {(QBDI::rword) v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    REQUIRE(v1 == v2);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_stosl2(uint64_t v1, uint64_t* v2) {
    asm ("mov %0, %%rax;"
         "mov %1, %%rdi;"
         "std;"
         "stosl;"
         "cld;"
         :: "r"(v1), "r" (v2)
         : "rax", "rdi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-stosl2") {
    uint64_t v1 = 0x629ebf, v2 = 0x1234567;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("STOSL", QBDI::POSTINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_stosl2, {(QBDI::rword) v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    REQUIRE(v1 == v2);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_stosq2(uint64_t v1, uint64_t* v2) {
    asm ("mov %0, %%rax;"
         "mov %1, %%rdi;"
         "std;"
         "stosq;"
         "cld;"
         :: "r"(v1), "r" (v2)
         : "rax", "rdi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-stosq2") {
    uint64_t v1 = 0x6efab792eb, v2 = 0xebaf719630145;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("STOSQ", QBDI::POSTINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_stosq2, {(QBDI::rword) v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    REQUIRE(v1 == v2);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}
