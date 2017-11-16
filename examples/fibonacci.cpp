#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstdint>
#include <cstring>

#include "QBDI.h"

#define FAKE_RET_ADDR 42

int fibonacci(int n) {
    if(n <= 2)
        return 1;
    return fibonacci(n-1) + fibonacci(n-2);
}


QBDI::VMAction countInstruction(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState, 
                                QBDI::FPRState *fprState, void *data) {
    // Cast data to our counter
    uint32_t* counter = (uint32_t*) data;
    // Obtain an analysis of the instruction from the vm
    const QBDI::InstAnalysis* instAnalysis = vm->getInstAnalysis();
    // Printing disassembly
    std::cout << std::setbase(16) << instAnalysis->address << ": "
        << instAnalysis->disassembly << std::endl << std::setbase(10);
    // Incrementing the instruction counter
    (*counter)++;
    // Signaling the VM to continue execution
    return QBDI::VMAction::CONTINUE;
}

QBDI::VMAction countRecursion(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState, 
                              QBDI::FPRState *fprState, void *data) {
    *((unsigned*) data) += 1;
    return QBDI::VMAction::CONTINUE;
}

static const size_t STACK_SIZE = 0x100000; // 1MB

int main(int argc, char** argv) {
    int n = 0;
    uint32_t counter = 0;
    unsigned recursions = 0;
    uint8_t *fakestack = nullptr;
    QBDI::GPRState *state;

    std::cout << "Initializing VM ..." << std::endl;
    // Constructing a new QBDI vm
    QBDI::VM *vm = new QBDI::VM();
    // Registering countInstruction() callback to be called after every instruction
    vm->addCodeCB(QBDI::POSTINST, countInstruction, &counter);
    // Registering countRecursion() callback to be called before the first instruction of fibonacci
    vm->addCodeAddrCB((QBDI::rword) &fibonacci, QBDI::PREINST, countRecursion, &recursions);

    // Get a pointer to the GPR state of the vm
    state = vm->getGPRState();
    // Setup initial GPR state, this fakestack will produce a ret NULL at the end of the execution
    QBDI::allocateVirtualStack(state, STACK_SIZE, &fakestack);
    // Argument to the fibonnaci call
    if(argc >= 2) {
        n = atoi(argv[1]);
    }
    if(n < 1) {
        n = 1;
    }
    QBDI::simulateCall(state, FAKE_RET_ADDR, {(QBDI::rword) n});

    std::cout << "Running fibonacci(" << n << ") ..." << std::endl;
    // Instrument everything
    vm->instrumentAllExecutableMaps();
    // Run the DBI execution
    vm->run((QBDI::rword) fibonacci, (QBDI::rword) FAKE_RET_ADDR);
    std::cout << "fibonnaci ran in " << counter << " instructions, recursed " << recursions - 1
        << " times and returned " << QBDI_GPR_GET(state, QBDI::REG_RETURN) << std::endl;

    delete vm;
    QBDI::alignedFree(fakestack);

    return 0;
}
