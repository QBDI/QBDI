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
#ifndef DARWIN_PROCESS_H
#define DARWIN_PROCESS_H

#include "process.h"

#include <stdint.h>
#include <mach/task.h>

static const uint8_t BRK_INS = 0xCC;

#if defined(QBDI_ARCH_X86)
#define THREAD_STATE_ID x86_THREAD_STATE32
#define THREAD_STATE_COUNT x86_THREAD_STATE32_COUNT
#define THREAD_STATE_FP_ID x86_FLOAT_STATE32
#define THREAD_STATE_FP_COUNT x86_FLOAT_STATE32_COUNT
#define THREAD_STATE x86_thread_state32_t
#define THREAD_STATE_FP x86_float_state32_t
#define THREAD_STATE_BP __ebp
#define THREAD_STATE_SP __esp
#define THREAD_STATE_PC __eip
#elif defined(QBDI_ARCH_X86_64)
#define THREAD_STATE_ID x86_THREAD_STATE64
#define THREAD_STATE_COUNT x86_THREAD_STATE64_COUNT
#define THREAD_STATE_FP_ID x86_FLOAT_STATE64
#define THREAD_STATE_FP_COUNT x86_FLOAT_STATE64_COUNT
#define THREAD_STATE x86_thread_state64_t
#define THREAD_STATE_FP x86_float_state64_t
#define THREAD_STATE_BP __rbp
#define THREAD_STATE_SP __rsp
#define THREAD_STATE_PC __rip
#endif

class DarwinProcess : public Process {

private:
    
    pid_t pid;
    task_t task;
    thread_act_t mainThread;
    void* brk_address;
    uint8_t  brk_value;
    bool suspended;
    QBDI::rword pageSize;
    QBDI::rword prot_page;
    bool prot_rx;
    int kq;

    void suspend();

    void resume();

    void makeRW(void* address);

    void makeRX();

public:

    DarwinProcess(pid_t process);

    pid_t getPID() { return pid; }

    void setBreakpoint(void* address);

    void unsetBreakpoint();

    void continueExecution();

    int waitForStatus();

    void getProcessGPR(QBDI::GPRState *gprState);
    
    void getProcessFPR(QBDI::FPRState *fprState);
};

void threadStateToGPRState(THREAD_STATE* ts, QBDI::GPRState* gprState);

void floatStateToFPRState(THREAD_STATE_FP* fs, QBDI::FPRState* fprState);

#endif // DARWIN_PROCESS_H
