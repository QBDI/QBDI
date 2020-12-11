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

#include "inttypes.h"

#include "MemoryAccessTest.h"

#include "Platform.h"
#include "Memory.hpp"
#include "Range.h"

//static QBDI::VMAction debugCB(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState, QBDI::FPRState *fprState, void *data) {
//    const QBDI::InstAnalysis* instAnalysis = vm->getInstAnalysis();
//    printf("0x%lx (%10s): %s\n", instAnalysis->address, instAnalysis->mnemonic, instAnalysis->disassembly);
//    for (auto &a: vm->getInstMemoryAccess()) {
//        printf(" - inst: 0x%lx, addr: 0x%lx, size: %d, type: %c%c, value: 0xl%x, flags: 0x%x\n",
//            a.instAddress, a.accessAddress, a.size,
//            ((a.type & QBDI::MEMORY_READ) !=0 )?'r':'-',
//            ((a.type & QBDI::MEMORY_WRITE) !=0 )?'w':'-',
//            a.value, a.flags);
//    }
//    return QBDI::VMAction::CONTINUE;
//}

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
    if (std::all_of(info->accesses.begin(), info->accesses.end(), [](ExpectedMemoryAccess& a) {return a.see;}))
      return QBDI::VMAction::CONTINUE;

    CHECKED_IF(memaccesses.size() == info->accesses.size()) {
        for (size_t i = 0; i < info->accesses.size(); i++) {
            auto &memaccess = memaccesses[i];
            auto &expect = info->accesses[i];
            CHECKED_IF(memaccess.accessAddress == expect.address)
                CHECKED_IF( ( memaccess.value == expect.value || expect.value == 0))
                    CHECKED_IF(memaccess.size == expect.size)
                        CHECKED_IF(memaccess.type == expect.type)
                            CHECKED_IF(memaccess.flags == expect.flags)
                                expect.see = true;
        }
    }
    return QBDI::VMAction::CONTINUE;
}

// test stack memory access
// PUSH POP CALL RET

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-push_pop_reg") {

    const char source[] = "xchg %rsp, %rbx\n"
                          "push %rax\n"
                          "pop %rax\n"
                          "xchg %rsp, %rbx\n" ;

    QBDI::rword v1 = 0xab3672016bef61ae;
    QBDI::rword tmpStack[10] = {0};
    ExpectedMemoryAccesses expectedPush = {{
        { (QBDI::rword) &tmpStack[8], v1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};
    ExpectedMemoryAccesses expectedPop = {{
        { (QBDI::rword) &tmpStack[8], v1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("PUSH64r", QBDI::POSTINST, checkAccess, &expectedPush);
    vm->addMnemonicCB("POP64r", QBDI::POSTINST, checkAccess, &expectedPop);

    QBDI::GPRState* state = vm->getGPRState();
    state->rax = v1;
    state->rbx = (QBDI::rword) &tmpStack[9];
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    for (auto& e: expectedPop.accesses)
        CHECK(e.see);
    for (auto& e: expectedPush.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-push_pop_mem") {

    const char source[] = "xchg %rsp, %rbx\n"
                          "push (%rax)\n"
                          "pop (%rax)\n"
                          "xchg %rsp, %rbx\n" ;

    QBDI::rword v1 = 0xab3672016bef61ae;
    QBDI::rword tmpStack[10] = {0};
    ExpectedMemoryAccesses expectedPush = {{
        { (QBDI::rword) &v1, v1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &tmpStack[8], v1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};
    ExpectedMemoryAccesses expectedPop = {{
        { (QBDI::rword) &tmpStack[8], v1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v1, v1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("PUSH64rmm", QBDI::POSTINST, checkAccess, &expectedPush);
    vm->addMnemonicCB("POP64rmm", QBDI::POSTINST, checkAccess, &expectedPop);

    QBDI::GPRState* state = vm->getGPRState();
    state->rax = (QBDI::rword) &v1;
    state->rbx = (QBDI::rword) &tmpStack[9];
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    for (auto& e: expectedPop.accesses)
        CHECK(e.see);
    for (auto& e: expectedPush.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-call_ret") {

    const char source[] = "    xchg %rsp, %rbx\n"
                          "    call test_call_ret_custom_call\n"
                          "    jmp test_call_ret_custom_end\n"
                          "test_call_ret_custom_call:\n"
                          "    ret\n"
                          "test_call_ret_custom_end:\n"
                          "    xchg %rsp, %rbx\n";

    QBDI::rword tmpStack[10] = {0};
    ExpectedMemoryAccesses expectedCall = {{
        { (QBDI::rword) &tmpStack[8], 0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};
    ExpectedMemoryAccesses expectedRet = {{
        { (QBDI::rword) &tmpStack[8], 0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("CALL*", QBDI::POSTINST, checkAccess, &expectedCall);
    vm->addMnemonicCB("RET*", QBDI::POSTINST, checkAccess, &expectedRet);

    QBDI::GPRState* state = vm->getGPRState();
    state->rbx = (QBDI::rword) &tmpStack[9];
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    for (auto& e: expectedCall.accesses)
        CHECK(e.see);
    for (auto& e: expectedRet.accesses)
        CHECK(e.see);
}

// COMPS MOVS SCAS LODS STOS
// REP and REPNE prefix

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-cmpsb") {

    const char source[] = "cmpsb\n";

    QBDI::rword v1 = 0xaa, v2 = 0x55;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v2, v2, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ);
    vm->addMnemonicCB("CMPSB", QBDI::PREINST, checkAccess, &expected);

    QBDI::GPRState* state = vm->getGPRState();
    state->rsi = (QBDI::rword) &v1;
    state->rdi = (QBDI::rword) &v2;
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-cmpsw") {

    const char source[] = "cmpsw\n";

    QBDI::rword v1 = 0x783, v2 = 0xbd7a;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v2, v2, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ);
    vm->addMnemonicCB("CMPSW", QBDI::PREINST, checkAccess, &expected);

    QBDI::GPRState* state = vm->getGPRState();
    state->rsi = (QBDI::rword) &v1;
    state->rdi = (QBDI::rword) &v2;
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-cmpsd") {

    const char source[] = "cmpsl\n";

    QBDI::rword v1 = 0x6ef9efbd, v2 = 0xef783b2a;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v2, v2, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ);
    vm->addMnemonicCB("CMPSL", QBDI::PREINST, checkAccess, &expected);

    QBDI::GPRState* state = vm->getGPRState();
    state->rsi = (QBDI::rword) &v1;
    state->rdi = (QBDI::rword) &v2;
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-cmpsq") {

    const char source[] = "cmpsq\n";

    QBDI::rword v1 = 0x6723870bdefa, v2 = 0x1234098765efdbac;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v2, v2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ);
    vm->addMnemonicCB("CMPSQ", QBDI::PREINST, checkAccess, &expected);

    QBDI::GPRState* state = vm->getGPRState();
    state->rsi = (QBDI::rword) &v1;
    state->rdi = (QBDI::rword) &v2;
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-rep_cmpsb") {

    const char source[] = "cld\n"
                          "rep cmpsb\n";

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

    QBDI::GPRState* state = vm->getGPRState();
    state->rsi = (QBDI::rword) &v1;
    state->rdi = (QBDI::rword) &v2;
    state->rcx = sizeof(v1);
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);

    for (auto& e: expectedPre.accesses)
        CHECK(e.see);

    for (auto& e: expectedPost.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-repne_cmpsb") {

    const char source[] = "cld\n"
                          "repne cmpsb\n";

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

    QBDI::GPRState* state = vm->getGPRState();
    state->rsi = (QBDI::rword) &v1;
    state->rdi = (QBDI::rword) &v2;
    state->rcx = sizeof(v1);
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);

    for (auto& e: expectedPre.accesses)
        CHECK(e.see);

    for (auto& e: expectedPost.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-rep_cmpsb2") {

    const char source[] = "std\n"
                          "rep cmpsb\n"
                          "cld\n";

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

    QBDI::GPRState* state = vm->getGPRState();
    state->rsi = (QBDI::rword) &v1[9];
    state->rdi = (QBDI::rword) &v2[9];
    state->rcx = sizeof(v1);
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);

    for (auto& e: expectedPre.accesses)
        CHECK(e.see);

    for (auto& e: expectedPost.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-rep_cmpsw") {

    const char source[] = "cld\n"
                          "rep cmpsw\n";

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

    QBDI::GPRState* state = vm->getGPRState();
    state->rsi = (QBDI::rword) &v1;
    state->rdi = (QBDI::rword) &v2;
    state->rcx = sizeof(v1) / sizeof(uint16_t);
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);

    for (auto& e: expectedPre.accesses)
        CHECK(e.see);

    for (auto& e: expectedPost.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-rep_cmpsw2") {

    const char source[] = "std\n"
                          "rep cmpsw\n"
                          "cld\n";

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

    QBDI::GPRState* state = vm->getGPRState();
    state->rsi = (QBDI::rword) &v1[4];
    state->rdi = (QBDI::rword) &v2[4];
    state->rcx = sizeof(v1) / sizeof(uint16_t);
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);

    for (auto& e: expectedPre.accesses)
        CHECK(e.see);

    for (auto& e: expectedPost.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-movsb") {

    const char source[] = "cld\n"
                          "movsb\n";

    QBDI::rword v1 = 0xbf, v2 = 0x78;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v2, v1, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("MOVSB", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm->getGPRState();
    state->rsi = (QBDI::rword) &v1;
    state->rdi = (QBDI::rword) &v2;
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(v2 == v1);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}


TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-movsw") {

    const char source[] = "cld\n"
                          "movsw\n";

    QBDI::rword v1 = 0x789f, v2 = 0xbd67;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v2, v1, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("MOVSW", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm->getGPRState();
    state->rsi = (QBDI::rword) &v1;
    state->rdi = (QBDI::rword) &v2;
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(v2 == v1);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-movsl") {

    const char source[] = "cld\n"
                          "movsl\n";

    QBDI::rword v1 = 0xa579eb9d, v2 = 0x2389befa;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v2, v1, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("MOVSL", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm->getGPRState();
    state->rsi = (QBDI::rword) &v1;
    state->rdi = (QBDI::rword) &v2;
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(v2 == v1);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-movsq") {

    const char source[] = "cld\n"
                          "movsq\n";

    QBDI::rword v1 = 0xb036789eb8ea, v2 = 0xab8e602baef846;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v2, v1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("MOVSQ", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm->getGPRState();
    state->rsi = (QBDI::rword) &v1;
    state->rdi = (QBDI::rword) &v2;
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(v2 == v1);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-movsb2") {

    const char source[] = "std\n"
                          "movsb\n"
                          "cld\n";

    QBDI::rword v1 = 0x8, v2 = 0x7f;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v2, v1, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("MOVSB", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm->getGPRState();
    state->rsi = (QBDI::rword) &v1;
    state->rdi = (QBDI::rword) &v2;
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(v2 == v1);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-movsw2") {

    const char source[] = "std\n"
                          "movsw\n"
                          "cld\n";

    QBDI::rword v1 = 0xad63, v2 = 0x6219;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v2, v1, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("MOVSW", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm->getGPRState();
    state->rsi = (QBDI::rword) &v1;
    state->rdi = (QBDI::rword) &v2;
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(v2 == v1);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-movsl2") {

    const char source[] = "std\n"
                          "movsl\n"
                          "cld\n";

    QBDI::rword v1 = 0xefa036db, v2 = 0xefd7137a;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v2, v1, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("MOVSL", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm->getGPRState();
    state->rsi = (QBDI::rword) &v1;
    state->rdi = (QBDI::rword) &v2;
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(v2 == v1);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-movsq2") {

    const char source[] = "std\n"
                          "movsq\n"
                          "cld\n";

    QBDI::rword v1 = 0x2360Abed083, v2 = 0xeb0367a801346;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v2, v1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("MOVSQ", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm->getGPRState();
    state->rsi = (QBDI::rword) &v1;
    state->rdi = (QBDI::rword) &v2;
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(v2 == v1);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-rep_movsl") {

    const char source[] = "cld\n"
                          "rep movsl\n";

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

    QBDI::GPRState* state = vm->getGPRState();
    state->rsi = (QBDI::rword) &v1;
    state->rdi = (QBDI::rword) &v2;
    state->rcx = sizeof(v1) / sizeof(uint32_t);
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    for (size_t i = 0; i < sizeof(v1) / sizeof(uint32_t); i++)
        CHECK(v2[i] == v1[i]);
    for (auto& e: expectedPre.accesses)
        CHECK(e.see);
    for (auto& e: expectedPost.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-rep_movsl2") {

    const char source[] = "std\n"
                          "rep movsl\n"
                          "cld\n";

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

    QBDI::GPRState* state = vm->getGPRState();
    state->rsi = (QBDI::rword) &v1[4];
    state->rdi = (QBDI::rword) &v2[4];
    state->rcx = sizeof(v1) / sizeof(uint32_t);
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    for (size_t i = 0; i < sizeof(v1) / sizeof(uint32_t); i++)
        CHECK(v2[i] == v1[i]);
    for (auto& e: expectedPre.accesses)
        CHECK(e.see);
    for (auto& e: expectedPost.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-scasb") {

    const char source[] = "cld\n"
                          "scasb\n";

    QBDI::rword v1 = 0x8, v2 = 0x6a;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v2, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("SCASB", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm->getGPRState();
    state->rsi = (QBDI::rword) v1;
    state->rdi = (QBDI::rword) &v2;
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-scasw") {

    const char source[] = "cld\n"
                          "scasw\n";

    QBDI::rword v1 = 0x5ef1, v2 = 0x6789;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v2, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("SCASW", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm->getGPRState();
    state->rsi = (QBDI::rword) v1;
    state->rdi = (QBDI::rword) &v2;
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-scasl") {

    const char source[] = "cld\n"
                          "scasl\n";

    QBDI::rword v1 = 0x629ebf, v2 = 0x1234567;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v2, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("SCASL", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm->getGPRState();
    state->rsi = (QBDI::rword) v1;
    state->rdi = (QBDI::rword) &v2;
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-scasq") {

    const char source[] = "cld\n"
                          "scasq\n";

    QBDI::rword v1 = 0x6efab792eb, v2 = 0xebaf719630145;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v2, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("SCASQ", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm->getGPRState();
    state->rsi = (QBDI::rword) v1;
    state->rdi = (QBDI::rword) &v2;
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-lodsb") {

    const char source[] = "cld\n"
                          "lodsb\n";

    QBDI::rword v1 = 0x6a;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("LODSB", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm->getGPRState();
    state->rsi = (QBDI::rword) &v1;
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(retval == v1);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-lodsw") {

    const char source[] = "cld\n"
                          "lodsw\n";

    QBDI::rword v1 = 0x6789;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("LODSW", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm->getGPRState();
    state->rsi = (QBDI::rword) &v1;
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(retval == v1);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-lodsl") {

    const char source[] = "cld\n"
                          "lodsl\n";

    QBDI::rword v1 = 0x1234567;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("LODSL", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm->getGPRState();
    state->rsi = (QBDI::rword) &v1;
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(retval == v1);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-lodsq") {

    const char source[] = "cld\n"
                          "lodsq\n";

    QBDI::rword v1 = 0xebaf719630145;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 8, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("LODSQ", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm->getGPRState();
    state->rsi = (QBDI::rword) &v1;
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(retval == v1);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-stosb") {

    const char source[] = "cld\n"
                          "stosb\n";

    QBDI::rword v1 = 0x8, v2 = 0x6a;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("STOSB", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm->getGPRState();
    state->rax = v1;
    state->rdi = (QBDI::rword) &v2;
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(v1 == v2);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-stosw") {

    const char source[] = "cld\n"
                          "stosw\n";

    QBDI::rword v1 = 0x5ef1, v2 = 0x6789;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("STOSW", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm->getGPRState();
    state->rax = v1;
    state->rdi = (QBDI::rword) &v2;
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(v1 == v2);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-stosl") {

    const char source[] = "cld\n"
                          "stosl\n";

    QBDI::rword v1 = 0x629ebf, v2 = 0x1234567;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("STOSL", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm->getGPRState();
    state->rax = v1;
    state->rdi = (QBDI::rword) &v2;
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(v1 == v2);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-stosq") {

    const char source[] = "cld\n"
                          "stosq\n";

    QBDI::rword v1 = 0x6efab792eb, v2 = 0xebaf719630145;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("STOSQ", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm->getGPRState();
    state->rax = v1;
    state->rdi = (QBDI::rword) &v2;
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(v1 == v2);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-stosb2") {

    const char source[] = "std\n"
                          "stosb\n"
                          "cld\n";

    QBDI::rword v1 = 0x8, v2 = 0x6a;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("STOSB", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm->getGPRState();
    state->rax = v1;
    state->rdi = (QBDI::rword) &v2;
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(v1 == v2);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-stosw2") {

    const char source[] = "std\n"
                          "stosw\n"
                          "cld\n";

    QBDI::rword v1 = 0x5ef1, v2 = 0x6789;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("STOSW", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm->getGPRState();
    state->rax = v1;
    state->rdi = (QBDI::rword) &v2;
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(v1 == v2);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-stosl2") {

    const char source[] = "std\n"
                          "stosl\n"
                          "cld\n";

    QBDI::rword v1 = 0x629ebf, v2 = 0x1234567;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("STOSL", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm->getGPRState();
    state->rax = v1;
    state->rdi = (QBDI::rword) &v2;
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(v1 == v2);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86_64-stosq2") {

    const char source[] = "std\n"
                          "stosq\n"
                          "cld\n";

    QBDI::rword v1 = 0x6efab792eb, v2 = 0xebaf719630145;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm->recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm->addMnemonicCB("STOSQ", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm->getGPRState();
    state->rax = v1;
    state->rdi = (QBDI::rword) &v2;
    vm->setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(v1 == v2);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

