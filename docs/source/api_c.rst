.. highlight:: c

.. _vm_c:

VM C
====

Initialization
--------------

The :c:func:`qbdi_initVM` function is used to create a new VM instance, using host
CPU as a reference::

    VMInstanceRef vm;
    qbdi_initVM(&vm, NULL, NULL);

.. doxygenfunction:: qbdi_initVM
   :project: QBDI_C

The :c:func:`qbdi_initVM` function takes two optional parameters used to specify
the CPU type or architecture and additional attributes of the CPU. Those are the same used by LLVM.
Here's how you would initialize a VM for an ARM Cortex-A9 supporting the VFP2 floating point
instructions::

   VMInstanceRef vm;
   const char* mattrs[] = {"vfp2"};
   qbdi_initVM(&vm, "cortex-a9", mattrs);


State Management
----------------

The state of the guest processor can be queried and modified using two architectures dependent
structures :c:type:`GPRState` and :c:type:`FPRState` which are detailed below for each architecture
(:ref:`c-state-x86`, :ref:`c-state-x86-64` and :ref:`c-state-arm`).

The :c:func:`qbdi_getGPRState` and :c:func:`qbdi_getFPRState` allow to
query the current state of the guest processor while the :c:func:`qbdi_setGPRState`
and :c:func:`qbdi_setFPRState` allow to modify it::

   GPRState gprState; // local GPRState structure
   gprState = *qbdi_getGPRState(vm); // Copy the VM structure into our local structure
   gprState.rax = (rword) 42; // Set the value of the RAX register
   qbdi_setGPRState(vm, &gprState); // Copy our local structure to the VM structure

As :c:func:`qbdi_getGPRState` and :c:func:`qbdi_getFPRState` return
pointers to the VM context, it is possible to use them to modify the GPR and FPR states
directly. The code below is equivalent to the example from above but avoid unnecessary copies::

   qbdi_getGPRState(vm)->rax = (rword) 42;

.. doxygenfunction:: qbdi_getGPRState
   :project: QBDI_C

.. doxygenfunction:: qbdi_getFPRState
   :project: QBDI_C

.. doxygenfunction:: qbdi_setGPRState
   :project: QBDI_C

.. doxygenfunction:: qbdi_setFPRState
   :project: QBDI_C

.. _c-state-x86:

X86
^^^

The X86 :c:type:`GPRState` structure is the following:

.. include:: ../../include/QBDI/State.h
   :start-after: SPHINX_X86_GPRSTATE_BEGIN
   :end-before: SPHINX_X86_GPRSTATE_END
   :code:

The X86 :c:type:`FPRState` structure is the following:

.. include:: ../../include/QBDI/State.h
   :start-after: SPHINX_X86_FPRSTATE_BEGIN
   :end-before: SPHINX_X86_FPRSTATE_END
   :code:

.. _c-state-x86-64:

X86_64
^^^^^^

The X86_64 :c:type:`GPRState` structure is the following:

.. include:: ../../include/QBDI/State.h
   :start-after: SPHINX_X86_64_GPRSTATE_BEGIN
   :end-before: SPHINX_X86_64_GPRSTATE_END
   :code:

The X86_64 :c:type:`FPRState` structure is the following:

.. include:: ../../include/QBDI/State.h
   :start-after: SPHINX_X86_64_FPRSTATE_BEGIN
   :end-before: SPHINX_X86_64_FPRSTATE_END
   :code:

.. _c-state-arm:

ARM
^^^

The ARM :c:type:`GPRState` structure is the following:

.. include:: ../../include/QBDI/State.h
   :start-after: SPHINX_ARM_GPRSTATE_BEGIN
   :end-before: SPHINX_ARM_GPRSTATE_END
   :code:

The ARM :c:type:`FPRState` structure is the following:

.. include:: ../../include/QBDI/State.h
   :start-after: SPHINX_ARM_FPRSTATE_BEGIN
   :end-before: SPHINX_ARM_FPRSTATE_END
   :code:

State Initialization
^^^^^^^^^^^^^^^^^^^^

Since the instrumented software and the VM need to run on different stacks, helper initialization
functions exist for this purpose. The :c:func:`qbdi_alignedAlloc` function offers a cross-platform
interface for aligned allocation which is required for allocating a stack. The
:c:func:`qbdi_allocateVirtualStack` allows to allocate this stack and also setup the
:c:type:`GPRState` accordingly such that the required registers point to this stack. The
:c:func:`qbdi_simulateCall` allows to simulate the call to a function by modifying the
:c:type:`GPRState` and stack to setup the return address and the arguments. The
:ref:`fibonacci-c` example is using those functions to call a function through the QBDI.

.. doxygenfunction:: qbdi_alignedAlloc
   :project: QBDI_C

.. doxygenfunction:: qbdi_allocateVirtualStack
   :project: QBDI_C

.. doxygenfunction:: qbdi_simulateCall
   :project: QBDI_C


Execution
---------

Starting a Run
^^^^^^^^^^^^^^

The run function is used to start the execution of a piece of code by the DBI. It first sets the
Program Counter of the VM CPU state to the start parameter then runs the code through the DBI
until a control flow instruction branches on the stop parameter. See the :ref:`fibonacci-c`
example for a basic usage of QBDI and the :c:func:`qbdi_run` function.

.. doxygenfunction:: qbdi_run
   :project: QBDI_C

:c:func:`qbdi_run` is a low level function which implies that the :c:type:`GPRState`
has been properly initialized before.

But a very common usage of QBDI is to start instrumentation on a function call.
:c:func:`qbdi_call` is a helper which behaves like a function call.
It takes a function pointer, a list of arguments, and return the call's result.
The only requirement is a valid (and properly aligned) stack pointer.
In most cases, using :c:func:`qbdi_allocateVirtualStack` is enough, as a call
rarely needs to work on the original thread stack.

.. doxygenfunction:: qbdi_call
   :project: QBDI_C

.. _execution-filtering-c:

Execution Filtering
^^^^^^^^^^^^^^^^^^^

For performance reasons but also integrity reasons it is rarely preferable to instrument the
entirety of the guest code. The VM stores a set of instrumented ranges, when the execution gets out
of those ranges the VM attempts to transfer the execution to a real execution via the
:c:type:`ExecBroker` which implements a return address hooking mechanism. As this hooking
process is only a heuristic, it is neither guaranteed to be triggered as soon as the execution gets
out of the set of instrumented ranges nor guaranteed to actually catch the return to the
set of instrumented ranges.

.. warning::
   By default the set of instrumented ranges is empty, a call to :c:func:`qbdi_run` will thus very
   quickly escape from the instrumentation process through an execution transfer made by the
   ExecBroker.

The basic functions to manipulate this set of instrumented ranges are
:c:func:`qbdi_addInstrumentedRange` and :c:func:`qbdi_removeInstrumentedRange`.

.. doxygenfunction:: qbdi_addInstrumentedRange
   :project: QBDI_C


.. doxygenfunction:: qbdi_removeInstrumentedRange
   :project: QBDI_C

As manipulating address ranges can be problematic, helpers API allow to use process memory maps
information. The :c:func:`qbdi_addInstrumentedModule` and :c:func:`qbdi_removeInstrumentedModule` allow
to use executable module names instead of address ranges. The currently loaded module names can be
queried using :c:func:`qbdi_getModuleNames`. The :c:func:`qbdi_addInstrumentedModuleFromAddr` and
:c:func:`qbdi_removeInstrumentedModuleFromAddr` functions allow to use any known address from a module to instrument it.

The :c:func:`qbdi_instrumentAllExecutableMaps` allows to instrument every memory map which is
executable, the undesired address range can later be removed using the previous APIs in a blacklist
approach. This approach is demonstrated in the :ref:`thedude-c` example where sub-functions are
removed in function of a trace level supplied as argument.

The :c:func:`qbdi_removeAllInstrumentedRanges` can be used to remove **all** ranges recorded
through previous APIs (after this call, the VM will have no range left to instrument).

Below is an example of how to obtain, iterate then destroy the module names using the API.

.. include:: ../../examples/modules.c
   :code:

.. doxygenstruct:: qbdi_MemoryMap
   :project: QBDI_C
   :members:

.. doxygenenum:: qbdi_Permission
   :project: QBDI_C

.. doxygenfunction:: qbdi_getCurrentProcessMaps
   :project: QBDI_C

.. doxygenfunction:: qbdi_freeMemoryMapArray
   :project: QBDI_C

.. doxygenfunction:: qbdi_getModuleNames
   :project: QBDI_C

.. doxygenfunction:: qbdi_addInstrumentedModule
   :project: QBDI_C

.. doxygenfunction:: qbdi_removeInstrumentedModule
   :project: QBDI_C

.. doxygenfunction:: qbdi_addInstrumentedModuleFromAddr
   :project: QBDI_C

.. doxygenfunction:: qbdi_removeInstrumentedModuleFromAddr
   :project: QBDI_C

.. doxygenfunction:: qbdi_instrumentAllExecutableMaps
   :project: QBDI_C

.. warning::
   Instrumenting the libc very often results in deadlock problems as it is also used by QBDI.
   It is thus recommended to always exclude it from the instrumentation.

.. doxygenfunction:: qbdi_removeAllInstrumentedRanges
   :project: QBDI_C


Instrumentation
---------------

Instrumentation allows to run additional code alongside the original code of the software. There
are three types of instrumentations implemented by the VM. The first one are instruction event
callbacks where the execution of an instruction, under certain conditions, triggers a callback from
the guest to the host. The second one are VM event callbacks where certain actions taken by the VM
itself trigger a callback. The last one are custom instrumentations which allow to use the
internal instrumentation mechanism of the VM.

Instruction Callback
^^^^^^^^^^^^^^^^^^^^

The instruction callbacks follow the prototype of :c:type:`InstCallback`. A user supplied
data pointer parameter can be passed to the callback. Here's a simple callback which would
increment an integer passed as a pointer through the data parameter::

   VMAction increment(VMInstanceRef vm, GPRState *gprState, FPRState *fprState, void *data) {
       int *counter = (int*) data;
       *counter += 1;
       return QBDI_CONTINUE;
   }

The return value of this function is very important to determine how the VM should handle the
resuming of execution. :cpp:enumerator:`QBDI_CONTINUE` will directly switch back to the
guest code in the current ExecBlock without further processing by the VM. This means that
modification of the Program Counter will not be taken into account and modifications of the program
code will only be effective after the end of the current basic block (provided the code cache is
cleared, which is not currently exported via the API). One can force those changes to be taken into
account by breaking from the ExecBlock execution and returning early inside the VM using the
:cpp:enumerator:`QBDI_BREAK_TO_VM`. :cpp:enumerator:`QBDI_STOP` causes the VM to stop the execution and the
:c:func:`qbdi_run` to return.

The :c:func:`qbdi_addCodeCB` function allows to add a callback on every instruction (inside the
instrumented range).
If we register the previous callback to be called before every instruction, we effectively get
an instruction counting instrumentation::

   int counter = 0;
   qbdi_addCodeCB(vm, QBDI_PREINST, increment, &counter);
   qbdi_run(vm, ...); // Run the VM of some piece of code
   printf("Instruction count: %d\n", counter); // counter contains the number of instructions executed

.. doxygenenum:: InstPosition
   :project: QBDI_C

.. doxygentypedef:: InstCallback
   :project: QBDI_C

.. doxygenenum:: VMAction
   :project: QBDI_C

.. doxygenfunction:: qbdi_addCodeCB
   :project: QBDI_C

It is also possible to register an :c:type:`InstCallback` for a specific instruction address or
address range with the :c:func:`qbdi_addCodeAddrCB` and :c:func:`qbdi_addCodeRangeCB` functions. These
allow to fine-tune the instrumentation to specific codes or even portions of them.

.. doxygenfunction:: qbdi_addCodeAddrCB
   :project: QBDI_C

.. doxygenfunction:: qbdi_addCodeRangeCB
   :project: QBDI_C

.. doxygenfunction:: qbdi_addMnemonicCB
   :project: QBDI_C

.. note:: Mnemonics can be instrumented using LLVM convention (You can register a callback on *ADD64rm* or *ADD64rr* for instance).
.. note:: You can also use "*" as a wildcard. (eg : *ADD\*rr*)



If the execution of an instruction triggers more than one callback, those will be called in the
order they were added to the VM.

Memory Callback
^^^^^^^^^^^^^^^

The memory callbacks (currently only supported under X86_64) allow to trigger callbacks on specific
memory events. They use the same callback prototype, :c:type:`InstCallback`, than
instruction callback. They can be registered using the :c:func:`qbdi_addMemAccessCB` API.
The API takes care of enabling the corresponding inline memory access logging instrumentation using
:c:func:`qbdi_recordMemoryAccess`. The memory accesses themselves are not directly provided
as a callback parameter and need to be retrieved using the memory access APIs
(see :ref:`memory-access-analysis-c`). The :ref:`cryptolock-c` example shows how to use those APIs
to log the memory writes.

The type parameter allows to filter the callback on specific memory access type:

* :cpp:enumerator:`QBDI_MEMORY_READ` triggers the callback **before** every instruction performing a memory read.
* :cpp:enumerator:`QBDI_MEMORY_WRITE` triggers the callback **after** every instruction performing a memory write.
* :cpp:enumerator:`QBDI_MEMORY_READ_WRITE` triggers the callback **after** every instruction performing a memory read and/or write.

.. doxygenfunction:: qbdi_addMemAccessCB
   :project: QBDI_C

To enable more flexibility on the callback filtering :c:func:`qbdi_addMemAddrCB` and
:c:func:`qbdi_addMemRangeCB` allow to filter for memory accesses targeting a specific
memory address or range. However those callbacks are virtual callbacks: they cannot be directly
triggered by the instrumentation because the access address needs to be checked dynamically but
are instead triggered by a gate callback which takes care of the dynamic access address check and
forwards or not the event to the virtual callback. This system thus has the same overhead as
:c:func:`qbdi_addMemAccessCB` and is only meant as an API helper.

.. doxygenfunction:: qbdi_addMemAddrCB
   :project: QBDI_C

.. doxygenfunction:: qbdi_addMemRangeCB
   :project: QBDI_C

VM Events
^^^^^^^^^

VM events are triggered by the VM itself when it takes specific actions related to the execution of
the instrumented program. The callback prototype :c:type:`VMCallback` is different as it is
triggered by the VM and not by an instruction. The callback receives a :c:type:`VMState` structure.
:c:type:`VMCallback` can be registered for several events at the same time by combining several
values of :c:type:`VMEvent` in a mask using the ``|`` binary operator.

.. doxygentypedef:: VMCallback
   :project: QBDI_C

.. doxygenfunction:: qbdi_addVMEventCB
   :project: QBDI_C

.. doxygenstruct:: VMState
   :project: QBDI_C
   :members:
   :undoc-members:

.. doxygenenum:: VMEvent
   :project: QBDI_C


Custom Instrumentation
^^^^^^^^^^^^^^^^^^^^^^

Custom instrumentation is not available via the C API as it requires manipulating internal C++
classes.

Removing Instrumentations
^^^^^^^^^^^^^^^^^^^^^^^^^

The id returned by the previous instrumentation functions can be used to modify the instrumentation
afterward. The :c:func:`qbdi_deleteInstrumentation` function allows to remove an instrumentation by id
while the :c:func:`qbdi_deleteAllInstrumentations` function will remove all instrumentations.
This code would do two different executions with different callbacks::

   uint32_t cb1 = qbdi_addCodeCB(vm, QBDI_PREINST, Callback1, &some_data);
   uint32_t cb2 = qbdi_addVMEventCB(vm, QBDI_EXEC_TRANSFER_CALL | QBDI_BASIC_BLOCK_ENTRY, Callback2, &some_data);
   qbdi_run(vm, ...); // Run the VM of some piece of code with all instrumentations
   qbdi_deleteInstrumentation(vm, cb1);
   qbdi_run(vm, ...); // Run the VM of some piece of code with some instrumentation
   qbdi_deleteAllInstrumentations(vm);
   qbdi_run(vm, ...); // Run the VM of some piece of code without instrumentation

.. doxygenfunction:: qbdi_deleteInstrumentation
   :project: QBDI_C

.. doxygenfunction:: qbdi_deleteAllInstrumentations
   :project: QBDI_C

Instruction Analysis
--------------------

Callback based instrumentation has only limited utility if the current context of execution is not
available. To obtain more information about an instruction, the VM can parse its internal structures
for us and provide analysis results in a :c:type:`InstAnalysis` structure::

   CallbackResult increment(VMInstanceRef vm, GPRState *gprState, FPRState *fprState, void *data) {
       const InstAnalysis *instAnalysis = qbdi_getInstAnalysis(vm, QBDI_ANALYSIS_INSTRUCTION | QBDI_ANALYSIS_DISASSEMBLY);

       printf("%s\n", instAnalysis->disassembly);

       return QBDI_CONTINUE;
   }

.. doxygenfunction:: qbdi_getInstAnalysis
   :project: QBDI_C

Analysis can gather all kinds of properties linked with the instruction.
It's possible to select which one will be retrieved using `type` argument.

.. doxygenenum:: AnalysisType
   :project: QBDI_C

Every analysis has a performance cost, which can be reduced by selecting types carefully,
and is amortized using a cache inside the VM.

.. doxygenstruct:: InstAnalysis
   :project: QBDI_C
   :members:
   :undoc-members:

If provided :c:type:`AnalysisType` is equal to `QBDI_ANALYSIS_OPERANDS`, then
:c:func:`qbdi_getInstAnalysis` will also analyze all instruction operands, and
fill an array of :c:type:`OperandAnalysis` (of length `numOperands`).

.. doxygenstruct:: OperandAnalysis
   :project: QBDI_C
   :members:
   :undoc-members:

.. doxygenenum:: OperandType
   :project: QBDI_C

.. doxygenenum:: OperandFlag
   :project: QBDI_C

.. doxygenenum:: RegisterAccessType
   :project: QBDI_C

.. _memory-access-analysis-c:

Memory Access Analysis
----------------------

QBDI VM can log memory access using inline instrumentation. This adds instrumentation rules
which store the accessed addresses and values into specific memory variables called
*instruction shadows*. To enable this memory logging, the :c:func:`qbdi_recordMemoryAccess`
API can be used. This memory logging is also automatically enabled when calling one of the memory
callbacks.

.. doxygenfunction:: qbdi_recordMemoryAccess
   :project: QBDI_C

The memory access type always refers to either :cpp:enumerator:`QBDI_MEMORY_READ`, :cpp:enumerator:`QBDI_MEMORY_WRITE`,
:cpp:enumerator:`QBDI_MEMORY_READ_WRITE` (which is a bitfield combination of the two previous ones).

Once the logging has been enabled, :c:func:`qbdi_getInstMemoryAccess` and
:c:func:`qbdi_getBBMemoryAccess` can be used to retrieve the memory accesses made by the
last instruction or by the last basic block. These two APIs return :c:func:`qbdi_MemoryAccess`
structures. Write memory accesses are only returned if the instruction has already been executed
(i.e. in the case of a :cpp:enumerator:`QBDI_POSTINST`). The :ref:`cryptolock-c` example
shows how to use those APIs to log the memory writes.


.. doxygenstruct:: MemoryAccess
   :project: QBDI_C
   :members:
   :undoc-members:

.. doxygenenum:: MemoryAccessType
   :project: QBDI_C

.. doxygenfunction:: qbdi_getInstMemoryAccess
   :project: QBDI_C

.. doxygenfunction:: qbdi_getBBMemoryAccess
   :project: QBDI_C


Free resources
--------------

Some resources returned by a VM instance must be manually freed using
specialized functions.
A VM instance itself must be destroyed using :c:func:`qbdi_terminateVM`.

.. doxygenfunction:: qbdi_terminateVM
   :project: QBDI_C

.. doxygenfunction:: qbdi_alignedFree
   :project: QBDI_C


Cache management
----------------

QBDI provides a cache system for basic blocks that you might want to deal directly with it.
There are a few functions that can help you with that.

.. doxygenfunction:: qbdi_precacheBasicBlock
   :project: QBDI_C

.. doxygenfunction:: qbdi_clearCache
   :project: QBDI_C

.. doxygenfunction:: qbdi_clearAllCache
   :project: QBDI_C


Examples
--------

.. _fibonacci-c:

Fibonacci
^^^^^^^^^

This example instruments its own code to count the executed instructions and displays the
disassembly of every instruction executed.

.. include:: ../../examples/fibonacci.c
   :code:

.. _thedude-c:

The Dude
^^^^^^^^

This example instruments its own code to count the executed instructions and displays the
disassembly of every instruction executed. A supplied trace level argument allows to disable the
tracing in specific sub-functions using the APIs presented in :ref:`execution-filtering-c`.

.. include:: ../../examples/thedude.c
   :code:

.. _cryptolock-c:

Cryptolock
^^^^^^^^^^

This example instruments its own code to display instruction performing memory writes and the
written values. This can be used to discover the password of the cryptolock and reveal the message
(hint: if the password is correct then the hash buffer is all zero).

.. include:: ../../examples/cryptolock.c
   :code:
