#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "QBDI.h"

void stripLF(char *s) {
  char *pos = strrchr(s, '\n');
  if (pos != NULL) {
    *pos = '\0';
  }
}

QBDI_NOINLINE uint64_t magicPow(uint64_t n, uint64_t e) {
  uint64_t r = 1;
  uint64_t i = 0;

  for (i = 0; i < e; i++) {
    r = (r * n);
  }
  return r;
}

QBDI_NOINLINE uint64_t magicHash(char *secret) {
  uint64_t hash = 0;
  uint64_t acc = 1;
  int len = strlen(secret);
  int i = 0;

  for (i = 0; i < len; i++) {
    uint64_t magic = magicPow(secret[i], acc);
    hash ^= magic;
    acc = (acc + magic) % 256;
  }

  return hash;
}

QBDI_NOINLINE int thedude() {
  static int BUFFER_SIZE = 64;
  time_t t = 0;
  char *name = (char *)malloc(BUFFER_SIZE);
  char *secret = (char *)malloc(2 * BUFFER_SIZE);
  uint64_t hash = 0;

  printf("Hi I'm the dude.\n");
  printf("Give me your name and I'll give you a hash.\n");
  printf("So what's your name ? ");
  if (!fgets(name, BUFFER_SIZE, stdin)) {
    return 1;
  }
  stripLF(name);
  time(&t);
  sprintf(secret, "%" PRIu64 ":%s", (uint64_t)t, name);
  printf("Ok I'll give you the hash of %s.\n", secret);
  hash = magicHash(secret);
  printf("Your hash is %" PRIx64 ".\n", hash);
  printf("No need to thank me.\n");

  free(name);
  free(secret);

  return 0;
}

VMAction count(VMInstanceRef vm, GPRState *gprState, FPRState *fprState,
               void *data) {
  // Cast data to our CallbackInfo struct
  uint32_t *counter = (uint32_t *)data;
  // Obtain an analysis of the instruction from the VM
  const InstAnalysis *instAnalysis = qbdi_getInstAnalysis(
      vm, QBDI_ANALYSIS_INSTRUCTION | QBDI_ANALYSIS_DISASSEMBLY);
  // Printing disassembly
  printf("%" PRIRWORD ": %s\n", instAnalysis->address,
         instAnalysis->disassembly);
  // Incrementing the instruction counter
  (*counter)++;
  // Signaling the VM to continue execution
  return QBDI_CONTINUE;
}

static const size_t STACK_SIZE = 0x100000; // 1MB
static const rword FAKE_RET_ADDR = 0x40000;

int main(int argc, char **argv) {
  uint8_t *fakestack;
  VMInstanceRef vm;
  GPRState *state;
  int traceLevel = 0;
  uint32_t counter = 0;
  bool instrumented;

  if (argc > 1) {
    traceLevel = atoi(argv[1]);
    if (traceLevel > 2) {
      traceLevel = 2;
    }
  }

  printf("Initializing VM ...\n");
  // Constructing a new QBDI VM
  qbdi_initVM(&vm, NULL, NULL, 0);
  // Registering count() callback to be called after every instruction
  qbdi_addCodeCB(vm, QBDI_POSTINST, count, &counter);

  // Get a pointer to the GPR state of the VM
  state = qbdi_getGPRState(vm);
  // Setup initial GPR state, this fakestack will produce a ret NULL at the end
  // of the execution
  qbdi_allocateVirtualStack(state, STACK_SIZE, &fakestack);
  qbdi_simulateCall(state, FAKE_RET_ADDR, 0);

  printf("Running thedude() with trace level %d...\n", traceLevel);
  // Running DBI execution
  instrumented = qbdi_addInstrumentedModuleFromAddr(vm, (rword)&main);
  if (instrumented) {
    if (traceLevel < 1) {
      qbdi_removeInstrumentedRange(vm, (rword)magicHash, (rword)magicHash + 32);
    }
    if (traceLevel < 2) {
      qbdi_removeInstrumentedRange(vm, (rword)magicPow, (rword)magicPow + 32);
    }
    qbdi_run(vm, (rword)thedude, (rword)FAKE_RET_ADDR);
    printf("thedude ran in %u instructions\n", counter);
  } else {
    printf("failed to find main module...\n");
  }
  qbdi_terminateVM(vm);
  qbdi_alignedFree(fakestack);

  return 0;
}
