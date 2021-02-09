#!/usr/bin/env python3

import pyqbdi

def insnCB(vm, gpr, fpr, data):
    instAnalysis = vm.getInstAnalysis()
    print("0x{:x}: {}".format(instAnalysis.address, instAnalysis.disassembly))
    return pyqbdi.CONTINUE


def pyqbdipreload_on_run(vm, start, stop):
    vm.addCodeCB(pyqbdi.PREINST, showInstruction, None)
    vm.run(start, stop)

