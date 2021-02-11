.. currentmodule:: pyqbdi

.. _get_started-pyqbdi:

PyQBDI
======

PyQBDI brings Python3 bindings over the QBDI API.
That way, you can take advantage of the QBDI features directly from your Python scripts without bothering using C/C++.
It may be pretty useful if you need to build something quickly.
However, it introduces some limitations:

- PyQBDI cannot be used to instrument a Python process
- The performances are poorer than when using the C/C++ APIs
- The Python runtime's and the target's architectures must be the same

Memory allocation
-----------------

Unlike the C/C++ APIs, interacting with the process' memory is much more complicated while in Python -- that is, memory regions cannot be allocated, read or written.
Luckily, PyQBDI offers helpers to allow users perform these actions.

.. code:: python

    import pyqbdi

    value = b"bytes array"

    addr = pyqbdi.allocateMemory(len(value))
    pyqbdi.writeMemory(addr, value)
    value2 = pyqbdi.readMemory(addr, len(value))
    assert value == value2
    pyqbdi.freeMemory(addr)

Load the target code
--------------------

In this tutorial, we aim at executing the ``foo`` function which lies in a shared library whose name is ``mylib.so``, in the context of QBDI.
PyQBDI will give us a hand doing so.

.. code:: python

    import pyqbdi
    import ctypes

    mylib = ctypes.cdll.LoadLibrary("mylib.so")
    funcPtr = ctypes.cast(mylib.foo, ctypes.c_void_p).value

Note that if you want to instrument a whole binary, PyQBDIPreload should be preferred (see :ref:`get_started-pyqbdipreload`).

Initialise the virtual machine
------------------------------

First off, we need to initialise the virtual machine (:class:`VM`) itself.
Calling the :func:`pyqbdi.VM` is needed to craft a new instance.

.. code:: python

    vm = pyqbdi.VM()

Allocate a virtual stack
------------------------

The virtual machine does not work with the regular stack that your process uses -- instead, QBDI needs its own stack.
Therefore, we have to ask for a virtual stack using :func:`pyqbdi.allocateVirtualStack`. This function is
responsible for allocating an aligned memory space, set the stack pointer register accordingly and return the top address of this brand-new memory region.

.. code:: python

    state = vm.getGPRState()
    fakestack = pyqbdi.allocateVirtualStack(state, 0x100000)
    assert fakestack != None


Write our first callback function
---------------------------------

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

Register a callback
-------------------

The callback must be registered in the VM. The function :func:`pyqbdi.VM.addCodeCB` allows registering
a callback for every instruction. The callback can be called before the instruction
(:data:`pyqbdi.PREINST <pyqbdi.InstPosition>`) or after the instruction
(:data:`pyqbdi.POSTINST <pyqbdi.InstPosition>`).

.. code:: python

    cid = vm.addCodeCB(pyqbdi.PREINST, showInstruction, None)
    assert cid != pyqbdi.INVALID_EVENTID

The function returns a callback ID or the special ID :data:`pyqbdi.INVALID_EVENTID <pyqbdi.VMError>` if
the registration fails. The callback ID can be kept if you want to unregister the callback later.

Set instrumented ranges
-----------------------

QBDI needs a range of addresses where the code should be instrumented. If the execution goes out of this scope,
QBDI will try to restore an uninstrumented execution.

In our example, we need to include the method in the instrumented range. The method :func:`pyqbdi.VM.addInstrumentedModuleFromAddr`
can be used to add a whole module (binary or library) in the range of instrumentation with a single address of this module.

.. code:: python

    assert vm.addInstrumentedModuleFromAddr(funcPtr)

Run the instrumentation
-----------------------

We can finally run the instrumentation using the :func:`pyqbdi.VM.call` function.
It aligns the stack, sets the argument(s) (if needed) and a fake return address and
calls the target function through QBDI. The execution stops when the instrumented code returns to the
fake address.

.. code:: python

    asrun, retval = vm.call(funcPtr, [args1, args2])
    assert asrun

:func:`pyqbdi.VM.call` returns if the function has completely run in the context of QBDI.
The first argument has been filled with the value of the return register (e.g. ``RAX`` for X86_64).

It may turn out that the function does not expect the calling convention :func:`pyqbdi.VM.call` uses.
In this precise case, you must set up the proper context and the stack yourself and call :func:`pyqbdi.VM.run` afterwards.

Terminate the execution properly
--------------------------------

At last, before exiting, we need to free up the virtual stack we have allocated calling :func:`pyqbdi.alignedFree`.

.. code:: python

    pyqbdi.alignedFree(fakestack)

Full example
------------

Merging everything we have learnt throughout this tutorial, we are now able to solve real problems.
For instance, the following example shows how one can generate an execution trace of the ``sin`` function by using a PyQBDI script:

.. include:: ../../examples/pyqbdi/trace_sin.py
   :code:

