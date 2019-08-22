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

QBDI::VMAction inc(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState, QBDI::FPRState *fprState, void *data) {
    *static_cast<QBDI::rword*>(data) += 1;
    return QBDI::CONTINUE;
}

QBDI::VMAction min(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState, QBDI::FPRState *fprState, void *data) {
    QBDI::rword* min_stack = static_cast<QBDI::rword*>(data);
    if (*min_stack > gprState->rsp) *min_stack = gprState->rsp;
    return QBDI::CONTINUE;
}

static const size_t STACK_SIZE = 0x100000; // 1MB

typedef struct {
    QBDI::rword instCounter = 0;
    QBDI::rword recursions = 0;
    QBDI::rword minStack;
} Counter;

std::vector<QBDI::InstrumentDataCBK> on_instrument(QBDI::VMInstanceRef vm, const QBDI::InstAnalysis *inst, void* _data) {
    std::vector<QBDI::InstrumentDataCBK> cbks;
    Counter* data = static_cast<Counter*>(_data);

    std::cout << "Instrument instruction " << std::left << std::setw(30) << inst->disassembly << std::right;

    // call back instruction counter
    cbks.push_back({inc, &(data->instCounter), QBDI::POSTINST});
    std::cout << " inc(instCounter)";

    if (inst->address == reinterpret_cast<QBDI::rword>(fibonacci)) {
        cbks.push_back({inc, &(data->recursions), QBDI::POSTINST});
        std::cout << " inc(recursions)";
    }

    for (size_t i = 0; i < inst->numOperands; i++) {
        if (inst->operands[i].type == QBDI::OPERAND_GPR &&
                inst->operands[i].regCtxIdx == QBDI::REG_SP) {
            cbks.push_back({min, &(data->minStack), QBDI::POSTINST});
            std::cout << " min(minStack)";
            break;
        }
    }


    std::cout << std::endl;
    return cbks;
}

int main(int argc, char** argv) {
    int n = 0;
    Counter counter = Counter();
    QBDI::rword returnValue;
    uint8_t *fakestack = nullptr;

    // Argument to the fibonnaci call
    if(argc >= 2) {
        n = atoi(argv[1]);
    }
    if(n < 1) {
        n = 1;
    }

    std::cout << "Initializing VM ..." << std::endl;
    // Constructing a new QBDI vm
    QBDI::VM vm = QBDI::VM();
    // Setup guest stack
    QBDI::allocateVirtualStack(vm.getGPRState(), STACK_SIZE, &fakestack);
    counter.minStack = vm.getGPRState()->rsp;

    vm.addInstrRule(&on_instrument, QBDI::ANALYSIS_INSTRUCTION | QBDI::ANALYSIS_OPERANDS | QBDI::ANALYSIS_DISASSEMBLY, &counter);

    // Instrument fibonacci module
    vm.addInstrumentedModuleFromAddr(reinterpret_cast<QBDI::rword>(fibonacci));

    // Call fibonacci with QBDI
    std::cout << "Running fibonacci(" << n << ") ..." << std::endl;
    vm.call(&returnValue, reinterpret_cast<QBDI::rword>(fibonacci), {static_cast<QBDI::rword>(n)});

    std::cout << "fibonnaci ran in " << counter.instCounter << " instructions, recursed " << counter.recursions - 1
        << " times, with a maximum size of stack of " << STACK_SIZE - (counter.minStack - reinterpret_cast<QBDI::rword>(fakestack))
        << " bytes and returned " << returnValue << std::endl;

    QBDI::alignedFree(fakestack);

    return 0;
}
