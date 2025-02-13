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
#include "VMTest_X86.h"

#define MNEM_IMM_SHORT_VAL 66
#define MNEM_IMM_VAL 42424242
#define MNEM_IMM_SHORT_STRVAL "66"
#define MNEM_IMM_STRVAL "42424242"

const struct TestInst TestInsts[MNEM_COUNT] = {
    {3,
     2,
     true,
     QBDI::REGISTER_WRITE,
     {
         {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 1, 8, 3, "DH",
          QBDI::REGISTER_READ},
         {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_NONE, MNEM_IMM_SHORT_VAL, 1, 0,
          -1, nullptr, QBDI::REGISTER_UNUSED},
     }},
    {3,
     2,
     true,
     QBDI::REGISTER_WRITE,
     {
         {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 2, 0, 0, "AX",
          QBDI::REGISTER_READ},
         {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 2, 0, 1, "BX",
          QBDI::REGISTER_READ},
     }},
    {5,
     2,
     true,
     QBDI::REGISTER_WRITE,
     {
         {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_NONE, MNEM_IMM_VAL, 4, 0, -1,
          nullptr, QBDI::REGISTER_UNUSED},
         {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 0, "EAX",
          QBDI::REGISTER_READ},
     }},
    {1,
     5,
     false,
     QBDI::REGISTER_READ | QBDI::REGISTER_WRITE,
     {
         {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, sizeof(QBDI::rword), 0,
          5, QBDI::GPR_NAMES[5], QBDI::REGISTER_READ},
         {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, sizeof(QBDI::rword), 0,
          4, QBDI::GPR_NAMES[4], QBDI::REGISTER_READ},
         {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_NONE, 0, 0, 0, -1, NULL,
          QBDI::REGISTER_UNUSED},
         {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 5, "EDI",
          QBDI::REGISTER_READ_WRITE},
         {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 4, 0, 4, "ESI",
          QBDI::REGISTER_READ_WRITE},
     }},
    {4,
     6,
     true,
     QBDI::REGISTER_WRITE,
     {
         {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, sizeof(QBDI::rword), 0,
          0, QBDI::GPR_NAMES[0], QBDI::REGISTER_READ},
         {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, sizeof(QBDI::rword), 0,
          4, QBDI::GPR_NAMES[4], QBDI::REGISTER_READ},
         {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 1, sizeof(QBDI::rword), 0,
          -1, NULL, QBDI::REGISTER_UNUSED},
         {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_ADDR, 0, sizeof(QBDI::rword), 0,
          5, QBDI::GPR_NAMES[5], QBDI::REGISTER_READ},
         {QBDI::OPERAND_IMM, QBDI::OPERANDFLAG_ADDR, 3, sizeof(QBDI::rword), 0,
          -1, NULL, QBDI::REGISTER_UNUSED},
         {QBDI::OPERAND_INVALID, QBDI::OPERANDFLAG_NONE, 0, 0, 0, -1, NULL,
          QBDI::REGISTER_UNUSED},
     }},
};

QBDI_NOINLINE QBDI::rword satanicFun(QBDI::rword arg0) {
  QBDI::rword volatile res = arg0 + 0x666;
  QBDI::rword p = 0x42;
  QBDI::rword v[2] = {0x67, 0x45};
#ifndef QBDI_PLATFORM_WINDOWS
  asm("cmp $" MNEM_IMM_SHORT_STRVAL ", %%dh" ::: "dh");
  asm("cmp %%bx, %%ax" ::: "bx", "ax");
  asm("cmp $" MNEM_IMM_STRVAL ", %%eax" ::: "eax"); // explicit register
  asm("mov %0, %%edi; mov %1, %%esi; cmpsb %%es:(%%edi), (%%esi)" ::"r"(&p),
      "r"(&p)
      : "edi", "esi");
  asm("mov %0, %%edi; mov $1, %%esi; cmp 0x3(%%esi,%%edi,1), %%eax" ::"r"(v)
      : "edi", "esi", "eax");
#endif
  return res;
}

// clang-format off
std::vector<uint8_t> VMTest_X86_InvalidInstruction = {
  0xb9, 0x64, 0x00, 0x00, 0x00,   // 00: mov    ecx,0x64
  0x31, 0xc0,                     // 05: xor    eax,eax
  0x31, 0xdb,                     // 07: xor    ebx,ebx
  0x01, 0xc8,                     // 09: add    eax,ecx
  0x83, 0xe9, 0x01,               // 0b: sub    ecx,0x1
  0x83, 0xf9, 0x00,               // 0e: cmp    ecx,0x0
  0x01, 0xc3,                     // 11: add    ebx,eax
  0xff,                           // 13: invalid instruction
};

std::vector<uint8_t> VMTest_X86_BreakingInstruction = {
  0xb9, 0x64, 0x00, 0x00, 0x00,   // 00: mov    ecx,0x64
  0x31, 0xc0,                     // 05: xor    eax,eax
  0x31, 0xdb,                     // 07: xor    ebx,ebx
  0x01, 0xc8,                     // 09: add    eax,ecx
  0x83, 0xe9, 0x01,               // 0b: sub    ecx,0x1
  0x83, 0xf9, 0x00,               // 0e: cmp    ecx,0x0
  0x01, 0xc3,                     // 11: add    ebx,eax
  0xc3,                           // 13: ret
};

std::vector<uint8_t> VMTest_X86_SelfModifyingCode1 = {
  0xe8, 0x00, 0x00, 0x00, 0x00,           // 00: call   $+5
  0x58,                                   // 05: pop    eax
  0xc6, 0x40, 0x14, 0xc3,                 // 06: mov    BYTE PTR [eax+0x14],0xc3
  0xb9, 0x2a, 0x00, 0x00, 0x00,           // 0a: mov    ecx,0x2a
  0x66, 0xc7, 0x40, 0x12, 0x01, 0xc8,     // 0f: mov    WORD PTR [eax+0x12],0xc801
  0x31, 0xc0,                             // 15: xor    eax, eax
  0x0f, 0xff,                             // 17: invalid instruction, replaced by 'add    eax,ecx'
  0x55,                                   // 19: invalid instruction, replaced by 'ret'
};

std::vector<uint8_t> VMTest_X86_SelfModifyingCode2 = {
  0xe8, 0x00, 0x00, 0x00, 0x00,           // 00: call   $+5
  0x58,                                   // 05: pop    eax
  0xc6, 0x40, 0x12, 0xc3,                 // 06: mov    BYTE PTR [eax+0x12],0xc3
  0xb9, 0x2a, 0x00, 0x00, 0x00,           // 0a: mov    ecx,0x2a
  0x66, 0xc7, 0x40, 0x10, 0x89, 0xc8,     // 0f: mov    WORD PTR [eax+0x10],0xc889
  0x31, 0xc0,                             // 15: xor    eax,eax   , 15: replaced by 'mov    eax,ecx'
  0xff, 0xe0,                             // 17: jmp    eax       , 17: replaced by 'ret'
};
// clang-format on

std::unordered_map<std::string, SizedTestCode> TestCode = {
    {"VMTest-InvalidInstruction", {VMTest_X86_InvalidInstruction, 0x11}},
    {"VMTest-BreakingInstruction", {VMTest_X86_BreakingInstruction, 0x0b}},
    {"VMTest-SelfModifyingCode1", {VMTest_X86_SelfModifyingCode1}},
    {"VMTest-SelfModifyingCode2", {VMTest_X86_SelfModifyingCode2}}};
