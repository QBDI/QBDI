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
#include "QBDIPreload.h"

#include <dlfcn.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ucontext.h>
#include <sys/wait.h>
#include <unistd.h>

#include <QBDI.h>

#if defined(QBDI_ARCH_X86)
#include "X86/linux_X86.h"
#elif defined(QBDI_ARCH_X86_64)
#include "X86_64/linux_X86_64.h"
#elif defined(QBDI_ARCH_ARM)
#include "ARM/linux_ARM.h"
#elif defined(QBDI_ARCH_AARCH64)
#include "AARCH64/linux_AARCH64.h"
#else
#error "Architecture not supported"
#endif

static const size_t STACK_SIZE = 8388608;
static bool HAS_EXITED = false;
static bool HAS_PRELOAD = false;
static bool DEFAULT_HANDLER = false;
GPRState ENTRY_GPR;
FPRState ENTRY_FPR;

struct sigaction default_sa;
void redirectExec(int signum, siginfo_t *info, void *data);

static struct {
  void *address;
  long value;
} ENTRY_BRK;

void setEntryBreakpoint(void *address_) {
  long mask, bytecode;
  void *address = correctAddress(address_, &bytecode, &mask);
  long pageSize = sysconf(_SC_PAGESIZE);
  uintptr_t base = (uintptr_t)address - ((uintptr_t)address % pageSize);
  bool twoPages = ((((uintptr_t)address) + sizeof(long)) > (base + pageSize));

  ENTRY_BRK.address = address;
  mprotect((void *)base, pageSize, PROT_READ | PROT_WRITE);
  if (twoPages) {
    mprotect((void *)(base + pageSize), pageSize, PROT_READ | PROT_WRITE);
  }

  ENTRY_BRK.value = *((long *)address);
  *((long *)address) = bytecode | (ENTRY_BRK.value & (~mask));

  mprotect((void *)base, pageSize, PROT_READ | PROT_EXEC);
  if (twoPages) {
    mprotect((void *)(base + pageSize), pageSize, PROT_READ | PROT_EXEC);
  }
}

void unsetEntryBreakpoint() {
  long pageSize = sysconf(_SC_PAGESIZE);
  uintptr_t base =
      (uintptr_t)ENTRY_BRK.address - ((uintptr_t)ENTRY_BRK.address % pageSize);
  bool twoPages =
      ((((uintptr_t)ENTRY_BRK.address) + sizeof(long)) > (base + pageSize));

  mprotect((void *)base, pageSize, PROT_READ | PROT_WRITE);
  if (twoPages) {
    mprotect((void *)(base + pageSize), pageSize, PROT_READ | PROT_WRITE);
  }

  *((long *)ENTRY_BRK.address) = ENTRY_BRK.value;

  mprotect((void *)base, pageSize, PROT_READ | PROT_EXEC);
  if (twoPages) {
    mprotect((void *)(base + pageSize), pageSize, PROT_READ | PROT_EXEC);
  }

  struct sigaction sa;
  if (sigaction(SIGBRK, NULL, &sa) == 0 && (sa.sa_flags & SA_SIGINFO) != 0 &&
      sa.sa_sigaction == redirectExec) {
    if (sigaction(SIGBRK, &default_sa, NULL) != 0) {
      fprintf(stderr, "Fail to restore sigaction");
    }
  }
}

void catchEntrypoint(int argc, char **argv) {
  int status = QBDIPRELOAD_NOT_HANDLED;

  unsetEntryBreakpoint();

  status = qbdipreload_on_main(argc, argv);

  if (DEFAULT_HANDLER && (status == QBDIPRELOAD_NOT_HANDLED)) {
    VMInstanceRef vm;
    qbdi_initVM(&vm, NULL, NULL, 0);
    qbdi_instrumentAllExecutableMaps(vm);

    size_t size = 0;
    qbdi_MemoryMap *modules = qbdi_getCurrentProcessMaps(false, &size);

    // Filter some modules to avoid conflicts
    qbdi_removeInstrumentedModuleFromAddr(vm, (rword)&catchEntrypoint);
    removeConflictModule(vm, modules, size);

    qbdi_freeMemoryMapArray(modules, size);

    // Set original states
    qbdi_setGPRState(vm, &ENTRY_GPR);
    qbdi_setFPRState(vm, &ENTRY_FPR);

    rword start = QBDI_GPR_GET(qbdi_getGPRState(vm), REG_PC);
    rword stop = getReturnAddress(qbdi_getGPRState(vm));
    status = qbdipreload_on_run(vm, start, stop);
  }
  exit(status);
}

void redirectExec(int signum, siginfo_t *info, void *data) {
  ucontext_t *uap = (ucontext_t *)data;

  fix_ucontext_t(uap);

  int status = qbdipreload_on_premain((void *)uap, (void *)uap);

  // Copying the initial thread state
  qbdipreload_threadCtxToGPRState(uap, &ENTRY_GPR);
  qbdipreload_floatCtxToFPRState(uap, &ENTRY_FPR);

  // if not handled, use default handler
  if (status == QBDIPRELOAD_NOT_HANDLED) {
    DEFAULT_HANDLER = true;
    void *newStack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE,
                          MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    prepareStack(newStack, STACK_SIZE, uap);
  }

  setPC(uap, (rword)catchEntrypoint);
}

static void *setupExceptionHandler(void (*action)(int, siginfo_t *, void *)) {
  struct sigaction sa;
  sa.sa_sigaction = action;
  sa.sa_flags = SA_SIGINFO;
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGBRK, &sa, &default_sa) == -1) {
    fprintf(stderr, "Could not set redirectExec handler ...");
    exit(QBDIPRELOAD_ERR_STARTUP_FAILED);
  }
  return NULL;
}

void *qbdipreload_setup_exception_handler(uint32_t target, uint32_t mask,
                                          void *handler) {
  return setupExceptionHandler(handler);
}

int qbdipreload_hook_main(void *main) {
  setEntryBreakpoint(main);
  setupExceptionHandler(redirectExec);
  return QBDIPRELOAD_NO_ERROR;
}

QBDI_FORCE_EXPORT void exit(int status) {
  if (!HAS_EXITED && HAS_PRELOAD) {
    HAS_EXITED = true;
    qbdipreload_on_exit(status);
  }
  ((void (*)(int))dlsym(RTLD_NEXT, "exit"))(status);
  __builtin_unreachable();
}

QBDI_FORCE_EXPORT void _exit(int status) {
  if (!HAS_EXITED && HAS_PRELOAD) {
    HAS_EXITED = true;
    qbdipreload_on_exit(status);
  }
  ((void (*)(int))dlsym(RTLD_NEXT, "_exit"))(status);
  __builtin_unreachable();
}

typedef int (*start_main_fn)(int (*)(int, char **, char **), int, char **,
                             void (*)(void), void (*)(void), void (*)(void),
                             void *);

QBDI_FORCE_EXPORT int __libc_start_main(int (*main)(int, char **, char **),
                                        int argc, char **ubp_av,
                                        void (*init)(void), void (*fini)(void),
                                        void (*rtld_fini)(void),
                                        void(*stack_end)) {

  start_main_fn o_libc_start_main =
      (start_main_fn)dlsym(RTLD_NEXT, "__libc_start_main");

  // do nothing if the library isn't preload
  if (getenv("LD_PRELOAD") == NULL)
    return o_libc_start_main(main, argc, ubp_av, init, fini, rtld_fini,
                             stack_end);

  HAS_PRELOAD = true;
  int status = qbdipreload_on_start(main);
  if (status == QBDIPRELOAD_NOT_HANDLED) {
    status = qbdipreload_hook_main(main);
  }
  if (status == QBDIPRELOAD_NO_ERROR) {
    return o_libc_start_main(main, argc, ubp_av, init, fini, rtld_fini,
                             stack_end);
  }
  exit(0);
}

int qbdipreload_hook_init() {
  // not used on linux
  return QBDIPRELOAD_NO_ERROR;
}
