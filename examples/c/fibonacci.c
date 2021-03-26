#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "QBDI.h"

int fibonacci(int n) {
  if (n <= 2)
    return 1;
  return fibonacci(n - 1) + fibonacci(n - 2);
}

VMAction showInstruction(VMInstanceRef vm, GPRState *gprState,
                         FPRState *fprState, void *data) {
  // Obtain an analysis of the instruction from the VM
  const InstAnalysis *instAnalysis = qbdi_getInstAnalysis(
      vm, QBDI_ANALYSIS_INSTRUCTION | QBDI_ANALYSIS_DISASSEMBLY);

  // Printing disassembly
  printf("0x%" PRIRWORD ": %s\n", instAnalysis->address,
         instAnalysis->disassembly);

  return QBDI_CONTINUE;
}

VMAction countIteration(VMInstanceRef vm, GPRState *gprState,
                        FPRState *fprState, void *data) {
  (*((int *)data))++;

  return QBDI_CONTINUE;
}

static const size_t STACK_SIZE = 0x100000; // 1MB

int main(int argc, char **argv) {
  int n = 0;

  int iterationCount = 0;
  uint8_t *fakestack;
  VMInstanceRef vm;
  GPRState *state;
  rword retvalue;

  if (argc >= 2) {
    n = atoi(argv[1]);
  }
  if (n < 1) {
    n = 1;
  }

  // Constructing a new QBDI VM
  qbdi_initVM(&vm, NULL, NULL, 0);

  // Get a pointer to the GPR state of the VM
  state = qbdi_getGPRState(vm);
  assert(state != NULL);

  // Setup initial GPR state,
  qbdi_allocateVirtualStack(state, STACK_SIZE, &fakestack);

  // Registering showInstruction() callback to print a trace of the execution
  uint32_t cid = qbdi_addCodeCB(vm, QBDI_PREINST, showInstruction, NULL);
  assert(cid != QBDI_INVALID_EVENTID);

  // Registering countIteration() callback
  qbdi_addMnemonicCB(vm, "CALL*", QBDI_PREINST, countIteration,
                     &iterationCount);

  // Setup Instrumentation Range
  bool res = qbdi_addInstrumentedModuleFromAddr(vm, (rword)&fibonacci);
  assert(res == true);

  // Running DBI execution
  printf("Running fibonacci(%d) ...\n", n);
  res = qbdi_call(vm, &retvalue, (rword)fibonacci, 1, n);
  assert(res == true);

  printf("fibonnaci(%d) returns %ld after %d recursions\n", n, retvalue,
         iterationCount);

  // cleanup
  qbdi_alignedFree(fakestack);
  qbdi_terminateVM(vm);

  return 0;
}
