/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2023 Quarkslab
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
#include "VMTest_ARM.h"

#define MNEM_IMM_SHORT_VAL 66
#define MNEM_IMM_VAL 42
#define MNEM_IMM_SHORT_STRVAL "66"
#define MNEM_IMM_STRVAL "42"

const struct TestInst TestInsts[MNEM_COUNT] = {
    {4,
     2,
     true,
     QBDI::REGISTER_WRITE,
     {
         {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 3, "R3",
          QBDI::REGISTER_READ},
         {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_NONE, MNEM_IMM_SHORT_VAL, 4, 0,
          -1, nullptr, QBDI::REGISTER_UNUSED},
     }}};

QBDI_NOINLINE QBDI::rword satanicFun(QBDI::rword arg0) {
  QBDI::rword volatile res = arg0 + 0x666;
  asm("cmp r3, #" MNEM_IMM_SHORT_STRVAL);
  return res;
}

// clang-format off
std::vector<uint8_t> VMTest_ARM_InvalidInstruction = {
  0x64, 0x00, 0xa0, 0xe3,     // mov      r0, #0x64
  0x01, 0x10, 0x21, 0xe0,     // eor      r1, r1, r1
  0x00, 0x10, 0x01, 0xe0,     // add      r1, r1, r0
  0x01 ,0x00, 0x40, 0xe2,     // sub      r0, r0, #1
  0x00, 0x00, 0x50, 0xe3,     // cmp      r0, #0
  0xff, 0xff, 0xff, 0xff,     // invalid instruction
  0xaa, 0xab                  // unaligned instruction
};

std::vector<uint8_t> VMTest_ARM_BreakingInstruction = {
  0x64, 0x00, 0xa0, 0xe3,     // mov      r0, #0x64
  0x01, 0x10, 0x21, 0xe0,     // eor      r1, r1, r1
  0x00, 0x10, 0x01, 0xe0,     // add      r1, r1, r0
  0x01 ,0x00, 0x40, 0xe2,     // sub      r0, r0, #1
  0x00, 0x00, 0x50, 0xe3,     // cmp      r0, #0
  0x1e, 0xff, 0x2f, 0xe1      // bx       lr
};

std::vector<uint8_t> VMTest_ARM_SelfModifyingCode1 = {
  0x00, 0x00, 0xa0, 0xe3,     // mov  r0, #0x0
  0x00, 0x00, 0x0f, 0xe5,     // str  r0, [pc, #0]
  0x2a, 0x00, 0xa0, 0xe3,     // mov  r0, #0x2a
  0xff, 0xff, 0xff, 0xff,     // invalid instruction, replaced by 'andeq r0, r0, r0'
  0x1e, 0xff, 0x2f, 0xe1      // bx   lr
};

std::vector<uint8_t> VMTest_ARM_SelfModifyingCode2 = {
  0x00, 0x00, 0xa0, 0xe3,     // mov  r0, #0x0
  0x00, 0x00, 0x0f, 0xe5,     // str  r0, [pc, #0]
  0x2a, 0x00, 0xa0, 0xe3,     // mov  r0, #0x2a
  0x01, 0x0c, 0x80, 0xe2,     // add  r0, r0, #256, replaced by 'andeq r0, r0, r0'
  0x1e, 0xff, 0x2f, 0xe1      // bx   lr
};
// clang-format on

std::unordered_map<std::string, SizedTestCode> TestCode = {
    {"VMTest-InvalidInstruction", {VMTest_ARM_InvalidInstruction, 0x10}},
    {"VMTest-BreakingInstruction", {VMTest_ARM_BreakingInstruction, 0x10}},
    {"VMTest-SelfModifyingCode1", {VMTest_ARM_SelfModifyingCode1}},
    {"VMTest-SelfModifyingCode2", {VMTest_ARM_SelfModifyingCode2}}};
