#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import pyqbdi


def mycb(vm, gpr, fpr):
    inst = vm.getInstAnalysis()
    print "0x%x: %s" % (inst.address, inst.disassembly)
    return pyqbdi.CONTINUE


def pyqbdipreload_on_run(vm, start, stop):
    vm.addCodeCB(pyqbdi.PREINST, mycb)
    vm.run(start, stop)
