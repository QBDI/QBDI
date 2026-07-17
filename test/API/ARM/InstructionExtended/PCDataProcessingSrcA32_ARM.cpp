/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2026 Quarkslab
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
#include <catch2/catch_test_macros.hpp>
#include "API/APITest.h"

#include "MemAccessTestUtils_ARM.h"

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::checkFeature;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

static constexpr QBDI::rword CPSR_N = (1u << 31);
static constexpr QBDI::rword CPSR_Z = (1u << 30);
static constexpr QBDI::rword CPSR_C = (1u << 29);
static constexpr QBDI::rword CPSR_V = (1u << 28);

struct NZCV {
  bool n, z, c, v;
};

static NZCV addFlags(uint32_t a, uint32_t b, bool carryIn) {
  uint64_t result = (uint64_t)a + (uint64_t)b + (carryIn ? 1u : 0u);
  uint32_t r32 = (uint32_t)result;
  bool c = (result >> 32) != 0;
  bool v = (((~(a ^ b)) & (a ^ r32)) & 0x80000000u) != 0;
  return {(r32 & 0x80000000u) != 0, r32 == 0, c, v};
}

static NZCV subFlags(uint32_t a, uint32_t b) { return addFlags(a, ~b, true); }

static NZCV logicFlags(uint32_t result, bool carryIn, bool overflowIn) {
  return {(result & 0x80000000u) != 0, result == 0, carryIn, overflowIn};
}

static void checkFlags(QBDI::rword cpsr, NZCV expected) {
  CHECK(((cpsr & CPSR_N) != 0) == expected.n);
  CHECK(((cpsr & CPSR_Z) != 0) == expected.z);
  CHECK(((cpsr & CPSR_C) != 0) == expected.c);
  CHECK(((cpsr & CPSR_V) != 0) == expected.v);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-cmnri_pc") {
  const char source[] = "cmn pc, #4\n";

  QBDI::rword pcVal = 0;
  bool seen = false;
  vm.addMnemonicCB("CMNri", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     pcVal = ia->address + 8;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("CMNri", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     checkFlags(gprState->cpsr, addFlags(pcVal, 4, false));
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-cmpri_pc") {
  const char source[] = "cmp pc, #4\n";

  QBDI::rword pcVal = 0;
  bool seen = false;
  vm.addMnemonicCB("CMPri", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     pcVal = ia->address + 8;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("CMPri", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     checkFlags(gprState->cpsr, subFlags(pcVal, 4));
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-teqri_pc") {
  const char source[] =
      "cmp r3, r3\n"
      "teq pc, #4\n";

  QBDI::rword pcVal = 0;
  bool seen = false;
  vm.addMnemonicCB("TEQri", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     pcVal = ia->address + 8;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("TEQri", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     checkFlags(gprState->cpsr,
                                logicFlags(pcVal ^ 4u, true, false));
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-tstri_pc") {
  const char source[] =
      "cmp r3, r3\n"
      "tst pc, #4\n";

  QBDI::rword pcVal = 0;
  bool seen = false;
  vm.addMnemonicCB("TSTri", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     pcVal = ia->address + 8;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("TSTri", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     checkFlags(gprState->cpsr,
                                logicFlags(pcVal & 4u, true, false));
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-pldwi12_pc") {
  if (!checkFeature("mp")) {
    return;
  }
  const char source[] =
      "pldw [pc, #4]\n"
      "mov r0, #0x1111\n";

  bool seenPre = false;
  bool seenPost = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PLDWi12", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seenPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("PLDWi12", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"mp"});

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == 0x1111);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-pldwrs_pc") {
  if (!checkFeature("mp")) {
    return;
  }
  const char source[] =
      "mov r1, #4\n"
      "pldw [pc, r1]\n"
      "mov r0, #0x1111\n";

  bool seenPre = false;
  bool seenPost = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PLDWrs", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seenPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("PLDWrs", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source, {}, QBDI::CPUMode::ARM, {"mp"});

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == 0x1111);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-pldi12_pc") {
  const char source[] =
      "pld [pc, #4]\n"
      "mov r0, #0x1111\n";

  bool seenPre = false;
  bool seenPost = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PLDi12", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seenPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("PLDi12", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == 0x1111);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-pldrs_pc") {
  const char source[] =
      "mov r1, #4\n"
      "pld [pc, r1]\n"
      "mov r0, #0x1111\n";

  bool seenPre = false;
  bool seenPost = false;
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PLDrs", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seenPre = true;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("PLDrs", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     CHECK(vmi->getInstMemoryAccess().empty());
                     seenPost = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seenPre);
  CHECK(seenPost);
  CHECK(retval == 0x1111);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-cmnzrr_pc") {
  const char source[] = "cmn pc, r1\n";

  QBDI::rword pcVal = 0;
  bool seen = false;
  vm.addMnemonicCB("CMNzrr", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     pcVal = ia->address + 8;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("CMNzrr", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     checkFlags(gprState->cpsr, addFlags(pcVal, 0x1000, false));
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r1 = 0x1000;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-cmnzrsi_pc") {
  const char source[] = "cmn pc, r1, lsl #2\n";

  QBDI::rword pcVal = 0;
  bool seen = false;
  vm.addMnemonicCB("CMNzrsi", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     pcVal = ia->address + 8;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("CMNzrsi", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     checkFlags(gprState->cpsr,
                                addFlags(pcVal, 0x1000u << 2, false));
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r1 = 0x1000;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-cmprr_pc") {
  const char source[] = "cmp pc, r1\n";

  QBDI::rword pcVal = 0;
  bool seen = false;
  vm.addMnemonicCB("CMPrr", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     pcVal = ia->address + 8;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("CMPrr", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     checkFlags(gprState->cpsr, subFlags(pcVal, 0x1000));
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r1 = 0x1000;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-teqrr_pc") {
  const char source[] =
      "cmp r3, r3\n"
      "teq pc, r1\n";

  QBDI::rword pcVal = 0;
  bool seen = false;
  vm.addMnemonicCB("TEQrr", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     pcVal = ia->address + 8;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("TEQrr", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     checkFlags(gprState->cpsr,
                                logicFlags(pcVal ^ 0x1000u, true, false));
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r1 = 0x1000;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-tstrr_pc") {
  const char source[] =
      "cmp r3, r3\n"
      "tst pc, r1\n";

  QBDI::rword pcVal = 0;
  bool seen = false;
  vm.addMnemonicCB("TSTrr", QBDI::PREINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     const QBDI::InstAnalysis *ia =
                         vmi->getInstAnalysis(QBDI::ANALYSIS_INSTRUCTION);
                     pcVal = ia->address + 8;
                     return QBDI::VMAction::CONTINUE;
                   });
  vm.addMnemonicCB("TSTrr", QBDI::POSTINST,
                   [&](QBDI::VMInstanceRef vmi, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState) -> QBDI::VMAction {
                     checkFlags(gprState->cpsr,
                                logicFlags(pcVal & 0x1000u, true, false));
                     seen = true;
                     return QBDI::VMAction::CONTINUE;
                   });

  QBDI::GPRState *state = vm.getGPRState();
  state->r1 = 0x1000;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);

  CHECK(ran);
  CHECK(seen);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-stri12_pc_value") {
  static QBDI::rword valBuf = 0;
  const char source[] =
      "str pc, [r0, #4]\n"
      "bx lr\n";
  QBDI::rword codeAddr = genASM(source);

  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost = {
      {{(QBDI::rword)&valBuf, codeAddr + 8, 4, QBDI::MEMORY_WRITE,
        QBDI::MEMORY_NO_FLAGS}}};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STRi12", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRi12", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&valBuf - 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(valBuf == codeAddr + 8);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strrs_pc_base") {
  static QBDI::rword valBuf = 0;
  const char source[] =
      "str r1, [pc, r2]\n"
      "bx lr\n";
  QBDI::rword codeAddr = genASM(source);

  QBDI::rword v = 0x12345678;
  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost = {{
      {(QBDI::rword)&valBuf, v, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STRrs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRrs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r1 = v;
  state->r2 = (QBDI::rword)&valBuf - (codeAddr + 8);
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(valBuf == v);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-strrs_pc_value") {
  static QBDI::rword valBuf = 0;
  const char source[] =
      "str pc, [r0, r2]\n"
      "bx lr\n";
  QBDI::rword codeAddr = genASM(source);

  ExpectedMemoryAccesses expectedPre{};
  ExpectedMemoryAccesses expectedPost = {
      {{(QBDI::rword)&valBuf, codeAddr + 8, 4, QBDI::MEMORY_WRITE,
        QBDI::MEMORY_NO_FLAGS}}};

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("STRrs", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("STRrs", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::GPRState *state = vm.getGPRState();
  state->r0 = (QBDI::rword)&valBuf - 4;
  state->r2 = 4;
  vm.setGPRState(state);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
  CHECK(valBuf == codeAddr + 8);
}
