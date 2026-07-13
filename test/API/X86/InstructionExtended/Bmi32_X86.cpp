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

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-ADCX32rm") {
  if (!checkFeature("adx")) {
    return;
  }
  const char source[] = "adcxl 0x11(%ebx,%esi,4), %eax\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x00000005;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x00000005, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x00000005, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADCX32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADCX32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x0000000a;
  state->eflags &= ~0x901;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x00000005);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->eax & 0xffffffff) == 0x0000000f);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-ADOX32rm") {
  if (!checkFeature("adx")) {
    return;
  }
  const char source[] = "adoxl 0x11(%ebx,%esi,4), %eax\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x00000005;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x00000005, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x00000005, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ADOX32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ADOX32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x0000000a;
  state->eflags &= ~0x901;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x00000005);
  QBDI::GPRState *finalState = vm.getGPRState();
  CHECK((finalState->eax & 0xffffffff) == 0x0000000f);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-ANDN32rm") {
  if (!checkFeature("bmi")) {
    return;
  }
  const char source[] = "andnl 0x11(%ebx,%esi,4), %eax, %ecx\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x0000000f;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000f, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000f, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("ANDN32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("ANDN32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x000000f0;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000f);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-BEXTR32rm") {
  if (!checkFeature("bmi")) {
    return;
  }
  const char source[] = "bextrl %eax, 0x11(%ebx,%esi,4), %ecx\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x12345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x12345678, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x12345678, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BEXTR32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BEXTR32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x00000408;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x12345678);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-BEXTRI32mi") {
  if (!checkFeature("tbm")) {
    return;
  }
  const char source[] = "bextrl $0x0408, 0x11(%ebx,%esi,4), %eax\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x12345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x12345678, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x12345678, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BEXTRI32mi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BEXTRI32mi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x12345678);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-BLCFILL32rm") {
  if (!checkFeature("tbm")) {
    return;
  }
  const char source[] = "blcfilll 0x11(%ebx,%esi,4), %eax\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x0000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000c, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000c, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BLCFILL32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BLCFILL32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000c);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-BLCI32rm") {
  if (!checkFeature("tbm")) {
    return;
  }
  const char source[] = "blcil 0x11(%ebx,%esi,4), %eax\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x0000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000c, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000c, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BLCI32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BLCI32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000c);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-BLCIC32rm") {
  if (!checkFeature("tbm")) {
    return;
  }
  const char source[] = "blcicl 0x11(%ebx,%esi,4), %eax\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x0000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000c, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000c, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BLCIC32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BLCIC32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000c);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-BLCMSK32rm") {
  if (!checkFeature("tbm")) {
    return;
  }
  const char source[] = "blcmskl 0x11(%ebx,%esi,4), %eax\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x0000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000c, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000c, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BLCMSK32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BLCMSK32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000c);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-BLCS32rm") {
  if (!checkFeature("tbm")) {
    return;
  }
  const char source[] = "blcsl 0x11(%ebx,%esi,4), %eax\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x0000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000c, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000c, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BLCS32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BLCS32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000c);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-BLSFILL32rm") {
  if (!checkFeature("tbm")) {
    return;
  }
  const char source[] = "blsfilll 0x11(%ebx,%esi,4), %eax\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x0000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000c, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000c, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BLSFILL32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BLSFILL32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000c);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-BLSIC32rm") {
  if (!checkFeature("tbm")) {
    return;
  }
  const char source[] = "blsicl 0x11(%ebx,%esi,4), %eax\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x0000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000c, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000c, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BLSIC32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BLSIC32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000c);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-T1MSKC32rm") {
  if (!checkFeature("tbm")) {
    return;
  }
  const char source[] = "t1mskcl 0x11(%ebx,%esi,4), %eax\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x0000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000c, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000c, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("T1MSKC32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("T1MSKC32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000c);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-TZMSK32rm") {
  if (!checkFeature("tbm")) {
    return;
  }
  const char source[] = "tzmskl 0x11(%ebx,%esi,4), %eax\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x0000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000c, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000c, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("TZMSK32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("TZMSK32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000c);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-BLSI32rm") {
  if (!checkFeature("bmi")) {
    return;
  }
  const char source[] = "blsil 0x11(%ebx,%esi,4), %eax\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x0000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000c, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000c, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BLSI32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BLSI32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000c);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-BLSMSK32rm") {
  if (!checkFeature("bmi")) {
    return;
  }
  const char source[] = "blsmskl 0x11(%ebx,%esi,4), %eax\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x0000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000c, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000c, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BLSMSK32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BLSMSK32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000c);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-BLSR32rm") {
  if (!checkFeature("bmi")) {
    return;
  }
  const char source[] = "blsrl 0x11(%ebx,%esi,4), %eax\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x0000000c;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x0000000c, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x0000000c, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BLSR32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BLSR32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000c);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-BZHI32rm") {
  if (!checkFeature("bmi2")) {
    return;
  }
  const char source[] = "bzhil %eax, 0x11(%ebx,%esi,4), %ecx\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x12345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x12345678, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x12345678, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BZHI32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BZHI32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x00000008;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x12345678);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-MULX32rm") {
  if (!checkFeature("bmi2")) {
    return;
  }
  const char source[] = "mulxl 0x11(%ebx,%esi,4), %ecx, %eax\n";
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
  vm.addMnemonicCB("MULX32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("MULX32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->edx = 0x00000003;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x0000000a);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-PDEP32rm") {
  if (!checkFeature("bmi2")) {
    return;
  }
  const char source[] = "pdepl 0x11(%ebx,%esi,4), %eax, %ecx\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x12345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x12345678, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x12345678, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PDEP32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PDEP32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x0f0f0f0f;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x12345678);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-PEXT32rm") {
  if (!checkFeature("bmi2")) {
    return;
  }
  const char source[] = "pextl 0x11(%ebx,%esi,4), %eax, %ecx\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x12345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x12345678, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x12345678, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("PEXT32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("PEXT32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x0f0f0f0f;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x12345678);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-RORX32mi") {
  if (!checkFeature("bmi2")) {
    return;
  }
  const char source[] = "rorxl $0x5, 0x11(%ebx,%esi,4), %eax\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x12345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x12345678, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x12345678, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("RORX32mi", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("RORX32mi", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x12345678);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-SARX32rm") {
  if (!checkFeature("bmi2")) {
    return;
  }
  const char source[] = "sarxl %eax, 0x11(%ebx,%esi,4), %ecx\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x12345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x12345678, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x12345678, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SARX32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SARX32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x00000003;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x12345678);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-SHLX32rm") {
  if (!checkFeature("bmi2")) {
    return;
  }
  const char source[] = "shlxl %eax, 0x11(%ebx,%esi,4), %ecx\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x12345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x12345678, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x12345678, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHLX32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHLX32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x00000003;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x12345678);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-SHRX32rm") {
  if (!checkFeature("bmi2")) {
    return;
  }
  const char source[] = "shrxl %eax, 0x11(%ebx,%esi,4), %ecx\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x12345678;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x12345678, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x12345678, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("SHRX32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("SHRX32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  state->eax = 0x00000003;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x12345678);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-BSF32rm") {
  const char source[] = "bsfl 0x11(%ebx,%esi,4), %eax\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x00000008;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x00000008, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x00000008, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BSF32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BSF32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x00000008);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-BSR32rm") {
  const char source[] = "bsrl 0x11(%ebx,%esi,4), %eax\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x00000008;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x00000008, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x00000008, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("BSR32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("BSR32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x00000008);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-LZCNT32rm") {
  if (!checkFeature("lzcnt")) {
    return;
  }
  const char source[] = "lzcntl 0x11(%ebx,%esi,4), %eax\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x00000008;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x00000008, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x00000008, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("LZCNT32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("LZCNT32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x00000008);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-TZCNT32rm") {
  if (!checkFeature("bmi")) {
    return;
  }
  const char source[] = "tzcntl 0x11(%ebx,%esi,4), %eax\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x00000008;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x00000008, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x00000008, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("TZCNT32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("TZCNT32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x00000008);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}

TEST_CASE_METHOD(APITest, "InstructionExtendedTest_X86-POPCNT32rm") {
  if (!checkFeature("popcnt")) {
    return;
  }
  const char source[] = "popcntl 0x11(%ebx,%esi,4), %eax\n";
  uint8_t buffer[40] = {0};
  uint32_t *target = reinterpret_cast<uint32_t *>(&buffer[21]);
  *target = 0x00000008;
  QBDI::rword targetAddr = (QBDI::rword)target;
  ExpectedMemoryAccesses expectedPre = {{
      {targetAddr, 0x00000008, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  ExpectedMemoryAccesses expectedPost = {{
      {targetAddr, 0x00000008, 4, QBDI::MEMORY_READ, QBDI::MEMORY_NO_FLAGS},
  }};
  vm.recordMemoryAccess(QBDI::MEMORY_READ_WRITE);
  vm.addMnemonicCB("POPCNT32rm", QBDI::PREINST, checkAccess, &expectedPre);
  vm.addMnemonicCB("POPCNT32rm", QBDI::POSTINST, checkAccess, &expectedPost);
  QBDI::GPRState *state = vm.getGPRState();
  state->ebx = (QBDI::rword)&buffer[0];
  state->esi = 1;
  vm.setGPRState(state);
  QBDI::rword retval;
  bool ran = runOnASM(&retval, source);
  CHECK(ran);
  CHECK(*target == 0x00000008);
  CHECK(expectedPre.see);
  CHECK(expectedPost.see);
}
