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
#include <gtest/gtest.h>
#include "VMTest.h"

#include "inttypes.h"

#include "Utility/String.h"
#include "Platform.h"
#include "Memory.hpp"

#ifndef QBDI_OS_WIN
// Can be used to log failure on a test (usefull in subroutines)
#define TEST_GUARD(T) ({    \
    bool res = false;       \
    EXPECT_TRUE(res = (T)); \
    res; })
#else
#define TEST_GUARD
#endif


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
#define MNEM_VALIDATION 159u
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
    QBDI::OperandAnalysis operands[6];
};

#if defined(QBDI_ARCH_X86) || defined(QBDI_ARCH_X86_64)
struct TestInst TestInsts[MNEM_COUNT] = {
    {3, 3, true, {
           {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 1, 8, 3, "DH", QBDI::REGISTER_READ},
           {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_NONE, MNEM_IMM_SHORT_VAL, 1, 0, 0, NULL, QBDI::REGISTER_UNUSED},
           {QBDI::OPERAND_FLAG, QBDI::OPERANDFLAG_NONE, 0, 32, 0, QBDI::REG_FLAG, "EFLAGS", QBDI::REGISTER_WRITE},
        }
    },
#if defined(QBDI_ARCH_X86_64)
    {3, 3, true, {
           {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 0, "RAX", QBDI::REGISTER_READ},
           {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 1, "RBX", QBDI::REGISTER_READ},
           {QBDI::OPERAND_FLAG, QBDI::OPERANDFLAG_NONE, 0, 32, 0, QBDI::REG_FLAG, "EFLAGS", QBDI::REGISTER_WRITE},
        }
    },
#else
    {3, 3, true, {
           {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 2, 0, 0, "AX", QBDI::REGISTER_READ},
           {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 2, 0, 1, "BX", QBDI::REGISTER_READ},
           {QBDI::OPERAND_FLAG, QBDI::OPERANDFLAG_NONE, 0, 32, 0, QBDI::REG_FLAG, "EFLAGS", QBDI::REGISTER_WRITE},
        }
    },
#endif
    {5, 3, true, {
           {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_NONE, MNEM_IMM_VAL, 4, 0, 0, NULL, QBDI::REGISTER_UNUSED},
           {QBDI::OPERAND_FLAG, QBDI::OPERANDFLAG_NONE, 0, 32, 0, QBDI::REG_FLAG, "EFLAGS", QBDI::REGISTER_WRITE},
           {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 0, "EAX", QBDI::REGISTER_READ},
        }
    },
    {1, 6, false, {
           {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, sizeof(QBDI::rword), 0, 5, QBDI::GPR_NAMES[5], QBDI::REGISTER_READ},
           {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, sizeof(QBDI::rword), 0, 4, QBDI::GPR_NAMES[4], QBDI::REGISTER_READ},
           {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 5, "EDI", QBDI::REGISTER_READ_WRITE},
           {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 4, "ESI", QBDI::REGISTER_READ_WRITE},
           {QBDI::OPERAND_FLAG, QBDI::OPERANDFLAG_NONE, 0, 32, 0, QBDI::REG_FLAG, "EFLAGS", QBDI::REGISTER_WRITE},
           {QBDI::OPERAND_FLAG, QBDI::OPERANDFLAG_NONE, 0, 1, 14, QBDI::REG_FLAG, "DF", QBDI::REGISTER_READ},
        }
    },
#if defined(QBDI_ARCH_X86_64)
    {5, 6, true, {
#else
    {4, 6, true, {
#endif
           {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, sizeof(QBDI::rword), 0, 0, QBDI::GPR_NAMES[0], QBDI::REGISTER_READ},
           {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, sizeof(QBDI::rword), 0, 4, QBDI::GPR_NAMES[4], QBDI::REGISTER_READ},
           {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 1, sizeof(QBDI::rword), 0, 0, NULL, QBDI::REGISTER_UNUSED},
           {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, sizeof(QBDI::rword), 0, 5, QBDI::GPR_NAMES[5], QBDI::REGISTER_READ},
           {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 3, sizeof(QBDI::rword), 0, 0, NULL, QBDI::REGISTER_UNUSED},
           {QBDI::OPERAND_FLAG, QBDI::OPERANDFLAG_NONE, 0, 32, 0, QBDI::REG_FLAG, "EFLAGS", QBDI::REGISTER_WRITE},
        }
    },
};
#elif defined(QBDI_ARCH_ARM)
struct TestInst TestInsts[MNEM_COUNT] = {
    {4, 3, true, {
            {QBDI::OPERAND_GPR, 0, sizeof(QBDI::rword), 0, 3, "R3", QBDI::REGISTER_READ},
            {QBDI::OPERAND_IMM, MNEM_IMM_SHORT_VAL, sizeof(QBDI::rword), 0, 0, NULL, QBDI::REGISTER_UNUSED},
            {QBDI::OPERAND_PRED, 0, sizeof(QBDI::rword), 0, 0, NULL, QBDI::REGISTER_UNUSED},
        }
    }
};
#endif


QBDI_NOSTACKPROTECTOR QBDI_NOINLINE QBDI::rword satanicFun(QBDI::rword arg0) {
    QBDI::rword volatile res = arg0 + 0x666;
#if defined(QBDI_ARCH_X86) || defined(QBDI_ARCH_X86_64)
    QBDI::rword p = 0x42;
    QBDI::rword v[2] = {0x67, 0x45};
 #ifndef QBDI_OS_WIN
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


void VMTest::SetUp() {
    // Constructing a new QBDI vm
    vm = new QBDI::VM();
    ASSERT_NE(vm, nullptr);

    bool instrumented = vm->addInstrumentedModuleFromAddr((QBDI::rword) &dummyFun0);
    ASSERT_TRUE(instrumented);

    // get GPR state
    state = vm->getGPRState();

    // Get a pointer to the GPR state of the vm
    // Setup initial GPR state, this fakestack will produce a ret NULL at the end of the execution
    bool ret = QBDI::allocateVirtualStack(state, STACK_SIZE, &fakestack);
    ASSERT_EQ(ret, true);
}


void VMTest::TearDown() {
    QBDI::alignedFree(fakestack);
    delete vm;
}


TEST_F(VMTest, Call0) {
    QBDI::simulateCall(state, FAKE_RET_ADDR);

    vm->run((QBDI::rword) dummyFun0, (QBDI::rword) FAKE_RET_ADDR);
    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ(ret, (QBDI::rword) 42);

    SUCCEED();
}


TEST_F(VMTest, Call1) {
    QBDI::simulateCall(state, FAKE_RET_ADDR, {42});

    vm->run((QBDI::rword) dummyFun1, (QBDI::rword) FAKE_RET_ADDR);
    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ(ret, (QBDI::rword) dummyFun1(42));

    SUCCEED();
}


TEST_F(VMTest, Call4) {
    QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 5});

    vm->run((QBDI::rword) dummyFun4, (QBDI::rword) FAKE_RET_ADDR);
    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ(ret, (QBDI::rword) dummyFun4(1, 2, 3, 5));

    SUCCEED();
}


TEST_F(VMTest, Call5) {
    QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 5, 8});

    vm->run((QBDI::rword) dummyFun5, (QBDI::rword) FAKE_RET_ADDR);
    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ(ret, (QBDI::rword) dummyFun5(1, 2, 3, 5, 8));

    SUCCEED();
}

TEST_F(VMTest, Call8) {
    QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 5, 8, 13, 21, 34});

    vm->run((QBDI::rword) dummyFun8, (QBDI::rword) FAKE_RET_ADDR);
    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ(ret, (QBDI::rword) dummyFun8(1, 2, 3, 5, 8, 13, 21, 34));

    SUCCEED();
}


TEST_F(VMTest, ExternalCall) {
    QBDI::simulateCall(state, FAKE_RET_ADDR, {42});

    vm->run((QBDI::rword) dummyFunCall, (QBDI::rword) FAKE_RET_ADDR);
    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ(ret, (QBDI::rword) dummyFun1(42));

    SUCCEED();
}


QBDI::VMAction countInstruction(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState, QBDI::FPRState *fprState, void *data) {
    *((uint32_t*)data) += 1;
    return QBDI::VMAction::CONTINUE;
}


QBDI::VMAction evilCbk(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState, QBDI::FPRState *fprState, void *data) {
    const QBDI::InstAnalysis* ana = vm->getInstAnalysis();
    EXPECT_NE(ana->mnemonic, nullptr);
    EXPECT_NE(ana->disassembly, nullptr);
    EXPECT_EQ(ana->operands, nullptr);
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
TEST_F(VMTest, Breakpoint) {
    uint32_t counter = 0;
    QBDI::rword retval = 0;
    vm->addCodeAddrCB((QBDI::rword)dummyFun0, QBDI::InstPosition::PREINST, countInstruction, &counter);
    vm->call(&retval, (QBDI::rword) dummyFun0);
    ASSERT_EQ(retval, (QBDI::rword) 42);
    ASSERT_EQ(counter, 1u);

    SUCCEED();
}


TEST_F(VMTest, InstCallback) {
    QBDI::rword info[2] = {42, 0};
    QBDI::simulateCall(state, FAKE_RET_ADDR, {info[0]});

    QBDI::rword rstart = (QBDI::rword) &satanicFun;
    QBDI::rword rend = (QBDI::rword) (((uint8_t*) &satanicFun) + 100);

    bool success = vm->removeInstrumentedModuleFromAddr((QBDI::rword) &dummyFun0);
    ASSERT_TRUE(success);
    vm->addInstrumentedRange(rstart, rend);

    uint32_t instrId = vm->addCodeRangeCB(rstart,
                                          rend,
                                          QBDI::InstPosition::POSTINST,
                                          evilCbk, &info);

    bool ran = vm->run((QBDI::rword) satanicFun, (QBDI::rword) FAKE_RET_ADDR);
    ASSERT_TRUE(ran);

    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ(ret, (QBDI::rword) satanicFun(info[0]));
    ASSERT_EQ(info[1], (QBDI::rword) 1);

    success = vm->deleteInstrumentation(instrId);
    ASSERT_TRUE(success);

    SUCCEED();
}

#define MNEM_CMP "CMP*"

QBDI::VMAction evilMnemCbk(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState, QBDI::FPRState *fprState, void *data) {
    QBDI::rword* info = (QBDI::rword*) data;
    // get instruction metadata
    const QBDI::InstAnalysis* ana = vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION | QBDI::ANALYSIS_OPERANDS);
    // validate mnemonic
    if (TEST_GUARD(QBDI::String::startsWith(MNEM_CMP, ana->mnemonic)) && info[0] < MNEM_COUNT) {
        info[0]++; // CMP count
        info[1]++;
        // validate address
        if (TEST_GUARD(ana->address >= (QBDI::rword) &satanicFun &&
            ana->address < (((QBDI::rword) &satanicFun) + 0x100))) {
            info[1]++;
        }
        // validate inst size
        struct TestInst& currentInst = TestInsts[info[0] - 1];
#if defined(QBDI_ARCH_X86) || defined(QBDI_ARCH_X86_64)
        if (TEST_GUARD(ana->instSize == currentInst.instSize)) {
#else
        {
#endif
            info[1]++;
        }
        // validate instruction type (kinda...)
        if (currentInst.isCompare) {
            if (TEST_GUARD(!ana->isBranch &&
                !ana->isCall &&
                !ana->isReturn &&
                ana->isCompare)) {
                info[1]++;
            }
        }
        // validate number of analyzed operands
        if (TEST_GUARD(ana->numOperands == currentInst.numOperands)) {
            info[1]++;
        }
        // validate operands
        if (TEST_GUARD(ana->operands != nullptr)) {
            info[1]++;
            for (uint8_t idx = 0; idx < std::min(ana->numOperands, currentInst.numOperands); idx++) {
                const QBDI::OperandAnalysis& cmpOp = currentInst.operands[idx];
                const QBDI::OperandAnalysis& op = ana->operands[idx];
                if (TEST_GUARD(op.type == cmpOp.type)) {
                    info[1]++;
                }
                if (op.type == QBDI::OPERAND_IMM) {
                    if (TEST_GUARD(op.value == cmpOp.value)) {
                        info[1]++;
                    }
                }
                if (op.regName == nullptr && cmpOp.regName == nullptr) {
                    info[1]++;
                } else if (TEST_GUARD(op.regName != nullptr && cmpOp.regName != nullptr &&
                    std::string(op.regName) == std::string(cmpOp.regName))) {
                    info[1]++;
                }
                if (TEST_GUARD(op.size == cmpOp.size)) {
                    info[1]++;
                }
                if (TEST_GUARD(op.regCtxIdx == cmpOp.regCtxIdx)) {
                    info[1]++;
                }
                if (TEST_GUARD(op.regOff == cmpOp.regOff)) {
                    info[1]++;
                }
                if (TEST_GUARD(op.regAccess == cmpOp.regAccess)) {
                    info[1]++;
                }
            }
        }
    }
    return QBDI::VMAction::CONTINUE;
}


TEST_F(VMTest, MnemCallback) {
    QBDI::rword info[3] = {0, 0, 42};
    QBDI::rword retval = 0;
    const char* noop = MNEM_CMP;

    uint32_t instrId = vm->addMnemonicCB(noop,
                                         QBDI::InstPosition::PREINST,
                                         evilMnemCbk, &info);

    bool ran = vm->call(&retval, (QBDI::rword) satanicFun, {info[2]});
    ASSERT_TRUE(ran);

    EXPECT_EQ(retval, (QBDI::rword) satanicFun(info[2]));
    // TODO: try to find a way to support windows
#ifdef QBDI_OS_WIN
    EXPECT_EQ(info[1], (QBDI::rword) 0);
#else
    EXPECT_EQ(info[0], MNEM_COUNT);
    EXPECT_EQ(info[1], (QBDI::rword) MNEM_VALIDATION);
#endif

    bool success = vm->deleteInstrumentation(instrId);
    ASSERT_TRUE(success);

    SUCCEED();
}


QBDI::VMAction checkTransfer(QBDI::VMInstanceRef vm, const QBDI::VMState *state, QBDI::GPRState *gprState, QBDI::FPRState *fprState, void *data) {
    int* s = (int*) data;
#if defined(QBDI_OS_LINUX) || defined(QBDI_OS_ANDROID) || defined(QBDI_OS_DARWIN)
    QBDI::rword allocAPI = (QBDI::rword) &posix_memalign;
    QBDI::rword freeAPI = (QBDI::rword) &free;
#elif defined(QBDI_OS_WIN)
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
            printf("Calling unknown address 0x%" PRIRWORD "\n", state->sequenceStart);
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
            printf("Returning from unknown address 0x%" PRIRWORD "\n", state->sequenceStart);
        }
    }
    return QBDI::VMAction::CONTINUE;
}

TEST_F(VMTest, VMEvent_ExecTransfer) {
    int s = 0;
    QBDI::simulateCall(state, FAKE_RET_ADDR, {42});
    bool instrumented = vm->addInstrumentedModuleFromAddr((QBDI::rword)&dummyFunCall);
    ASSERT_TRUE(instrumented);
    uint32_t id = vm->addVMEventCB(QBDI::VMEvent::EXEC_TRANSFER_CALL, checkTransfer, (void*) &s);
    ASSERT_NE(id, QBDI::INVALID_EVENTID);
    id = vm->addVMEventCB(QBDI::VMEvent::EXEC_TRANSFER_RETURN, checkTransfer, (void*) &s);
    ASSERT_NE(id, QBDI::INVALID_EVENTID);
    bool ran = vm->run((QBDI::rword) dummyFunCall, (QBDI::rword) FAKE_RET_ADDR);
    ASSERT_TRUE(ran);
    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ(ret, (QBDI::rword) 42);
    ASSERT_EQ(4, s);
    vm->deleteAllInstrumentations();
}

TEST_F(VMTest, CacheInvalidation) {
    uint32_t count1 = 0;
    uint32_t count2 = 0;

    bool instrumented = vm->addInstrumentedModuleFromAddr((QBDI::rword)&dummyFunCall);
    ASSERT_TRUE(instrumented);
    uint32_t instr1 = vm->addCodeCB(QBDI::InstPosition::POSTINST, countInstruction, &count1);

    count1 = 0;
    count2 = 0;
    QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 4});
    bool ran = vm->run((QBDI::rword) dummyFun4, (QBDI::rword) FAKE_RET_ADDR);
    ASSERT_TRUE(ran);
    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ(ret, (QBDI::rword) 10);
    ASSERT_NE((uint32_t) 0, count1);
    ASSERT_EQ((uint32_t) 0, count2);

    uint32_t instr2 = vm->addCodeRangeCB((QBDI::rword)&dummyFun5, ((QBDI::rword)&dummyFun5) + 64,
                                         QBDI::InstPosition::POSTINST, countInstruction, &count2);

    count1 = 0;
    count2 = 0;
    QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 4, 5});
    ran = vm->run((QBDI::rword) dummyFun5, (QBDI::rword) FAKE_RET_ADDR);
    ASSERT_TRUE(ran);
    ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ(ret, (QBDI::rword) 15);
    ASSERT_NE((uint32_t) 0, count1);
    ASSERT_NE((uint32_t) 0, count2);

    vm->deleteInstrumentation(instr1);

    count1 = 0;
    count2 = 0;
    QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 4});
    ran = vm->run((QBDI::rword) dummyFun4, (QBDI::rword) FAKE_RET_ADDR);
    ASSERT_TRUE(ran);
    ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ(ret, (QBDI::rword) 10);
    ASSERT_EQ((uint32_t) 0, count1);
    ASSERT_EQ((uint32_t) 0, count2);

    count1 = 0;
    count2 = 0;
    QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 4, 5});
    ran = vm->run((QBDI::rword) dummyFun5, (QBDI::rword) FAKE_RET_ADDR);
    ASSERT_TRUE(ran);
    ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ(ret, (QBDI::rword) 15);
    ASSERT_EQ((uint32_t) 0, count1);
    ASSERT_NE((uint32_t) 0, count2);

    instr1 = vm->addCodeCB(QBDI::InstPosition::POSTINST, countInstruction, &count1);

    count1 = 0;
    count2 = 0;
    QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 4, 5});
    ran = vm->run((QBDI::rword) dummyFun5, (QBDI::rword) FAKE_RET_ADDR);
    ASSERT_TRUE(ran);
    ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ(ret, (QBDI::rword) 15);
    ASSERT_NE((uint32_t) 0, count1);
    ASSERT_NE((uint32_t) 0, count2);

    vm->deleteInstrumentation(instr2);

    count1 = 0;
    count2 = 0;
    QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 4});
    ran = vm->run((QBDI::rword) dummyFun4, (QBDI::rword) FAKE_RET_ADDR);
    ASSERT_TRUE(ran);
    ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ(ret, (QBDI::rword) 10);
    ASSERT_NE((uint32_t) 0, count1);
    ASSERT_EQ((uint32_t) 0, count2);

    count1 = 0;
    count2 = 0;
    QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 4, 5});
    ran = vm->run((QBDI::rword) dummyFun5, (QBDI::rword) FAKE_RET_ADDR);
    ASSERT_TRUE(ran);
    ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ(ret, (QBDI::rword) 15);
    ASSERT_NE((uint32_t) 0, count1);
    ASSERT_EQ((uint32_t) 0, count2);
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
    EXPECT_EQ(instAnalysis3->disassembly, nullptr);
    EXPECT_EQ(instAnalysis3->operands, nullptr);
    const QBDI::InstAnalysis* instAnalysis4 = vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION | QBDI::ANALYSIS_DISASSEMBLY);
    EXPECT_NE(instAnalysis4->disassembly, nullptr);
    EXPECT_EQ(instAnalysis4->operands, nullptr);
    const QBDI::InstAnalysis* instAnalysis5 = vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
    EXPECT_NE(instAnalysis5->disassembly, nullptr);
    EXPECT_EQ(instAnalysis5->operands, nullptr);

    return QBDI::VMAction::BREAK_TO_VM;
}

TEST_F(VMTest, DelayedCacheFlush) {
    uint32_t count = 0;
    FunkyInfo info = FunkyInfo {0, 0};

    bool instrumented = vm->addInstrumentedModuleFromAddr((QBDI::rword)&dummyFunCall);
    ASSERT_TRUE(instrumented);
    vm->addCodeCB(QBDI::InstPosition::POSTINST, countInstruction, &count);
    info.instID = vm->addCodeRangeCB((QBDI::rword) dummyFun4, ((QBDI::rword) dummyFun4) + 10,
                                    QBDI::InstPosition::POSTINST, funkyCountInstruction, &info);

    QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 4});
    bool ran = vm->run((QBDI::rword) dummyFun4, (QBDI::rword) FAKE_RET_ADDR);
    ASSERT_TRUE(ran);
    QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
    ASSERT_EQ(ret, (QBDI::rword) 10);
    ASSERT_EQ(count, info.count);
}

