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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-t2ldrpci_misaligned") {
  const char source[] =
      "nop\n"
      "ldr.w r0, [pc, #4]\n"
      "bx lr\n"
      ".word 0x11223344\n";
  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb);

  ExpectedMemoryAccesses expectedPre = {{
      {(codeAddr & ~(QBDI::rword)1) + 8, 0x11223344, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRpci", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRpci", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == 0x11223344);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-t2ldrbpci_misaligned") {
  const char source[] =
      "nop\n"
      "ldrb r0, [pc, #4]\n"
      "bx lr\n"
      ".byte 0x92\n";
  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb);

  ExpectedMemoryAccesses expectedPre = {{
      {(codeAddr & ~(QBDI::rword)1) + 8, 0x92, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRBpci", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRBpci", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == 0x92);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-t2ldrhpci_misaligned") {
  const char source[] =
      "nop\n"
      "ldrh r0, [pc, #4]\n"
      "bx lr\n"
      ".hword 0x9234\n";
  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb);

  ExpectedMemoryAccesses expectedPre = {{
      {(codeAddr & ~(QBDI::rword)1) + 8, 0x9234, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRHpci", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRHpci", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == 0x9234);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-t2ldrsbpci_misaligned") {
  const char source[] =
      "nop\n"
      "ldrsb r0, [pc, #4]\n"
      "bx lr\n"
      ".byte 0x92\n";
  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb);

  ExpectedMemoryAccesses expectedPre = {{
      {(codeAddr & ~(QBDI::rword)1) + 8, 0x92, 1, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRSBpci", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRSBpci", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)(int32_t)(int8_t)0x92);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-t2ldrshpci_misaligned") {
  const char source[] =
      "nop\n"
      "ldrsh r0, [pc, #4]\n"
      "bx lr\n"
      ".hword 0x9234\n";
  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb);

  ExpectedMemoryAccesses expectedPre = {{
      {(codeAddr & ~(QBDI::rword)1) + 8, 0x9234, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRSHpci", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRSHpci", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == (QBDI::rword)(int32_t)(int16_t)0x9234);
}

TEST_CASE_METHOD(APITest,
                 "InstructionExtendedTest_ARM-t2ldrdi8_pc_misaligned") {
  const char source[] =
      "nop\n"
      "ldrd r0, r1, [pc, #4]\n"
      "bx lr\n"
      ".word 0x11223344\n"
      ".word 0x55667788\n";
  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb);

  ExpectedMemoryAccesses expectedPre = {{
      {(codeAddr & ~(QBDI::rword)1) + 8, 0x11223344, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(codeAddr & ~(QBDI::rword)1) + 12, 0x55667788, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("t2LDRDi8", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("t2LDRDi8", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK(finalState->r0 == 0x11223344);
  CHECK(finalState->r1 == 0x55667788);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vldrd_pc_misaligned") {
  if (!checkFeature("vfp2")) {
    return;
  }
  const char source[] =
      "nop\n"
      "vldr d0, [pc, #4]\n"
      "bx lr\n"
      ".word 0x11223344\n"
      ".word 0x55667788\n";
  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb);

  ExpectedMemoryAccesses expectedPre = {{
      {(codeAddr & ~(QBDI::rword)1) + 8, 0x11223344, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
      {(codeAddr & ~(QBDI::rword)1) + 12, 0x55667788, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VLDRD", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLDRD", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  CHECK(fpr->vreg.q[0][0] == 0x44);
  CHECK(fpr->vreg.q[0][4] == 0x88);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vldrs_pc_misaligned") {
  if (!checkFeature("vfp2")) {
    return;
  }
  const char source[] =
      "nop\n"
      "vldr s0, [pc, #4]\n"
      "bx lr\n"
      ".word 0x11223344\n";
  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb);

  ExpectedMemoryAccesses expectedPre = {{
      {(codeAddr & ~(QBDI::rword)1) + 8, 0x11223344, 4, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VLDRS", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLDRS", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  CHECK(fpr->vreg.q[0][0] == 0x44);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_ARM-vldrh_pc_misaligned") {
  if (!checkFeature("fullfp16")) {
    return;
  }
  const char source[] =
      "nop\n"
      "vldr.16 s0, [pc, #4]\n"
      "bx lr\n"
      ".hword 0x9234\n";
  QBDI::rword codeAddr = genASM(source, QBDI::CPUMode::Thumb, {"fullfp16"});

  ExpectedMemoryAccesses expectedPre = {{
      {(codeAddr & ~(QBDI::rword)1) + 8, 0x9234, 2, QBDI::MEMORY_READ,
       QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = expectedPre;

  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("VLDRH", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("VLDRH", QBDI::POSTINST, checkAccess, &expectedPost);

  QBDI::rword retval;
  bool ran = vm.call(&retval, codeAddr, {});

  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);

  QBDI::FPRState *fpr = vm.getFPRState();
  CHECK(fpr->vreg.q[0][0] == 0x34);
  CHECK(fpr->vreg.q[0][1] == 0x92);
}
