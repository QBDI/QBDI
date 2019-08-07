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
                            expect.see = true;

        }
    }
    return QBDI::VMAction::CONTINUE;
}

static QBDI::rword test_cpmsb(uint32_t* v1, uint32_t* v2) {
    asm ("mov %0, %%esi;"
         "mov %1, %%edi;"
         "cmpsb;"
         :: "r" (v1), "r" (v2)
         : "esi", "edi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-cmpsb") {
    uint32_t v1 = 30, v2 = 45;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 1, QBDI::MEMORY_READ},
        { (QBDI::rword) &v2, v2, 1, QBDI::MEMORY_READ},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ);
    vm->addMnemonicCB("CMPSB", QBDI::PREINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_cpmsb, {(QBDI::rword) &v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_cpmsw(uint32_t* v1, uint32_t* v2) {
    asm ("mov %0, %%esi;"
         "mov %1, %%edi;"
         "cmpsw;"
         :: "r" (v1), "r" (v2)
         : "esi", "edi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-cmpsw") {
    uint32_t v1 = 0x6789, v2 = 0x124;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 2, QBDI::MEMORY_READ},
        { (QBDI::rword) &v2, v2, 2, QBDI::MEMORY_READ},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ);
    vm->addMnemonicCB("CMPSW", QBDI::PREINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_cpmsw, {(QBDI::rword) &v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_cpmsd(uint32_t* v1, uint32_t* v2) {
    asm ("mov %0, %%esi;"
         "mov %1, %%edi;"
         "cmpsl;"
         :: "r" (v1), "r" (v2)
         : "esi", "edi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-cmpsd") {
    uint32_t v1 = 0xA90EF, v2 = 0x6734562;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 4, QBDI::MEMORY_READ},
        { (QBDI::rword) &v2, v2, 4, QBDI::MEMORY_READ},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ);
    vm->addMnemonicCB("CMPSL", QBDI::PREINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_cpmsd, {(QBDI::rword) &v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_movsb(uint32_t* v1, uint32_t* v2) {
    asm ("mov %0, %%esi;"
         "mov %1, %%edi;"
         "cld;"
         "movsb;"
         :: "r" (v1), "r" (v2)
         : "esi", "edi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-movsb") {
    uint32_t v1 = 0x87, v2 = 0x34;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 1, QBDI::MEMORY_READ},
        { (QBDI::rword) &v2, v1, 1, QBDI::MEMORY_WRITE},
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

static QBDI::rword test_movsw(uint32_t* v1, uint32_t* v2) {
    asm ("mov %0, %%esi;"
         "mov %1, %%edi;"
         "cld;"
         "movsw;"
         :: "r" (v1), "r" (v2)
         : "esi", "edi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-movsw") {
    uint32_t v1 = 0xbefa, v2 = 0x7623;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 2, QBDI::MEMORY_READ},
        { (QBDI::rword) &v2, v1, 2, QBDI::MEMORY_WRITE},
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

static QBDI::rword test_movsl(uint32_t* v1, uint32_t* v2) {
    asm ("mov %0, %%esi;"
         "mov %1, %%edi;"
         "cld;"
         "movsl;"
         :: "r" (v1), "r" (v2)
         : "esi", "edi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-movsl") {
    uint32_t v1 = 0x78ef6bda, v2 = 0x5aefbd6;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 4, QBDI::MEMORY_READ},
        { (QBDI::rword) &v2, v1, 4, QBDI::MEMORY_WRITE},
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

static QBDI::rword test_movsb2(uint32_t* v1, uint32_t* v2) {
    asm ("mov %0, %%esi;"
         "mov %1, %%edi;"
         "std;"
         "movsb;"
         "cld;"
         :: "r" (v1), "r" (v2)
         : "esi", "edi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-movsb2") {
    uint32_t v1 = 0x92, v2 = 0xf8;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 1, QBDI::MEMORY_READ},
        { (QBDI::rword) &v2, v1, 1, QBDI::MEMORY_WRITE},
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

static QBDI::rword test_movsw2(uint32_t* v1, uint32_t* v2) {
    asm ("mov %0, %%esi;"
         "mov %1, %%edi;"
         "std;"
         "movsw;"
         "cld;"
         :: "r" (v1), "r" (v2)
         : "esi", "edi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-movsw2") {
    uint32_t v1 = 0xb56, v2 = 0x6ea8;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 2, QBDI::MEMORY_READ},
        { (QBDI::rword) &v2, v1, 2, QBDI::MEMORY_WRITE},
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

static QBDI::rword test_movsl2(uint32_t* v1, uint32_t* v2) {
    asm ("mov %0, %%esi;"
         "mov %1, %%edi;"
         "std;"
         "movsl;"
         "cld;"
         :: "r" (v1), "r" (v2)
         : "esi", "edi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-movsl2") {
    uint32_t v1 = 0xab673, v2 = 0xdb72f;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 4, QBDI::MEMORY_READ},
        { (QBDI::rword) &v2, v1, 4, QBDI::MEMORY_WRITE},
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

static QBDI::rword test_scasb(uint32_t v1, uint32_t* v2) {
    asm ("mov %0, %%eax;"
         "mov %1, %%edi;"
         "scasb;"
         :: "r" (v1), "r" (v2)
         : "eax", "edi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-scasb") {
    uint32_t v1 = 30, v2 = 45;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v2, 1, QBDI::MEMORY_READ},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("SCASB", QBDI::POSTINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_scasb, {(QBDI::rword) v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_scasw(uint32_t v1, uint32_t* v2) {
    asm ("mov %0, %%eax;"
         "mov %1, %%edi;"
         "scasw;"
         :: "r" (v1), "r" (v2)
         : "eax", "edi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-scasw") {
    uint32_t v1 = 0x5ef1, v2 = 0x6789;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v2, 2, QBDI::MEMORY_READ},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("SCASW", QBDI::POSTINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_scasw, {(QBDI::rword) v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_scasl(uint32_t v1, uint32_t* v2) {
    asm ("mov %0, %%eax;"
         "mov %1, %%edi;"
         "scasl;"
         :: "r" (v1), "r" (v2)
         : "eax", "edi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-scasl") {
    uint32_t v1 = 0x629ebf, v2 = 0x1234567;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v2, 4, QBDI::MEMORY_READ},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("SCASL", QBDI::POSTINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = vm->call(&retval, (QBDI::rword) test_scasl, {(QBDI::rword) v1, (QBDI::rword) &v2});

    REQUIRE(ran);
    for (auto& e: expected.accesses)
        REQUIRE(e.see);
}

static QBDI::rword test_lodsb(uint32_t* v1, uint32_t* v2) {
    asm ("mov %1, %%esi;"
         "lodsb;"
         "mov %%eax, %0;"
         : "=r"(*v1)
         : "r" (v2)
         : "eax", "esi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-lodsb") {
    uint32_t v1 = 0x8, v2 = 0x6a;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v2, 1, QBDI::MEMORY_READ},
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

static QBDI::rword test_lodsw(uint32_t* v1, uint32_t* v2) {
    asm ("mov %1, %%esi;"
         "lodsw;"
         "mov %%eax, %0;"
         : "=r"(*v1)
         : "r" (v2)
         : "eax", "esi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-lodsw") {
    uint32_t v1 = 0x5ef1, v2 = 0x6789;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v2, 2, QBDI::MEMORY_READ},
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

static QBDI::rword test_lodsl(uint32_t* v1, uint32_t* v2) {
    asm ("mov %1, %%esi;"
         "lodsl;"
         "mov %%eax, %0;"
         : "=r"(*v1)
         : "r" (v2)
         : "eax", "esi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-lodsl") {
    uint32_t v1 = 0x629ebf, v2 = 0x1234567;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v2, 4, QBDI::MEMORY_READ},
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

static QBDI::rword test_stosb(uint32_t v1, uint32_t* v2) {
    asm ("mov %0, %%eax;"
         "mov %1, %%edi;"
         "cld;"
         "stosb;"
         :: "r"(v1), "r" (v2)
         : "eax", "edi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-stosb") {
    uint32_t v1 = 0x8, v2 = 0x6a;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 1, QBDI::MEMORY_WRITE},
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

static QBDI::rword test_stosw(uint32_t v1, uint32_t* v2) {
    asm ("mov %0, %%eax;"
         "mov %1, %%edi;"
         "cld;"
         "stosw;"
         :: "r"(v1), "r" (v2)
         : "eax", "edi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-stosw") {
    uint32_t v1 = 0x5ef1, v2 = 0x6789;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 2, QBDI::MEMORY_WRITE},
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

static QBDI::rword test_stosl(uint32_t v1, uint32_t* v2) {
    asm ("mov %0, %%eax;"
         "mov %1, %%edi;"
         "cld;"
         "stosl;"
         :: "r"(v1), "r" (v2)
         : "eax", "edi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-stosl") {
    uint32_t v1 = 0x629ebf, v2 = 0x1234567;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 4, QBDI::MEMORY_WRITE},
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

static QBDI::rword test_stosb2(uint32_t v1, uint32_t* v2) {
    asm ("mov %0, %%eax;"
         "mov %1, %%edi;"
         "std;"
         "stosb;"
         "cld;"
         :: "r"(v1), "r" (v2)
         : "eax", "edi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-stosb2") {
    uint32_t v1 = 0x8, v2 = 0x6a;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 1, QBDI::MEMORY_WRITE},
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

static QBDI::rword test_stosw2(uint32_t v1, uint32_t* v2) {
    asm ("mov %0, %%eax;"
         "mov %1, %%edi;"
         "std;"
         "stosw;"
         "cld;"
         :: "r"(v1), "r" (v2)
         : "eax", "edi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-stosw2") {
    uint32_t v1 = 0x5ef1, v2 = 0x6789;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 2, QBDI::MEMORY_WRITE},
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

static QBDI::rword test_stosl2(uint32_t v1, uint32_t* v2) {
    asm ("mov %0, %%eax;"
         "mov %1, %%edi;"
         "std;"
         "stosl;"
         "cld;"
         :: "r"(v1), "r" (v2)
         : "eax", "edi");
    return 0;
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-stosl2") {
    uint32_t v1 = 0x629ebf, v2 = 0x1234567;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 4, QBDI::MEMORY_WRITE},
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
