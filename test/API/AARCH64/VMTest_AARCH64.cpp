/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2025 Quarkslab
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
#include "VMTest_AARCH64.h"

#define MNEM_IMM_SHORT_VAL 66
#define MNEM_IMM_VAL 42424242
#define MNEM_IMM_SHORT_STRVAL "66"
#define MNEM_IMM_STRVAL "42424242"

const struct TestInst TestInsts[MNEM_COUNT] = {
    {4,
     4,
     true,
     QBDI::REGISTER_WRITE,
     {
         {QBDI::OPERAND_SEG, QBDI::OPERANDFLAG_NONE, 0, 8, 0, -1, "XZR",
          QBDI::REGISTER_WRITE},
         {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 3, "X3",
          QBDI::REGISTER_READ},
         {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_NONE, MNEM_IMM_SHORT_VAL, 8, 0,
          -1, nullptr, QBDI::REGISTER_UNUSED},
         {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_NONE, 0, 8, 0, -1, nullptr,
          QBDI::REGISTER_UNUSED},
     }}};

QBDI_NOINLINE QBDI::rword satanicFun(QBDI::rword arg0) {
  QBDI::rword volatile res = arg0 + 0x666;
  asm("cmp x3, #" MNEM_IMM_SHORT_STRVAL);
  return res;
}

// clang-format off
std::vector<uint8_t> VMTest_AARCH64_InvalidInstruction = {
  0x80, 0x0c, 0x80, 0xd2,     // movz     x0, #0x64
  0x21, 0x00, 0x01, 0xca,     // eor      x1, x1, x1
  0x21, 0x00, 0x00, 0x8b,     // add      x1, x1, x0
  0x00, 0x04, 0x00, 0xd1,     // sub      x0, x0, #1
  0x1f, 0x00, 0x00, 0xf1,     // cmp      x0, #0
  0xff, 0xff, 0xff, 0xff,     // invalid instruction
  0xaa, 0xab                  // unaligned instruction
};

std::vector<uint8_t> VMTest_AARCH64_BreakingInstruction = {
  0x80, 0x0c, 0x80, 0xd2,     // movz     x0, #0x64
  0x21, 0x00, 0x01, 0xca,     // eor      x1, x1, x1
  0x21, 0x00, 0x00, 0x8b,     // add      x1, x1, x0
  0x00, 0x04, 0x00, 0xd1,     // sub      x0, x0, #1
  0x1f, 0x00, 0x00, 0xf1,     // cmp      x0, #0
  0xc0, 0x03, 0x5f, 0xd6      // ret
};

std::vector<uint8_t> VMTest_AARCH64_SelfModifyingCode1 = {
  0xe0, 0xcb, 0x9a, 0xd2,     // movz x0, #0xd65f
  0x01, 0x78, 0x80, 0xd2,     // movz x1, #0x3c0
  0x21, 0x40, 0x00, 0x8b,     // add	x1, x1, x0, lsl #16
  0x40, 0x05, 0x80, 0xd2,     // mov	x0, #0x2a
  0x02, 0x00, 0x00, 0x10,     // adr  x2, #0x0
  0x41, 0x80, 0x00, 0xb8,     // stur w1, [x2, #8]
  0xff, 0xff, 0xff, 0xff      // invalid instruction, replaced by 'ret'
};

std::vector<uint8_t> VMTest_AARCH64_SelfModifyingCode2 = {
  0xe0, 0xcb, 0x9a, 0xd2,     // movz x0, #0xd65f
  0x01, 0x78, 0x80, 0xd2,     // movz x1, #0x3c0
  0x21, 0x40, 0x00, 0x8b,     // add	x1, x1, x0, lsl #16
  0x40, 0x05, 0x80, 0xd2,     // mov	x0, #0x2a
  0x02, 0x00, 0x00, 0x10,     // adr  x2, #0x0
  0x41, 0x80, 0x00, 0xb8,     // stur w1, [x2, #8]
  0xe0, 0x03, 0x1f, 0xd6      // br xzr  replaced by 'ret'
};
// clang-format on

std::unordered_map<std::string, SizedTestCode> TestCode = {
    {"VMTest-InvalidInstruction", {VMTest_AARCH64_InvalidInstruction, 0x10}},
    {"VMTest-BreakingInstruction", {VMTest_AARCH64_BreakingInstruction, 0x10}},
    {"VMTest-SelfModifyingCode1", {VMTest_AARCH64_SelfModifyingCode1}},
    {"VMTest-SelfModifyingCode2", {VMTest_AARCH64_SelfModifyingCode2}}};
