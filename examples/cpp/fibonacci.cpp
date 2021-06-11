#include <assert.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>

#include "QBDI.h"

int fibonacci(int n) {
  if (n <= 2)
    return 1;
  return fibonacci(n - 1) + fibonacci(n - 2);
}

QBDI::VMAction showInstruction(QBDI::VM *vm, QBDI::GPRState *gprState,
                               QBDI::FPRState *fprState, void *data) {
  // Obtain an analysis of the instruction from the VM
  const QBDI::InstAnalysis *instAnalysis = vm->getInstAnalysis();

  // Printing disassembly
  std::cout << std::setbase(16) << instAnalysis->address << ": "
            << instAnalysis->disassembly << std::endl
            << std::setbase(10);

  return QBDI::VMAction::CONTINUE;
}

QBDI::VMAction countIteration(QBDI::VM *vm, QBDI::GPRState *gprState,
                              QBDI::FPRState *fprState, void *data) {
  (*((unsigned *)data))++;

  return QBDI::CONTINUE;
}

static const size_t STACK_SIZE = 0x100000; // 1MB

int main(int argc, char **argv) {
  int n = 0;
  bool res;
  uint32_t cid;

  unsigned iterationCount = 0;
  uint8_t *fakestack;
  QBDI::GPRState *state;
  QBDI::rword retvalue;

  // Argument to the fibonnaci call
  if (argc >= 2) {
    n = atoi(argv[1]);
  }
  if (n < 1) {
    n = 1;
  }

  // Constructing a new QBDI vm
  QBDI::VM vm{};

  // Get a pointer to the GPR state of the VM
  state = vm.getGPRState();
  assert(state != nullptr);

  // Setup initial GPR state,
  res = QBDI::allocateVirtualStack(state, STACK_SIZE, &fakestack);
  assert(res == true);

  // Registering showInstruction() callback to print a trace of the execution
  cid = vm.addCodeCB(QBDI::PREINST, showInstruction, nullptr);
  assert(cid != QBDI::INVALID_EVENTID);

  // Registering countIteration() callback
  vm.addMnemonicCB("CALL*", QBDI::PREINST, countIteration, &iterationCount);

  // Setup Instrumentation Range
  res = vm.addInstrumentedModuleFromAddr(
      reinterpret_cast<QBDI::rword>(fibonacci));
  assert(res == true);

  // Running DBI execution
  std::cout << "Running fibonacci(" << n << ") ..." << std::endl;
  res = vm.call(&retvalue, reinterpret_cast<QBDI::rword>(fibonacci),
                {static_cast<QBDI::rword>(n)});
  assert(res == true);

  std::cout << "fibonnaci(" << n << ") returns " << retvalue << " after "
            << iterationCount << " recursions" << std::endl;

  QBDI::alignedFree(fakestack);

  return 0;
}
