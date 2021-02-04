C API
=====

A step-by-step example illustrating a basic (yet powerful) usage of the QBDI C API.

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

Note that if you want to instrument a whole binary, QBDIPreload should be preferred (see :ref:`get_started-preload-c`).

Initialise the virtual machine
------------------------------

First off, we need to initialise the virtual machine (often referred to as VM) itself. The type ``VMInstanceRef`` represents an instance of the VM.
The :cpp:func:`qbdi_initVM` function needs to be called to set the instance up.
The second, third and fourth arguments are used to customise the instance depending on what you want to do.

.. code:: c

    #include "QBDI.h"

    VMInstanceRef vm;
    qbdi_initVM(&vm, NULL, NULL, 0);

Retrieve the VM context
-----------------------

Prior to initialising the virtual stack (see next section),
we need to get a pointer to the virtual machine state (:cpp:type:`GPRState`), which can be obtained by calling :cpp:func:`qbdi_getGPRState`.
This object represents the current VM's context.

.. code:: c

    GPRState *state = qbdi_getGPRState(vm);
    assert(state != NULL);

Allocate a virtual stack
------------------------

The virtual machine does not work with the regular stack that your process uses -- instead, QBDI needs its own stack.
Therefore, we have to ask for a virtual stack using :cpp:func:`qbdi_allocateVirtualStack`. This function is
responsible for allocating an aligned memory space, set the stack pointer register accordingly and return the top address of this brand-new memory region.

.. code:: c

    uint8_t* fakestack;
    bool res = qbdi_allocateVirtualStack(state, STACK_SIZE, &fakestack);
    assert(res == true);


Write our first callback function
---------------------------------

Now that the virtual machine has been set up, we can start playing with QBDI core features.

To have a trace of our execution, we will need a callback that will retrieve the current address
and the disassembly of the instruction and print it.

As the callback will be called on an instruction, the callback must follow the :cpp:type:`InstCallback` type. Inside the
callback, we can get an :cpp:struct:`InstAnalysis` of the current instruction with :cpp:func:`qbdi_getInstAnalysis`.
To have the address and the disassembly, the :cpp:struct:`InstAnalysis` needs to have the type
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

Register a callback
-------------------

The callback must be registered in the VM. The function :cpp:func:`qbdi_addCodeCB` allows registering
a callback for every instruction. The callback can be called before the instruction
(:cpp:enumerator:`QBDI_PREINST <InstPosition::QBDI_PREINST>`) or after the instruction
(:cpp:enumerator:`QBDI_POSTINST <InstPosition::QBDI_POSTINST>`).

.. code:: c

    uint32_t cid = qbdi_addCodeCB(vm, QBDI_PREINST, showInstruction, NULL);
    assert(cid != QBDI_INVALID_EVENTID);

The function returns a callback ID or the special ID :cpp:enumerator:`QBDI_INVALID_EVENTID <VMError::QBDI_INVALID_EVENTID>` if
the registration fails. The callback ID can be kept if you want to unregister the callback later.

Count the iterations
--------------------

With the current implementation of Fibonacci, the function will iterate by recursively calling itself.
Consequently, we can determine the number of iterations the function is doing by counting the number of calls.
:cpp:func:`qbdi_addMnemonicCB` can be used to register a callback which is solely called when encountering specific instructions.
All QBDI callbacks allow users to pass a custom parameter ``data`` of type ``void *``.

.. code:: c

    VMAction countIteration(VMInstanceRef vm, GPRState *gprState, FPRState *fprState, void *data) {
        (*((unsigned*) data))++;

        return QBDI_CONTINUE;
    }

    unsigned iterationCount = 0;
    qbdi_addMnemonicCB(vm, "CALL*", QBDI_PREINST, countIteration, &iterationCount);


Set instrumented ranges
-----------------------

QBDI needs a range of addresses where the code should be instrumented. If the execution goes out of this scope,
QBDI will try to restore an uninstrumented execution.

In our example, we need to include the function we are looking into in the instrumented range. :cpp:func:`qbdi_addInstrumentedModuleFromAddr`
can be used to add a whole module (binary or library) in the range of instrumentation with a single address of this module.

.. code:: c

    res = qbdi_addInstrumentedModuleFromAddr(vm, (rword) &fibonacci);
    assert(res == true);

Run the instrumentation
-----------------------

We can finally run the instrumentation using the :cpp:func:`qbdi_call` function.
It aligns the stack, sets the argument(s) (if needed) and a fake return address and
calls the target function through QBDI. The execution stops when the instrumented code returns to the
fake address.

.. code:: c

    rword retval;
    res = qbdi_call(vm, &retval, (rword) fibonacci, 1, 25);
    assert(res == true);

:cpp:func:`qbdi_call` returns if the function has completely run in the context of QBDI.
The first argument has been filled with the value of the return register (e.g. ``RAX`` for X86_64).

It may turn out that the function does not expect the calling convention :cpp:func:`qbdi_call` uses.
In this precise case, you must set up the proper context and the stack yourself and call :cpp:func:`qbdi_run` afterwards.

Terminate the execution properly
--------------------------------

At last, before exiting, we need to free up the resources we have allocated: both the virtual stack and the virtual machine by respectively calling :cpp:func:`qbdi_alignedFree` and :cpp:func:`qbdi_terminateVM`.

.. code:: c

    qbdi_alignedFree(fakestack);
    qbdi_terminateVM(vm);

Full example
------------

Merging everything we have learnt throughout this tutorial, we are now able to write our C source code file:

.. include:: ../../examples/c/fibonacci.c
   :code:

Generate a template
-------------------

A QBDI template can be considered as a baseline project, a minimal component you can modify and build your instrumentation tool on.
They are provided to help you effortlessly start off a new QBDI based project.
The binary responsible for generating a template is shipped in the release packages and can be used as follows:

.. code:: bash

    mkdir new_project
    cd new_project
    qbdi-template

A template consists of a simple C source code file and a basic CMake build script.
A file called ``README.txt`` is also present, it describes the compilation procedure.
