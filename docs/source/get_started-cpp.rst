C++ API
=======

A step-by-step example illustrating a basic (yet powerful) usage of the QBDI C++ API.

Load the target code
--------------------

In this tutorial, we aim at figuring out how many iterations a Fibonacci function is doing.
To do so, we will rely on QBDI to instrument the function.
For convenience sake, its source code is compiled along with the one we are about to write.

.. code:: c

    int fibonacci(int n) {
        if(n <= 2)
            return 1;
        return fibonacci(n-1) + fibonacci(n-2);
    }

However, it's not always the case.
Sometimes, we need to look into a function we don't have the source code of -- that is, it has been already compiled.
As a result, we have to find a way to load the code we want to inspect into our process' memory space.
For instance, if the function of interest is embedded in a dynamic library, we can link our code with this library when compiling
or import it at runtime by calling either ``dlopen`` or ``LoadLibraryA``.

Note that if you want to instrument a whole binary, QBDIPreload should be preferred (see :ref:`get_started-preload-cpp`).

Initialise the virtual machine
------------------------------

First off, we need to initialise the virtual machine (:cpp:class:`QBDI::VM`) itself.

.. code:: cpp

    #include "QBDI.h"

    QBDI::VM vm;

Retrieve the VM context
-----------------------

Prior to initialising the virtual stack (see next section),
we need to get a pointer to the virtual machine state (:cpp:type:`QBDI::GPRState`), which can be obtained by calling :cpp:func:`QBDI::VM::getGPRState`.
This object represents the current VM's context.

.. code:: cpp

    QBDI::GPRState *state = vm.getGPRState();
    assert(state != NULL);

Allocate a virtual stack
------------------------

The virtual machine does not work with the regular stack that your process uses -- instead, QBDI needs its own stack.
Therefore, we have to ask for a virtual stack using :cpp:func:`QBDI::allocateVirtualStack`. This function is
responsible for allocating an aligned memory space, set the stack pointer register accordingly and return the top address of this brand-new memory region.

.. code:: cpp

    uint8_t* fakestack;
    bool res = QBDI::allocateVirtualStack(state, STACK_SIZE, &fakestack);
    assert(res == true);

Our first callback function
---------------------------

Now that the virtual machine has been set up, we can start playing with QBDI core features.

To have a trace of our execution, we will need a callback that will retrieve the current address
and the disassembly of the instruction and print it.

As the callback will be called on an instruction, the callback must follow the :cpp:type:`QBDI::InstCallback` type. Inside the
callback, we can get an :cpp:struct:`QBDI::InstAnalysis` of the current instruction with :cpp:func:`QBDI::VM::getInstAnalysis`.
To have the address and the disassembly, the :cpp:struct:`QBDI::InstAnalysis` need to have the type
:cpp:enumerator:`QBDI::AnalysisType::ANALYSIS_INSTRUCTION` (for the address) and
:cpp:enumerator:`QBDI::AnalysisType::ANALYSIS_DISASSEMBLY` (for the disassembly). These
two :cpp:enum:`QBDI::AnalysisType` are the default parameter of :cpp:func:`QBDI::VM::getInstAnalysis` and
can be omitted.


.. code:: cpp

    QBDI::VMAction showInstruction(QBDI::VM* vm, QBDI::GPRState *gprState, QBDI::FPRState *fprState, void *data) {
        // Obtain an analysis of the instruction from the VM
        const QBDI::InstAnalysis* instAnalysis = vm->getInstAnalysis();

        // Printing disassembly
        std::cout << std::setbase(16) << instAnalysis->address << ": "
                  << instAnalysis->disassembly << std::endl << std::setbase(10);

        return QBDI::VMAction::CONTINUE;
    }

An :cpp:type:`QBDI::InstCallback` must always return an action (:cpp:enum:`QBDI::VMAction`) to the VM to specify if the execution should
continue or stop. In most cases :cpp:enumerator:`QBDI::VMAction::CONTINUE`
should be returned to continue the execution.

Register a callback
-------------------

The callback must be registered in the VM. The function :cpp:func:`QBDI::VM::addCodeCB` allows registering
a callback for every instruction. The callback can be called before the instruction
(:cpp:enumerator:`QBDI::InstPosition::PREINST`) or after the instruction
(:cpp:enumerator:`QBDI::InstPosition::POSTINST`).

.. code:: cpp

    uint32_t cid = vm.addCodeCB(QBDI::PREINST, showInstruction, nullptr);
    assert(cid != QBDI::INVALID_EVENTID);

The function returns a callback ID or the special ID :cpp:enumerator:`QBDI::VMError::INVALID_EVENTID` if
the registration fails. The callback ID can be kept if you want to unregister the callback later.

Count the iterations
--------------------

With the current implementation of Fibonacci, the function will iterate by recursively calling itself.
Consequently, we can determine the number of iterations the function is doing by counting the number of calls.
:cpp:func:`QBDI::VM::addMnemonicCB` can be used to register a callback which is solely called when encountering specific instructions.
All QBDI callbacks allow users to pass a custom parameter ``data`` of type ``void *``.

.. code:: cpp

    QBDI::VMAction countIteration(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState, QBDI::FPRState *fprState, void *data) {
        (*((unsigned*) data))++;

        return QBDI::CONTINUE;
    }

    unsigned iterationCount = 0;
    vm.addMnemonicCB("CALL*", QBDI::PREINST, countIteration, &iterationCount);


Set instrumented ranges
-----------------------

QBDI needs a range of addresses where the code should be instrumented. If the execution goes out of this scope,
QBDI will try to restore an uninstrumented execution.

In our example, we need to include the method in the instrumented range. The method :cpp:func:`QBDI::VM::addInstrumentedModuleFromAddr`
can be used to add a whole module (binary or library) in the range of instrumentation with a single address of this module.

.. code:: cpp

    res = vm.addInstrumentedModuleFromAddr(reinterpret_cast<QBDI::rword>(fibonacci));
    assert(res == true);

Run the instrumentation
-----------------------

We can finally run the instrumentation using the :cpp:func:`QBDI::VM::call` function.
It aligns the stack, sets the argument(s) (if needed) and a fake return address and
calls the target function through QBDI. The execution stops when the instrumented code returns to the
fake address.

.. code:: cpp

    rword retval;
    res = vm.call(&retval, reinterpret_cast<QBDI::rword>(fibonacci), {25});
    assert(res == true);

:cpp:func:`QBDI::VM::call` returns if the function has completely run in the context of QBDI.
The first argument has been filled with the value of the return register (e.g. ``RAX`` for X86_64).

It may turn out that the function does not expect the calling convention :cpp:func:`QBDI::VM::call` uses.
In this precise case, you must set up the proper context and the stack yourself and call :cpp:func:`QBDI::VM::run` afterwards.

Terminate the execution properly
--------------------------------

At last, before exiting, we need to free up the virtual stack we have allocated calling :cpp:func:`QBDI::alignedFree`.

.. code:: cpp

    QBDI::alignedFree(fakestack);

Full example
------------

Merging everything we have learnt throughout this tutorial, we are now able to write our C++ source code file:

.. include:: ../../examples/cpp/fibonacci.cpp
   :code:
