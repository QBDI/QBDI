#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#include "QBDI.h"

#define FAKE_RET_ADDR 42


int fibonacci(int n) {
    if(n <= 2)
        return 1;
    return fibonacci(n-1) + fibonacci(n-2);
}

VMAction countRecursion(VMInstanceRef vm, GPRState *gprState, FPRState *fprState, void *data) {
    *((unsigned*) data) += 1;
    return QBDI_CONTINUE;
}
    
VMAction countInstruction(VMInstanceRef vm, GPRState *gprState, FPRState *fprState, void *data) {
    // Cast data to our counter
    uint32_t* counter = (uint32_t *) data;
    // Obtain an analysis of the instruction from the VM
    const InstAnalysis* instAnalysis = qbdi_getInstAnalysis(vm, QBDI_ANALYSIS_INSTRUCTION | QBDI_ANALYSIS_DISASSEMBLY);
    // Printing disassembly
    printf("%" PRIRWORD ": %s\n", instAnalysis->address, instAnalysis->disassembly);
    // Incrementing the instruction counter
    (*counter)++;
    // Signaling the VM to continue execution
    return QBDI_CONTINUE;
}

static const size_t STACK_SIZE = 0x100000; // 1MB

int main(int argc, char **argv) {
    int n = 0;
    unsigned recursions = 0;
    uint32_t counter = 0;
    uint8_t *fakestack;
    VMInstanceRef vm;
    GPRState *state;

    printf("Initializing VM ...\n");
    // Constructing a new QBDI VM
    qbdi_initVM(&vm, NULL, NULL);
    // Registering countInstruction() callback to be called after every instruction
    qbdi_addCodeCB(vm, QBDI_POSTINST, countInstruction, &counter);
    qbdi_addCodeAddrCB(vm, (rword) &fibonacci, QBDI_PREINST, countRecursion, &recursions);

    // Get a pointer to the GPR state of the VM
    state = qbdi_getGPRState(vm);
    // Setup initial GPR state, this fakestack will produce a ret NULL at the end of the execution
    qbdi_allocateVirtualStack(state, STACK_SIZE, &fakestack);
    // Argument to the fibonnaci call
    if(argc >= 2) {
        n = atoi(argv[1]);
    }
    if(n < 1) {
        n = 1;
    }
    qbdi_simulateCall(state, FAKE_RET_ADDR, 1, (rword) n);

    printf("Running fibonacci(%d) ...\n", n);
    // Running DBI execution
    qbdi_instrumentAllExecutableMaps(vm);
    qbdi_run(vm, (rword) fibonacci, (rword) FAKE_RET_ADDR);
    printf("fibonnaci ran in %u instructions, recursed %u times and returned %d\n",
            counter, recursions - 1, (int) QBDI_GPR_GET(state, REG_RETURN));
    qbdi_terminateVM(vm);
    qbdi_alignedFree(fakestack);

    return 0;
}
