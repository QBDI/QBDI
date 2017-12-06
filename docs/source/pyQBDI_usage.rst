PyQBDI Usage
============

Simple example
--------------

Using python bindings alllows you to process information from the instrumentation in python.
Here is a simple example, where we define a callback set on all instructions that displays their address and dissassembly.

.. code-block:: python

    #!/usr/bin/env python2
    # -*- coding: utf-8 -*-

    import pyqbdi

    def mycb(vm, gpr, fpr, data):
        inst = vm.getInstAnalysis()
        print "0x%x: %s" % (inst.address, inst.disassembly)
        return pyqbdi.CONTINUE

    def pyqbdipreload_on_run(vm, start, stop):
        vm.addCodeCB(pyqbdi.PREINST, mycb, None)
        vm.run(start, stop)


How to run it ?
---------------

To be able to run pyQBDI you need to preload **libpyqbdi** library (using either **LD_PRELOAD** or **DYLD_INSERT_LIBRARIES**).
You also need to specify the path to your python tool using the environment variable **PYQBDI_TOOL**.

.. code-block:: bash

    LD_PRELOAD=/usr/local/lib/libpyqbdi.so PYQBDI_TOOL=./example.py /usr/bin/id

.. note:: We will soon provide a loader script, making the use of pyQBDI easier.
    
An in-depth example
-------------------

If the previous example was not enough, here is an extensive test code showcasing current pyqbdi capatabilities.
This example define multiple callbacks, based on events, instructions, and mnemonic. Those are instrumenting the **sin()** native function and its result is being compared with the python result.

.. code-block:: python

    #!/usr/bin/env python2
    # -*- coding: utf-8 -*-

    import sys
    import math
    import ctypes
    import pyqbdi


    def vmCB(vm, evt, gpr, fpr, data):
        if evt.event == pyqbdi.BASIC_BLOCK_ENTRY:
            print "[*] Basic Block: 0x%x -> 0x%x" % \
                    (evt.basicBlockStart, evt.basicBlockEnd)
        elif evt.event == pyqbdi.BASIC_BLOCK_EXIT:
            for acs in vm.getBBMemoryAccess():
                print "@ {:#x} {:#x}:{:#x}".format(acs.instAddress,
                                                   acs.accessAddress, acs.value)


    def insnCB(vm, gpr, fpr, data):
        data['insn'] += 1
        types = pyqbdi.ANALYSIS_INSTRUCTION | pyqbdi.ANALYSIS_DISASSEMBLY
        types |= pyqbdi.ANALYSIS_OPERANDS | pyqbdi.ANALYSIS_SYMBOL
        inst = vm.getInstAnalysis(types)
        print "%s;0x%x: %s" % (inst.module, inst.address, inst.disassembly)
        for op in inst.operands:
            if op.type == pyqbdi.OPERAND_IMM:
                print("const: {:d}".format(op.value))
            elif op.type == pyqbdi.OPERAND_GPR:
                print("reg: {:s}".format(op.regName))
        return pyqbdi.CONTINUE


    def cmpCB(vm, gpr, fpr, data):
        data['cmp'] += 1
        return pyqbdi.CONTINUE


    def pyqbdipreload_on_run(vm, start, stop):
        # get sin function ptr
        libcname = 'libSystem.dylib' if sys.platform == 'darwin' else 'libm.so.6'
        libc = ctypes.cdll.LoadLibrary(libcname)
        funcPtr = ctypes.cast(libc.sin, ctypes.c_void_p).value
        # init VM
        vm.addVMEventCB(pyqbdi.BASIC_BLOCK_ENTRY | pyqbdi.BASIC_BLOCK_EXIT,
                        vmCB, None)
        state = vm.getGPRState()
        success, addr = pyqbdi.allocateVirtualStack(state, 0x100000)
        vm.addInstrumentedModuleFromAddr(funcPtr)
        vm.recordMemoryAccess(pyqbdi.MEMORY_READ_WRITE)
        # add callbacks on instructions
        udata = {"insn": 0, "cmp": 0}
        iid = vm.addCodeCB(pyqbdi.PREINST, insnCB, udata)
        iid2 = vm.addMnemonicCB("CMP*", pyqbdi.PREINST, cmpCB, udata)
        # Cast double arg to long
        arg = 1.0
        carg = pyqbdi.encodeFloat(arg)
        # set FPR argument
        fpr = vm.getFPRState()
        fpr.xmm0 = carg
        vm.setFPRState(fpr)
        pyqbdi.simulateCall(state, 0x42424242)
        vm.setGPRState(state)
        # call sin(1.0)
        success = vm.run(funcPtr, 0x42424242)
        print(udata)
        # Retrieve output FPR state
        fpr = vm.getFPRState()
        # Cast long arg to double
        res = pyqbdi.decodeFloat(fpr.xmm0)
        print("%f (python) vs %f (qbdi)" % (math.sin(arg), res))
        vm.deleteInstrumentation(iid)
        vm.deleteInstrumentation(iid2)