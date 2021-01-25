Get started with C++ API
========================

A step-by-step illustrating a basic (yet powerful) usage of QBDI C++ APIs.

Target code loading
-------------------

The first step is to be able to run the target code inside our process. In this tutorial, we
will instrument a Fibonacci function that is included in our source code.

.. code:: c

    int fibonacci(int n) {
        if(n <= 2)
            return 1;
        return fibonacci(n-1) + fibonacci(n-2);
    }

If the code to instrument is in a library, we can link your code with the library
or import it at the runtime with ``dlopen`` or ``LoadLibraryA``. To instrument a
whole binary, QBDIPreload can be used (see :ref:`get_started-preload-cpp`).

Virtual Machine initialization
------------------------------

We then need to initialize the Virtual Machine (:cpp:class:`QBDI::VM`) itself.

.. code:: cpp

    #include "QBDI.h"

    QBDI::VM vm;

Pointer to VM context
---------------------

In order to initialize the virtual stack properly (see next section),
we need a pointer to the virtual machine state (:cpp:type:`QBDI::GPRState`), which can be obtained using :cpp:func:`QBDI::VM::getGPRState`.

.. code:: cpp

    QBDI::GPRState *state = vm.getGPRState();
    assert(state != NULL);

Allocate a virtual stack
------------------------

The virtual machine will not run on the same stack as QBDI. It will need a separate stack to run.
We can allocate and initialize a virtual stack with :cpp:func:`QBDI::allocateVirtualStack`. This function will
allocate an aligned memory space, set the stack pointer register to the last aligned address of the allocated memory
and return the top address of the allocated memory.

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

Register a Callback
-------------------

The callback must be registered in the VM. The function :cpp:func:`QBDI::VM::addCodeCB` allows registering
a callback for every instruction. The callback can be registered before the instruction
(:cpp:enumerator:`QBDI::InstPosition::PREINST`) or after the instruction
(:cpp:enumerator:`QBDI::InstPosition::POSTINST`).

.. code:: cpp

    uint32_t cid = vm.addCodeCB(QBDI::PREINST, showInstruction, nullptr);
    assert(cid != QBDI::INVALID_EVENTID);

The function returns a callback ID or the special ID :cpp:enumerator:`QBDI::VMError::INVALID_EVENTID` if
the registration fails. The callback ID can be kept if you want to unregister the callback later.

Count the Iteration
-------------------

With the current implementation of Fibonacci, the function will iterate by calling itself. We can count the iteration
by counted the number of call. :cpp:func:`QBDI::VM::addMnemonicCB` can be used to register a callback for specific instructions.
All QBDI callback allows having a user parameter with the ``void*`` type.

.. code:: cpp

    QBDI::VMAction countIteration(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState, QBDI::FPRState *fprState, void *data) {
        (*((unsigned*) data))++;

        return QBDI::CONTINUE;
    }

    unsigned iterationCount = 0;
    vm.addMnemonicCB("CALL*", QBDI::PREINST, countIteration, &iterationCount);


Set instrumented range
----------------------

QBDI needs a range of address where the code should be instrumented. If the execution goes out of this range,
QBDI will try to restore an uninstrumented execution.

In our example, we need to include the method in the instrumented range. The method :cpp:func:`QBDI::VM::addInstrumentedModuleFromAddr`
can be used to add a whole module (binary or library) in the range of instrumentation with a single address of this module.

.. code:: cpp

    res = vm.addInstrumentedModuleFromAddr(reinterpret_cast<QBDI::rword>(fibonacci));
    assert(res == true);

Run the instrumentation
-----------------------

We can finally run the instrumentation using the :cpp:func:`QBDI::VM::call` function.
This method will align the stack, push the argument (if needed) and a fake return address and
call the method through QBDI. The execution will stop when the instrumented code returns to the
fake address.

.. code:: cpp

    rword retval;
    res = vm.call(&retval, reinterpret_cast<QBDI::rword>(fibonacci), {25});
    assert(res == true);

:cpp:func:`QBDI::VM::call` return if the method has run. His first argument will receive the value of
the return register (``RAX`` for X86_64).

If the convention call used by :cpp:func:`QBDI::VM::call` doesn't match your need, you can initialize the stack yourself
and use :cpp:func:`QBDI::VM::run`.

End program properly
--------------------

Finally, we need to free allocated stack using :cpp:func:`QBDI::alignedFree`.

.. code:: cpp

    QBDI::alignedFree(fakestack);


Fully working example
---------------------

You can find a fully working example below, based on the precedent explanations.

.. include:: ../../examples/cpp/fibonacci.cpp
   :code:
