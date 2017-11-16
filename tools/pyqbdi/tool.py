#!/usr/bin/env python2
## -*- coding: utf-8 -*-

import pyqbdi

def mycb(inst, gpr, fpr):
    print "0x%x: %s" %(inst.address, inst.disassembly)
    return pyqbdi.CONTINUE

if __name__ == '__main__':
    pyqbdi.addCodeCB(pyqbdi.PREINST, mycb)
    pyqbdi.run(pyqbdi.start, pyqbdi.stop)
