API description
===============

This section gives the big picture of all the APIs QBDI offers.
It may help you better understand the way QBDI works and the overall design behind it.

Injection
---------

Instrumentation with QBDI implies having both QBDI and the target program inside the same process.
Three types of injections are currently possible with QBDI.

Create a QBDI program
+++++++++++++++++++++

The first type of injection is to create an executable that uses QBDI and loads the target code.
This injection is available with C, C++ and PyQBDI APIs and should be used if you can compile the
target code inside the client binary or load it as a library.

The main steps of this injection are:

- Load the code (``dlopen``, ``LoadLibrary``, ...)
- Initialise a new QBDI VM
- Select instrumentation ranges
- Allocate a stack for the target process
- Define callbacks
- Call a target method inside the VM

Inject a preload library
++++++++++++++++++++++++

This injection forces the loader to add a custom library inside a new process.
The preloaded library will catch the main address and allow users to instrument it.
This injection should be used if you want to instrument a program from the start
of the ``main`` method to the end.

We provide a small tool called :ref:`QBDIPreload <get_started-qbdipreload>` for doing so on Linux and macOS.
QBDIPreload (and its implementation :ref:`PyQBDIPreload <get_started-pyqbdipreload>` for :ref:`PyQBDI <get_started-pyqbdi>`)
automatically puts a hook on the ``main`` method and accordingly prepares a QBDI VM that can be used out of the box.

Typically, the different stages are:

- Wait for :cpp:func:`qbdipreload_on_main` or :py:func:`pyqbdipreload_on_run` to be called
  with an initialised QBDI VM
- Define callbacks
- Run the ``main`` method inside the VM

Inject with Frida
+++++++++++++++++

Frida works on a wide variety of systems and brings powerful and handy injection mechanisms.
It is why the Frida/QBDI subproject relies on those to inject itself into processes.
On top of that, while developing their scripts, users are free to take advantage of both Frida and QBDI APIs.

Tracing a specific function can be summed up as:

- Inject a script into a process with Frida
- Put callbacks on the target functions
- Wait for the hooks to be triggered
- Create a QBDI VM, synchronise the context and define instrumentation ranges
- Register callbacks
- Remove the instrumentation added by Frida
- Run the function in the context of QBDI
- Restore the Frida hook for potential future calls
- Return the return value obtained from QBDI

.. _instrumentation_range:

Instrumentation ranges
----------------------
- Global APIs: :ref:`C <instrumentation-range-c>`, :ref:`C++ <instrumentation-range-cpp>`, :ref:`PyQBDI <instrumentation-range-pyqbdi>`, :ref:`Frida/QBDI <instrumentation-range-js>`

The Instrumentation Range is a range of addresses where QBDI will intrument the original code.
When the programme counter gets out of this scope, QBDI will try to restore a native and uninstrumented execution.
For performance reasons, it is rarely recommended to instrument all the guest code.
Moreover, you must bear in mind that QBDI shares the same standard library as the guest and some methods are not *reentrant* (more details in :ref:`intro_limitations`).

The current mechanism is implemented by the :cpp:class:`ExecBroker` and only supports external calls out of the instrumentation range.
When the execution gets out of the range, :cpp:class:`ExecBroker` will try to find an address on the stack that is inside the instrumentation range.
If an address is found, it will be replaced by a custom one and the execution is restored without instrumentation. When the process returns to
this address :cpp:class:`ExecBroker` will capture its state and continue the execution at the expected address. If any valid return address is found,
the instrumentation will continue until finding a valid return address.

The following limitations are known:

- The instrumentation range must be at a function level, and if possible, at library level.
  A range that includes only some instructions of a function will produce an unpredictable result.
- When the native instrumentation goes out of the instrumentation range, the only method to restore
  the instrumentation is to return to the modified address. Any other executions of code inside the
  instrumentation range will not be caught (callbacks, ...).
- The current :cpp:class:`ExecBroker` doesn't support any exception mechanism, included the `setjmp/longjmp`.
- The instrumentation range, and QBDI in general, are **not** a security sandbox. The code may
  escape and runs without instrumentation.

The instrumentation ranges can be managed through:

- ``addInstrumentedRange`` and ``removeInstrumentedRange`` to add or remove a specific range of address
- ``addInstrumentedModule`` and ``removeInstrumentedModule`` to add or remove a library/module with his name
- ``addInstrumentedModuleFromAddr`` and ``removeInstrumentedModuleFromAddr`` to add or remove a library/module with one of his addresses
- ``instrumentAllExecutableMaps`` and ``removeAllInstrumentedRanges`` to add or remove all the executable range


Register state
--------------
- Global APIs: :ref:`C <register-state-c>`, :ref:`C++ <register-state-cpp>`, :ref:`PyQBDI <register-state-pyqbdi>`, :ref:`Frida/QBDI <register-state-js>`
- Management APIs: :ref:`C <state-management-c>`, :ref:`C++ <state-management-cpp>`, :ref:`PyQBDI <state-management-pyqbdi>`, :ref:`Frida/QBDI <state-management-js>`

QBDI defines two structures for the registers: ``GPRState`` and ``FPRState``.

- ``GPRState`` contains all the General Purpose registers such as ``rax``, ``rsp``, ``rip`` or ``eflags`` on X86_64.
- ``FPRState`` contains the the Floating Point registers.

Inside a ``InstCallback`` and ``VMCallback``, the current state is passed as a parameter and any change on it will affect the execution.
Outside of a callback, ``GPRState`` and ``FPRState`` can be retrieved and set with ``getGPRState``, ``getFPRState``, ``getGPRState`` and ``getFPRState``.

.. note::

    A modification of the instruction counter (e.g. ``RIP``) in an ``InstCallback`` or a ``VMCallback`` is not effective if ``BREAK_TO_VM`` is not returned.

User callbacks
--------------
- Global APIs: :ref:`C <callback-c>`, :ref:`C++ <callback-cpp>`, :ref:`PyQBDI <callback-pyqbdi>`, :ref:`Frida/QBDI <callback-js>`

QBDI allows users to register callbacks that are called throughout the execution of the target. These callbacks can be used to determine what the program is doing
or to modify its state. Some callbacks must return an action to specify whether the execution should continue or stop or if the context needs to be reevaluated.

All user callbacks must be written in C, C++, Python or JS. However, there are a few limitations:

- As the target registers are saved, the callback can use any register by respecting the standard calling convention of the current platform.
- Some methods of the VM are not *reentrant* and must not be called within the scope of a callback.
  (``run``, ``call``, ``setOptions``, ``precacheBasicBlock``, destructor, copy and move operators)
- The ``BREAK_TO_VM`` action should be returned instead of the ``CONTINUE`` action if the state of the VM is somehow changed. It covers:

  - Add or remove callbacks
  - Modify instrumentation ranges
  - Clear the cache
  - Change the instruction counter register in ``GPRState`` (the other registers can be altered without the need of returning ``BREAK_TO_VM``).


Instruction callbacks
+++++++++++++++++++++
- Global APIs: :ref:`C <instcallback-management-c>`, :ref:`C++ <instcallback-management-cpp>`, :ref:`PyQBDI <instcallback-management-pyqbdi>`, :ref:`Frida/QBDI <instcallback-management-js>`

An instruction callback (``InstCallback``) is a callback that will be called **before** or **after** executing an instruction.
Therefore, an ``InstCallback`` can be inserted at two different positions:

- before the instruction (``PREINST`` -- short for *pre-instruction*)
- after the instruction (``POSTINST`` -- short for *post-instruction*). At this point, the register state has been automatically updated so the instruction counter points to the next code address.

.. note::

    A ``POSTINST`` callback will be called after the instruction and before the next one. If on a call instruction, the callback
    is then called before the first instruction of the called method.

An ``InstCallback`` can be registered for a specific instruction (``addCodeAddrCB``),
any instruction in a specified range (``addCodeRangeCB``) or any instrumented instruction (``addCodeCB``).
The instruction also be targeted by their mnemonic (or LLVM opcode) (``addMnemonicCB``).

VM callbacks
++++++++++++
- ``VMCallback`` APIs: :ref:`C <vmcallback-management-c>`, :ref:`C++ <vmcallback-management-cpp>`, :ref:`PyQBDI <vmcallback-management-pyqbdi>`, :ref:`Frida/QBDI <vmcallback-management-js>`
- ``VMEvent`` APIs: :ref:`C <vmevent-c>`, :ref:`C++ <vmevent-cpp>`, :ref:`PyQBDI <vmevent-pyqbdi>`, :ref:`Frida/QBDI <vmevent-js>`

A ``VMEvent`` callback (``VMCallback``) is a callback that will be called when the VM reaches a specific state. The current supported events are:

- At the beginning and the end of a basic block (``BASIC_BLOCK_ENTRY`` and ``BASIC_BLOCK_EXIT``).
  A basic block in QBDI consists of consecutive instructions that don't change the instruction counter except the last one.
  These events are triggered respectively by ``SEQUENCE_ENTRY`` and ``SEQUENCE_EXIT`` as well.
- At the beginning and the end of a sequence (``SEQUENCE_ENTRY`` and ``SEQUENCE_EXIT``).
  A sequence is a part of a basic block that has been *JIT'd* consecutively. These events should only be used for ``getBBMemoryAccess``.
- When a new uncached basic block is being *JIT'd* (``BASIC_BLOCK_NEW``). This event is also always triggered by ``BASIC_BLOCK_ENTRY`` and ``SEQUENCE_ENTRY``.
- Before and after executing some uninstrumented code with the :cpp:class:`ExecBroker` (``EXEC_TRANSFER_CALL`` and ``EXEC_TRANSFER_RETURN``).

When a ``VMCallback`` is called, a state of the VM (``VMState``) is passed in argument. This state contains:

- A set of events that trigger the callback. If the callback is registered for several events that trigger at the same moment,
  the callback will be called only once.
- If the event is related to a basic block or a sequence, the start and the end addresses of the current basic block and sequence are provided.

Memory callbacks
++++++++++++++++
- Global APIs: :ref:`C <memorycallback-management-c>`, :ref:`C++ <memorycallback-management-cpp>`, :ref:`PyQBDI <memorycallback-management-pyqbdi>`, :ref:`Frida/QBDI <memorycallback-management-js>`

The memory callback is an ``InstCallback`` that will be called when the target program reads or writes the memory.
The callback can be called only when a specific address is accessed (``addMemAddrCB``),
when a range of address is accessed (``addMemRangeCB``) or when any memory is accessed (``addMemAccessCB``).

Unlike with the instruction callback registration, the position of a memory callback cannot be manually specified.
If a memory callback is solely registered for read accesses, it will be called **before** the instruction.
Otherwise, it will be called **after** executing the instruction.

Instrumentation rule callbacks
++++++++++++++++++++++++++++++
- Global APIs: :ref:`C <instrrulecallback-management-c>`, :ref:`C++ <instrrulecallback-management-cpp>`, :ref:`PyQBDI <instrrulecallback-management-pyqbdi>`, :ref:`Frida/QBDI <instrrulecallback-management-js>`

Instrumentation rule callbacks are an advanced feature of QBDI. It allows users to define a callback (``InstrRuleCallback``) that will be called during the instrumentation process.
The callback will be called for each instruction and can define an ``InstCallback`` to call before or after the current instruction.
An argument contains an ``InstAnalysis`` of the current instruction and can be used to define the callback to insert for this instruction.

An ``InstrRuleCallback`` can be registered for all instructions (``addInstrRule``) or only for a specific range (``addInstrRuleRange``).

.. note::

    The instrumentation process of QBDI responsible of *JITing* instructions may analyse more than once the same instruction.
    Consequently, the instrumentation rule callback must always return the same result even though the instruction has already been instrumented.

Instruction analysis
--------------------
- Global APIs: :ref:`C <instanalysis-c>`, :ref:`C++ <instanalysis-cpp>`, :ref:`PyQBDI <instanalysis-pyqbdi>`, :ref:`Frida/QBDI <instanalysis-js>`
- Getter APIs: :ref:`C <instanalysis-getter-c>`, :ref:`C++ <instanalysis-getter-cpp>`, :ref:`PyQBDI <instanalysis-getter-pyqbdi>`, :ref:`Frida/QBDI <instanalysis-getter-js>`

QBDI provides some basic analysis of the instruction through the ``InstAnalysis`` object. Within an ``InstCallback``, the analysis of the current instruction should be retrieved with
``getInstAnalysis``. Otherwise, the analysis of any instruction in the cache can be obtained with ``getCachedInstAnalysis``. The ``InstAnalysis`` is cached inside QBDI and
is valid until the next cache modification (add new instructions, clear cache, ...).

Four types of analysis are available. If a type of analysis is not selected, the corresponding field of the ``InstAnalysis`` object remains empty and should not be used.

- ``ANALYSIS_INSTRUCTION``: This analysis type provides some generic information about the instruction, like its address, its size, its mnemonic (LLVM opcode)
  or its condition type if the instruction is conditional.
- ``ANALYSIS_DISASSEMBLY``: This analysis type provides the disassembly of the instruction. For X86 and X86_64, the syntax Intel is used by default.
  The syntax can be changed with the option ``OPT_ATT_SYNTAX``.
- ``ANALYSIS_OPERANDS``: This analysis type provides information about the operand of the instruction.
  An operand can be a register or an immediate. If a register operand can be empty, the special type ``OPERAND_INVALID`` is used.
  The implicit register of instruction is also present with a specific flag.
  Moreover, the member ``flagsAccess`` specifies whether the instruction will use or set the generic flag.
- ``ANALYSIS_SYMBOL``: This analysis type detects whether a symbol is associated with the current instruction.

The source file ``test/API/InstAnalysisTest_<arch>.cpp`` shows how one can deal with instruction analysis and may be taken as a reference for the ``ANALYSIS_INSTRUCTION`` and ``ANALYSIS_OPERANDS`` types.

Memory accesses
---------------
- Global APIs: :ref:`C <memaccess-c>`, :ref:`C++ <memaccess-cpp>`, :ref:`PyQBDI <memaccess-pyqbdi>`, :ref:`Frida/QBDI <memaccess-js>`
- Getter APIs: :ref:`C <memaccess-getter-c>`, :ref:`C++ <memaccess-getter-cpp>`, :ref:`PyQBDI <memaccess-getter-pyqbdi>`, :ref:`Frida/QBDI <memaccess-getter-js>`

Due to performance considerations, the capture of memory accesses (``MemoryAccess``) is not enabled by default.
It is only turned on when a memory callback is registered or explicitly requested with ``recordMemoryAccess``.
Collecting read and written accesses can be enabled either together or separately.

Two APIs can be used to get the memory accesses:

- ``getInstMemoryAccess`` can be used within an instruction callback or a memory callback to retrieve the access of the current instruction.
  If the callback is before the instruction (``PREINST``), only read accesses will be available.
- ``getBBMemoryAccess`` must be used in a ``VMEvent`` callback with ``SEQUENCE_EXIT`` to get all the memory accesses for the last sequence.

Both return a list of ``MemoryAccess``. Generally speaking, a ``MemoryAccess`` will have the address of the instruction responsible of the access,
the access address and size, the type of access and the value read or written. However, some instructions can do complex accesses and
some information can be missing or incomplete. The ``flags`` of ``MemoryAccess`` can be used to detect these cases:

- ``MEMORY_UNKNOWN_SIZE``: the size of the access is unknown.
  This is currently used for instruction with ``REP`` prefix before the execution of the instruction.
  The size is determined after the instruction when the access has been completed.
- ``MEMORY_MINIMUM_SIZE``: The size of the access is a minimum size. The access is complex but at least ``size`` of memory is accessed.
  This is currently used for the ``XSAVE*`` and ``XRSTOR*`` instructions.
- ``MEMORY_UNKNOWN_VALUE``: The value of the access hasn't been captured. This flag will be used when the access size is greater than the size of a ``rword``.
  It's also used for instructions with ``REP`` in ``X86`` and ``X86_64``.


Options
-------

The ``options`` of the VM allow changing some internal mechanisms of QBDI. For most uses of QBDI, no option needs to be specified.
The options are specified when the VM is created and can be changed with ``setOptions`` when the VM is not running. After changing the options,
the cache needs to be cleared to apply the changes to all the instructions.

- ``OPT_DISABLE_FPR``: This option disables the Floating Point Registers support. QBDI will not back up and restore any ``FPRState`` registers.
- ``OPT_DISABLE_OPTIONAL_FPR``: if ``OPT_DISABLE_FPR`` is not enabled, this option will force the ``FPRState`` to be restored and saved
  before and after any instruction. By default, QBDI will try to detect the instructions that make use of floating point registers and only restore for
  these precise instructions.
- ``OPT_ATT_SYNTAX``: For X86 and X86_64 architectures, this option changes
  the syntax of ``InstAnalysis.disassembly`` to AT&T instead of the Intel one.
