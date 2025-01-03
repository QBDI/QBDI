#include <cinttypes>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>

#include "QBDI.h"

QBDI_NOINLINE uint64_t magicPow(uint64_t n, uint64_t e) {
  uint64_t r = 1;
  uint64_t i = 0;

  for (i = 0; i < e; i++) {
    r = (r * n);
  }
  return r;
}

QBDI_NOINLINE uint64_t magicHash(const char *secret) {
  uint64_t hash = 0;
  uint64_t acc = 1;
  size_t len = strlen(secret);
  size_t i = 0;

  for (i = 0; i < len; i++) {
    uint64_t magic = magicPow(secret[i], acc);
    hash ^= magic;
    acc = (acc + magic) % 256;
  }

  return hash;
}

QBDI_NOINLINE int thedude() {
  time_t t = 0;
  std::string name;
  uint64_t hash = 0;
  char *secret = nullptr;

  std::cout << "Hi I'm the dude." << std::endl;
  std::cout << "Give me your name and I'll give you a hash." << std::endl;
  std::cout << "So what's your name ? ";
  std::cin >> name;
  time(&t);
  secret = new char[name.length() + 16 + 2];
  snprintf(secret, name.length() + 16 + 2, "%" PRIu64 ":%s", (uint64_t)t,
           name.c_str());
  std::cout << "Ok I'll give you the hash of " << secret << "." << std::endl;
  hash = magicHash(secret);
  std::cout << "Your hash is " << hash << "." << std::endl;
  std::cout << "No need to thank me." << std::endl;

  delete[] secret;
  return 0;
}

QBDI::VMAction count(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState,
                     QBDI::FPRState *fprState, void *data) {
  // Cast data to our counter
  uint32_t *counter = (uint32_t *)data;
  // Obtain an analysis of the instruction from the vm
  const QBDI::InstAnalysis *instAnalysis = vm->getInstAnalysis();
  // Printing disassembly
  std::cout << std::setbase(16) << instAnalysis->address << ": "
            << instAnalysis->disassembly << std::endl
            << std::setbase(10);
  // Incrementing the instruction counter
  (*counter)++;
  // Signaling the VM to continue execution
  return QBDI::VMAction::CONTINUE;
}

static const size_t STACK_SIZE = 0x100000; // 1MB
static const QBDI::rword FAKE_RET_ADDR = 0x40000;

int main(int argc, char **argv) {
  uint8_t *fakestack = nullptr;
  QBDI::GPRState *state;
  int traceLevel = 0;
  bool instrumented;
  uint32_t counter = 0;

  if (argc > 1) {
    traceLevel = atoi(argv[1]);
    if (traceLevel > 2) {
      traceLevel = 2;
    }
  }

  std::cout << "Initializing VM ..." << std::endl;
  // Constructing a new QBDI vm
  QBDI::VM *vm = new QBDI::VM();
  // Registering count() callback to be called after every instruction
  vm->addCodeCB(QBDI::POSTINST, count, &counter);

  // Get a pointer to the GPR state of the vm
  state = vm->getGPRState();
  // Setup initial GPR state, this fakestack will produce a ret NULL at the end
  // of the execution
  QBDI::allocateVirtualStack(state, STACK_SIZE, &fakestack);
  QBDI::simulateCall(state, FAKE_RET_ADDR);

  std::cout << "Running thedude() with trace level " << traceLevel << "..."
            << std::endl;
  // Select which part to instrument
  instrumented = vm->addInstrumentedModuleFromAddr((QBDI::rword)&main);
  if (instrumented) {
    if (traceLevel < 1) {
      vm->removeInstrumentedRange((QBDI::rword)magicHash,
                                  (QBDI::rword)magicHash + 32);
    }
    if (traceLevel < 2) {
      vm->removeInstrumentedRange((QBDI::rword)magicPow,
                                  (QBDI::rword)magicPow + 32);
    }
    // Run the DBI execution
    vm->run((QBDI::rword)thedude, (QBDI::rword)FAKE_RET_ADDR);
    std::cout << "thedude ran in " << counter << " instructions" << std::endl;
  } else {
    std::cout << "failed to find main module..." << std::endl;
  }
  delete vm;
  QBDI::alignedFree(fakestack);

  return 0;
}
