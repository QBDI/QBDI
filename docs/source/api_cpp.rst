.. highlight:: c++

VM C++
======

Initialization
--------------

The :cpp:func:`QBDI::VM::VM` constructor is used to create a new VM instance, using host
CPU as a reference::

    QBDI::VM *vm = new QBDI::VM();

.. doxygenfunction:: QBDI::VM::VM

The :cpp:func:`QBDI::VM::VM` constructor can also take two optional parameters used to specify 
the CPU type or architecture and additional attributes of the CPU. Those are the same used by LLVM. 
Here's how you would initialize a VM for an ARM Cortex-A9 supporting the VFP2 floating point 
instructions::

    QBDI::VM *vm = new QBDI::VM("cortex-a9", {"vfp2"});

State Management
----------------

The state of the guest processor can be queried and modified using two architectures dependent structures :cpp:class:`GPRState` and :cpp:class:`FPRState` which are detailed below for each architecture 
(:ref:`cpp-state-x86-64` and :ref:`cpp-state-arm`).

The :cpp:func:`QBDI::VM::getGPRState` and :cpp:func:`QBDI::VM::getFPRState` allow to 
query the current state of the guest processor while the :cpp:func:`QBDI::VM::setGPRState` 
and :cpp:func:`QBDI::VM::setFPRState` allow to modify it::

   QBDI::GPRState gprState; // local GPRState structure
   gprState = *(vm->getGPRState()); // Copy the vm structure into our local structure
   gprState.rax = (QBDI::rword) 42; // Set the value of the RAX register
   vm->setGPRState(&gprState); // Copy our local structure to the vm structure

As :cpp:func:`QBDI::VM::getGPRState` and :cpp:func:`QBDI::VM::getFPRState` return 
pointers to the VM context, it is possible to use them to modify the GPR and FPR states 
directly. The code below is equivalent to the example from above but avoid unnecessary copies::

   vm->getGPRState()->rax = (QBDI::rword) 42;

.. doxygenfunction:: QBDI::VM::getGPRState

.. doxygenfunction:: QBDI::VM::getFPRState

.. doxygenfunction:: QBDI::VM::setGPRState

.. doxygenfunction:: QBDI::VM::setFPRState

.. _cpp-state-x86-64:

X86_64
^^^^^^

The X86_64 :cpp:class:`QBDI::GPRState` structure is the following:

.. include:: ../../include/QBDI/State.h
   :start-after: SPHINX_X86_64_GPRSTATE_BEGIN
   :end-before: SPHINX_X86_64_GPRSTATE_END
   :code:

The X86_64 :cpp:class:`FPRState` structure is the following:

.. include:: ../../include/QBDI/State.h
   :start-after: SPHINX_X86_64_FPRSTATE_BEGIN
   :end-before: SPHINX_X86_64_FPRSTATE_END
   :code:

.. _cpp-state-arm:

ARM
^^^

The ARM :cpp:class:`GPRState` structure is the following:

.. include:: ../../include/QBDI/State.h
   :start-after: SPHINX_ARM_GPRSTATE_BEGIN
   :end-before: SPHINX_ARM_GPRSTATE_END
   :code:

The ARM :cpp:class:`FPRState` structure is the following:

.. include:: ../../include/QBDI/State.h
   :start-after: SPHINX_ARM_FPRSTATE_BEGIN
   :end-before: SPHINX_ARM_FPRSTATE_END
   :code:


State Initialization
^^^^^^^^^^^^^^^^^^^^

Since the instrumented software and the VM need to run on different stacks, helper initialization 
functions exist for this purpose. The :cpp:func:`QBDI::alignedAlloc` function offers a cross-platform 
interface for aligned allocation which is required for allocating a stack. The 
:cpp:func:`QBDI::allocateVirtualStack` allows to allocate this stack and also setup the 
:cpp:class:`QBDI::GPRState` accordingly such that the required registers point to this stack. The 
:cpp:func:`QBDI::simulateCall` allows to simulate the call to a function by modifying the 
:cpp:class:`QBDI::GPRState` and stack to setup the return address and the arguments. The 
:ref:`fibonacci-cpp` example is using those functions to call a function through the QBDI.

.. doxygenfunction:: QBDI::alignedAlloc
   :project: QBDI_CPP

.. doxygenfunction:: QBDI::allocateVirtualStack
   :project: QBDI_CPP

.. doxygenfunction:: QBDI::simulateCall
   :project: QBDI_CPP


Execution
---------

Starting a Run
^^^^^^^^^^^^^^

The :cpp:func:`QBDI::VM::run` method is used to start the execution of a piece of code by the 
DBI. It first sets the Program Counter of the VM CPU state to the start parameter then runs the 
code through the DBI until a control flow instruction branches on the stop parameter. See the 
:ref:`fibonacci-cpp` example for a basic usage of QBDI and the :cpp:func:`QBDI::VM::run` 
method.

.. doxygenfunction:: QBDI::VM::run

:cpp:func:`QBDI::VM::run` is a low level method which implies that the :cpp:class:`QBDI::GPRState`
has been properly initialized before.

But a very common usage of QBDI is to start instrumentation on a function call.
The :cpp:func:`QBDI::VM::call` is a helper method which behaves like a function call.
It takes a function pointer, a list of arguments, and return the call's result.
The only requirement is a valid (and properly aligned) stack pointer.
In most cases, using :cpp:func:`QBDI::allocateVirtualStack` is enough, as a call
rarely needs to work on the original thread stack.

.. doxygenfunction:: QBDI::VM::call
   :project: QBDI_CPP

.. _execution-filtering:

Execution Filtering
^^^^^^^^^^^^^^^^^^^

For performance reasons but also integrity reasons it is rarely preferable to instrument the 
entirety of the guest code. The VM stores a set of instrumented ranges, when the execution gets out 
of those ranges the VM attempts to transfer the execution to a real execution via the
:cpp:class:`QBDI::ExecBroker` which implements a return address hooking mechanism. As this hooking 
process is only a heuristic, it is neither guaranteed to be triggered as soon as the execution gets 
out of the set of instrumented ranges nor guaranteed to actually catch the return to the 
set of instrumented ranges.

.. warning::
   By default the set of instrumented ranges is empty, a call to :cpp:func:`QBDI::VM::run` 
   will thus very quickly escape from the instrumentation process through an execution transfer 
   made by the :cpp:class:`QBDI::ExecBroker`.

The basic methods to manipulate this set of instrumented ranges are 
:cpp:func:`QBDI::VM::addInstrumentedRange` and :cpp:func:`QBDI::VM::removeInstrumentedRange`.

.. doxygenfunction:: QBDI::VM::addInstrumentedRange
   :project: QBDI_CPP


.. doxygenfunction:: QBDI::VM::removeInstrumentedRange
   :project: QBDI_CPP

As manipulating address ranges can be problematic, helpers API allow to use process memory maps 
information. The :cpp:func:`QBDI::VM::addInstrumentedModule` and 
:cpp:func:`QBDI::VM::removeInstrumentedModule` allow to use executable module names instead 
of address ranges. The currently loaded module names can be queried using 
:cpp:func:`QBDI::getModuleNames`. The :cpp:func:`QBDI::VM::addInstrumentedModuleFromAddr`
and :cpp:func:`QBDI::VM::removeInstrumentedModuleFromAddr` methods allow to use any known
address from a module to instrument it.

The :cpp:func:`QBDI::VM::instrumentAllExecutableMaps` allows to instrument every memory map 
which is executable, the undesired address range can later be removed using the previous APIs in a 
blacklist approach. This approach is demonstrated in the :ref:`thedude-cpp` example where 
sub-functions are removed in function of a trace level supplied as argument.

The :cpp:func:`QBDI::VM::removeAllInstrumentedRanges` can be used to remove **all** ranges recorded
through previous APIs (after this call, the VM will have no range left to instrument).

Below is an example of how to obtain, iterate then print the module names using the API.

.. include:: ../../examples/modules.cpp
   :code:

.. doxygenclass:: QBDI::MemoryMap
   :project: QBDI_CPP
   :members:

.. doxygenenum:: QBDI::Permission
   :project: QBDI_CPP

.. doxygenclass:: QBDI::Range
   :project: QBDI_CPP
   :members:

.. doxygenfunction:: QBDI::getCurrentProcessMaps
   :project: QBDI_CPP

.. doxygenfunction:: QBDI::getModuleNames
   :project: QBDI_CPP

.. doxygenfunction:: QBDI::VM::addInstrumentedModule

.. doxygenfunction:: QBDI::VM::removeInstrumentedModule

.. doxygenfunction:: QBDI::VM::addInstrumentedModuleFromAddr

.. doxygenfunction:: QBDI::VM::removeInstrumentedModuleFromAddr

.. doxygenfunction:: QBDI::VM::instrumentAllExecutableMaps 

.. warning::
   Instrumenting the libc very often results in deadlock problems as it is also used by QBDI. 
   It is thus recommended to always exclude it from the instrumentation.

.. doxygenfunction:: QBDI::VM::removeAllInstrumentedRanges


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

The instruction callbacks follow the prototype of :cpp:type:`QBDI::InstCallback`. A user 
supplied data pointer parameter can be passed to the callback. Here's a simple callback which would 
increment an integer passed as a pointer through the data parameter::

   QBDI::VMAction increment(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState, QBDI::FPRState *fprState, void *data) {
       int *counter = (int*) data;
       *counter += 1;
       return QBDI::VMAction::CONTINUE;
   } 

The return value of this method is very important to determine how the VM should handle the 
resuming of execution. :cpp:enumerator:`CONTINUE` will directly switch back to the 
guest code in the current ExecBlock without further processing by the VM. This means that 
modification of the Program Counter will not be taken into account and modifications of the program 
code will only be effective after the end of the current basic block (provided the code cache is 
cleared, which is not currently exported via the API). One can force those changes to be taken into 
account by breaking from the ExecBlock execution and returning early inside the VM using the 
:cpp:enumerator:`BREAK_TO_VM`. :cpp:enumerator:`STOP` is not yet implemented.

The :cpp:func:`QBDI::VM::addCodeCB` method allows to add a callback on every instruction (inside the
instrumented range).
If we register the previous callback to be called before every instruction, we effectively get
an instruction counting instrumentation::

   int counter = 0;
   vm->addCodeCB(QBDI::PREINST, increment, &counter);
   vm->run(...); // Run the VM of some piece of code
   // counter contains the number of instructions executed
   std::cout << "Instruction count: " << counter << std::endl;

.. doxygenenum:: QBDI::InstPosition

.. doxygentypedef:: QBDI::InstCallback

.. doxygenenum:: QBDI::VMAction

.. doxygenfunction:: QBDI::VM::addCodeCB

It is also possible to register an :cpp:type:`QBDI::InstCallback` for a specific instruction 
address or address range with the :cpp:func:`QBDI::VM::addCodeAddrCB` and 
:cpp:func:`QBDI::VM::addCodeRangeCB` methods. These allow to fine-tune the instrumentation to 
specific codes or portions of them.

.. doxygenfunction:: QBDI::VM::addCodeAddrCB

.. doxygenfunction:: QBDI::VM::addCodeRangeCB

.. doxygenfunction:: QBDI::VM::addMnemonicCB

.. note:: Mnemonics can be instrumented using LLVM convention (You can register a callback on *ADD64rm* or *ADD64rr* for instance).
.. note:: You can also use "*" as a wildcard. (eg : *ADD\*rr*)



If the execution of an instruction triggers more than one callback, those will be called in the 
order they were added to the VM.

Memory Callback
^^^^^^^^^^^^^^^

The memory callbacks (currently only supported under X86_64) allow to trigger callbacks on specific 
memory events. They use the same callback prototype, :cpp:type:`QBDI::InstCallback`, than 
instruction callback. They can be registered using the :cpp:func:`QBDI::VM::addMemAccessCB` API.
The API takes care of enabling the corresponding inline memory access logging instrumentation using 
:cpp:func:`QBDI::VM::recordMemoryAccess`. The memory accesses themselves are not directly provided 
as a callback parameter and need to be retrieved using the memory access APIs (see 
:ref:`memory-access-analysis-cpp`). The :ref:`cryptolock-cpp` example shows how to use those APIs 
to log the memory writes.


The type parameter allows to filter the callback on specific memory access type:

* :cpp:enumerator:`QBDI::MEMORY_READ` triggers the callback **before** every instruction performing a memory read.
* :cpp:enumerator:`QBDI::MEMORY_WRITE` triggers the callback **after** every instruction performing a memory write.
* :cpp:enumerator:`QBDI::MEMORY_READ_WRITE` triggers the callback **after** every instruction performing a memory read and/or write.

.. doxygenfunction:: QBDI::VM::addMemAccessCB

To enable more flexibility on the callback filtering, :cpp:func:`QBDI::VM::addMemAddrCB` and 
:cpp:func:`QBDI::VM::addMemRangeCB` allow to filter for memory accesses targeting a specific 
memory address or range. However those callbacks are virtual callbacks: they cannot be directly 
triggered by the instrumentation because the access address needs to be checked dynamically but 
are instead triggered by a gate callback which takes care of the dynamic access address check and 
forwards or not the event to the virtual callback. This system thus has the same overhead as 
:cpp:func:`QBDI::VM::addMemAccessCB` and is only meant as an API helper.

.. doxygenfunction:: QBDI::VM::addMemAddrCB

.. doxygenfunction:: QBDI::VM::addMemRangeCB

VM Events
^^^^^^^^^

VM events are triggered by the VM itself when it takes specific actions related to the execution of 
the instrumented program. The callback prototype :cpp:type:`QBDI::VMCallback` is different as it is 
triggered by the VM and not by an instruction. The callback receives a :cpp:class:`QBDI::VMState` structure. 
:cpp:type:`QBDI::VMCallback` can be registered for several events at the same time by combining 
several values of :cpp:type:`QBDI::VMEvent` in a mask using the ``|`` binary operator.

.. doxygentypedef:: QBDI::VMCallback

.. doxygenfunction:: QBDI::VM::addVMEventCB

.. doxygenstruct:: QBDI::VMState
   :members:
   :undoc-members:

.. doxygenenum:: QBDI::VMEvent


Custom Instrumentation
^^^^^^^^^^^^^^^^^^^^^^

Custom instrumentation can be pushed by inserting your own :cpp:class:`QBDI::InstrRule` in the 
VM. This requires using private headers to define your own rules but allows a very high level 
of flexibility.

.. doxygenfunction:: QBDI::VM::addInstrRule

Removing Instrumentations
^^^^^^^^^^^^^^^^^^^^^^^^^


Several callbacks can be registered for the same event and will run in the order they were added.
The id returned by this method can be used to modify the callback afterward. The 
:cpp:func:`QBDI::VM::deleteInstrumentation` method allows to remove an instrumentation by id 
while the :cpp:func:`QBDI::VM::deleteAllInstrumentations` method will remove all 
instrumentations. This code would do two different executions with different callbacks::

   uint32_t cb1 = vm->addCodeCB(QBDI::InstPosition::PREINST, Callback1, &some_data);
   uint32_t cb2 = vm->addVMEventCB(QBDI::VMEvent::EXEC_TRANSFER_CALL | QBDI::VMEvent::BASIC_BLOCK_ENTRY, Callback2, &some_data);
   vm->run(...); // Run the VM of some piece of code with all instrumentations
   vm->deleteInstrumentation(cb1);
   vm->run(...); // Run the VM of some piece of code with some instrumentation
   vm->deleteAllInstrumentations();
   vm->run(...); // Run the VM of some piece of code without instrumentation

.. doxygenfunction:: QBDI::VM::deleteInstrumentation

.. doxygenfunction:: QBDI::VM::deleteAllInstrumentations

Instruction Analysis
--------------------

Callback based instrumentation has only limited utility if the current context of execution is not 
available. 
To obtain more information about an instruction, the VM can parse its internal structures
for us and provide analysis results in a :cpp:class:`QBDI::InstAnalysis` structure::

   QBDI::CallbackResult increment(QBDI::VMInstanceRef vm, QBDI::GPRState *gprState, QBDI::FPRState *fprState, void *data) {
       const QBDI::InstAnalysis *instAnalysis = vm->getInstAnalysis();

       std::cout << instAnalysis->disassembly << std::endl;

       return QBDI::CallbackResult::CONTINUE;
   } 

.. doxygenfunction:: QBDI::VM::getInstAnalysis

Analysis can gather all kinds of properties linked with the instruction.
By default, instructions basic properties (address, mnemonic, ...), as well
as disassembly, are returned (as this is a very common case).
It's also possible to select which properties will be retrieved using `type` argument.

.. doxygenenum:: AnalysisType
   :project: QBDI_CPP

Every analysis has a performance cost, which can be reduced by selecting types carefully,
and is amortized using a cache inside the VM.

.. doxygenstruct:: QBDI::InstAnalysis
   :members:
   :undoc-members:

If provided :cpp:type:`QBDI::AnalysisType` is equal to `QBDI::ANALYSIS_OPERANDS`, then
:cpp:func:`QBDI::getInstAnalysis` will also analyze all instruction operands, and
fill an array of :cpp:type:`QBDI::OperandAnalysis` (of length `numOperands`).

.. doxygenstruct:: QBDI::OperandAnalysis
   :members:
   :undoc-members:

.. doxygenenum:: OperandType
   :project: QBDI_CPP

.. doxygenenum:: RegisterAccessType
   :project: QBDI_CPP

.. _memory-access-analysis-cpp:

Memory Access Analysis
----------------------

QBDI VM can log memory access using inline instrumentation. This adds instrumentation rules 
which store the accessed addresses and values into specific memory variables called 
*instruction shadows*. To enable this memory logging, the :cpp:func:`QBDI::VM::recordMemoryAccess` 
API can be used. This memory logging is also automatically enabled when calling one of the memory 
callbacks.

.. doxygenfunction:: QBDI::VM::recordMemoryAccess

The memory access type always refers to either :cpp:enumerator:`QBDI::MEMORY_READ`, 
:cpp:enumerator:`QBDI::MEMORY_WRITE`, :cpp:enumerator:`QBDI::MEMORY_READ_WRITE` (which is a bit field combination 
of the two previous ones).

Once the logging has been enabled, :cpp:func:`QBDI::VM::getInstMemoryAccess` and 
:cpp:func:`QBDI::VM::getBBMemoryAccess` can be used to retrieve the memory accesses made by the 
last instruction or by the last basic block. These two APIs return :cpp:class:`QBDI::MemoryAccess` 
structures. Write memory accesses are only returned if the instruction has already been executed 
(i.e. in the case of a :cpp:enumerator:`QBDI::InstPosition::POSTINST`). The :ref:`cryptolock-cpp` 
example shows how to use those APIs to log the memory writes.


.. doxygenstruct:: QBDI::MemoryAccess
   :members:
   :undoc-members:

.. doxygenenum:: QBDI::MemoryAccessType

.. doxygenfunction:: QBDI::VM::getInstMemoryAccess

.. doxygenfunction:: QBDI::VM::getBBMemoryAccess


Cache management
----------------

QBDI provides a cache system for basic blocks that you might want to deal directly with it.
There are a few functions that can help you with that.

.. doxygenfunction:: QBDI::VM::precacheBasicBlock
   :project: QBDI_CPP

.. doxygenfunction:: QBDI::VM::clearCache
   :project: QBDI_CPP

.. doxygenfunction:: QBDI::VM::clearAllCache
   :project: QBDI_CPP


Free resources
--------------

Some resources returned by a VM instance must be manually freed using
specialized functions.
A VM instance itself must be destroyed using `delete`.

.. doxygenfunction:: QBDI::alignedFree


Examples
--------

.. _fibonacci-cpp:

Fibonacci
^^^^^^^^^

This example instruments its own code to count the executed instructions and displays the 
disassembly of every instruction executed.

.. include:: ../../examples/fibonacci.cpp
   :code:

.. _thedude-cpp:

The Dude
^^^^^^^^

This example instruments its own code to count the executed instructions and displays the 
disassembly of every instruction executed. A supplied trace level argument allows to disable the 
tracing in specific sub-functions using the APIs presented in :ref:`execution-filtering`.

.. include:: ../../examples/thedude.cpp
   :code:

.. _cryptolock-cpp:

Cryptolock
^^^^^^^^^^

This example instruments its own code to display instruction performing memory writes and the 
written values. This can be used to discover the password of the cryptolock and reveal the message 
(hint: if the password is correct then the hash buffer is all zero). 

.. include:: ../../examples/cryptolock.cpp
   :code:

Reference
---------

.. doxygenclass:: QBDI::VM
   :members:
   :outline:
   :no-link:
