/*
 * This file is part of QBDI.
 *
 * Copyright 2017 - 2024 Quarkslab
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

#ifndef LINUX_ARM_H
#define LINUX_ARM_H

#include <unistd.h>

#include <QBDI.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include "linux_process.h"

// Let's have fun with undocumented ptrace interfaces!
// From gdb/arm-linux-nat.c
#ifndef PTRACE_GETVFPREGS
#define PTRACE_GETVFPREGS (__ptrace_request)27
#define PTRACE_SETVFPREGS (__ptrace_request)28
#endif

#define SIGBRK SIGTRAP

// From gdb/aarch32-linux-nat.h
#define VFP_REGS_SIZE (32 * 8 + 4)

typedef struct user_regs GPR_STRUCT;
typedef struct __attribute__((__packed__)) {
  uint64_t d[QBDI_NUM_FPR];
  uint64_t __reserved[32 - QBDI_NUM_FPR];
  uint32_t FPSCR;
} FPR_STRUCT;

#ifdef __cplusplus
static_assert(sizeof(float) == 4, "Bad size of float");
static_assert(sizeof(double) == 8, "Bad size of double");
static_assert(sizeof(FPR_STRUCT) == VFP_REGS_SIZE, "Bad size of FPR_STRUCT");
#endif

void userToGPRState(const GPR_STRUCT *user, QBDI::GPRState *gprState);
void userToFPRState(const FPR_STRUCT *user, QBDI::FPRState *fprState);

#endif // LINUX_ARM_H
