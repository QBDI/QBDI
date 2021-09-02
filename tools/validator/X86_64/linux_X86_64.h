#ifndef LINUX_X86_64_H
#define LINUX_X86_64_H

#include <unistd.h>

#include <QBDI.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include "linux_process.h"

#define SIGBRK SIGTRAP
static const long BRK_MASK = 0xFF;
static const long BRK_INS = 0xCC;

typedef struct user_regs_struct GPR_STRUCT;
typedef struct user_fpregs_struct FPR_STRUCT;

void userToGPRState(const GPR_STRUCT *user, QBDI::GPRState *gprState);
void userToFPRState(const FPR_STRUCT *user, QBDI::FPRState *fprState);

static inline void fix_GPR_STRUCT(GPR_STRUCT *user) { user->rip -= 1; }

#endif // LINUX_X86_64_H
