Get started with C API
======================

A step-by-step illustrating a basic (yet powerful) usage of QBDI C APIs.

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
whole binary, QBDIPreload can be used (see :ref:`get_started-preload-c`).

Virtual Machine Initialization
------------------------------

We then need to initialize the Virtual Machine (VM) itself. For the C API, the obscure type
``VMInstanceRef`` represents an instance of the VM.
The method :cpp:func:`qbdi_initVM` needs to be called to initialize the instance. The second, third and fourth parameters
can be used to customize the instance.

.. code:: c

    #include "QBDI.h"

    VMInstanceRef vm;
    qbdi_initVM(&vm, NULL, NULL, 0);

Pointer to VM context
---------------------

In order to initialize the virtual stack properly (see next section),
we need a pointer to the virtual machine state (:cpp:type:`GPRState`), which can be obtained using :cpp:func:`qbdi_getGPRState`.

.. code:: c

    GPRState *state = qbdi_getGPRState(vm);
    assert(state != NULL);

Allocate a virtual stack
------------------------

The virtual machine will not run on the same stack as QBDI. It will need a separate stack to run.
We can allocate and initialize a virtual stack with :cpp:func:`qbdi_allocateVirtualStack`. This function will
allocate an aligned memory space, set the stack pointer register to the last aligned address of the allocated memory
and return the top address of the allocated memory.

.. code:: c

    uint8_t* fakestack;
    bool res = qbdi_allocateVirtualStack(state, STACK_SIZE, &fakestack);
    assert(res == true);


Our first callback function
---------------------------

Now that the virtual machine has been set up, we can start playing with QBDI core features.

To have a trace of our execution, we will need a callback that will retrieve the current address
and the disassembly of the instruction and print it.

As the callback will be called on an instruction, the callback must follow the :cpp:type:`InstCallback` type. Inside the
callback, we can get an :cpp:struct:`InstAnalysis` of the current instruction with :cpp:func:`qbdi_getInstAnalysis`.
To have the address and the disassembly, the :cpp:struct:`InstAnalysis` need to have the type
:cpp:enumerator:`QBDI_ANALYSIS_INSTRUCTION <AnalysisType::QBDI_ANALYSIS_INSTRUCTION>` (for the address) and
:cpp:enumerator:`QBDI_ANALYSIS_DISASSEMBLY <AnalysisType::QBDI_ANALYSIS_DISASSEMBLY>` (for the disassembly).


.. code:: c

    VMAction showInstruction(VMInstanceRef vm, GPRState *gprState, FPRState *fprState, void *data) {
        // Obtain an analysis of the instruction from the VM
        const InstAnalysis* instAnalysis = qbdi_getInstAnalysis(vm, QBDI_ANALYSIS_INSTRUCTION | QBDI_ANALYSIS_DISASSEMBLY);

        // Printing disassembly
        printf("0x%" PRIRWORD ": %s\n", instAnalysis->address, instAnalysis->disassembly);

        return QBDI_CONTINUE;
    }

An :cpp:type:`InstCallback` must always return an action (:cpp:enum:`VMAction`) to the VM to specify if the execution should
continue or stop. In most cases :cpp:enumerator:`QBDI_CONTINUE <VMAction::QBDI_CONTINUE>`
should be returned to continue the execution.

Register a Callback
-------------------

The callback must be registered in the VM. The function :cpp:func:`qbdi_addCodeCB` allows registering
a callback for every instruction. The callback can be registered before the instruction
(:cpp:enumerator:`QBDI_PREINST <InstPosition::QBDI_PREINST>`) or after the instruction
(:cpp:enumerator:`QBDI_POSTINST <InstPosition::QBDI_POSTINST>`).

.. code:: c

    uint32_t cid = qbdi_addCodeCB(vm, QBDI_PREINST, showInstruction, NULL);
    assert(cid != QBDI_INVALID_EVENTID);

The function returns a callback ID or the special ID :cpp:enumerator:`QBDI_INVALID_EVENTID <VMError::QBDI_INVALID_EVENTID>` if
the registration fails. The callback ID can be kept if you want to unregister the callback later.

Count the Iteration
-------------------

With the current implementation of Fibonacci, the function will iterate by calling itself. We can count the iteration
by counted the number of call. :cpp:func:`qbdi_addMnemonicCB` can be used to register a callback for specific instructions.
All QBDI callback allows having a user parameter with the ``void*`` type.

.. code:: c

    VMAction countIteration(VMInstanceRef vm, GPRState *gprState, FPRState *fprState, void *data) {
        (*((unsigned*) data))++;

        return QBDI_CONTINUE;
    }

    unsigned iterationCount = 0;
    qbdi_addMnemonicCB(vm, "CALL*", QBDI_PREINST, countIteration, &iterationCount);


Set instrumented range
----------------------

QBDI needs a range of address where the code should be instrumented. If the execution goes out of this range,
QBDI will try to restore an uninstrumented execution.

In our example, we need to include the method in the instrumented range. The method :cpp:func:`qbdi_addInstrumentedModuleFromAddr`
can be used to add a whole module (binary or library) in the range of instrumentation with a single address of this module.

.. code:: c

    res = qbdi_addInstrumentedModuleFromAddr(vm, (rword) &fibonacci);
    assert(res == true);

Run the instrumentation
-----------------------

We can finally run the instrumentation using the :cpp:func:`qbdi_call` function.
This method will align the stack, push the argument (if needed) and a fake return address and
call the method through QBDI. The execution will stop when the instrumented code returns to the
fake address.

.. code:: c

    rword retval;
    res = qbdi_call(vm, &retval, (rword) fibonacci, 1, 25);
    assert(res == true);

:cpp:func:`qbdi_call` return if the method has run. His first argument will receive the value of
the return register (``RAX`` for X86_64).

If the convention call used by :cpp:func:`qbdi_call` doesn't match your need, you can initialize the stack yourself
and use :cpp:func:`qbdi_run`.

End program properly
--------------------

Finally, we need to free allocated stack and exit the virtual machine properly using :cpp:func:`qbdi_alignedFree` and :cpp:func:`qbdi_terminateVM`.

.. code:: c

    qbdi_alignedFree(fakestack);
    qbdi_terminateVM(vm);


Fully working example
---------------------

You can find a fully working example below, based on the precedent explanations.

.. include:: ../../examples/c/fibonacci.c
   :code:

Template
--------

QBDI templates are included in packages. To begin a new project or test your installation, you can run the follow commands:

.. code:: bash

    $ mkdir new_project
    $ cd new_project
    $ qbdi-template

The template consists of a sample annotated source code with a basic CMake build script.
A ``README`` is also provided with simple steps to compile the template.

