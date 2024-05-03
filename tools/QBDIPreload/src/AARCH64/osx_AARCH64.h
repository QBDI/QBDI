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
#ifndef QBDIPRELOAD_X86_H
#define QBDIPRELOAD_X86_H

#include <QBDI.h>

#include <mach/thread_status.h>

static const uint32_t BRK_INS = 0xD4200000; // brk 3ff;

#define MACH_HEADER mach_header_64
#define MACH_MAGIC MH_MAGIC_64
#define MACH_SEG segment_command_64
#define MACH_SEG_CMD LC_SEGMENT_64

#define THREAD_STATE arm_thread_state64_t
#define THREAD_STATE_COUNT ARM_THREAD_STATE64_COUNT
#define THREAD_STATE_FP arm_neon_state64_t
#define THREAD_STATE_FP_COUNT ARM_NEON_STATE64_COUNT
#define THREAD_STATE_FP_ID ARM_NEON_STATE64
#define THREAD_STATE_ID ARM_THREAD_STATE64

void qbdipreload_threadCtxToGPRState(const void *gprCtx, GPRState *gprState);
void qbdipreload_floatCtxToFPRState(const void *fprCtx, FPRState *fprState);

static inline rword getPC(THREAD_STATE *state) {
  return __darwin_arm_thread_state64_get_pc(*state);
}

static inline void setPC(THREAD_STATE *state, rword address) {
  __darwin_arm_thread_state64_set_pc_fptr(*state, address);
}

static inline void prepareStack(void *newStack, size_t sizeStack,
                                THREAD_STATE *state) {
  __darwin_arm_thread_state64_set_sp(*state, (rword)newStack + sizeStack - 16);
  __darwin_arm_thread_state64_set_fp(*state, (rword)newStack + sizeStack - 16);
}

static inline void fixSignalPC(THREAD_STATE *state) {}

#endif // QBDIPRELOAD_X86_H
