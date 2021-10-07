/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2021 Quarkslab
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
#include "APITest.h"

#include "inttypes.h"

#include "QBDI/Memory.hpp"
#include "QBDI/Platform.h"
#include "Utility/LogSys.h"
#include "Utility/String.h"

#if defined(QBDI_ARCH_X86)
#include "X86/VMTest_X86.h"
#elif defined(QBDI_ARCH_X86_64)
#include "X86_64/VMTest_X86_64.h"
#elif defined(QBDI_ARCH_ARM)
#include "ARM/VMTest_ARM.h"
#elif defined(QBDI_ARCH_AARCH64)
#include "AARCH64/VMTest_AARCH64.h"
#else
#error "Architecture not supported"
#endif

#define FAKE_RET_ADDR 0x666

QBDI_DISABLE_ASAN QBDI_NOINLINE int dummyFun0() { return 42; }

QBDI_DISABLE_ASAN QBDI_NOINLINE int dummyFun1(int arg0) { return arg0; }

QBDI_DISABLE_ASAN QBDI_NOINLINE int dummyFun4(int arg0, int arg1, int arg2,
                                              int arg3) {
  return arg0 + arg1 + arg2 + arg3;
}

QBDI_DISABLE_ASAN QBDI_NOINLINE int dummyFun5(int arg0, int arg1, int arg2,
                                              int arg3, int arg4) {
  return arg0 + arg1 + arg2 + arg3 + arg4;
}

QBDI_DISABLE_ASAN QBDI_NOINLINE int dummyFun8(int arg0, int arg1, int arg2,
                                              int arg3, int arg4, int arg5,
                                              int arg6, int arg7) {
  return arg0 + arg1 + arg2 + arg3 + arg4 + arg5 + arg6 + arg7;
}

QBDI_DISABLE_ASAN QBDI_NOINLINE int dummyFunCall(int arg0) {
  // use simple BUT multiplatform functions to test external calls
  uint8_t *useless = (uint8_t *)QBDI::alignedAlloc(256, 16);
  if (useless) {
    *(int *)useless = arg0;
    QBDI::alignedFree(useless);
  }
  return dummyFun1(arg0);
}

QBDI_DISABLE_ASAN QBDI_NOINLINE int dummyFunBB(int arg0, int arg1, int arg2,
                                               int (*f0)(int), int (*f1)(int),
                                               int (*f2)(int)) {
  int r = 0;
  if (arg0 & 1) {
    r = f1(f0(arg1)) + arg2;
    r ^= arg0;
  } else {
    r = f0(f1(arg2)) + arg1;
    r ^= arg0;
  }
  r = f2(r + arg0 + arg1 + arg2);
  if (arg0 & 2) {
    r += f1(f0(arg2 + r)) + arg1;
    r ^= arg0;
  } else {
    r += f0(f1(arg1 + r)) + arg2;
    r ^= arg0;
  }
  return r;
}

TEST_CASE_METHOD(APITest, "VMTest-Call0") {
  QBDI::simulateCall(state, FAKE_RET_ADDR);

  vm.run((QBDI::rword)dummyFun0, (QBDI::rword)FAKE_RET_ADDR);
  QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
  REQUIRE(ret == (QBDI::rword)42);

  SUCCEED();
}

TEST_CASE_METHOD(APITest, "VMTest-Call1") {
  QBDI::simulateCall(state, FAKE_RET_ADDR, {42});

  vm.run((QBDI::rword)dummyFun1, (QBDI::rword)FAKE_RET_ADDR);
  QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
  REQUIRE(ret == (QBDI::rword)dummyFun1(42));

  SUCCEED();
}

TEST_CASE_METHOD(APITest, "VMTest-Call4") {
  QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 5});

  vm.run((QBDI::rword)dummyFun4, (QBDI::rword)FAKE_RET_ADDR);
  QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
  REQUIRE(ret == (QBDI::rword)dummyFun4(1, 2, 3, 5));

  SUCCEED();
}

TEST_CASE_METHOD(APITest, "VMTest-Call5") {
  QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 5, 8});

  vm.run((QBDI::rword)dummyFun5, (QBDI::rword)FAKE_RET_ADDR);
  QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
  REQUIRE(ret == (QBDI::rword)dummyFun5(1, 2, 3, 5, 8));

  SUCCEED();
}

TEST_CASE_METHOD(APITest, "VMTest-Call8") {
  QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 5, 8, 13, 21, 34});

  vm.run((QBDI::rword)dummyFun8, (QBDI::rword)FAKE_RET_ADDR);
  QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
  REQUIRE(ret == (QBDI::rword)dummyFun8(1, 2, 3, 5, 8, 13, 21, 34));

  SUCCEED();
}

TEST_CASE_METHOD(APITest, "VMTest-ExternalCall") {

  dummyFunCall(42);

  QBDI::simulateCall(state, FAKE_RET_ADDR, {42});

  vm.run((QBDI::rword)dummyFunCall, (QBDI::rword)FAKE_RET_ADDR);
  QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
  REQUIRE(ret == (QBDI::rword)dummyFun1(42));

  SUCCEED();
}

QBDI::VMAction countInstruction(QBDI::VMInstanceRef vm,
                                QBDI::GPRState *gprState,
                                QBDI::FPRState *fprState, void *data) {
  *((uint32_t *)data) += 1;
  return QBDI::VMAction::CONTINUE;
}

QBDI::VMAction evilCbk(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState, void *data) {
  const QBDI::InstAnalysis *ana = vm->getInstAnalysis();
  CHECK(ana->mnemonic != nullptr);
  CHECK(ana->disassembly != nullptr);
  CHECK(ana->operands == nullptr);
  QBDI::rword *info = (QBDI::rword *)data;
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
TEST_CASE_METHOD(APITest, "VMTest-Breakpoint") {
  uint32_t counter = 0;
  QBDI::rword retval = 0;
  vm.addCodeAddrCB((QBDI::rword)dummyFun0, QBDI::InstPosition::PREINST,
                   countInstruction, &counter);
  vm.call(&retval, (QBDI::rword)dummyFun0);
  REQUIRE(retval == (QBDI::rword)42);
  REQUIRE(counter == 1u);

  SUCCEED();
}

TEST_CASE_METHOD(APITest, "VMTest-InstCallback") {
  QBDI::rword info[2] = {42, 0};
  QBDI::simulateCall(state, FAKE_RET_ADDR, {info[0]});

  uint32_t instrId = vm.addCodeCB(QBDI::InstPosition::POSTINST, evilCbk, &info);

  bool ran = vm.run((QBDI::rword)satanicFun, (QBDI::rword)FAKE_RET_ADDR);
  REQUIRE(ran);

  QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
  REQUIRE(ret == (QBDI::rword)satanicFun(info[0]));
  REQUIRE(info[1] == (QBDI::rword)1);

  bool success = vm.deleteInstrumentation(instrId);
  REQUIRE(success);

  SUCCEED();
}

QBDI::VMAction evilMnemCbk(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState,
                           QBDI::FPRState *fprState, void *data) {
  QBDI::rword *info = (QBDI::rword *)data;
  if (info[0] >= MNEM_COUNT)
    return QBDI::VMAction::CONTINUE;

  // get instruction metadata
  const QBDI::InstAnalysis *ana =
      vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION | QBDI::ANALYSIS_OPERANDS);
  // validate mnemonic
  CHECKED_IF(QBDI::String::startsWith(MNEM_CMP, ana->mnemonic)) {
    info[0]++; // CMP count
    info[1]++;
    // validate address
    CHECKED_IF(ana->address >= (QBDI::rword)&satanicFun)
    CHECKED_IF(ana->address < (((QBDI::rword)&satanicFun) + 0x100))
    info[1]++;
    // validate inst size
    const struct TestInst &currentInst = TestInsts[info[0] - 1];
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
    CHECKED_IF(ana->flagsAccess == currentInst.flagsAccess) { info[1]++; }
    // validate number of analyzed operands
    CHECKED_IF(ana->numOperands == currentInst.numOperands) { info[1]++; }
    // validate operands
    CHECKED_IF(ana->operands != nullptr) {
      info[1]++;
      for (uint8_t idx = 0;
           idx < std::min(ana->numOperands, currentInst.numOperands); idx++) {
        const QBDI::OperandAnalysis &cmpOp = currentInst.operands[idx];
        const QBDI::OperandAnalysis &op = ana->operands[idx];
        CHECKED_IF(op.type == cmpOp.type) { info[1]++; }
        if (op.type == QBDI::OPERAND_IMM) {
          CHECKED_IF(op.value == cmpOp.value) { info[1]++; }
        }
        if (op.regName == nullptr && cmpOp.regName == nullptr) {
          info[1]++;
        } else {
          CHECKED_IF(op.regName != nullptr)
          CHECKED_IF(cmpOp.regName != nullptr)
          CHECKED_IF(std::string(op.regName) == std::string(cmpOp.regName))
          info[1]++;
        }
        CHECKED_IF(op.size == cmpOp.size) { info[1]++; }
        CHECKED_IF(op.regCtxIdx == cmpOp.regCtxIdx) { info[1]++; }
        CHECKED_IF(op.regOff == cmpOp.regOff) { info[1]++; }
        CHECKED_IF(op.regAccess == cmpOp.regAccess) { info[1]++; }
      }
    }
  }
  return QBDI::VMAction::CONTINUE;
}

TEST_CASE_METHOD(APITest, "VMTest-MnemCallback") {
  QBDI::rword info[3] = {0, 0, 42};
  QBDI::rword retval = 0;
  const char *noop = MNEM_CMP;

  uint32_t instrId =
      vm.addMnemonicCB(noop, QBDI::InstPosition::PREINST, evilMnemCbk, &info);

  bool ran = vm.call(&retval, (QBDI::rword)satanicFun, {info[2]});
  REQUIRE(ran);

  CHECK(retval == (QBDI::rword)satanicFun(info[2]));
  // TODO: try to find a way to support windows
#ifdef QBDI_PLATFORM_WINDOWS
  CHECK(info[1] == (QBDI::rword)0);
#else
  CHECK(info[0] == MNEM_COUNT);
  CHECK(info[1] == (QBDI::rword)MNEM_VALIDATION);
#endif

  bool success = vm.deleteInstrumentation(instrId);
  REQUIRE(success);

  SUCCEED();
}

QBDI::VMAction checkTransfer(QBDI::VMInstanceRef vm, const QBDI::VMState *state,
                             QBDI::GPRState *gprState, QBDI::FPRState *fprState,
                             void *data) {
  int *s = (int *)data;
  if (state->event == QBDI::VMEvent::EXEC_TRANSFER_CALL) {
    REQUIRE((*s % 2) == 0);
    REQUIRE(reinterpret_cast<QBDI::rword>(dummyFun1) == state->sequenceStart);
    *s += 1;
  } else if (state->event == QBDI::VMEvent::EXEC_TRANSFER_RETURN) {
    REQUIRE((*s % 2) == 1);
    REQUIRE(reinterpret_cast<QBDI::rword>(dummyFun1) == state->sequenceStart);
    *s += 1;
  }
  return QBDI::VMAction::CONTINUE;
}

TEST_CASE_METHOD(APITest, "VMTest-VMEvent_ExecTransfer") {
  int s = 0;

  bool instrumented = vm.addInstrumentedModuleFromAddr(
      reinterpret_cast<QBDI::rword>(dummyFunBB));
  REQUIRE(instrumented);
  vm.removeInstrumentedRange(reinterpret_cast<QBDI::rword>(dummyFun1),
                             reinterpret_cast<QBDI::rword>(dummyFun1) + 1);

  uint32_t id = vm.addVMEventCB(QBDI::VMEvent::EXEC_TRANSFER_CALL,
                                checkTransfer, (void *)&s);
  REQUIRE(id != QBDI::INVALID_EVENTID);
  id = vm.addVMEventCB(QBDI::VMEvent::EXEC_TRANSFER_RETURN, checkTransfer,
                       (void *)&s);
  REQUIRE(id != QBDI::INVALID_EVENTID);
  QBDI::rword retval;
  bool ran = vm.call(&retval, reinterpret_cast<QBDI::rword>(dummyFunBB),
                     {0, 0, 0, reinterpret_cast<QBDI::rword>(dummyFun1),
                      reinterpret_cast<QBDI::rword>(dummyFun1),
                      reinterpret_cast<QBDI::rword>(dummyFun1)});
  REQUIRE(ran);
  REQUIRE(retval == (QBDI::rword)0);
  REQUIRE(10 == s);
  vm.deleteAllInstrumentations();
}

struct CheckBasicBlockData {
  bool waitingEnd;
  QBDI::rword BBStart;
  QBDI::rword BBEnd;
  size_t count;
};

static QBDI::VMAction checkBasicBlock(QBDI::VMInstanceRef vm,
                                      const QBDI::VMState *vmState,
                                      QBDI::GPRState *gprState,
                                      QBDI::FPRState *fprState, void *data_) {
  CheckBasicBlockData *data = static_cast<CheckBasicBlockData *>(data_);
  CHECK((vmState->event & (QBDI::BASIC_BLOCK_ENTRY | QBDI::BASIC_BLOCK_EXIT)) !=
        0);
  CHECK((vmState->event & (QBDI::BASIC_BLOCK_ENTRY | QBDI::BASIC_BLOCK_EXIT)) !=
        (QBDI::BASIC_BLOCK_ENTRY | QBDI::BASIC_BLOCK_EXIT));
  if (vmState->event & QBDI::BASIC_BLOCK_ENTRY) {
    CHECK_FALSE(data->waitingEnd);
    CHECK(vmState->basicBlockStart == vmState->sequenceStart);
    *data = {true, vmState->basicBlockStart, vmState->basicBlockEnd,
             data->count};
  } else if (vmState->event & QBDI::BASIC_BLOCK_EXIT) {
    CHECK(data->waitingEnd);
    CHECK(data->BBStart == vmState->basicBlockStart);
    CHECK(data->BBEnd == vmState->basicBlockEnd);
    CHECK(vmState->basicBlockEnd == vmState->sequenceEnd);
    data->waitingEnd = false;
    data->count++;
  }
  return QBDI::VMAction::CONTINUE;
}

TEST_CASE_METHOD(APITest, "VMTest-VMEvent_BasicBlock") {
  CheckBasicBlockData data{false, 0, 0, 0};
  vm.addVMEventCB(QBDI::BASIC_BLOCK_ENTRY | QBDI::BASIC_BLOCK_EXIT,
                  checkBasicBlock, &data);

  // backup GPRState to have the same state before each run
  QBDI::GPRState backup = *(vm.getGPRState());

  for (QBDI::rword j = 0; j < 4; j++) {
    for (QBDI::rword i = 0; i < 8; i++) {
      QBDI_DEBUG("Begin Loop iteration {} {}", j, i);
      vm.setGPRState(&backup);

      data.waitingEnd = false;
      data.count = 0;
      QBDI::rword retval;
      bool ran =
          vm.call(&retval, reinterpret_cast<QBDI::rword>(dummyFunBB),
                  {i ^ j, 5, 13, reinterpret_cast<QBDI::rword>(dummyFun1),
                   reinterpret_cast<QBDI::rword>(dummyFun1),
                   reinterpret_cast<QBDI::rword>(dummyFun1)});
      CHECK(ran);
      CHECK_FALSE(data.waitingEnd);
      CHECK(data.count != 0);
    }
    vm.clearAllCache();
  }
}

TEST_CASE_METHOD(APITest, "VMTest-CacheInvalidation") {
  uint32_t count1 = 0;
  uint32_t count2 = 0;

  bool instrumented =
      vm.addInstrumentedModuleFromAddr((QBDI::rword)&dummyFunCall);
  REQUIRE(instrumented);
  uint32_t instr1 =
      vm.addCodeCB(QBDI::InstPosition::POSTINST, countInstruction, &count1);

  count1 = 0;
  count2 = 0;
  QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 4});
  bool ran = vm.run((QBDI::rword)dummyFun4, (QBDI::rword)FAKE_RET_ADDR);
  REQUIRE(ran);
  QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
  REQUIRE(ret == (QBDI::rword)10);
  REQUIRE((uint32_t)0 != count1);
  REQUIRE((uint32_t)0 == count2);

  uint32_t instr2 = vm.addCodeRangeCB(
      (QBDI::rword)&dummyFun5, ((QBDI::rword)&dummyFun5) + 64,
      QBDI::InstPosition::POSTINST, countInstruction, &count2);

  count1 = 0;
  count2 = 0;
  QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 4, 5});
  ran = vm.run((QBDI::rword)dummyFun5, (QBDI::rword)FAKE_RET_ADDR);
  REQUIRE(ran);
  ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
  REQUIRE(ret == (QBDI::rword)15);
  REQUIRE((uint32_t)0 != count1);
  REQUIRE((uint32_t)0 != count2);

  vm.deleteInstrumentation(instr1);

  count1 = 0;
  count2 = 0;
  QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 4});
  ran = vm.run((QBDI::rword)dummyFun4, (QBDI::rword)FAKE_RET_ADDR);
  REQUIRE(ran);
  ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
  REQUIRE(ret == (QBDI::rword)10);
  REQUIRE((uint32_t)0 == count1);
  REQUIRE((uint32_t)0 == count2);

  count1 = 0;
  count2 = 0;
  QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 4, 5});
  ran = vm.run((QBDI::rword)dummyFun5, (QBDI::rword)FAKE_RET_ADDR);
  REQUIRE(ran);
  ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
  REQUIRE(ret == (QBDI::rword)15);
  REQUIRE((uint32_t)0 == count1);
  REQUIRE((uint32_t)0 != count2);

  instr1 =
      vm.addCodeCB(QBDI::InstPosition::POSTINST, countInstruction, &count1);

  count1 = 0;
  count2 = 0;
  QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 4, 5});
  ran = vm.run((QBDI::rword)dummyFun5, (QBDI::rword)FAKE_RET_ADDR);
  REQUIRE(ran);
  ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
  REQUIRE(ret == (QBDI::rword)15);
  REQUIRE((uint32_t)0 != count1);
  REQUIRE((uint32_t)0 != count2);

  vm.deleteInstrumentation(instr2);

  count1 = 0;
  count2 = 0;
  QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 4});
  ran = vm.run((QBDI::rword)dummyFun4, (QBDI::rword)FAKE_RET_ADDR);
  REQUIRE(ran);
  ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
  REQUIRE(ret == (QBDI::rword)10);
  REQUIRE((uint32_t)0 != count1);
  REQUIRE((uint32_t)0 == count2);

  count1 = 0;
  count2 = 0;
  QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 4, 5});
  ran = vm.run((QBDI::rword)dummyFun5, (QBDI::rword)FAKE_RET_ADDR);
  REQUIRE(ran);
  ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
  REQUIRE(ret == (QBDI::rword)15);
  REQUIRE((uint32_t)0 != count1);
  REQUIRE((uint32_t)0 == count2);
}

struct FunkyInfo {
  uint32_t instID;
  uint32_t count;
};

QBDI::VMAction funkyCountInstruction(QBDI::VMInstanceRef vm,
                                     QBDI::GPRState *gprState,
                                     QBDI::FPRState *fprState, void *data) {
  FunkyInfo *info = (FunkyInfo *)data;

  const QBDI::InstAnalysis *instAnalysis1 =
      vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
  vm->deleteInstrumentation(info->instID);
  const QBDI::InstAnalysis *instAnalysis2 =
      vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
  info->instID = vm->addCodeRangeCB(QBDI_GPR_GET(gprState, QBDI::REG_PC),
                                    QBDI_GPR_GET(gprState, QBDI::REG_PC) + 10,
                                    QBDI::InstPosition::POSTINST,
                                    funkyCountInstruction, data);
  const QBDI::InstAnalysis *instAnalysis3 =
      vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
  // instAnalysis1, instAnalysis2 and instAnalysis3 should be the same pointer
  // because the cache flush initiated by deleteInstrumentation and
  // addCodeRangeCB is delayed.
  if (instAnalysis1 == instAnalysis2 && instAnalysis2 == instAnalysis3) {
    info->count += 1;
  }

  // instAnalysis3 should not have disassembly information, but instAnalysis4
  // and instAnalysis5 should.
  CHECK(instAnalysis3->disassembly == nullptr);
  CHECK(instAnalysis3->operands == nullptr);
  const QBDI::InstAnalysis *instAnalysis4 = vm->getInstAnalysis(
      QBDI::ANALYSIS_INSTRUCTION | QBDI::ANALYSIS_DISASSEMBLY);
  CHECK(instAnalysis4->disassembly != nullptr);
  CHECK(instAnalysis4->operands == nullptr);
  const QBDI::InstAnalysis *instAnalysis5 =
      vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
  CHECK(instAnalysis5->disassembly != nullptr);
  CHECK(instAnalysis5->operands == nullptr);

  return QBDI::VMAction::BREAK_TO_VM;
}

TEST_CASE_METHOD(APITest, "VMTest-DelayedCacheFlush") {
  uint32_t count = 0;
  FunkyInfo info = FunkyInfo{0, 0};

  bool instrumented =
      vm.addInstrumentedModuleFromAddr((QBDI::rword)&dummyFunCall);
  REQUIRE(instrumented);
  vm.addCodeCB(QBDI::InstPosition::POSTINST, countInstruction, &count);
  info.instID = vm.addCodeRangeCB(
      (QBDI::rword)dummyFun4, ((QBDI::rword)dummyFun4) + 10,
      QBDI::InstPosition::POSTINST, funkyCountInstruction, &info);

  QBDI::simulateCall(state, FAKE_RET_ADDR, {1, 2, 3, 4});
  bool ran = vm.run((QBDI::rword)dummyFun4, (QBDI::rword)FAKE_RET_ADDR);
  REQUIRE(ran);
  QBDI::rword ret = QBDI_GPR_GET(state, QBDI::REG_RETURN);
  REQUIRE(ret == (QBDI::rword)10);
  REQUIRE(count == info.count);
}

struct PriorityDataCall {
  QBDI::rword addr;
  QBDI::InstPosition pos;
  int priority;

  PriorityDataCall(QBDI::rword addr, QBDI::InstPosition pos, int priority)
      : addr(addr), pos(pos), priority(priority) {}
};

static std::vector<QBDI::InstrRuleDataCBK>
priorityInstrCB(QBDI::VMInstanceRef vm, const QBDI::InstAnalysis *inst,
                void *data_) {
  std::vector<QBDI::InstrRuleDataCBK> r;

  r.emplace_back(
      QBDI::InstPosition::PREINST,
      [](QBDI::VMInstanceRef vm, QBDI::GPRState *gprState,
         QBDI::FPRState *fprState, void *data) -> QBDI::VMAction {
        ((std::vector<PriorityDataCall> *)data)
            ->emplace_back(
                vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION)->address,
                QBDI::InstPosition::PREINST, -100);

        return QBDI::VMAction::CONTINUE;
      },
      data_, -100);

  r.emplace_back(
      QBDI::InstPosition::POSTINST,
      [](QBDI::VMInstanceRef vm, QBDI::GPRState *gprState,
         QBDI::FPRState *fprState, void *data) -> QBDI::VMAction {
        ((std::vector<PriorityDataCall> *)data)
            ->emplace_back(
                vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION)->address,
                QBDI::InstPosition::POSTINST, 0);

        return QBDI::VMAction::CONTINUE;
      },
      data_, 0);

  r.emplace_back(
      QBDI::InstPosition::POSTINST,
      [](QBDI::VMInstanceRef vm, QBDI::GPRState *gprState,
         QBDI::FPRState *fprState, void *data) -> QBDI::VMAction {
        ((std::vector<PriorityDataCall> *)data)
            ->emplace_back(
                vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION)->address,
                QBDI::InstPosition::POSTINST, 100);

        return QBDI::VMAction::CONTINUE;
      },
      data_, 100);

  r.emplace_back(
      QBDI::InstPosition::PREINST,
      [](QBDI::VMInstanceRef vm, QBDI::GPRState *gprState,
         QBDI::FPRState *fprState, void *data) -> QBDI::VMAction {
        ((std::vector<PriorityDataCall> *)data)
            ->emplace_back(
                vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION)->address,
                QBDI::InstPosition::PREINST, 100);

        return QBDI::VMAction::CONTINUE;
      },
      data_, 100);

  r.emplace_back(
      QBDI::InstPosition::PREINST,
      [](QBDI::VMInstanceRef vm, QBDI::GPRState *gprState,
         QBDI::FPRState *fprState, void *data) -> QBDI::VMAction {
        ((std::vector<PriorityDataCall> *)data)
            ->emplace_back(
                vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION)->address,
                QBDI::InstPosition::PREINST, 0);

        return QBDI::VMAction::CONTINUE;
      },
      data_, 0);

  r.emplace_back(
      QBDI::InstPosition::POSTINST,
      [](QBDI::VMInstanceRef vm, QBDI::GPRState *gprState,
         QBDI::FPRState *fprState, void *data) -> QBDI::VMAction {
        ((std::vector<PriorityDataCall> *)data)
            ->emplace_back(
                vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION)->address,
                QBDI::InstPosition::POSTINST, -100);

        return QBDI::VMAction::CONTINUE;
      },
      data_, -100);

  return r;
}

TEST_CASE_METHOD(APITest, "VMTest-Priority") {
  std::vector<PriorityDataCall> callList;
  QBDI::rword retval = 0;

  vm.addCodeCB(
      QBDI::InstPosition::PREINST,
      [](QBDI::VMInstanceRef vm, QBDI::GPRState *gprState,
         QBDI::FPRState *fprState, void *data) -> QBDI::VMAction {
        ((std::vector<PriorityDataCall> *)data)
            ->emplace_back(
                vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION)->address,
                QBDI::InstPosition::PREINST, -10);

        return QBDI::VMAction::CONTINUE;
      },
      &callList, -10);

  vm.addCodeCB(
      QBDI::InstPosition::POSTINST,
      [](QBDI::VMInstanceRef vm, QBDI::GPRState *gprState,
         QBDI::FPRState *fprState, void *data) -> QBDI::VMAction {
        ((std::vector<PriorityDataCall> *)data)
            ->emplace_back(
                vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION)->address,
                QBDI::InstPosition::POSTINST, -67);

        return QBDI::VMAction::CONTINUE;
      },
      &callList, -67);

  vm.addCodeCB(
      QBDI::InstPosition::POSTINST,
      [](QBDI::VMInstanceRef vm, QBDI::GPRState *gprState,
         QBDI::FPRState *fprState, void *data) -> QBDI::VMAction {
        ((std::vector<PriorityDataCall> *)data)
            ->emplace_back(
                vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION)->address,
                QBDI::InstPosition::POSTINST, 56);

        return QBDI::VMAction::CONTINUE;
      },
      &callList, 56);

  vm.addInstrRule(priorityInstrCB, QBDI::ANALYSIS_INSTRUCTION, &callList);

  vm.addCodeCB(
      QBDI::InstPosition::PREINST,
      [](QBDI::VMInstanceRef vm, QBDI::GPRState *gprState,
         QBDI::FPRState *fprState, void *data) -> QBDI::VMAction {
        ((std::vector<PriorityDataCall> *)data)
            ->emplace_back(
                vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION)->address,
                QBDI::InstPosition::PREINST, 27);

        return QBDI::VMAction::CONTINUE;
      },
      &callList, 27);

  vm.addCodeCB(
      QBDI::InstPosition::PREINST,
      [](QBDI::VMInstanceRef vm, QBDI::GPRState *gprState,
         QBDI::FPRState *fprState, void *data) -> QBDI::VMAction {
        ((std::vector<PriorityDataCall> *)data)
            ->emplace_back(
                vm->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION)->address,
                QBDI::InstPosition::PREINST, -77);

        return QBDI::VMAction::CONTINUE;
      },
      &callList, -77);

  vm.call(&retval, (QBDI::rword)dummyFun0);
  REQUIRE(retval == (QBDI::rword)42);

  REQUIRE(callList.size() >= 11);

  for (int i = 1; i < callList.size(); i++) {
    if (callList[i - 1].addr == callList[i].addr) {
      if (callList[i - 1].pos == callList[i].pos) {
        REQUIRE(callList[i - 1].priority >= callList[i].priority);
      } else {
        REQUIRE(callList[i - 1].pos == QBDI::InstPosition::PREINST);
        REQUIRE(callList[i].pos == QBDI::InstPosition::POSTINST);
      }
    }
  }

  SUCCEED();
}

// Test copy/move constructor/assignment operator

struct MoveCallbackStruct {
  QBDI::VMInstanceRef expectedRef;
  bool allowedNewBlock;

  bool reachEventCB;
  bool reachInstCB;
  bool reachInstrumentCB;
  bool reachCB2;
};

static QBDI::VMAction allowedNewBlock(QBDI::VMInstanceRef vm,
                                      const QBDI::VMState *state,
                                      QBDI::GPRState *, QBDI::FPRState *,
                                      void *data_) {
  MoveCallbackStruct *data = static_cast<MoveCallbackStruct *>(data_);
  CHECK(data->expectedRef == vm);
  CHECK(
      (data->allowedNewBlock or ((state->event & QBDI::BASIC_BLOCK_NEW) == 0)));

  data->reachEventCB = true;
  return QBDI::VMAction::CONTINUE;
}

static std::vector<QBDI::InstrRuleDataCBK>
instrumentCopyCB(QBDI::VMInstanceRef vm, const QBDI::InstAnalysis *inst,
                 void *data_) {
  MoveCallbackStruct *data = static_cast<MoveCallbackStruct *>(data_);
  CHECK(data->expectedRef == vm);
  CHECK(data->allowedNewBlock);

  data->reachInstrumentCB = true;
  return {};
}

static QBDI::VMAction verifyVMRef(QBDI::VMInstanceRef vm, QBDI::GPRState *,
                                  QBDI::FPRState *, void *data_) {
  MoveCallbackStruct *data = static_cast<MoveCallbackStruct *>(data_);
  CHECK(data->expectedRef == vm);

  data->reachInstCB = true;
  return QBDI::VMAction::CONTINUE;
}

static QBDI::VMAction verifyCB2(QBDI::VMInstanceRef vm, QBDI::GPRState *,
                                QBDI::FPRState *, void *data_) {
  MoveCallbackStruct *data = static_cast<MoveCallbackStruct *>(data_);
  CHECK(data->expectedRef == vm);

  data->reachCB2 = true;
  return QBDI::VMAction::CONTINUE;
}

TEST_CASE("VMTest-MoveConstructor") {

  APITest vm1_;
  std::unique_ptr<QBDI::VM> vm1 = std::make_unique<QBDI::VM>(vm1_.vm);
  QBDI::VM *vm = vm1.get();

  MoveCallbackStruct data{vm, true, false, false, false, false};

  bool instrumented =
      vm->addInstrumentedModuleFromAddr((QBDI::rword)&dummyFunCall);
  REQUIRE(instrumented);

  vm->addCodeCB(QBDI::InstPosition::POSTINST, verifyVMRef, &data);
  vm->addInstrRule(instrumentCopyCB, QBDI::ANALYSIS_INSTRUCTION, &data);
  vm->addVMEventCB(QBDI::SEQUENCE_ENTRY | QBDI::SEQUENCE_EXIT |
                       QBDI::BASIC_BLOCK_NEW,
                   allowedNewBlock, &data);

  QBDI::rword retvalue;

  QBDI_DEBUG("Execute dummyFun1 with the original VM");
  vm->call(&retvalue, (QBDI::rword)dummyFun1, {350});
  REQUIRE(retvalue == 350);
  REQUIRE(data.reachEventCB);
  REQUIRE(data.reachInstCB);
  REQUIRE(data.reachInstrumentCB);

  data.reachEventCB = false;
  data.reachInstCB = false;
  data.reachInstrumentCB = false;
  data.allowedNewBlock = false;

  REQUIRE(vm == data.expectedRef);

  // move vm
  QBDI_DEBUG("Move the VM");
  QBDI::VM movedVM(std::move(*vm));
  vm = nullptr;

  vm1.reset();
  REQUIRE(vm1.get() == nullptr);

  REQUIRE(data.expectedRef != &movedVM);
  data.expectedRef = &movedVM;

  QBDI_DEBUG("Execute with the moved VM");
  movedVM.call(&retvalue, (QBDI::rword)dummyFun1, {780});

  REQUIRE(retvalue == 780);
  REQUIRE(data.reachEventCB);
  REQUIRE(data.reachInstCB);
  REQUIRE_FALSE(data.reachInstrumentCB);

  data.allowedNewBlock = true;
  movedVM.precacheBasicBlock((QBDI::rword)dummyFun0);
  REQUIRE(data.reachInstrumentCB);
}

TEST_CASE("VMTest-CopyConstructor") {

  APITest vm1;
  QBDI::VM *vm = &vm1.vm;

  MoveCallbackStruct data{vm, true, false, false, false, false};

  bool instrumented =
      vm->addInstrumentedModuleFromAddr((QBDI::rword)&dummyFunCall);
  REQUIRE(instrumented);

  vm->addCodeCB(QBDI::InstPosition::POSTINST, verifyVMRef, &data);
  vm->addInstrRule(instrumentCopyCB, QBDI::ANALYSIS_INSTRUCTION, &data);
  vm->addVMEventCB(QBDI::SEQUENCE_ENTRY | QBDI::SEQUENCE_EXIT |
                       QBDI::BASIC_BLOCK_NEW,
                   allowedNewBlock, &data);

  QBDI::rword retvalue;

  QBDI_DEBUG("Execute dummyFun1 with the original VM");
  vm->call(&retvalue, (QBDI::rword)dummyFun1, {350});
  REQUIRE(retvalue == 350);
  REQUIRE(data.reachEventCB);
  REQUIRE(data.reachInstCB);
  REQUIRE(data.reachInstrumentCB);

  data.reachEventCB = false;
  data.reachInstCB = false;
  data.reachInstrumentCB = false;
  data.allowedNewBlock = false;

  // copy vm
  QBDI_DEBUG("Copy the VM");
  QBDI::VM movedVM(*vm);

  REQUIRE(data.expectedRef != &movedVM);

  QBDI_DEBUG("Execute second time with the original VM");
  vm->call(&retvalue, (QBDI::rword)dummyFun1, {620});
  REQUIRE(retvalue == 620);
  REQUIRE(data.reachEventCB);
  REQUIRE(data.reachInstCB);
  REQUIRE_FALSE(data.reachInstrumentCB);

  data.reachEventCB = false;
  data.reachInstCB = false;
  data.reachInstrumentCB = false;
  data.allowedNewBlock = true;
  data.expectedRef = &movedVM;

  QBDI_DEBUG("Execute with the copied VM");
  movedVM.call(&retvalue, (QBDI::rword)dummyFun1, {780});
  REQUIRE(retvalue == 780);
  REQUIRE(data.reachEventCB);
  REQUIRE(data.reachInstCB);
  REQUIRE(data.reachInstrumentCB);
}

TEST_CASE("VMTest-MoveAssignmentOperator") {

  APITest vm1__;
  APITest vm2__;
  std::unique_ptr<QBDI::VM> vm1_ = std::make_unique<QBDI::VM>(vm1__.vm);
  std::unique_ptr<QBDI::VM> vm2_ = std::make_unique<QBDI::VM>(vm2__.vm);
  QBDI::VM *vm1 = vm1_.get();
  QBDI::VM *vm2 = vm2_.get();
  REQUIRE(vm1 != vm2);

  MoveCallbackStruct data1{vm1, true, false, false, false, false};
  MoveCallbackStruct data2{vm2, true, false, false, false, false};

  bool instrumented =
      vm1->addInstrumentedModuleFromAddr((QBDI::rword)&dummyFunCall);
  REQUIRE(instrumented);
  instrumented = vm2->addInstrumentedModuleFromAddr((QBDI::rword)&dummyFunCall);
  REQUIRE(instrumented);

  vm1->addCodeCB(QBDI::InstPosition::POSTINST, verifyVMRef, &data1);
  vm1->addInstrRule(instrumentCopyCB, QBDI::ANALYSIS_INSTRUCTION, &data1);
  vm1->addVMEventCB(QBDI::SEQUENCE_ENTRY | QBDI::SEQUENCE_EXIT |
                        QBDI::BASIC_BLOCK_NEW,
                    allowedNewBlock, &data1);

  vm2->addCodeCB(QBDI::InstPosition::POSTINST, verifyVMRef, &data2);
  vm2->addInstrRule(instrumentCopyCB, QBDI::ANALYSIS_INSTRUCTION, &data2);
  vm2->addVMEventCB(QBDI::SEQUENCE_ENTRY | QBDI::SEQUENCE_EXIT |
                        QBDI::BASIC_BLOCK_NEW,
                    allowedNewBlock, &data2);

  QBDI::rword retvalue;

  vm1->call(&retvalue, (QBDI::rword)dummyFun1, {350});
  REQUIRE(retvalue == 350);
  REQUIRE(data1.reachEventCB);
  REQUIRE(data1.reachInstCB);
  REQUIRE(data1.reachInstrumentCB);

  data1.reachEventCB = false;
  data1.reachInstCB = false;
  data1.reachInstrumentCB = false;
  data1.allowedNewBlock = false;

  vm2->call(&retvalue, (QBDI::rword)dummyFun1, {670});
  REQUIRE(retvalue == 670);
  REQUIRE(data2.reachEventCB);
  REQUIRE(data2.reachInstCB);
  REQUIRE(data2.reachInstrumentCB);

  data2.reachEventCB = false;
  data2.reachInstCB = false;
  data2.reachInstrumentCB = false;
  data2.allowedNewBlock = false;

  data1.expectedRef = vm2;
  data2.expectedRef = nullptr;

  // move vm
  *vm2 = std::move(*vm1);
  vm1 = nullptr;

  vm1_.reset();
  REQUIRE(vm1_.get() == nullptr);

  vm2->call(&retvalue, (QBDI::rword)dummyFun1, {780});

  REQUIRE(retvalue == 780);
  REQUIRE(data1.reachEventCB);
  REQUIRE(data1.reachInstCB);
  REQUIRE_FALSE(data1.reachInstrumentCB);
  REQUIRE_FALSE(data2.reachEventCB);
  REQUIRE_FALSE(data2.reachInstCB);
  REQUIRE_FALSE(data2.reachInstrumentCB);

  data1.allowedNewBlock = true;
  vm2->precacheBasicBlock((QBDI::rword)dummyFun0);
  REQUIRE(data1.reachInstrumentCB);
}

TEST_CASE("VMTest-CopyAssignmentOperator") {

  APITest vm1_;
  APITest vm2_;
  QBDI::VM *vm1 = &vm1_.vm;
  QBDI::VM *vm2 = &vm2_.vm;
  REQUIRE(vm1 != vm2);

  MoveCallbackStruct data1{vm1, true, false, false, false, false};
  MoveCallbackStruct data2{vm2, true, false, false, false, false};

  bool instrumented =
      vm1->addInstrumentedModuleFromAddr((QBDI::rword)&dummyFunCall);
  REQUIRE(instrumented);
  instrumented = vm2->addInstrumentedModuleFromAddr((QBDI::rword)&dummyFunCall);
  REQUIRE(instrumented);

  vm1->addCodeCB(QBDI::InstPosition::POSTINST, verifyVMRef, &data1);
  vm1->addCodeCB(QBDI::InstPosition::POSTINST, verifyCB2, &data1);
  vm1->addInstrRule(instrumentCopyCB, QBDI::ANALYSIS_INSTRUCTION, &data1);
  vm1->addVMEventCB(QBDI::SEQUENCE_ENTRY | QBDI::SEQUENCE_EXIT |
                        QBDI::BASIC_BLOCK_NEW,
                    allowedNewBlock, &data1);

  vm2->addCodeCB(QBDI::InstPosition::POSTINST, verifyVMRef, &data2);
  vm2->addInstrRule(instrumentCopyCB, QBDI::ANALYSIS_INSTRUCTION, &data2);
  vm2->addVMEventCB(QBDI::SEQUENCE_ENTRY | QBDI::SEQUENCE_EXIT |
                        QBDI::BASIC_BLOCK_NEW,
                    allowedNewBlock, &data2);

  QBDI::rword retvalue;

  vm1->call(&retvalue, (QBDI::rword)dummyFun1, {350});
  REQUIRE(retvalue == 350);
  REQUIRE(data1.reachEventCB);
  REQUIRE(data1.reachInstCB);
  REQUIRE(data1.reachInstrumentCB);
  REQUIRE(data1.reachCB2);

  data1.reachEventCB = false;
  data1.reachInstCB = false;
  data1.reachInstrumentCB = false;
  data1.allowedNewBlock = false;
  data1.reachCB2 = false;

  vm2->call(&retvalue, (QBDI::rword)dummyFun1, {670});
  REQUIRE(retvalue == 670);
  REQUIRE(data2.reachEventCB);
  REQUIRE(data2.reachInstCB);
  REQUIRE(data2.reachInstrumentCB);
  REQUIRE_FALSE(data2.reachCB2);

  data2.reachEventCB = false;
  data2.reachInstCB = false;
  data2.reachInstrumentCB = false;
  data2.allowedNewBlock = false;
  data2.expectedRef = nullptr;

  // copy vm
  *vm2 = *vm1;

  vm1->call(&retvalue, (QBDI::rword)dummyFun1, {780});
  REQUIRE(retvalue == 780);
  REQUIRE(data1.reachEventCB);
  REQUIRE(data1.reachInstCB);
  REQUIRE_FALSE(data1.reachInstrumentCB);
  REQUIRE(data1.reachCB2);
  REQUIRE_FALSE(data2.reachEventCB);
  REQUIRE_FALSE(data2.reachInstCB);
  REQUIRE_FALSE(data2.reachCB2);
  REQUIRE_FALSE(data2.reachInstrumentCB);

  data1.reachEventCB = false;
  data1.reachInstCB = false;
  data1.reachInstrumentCB = false;
  data1.allowedNewBlock = true;
  data1.expectedRef = vm2;
  data1.reachCB2 = false;

  vm2->call(&retvalue, (QBDI::rword)dummyFun1, {567});

  REQUIRE(retvalue == 567);
  REQUIRE(data1.reachEventCB);
  REQUIRE(data1.reachInstCB);
  REQUIRE(data1.reachInstrumentCB);
  REQUIRE(data1.reachCB2);
  REQUIRE_FALSE(data2.reachEventCB);
  REQUIRE_FALSE(data2.reachInstCB);
  REQUIRE_FALSE(data2.reachCB2);
  REQUIRE_FALSE(data2.reachInstrumentCB);
}
