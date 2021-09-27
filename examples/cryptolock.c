#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "QBDI.h"

QBDI_NOINLINE void hashPassword(char *hash, const char *password) {
  char acc = 42;
  size_t i = 0;
  size_t hash_len = strlen(hash);
  size_t password_len = strlen(password);

  for (i = 0; i < hash_len && i < password_len; i++) {
    hash[i] = (hash[i] ^ acc) - password[i];
    acc = password[i];
  }
}

char SECRET[] =
    "\x29\x0d\x20\x00\x00\x00\x00\x0a\x65\x1f\x32\x00\x19\x0c\x4e"
    "\x1b\x2d\x09\x66\x0c\x1a\x06\x05\x06\x20\x1f\x46";

QBDI_NOINLINE const char *getSecret(const char *password) {
  size_t i = 0;
  size_t password_len = strlen(password);
  for (i = 0; i < sizeof(SECRET); i++) {
    SECRET[i] ^= password[i % password_len];
  }
  return SECRET;
}

QBDI_NOINLINE const char *cryptolock(const char *password) {
  char hash[] = "\x6f\x29\x2a\x29\x1a\x1c\x07\x01";
  size_t i = 0;

  hashPassword(hash, password);

  bool good = true;
  for (i = 0; i < sizeof(hash); i++) {
    if (hash[i] != 0) {
      good = false;
    }
  }

  if (good) {
    return getSecret(password);
  } else {
    return NULL;
  }
}

VMAction onwrite(VMInstanceRef vm, GPRState *gprState, FPRState *fprState,
                 void *data) {
  size_t i = 0;
  size_t num_access = 0;
  // Obtain an analysis of the instruction from the vm
  const InstAnalysis *instAnalysis = qbdi_getInstAnalysis(
      vm, QBDI_ANALYSIS_INSTRUCTION | QBDI_ANALYSIS_DISASSEMBLY);
  // Obtain the instruction memory accesses
  MemoryAccess *memAccesses = qbdi_getInstMemoryAccess(vm, &num_access);

  // Printing disassembly
  printf("%" PRIRWORD ": %s\n", instAnalysis->address,
         instAnalysis->disassembly);
  // Printing write memory accesses
  for (i = 0; i < num_access; i++) {
    // Checking the access mode
    if (memAccesses[i].type == QBDI_MEMORY_WRITE) {
      // Writing the written value, the size and the address
      printf("\tWrote 0x%" PRIRWORD " on %u bytes at 0x%" PRIRWORD "\n",
             memAccesses[i].value, (unsigned)memAccesses[i].size,
             memAccesses[i].accessAddress);
    }
  }
  printf("\n");
  free(memAccesses);

  return QBDI_CONTINUE;
}

static const size_t STACK_SIZE = 0x100000; // 1MB
static const rword FAKE_RET_ADDR = 0x40000;

int main(int argc, char **argv) {
  uint8_t *fakestack = NULL;
  VMInstanceRef vm;
  GPRState *state;

  if (argc < 2) {
    printf("Please give a password as first argument\n");
    return 1;
  }

  printf("Initializing VM ...\n");
  // Constructing a new QBDI vm
  qbdi_initVM(&vm, NULL, NULL, 0);
  // Registering a callback on every memory write to our onwrite() function
  qbdi_addMemAccessCB(vm, QBDI_MEMORY_WRITE, onwrite, NULL, 0);
  // Instrument this module
  qbdi_addInstrumentedModuleFromAddr(vm, (rword)&cryptolock);
  // Get a pointer to the GPR state of the vm
  state = qbdi_getGPRState(vm);
  // Setup initial GPR state, this fakestack will produce a ret FAKE_RET_ADDR
  //   at the end of the execution
  // Also setup one argument on the stack which is the password string
  qbdi_allocateVirtualStack(state, STACK_SIZE, &fakestack);
  qbdi_simulateCall(state, FAKE_RET_ADDR, 1, argv[1]);

  printf("Running cryptolock(\"%s\")\n", argv[1]);
  qbdi_run(vm, (rword)cryptolock, (rword)FAKE_RET_ADDR);
  // Getting the return value from the call
  const char *ret = (const char *)QBDI_GPR_GET(state, REG_RETURN);
  // If it is not null, display it
  if (ret != NULL) {
    printf("Returned \"%s\"\n", ret);
  } else {
    printf("Returned null\n");
  }

  qbdi_terminateVM(vm);
  qbdi_alignedFree(fakestack);

  return 0;
}
