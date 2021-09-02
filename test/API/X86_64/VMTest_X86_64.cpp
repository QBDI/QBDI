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
#include "VMTest_X86_64.h"

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
         {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 0, "RAX",
          QBDI::REGISTER_READ},
         {QBDI::OPERAND_GPR, QBDI::OPERANDFLAG_NONE, 0, 8, 0, 1, "RBX",
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
    {5,
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
  asm("cmp %%rbx, %%rax" ::: "rbx", "rax");
  asm("cmp $" MNEM_IMM_STRVAL ", %%eax" ::: "eax"); // explicit register
  asm("movq %0, %%rdi; movq %1, %%rsi; cmpsb %%es:(%%rdi), (%%rsi)" ::"r"(&p),
      "r"(&p)
      : "rdi", "rsi");
  asm("mov %0, %%rdi; mov $1, %%rsi; cmp 0x3(%%rsi,%%rdi,1), %%rax" ::"r"(v)
      : "rdi", "rsi", "rax");
#endif
  return res;
}