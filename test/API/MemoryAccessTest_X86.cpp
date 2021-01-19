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

/*
static QBDI::VMAction debugCB(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState, QBDI::FPRState *fprState, void *data) {
    const QBDI::InstAnalysis* instAnalysis = vm->getInstAnalysis();
    printf("0x%x (%10s): %s\n", instAnalysis->address, instAnalysis->mnemonic, instAnalysis->disassembly);

    for (auto &a: vm->getInstMemoryAccess()) {
        printf(" - inst: 0x%x, addr: 0x%x, size: %d, type: %c%c, value: 0x%x, flags: 0x%x\n",
            a.instAddress, a.accessAddress, a.size,
            ((a.type & QBDI::MEMORY_READ) !=0 )?'r':'-',
            ((a.type & QBDI::MEMORY_WRITE) !=0 )?'w':'-',
            a.value, a.flags);
    }
    return QBDI::VMAction::CONTINUE;
}// */

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

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-push_pop_reg") {

    const char source[] = "xchg %esp, %ebx\n"
                          "push %eax\n"
                          "pop %eax\n"
                          "xchg %esp, %ebx\n" ;

    QBDI::rword v1 = 0x6bef61ae;
    QBDI::rword tmpStack[10] = {0};
    ExpectedMemoryAccesses expectedPush = {{
        { (QBDI::rword) &tmpStack[8], v1, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};
    ExpectedMemoryAccesses expectedPop = {{
        { (QBDI::rword) &tmpStack[8], v1, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("PUSH32r", QBDI::POSTINST, checkAccess, &expectedPush);
    vm.addMnemonicCB("POP32r", QBDI::PREINST, checkAccess, &expectedPop);

    QBDI::GPRState* state = vm.getGPRState();
    state->eax = v1;
    state->ebx = (QBDI::rword) &tmpStack[9];
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    for (auto& e: expectedPop.accesses)
        CHECK(e.see);
    for (auto& e: expectedPush.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-pusha_popa") {

    const char source[] = "xchg %esp, %ebx\n"
                          "pusha\n"
                          "popa\n"
                          "xchg %esp, %ebx\n" ;

    QBDI::rword tmpStack[10] = {0};
    ExpectedMemoryAccesses expectedPusha = {{
        { (QBDI::rword) &tmpStack[1], 0, 4 * 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
    }};
    ExpectedMemoryAccesses expectedPopa = {{
        { (QBDI::rword) &tmpStack[1], 0, 4 * 8, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("PUSHA32", QBDI::POSTINST, checkAccess, &expectedPusha);
    vm.addMnemonicCB("POPA32", QBDI::PREINST, checkAccess, &expectedPopa);

    QBDI::GPRState* state = vm.getGPRState();
    state->ebx = (QBDI::rword) &tmpStack[9];
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    for (auto& e: expectedPopa.accesses)
        CHECK(e.see);
    for (auto& e: expectedPusha.accesses)
        CHECK(e.see);
}


TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-push_pop_mem") {

    const char source[] = "xchg %esp, %ebx\n"
                          "push (%eax)\n"
                          "pop (%eax)\n"
                          "xchg %esp, %ebx\n" ;

    QBDI::rword v1 = 0xab367201;
    QBDI::rword tmpStack[10] = {0};
    ExpectedMemoryAccesses expectedPushPre = {{
        { (QBDI::rword) &v1, v1, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};
    ExpectedMemoryAccesses expectedPushPost = {{
        { (QBDI::rword) &v1, v1, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &tmpStack[8], v1, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};
    ExpectedMemoryAccesses expectedPopPre = {{
        { (QBDI::rword) &tmpStack[8], v1, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};
    ExpectedMemoryAccesses expectedPopPost = {{
        { (QBDI::rword) &tmpStack[8], v1, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v1, v1, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("PUSH32rmm", QBDI::PREINST, checkAccess, &expectedPushPre);
    vm.addMnemonicCB("PUSH32rmm", QBDI::POSTINST, checkAccess, &expectedPushPost);
    vm.addMnemonicCB("POP32rmm", QBDI::PREINST, checkAccess, &expectedPopPre);
    vm.addMnemonicCB("POP32rmm", QBDI::POSTINST, checkAccess, &expectedPopPost);

    QBDI::GPRState* state = vm.getGPRState();
    state->eax = (QBDI::rword) &v1;
    state->ebx = (QBDI::rword) &tmpStack[9];
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    for (auto& e: expectedPopPre.accesses)
        CHECK(e.see);
    for (auto& e: expectedPopPost.accesses)
        CHECK(e.see);
    for (auto& e: expectedPushPre.accesses)
        CHECK(e.see);
    for (auto& e: expectedPushPost.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-call_ret") {

    const char source[] = "    xchg %esp, %ebx\n"
                          "    call test_call_ret_custom_call\n"
                          "    jmp test_call_ret_custom_end\n"
                          "test_call_ret_custom_call:\n"
                          "    ret\n"
                          "test_call_ret_custom_end:\n"
                          "    xchg %esp, %ebx\n";

    QBDI::rword tmpStack[10] = {0};
    ExpectedMemoryAccesses expectedCall = {{
        { (QBDI::rword) &tmpStack[8], 0, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};
    ExpectedMemoryAccesses expectedRet = {{
        { (QBDI::rword) &tmpStack[8], 0, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("CALL*", QBDI::POSTINST, checkAccess, &expectedCall);
    vm.addMnemonicCB("RET*", QBDI::PREINST, checkAccess, &expectedRet);

    QBDI::GPRState* state = vm.getGPRState();
    state->ebx = (QBDI::rword) &tmpStack[9];
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    for (auto& e: expectedCall.accesses)
        CHECK(e.see);
    for (auto& e: expectedRet.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-movao") {

    const char source[] = "mov %gs:0x0, %eax\n";

    ExpectedMemoryAccesses expected = {{
        { 0, 0, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ);
    vm.addMnemonicCB("MOV32ao32", QBDI::PREINST, checkAccess, &expected);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

// COMPS MOVS SCAS LODS STOS
// REP and REPNE prefix

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-cmpsb") {

    const char source[] = "cmpsb\n";

    QBDI::rword v1 = 0xaa, v2 = 0x55;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v2, v2, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ);
    vm.addMnemonicCB("CMPSB", QBDI::PREINST, checkAccess, &expected);

    QBDI::GPRState* state = vm.getGPRState();
    state->esi = (QBDI::rword) &v1;
    state->edi = (QBDI::rword) &v2;
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-cmpsw") {

    const char source[] = "cmpsw\n";

    QBDI::rword v1 = 0x783, v2 = 0xbd7a;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v2, v2, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ);
    vm.addMnemonicCB("CMPSW", QBDI::PREINST, checkAccess, &expected);

    QBDI::GPRState* state = vm.getGPRState();
    state->esi = (QBDI::rword) &v1;
    state->edi = (QBDI::rword) &v2;
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-cmpsd") {

    const char source[] = "cmpsl\n";

    QBDI::rword v1 = 0x6ef9efbd, v2 = 0xef783b2a;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v2, v2, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ);
    vm.addMnemonicCB("CMPSL", QBDI::PREINST, checkAccess, &expected);

    QBDI::GPRState* state = vm.getGPRState();
    state->esi = (QBDI::rword) &v1;
    state->edi = (QBDI::rword) &v2;
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-rep_cmpsb") {

    const char source[] = "cld\n"
                          "rep cmpsb\n";

    uint8_t v1[10] = {0x56, 0x78, 0x89, 0xab, 0xe6, 0xe7, 0x1a, 0xfa, 0xc8, 0x6d};
    uint8_t v2[10] = {0x56, 0x78, 0x89, 0xab, 0xe6, 0xe7, 0x1a, 0xfa, 0xc8, 0x6c};
    ExpectedMemoryAccesses expectedPre = {{
        { (QBDI::rword) &v1, 0, 0, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE | QBDI::MEMORY_UNKNOWN_SIZE},
        { (QBDI::rword) &v2, 0, 0, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE | QBDI::MEMORY_UNKNOWN_SIZE},
    }};
    ExpectedMemoryAccesses expectedPost = {{
        { (QBDI::rword) &v1, 0, 10, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
        { (QBDI::rword) &v2, 0, 10, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ);
    vm.addMnemonicCB("CMPSB", QBDI::PREINST, checkAccess, &expectedPre);
    vm.addMnemonicCB("CMPSB", QBDI::POSTINST, checkAccess, &expectedPost);

    QBDI::GPRState* state = vm.getGPRState();
    state->esi = (QBDI::rword) &v1;
    state->edi = (QBDI::rword) &v2;
    state->ecx = sizeof(v1);
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);

    for (auto& e: expectedPre.accesses)
        CHECK(e.see);

    for (auto& e: expectedPost.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-repne_cmpsb") {

    const char source[] = "cld\n"
                          "repne cmpsb\n";

    uint8_t v1[10] = {0x56, 0x78, 0x89, 0xab, 0xe6, 0xe7, 0x1a, 0xfa, 0xc8, 0x6d};
    uint8_t v2[10] = {0xb1, 0x5, 0x98, 0xae, 0xe2, 0xe6, 0x19, 0xf9, 0xc7, 0x6d};
    ExpectedMemoryAccesses expectedPre = {{
        { (QBDI::rword) &v1, 0, 0, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE | QBDI::MEMORY_UNKNOWN_SIZE},
        { (QBDI::rword) &v2, 0, 0, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE | QBDI::MEMORY_UNKNOWN_SIZE},
    }};
    ExpectedMemoryAccesses expectedPost = {{
        { (QBDI::rword) &v1, 0, 10, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
        { (QBDI::rword) &v2, 0, 10, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ);
    vm.addMnemonicCB("CMPSB", QBDI::PREINST, checkAccess, &expectedPre);
    vm.addMnemonicCB("CMPSB", QBDI::POSTINST, checkAccess, &expectedPost);

    QBDI::GPRState* state = vm.getGPRState();
    state->esi = (QBDI::rword) &v1;
    state->edi = (QBDI::rword) &v2;
    state->ecx = sizeof(v1);
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);

    for (auto& e: expectedPre.accesses)
        CHECK(e.see);

    for (auto& e: expectedPost.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-rep_cmpsb2") {

    const char source[] = "std\n"
                          "rep cmpsb\n"
                          "cld\n";

    uint8_t v1[10] = {0x5c, 0x78, 0x89, 0xab, 0xe6, 0xe7, 0x1a, 0xfa, 0xc8, 0x6c};
    uint8_t v2[10] = {0x56, 0x78, 0x89, 0xab, 0xe6, 0xe7, 0x1a, 0xfa, 0xc8, 0x6c};
    ExpectedMemoryAccesses expectedPre = {{
        { (QBDI::rword) &v1[9], 0, 0, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE | QBDI::MEMORY_UNKNOWN_SIZE},
        { (QBDI::rword) &v2[9], 0, 0, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE | QBDI::MEMORY_UNKNOWN_SIZE},
    }};
    ExpectedMemoryAccesses expectedPost = {{
        { (QBDI::rword) &v1, 0, 10, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
        { (QBDI::rword) &v2, 0, 10, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ);
    vm.addMnemonicCB("CMPSB", QBDI::PREINST, checkAccess, &expectedPre);
    vm.addMnemonicCB("CMPSB", QBDI::POSTINST, checkAccess, &expectedPost);

    QBDI::GPRState* state = vm.getGPRState();
    state->esi = (QBDI::rword) &v1[9];
    state->edi = (QBDI::rword) &v2[9];
    state->ecx = sizeof(v1);
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);

    for (auto& e: expectedPre.accesses)
        CHECK(e.see);

    for (auto& e: expectedPost.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-rep_cmpsw") {

    const char source[] = "cld\n"
                          "rep cmpsw\n";

    uint16_t v1[5] = {0x5c78, 0x89ab, 0xe6e7, 0x1afa, 0xc86c};
    uint16_t v2[5] = {0x5c78, 0x89ab, 0xe6e7, 0x1afa, 0xc86d};
    ExpectedMemoryAccesses expectedPre = {{
        { (QBDI::rword) &v1, 0, 0, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE | QBDI::MEMORY_UNKNOWN_SIZE},
        { (QBDI::rword) &v2, 0, 0, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE | QBDI::MEMORY_UNKNOWN_SIZE},
    }};
    ExpectedMemoryAccesses expectedPost = {{
        { (QBDI::rword) &v1, 0, 10, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
        { (QBDI::rword) &v2, 0, 10, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ);
    vm.addMnemonicCB("CMPSW", QBDI::PREINST, checkAccess, &expectedPre);
    vm.addMnemonicCB("CMPSW", QBDI::POSTINST, checkAccess, &expectedPost);

    QBDI::GPRState* state = vm.getGPRState();
    state->esi = (QBDI::rword) &v1;
    state->edi = (QBDI::rword) &v2;
    state->ecx = sizeof(v1) / sizeof(uint16_t);
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);

    for (auto& e: expectedPre.accesses)
        CHECK(e.see);

    for (auto& e: expectedPost.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-rep_cmpsw2") {

    const char source[] = "std\n"
                          "rep cmpsw\n"
                          "cld\n";

    uint16_t v1[5] = {0x5c78, 0x89ab, 0xe6e7, 0x1afa, 0xc86c};
    uint16_t v2[5] = {0x5678, 0x89ab, 0xe6e7, 0x1afa, 0xc86c};
    ExpectedMemoryAccesses expectedPre = {{
        { (QBDI::rword) &v1[4], 0, 0, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE | QBDI::MEMORY_UNKNOWN_SIZE},
        { (QBDI::rword) &v2[4], 0, 0, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE | QBDI::MEMORY_UNKNOWN_SIZE},
    }};
    ExpectedMemoryAccesses expectedPost = {{
        { (QBDI::rword) &v1, 0, 10, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
        { (QBDI::rword) &v2, 0, 10, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ);
    vm.addMnemonicCB("CMPSW", QBDI::PREINST, checkAccess, &expectedPre);
    vm.addMnemonicCB("CMPSW", QBDI::POSTINST, checkAccess, &expectedPost);

    QBDI::GPRState* state = vm.getGPRState();
    state->esi = (QBDI::rword) &v1[4];
    state->edi = (QBDI::rword) &v2[4];
    state->ecx = sizeof(v1) / sizeof(uint16_t);
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);

    for (auto& e: expectedPre.accesses)
        CHECK(e.see);

    for (auto& e: expectedPost.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-movsb") {

    const char source[] = "cld\n"
                          "movsb\n";

    QBDI::rword v1 = 0xbf, v2 = 0x78;
    ExpectedMemoryAccesses expectedPre = {{
        { (QBDI::rword) &v1, v1, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};
    ExpectedMemoryAccesses expectedPost = {{
        { (QBDI::rword) &v2, v1, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v1, v1, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("MOVSB", QBDI::PREINST, checkAccess, &expectedPre);
    vm.addMnemonicCB("MOVSB", QBDI::POSTINST, checkAccess, &expectedPost);

    QBDI::GPRState* state = vm.getGPRState();
    state->esi = (QBDI::rword) &v1;
    state->edi = (QBDI::rword) &v2;
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(v2 == v1);
    for (auto& e: expectedPre.accesses)
        CHECK(e.see);
    for (auto& e: expectedPost.accesses)
        CHECK(e.see);
}


TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-movsw") {

    const char source[] = "cld\n"
                          "movsw\n";

    QBDI::rword v1 = 0x789f, v2 = 0xbd67;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v1, v1, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("MOVSW", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm.getGPRState();
    state->esi = (QBDI::rword) &v1;
    state->edi = (QBDI::rword) &v2;
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(v2 == v1);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-movsl") {

    const char source[] = "cld\n"
                          "movsl\n";

    QBDI::rword v1 = 0xa579eb9d, v2 = 0x2389befa;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v1, v1, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("MOVSL", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm.getGPRState();
    state->esi = (QBDI::rword) &v1;
    state->edi = (QBDI::rword) &v2;
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(v2 == v1);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-movsb2") {

    const char source[] = "std\n"
                          "movsb\n"
                          "cld\n";

    QBDI::rword v1 = 0x8, v2 = 0x7f;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v1, v1, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("MOVSB", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm.getGPRState();
    state->esi = (QBDI::rword) &v1;
    state->edi = (QBDI::rword) &v2;
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(v2 == v1);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-movsw2") {

    const char source[] = "std\n"
                          "movsw\n"
                          "cld\n";

    QBDI::rword v1 = 0xad63, v2 = 0x6219;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v1, v1, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("MOVSW", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm.getGPRState();
    state->esi = (QBDI::rword) &v1;
    state->edi = (QBDI::rword) &v2;
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(v2 == v1);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-movsl2") {

    const char source[] = "std\n"
                          "movsl\n"
                          "cld\n";

    QBDI::rword v1 = 0xefa036db, v2 = 0xefd7137a;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &v1, v1, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("MOVSL", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm.getGPRState();
    state->esi = (QBDI::rword) &v1;
    state->edi = (QBDI::rword) &v2;
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(v2 == v1);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-rep_movsl") {

    const char source[] = "cld\n"
                          "rep movsl\n";

    uint32_t v1[5] = {0xab673, 0xeba9256, 0x638feba8, 0x7182faB, 0x7839021b};
    uint32_t v2[5] = {0};
    ExpectedMemoryAccesses expectedPre = {{
        { (QBDI::rword) &v1, 0, 0, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE | QBDI::MEMORY_UNKNOWN_SIZE},
    }};
    ExpectedMemoryAccesses expectedPost = {{
        { (QBDI::rword) &v2, 0, sizeof(v1), QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
        { (QBDI::rword) &v1, 0, sizeof(v1), QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("MOVSL", QBDI::PREINST, checkAccess, &expectedPre);
    vm.addMnemonicCB("MOVSL", QBDI::POSTINST, checkAccess, &expectedPost);

    QBDI::GPRState* state = vm.getGPRState();
    state->esi = (QBDI::rword) &v1;
    state->edi = (QBDI::rword) &v2;
    state->ecx = sizeof(v1) / sizeof(uint32_t);
    vm.setGPRState(state);

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

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-rep_movsl2") {

    const char source[] = "std\n"
                          "rep movsl\n"
                          "cld\n";

    uint32_t v1[5] = {0xab673, 0xeba9256, 0x638feba8, 0x7182faB, 0x7839021b};
    uint32_t v2[5] = {0};
    ExpectedMemoryAccesses expectedPre = {{
        { (QBDI::rword) &v1[4], 0, 0, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE | QBDI::MEMORY_UNKNOWN_SIZE},
    }};
    ExpectedMemoryAccesses expectedPost = {{
        { (QBDI::rword) &v2, 0, sizeof(v1), QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
        { (QBDI::rword) &v1, 0, sizeof(v1), QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("MOVSL", QBDI::PREINST, checkAccess, &expectedPre);
    vm.addMnemonicCB("MOVSL", QBDI::POSTINST, checkAccess, &expectedPost);

    QBDI::GPRState* state = vm.getGPRState();
    state->esi = (QBDI::rword) &v1[4];
    state->edi = (QBDI::rword) &v2[4];
    state->ecx = sizeof(v1) / sizeof(uint32_t);
    vm.setGPRState(state);

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

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-scasb") {

    const char source[] = "cld\n"
                          "scasb\n";

    QBDI::rword v1 = 0x8, v2 = 0x6a;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v2, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("SCASB", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm.getGPRState();
    state->esi = (QBDI::rword) v1;
    state->edi = (QBDI::rword) &v2;
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-scasw") {

    const char source[] = "cld\n"
                          "scasw\n";

    QBDI::rword v1 = 0x5ef1, v2 = 0x6789;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v2, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("SCASW", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm.getGPRState();
    state->esi = (QBDI::rword) v1;
    state->edi = (QBDI::rword) &v2;
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-scasl") {

    const char source[] = "cld\n"
                          "scasl\n";

    QBDI::rword v1 = 0x629ebf, v2 = 0x1234567;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v2, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("SCASL", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm.getGPRState();
    state->esi = (QBDI::rword) v1;
    state->edi = (QBDI::rword) &v2;
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-lodsb") {

    const char source[] = "cld\n"
                          "lodsb\n";

    QBDI::rword v1 = 0x6a;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("LODSB", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm.getGPRState();
    state->esi = (QBDI::rword) &v1;
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(retval == v1);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-lodsw") {

    const char source[] = "cld\n"
                          "lodsw\n";

    QBDI::rword v1 = 0x6789;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 2, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("LODSW", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm.getGPRState();
    state->esi = (QBDI::rword) &v1;
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(retval == v1);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-lodsl") {

    const char source[] = "cld\n"
                          "lodsl\n";

    QBDI::rword v1 = 0x1234567;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v1, v1, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("LODSL", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm.getGPRState();
    state->esi = (QBDI::rword) &v1;
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(retval == v1);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-stosb") {

    const char source[] = "cld\n"
                          "stosb\n";

    QBDI::rword v1 = 0x8, v2 = 0x6a;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("STOSB", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm.getGPRState();
    state->eax = v1;
    state->edi = (QBDI::rword) &v2;
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(v1 == v2);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-stosw") {

    const char source[] = "cld\n"
                          "stosw\n";

    QBDI::rword v1 = 0x5ef1, v2 = 0x6789;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("STOSW", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm.getGPRState();
    state->eax = v1;
    state->edi = (QBDI::rword) &v2;
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(v1 == v2);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-stosl") {

    const char source[] = "cld\n"
                          "stosl\n";

    QBDI::rword v1 = 0x629ebf, v2 = 0x1234567;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("STOSL", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm.getGPRState();
    state->eax = v1;
    state->edi = (QBDI::rword) &v2;
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(v1 == v2);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-stosb2") {

    const char source[] = "std\n"
                          "stosb\n"
                          "cld\n";

    QBDI::rword v1 = 0x8, v2 = 0x6a;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 1, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("STOSB", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm.getGPRState();
    state->eax = v1;
    state->edi = (QBDI::rword) &v2;
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(v1 == v2);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-stosw2") {

    const char source[] = "std\n"
                          "stosw\n"
                          "cld\n";

    QBDI::rword v1 = 0x5ef1, v2 = 0x6789;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 2, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("STOSW", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm.getGPRState();
    state->eax = v1;
    state->edi = (QBDI::rword) &v2;
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(v1 == v2);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-stosl2") {

    const char source[] = "std\n"
                          "stosl\n"
                          "cld\n";

    QBDI::rword v1 = 0x629ebf, v2 = 0x1234567;
    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v2, v1, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("STOSL", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm.getGPRState();
    state->eax = v1;
    state->edi = (QBDI::rword) &v2;
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(v1 == v2);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-movzx") {

    const char source[] = "movzbl  0x5(%ebx), %eax\n";

    uint8_t v[] = {0xeb, 0xaf, 0x71, 0x96, 0x30, 0x14, 0x52, 0xce};

    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v[5], v[5], 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("MOVZX32rm8", QBDI::POSTINST, checkAccess, &expected);

    QBDI::GPRState* state = vm.getGPRState();
    state->eax = 0xfab792eb;
    state->ebx = (QBDI::rword) &v;
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(state->eax == v[5]);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-addmr") {

    const char source[] = "addl %eax, (%ebx)";

    const uint32_t v1 = 0xebaf7196;
    const uint32_t v2 = 0xfab792eb;
    uint32_t buff = v1;

    ExpectedMemoryAccesses expectedPre = {{
        { (QBDI::rword) &buff, v1, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};
    ExpectedMemoryAccesses expectedPost = {{
        { (QBDI::rword) &buff, v1, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &buff, v1 + v2, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("ADD32mr", QBDI::PREINST, checkAccess, &expectedPre);
    vm.addMnemonicCB("ADD32mr", QBDI::POSTINST, checkAccess, &expectedPost);

    QBDI::GPRState* state = vm.getGPRState();
    state->eax = v2;
    state->ebx = (QBDI::rword) &buff;
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(buff == v1 + v2);
    for (auto& e: expectedPre.accesses)
        CHECK(e.see);
    for (auto& e: expectedPost.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-xchgrm") {

    const char source[] = "xchgl %eax, (%eax)";

    const uint32_t v1 = 0x96761ef1;
    uint32_t buff = v1;

    ExpectedMemoryAccesses expectedPre = {{
        { (QBDI::rword) &buff, v1, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};
    ExpectedMemoryAccesses expectedPost = {{
        { (QBDI::rword) &buff, (QBDI::rword) &buff, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
        { (QBDI::rword) &buff, v1, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("XCHG32rm", QBDI::PREINST, checkAccess, &expectedPre);
    vm.addMnemonicCB("XCHG32rm", QBDI::POSTINST, checkAccess, &expectedPost);

    QBDI::GPRState* state = vm.getGPRState();
    state->eax = (QBDI::rword) &buff;
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(state->eax == v1);
    CHECK(buff == (uint32_t) &buff);
    for (auto& e: expectedPre.accesses)
        CHECK(e.see);
    for (auto& e: expectedPost.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-enter-leave") {

    const char source[] = "xchg %esp, %ebx\n"
                          "enter $0x0, $0x0\n"
                          "leave\n"
                          "xchg %esp, %ebx\n";

    const uint64_t v = 0x819abe76;
    QBDI::rword tmpStack[10] = {0};

    ExpectedMemoryAccesses expectedEnter = {{
        { (QBDI::rword) &tmpStack[8], v, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};
    ExpectedMemoryAccesses expectedLeave = {{
        { (QBDI::rword) &tmpStack[8], v, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("ENTER", QBDI::POSTINST, checkAccess, &expectedEnter);
    vm.addMnemonicCB("LEAVE*", QBDI::PREINST, checkAccess, &expectedLeave);

    QBDI::GPRState* state = vm.getGPRState();
    state->ebx = (QBDI::rword) &tmpStack[9];
    state->ebp = v;
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(state->ebp == v);
    CHECK(state->ebx == (QBDI::rword) &tmpStack[9]);
    for (auto& e: expectedEnter.accesses)
        CHECK(e.see);
    for (auto& e: expectedLeave.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-fld-fstp") {

    const char source[] = "flds	(%eax)\n"
                          "fldl (%ebx)\n"
                          "movl	$0x0, (%eax)\n"
                          "movl	$0x0, (%ebx)\n"
                          "fstpl (%ebx)\n"
                          "fstps (%eax)\n";

    const uint32_t v1 = 0x416ac41e;
    const uint64_t v2 = 0x79819abe76;
    uint32_t buff1 = v1;
    uint64_t buff2 = v2;

    ExpectedMemoryAccesses expectedLoad32 = {{
        { (QBDI::rword) &buff1, v1, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};
    ExpectedMemoryAccesses expectedLoad64 = {{
        { (QBDI::rword) &buff2, 0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
    }};
    ExpectedMemoryAccesses expectedStore64 = {{
        { (QBDI::rword) &buff2, 0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
    }};
    ExpectedMemoryAccesses expectedStore32 = {{
        { (QBDI::rword) &buff1, v1, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("LD_F32m", QBDI::PREINST, checkAccess, &expectedLoad32);
    vm.addMnemonicCB("LD_F64m", QBDI::PREINST, checkAccess, &expectedLoad64);
    vm.addMnemonicCB("ST_FP64m", QBDI::POSTINST, checkAccess, &expectedStore64);
    vm.addMnemonicCB("ST_FP32m", QBDI::POSTINST, checkAccess, &expectedStore32);

    QBDI::GPRState* state = vm.getGPRState();
    state->eax = (QBDI::rword) &buff1;
    state->ebx = (QBDI::rword) &buff2;
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(buff1 == v1);
    CHECK(buff2 == v2);
    for (auto& e: expectedLoad32.accesses)
        CHECK(e.see);
    for (auto& e: expectedLoad64.accesses)
        CHECK(e.see);
    for (auto& e: expectedStore64.accesses)
        CHECK(e.see);
    for (auto& e: expectedStore32.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-movapd") {

    const char source[] = "movapd	(%eax), %xmm1\n"
                          "movapd %xmm2, (%ebx)\n";

    const uint8_t v1[16] = {0x41, 0x6a, 0xc4, 0x1e, 0x14, 0xa9, 0x5d, 0x27, 0x67, 0x4f, 0x91, 0x6e, 0x4b, 0x57, 0x4d, 0xc9};
    const uint8_t v2[16] = {0xa9, 0x5d, 0x27, 0x6a, 0xc4, 0x91, 0x6e, 0x4b, 0x57, 0x4d, 0x41, 0x6a, 0x0e, 0x80, 0xeb, 0xad};
    QBDI_ALIGNED(16) uint8_t buff1[16];
    QBDI_ALIGNED(16) uint8_t buff2[16] = {0};

    memcpy(buff1, v1, sizeof(v1));

    ExpectedMemoryAccesses expectedLoad = {{
        { (QBDI::rword) &buff1, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
    }};
    ExpectedMemoryAccesses expectedStore = {{
        { (QBDI::rword) &buff2, 0, 16, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("MOVAPDrm", QBDI::PREINST, checkAccess, &expectedLoad);
    vm.addMnemonicCB("MOVAPDmr", QBDI::POSTINST, checkAccess, &expectedStore);

    QBDI::GPRState* state = vm.getGPRState();
    state->eax = (QBDI::rword) &buff1;
    state->ebx = (QBDI::rword) &buff2;
    vm.setGPRState(state);

    QBDI::FPRState* fstate = vm.getFPRState();
    memset(fstate->xmm1, '\x00', sizeof(v1));
    memcpy(fstate->xmm2, v2, sizeof(v2));
    vm.setFPRState(fstate);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(memcmp(fstate->xmm2, buff2, sizeof(v2)) == 0);
    CHECK(memcmp(fstate->xmm1, v1, sizeof(v1)) == 0);
    for (auto& e: expectedLoad.accesses)
        CHECK(e.see);
    for (auto& e: expectedStore.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-maskmovdqu") {

    const char source[] = "maskmovdqu	%xmm1, %xmm0\n";

    const uint8_t v1[16] = {0x41, 0x6a, 0xc4, 0x1e, 0x14, 0xa9, 0x5d, 0x27, 0x67, 0x4f, 0x91, 0x6e, 0x4b, 0x57, 0x4d, 0xc9};
    const uint8_t v2[16] = {0xa9, 0x5d, 0x27, 0x6a, 0xc4, 0x91, 0x6e, 0x4b, 0x57, 0x4d, 0x41, 0x6a, 0x0e, 0x80, 0xeb, 0xad};
    const uint8_t mask[16] = {0x80, 0x80, 0x80, 0x80, 0x0, 0x80, 0x0, 0x80, 0x80, 0x0, 0x80, 0x0, 0x0, 0x80, 0x80, 0x0};
    QBDI_ALIGNED(16) uint8_t buff1[16];

    memcpy(buff1, v1, sizeof(v1));

    ExpectedMemoryAccesses expectedPre = {{
        { (QBDI::rword) &buff1, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
    }};
    ExpectedMemoryAccesses expectedPost = {{
        { (QBDI::rword) &buff1, 0, 16, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
        { (QBDI::rword) &buff1, 0, 16, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("MASKMOVDQU", QBDI::PREINST, checkAccess, &expectedPre);
    vm.addMnemonicCB("MASKMOVDQU", QBDI::POSTINST, checkAccess, &expectedPost);

    QBDI::GPRState* state = vm.getGPRState();
    state->edi = (QBDI::rword) &buff1;
    vm.setGPRState(state);

    QBDI::FPRState* fstate = vm.getFPRState();
    memcpy(fstate->xmm0, v2, sizeof(v2));
    memcpy(fstate->xmm1, mask, sizeof(mask));
    vm.setFPRState(fstate);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    for (unsigned i = 0; i < sizeof(buff1); i++)
        CHECK( ((mask[i] == 0)?v1[i]:v2[i]) == buff1[i]);

    for (auto& e: expectedPre.accesses)
        CHECK(e.see);
    for (auto& e: expectedPost.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-maskmovq") {

    const char source[] = "maskmovq	%mm1, %mm0\n";

    const uint8_t v1[8] = {0x41, 0x6a, 0xc4, 0x1e, 0x14, 0xa9, 0x5d, 0x27};
    const uint8_t v2[8] = {0xa9, 0x5d, 0x27, 0x6a, 0xc4, 0x91, 0x6e, 0x4b};
    const uint8_t mask[8] = {0x80, 0x80, 0x80, 0x80, 0x0, 0x80, 0x0, 0x80};
    QBDI_ALIGNED(8) uint8_t buff1[8];

    memcpy(buff1, v1, sizeof(v1));

    ExpectedMemoryAccesses expectedPre = {{
        { (QBDI::rword) &buff1, 0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
    }};
    ExpectedMemoryAccesses expectedPost = {{
        { (QBDI::rword) &buff1, 0, 8, QBDI::MEMORY_READ, QBDI::MEMORY_UNKNOWN_VALUE},
        { (QBDI::rword) &buff1, 0, 8, QBDI::MEMORY_WRITE, QBDI::MEMORY_UNKNOWN_VALUE},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("MMX_MASKMOVQ", QBDI::PREINST, checkAccess, &expectedPre);
    vm.addMnemonicCB("MMX_MASKMOVQ", QBDI::POSTINST, checkAccess, &expectedPost);

    QBDI::GPRState* state = vm.getGPRState();
    state->edi = (QBDI::rword) &buff1;
    vm.setGPRState(state);

    QBDI::FPRState* fstate = vm.getFPRState();
    memcpy(fstate->stmm0.reg, v2, sizeof(v2));
    memcpy(fstate->stmm1.reg, mask, sizeof(mask));
    vm.setFPRState(fstate);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    for (unsigned i = 0; i < sizeof(buff1); i++)
        CHECK( ((mask[i] == 0)?v1[i]:v2[i]) == buff1[i]);

    for (auto& e: expectedPre.accesses)
        CHECK(e.see);
    for (auto& e: expectedPost.accesses)
        CHECK(e.see);
}

TEST_CASE_METHOD(MemoryAccessTest, "MemoryAccessTest_X86-xlat") {

    const char source[] = "xlatb\n";

    const uint8_t v[8] = {0x41, 0x6a, 0xc4, 0x1e, 0x14, 0xa9, 0x5d, 0x27};

    ExpectedMemoryAccesses expected = {{
        { (QBDI::rword) &v[5], v[5], 1, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
    }};

    vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
    vm.addMnemonicCB("XLAT", QBDI::PREINST, checkAccess, &expected);

    QBDI::GPRState* state = vm.getGPRState();
    state->ebx = (QBDI::rword) &v;
    state->eax = 5;
    vm.setGPRState(state);

    QBDI::rword retval;
    bool ran = runOnASM(&retval, source);

    CHECK(ran);
    CHECK(v[5] == vm.getGPRState()->eax);
    for (auto& e: expected.accesses)
        CHECK(e.see);
}
