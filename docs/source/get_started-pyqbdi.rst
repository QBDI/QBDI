.. currentmodule:: pyqbdi

Get started with PyQBDI
=======================

PyQBDI is a Python3 binding of QBDI. He can be to script QBDI but has some limitations:

- PyQBDI cannot be used to instrument Python process;
- The performances are lower than C/C++ API;
- The Python runtime must have the same architecture as the target.

Memory Allocation
-----------------

Unless C/C++ API, the memory cannot be allocated, read and write easily in Python.
PyQBDI includes helpers methods to allocate, free, read and write memory.

.. code:: python

    import pyqbdi

    value = b"bytes array"

    addr = pyqbdi.allocateMemory(len(value))
    pyqbdi.writeMemory(addr, value)
    value2 = pyqbdi.readMemory(addr, len(value))
    assert value == value2
    pyqbdi.freeMemory(addr)

Target code loading
-------------------

The first step is to be able to run the target code inside our process. In this tutorial, we
will load a shared library ``mylib.so`` and execute the method ``foo``.

.. code:: python

    import pyqbdi
    import ctypes

    mylib = ctypes.cdll.LoadLibrary("mylib.so")
    funcPtr = ctypes.cast(mylib.foo, ctypes.c_void_p).value


To instrument a whole executable, PyQBDIPreload can be used (see :ref:`get_started-pyqbdipreload`).

Virtual Machine initialization
------------------------------

We then need to initialize the Virtual Machine (:class:`VM`) itself.

.. code:: python

    vm = pyqbdi.VM()

Allocate a virtual stack
------------------------

The virtual machine will not run on the same stack as QBDI. It will need a separate stack to run.
We can allocate and initialize a virtual stack with :func:`pyqbdi.allocateVirtualStack`. This function will
allocate an aligned memory space, set the stack pointer register to the last aligned address of the allocated memory
and return the top address of the aligned memory.

.. code:: python

    state = vm.getGPRState()
    fakestack = pyqbdi.allocateVirtualStack(state, 0x100000)
    assert fakestack != None


Our first callback function
---------------------------

Now that the virtual machine has been set up, we can start playing with QBDI core features.

To have a trace of our execution, we will need a callback that will retrieve the current address
and the disassembly of the instruction and print it.

As the callback will be called on an instruction, the callback must follow the :func:`InstCallback <pyqbdi.InstCallback>` type. Inside the
callback, we can get an :class:`InstAnalysis` of the current instruction with :func:`pyqbdi.VM.getInstAnalysis`.
To have the address and the disassembly, the :class:`InstAnalysis` need to have the type
:data:`pyqbdi.ANALYSIS_INSTRUCTION <pyqbdi.AnalysisType>` (for the address) and
:data:`pyqbdi.ANALYSIS_DISASSEMBLY <pyqbdi.AnalysisType>` (for the disassembly). These
two :data:`pyqbdi.AnalysisType` are the default parameter of :func:`pyqbdi.VM.getInstAnalysis` and
can be omitted.

.. code:: python

    def showInstruction(vm, gpr, fpr, data):
        # Obtain an analysis of the instruction from the VM
        instAnalysis = vm.getInstAnalysis()

        # Printing disassembly
        print("0x{:x}: {}".format(instAnalysis.address, instAnalysis.disassembly))

        return pyqbdi.CONTINUE

An :func:`InstCallback <pyqbdi.InstCallback>` must always return an action (:data:`pyqbdi.VMAction`) to the VM to specify if the execution should
continue or stop. In most cases :data:`CONTINUE <pyqbdi.VMAction>`
should be returned to continue the execution.

Register a Callback
-------------------

The callback must be registered in the VM. The function :func:`pyqbdi.VM.addCodeCB` allows registering
a callback for every instruction. The callback can be registered before the instruction
(:data:`pyqbdi.PREINST <pyqbdi.InstPosition>`) or after the instruction
(:data:`pyqbdi.POSTINST <pyqbdi.InstPosition>`).

.. code:: python

    cid = vm.addCodeCB(pyqbdi.PREINST, showInstruction, None)
    assert cid != pyqbdi.INVALID_EVENTID

The function returns a callback ID or the special ID :data:`pyqbdi.INVALID_EVENTID <pyqbdi.VMError>` if
the registration fails. The callback ID can be kept if you want to unregister the callback later.

Set instrumented range
----------------------

QBDI needs a range of address where the code should be instrumented. If the execution goes out of this range,
QBDI will try to restore an uninstrumented execution.

In our example, we need to include the method in the instrumented range. The method :func:`pyqbdi.VM.addInstrumentedModuleFromAddr`
can be used to add a whole module (binary or library) in the range of instrumentation with a single address of this module.

.. code:: python

    assert vm.addInstrumentedModuleFromAddr(funcPtr)

Run the instrumentation
-----------------------

We can finally run the instrumentation using the :func:`pyqbdi.VM.call` function.
This method will align the stack, push the argument (if needed) and a fake return address and
call the method through QBDI. The execution will stop when the instrumented code returns to the
fake address.

.. code:: python

    asrun, retval = vm.call(funcPtr, [args1, args2])
    assert asrun

:func:`pyqbdi.VM.call` returns if the method has run and the return register value (value of ``RAX`` for X86_64).

If the convention call used by :func:`pyqbdi.VM.call` doesn't match your need, you can initialize the stack yourself
and use :func:`pyqbdi.VM.run`.

End program properly
--------------------

Finally, we need to free allocated stack and exit the virtual machine properly using :func:`pyqbdi.alignedFree`.

.. code:: python

    pyqbdi.alignedFree(fakestack)

Fully working example
---------------------

You can find a fully working example below, based on the precedent explanations. The example traces the ``sin``
method and uses :func:`pyqbdi.simulateCall` and :func:`pyqbdi.VM.run` to push the return address and run the method.

.. include:: ../../examples/pyqbdi/trace_sin.py
   :code:

