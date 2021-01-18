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
#include <algorithm>
#include <catch2/catch.hpp>
#include "VMTest.h"

#include "inttypes.h"

#include "Utility/String.h"
#include "Platform.h"
#include "Memory.hpp"

#define STACK_SIZE 4096
#define FAKE_RET_ADDR 0x666


QBDI_NOINLINE int dummyFun0() {
	return 42;
}

QBDI_NOINLINE int dummyFun1(int arg0) {
	return arg0;
}

QBDI_NOINLINE int dummyFun4(int arg0, int arg1, int arg2, int arg3) {
	return arg0 + arg1 + arg2 + arg3;
}

QBDI_NOINLINE int dummyFun5(int arg0, int arg1, int arg2, int arg3, int arg4) {
	return arg0 + arg1 + arg2 + arg3 + arg4;
}

QBDI_NOINLINE int dummyFun8(int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7) {
	return arg0 + arg1 + arg2 + arg3 + arg4 + arg5 + arg6 + arg7;
}

QBDI_NOINLINE int dummyFunCall(int arg0) {
    // use simple BUT multiplatform functions to test external calls
    uint8_t* useless = (uint8_t*) QBDI::alignedAlloc(256, 16);
    if (useless) {
        *(int*) useless = arg0;
        QBDI::alignedFree(useless);
    }
    return dummyFun1(arg0);
}


#if defined(QBDI_ARCH_X86) || defined(QBDI_ARCH_X86_64)
#define MNEM_COUNT 5u
#define MNEM_VALIDATION 140u
#elif defined(QBDI_ARCH_ARM)
#define MNEM_COUNT 1u
#define MNEM_VALIDATION 25u
#endif
#define MNEM_IMM_SHORT_VAL 66
#define MNEM_IMM_VAL 42424242
#define MNEM_IMM_SHORT_STRVAL "66"
#define MNEM_IMM_STRVAL "42424242"

struct TestInst {
    uint32_t instSize;
    uint8_t numOperands;
    bool isCompare;
    QBDI::RegisterAccessType flagsAccess;
    QBDI::OperandAnalysis operands[6];
};

#if defined(QBDI_ARCH_X86) || defined(QBDI_ARCH_X86_64)
struct TestInst TestInsts[MNEM_COUNT] = {
    {3, 2, true, QBDI::REGISTER_WRITE, {
           {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 1, 8, 3, "DH", QBDI::REGISTER_READ},
           {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_NONE, MNEM_IMM_SHORT_VAL, 1, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
        }
    },
#if defined(QBDI_ARCH_X86_64)
    {3, 2, true, QBDI::REGISTER_WRITE, {
           {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 0, "RAX", QBDI::REGISTER_READ},
           {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 1, "RBX", QBDI::REGISTER_READ},
        }
    },
#else
    {3, 2, true, QBDI::REGISTER_WRITE, {
           {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 2, 0, 0, "AX", QBDI::REGISTER_READ},
           {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 2, 0, 1, "BX", QBDI::REGISTER_READ},
        }
    },
#endif
    {5, 2, true, QBDI::REGISTER_WRITE, {
           {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_NONE, MNEM_IMM_VAL, 4, 0, -1, nullptr, QBDI::REGISTER_UNUSED},
           {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 0, "EAX", QBDI::REGISTER_READ},
        }
    },
    {1, 5, false, QBDI::REGISTER_READ | QBDI::REGISTER_WRITE, {
           {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, sizeof(QBDI::rword), 0, 5, QBDI::GPR_NAMES[5], QBDI::REGISTER_READ},
           {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, sizeof(QBDI::rword), 0, 4, QBDI::GPR_NAMES[4], QBDI::REGISTER_READ},
           {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_NONE, 0, 0, 0, -1, NULL, QBDI::REGISTER_UNUSED},
           {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 5, "EDI", QBDI::REGISTER_READ_WRITE},
           {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 4, "ESI", QBDI::REGISTER_READ_WRITE},
        }
    },
#if defined(QBDI_ARCH_X86_64)
    {5, 6, true, QBDI::REGISTER_WRITE, {
#else
    {4, 6, true, QBDI::REGISTER_WRITE, {
#endif
           {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, sizeof(QBDI::rword), 0, 0, QBDI::GPR_NAMES[0], QBDI::REGISTER_READ},
           {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, sizeof(QBDI::rword), 0, 4, QBDI::GPR_NAMES[4], QBDI::REGISTER_READ},
           {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 1, sizeof(QBDI::rword), 0, -1, NULL, QBDI::REGISTER_UNUSED},
           {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, sizeof(QBDI::rword), 0, 5, QBDI::GPR_NAMES[5], QBDI::REGISTER_READ},
           {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 3, sizeof(QBDI::rword), 0, -1, NULL, QBDI::REGISTER_UNUSED},
           {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_NONE, 0, 0, 0, -1, NULL, QBDI::REGISTER_UNUSED},
        }
    },
};
#elif defined(QBDI_ARCH_ARM)
struct TestInst TestInsts[MNEM_COUNT] = {
    {4, 3, true, {
            {QBDI::OPERAND_GPR, 0, sizeof(QBDI::rword), 0, 3, "R3", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, MNEM_IMM_SHORT_VAL, sizeof(QBDI::rword), 0, -1, nullptr, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_PRED, 0, sizeof(QBDI::rword), 0, -1, nullptr, QBDI::REGISTER_UNUSED},
        }
    }
};
#endif


QBDI_NOSTACKPROTECTOR QBDI_NOINLINE QBDI::rword satanicFun(QBDI::rword arg0) {
    QBDI::rword volatile res = arg0 + 0x666;
#if defined(QBDI_ARCH_X86) || defined(QBDI_ARCH_X86_64)
    QBDI::rword p = 0x42;
    QBDI::rword v[2] = {0x67, 0x45};
 #ifndef QBDI_PLATFORM_WINDOWS
    asm("cmp $" MNEM_IMM_SHORT_STRVAL ", %%dh" ::: "dh");
  #if defined(QBDI_ARCH_X86_64)
    asm("cmp %%rbx, %%rax" ::: "rbx", "rax");
    asm("cmp $" MNEM_IMM_STRVAL ", %%eax" ::: "eax"); // explicit register
    asm("movq %0, %%rdi; movq %1, %%rsi; cmpsb %%es:(%%rdi), (%%rsi)"::"r"(&p), "r"(&p): "rdi", "rsi");
    asm("mov %0, %%rdi; mov $1, %%rsi; cmp 0x3(%%rsi,%%rdi,1), %%rax"::"r"(v): "rdi", "rsi", "rax");
  #else
    asm("cmp %%bx, %%ax" ::: "bx", "ax");
    asm("cmp $" MNEM_IMM_STRVAL ", %%eax" ::: "eax"); // explicit register
    asm("mov %0, %%edi; mov %1, %%esi; cmpsb %%es:(%%edi), (%%esi)"::"r"(&p), "r"(&p): "edi", "esi");
    asm("mov %0, %%edi; mov $1, %%esi; cmp 0x3(%%esi,%%edi,1), %%eax"::"r"(v): "edi", "esi", "eax");
  #endif
 #endif
#elif defined(QBDI_ARCH_ARM)
    asm("cmp r3, #" MNEM_IMM_SHORT_STRVAL);
#endif
    return res;
}


VMTest::VMTest() {
    // Constructing a new QBDI vm
    vm = std::make_unique<QBDI::VM>();
    REQUIRE(vm.get() != nullptr);

    bool instrumented = vm->addInstrumentedModuleFromAddr((QBDI::rword) &dummyFun0);
    REQUIRE(instrumented);

    // get GPR state
    state = vm->getGPRState();

    // Get a pointer to the GPR state of the vm
    // Setup initial GPR state, this fakestack will produce a ret NULL at the end of the execution
    bool ret = QBDI::allocateVirtualStack(state, STACK_SIZE, &fakestack);
    REQUIRE(ret == true);
}


VMTest::~VMTest() {
    QBDI::alignedFree(fakestack);
    vm.reset();
}


TEST_CASE_METHOD(VMTest, "VMTest-Call0") {
    QBDI::simulateCall(state, FAKE_RET_ADDR);

    vm->run((QBDI::rword) dummyFun0, (QBDI::rword) FAKE_RET_ADDR);
    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    REQUIRE(ret == (QBDI::rword) 42);

    SUCCEED();
}


TEST_CASE_METHOD(VMTest, "VMTest-Call1") {
    QBDI::simulateCall(state, FAKE_RET_ADDR, {42});

    vm->run((QBDI::rword) dummyFun1, (QBDI::rword) FAKE_RET_ADDR);
    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    REQUIRE(ret == (QBDI::rword) dummyFun1(42));

    SUCCEED();
}


TEST_CASE_METHOD(VMTest, "VMTest-Call4") {
    QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 5});

    vm->run((QBDI::rword) dummyFun4, (QBDI::rword) FAKE_RET_ADDR);
    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    REQUIRE(ret == (QBDI::rword) dummyFun4(1, 2, 3, 5));

    SUCCEED();
}


TEST_CASE_METHOD(VMTest, "VMTest-Call5") {
    QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 5, 8});

    vm->run((QBDI::rword) dummyFun5, (QBDI::rword) FAKE_RET_ADDR);
    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    REQUIRE(ret == (QBDI::rword) dummyFun5(1, 2, 3, 5, 8));

    SUCCEED();
}

TEST_CASE_METHOD(VMTest, "VMTest-Call8") {
    QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 5, 8, 13, 21, 34});

    vm->run((QBDI::rword) dummyFun8, (QBDI::rword) FAKE_RET_ADDR);
    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    REQUIRE(ret == (QBDI::rword) dummyFun8(1, 2, 3, 5, 8, 13, 21, 34));

    SUCCEED();
}


TEST_CASE_METHOD(VMTest, "VMTest-ExternalCall") {
    QBDI::simulateCall(state, FAKE_RET_ADDR, {42});

    vm->run((QBDI::rword) dummyFunCall, (QBDI::rword) FAKE_RET_ADDR);
    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    REQUIRE(ret == (QBDI::rword) dummyFun1(42));

    SUCCEED();
}


QBDI::VMAction countInstruction(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState, QBDI::FPRState *fprState, void *data) {
    *((uint32_t*)data) += 1;
    return QBDI::VMAction::CONTINUE;
}


QBDI::VMAction evilCbk(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState, QBDI::FPRState *fprState, void *data) {
    const QBDI::InstAnalysis* ana = vm->getInstAnalysis();
    CHECK(ana->mnemonic != nullptr);
    CHECK(ana->disassembly != nullptr);
    CHECK(ana->operands == nullptr);
    QBDI::rword* info = (QBDI::rword*) data;
    QBDI::rword cval = QBDI_GPR_GET(gprState, QBDI::REG_RETURN);
    // should never be reached (because we stop VM after value is incremented)
    if (info[1] != 0) {
        // if we failed this test, just return a fixed value
        QBDI_GPR_SET(gprState, QBDI::REG_RETURN, 0x21);
    }
    // return register is being set with our return value
    if (cval == (satanicFun(info[0]))) {
        info[1]++;
        return QBDI::VMAction::STOP;
    }
    return QBDI::VMAction::CONTINUE;
}


/* This test is used to ensure that addCodeAddrCB is not broken */
TEST_CASE_METHOD(VMTest, "VMTest-Breakpoint") {
    uint32_t counter = 0;
    QBDI::rword retval = 0;
    vm->addCodeAddrCB((QBDI::rword)dummyFun0, QBDI::InstPosition::PREINST, countInstruction, &counter);
    vm->call(&retval, (QBDI::rword) dummyFun0);
    REQUIRE(retval == (QBDI::rword) 42);
    REQUIRE(counter == 1u);

    SUCCEED();
}


TEST_CASE_METHOD(VMTest, "VMTest-InstCallback") {
    QBDI::rword info[2] = {42, 0};
    QBDI::simulateCall(state, FAKE_RET_ADDR, {info[0]});

    QBDI::rword rstart = (QBDI::rword) &satanicFun;
    QBDI::rword rend = (QBDI::rword) (((uint8_t*) &satanicFun) + 100);

    bool success = vm->removeInstrumentedModuleFromAddr((QBDI::rword) &dummyFun0);
    REQUIRE(success);
    vm->addInstrumentedRange(rstart, rend);

    uint32_t instrId = vm->addCodeRangeCB(rstart,
                                          rend,
                                          QBDI::InstPosition::POSTINST,
                                          evilCbk, &info);

    bool ran = vm->run((QBDI::rword) satanicFun, (QBDI::rword) FAKE_RET_ADDR);
    REQUIRE(ran);

    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    REQUIRE(ret == (QBDI::rword) satanicFun(info[0]));
    REQUIRE(info[1] == (QBDI::rword) 1);

    success = vm->deleteInstrumentation(instrId);
    REQUIRE(success);

    SUCCEED();
}

#define MNEM_CMP "CMP*"

QBDI::VMAction evilMnemCbk(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState, QBDI::FPRState *fprState, void *data) {
    QBDI::rword* info = (QBDI::rword*) data;
    if (info[0] >= MNEM_COUNT)
        return QBDI::VMAction::CONTINUE;

    // get instruction metadata
    const QBDI::InstAnalysis* ana = vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION | QBDI::ANALYSIS_OPERANDS);
    // validate mnemonic
    CHECKED_IF(QBDI::String::startsWith(MNEM_CMP, ana->mnemonic)) {
        info[0]++; // CMP count
        info[1]++;
        // validate address
        CHECKED_IF(ana->address >= (QBDI::rword) &satanicFun)
            CHECKED_IF(ana->address < (((QBDI::rword) &satanicFun) + 0x100))
                info[1]++;
        // validate inst size
        struct TestInst& currentInst = TestInsts[info[0] - 1];
#if defined(QBDI_ARCH_X86) || defined(QBDI_ARCH_X86_64)
        CHECKED_IF(ana->instSize == currentInst.instSize) {
#else
        {
#endif
            info[1]++;
        }
        // validate instruction type (kinda...)
        if (currentInst.isCompare) {
            // CHECKED_IF doesn't support && operator
            CHECKED_IF(!ana->isBranch)
                CHECKED_IF(!ana->isCall)
                    CHECKED_IF(!ana->isReturn)
                        CHECKED_IF(ana->isCompare)
                            info[1]++;
        }
        CHECKED_IF(ana->flagsAccess == currentInst.flagsAccess) {
            info[1]++;
        }
        // validate number of analyzed operands
        CHECKED_IF(ana->numOperands == currentInst.numOperands) {
            info[1]++;
        }
        // validate operands
        CHECKED_IF(ana->operands != nullptr) {
            info[1]++;
            for (uint8_t idx = 0; idx < std::min(ana->numOperands, currentInst.numOperands); idx++) {
                const QBDI::OperandAnalysis& cmpOp = currentInst.operands[idx];
                const QBDI::OperandAnalysis& op = ana->operands[idx];
                CHECKED_IF(op.type == cmpOp.type) {
                    info[1]++;
                }
                if (op.type == QBDI::OPERAND_IMM) {
                    CHECKED_IF(op.value == cmpOp.value) {
                        info[1]++;
                    }
                }
                if (op.regName == nullptr && cmpOp.regName == nullptr) {
                    info[1]++;
                } else {
                    CHECKED_IF(op.regName != nullptr)
                        CHECKED_IF(cmpOp.regName != nullptr)
                            CHECKED_IF(std::string(op.regName) == std::string(cmpOp.regName))
                                info[1]++;
                }
                CHECKED_IF(op.size == cmpOp.size) {
                    info[1]++;
                }
                CHECKED_IF(op.regCtxIdx == cmpOp.regCtxIdx) {
                    info[1]++;
                }
                CHECKED_IF(op.regOff == cmpOp.regOff) {
                    info[1]++;
                }
                CHECKED_IF(op.regAccess == cmpOp.regAccess) {
                    info[1]++;
                }
            }
        }
    }
    return QBDI::VMAction::CONTINUE;
}


TEST_CASE_METHOD(VMTest, "VMTest-MnemCallback") {
    QBDI::rword info[3] = {0, 0, 42};
    QBDI::rword retval = 0;
    const char* noop = MNEM_CMP;

    uint32_t instrId = vm->addMnemonicCB(noop,
                                         QBDI::InstPosition::PREINST,
                                         evilMnemCbk, &info);

    bool ran = vm->call(&retval, (QBDI::rword) satanicFun, {info[2]});
    REQUIRE(ran);

    CHECK(retval == (QBDI::rword) satanicFun(info[2]));
    // TODO: try to find a way to support windows
#ifdef QBDI_PLATFORM_WINDOWS
    CHECK(info[1] == (QBDI::rword) 0);
#else
    CHECK(info[0] == MNEM_COUNT);
    CHECK(info[1] == (QBDI::rword) MNEM_VALIDATION);
#endif

    bool success = vm->deleteInstrumentation(instrId);
    REQUIRE(success);

    SUCCEED();
}


QBDI::VMAction checkTransfer(QBDI::VMInstanceRef vm, const QBDI::VMState *state, QBDI::GPRState *gprState, QBDI::FPRState *fprState, void *data) {
    int* s = (int*) data;
#if defined(QBDI_PLATFORM_LINUX) || defined(QBDI_PLATFORM_ANDROID) || defined(QBDI_PLATFORM_OSX)
    QBDI::rword allocAPI = (QBDI::rword) &posix_memalign;
    QBDI::rword freeAPI = (QBDI::rword) &free;
#elif defined(QBDI_PLATFORM_WINDOWS)
    QBDI::rword allocAPI = (QBDI::rword) &_aligned_malloc;
    QBDI::rword freeAPI = (QBDI::rword) &_aligned_free;
#endif
    if (state->event == QBDI::VMEvent::EXEC_TRANSFER_CALL) {
        if (*s == 0 && state->sequenceStart == allocAPI) {
            *s = 1;
        }
        else if (*s == 2 && state->sequenceStart == freeAPI) {
            *s = 3;
        }
        else {
            INFO("Calling unknown address 0x" << std::hex << state->sequenceStart);
        }
    }
    else if (state->event == QBDI::VMEvent::EXEC_TRANSFER_RETURN) {
        if (*s == 1 && state->sequenceStart == allocAPI) {
            *s = 2;
        }
        else if (*s == 3 && state->sequenceStart == freeAPI) {
            *s = 4;
        }
        else {
            INFO("Returning from unknown address 0x" << std::hex << state->sequenceStart);
        }
    }
    return QBDI::VMAction::CONTINUE;
}

TEST_CASE_METHOD(VMTest, "VMTest-VMEvent_ExecTransfer") {
    int s = 0;
    QBDI::simulateCall(state, FAKE_RET_ADDR, {42});
    bool instrumented = vm->addInstrumentedModuleFromAddr((QBDI::rword)&dummyFunCall);
    REQUIRE(instrumented);
    uint32_t id = vm->addVMEventCB(QBDI::VMEvent::EXEC_TRANSFER_CALL, checkTransfer, (void*) &s);
    REQUIRE(id != QBDI::INVALID_EVENTID);
    id = vm->addVMEventCB(QBDI::VMEvent::EXEC_TRANSFER_RETURN, checkTransfer, (void*) &s);
    REQUIRE(id != QBDI::INVALID_EVENTID);
    bool ran = vm->run((QBDI::rword) dummyFunCall, (QBDI::rword) FAKE_RET_ADDR);
    REQUIRE(ran);
    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    REQUIRE(ret == (QBDI::rword) 42);
    REQUIRE(4 == s);
    vm->deleteAllInstrumentations();
}

TEST_CASE_METHOD(VMTest, "VMTest-CacheInvalidation") {
    uint32_t count1 = 0;
    uint32_t count2 = 0;

    bool instrumented = vm->addInstrumentedModuleFromAddr((QBDI::rword)&dummyFunCall);
    REQUIRE(instrumented);
    uint32_t instr1 = vm->addCodeCB(QBDI::InstPosition::POSTINST, countInstruction, &count1);

    count1 = 0;
    count2 = 0;
    QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 4});
    bool ran = vm->run((QBDI::rword) dummyFun4, (QBDI::rword) FAKE_RET_ADDR);
    REQUIRE(ran);
    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    REQUIRE(ret == (QBDI::rword) 10);
    REQUIRE((uint32_t) 0 != count1);
    REQUIRE((uint32_t) 0 == count2);

    uint32_t instr2 = vm->addCodeRangeCB((QBDI::rword)&dummyFun5, ((QBDI::rword)&dummyFun5) + 64,
                                         QBDI::InstPosition::POSTINST, countInstruction, &count2);

    count1 = 0;
    count2 = 0;
    QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 4, 5});
    ran = vm->run((QBDI::rword) dummyFun5, (QBDI::rword) FAKE_RET_ADDR);
    REQUIRE(ran);
    ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    REQUIRE(ret == (QBDI::rword) 15);
    REQUIRE((uint32_t) 0 != count1);
    REQUIRE((uint32_t) 0 != count2);

    vm->deleteInstrumentation(instr1);

    count1 = 0;
    count2 = 0;
    QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 4});
    ran = vm->run((QBDI::rword) dummyFun4, (QBDI::rword) FAKE_RET_ADDR);
    REQUIRE(ran);
    ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    REQUIRE(ret == (QBDI::rword) 10);
    REQUIRE((uint32_t) 0 == count1);
    REQUIRE((uint32_t) 0 == count2);

    count1 = 0;
    count2 = 0;
    QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 4, 5});
    ran = vm->run((QBDI::rword) dummyFun5, (QBDI::rword) FAKE_RET_ADDR);
    REQUIRE(ran);
    ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    REQUIRE(ret == (QBDI::rword) 15);
    REQUIRE((uint32_t) 0 == count1);
    REQUIRE((uint32_t) 0 != count2);

    instr1 = vm->addCodeCB(QBDI::InstPosition::POSTINST, countInstruction, &count1);

    count1 = 0;
    count2 = 0;
    QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 4, 5});
    ran = vm->run((QBDI::rword) dummyFun5, (QBDI::rword) FAKE_RET_ADDR);
    REQUIRE(ran);
    ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    REQUIRE(ret == (QBDI::rword) 15);
    REQUIRE((uint32_t) 0 != count1);
    REQUIRE((uint32_t) 0 != count2);

    vm->deleteInstrumentation(instr2);

    count1 = 0;
    count2 = 0;
    QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 4});
    ran = vm->run((QBDI::rword) dummyFun4, (QBDI::rword) FAKE_RET_ADDR);
    REQUIRE(ran);
    ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    REQUIRE(ret == (QBDI::rword) 10);
    REQUIRE((uint32_t) 0 != count1);
    REQUIRE((uint32_t) 0 == count2);

    count1 = 0;
    count2 = 0;
    QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 4, 5});
    ran = vm->run((QBDI::rword) dummyFun5, (QBDI::rword) FAKE_RET_ADDR);
    REQUIRE(ran);
    ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    REQUIRE(ret == (QBDI::rword) 15);
    REQUIRE((uint32_t) 0 != count1);
    REQUIRE((uint32_t) 0 == count2);
}

struct FunkyInfo {
    uint32_t instID;
    uint32_t count;
};

QBDI::VMAction funkyCountInstruction(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState, QBDI::FPRState *fprState, void *data) {
    FunkyInfo *info = (FunkyInfo*) data;

    const QBDI::InstAnalysis* instAnalysis1 = vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
    vm->deleteInstrumentation(info->instID);
    const QBDI::InstAnalysis* instAnalysis2 = vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
    info->instID = vm->addCodeRangeCB(QBDI_GPR_GET(gprState, QBDI::REG_PC), QBDI_GPR_GET(gprState, QBDI::REG_PC) + 10,
                                    QBDI::InstPosition::POSTINST, funkyCountInstruction, data);
    const QBDI::InstAnalysis* instAnalysis3 = vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
    // instAnalysis1, instAnalysis2 and instAnalysis3 should be the same pointer because the cache
    // flush initiated by deleteInstrumentation and addCodeRangeCB is delayed.
    if(instAnalysis1 == instAnalysis2 && instAnalysis2 == instAnalysis3) {
        info->count += 1;
    }

    // instAnalysis3 should not have disassembly information, but instAnalysis4 and instAnalysis5
    // should.
    CHECK(instAnalysis3->disassembly == nullptr);
    CHECK(instAnalysis3->operands == nullptr);
    const QBDI::InstAnalysis* instAnalysis4 = vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION | QBDI::ANALYSIS_DISASSEMBLY);
    CHECK(instAnalysis4->disassembly != nullptr);
    CHECK(instAnalysis4->operands == nullptr);
    const QBDI::InstAnalysis* instAnalysis5 = vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
    CHECK(instAnalysis5->disassembly != nullptr);
    CHECK(instAnalysis5->operands == nullptr);

    return QBDI::VMAction::BREAK_TO_VM;
}

TEST_CASE_METHOD(VMTest, "VMTest-DelayedCacheFlush") {
    uint32_t count = 0;
    FunkyInfo info = FunkyInfo {0, 0};

    bool instrumented = vm->addInstrumentedModuleFromAddr((QBDI::rword)&dummyFunCall);
    REQUIRE(instrumented);
    vm->addCodeCB(QBDI::InstPosition::POSTINST, countInstruction, &count);
    info.instID = vm->addCodeRangeCB((QBDI::rword) dummyFun4, ((QBDI::rword) dummyFun4) + 10,
                                    QBDI::InstPosition::POSTINST, funkyCountInstruction, &info);

    QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 4});
    bool ran = vm->run((QBDI::rword) dummyFun4, (QBDI::rword) FAKE_RET_ADDR);
    REQUIRE(ran);
    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    REQUIRE(ret == (QBDI::rword) 10);
    REQUIRE(count == info.count);
}

// Test copy/move constructor/assignment operator

struct MoveCallbackStruct {
    QBDI::VMInstanceRef expectedRef;
    bool allowedNewBlock;

    bool reachEventCB;
    bool reachInstCB;
    bool reachCB2 = false;
};

static QBDI::VMAction allowedNewBlock(QBDI::VMInstanceRef vm, const QBDI::VMState* state, QBDI::GPRState*, QBDI::FPRState*, void *data_) {
    MoveCallbackStruct* data = static_cast<MoveCallbackStruct*>(data_);
    CHECK(data->expectedRef == vm);
    CHECK(( data->allowedNewBlock or ( (state->event & QBDI::BASIC_BLOCK_NEW) == 0 ) ));

    data->reachEventCB = true;
    return QBDI::VMAction::CONTINUE;
}

static QBDI::VMAction verifyVMRef(QBDI::VMInstanceRef vm, QBDI::GPRState*, QBDI::FPRState*, void *data_) {
    MoveCallbackStruct* data = static_cast<MoveCallbackStruct*>(data_);
    CHECK(data->expectedRef == vm);

    data->reachInstCB = true;
    return QBDI::VMAction::CONTINUE;
}

static QBDI::VMAction verifyCB2(QBDI::VMInstanceRef vm, QBDI::GPRState*, QBDI::FPRState*, void *data_) {
    MoveCallbackStruct* data = static_cast<MoveCallbackStruct*>(data_);
    CHECK(data->expectedRef == vm);

    data->reachCB2 = true;
    return QBDI::VMAction::CONTINUE;
}

TEST_CASE("VMTest-MoveConstructor") {

    VMTest vm1;
    QBDI::VM* vm = vm1.vm.get();

    MoveCallbackStruct data {vm, true, false, false};

    bool instrumented = vm->addInstrumentedModuleFromAddr((QBDI::rword)&dummyFunCall);
    REQUIRE(instrumented);

    vm->addCodeCB(QBDI::InstPosition::POSTINST, verifyVMRef, &data);
    vm->addVMEventCB(QBDI::SEQUENCE_ENTRY | QBDI::SEQUENCE_EXIT | QBDI::BASIC_BLOCK_NEW, allowedNewBlock, &data);

    QBDI::rword retvalue;

    vm->call(&retvalue, (QBDI::rword) dummyFun1, {350});
    REQUIRE(retvalue == 350);
    REQUIRE(data.reachEventCB);
    REQUIRE(data.reachInstCB);

    data.reachEventCB = false;
    data.reachInstCB = false;
    data.allowedNewBlock = false;

    vm = vm1.vm.release();
    REQUIRE(vm1.vm.get() == nullptr);
    REQUIRE(vm == data.expectedRef);

    // move vm
    QBDI::VM movedVM(std::move(*vm));
    vm = nullptr;

    REQUIRE(data.expectedRef != &movedVM);
    data.expectedRef = &movedVM;

    movedVM.call(&retvalue, (QBDI::rword) dummyFun1, {780});
    REQUIRE(retvalue == 780);
    REQUIRE(data.reachEventCB);
    REQUIRE(data.reachInstCB);
}

TEST_CASE("VMTest-CopyConstructor") {

    VMTest vm1;
    QBDI::VM* vm = vm1.vm.get();

    MoveCallbackStruct data {vm, true, false, false};

    bool instrumented = vm->addInstrumentedModuleFromAddr((QBDI::rword)&dummyFunCall);
    REQUIRE(instrumented);

    vm->addCodeCB(QBDI::InstPosition::POSTINST, verifyVMRef, &data);
    vm->addVMEventCB(QBDI::SEQUENCE_ENTRY | QBDI::SEQUENCE_EXIT | QBDI::BASIC_BLOCK_NEW, allowedNewBlock, &data);

    QBDI::rword retvalue;

    vm->call(&retvalue, (QBDI::rword) dummyFun1, {350});
    REQUIRE(retvalue == 350);
    REQUIRE(data.reachEventCB);
    REQUIRE(data.reachInstCB);

    data.reachEventCB = false;
    data.reachInstCB = false;
    data.allowedNewBlock = false;

    // copy vm
    QBDI::VM movedVM(*vm);

    REQUIRE(data.expectedRef != &movedVM);

    vm->call(&retvalue, (QBDI::rword) dummyFun1, {620});
    REQUIRE(retvalue == 620);
    REQUIRE(data.reachEventCB);
    REQUIRE(data.reachInstCB);

    data.reachEventCB = false;
    data.reachInstCB = false;
    data.allowedNewBlock = true;
    data.expectedRef = &movedVM;

    movedVM.call(&retvalue, (QBDI::rword) dummyFun1, {780});
    REQUIRE(retvalue == 780);
    REQUIRE(data.reachEventCB);
    REQUIRE(data.reachInstCB);
}

TEST_CASE("VMTest-MoveAssignmentOperator") {

    VMTest vm1_;
    VMTest vm2_;
    QBDI::VM* vm1 = vm1_.vm.get();
    QBDI::VM* vm2 = vm2_.vm.get();
    REQUIRE( vm1 != vm2 );

    MoveCallbackStruct data1 {vm1, true, false, false};
    MoveCallbackStruct data2 {vm2, true, false, false};

    bool instrumented = vm1->addInstrumentedModuleFromAddr((QBDI::rword)&dummyFunCall);
    REQUIRE(instrumented);
    instrumented = vm2->addInstrumentedModuleFromAddr((QBDI::rword)&dummyFunCall);
    REQUIRE(instrumented);

    vm1->addCodeCB(QBDI::InstPosition::POSTINST, verifyVMRef, &data1);
    vm1->addVMEventCB(QBDI::SEQUENCE_ENTRY | QBDI::SEQUENCE_EXIT | QBDI::BASIC_BLOCK_NEW, allowedNewBlock, &data1);

    vm2->addCodeCB(QBDI::InstPosition::POSTINST, verifyVMRef, &data2);
    vm2->addVMEventCB(QBDI::SEQUENCE_ENTRY | QBDI::SEQUENCE_EXIT | QBDI::BASIC_BLOCK_NEW, allowedNewBlock, &data2);

    QBDI::rword retvalue;

    vm1->call(&retvalue, (QBDI::rword) dummyFun1, {350});
    REQUIRE(retvalue == 350);
    REQUIRE(data1.reachEventCB);
    REQUIRE(data1.reachInstCB);

    data1.reachEventCB = false;
    data1.reachInstCB = false;
    data1.allowedNewBlock = false;

    vm2->call(&retvalue, (QBDI::rword) dummyFun1, {670});
    REQUIRE(retvalue == 670);
    REQUIRE(data2.reachEventCB);
    REQUIRE(data2.reachInstCB);

    data2.reachEventCB = false;
    data2.reachInstCB = false;
    data2.allowedNewBlock = false;

    data1.expectedRef = vm2;
    data2.expectedRef = nullptr;

    vm1 = vm1_.vm.release();
    REQUIRE(vm1_.vm.get() == nullptr);

    // move vm
    *vm2 = std::move(*vm1);
    vm1 = nullptr;

    vm2->call(&retvalue, (QBDI::rword) dummyFun1, {780});
    REQUIRE(retvalue == 780);
    REQUIRE(data1.reachEventCB);
    REQUIRE(data1.reachInstCB);
    REQUIRE(( not data2.reachEventCB));
    REQUIRE(( not data2.reachInstCB));
}

TEST_CASE("VMTest-CopyAssignmentOperator") {

    VMTest vm1_;
    VMTest vm2_;
    QBDI::VM* vm1 = vm1_.vm.get();
    QBDI::VM* vm2 = vm2_.vm.get();
    REQUIRE( vm1 != vm2 );

    MoveCallbackStruct data1 {vm1, true, false, false};
    MoveCallbackStruct data2 {vm2, true, false, false};

    bool instrumented = vm1->addInstrumentedModuleFromAddr((QBDI::rword)&dummyFunCall);
    REQUIRE(instrumented);
    instrumented = vm2->addInstrumentedModuleFromAddr((QBDI::rword)&dummyFunCall);
    REQUIRE(instrumented);

    vm1->addCodeCB(QBDI::InstPosition::POSTINST, verifyVMRef, &data1);
    vm1->addCodeCB(QBDI::InstPosition::POSTINST, verifyCB2, &data1);
    vm1->addVMEventCB(QBDI::SEQUENCE_ENTRY | QBDI::SEQUENCE_EXIT | QBDI::BASIC_BLOCK_NEW, allowedNewBlock, &data1);

    vm2->addCodeCB(QBDI::InstPosition::POSTINST, verifyVMRef, &data2);
    vm2->addVMEventCB(QBDI::SEQUENCE_ENTRY | QBDI::SEQUENCE_EXIT | QBDI::BASIC_BLOCK_NEW, allowedNewBlock, &data2);

    QBDI::rword retvalue;

    vm1->call(&retvalue, (QBDI::rword) dummyFun1, {350});
    REQUIRE(retvalue == 350);
    REQUIRE(data1.reachEventCB);
    REQUIRE(data1.reachInstCB);
    REQUIRE(data1.reachCB2);

    data1.reachEventCB = false;
    data1.reachInstCB = false;
    data1.allowedNewBlock = false;
    data1.reachCB2 = false;

    vm2->call(&retvalue, (QBDI::rword) dummyFun1, {670});
    REQUIRE(retvalue == 670);
    REQUIRE(data2.reachEventCB);
    REQUIRE(data2.reachInstCB);
    REQUIRE(( not data2.reachCB2));

    data2.reachEventCB = false;
    data2.reachInstCB = false;
    data2.allowedNewBlock = false;
    data2.expectedRef = nullptr;

    // copy vm
    *vm2 = *vm1;

    vm1->call(&retvalue, (QBDI::rword) dummyFun1, {780});
    REQUIRE(retvalue == 780);
    REQUIRE(data1.reachEventCB);
    REQUIRE(data1.reachInstCB);
    REQUIRE(data1.reachCB2);
    REQUIRE(( not data2.reachEventCB));
    REQUIRE(( not data2.reachInstCB));
    REQUIRE(( not data2.reachCB2));

    data1.reachEventCB = false;
    data1.reachInstCB = false;
    data1.allowedNewBlock = true;
    data1.expectedRef = vm2;
    data1.reachCB2 = false;

    vm2->call(&retvalue, (QBDI::rword) dummyFun1, {567});
    REQUIRE(retvalue == 567);
    REQUIRE(data1.reachEventCB);
    REQUIRE(data1.reachInstCB);
    REQUIRE(data1.reachCB2);
    REQUIRE(( not data2.reachEventCB));
    REQUIRE(( not data2.reachInstCB));
    REQUIRE(( not data2.reachCB2));
}
