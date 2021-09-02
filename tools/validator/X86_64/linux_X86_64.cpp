
#include "linux_X86_64.h"
#include <stdlib.h>
#include <sys/ptrace.h>
#include "validator.h"

#include "Utility/LogSys.h"

void userToGPRState(const GPR_STRUCT *user, QBDI::GPRState *gprState) {
  gprState->rax = user->rax;
  gprState->rbx = user->rbx;
  gprState->rcx = user->rcx;
  gprState->rdx = user->rdx;
  gprState->rsi = user->rsi;
  gprState->rdi = user->rdi;
  gprState->rbp = user->rbp;
  gprState->rsp = user->rsp;
  gprState->r8 = user->r8;
  gprState->r9 = user->r9;
  gprState->r10 = user->r10;
  gprState->r11 = user->r11;
  gprState->r12 = user->r12;
  gprState->r13 = user->r13;
  gprState->r14 = user->r14;
  gprState->r15 = user->r15;
  gprState->rip = user->rip;
  gprState->eflags = user->eflags;
}

void userToFPRState(const FPR_STRUCT *user, QBDI::FPRState *fprState) {
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
  fprState->fop = user->fop;
  fprState->rfcw = user->cwd;
  fprState->rfsw = user->swd;
  fprState->mxcsr = user->mxcsr;
}

void LinuxProcess::getProcessGPR(QBDI::GPRState *gprState) {
  GPR_STRUCT user;
  if (ptrace(PTRACE_GETREGS, this->pid, NULL, &user) == -1) {
    QBDI_ERROR("Failed to get GPR state: {}", strerror(errno));
    exit(VALIDATOR_ERR_UNEXPECTED_API_FAILURE);
  }
  userToGPRState(&user, gprState);
}

void LinuxProcess::getProcessFPR(QBDI::FPRState *fprState) {
  FPR_STRUCT user;
  if (ptrace(PTRACE_GETFPREGS, this->pid, NULL, &user) == -1) {
    QBDI_ERROR("Failed to get FPR state: {}", strerror(errno));
    exit(VALIDATOR_ERR_UNEXPECTED_API_FAILURE);
  }
  userToFPRState(&user, fprState);
}
