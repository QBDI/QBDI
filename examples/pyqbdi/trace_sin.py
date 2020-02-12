#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import math
import ctypes
import pyqbdi
import struct


def vmCB(vm, evt, gpr, fpr, data):
    if evt.event & pyqbdi.BASIC_BLOCK_ENTRY:
        print("[*] Basic Block: 0x{:x} -> 0x{:x}".format(evt.basicBlockStart,
                                                         evt.basicBlockEnd))
    elif evt.event & pyqbdi.BASIC_BLOCK_EXIT:
        for acs in vm.getBBMemoryAccess():
            print("@ {:#x} {:#x}:{:#x}".format(acs.instAddress,
                                               acs.accessAddress, acs.value))
    return pyqbdi.CONTINUE


def insnCB(vm, gpr, fpr, data):
    data['insn'] += 1
    types = pyqbdi.ANALYSIS_INSTRUCTION | pyqbdi.ANALYSIS_DISASSEMBLY
    types |= pyqbdi.ANALYSIS_OPERANDS | pyqbdi.ANALYSIS_SYMBOL
    inst = vm.getInstAnalysis(types)
    print("{};0x{:x}: {}".format(inst.module, inst.address, inst.disassembly))
    for op in inst.operands:
        if op.type == pyqbdi.OPERAND_IMM:
            print("const: {:d}".format(op.value))
        elif op.type == pyqbdi.OPERAND_GPR:
            print("reg: {:s}".format(op.regName))
    return pyqbdi.CONTINUE


def cmpCB(vm, gpr, fpr, data):
    data['cmp'] += 1
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
    udata = {"insn": 0, "cmp": 0}
    vm.addCodeCB(pyqbdi.PREINST, insnCB, udata)
    vm.addMnemonicCB("CMP*", pyqbdi.PREINST, cmpCB, udata)
    vm.addVMEventCB(pyqbdi.BASIC_BLOCK_ENTRY | pyqbdi.BASIC_BLOCK_EXIT,
                    vmCB, None)

    # Cast double arg to long and set FPR
    arg = 1.0
    carg = struct.pack('<d', arg)
    fpr = vm.getFPRState()
    fpr.xmm0 = carg

    # call sin(1.0)
    pyqbdi.simulateCall(state, 0x42424242)
    success = vm.run(funcPtr, 0x42424242)
    print(udata)

    # Retrieve output FPR state
    fpr = vm.getFPRState()
    # Cast long arg to double
    res = struct.unpack('<d', fpr.xmm0[:8])[0]
    print("%f (python) vs %f (qbdi)" % (math.sin(arg), res))

    # cleanup
    pyqbdi.alignedFree(addr)

if __name__ == "__main__":
    run()
