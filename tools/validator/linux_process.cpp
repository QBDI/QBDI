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
#include "linux_process.h"
#include "validator.h"

#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>

#include "Utility/LogSys.h"

static void userToGPRState(const GPR_STRUCT* user, QBDI::GPRState* gprState) {
    #if defined(QBDI_ARCH_X86_64)
    gprState->rax = user->rax;
    gprState->rbx = user->rbx;
    gprState->rcx = user->rcx;
    gprState->rdx = user->rdx;
    gprState->rsi = user->rsi;
    gprState->rdi = user->rdi;
    gprState->rbp = user->rbp;
    gprState->rsp = user->rsp;
    gprState->r8  = user->r8;
    gprState->r9  = user->r9;
    gprState->r10 = user->r10;
    gprState->r11 = user->r11;
    gprState->r12 = user->r12;
    gprState->r13 = user->r13;
    gprState->r14 = user->r14;
    gprState->r15 = user->r15;
    gprState->rip = user->rip;
    gprState->eflags = user->eflags;
    #elif defined(QBDI_ARCH_X86)
    gprState->eax = user->eax;
    gprState->ebx = user->ebx;
    gprState->ecx = user->ecx;
    gprState->edx = user->edx;
    gprState->esi = user->esi;
    gprState->edi = user->edi;
    gprState->ebp = user->ebp;
    gprState->esp = user->esp;
    gprState->eip = user->eip;
    gprState->eflags = user->eflags;
    #elif defined(QBDI_ARCH_ARM)
    gprState->r0   = user->uregs[0];
    gprState->r1   = user->uregs[1];
    gprState->r2   = user->uregs[2];
    gprState->r3   = user->uregs[3];
    gprState->r4   = user->uregs[4];
    gprState->r5   = user->uregs[5];
    gprState->r6   = user->uregs[6];
    gprState->r7   = user->uregs[7];
    gprState->r8   = user->uregs[8];
    gprState->r9   = user->uregs[9];
    gprState->r10  = user->uregs[10];
    gprState->fp   = user->uregs[11];
    gprState->r12  = user->uregs[12];
    gprState->sp   = user->uregs[13];
    gprState->lr   = user->uregs[14];
    gprState->pc   = user->uregs[15];
    gprState->cpsr = user->uregs[16];
    #endif
}

void userToFPRState(const FPR_STRUCT* user, QBDI::FPRState* fprState) {
    #if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
    memcpy(&fprState->stmm0, &user->st_space[0], 10);
    memcpy(&fprState->stmm1, &user->st_space[4], 10);
    memcpy(&fprState->stmm2, &user->st_space[8], 10);
    memcpy(&fprState->stmm3, &user->st_space[12], 10);
    memcpy(&fprState->stmm4, &user->st_space[16], 10);
    memcpy(&fprState->stmm5, &user->st_space[20], 10);
    memcpy(&fprState->stmm6, &user->st_space[24], 10);
    memcpy(&fprState->stmm7, &user->st_space[28], 10);
    memcpy(&fprState->xmm0, &user->xmm_space[0], 16);
    memcpy(&fprState->xmm1, &user->xmm_space[4], 16);
    memcpy(&fprState->xmm2, &user->xmm_space[8], 16);
    memcpy(&fprState->xmm3, &user->xmm_space[12], 16);
    memcpy(&fprState->xmm4, &user->xmm_space[16], 16);
    memcpy(&fprState->xmm5, &user->xmm_space[20], 16);
    memcpy(&fprState->xmm6, &user->xmm_space[24], 16);
    memcpy(&fprState->xmm7, &user->xmm_space[28], 16);
    #if defined(QBDI_ARCH_X86_64)
    memcpy(&fprState->xmm8, &user->xmm_space[32], 16);
    memcpy(&fprState->xmm9, &user->xmm_space[36], 16);
    memcpy(&fprState->xmm10, &user->xmm_space[40], 16);
    memcpy(&fprState->xmm11, &user->xmm_space[44], 16);
    memcpy(&fprState->xmm12, &user->xmm_space[48], 16);
    memcpy(&fprState->xmm13, &user->xmm_space[52], 16);
    memcpy(&fprState->xmm14, &user->xmm_space[56], 16);
    memcpy(&fprState->xmm15, &user->xmm_space[60], 16);
    fprState->ftw = user->ftw;
    fprState->mxcsrmask = user->mxcr_mask;
    #else
    fprState->ftw = user->twd;
    fprState->mxcsrmask = 0xffff;
    #endif
    fprState->fop = user->fop;
    fprState->rfcw =  user->cwd;
    fprState->rfsw = user->swd;
    fprState->mxcsr = user->mxcsr;
    #elif defined(QBDI_ARCH_ARM)
    // According to gdb/aarch32-linux-nat.c
    // Copy d0 - d16
    for(uint32_t i = 0; i < QBDI_NUM_FPR; i++) {
        memcpy(&fprState->s[i], user + 4 * i, 4);
    }
    // Copy fpscr
    // memcpy(&fprState->fpscr, user + 8 * 32, 4);
    #endif
}

void LinuxProcess::setBreakpoint(void *address) {
    this->brk_address = address;
    this->brk_value = ptrace(PTRACE_PEEKDATA, this->pid, address, NULL);
    if (ptrace(PTRACE_POKEDATA, this->pid, address, BRK_INS | (this->brk_value & (~BRK_MASK))) == -1) {
        LogError("LinuxProcess::setBreakpoint", "Failed to set breakpoint: %s", strerror(errno));
        exit(VALIDATOR_ERR_UNEXPECTED_API_FAILURE);
    }
}

void LinuxProcess::unsetBreakpoint() {
    if (ptrace(PTRACE_POKEDATA, this->pid, this->brk_address, this->brk_value) == -1) {
        LogError("LinuxProcess::unsetBreakpoint", "Failed to unset breakpoint: %s", strerror(errno));
        exit(VALIDATOR_ERR_UNEXPECTED_API_FAILURE);
    }
}

void LinuxProcess::continueExecution() {
    ptrace(PTRACE_CONT, this->pid, NULL, NULL);
}

int LinuxProcess::waitForStatus() {
    int status = 0;
    waitpid(this->pid, &status, 0);
    #if defined(QBDI_ARCH_X86_64) || defined(QBDI_ARCH_X86)
    if(WSTOPSIG(status) == SIGBRK) {
        GPR_STRUCT user;
        ptrace(PTRACE_GETREGS, this->pid, NULL, &user);
#if defined(QBDI_ARCH_X86_64)
        user.rip -= 1;
#elif defined(QBDI_ARCH_X86)
        user.eip -= 1;
#endif
        ptrace(PTRACE_SETREGS, this->pid, NULL, &user);
    }
    #endif
    return status;
}


void LinuxProcess::getProcessGPR(QBDI::GPRState *gprState) {
    GPR_STRUCT user;
    if (ptrace(PTRACE_GETREGS, this->pid, NULL, &user) == -1) {
        LogError("LinuxProcess::getProcessGPR", "Failed to get GPR state: %s", strerror(errno));
        exit(VALIDATOR_ERR_UNEXPECTED_API_FAILURE);
    }
    userToGPRState(&user, gprState);
}

void LinuxProcess::getProcessFPR(QBDI::FPRState *fprState) {
    #if defined(QBDI_ARCH_ARM)
    // Let's have fun with undocumented ptrace interfaces!
    // From gdb/arm-linux-nat.c
    #ifndef PTRACE_GETVFPREGS
    #define PTRACE_GETVFPREGS (__ptrace_request)27
    #define PTRACE_SETVFPREGS (__ptrace_request)28
    #endif
    // From gdb/aarch32-linux-nat.h
    #define VFP_REGS_SIZE (32 * 8 + 4)
    uint8_t user[VFP_REGS_SIZE];
    if (ptrace(PTRACE_GETVFPREGS, this->pid, NULL, user) == -1) {
        LogError("LinuxProcess::getProcessFPR", "Failed to get FPR state: %s", strerror(errno));
        exit(VALIDATOR_ERR_UNEXPECTED_API_FAILURE);
    }
    userToFPRState(user, fprState);
    #else
    FPR_STRUCT user;
    #if defined(QBDI_ARCH_X86)
    if (ptrace(PTRACE_GETFPXREGS, this->pid, NULL, &user) == -1)
    #else
    if (ptrace(PTRACE_GETFPREGS, this->pid, NULL, &user) == -1)
    #endif
    {
        LogError("LinuxProcess::getProcessFPR", "Failed to get FPR state: %s", strerror(errno));
        exit(VALIDATOR_ERR_UNEXPECTED_API_FAILURE);
    }
    userToFPRState(&user, fprState);
    #endif
}

bool hasExited(int status) {
   return WIFEXITED(status) > 0;
}

bool hasStopped(int status) {
    return WSTOPSIG(status) == SIGBRK || WSTOPSIG(status) == SIGSTOP;
}

bool hasCrashed(int status) {
    return hasExited(status) == false && hasStopped(status) == false;
}
