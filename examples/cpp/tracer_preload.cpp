#include <iostream>
#include <iomanip>
#include "QBDIPreload.h"

static QBDI::VMAction onInstruction(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState, QBDI::FPRState *fprState, void *data) {
    const QBDI::InstAnalysis* instAnalysis = vm->getInstAnalysis();

    std::cout << std::setbase(16) << instAnalysis->address << ": "
              << instAnalysis->disassembly << std::endl << std::setbase(10);
    return QBDI::CONTINUE;
}

extern "C" {

QBDIPRELOAD_INIT;

int qbdipreload_on_start(void *main) {
    return QBDIPRELOAD_NOT_HANDLED;
}

int qbdipreload_on_premain(void *gprCtx, void *fpuCtx) {
    return QBDIPRELOAD_NOT_HANDLED;
}

int qbdipreload_on_main(int argc, char** argv) {
    return QBDIPRELOAD_NOT_HANDLED;
}

int qbdipreload_on_run(QBDI::VMInstanceRef vm, QBDI::rword start, QBDI::rword stop) {
    vm->addCodeCB(QBDI::PREINST, onInstruction, nullptr);
    vm->run(start, stop);
    return QBDIPRELOAD_NO_ERROR;
}

int qbdipreload_on_exit(int status) {
    return QBDIPRELOAD_NO_ERROR;
}

}
