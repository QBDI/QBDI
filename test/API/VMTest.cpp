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
#include <gtest/gtest.h>
#include "VMTest.h"

#include "inttypes.h"

#include "Utility/String.h"
#include "Platform.h"
#include "Memory.h"

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
#define CMP_REG_NAME "DH"
#define CMP_REG_SIZE 1
#define CMP_REG_OFF 8
#define CMP_OP_CNT 2
#elif defined(QBDI_ARCH_ARM)
#define CMP_REG_NAME "R3"
#define CMP_REG_SIZE sizeof(QBDI::rword)
#define CMP_REG_OFF 0
#define CMP_OP_CNT 3
#endif
#define CMP_REG_CTXIDX 3
#define CMP_IMM_VAL 42
#define CMP_IMM_STRVAL "42"


QBDI_NOINLINE QBDI::rword satanicFun(QBDI::rword arg0) {
#if defined(QBDI_ARCH_X86) || defined(QBDI_ARCH_X86_64)
 #ifndef QBDI_OS_WIN
    asm("cmp $" CMP_IMM_STRVAL ", %" CMP_REG_NAME);
 #endif
#elif defined(QBDI_ARCH_ARM)
    asm("cmp " CMP_REG_NAME ", #" CMP_IMM_STRVAL);
#endif
    return arg0 + 0x666;
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
    if (TEST_GUARD(QBDI::String::startsWith(MNEM_CMP, ana->mnemonic))) {
        info[1]++;
        // validate address
        if (TEST_GUARD(ana->address >= (QBDI::rword) &satanicFun &&
            ana->address < (((QBDI::rword) &satanicFun) + 100))) {
            info[1]++;
        }
        // validate inst size
#if defined(QBDI_ARCH_X86) || defined(QBDI_ARCH_X86_64)
        if (TEST_GUARD(ana->instSize == 3)) {
#else
        {
#endif
            info[1]++;
        }
        // validate instruction type
        if (TEST_GUARD(!ana->isBranch &&
            !ana->isCall &&
            !ana->isReturn &&
            ana->isCompare)) {
            info[1]++;
        }
        // validate number of analyzed operands
        if (TEST_GUARD(ana->numOperands == CMP_OP_CNT)) {
            info[1]++;
        }
        // validate operands
        if (TEST_GUARD(ana->operands != nullptr)) {
            info[1]++;
            for (uint8_t idx = 0; idx < ana->numOperands; idx++) {
                const QBDI::OperandAnalysis& op = ana->operands[idx];
                // validate immediate operand value
                if (op.type == QBDI::OPERAND_IMM) {
                    if (TEST_GUARD(op.value == CMP_IMM_VAL)) {
                        info[1]++;
                    }
                // validate GPR operand metadata
                } else if (op.type == QBDI::OPERAND_GPR) {
                    if (TEST_GUARD(op.regName != nullptr &&
                        std::string(op.regName) == CMP_REG_NAME)) {
                        info[1]++;
                    }
                    if (TEST_GUARD(op.size == CMP_REG_SIZE)) {
                        info[1]++;
                    }
                    if (TEST_GUARD(op.regCtxIdx == CMP_REG_CTXIDX)) {
                        info[1]++;
                    }
                    if (TEST_GUARD(op.regOff == CMP_REG_OFF)) {
                        info[1]++;
                    }
                    if (TEST_GUARD(op.regAccess == QBDI::REGISTER_READ)) {
                        info[1]++;
                    }
                }
            }
        }
    }
    return QBDI::VMAction::CONTINUE;
}


TEST_F(VMTest, MnemCallback) {
    QBDI::rword info[2] = {42, 0};
    QBDI::rword retval = 0;
    const char* noop = MNEM_CMP;

    uint32_t instrId = vm->addMnemonicCB(noop,
                                         QBDI::InstPosition::PREINST,
                                         evilMnemCbk, &info);

    bool ran = vm->call(&retval, (QBDI::rword) satanicFun, {info[0]});
    ASSERT_TRUE(ran);

    EXPECT_EQ(retval, (QBDI::rword) satanicFun(info[0]));
    // TODO: try to find a way to support windows
#ifdef QBDI_OS_WIN
    EXPECT_EQ(info[1], (QBDI::rword) 0);
#else
    EXPECT_EQ(info[1], (QBDI::rword) 12);
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

QBDI::VMAction countInstruction(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState, QBDI::FPRState *fprState, void *data) {
    *((uint32_t*)data) += 1;
    return QBDI::VMAction::CONTINUE;
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

