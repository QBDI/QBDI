#!/usr/bin/env python3

import sys
import math
import ctypes
import pyqbdi
import struct

def insnCB(vm, gpr, fpr, data):
    instAnalysis = vm.getInstAnalysis()
    print("0x{:x}: {}".format(instAnalysis.address, instAnalysis.disassembly))
    return pyqbdi.CONTINUE


def run():
    # get sin function ptr
    if sys.platform == 'darwin':
        libmname = 'libSystem.dylib'
    elif sys.platform == 'win32':
        libmname = 'api-ms-win-crt-math-l1-1-0.dll'
    else:
        libmname = 'libm.so.6'
    libm = ctypes.cdll.LoadLibrary(libmname)
    funcPtr = ctypes.cast(libm.sin, ctypes.c_void_p).value

    # init VM
    vm = pyqbdi.VM()

    # create stack
    state = vm.getGPRState()
    addr = pyqbdi.allocateVirtualStack(state, 0x100000)
    assert addr is not None

    # instrument library and register memory access
    vm.addInstrumentedModuleFromAddr(funcPtr)
    vm.recordMemoryAccess(pyqbdi.MEMORY_READ_WRITE)

    # add callbacks on instructions
    vm.addCodeCB(pyqbdi.PREINST, insnCB, None)

    # Cast double arg to long and set FPR
    arg = 1.0
    carg = struct.pack('<d', arg)
    fpr = vm.getFPRState()
    fpr.xmm0 = carg

    # call sin(1.0)
    pyqbdi.simulateCall(state, 0x42424242)
    success = vm.run(funcPtr, 0x42424242)

    # Retrieve output FPR state
    fpr = vm.getFPRState()
    # Cast long arg to double
    res = struct.unpack('<d', fpr.xmm0[:8])[0]
    print("%f (python) vs %f (qbdi)" % (math.sin(arg), res))

    # cleanup
    pyqbdi.alignedFree(addr)

if __name__ == "__main__":
    run()
