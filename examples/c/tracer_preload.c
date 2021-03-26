#include <stdio.h>
#include "QBDIPreload.h"

QBDIPRELOAD_INIT;

static VMAction onInstruction(VMInstanceRef vm, GPRState *gprState,
                              FPRState *fprState, void *data) {
  const InstAnalysis *instAnalysis = qbdi_getInstAnalysis(
      vm, QBDI_ANALYSIS_INSTRUCTION | QBDI_ANALYSIS_DISASSEMBLY);
  printf("0x%" PRIRWORD " %s\n", instAnalysis->address,
         instAnalysis->disassembly);
  return QBDI_CONTINUE;
}

int qbdipreload_on_start(void *main) { return QBDIPRELOAD_NOT_HANDLED; }

int qbdipreload_on_premain(void *gprCtx, void *fpuCtx) {
  return QBDIPRELOAD_NOT_HANDLED;
}

int qbdipreload_on_main(int argc, char **argv) {
  return QBDIPRELOAD_NOT_HANDLED;
}

int qbdipreload_on_run(VMInstanceRef vm, rword start, rword stop) {
  qbdi_addCodeCB(vm, QBDI_PREINST, onInstruction, NULL);
  qbdi_run(vm, start, stop);
  return QBDIPRELOAD_NO_ERROR;
}

int qbdipreload_on_exit(int status) { return QBDIPRELOAD_NO_ERROR; }
