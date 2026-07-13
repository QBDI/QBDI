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

#include "MemAccessTestUtils_X86.h"

using QBDITestBatch2::checkAccess;
using QBDITestBatch2::checkFeature;
using QBDITestBatch2::ExpectedMemoryAccess;
using QBDITestBatch2::ExpectedMemoryAccesses;

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-ADD_F32m") {
  const char source[] =
      "fld1\n"
      "fadds 0x11(%ebx,%esi,4)\n"
      "fstpl (%edx)\n";
  uint8_t buffer[40] = {0};
  double dummy = 0;
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x3f800000;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x3f800000, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x3f800000, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADD_F32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADD_F32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->edx = (QBDI::rword)&dummy;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-SUB_F32m") {
  const char source[] =
      "fld1\n"
      "fsubs 0x11(%ebx,%esi,4)\n"
      "fstpl (%edx)\n";
  uint8_t buffer[40] = {0};
  double dummy = 0;
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x3f800000;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x3f800000, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x3f800000, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SUB_F32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SUB_F32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->edx = (QBDI::rword)&dummy;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-SUBR_F32m") {
  const char source[] =
      "fld1\n"
      "fsubrs 0x11(%ebx,%esi,4)\n"
      "fstpl (%edx)\n";
  uint8_t buffer[40] = {0};
  double dummy = 0;
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x3f800000;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x3f800000, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x3f800000, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SUBR_F32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SUBR_F32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->edx = (QBDI::rword)&dummy;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-MUL_F32m") {
  const char source[] =
      "fld1\n"
      "fmuls 0x11(%ebx,%esi,4)\n"
      "fstpl (%edx)\n";
  uint8_t buffer[40] = {0};
  double dummy = 0;
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x3f800000;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x3f800000, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x3f800000, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MUL_F32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MUL_F32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->edx = (QBDI::rword)&dummy;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-DIV_F32m") {
  const char source[] =
      "fld1\n"
      "fdivs 0x11(%ebx,%esi,4)\n"
      "fstpl (%edx)\n";
  uint8_t buffer[40] = {0};
  double dummy = 0;
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x3f800000;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x3f800000, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x3f800000, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("DIV_F32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("DIV_F32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->edx = (QBDI::rword)&dummy;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-DIVR_F32m") {
  const char source[] =
      "fld1\n"
      "fdivrs 0x11(%ebx,%esi,4)\n"
      "fstpl (%edx)\n";
  uint8_t buffer[40] = {0};
  double dummy = 0;
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x3f800000;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x3f800000, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x3f800000, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("DIVR_F32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("DIVR_F32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->edx = (QBDI::rword)&dummy;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-ADD_FI32m") {
  const char source[] =
      "fld1\n"
      "fiaddl 0x11(%ebx,%esi,4)\n"
      "fstpl (%edx)\n";
  uint8_t buffer[40] = {0};
  double dummy = 0;
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x0000000a;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000a, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000a, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADD_FI32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADD_FI32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->edx = (QBDI::rword)&dummy;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-SUB_FI32m") {
  const char source[] =
      "fld1\n"
      "fisubl 0x11(%ebx,%esi,4)\n"
      "fstpl (%edx)\n";
  uint8_t buffer[40] = {0};
  double dummy = 0;
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x0000000a;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000a, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000a, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SUB_FI32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SUB_FI32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->edx = (QBDI::rword)&dummy;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-SUBR_FI32m") {
  const char source[] =
      "fld1\n"
      "fisubrl 0x11(%ebx,%esi,4)\n"
      "fstpl (%edx)\n";
  uint8_t buffer[40] = {0};
  double dummy = 0;
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x0000000a;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000a, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000a, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SUBR_FI32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SUBR_FI32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->edx = (QBDI::rword)&dummy;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-MUL_FI32m") {
  const char source[] =
      "fld1\n"
      "fimull 0x11(%ebx,%esi,4)\n"
      "fstpl (%edx)\n";
  uint8_t buffer[40] = {0};
  double dummy = 0;
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x0000000a;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000a, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000a, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("MUL_FI32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MUL_FI32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->edx = (QBDI::rword)&dummy;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-DIV_FI32m") {
  const char source[] =
      "fld1\n"
      "fidivl 0x11(%ebx,%esi,4)\n"
      "fstpl (%edx)\n";
  uint8_t buffer[40] = {0};
  double dummy = 0;
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x0000000a;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000a, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000a, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("DIV_FI32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("DIV_FI32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->edx = (QBDI::rword)&dummy;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-DIVR_FI32m") {
  const char source[] =
      "fld1\n"
      "fidivrl 0x11(%ebx,%esi,4)\n"
      "fstpl (%edx)\n";
  uint8_t buffer[40] = {0};
  double dummy = 0;
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x0000000a;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000a, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000a, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("DIVR_FI32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("DIVR_FI32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->edx = (QBDI::rword)&dummy;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-FCOM32m") {
  const char source[] =
      "fld1\n"
      "fcoms 0x11(%ebx,%esi,4)\n"
      "fstpl (%edx)\n";
  uint8_t buffer[40] = {0};
  double dummy = 0;
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x3f800000;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x3f800000, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x3f800000, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("FCOM32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("FCOM32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->edx = (QBDI::rword)&dummy;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-FCOMP32m") {
  const char source[] =
      "fld1\n"
      "fcomps 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x3f800000;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x3f800000, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x3f800000, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("FCOMP32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("FCOMP32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-FICOM32m") {
  const char source[] =
      "fld1\n"
      "ficoml 0x11(%ebx,%esi,4)\n"
      "fstpl (%edx)\n";
  uint8_t buffer[40] = {0};
  double dummy = 0;
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x0000000a;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000a, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000a, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("FICOM32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("FICOM32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->edx = (QBDI::rword)&dummy;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-FICOMP32m") {
  const char source[] =
      "fld1\n"
      "ficompl 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x0000000a;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000a, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000a, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("FICOMP32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("FICOMP32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-ILD_F32m") {
  const char source[] =
      "fildl 0x11(%ebx,%esi,4)\n"
      "fstpl (%edx)\n";
  uint8_t buffer[40] = {0};
  double dummy = 0;
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x0000000a;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000a, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000a, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ILD_F32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ILD_F32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->edx = (QBDI::rword)&dummy;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-LD_F32m") {
  const char source[] =
      "flds 0x11(%ebx,%esi,4)\n"
      "fstpl (%edx)\n";
  uint8_t buffer[40] = {0};
  double dummy = 0;
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x3f800000;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x3f800000, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x3f800000, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LD_F32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LD_F32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->edx = (QBDI::rword)&dummy;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-IST_F32m") {
  const char source[] =
      "fld1\n"
      "fistl 0x11(%ebx,%esi,4)\n"
      "fstpl (%edx)\n";
  uint8_t buffer[40] = {0};
  double dummy = 0;
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x00000001, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IST_F32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IST_F32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->edx = (QBDI::rword)&dummy;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x00000001);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-IST_FP32m") {
  const char source[] =
      "fld1\n"
      "fistpl 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x00000001, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("IST_FP32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("IST_FP32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x00000001);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-ISTT_FP32m") {
  const char source[] =
      "fld1\n"
      "fisttpl 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x00000001, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ISTT_FP32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ISTT_FP32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x00000001);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-ST_F32m") {
  const char source[] =
      "fld1\n"
      "fsts 0x11(%ebx,%esi,4)\n"
      "fstpl (%edx)\n";
  uint8_t buffer[40] = {0};
  double dummy = 0;
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x3f800000, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ST_F32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST_F32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->edx = (QBDI::rword)&dummy;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x3f800000);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-ST_FP32m") {
  const char source[] =
      "fld1\n"
      "fstps 0x11(%ebx,%esi,4)\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{}};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x3f800000, 4, QBDI::MEMORY_WRITE, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ST_FP32m", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ST_FP32m", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x3f800000);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}
