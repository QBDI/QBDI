API Description
===============

This part will introduce the different elements of the API of QBDI for the 4 bindings.

Injection
---------

The first step to instrument a program with QBDI is to inject QBDI and the target program inside the same process.
Three types of injections are currently possible with QBDI.

Create a QBDI program
++++++++++++++++++++++

The first type of injection is to create an executable that uses QBDI and loads the target code.
This injection is available with C, C++ and PyQBDI API and should be used if you can compile the
target code inside the client binary, or you can load it as a library.

The main steps of this injection are:

- Load the code (``dlopen``, ``LoadLibrary``, ...)
- Initialize a new VM
- Select the Instrumentation Range
- Allocate a stack for the target process
- Define your callbacks
- Call a target method inside the VM

Inject a preload library
++++++++++++++++++++++++

This injection forces the loader to add a custom library inside a new process.
The preloaded library will catch the main address and allow to instrument it.
This injection should be used if you want to instrument a program from the start
of the ``main`` method to the end.

We propose a small tool QBDIPreload for Linux and MacOS to perform the interposition.
QBDIPreload (and his implementation PyQBDIPreload for PyQBDI) will intercept the main
method and create a VM ready to run. The main step will be:

- Wait that :cpp:func:`qbdipreload_on_run` or :py:func:`pyqbdipreload_on_run` are called
  with an initialized VM
- Define your callbacks
- Run the ``main`` method inside the VM

Injection with Frida
++++++++++++++++++++

The Frida-QBDI binding can be used to inject any process with Frida. Once a process is injected,
you may use Frida to hook any method to analyze through QBDI.

The main steps of this injection are:

- Inject a process with Frida and hook the target method
- Wait a hook to trigger
- Create a VM, convert set the register state and the instrumentation range
- Add your callbacks
- Unregistered Frida hook (to restore the original code)
- Run QBDI
- Restore Frida hook
- Return from the current hook

.. _instrumentation_range:

Instrumentation Range
---------------------
VM Instrumentation Range API: :ref:`C <instrumentation-range-c>`, :ref:`C++ <instrumentation-range-cpp>`, :ref:`PyQBDI <instrumentation-range-pyqbdi>`, :ref:`Frida/QBDI <instrumentation-range-js>`

For performance reasons it is rarely preferable to instrument all guest code. Moreover, QBDI
uses the same standard library that the guest and some methods are not reentrant. The Instrumentation Range
is a range of address where QBDI will intrument the original code. When the current pointer get out of this range,
QBDI will try to restore a native and uninstrumented execution.

The current mechanism is implemented by the :cpp:class:`ExecBroker` and only support external call out of the instrumentation range.
When the execution gets out of the range :cpp:class:`ExecBroker` will try to find an address on the stack that is inside the instrumentation range.
If an address is found, it will be replaced by a custom one and the execution is restored without instrumentation. When the process returns to
this address :cpp:class:`ExecBroker` will capture his state and continue the execution at the expected address. If any valid return address is found,
the instrumentation will continue until found a valid return address.

The followed limitations are known for the :cpp:class:`ExecBroker` :

- The instrumentation range must be at a function level, and if possible, at library level.
  A range that includes only some instructions of a function will produce an unpredictable result.
- When the native instrumentation goes out of the instrumentation range, the only method to restore
  the instrumentation is to return to the modified address. Any other executions of code inside the
  instrumentation range will not be caught (callbacks, ...).
- The current ExecBroker doesn't support any exception mechanism, included the `setjmp/longjmp`.
- The instrumentation range, and QBDI in general, are **not** a security sandbox. The code may
  escape and runs without instrumentation.

The instrumentation range can be managed with:

- ``addInstrumentedRange`` and ``removeInstrumentedRange`` to add or remove a specific range of address
- ``addInstrumentedModule`` and ``removeInstrumentedModule`` to add or remove a library/module with his name
- ``addInstrumentedModuleFromAddr`` and ``removeInstrumentedModuleFromAddr`` to add or remove a library/module with one of his addresses
- ``instrumentAllExecutableMaps`` and ``removeAllInstrumentedRanges`` to add or remove all the executable range


Registers state
---------------
State API: :ref:`C <register-state-c>`, :ref:`C++ <register-state-cpp>`, :ref:`PyQBDI <register-state-pyqbdi>`, :ref:`Frida/QBDI <register-state-js>`
VM State Management API: :ref:`C <state-management-c>`, :ref:`C++ <state-management-cpp>`, :ref:`PyQBDI <state-management-pyqbdi>`, :ref:`Frida/QBDI <state-management-js>`

QBDI defines two structures for the registers: ``GPRState`` and ``FPRState``.

- ``GPRState`` contains all General Purpose registers like ``rax``, ``rsp``, ``rip`` or ``eflags`` on X86_64.
- ``FPRState`` contains the Floating Point registers.

Inside a ``InstCallback`` and ``VMCallback``, the current state is passed in parameters and any change will affect the execution.
Outside of a callback, ``GPRState`` and ``FPRState`` can be retrieved and set with ``getGPRState``, ``getFPRState``, ``getGPRState`` and ``getFPRState``.

.. note::

    A modification of the instruction counter (``RIP``) in a ``InstCallback`` or ``VMCallback`` isn't effective if ``BREAK_TO_VM`` isn't returned.

User Callbacks
--------------
Callback API: :ref:`C <callback-c>`, :ref:`C++ <callback-cpp>`, :ref:`PyQBDI <callback-pyqbdi>`, :ref:`Frida/QBDI <callback-js>`

QBDI allows the user to introduce callbacks that will be called during the execution of the target. Theses callbacks can be used to trace the program,
or to modify his state. Some callback must return an action to specify if the execution should continue, stop or the context must be reevaluated.

All users callbacks must be written in C, C++, Python or JS in function of the API used. They are few limitations:

- As the target registers are saved, the callback can use any register by respecting the standard calling convention of the current platform.
- Some methods of the VM isn't reentrant and mustn't be called during a callback.
  (``run``, ``call``, ``setOptions``, ``precacheBasicBlock``, destructor, copy and move operators)
- The ``BREAK_TO_VM`` action should be returning instead of ``CONTINUE`` action if the state of the VM is changed. That includes :

  - Add or remove callbacks
  - change instrumentation range
  - clear the cache
  - change instruction counter register in ``GPRState`` (the other registers can be changed without ``BREAK_TO_VM``).


Instruction Callback
++++++++++++++++++++
VM InstCallback API: :ref:`C <instcallback-management-c>`, :ref:`C++ <instcallback-management-cpp>`, :ref:`PyQBDI <instcallback-management-pyqbdi>`, :ref:`Frida/QBDI <instcallback-management-js>`

An instruction callback (``InstCallback``) is a callback that will be called before or after the execution of an instruction.
An ``InstCallback`` can be inserted at two positions:

- before the instruction (``PREINST``);
- after the instruction (``POSTINST``). The register state is updated and the instruction counter has the next instruction address.

.. note::

    A ``POSTINST`` callback will be called after the instruction and before the next one. In case of a call instruction, the callback
    will be called before the first instruction of the called method.

A ``InstCallback`` can be registered for a specific instruction (``addCodeAddrCB``),
any instruction in a specified range (``addCodeRangeCB``) or any instrumented instruction (``addCodeCB``).
The instruction also be targeted by their mnemonic (or LLVM opcode) (``addMnemonicCB``).

VMEvent Callback
++++++++++++++++
VM VMCallback API: :ref:`C <vmcallback-management-c>`, :ref:`C++ <vmcallback-management-cpp>`, :ref:`PyQBDI <vmcallback-management-pyqbdi>`, :ref:`Frida/QBDI <vmcallback-management-js>`
VMEvent API: :ref:`C <vmevent-c>`, :ref:`C++ <vmevent-cpp>`, :ref:`PyQBDI <vmevent-pyqbdi>`, :ref:`Frida/QBDI <vmevent-js>`

A VMEvent callback (``VMCallback``) is a callback that will be called when the VM reaches some state. The current supported events are:

- At the begin and the end of a basicBlock (``BASIC_BLOCK_ENTRY`` and ``BASIC_BLOCK_EXIT``).
  A basicBlock in QBDI is formed by consecutive instruction that doesn't change the instruction counter except the last one.
  These events are always reach with ``SEQUENCE_ENTRY`` and ``SEQUENCE_EXIT``.
- At the begin and the end of a sequence (``SEQUENCE_ENTRY`` and ``SEQUENCE_EXIT``).
  A sequence is a part of a basicBlock that has been JIT consecutively. The event should only be used for the API ``getBBMemoryAccess``.
- When a new un-cached basicBlock has been JIT (``BASIC_BLOCK_NEW``). This event is always reach with ``BASIC_BLOCK_ENTRY`` and ``SEQUENCE_ENTRY``.
- Before and after executing uninstrumented code with the :cpp:class:`ExecBroker` (``EXEC_TRANSFER_CALL`` and ``EXEC_TRANSFER_RETURN``).

When a ``VMCallback`` is called, a state of the VM (``VMState``) is passed in argument. This state contains:

- A set of events that triggers the callback. If the callback is registered for many events that trigger at the same moment,
  the callback will be called only once.
- If the event is related to a BasicBlock or a Sequence, the start and end address of the current BasicBlock and sequence are provided.

Memory Callback
+++++++++++++++
VM MemoryCallback API: :ref:`C <memorycallback-management-c>`, :ref:`C++ <memorycallback-management-cpp>`, :ref:`PyQBDI <memorycallback-management-pyqbdi>`, :ref:`Frida/QBDI <memorycallback-management-js>`

The memory callback is an InstCallback that will be called when the target program reads or writes the memory.
The callback can be called only when a specific address is accessed (``addMemAddrCB``),
when a range of address is accessed (``addMemRangeCB``) or when any memory is accessed (``addMemAccessCB``).

Unlike Instruction Callback registration, the position of Memory Callback cannot be specified. If a memory callback is registered only for read access,
it will be called before the instruction. Otherwise, it will be called after the instruction.

Instrument Callback
+++++++++++++++++++
VM InstrumentCallback API: :ref:`C <instrumentcallback-management-c>`, :ref:`C++ <instrumentcallback-management-cpp>`, :ref:`PyQBDI <instrumentcallback-management-pyqbdi>`, :ref:`Frida/QBDI <instrumentcallback-management-js>`

Instrument callbacks is an advanced API of QBDI. It allows the user to define a callback (``InstrumentCallback``) that will be called during the instrumentation process.
The callback will be called for each instruction and can define ``InstCallback`` to call before or after the current instruction.
An argument contains an ``InstAnalysis`` of the current instruction and can be used to define the callback to insert for this instruction.

An ``InstrumentCallback`` can be defined for all instruction (``addInstrRule``) or for only a specified range (``addInstrRuleRange``).

.. note::

    The instrumentation process of QBDI that JIT the instruction may analyze many times the same instruction.
    The instrument callback must return the same result if the same instruction is instrument many times.

Instruction Analysis
--------------------
InstAnalysis API: :ref:`C <instanalysis-c>`, :ref:`C++ <instanalysis-cpp>`, :ref:`PyQBDI <instanalysis-pyqbdi>`, :ref:`Frida/QBDI <instanalysis-js>`
VM InstAnalysis getter API: :ref:`C <instanalysis-getter-c>`, :ref:`C++ <instanalysis-getter-cpp>`, :ref:`PyQBDI <instanalysis-getter-pyqbdi>`, :ref:`Frida/QBDI <instanalysis-getter-js>`

QBDI provides some limited analysis of the instruction with the object ``InstAnalysis``. During a ``InstCallback``, the analysis of the current instruction should be retrieved with
``getInstAnalysis``. Otherwise, the analysis of any instruction in the cache can be retrieved with ``getCachedInstAnalysis``. The InstAnalysis is cached inside QBDI and
is valid until the next modification of the cache (add new instruction, clear cache, ...).

Four types of analysis are available. If a type of analysis isn't selected, the corresponded fields of the ``InstAnalysis`` are empty and should not be used.

- ``ANALYSIS_INSTRUCTION``: This analysis type provides some generic information about the instruction, like its address, its size, its mnemonic (LLVM opcode)
  or its conditions type if the instruction is conditional.
- ``ANALYSIS_DISASSEMBLY``: This analysis type provides the disassembly of the instruction. For X86 and X86_64, the syntax Intel is used by default.
  The syntax can be changed with the option ``OPT_ATT_SYNTAX``.
- ``ANALYSIS_OPERANDS``: This analysis type provides information about the operand of the instruction.
  An operand can be a register or an immediate. If a register operand can be empty, the special type ``OPERAND_INVALID`` is used.
  The implicit register of instruction is also present with a specific flag.
  Moreover, the member ``flagsAccess`` defines if the instruction will use or set the generic flag.
- ``ANALYSIS_SYMBOL``: This analysis type detects if a symbol is associated with the current instruction.

Some instruction analysis is tested in the file ``test/API/InstAnalysisTest_<arch>.cpp`` and may be used as a reference for the ``ANALYSIS_INSTRUCTION`` and ``ANALYSIS_OPERANDS`` type.

Memory Accesses
---------------
MemoryAccess API: :ref:`C <memaccess-c>`, :ref:`C++ <memaccess-cpp>`, :ref:`PyQBDI <memaccess-pyqbdi>`, :ref:`Frida/QBDI <memaccess-js>`
VM MemoryAccess getter API: :ref:`C <memaccess-getter-c>`, :ref:`C++ <memaccess-getter-cpp>`, :ref:`PyQBDI <memaccess-getter-pyqbdi>`, :ref:`Frida/QBDI <memaccess-getter-js>`

Due to performances, the capture of memory accesses (``MemoryAccess``) is not enabled by default.
The capture is enabled when a Memory Callback is registered, or explicitly with ``recordMemoryAccess``.
The capture of read access and written access can be enabled together or separately.

Two APIs can be used to retrieve the accesses:

- ``getInstMemoryAccess`` can be used in an Instruction Callback or a Memory Callback to retrieve the access of the current instruction.
  If the callback is before the instruction (``PREINST``), only read access will be available.
- ``getBBMemoryAccess`` must be used in a VMEvent Callback with ``SEQUENCE_EXIT`` to get all the memory accesses for the last sequence.

These two API returns a list of ``MemoryAccess``. For most of them, a ``MemoryAccess`` will have the address of the instruction that does the access,
the access address and size, the type of access and the value read or written. However, some instruction can do complex accesses and
some information can be missing or incomplete. The ``flags`` of ``MemoryAccess`` can be used to detect these cases:

- ``MEMORY_UNKNOWN_SIZE``: the size of the access is unknown.
  This is currently used for instruction with ``REP`` prefix before the execution of the instruction.
  The size is determined after the instruction when the access is completed.
- ``MEMORY_MINIMUM_SIZE``: The size of the access is a minimum size. The access is complex but at least ``size`` of memory is accessed.
  This is currently used for ``XSAVE*`` and ``XRSTOR*`` instructions.
- ``MEMORY_UNKNOWN_VALUE``: The value of the access hasn't been captured. This flag will be used when the access size is greater than an ``rword``.
  It's also used for instruction with ``REP`` in ``X86`` and ``X86_64``.


Options
-------

The ``options`` of the VM allow changing some internal mechanisms of QBDI. For most usage of QBDI, any options need to be specified.
The options are specified when the VM is created and can be changed with ``setOptions`` when the VM isn't running. After changing the options,
the cache should be cleared to apply the changes to all instruction.

- ``OPT_DISABLE_FPR``: This option disables the usage of FPRState. QBDI won't backup and restore any FPRState registers.
- ``OPT_DISABLE_OPTIONAL_FPR``: if ``OPT_DISABLE_FPR`` is not enabled, this option will force the FPRState to be restored and save
  before and after any instruction. By default, QBDI will try to detect instruction that uses FPR registers and only restored for
  these instructions.
- ``OPT_ATT_SYNTAX``: For X86 and X86_64 architecture, this option changes
  the syntax of ``InstAnalysis.disassembly`` to AT&T instead of Intel syntax.
