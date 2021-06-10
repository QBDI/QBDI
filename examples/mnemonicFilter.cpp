#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#include "QBDI.h"

#define RED     "\x1b[31m"
#define MAGENTA "\x1b[35m"
#define RESET   "\x1b[0m"


int HW(void) {
    int i;
    for (i=0; i < 10; i++) {
        printf("Hello World !\n");
    }
    return 0;
}


QBDI::VMAction CBMnemonic(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState,
                                QBDI::FPRState *fprState, void *data) {
    const QBDI::InstAnalysis* instAnalysis = vm->getInstAnalysis();
    printf("MNEMONIC @ %s%p%s =>%s%s%s\n",RED, (void *) instAnalysis->address, RESET, MAGENTA, instAnalysis->disassembly, RESET);
    return QBDI::VMAction::CONTINUE;
}


int main(int argc, char ** argv)
{
    uint8_t *fakestack = nullptr;
    QBDI::GPRState *state;

    QBDI::setLogPriority(QBDI::LogPriority::DEBUG);

    // Constructing a new QBDI vm
    QBDI::VM *vm = new QBDI::VM();

    // Add a callback on every instruction with a mnemonic starting with 'j' (ex: jmp)
    vm->addMnemonicCB("j*", QBDI::POSTINST, CBMnemonic, NULL);

    // Get a pointer to the GPR state of the vm
    state = vm->getGPRState();
    // Setup initial GPR state, this fakestack will produce a ret NULL at the end of the execution
    QBDI::allocateVirtualStack(state, 0x100000, &fakestack);
    // Add our binary to the instrumented range
    vm->addInstrumentedModuleFromAddr((QBDI::rword) &HW);
    // Call the hello world function
    QBDI::simulateCall(state, 0x0);
    vm->run((QBDI::rword) HW, (QBDI::rword) 0x0);

    delete vm;
    QBDI::alignedFree(fakestack);
    return 0;
}
