#ifndef QBDIPRELOAD_X86_H
#define QBDIPRELOAD_X86_H

#include <dlfcn.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include <QBDI.h>

#define SIGBRK SIGTRAP
static const long BRK_MASK = 0xFF;
static const long BRK_INS = 0xCC;

void qbdipreload_threadCtxToGPRState(const void *gprCtx, GPRState *gprState);
void qbdipreload_floatCtxToFPRState(const void *fprCtx, FPRState *fprState);

static inline rword getReturnAddress(GPRState *gprState) {
  return *((rword *)QBDI_GPR_GET(gprState, REG_SP));
}

static inline void fix_ucontext_t(ucontext_t *uap) {
  uap->uc_mcontext.gregs[REG_EIP] -= 1;
}

void prepareStack(void *newStack, size_t sizeStack, ucontext_t *uap);

static inline void setPC(ucontext_t *uap, rword address) {
  uap->uc_mcontext.gregs[REG_EIP] = address;
}

void removeConflictModule(VMInstanceRef vm, qbdi_MemoryMap *modules,
                          size_t size);

#endif // QBDIPRELOAD_X86_H