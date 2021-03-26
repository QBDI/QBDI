#include <cstring>
#include <iomanip>
#include <iostream>
#include <vector>

#include "QBDI.h"

QBDI_NOINLINE void hashPassword(char *hash, const char *password) {
  char acc = 42;
  size_t hash_len = strlen(hash);
  size_t password_len = strlen(password);

  for (size_t i = 0; i < hash_len && i < password_len; i++) {
    hash[i] = (hash[i] ^ acc) - password[i];
    acc = password[i];
  }
}

char SECRET[] =
    "\x29\x0d\x20\x00\x00\x00\x00\x0a\x65\x1f\x32\x00\x19\x0c\x4e"
    "\x1b\x2d\x09\x66\x0c\x1a\x06\x05\x06\x20\x1f\x46";

QBDI_NOINLINE const char *getSecret(const char *password) {
  size_t password_len = strlen(password);
  for (size_t i = 0; i < sizeof(SECRET); i++) {
    SECRET[i] ^= password[i % password_len];
  }
  return SECRET;
}

QBDI_NOINLINE const char *cryptolock(const char *password) {
  char hash[] = "\x6f\x29\x2a\x29\x1a\x1c\x07\x01";

  hashPassword(hash, password);

  bool good = true;
  for (size_t i = 0; i < sizeof(hash); i++) {
    if (hash[i] != 0) {
      good = false;
    }
  }

  if (good) {
    return getSecret(password);
  } else {
    return nullptr;
  }
}

QBDI::VMAction onwrite(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState,
                       QBDI::FPRState *fprState, void *data) {
  // Obtain an analysis of the instruction from the vm
  const QBDI::InstAnalysis *instAnalysis = vm->getInstAnalysis();
  // Obtain the instruction memory accesses
  std::vector<QBDI::MemoryAccess> memAccesses = vm->getInstMemoryAccess();

  // Printing disassembly
  std::cout << std::setbase(16) << instAnalysis->address << ": "
            << instAnalysis->disassembly << std::endl
            << std::setbase(10);
  // Printing write memory accesses
  for (const QBDI::MemoryAccess &memAccess : memAccesses) {
    // Checking the access mode
    if (memAccess.type == QBDI::MEMORY_WRITE) {
      // Writing the written value, the size and the address
      std::cout << "\t"
                << "Wrote 0x" << std::setbase(16) << memAccess.value << " on "
                << std::setbase(10) << (int)memAccess.size << " bytes at 0x"
                << std::setbase(16) << memAccess.accessAddress << std::endl;
    }
  }
  std::cout << std::endl;

  return QBDI::CONTINUE;
}

static const size_t STACK_SIZE = 0x100000; // 1MB
static const QBDI::rword FAKE_RET_ADDR = 0x40000;

int main(int argc, char **argv) {
  uint8_t *fakestack = nullptr;
  QBDI::GPRState *state;

  if (argc < 2) {
    std::cout << "Please give a password as first argument" << std::endl;
    return 1;
  }

  std::cout << "Initializing VM ..." << std::endl;
  // Constructing a new QBDI vm
  QBDI::VM *vm = new QBDI::VM();
  // Registering a callback on every memory write to our onwrite() function
  vm->addMemAccessCB(QBDI::MEMORY_WRITE, onwrite, nullptr);
  // Instrument this module
  vm->addInstrumentedModuleFromAddr((QBDI::rword)&cryptolock);
  // Get a pointer to the GPR state of the vm
  state = vm->getGPRState();
  // Setup initial GPR state, this fakestack will produce a ret FAKE_RET_ADDR
  //   at the end of the execution
  // Also setup one argument on the stack which is the password string
  QBDI::allocateVirtualStack(state, STACK_SIZE, &fakestack);
  QBDI::simulateCall(state, FAKE_RET_ADDR, {(QBDI::rword)argv[1]});

  std::cout << "Running cryptolock(\"" << argv[1] << "\")" << std::endl;
  vm->run((QBDI::rword)cryptolock, (QBDI::rword)FAKE_RET_ADDR);
  // Getting the return value from the call
  const char *ret = (const char *)QBDI_GPR_GET(state, QBDI::REG_RETURN);
  // If it is not null, display it
  if (ret != nullptr) {
    std::cout << "Returned \"" << ret << "\"" << std::endl;
  } else {
    std::cout << "Returned null" << std::endl;
  }

  delete vm;
  QBDI::alignedFree(fakestack);

  return 0;
}
