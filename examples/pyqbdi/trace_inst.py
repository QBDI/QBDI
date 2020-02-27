#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import pyqbdi


def mycb(vm, gpr, fpr, data):
    inst = vm.getInstAnalysis()
    print("0x{:x}: {}".format(inst.address, inst.disassembly))
    return pyqbdi.CONTINUE


def pyqbdipreload_on_run(vm, start, stop):
    vm.addCodeCB(pyqbdi.PREINST, mycb, None)
    vm.run(start, stop)
