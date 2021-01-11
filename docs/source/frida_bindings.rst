Frida bindings
==============

We propose bindings for Frida, the whole C/C++ API is available through them, but they are also backed up by helpers that fluidify script writing.

.. _frida-bindins-api:


API bindings
------------

Initialization
^^^^^^^^^^^^^^
.. js:autoclass:: QBDI

State Management
^^^^^^^^^^^^^^^^

.. js:autoclass:: GPRState
   :members:

.. js:autofunction:: QBDI#getGPRState

.. js:autofunction:: QBDI#setGPRState


State Initialization
^^^^^^^^^^^^^^^^^^^^

.. js:autofunction:: QBDI#alignedAlloc

.. js:autofunction:: QBDI#allocateVirtualStack

.. js:autofunction:: QBDI#simulateCall

.. js:autofunction:: QBDI#getFPRState

.. js:autofunction:: QBDI#setFPRState

Execution
^^^^^^^^^

.. js:autofunction:: QBDI#run

.. js:autofunction:: QBDI#call

.. js:autofunction:: QBDI#getModuleNames

.. js:autofunction:: QBDI#addInstrumentedModule

.. js:autofunction:: QBDI#addInstrumentedRange

.. js:autofunction:: QBDI#addInstrumentedModuleFromAddr

.. js:autofunction:: QBDI#instrumentAllExecutableMaps

.. js:autofunction:: QBDI#removeInstrumentedModule

.. js:autofunction:: QBDI#removeInstrumentedRange

.. js:autofunction:: QBDI#removeInstrumentedModuleFromAddr

.. js:autofunction:: QBDI#removeAllInstrumentedRanges

Instrumentation
^^^^^^^^^^^^^^^

.. js:autofunction:: QBDI#addCodeCB

.. js:autofunction:: QBDI#addCodeAddrCB

.. js:autofunction:: QBDI#addCodeRangeCB

.. js:autofunction:: QBDI#addMnemonicCB

.. js:autofunction:: QBDI#deleteInstrumentation

.. js:autofunction:: QBDI#deleteAllInstrumentations

Memory Callback
^^^^^^^^^^^^^^^

.. js:autofunction:: QBDI#addMemAddrCB

.. js:autofunction:: QBDI#addMemRangeCB

.. js:autofunction:: QBDI#addMemAccessCB

.. js:autofunction:: QBDI#recordMemoryAccess

Analysis
^^^^^^^^

.. js:autofunction:: QBDI#getInstAnalysis

.. js:autofunction:: QBDI#getCachedInstAnalysis

.. js:autofunction:: QBDI#getInstMemoryAccess

.. js:autofunction:: QBDI#getBBMemoryAccess

.. js:class:: InstAnalysis

  Object that describes the analysis of an instruction

  .. js:attribute:: address

      Instruction address (if ANALYSIS_INSTRUCTION)

  .. js:attribute:: affectControlFlow

      True if instruction affects control flow (if ANALYSIS_INSTRUCTION)

  .. js:attribute:: disassembly

      Instruction disassembly (if ANALYSIS_DISASSEMBLY)

  .. js:attribute:: instSize

      Instruction size (in bytes) (if ANALYSIS_INSTRUCTION)

  .. js:attribute:: isBranch

    True if instruction acts like a ‘jump’ (if ANALYSIS_INSTRUCTION)

  .. js:attribute:: isCall

    True if instruction acts like a ‘call’ (if ANALYSIS_INSTRUCTION)

  .. js:attribute:: isCompare

    True if instruction is a comparison (if ANALYSIS_INSTRUCTION)

  .. js:attribute:: isPredicable

    True if instruction contains a predicate (~is conditional) (if ANALYSIS_INSTRUCTION)

  .. js:attribute:: isReturn

    True if instruction acts like a ‘return’ (if ANALYSIS_INSTRUCTION)

  .. js:attribute:: mayLoad

    True if instruction ‘may’ load data from memory (if ANALYSIS_INSTRUCTION)

  .. js:attribute:: mayStore

    True if instruction ‘may’ store data to memory (if ANALYSIS_INSTRUCTION)

  .. js:attribute:: mnemonic

    LLVM mnemonic (if ANALYSIS_INSTRUCTION)

  .. js:attribute:: module

    Instruction module name (if ANALYSIS_SYMBOL and found)

  .. js:attribute:: operands

    Structure containing analysis results of an operand provided by the VM (if ANALYSIS_OPERANDS)

  .. js:attribute:: symbol

    Instruction symbol (if ANALYSIS_SYMBOL and found)


.. js:class:: MemoryAccess

  Object that describes a memory access

  .. js:attribute:: accessAddress

      Address of accessed memory

  .. js:attribute:: instAddress

      Address of instruction making the access

  .. js:attribute:: size

      Size of memory access (in bytes)

  .. js:attribute:: type

      Memory access type (READ / WRITE)

  .. js:attribute:: value

      Value read from / written to memory




Cache management
^^^^^^^^^^^^^^^^

.. js:autofunction:: QBDI#precacheBasicBlock

.. js:autofunction:: QBDI#clearCache

.. js:autofunction:: QBDI#clearAllCache


VM Events
^^^^^^^^^

.. js:autofunction:: QBDI#addVMEventCB

.. js:autoclass:: VMError

    .. js:autoattribute:: INVALID_EVENTID

Globals
-------

.. js:autoattribute:: QBDI_LIB_FULLPATH

.. js:autoattribute:: GPR_NAMES

.. js:autoattribute:: REG_PC

.. js:autoattribute:: REG_RETURN

.. js:autoattribute:: REG_SP

.. js:autoclass:: VMAction

    .. js:autoattribute:: CONTINUE
    .. js:autoattribute:: BREAK_TO_VM
    .. js:autoattribute:: STOP

.. js:autoclass:: VMEvent

    .. js:autoattribute:: SEQUENCE_ENTRY
    .. js:autoattribute:: SEQUENCE_EXIT
    .. js:autoattribute:: BASIC_BLOCK_ENTRY
    .. js:autoattribute:: BASIC_BLOCK_EXIT
    .. js:autoattribute:: BASIC_BLOCK_NEW
    .. js:autoattribute:: EXEC_TRANSFER_CALL
    .. js:autoattribute:: EXEC_TRANSFER_RETURN
    .. js:autoattribute:: SYSCALL_ENTRY
    .. js:autoattribute:: SYSCALL_EXIT
    .. js:autoattribute:: SIGNAL

.. js:autoclass:: InstPosition

    .. js:autoattribute:: PREINST
    .. js:autoattribute:: POSTINST

.. js:autoclass:: MemoryAccessType

    .. js:autoattribute:: MEMORY_READ
    .. js:autoattribute:: MEMORY_WRITE
    .. js:autoattribute:: MEMORY_READ_WRITE

.. js:autoclass:: SyncDirection

    .. js:autoattribute:: QBDI_TO_FRIDA
    .. js:autoattribute:: FRIDA_TO_QBDI

.. js:autoclass:: OperandType

    .. js:autoattribute:: OPERAND_INVALID
    .. js:autoattribute:: OPERAND_IMM
    .. js:autoattribute:: OPERAND_GPR
    .. js:autoattribute:: OPERAND_PRED

.. js:autoclass:: AnalysisType

    .. js:autoattribute:: ANALYSIS_INSTRUCTION
    .. js:autoattribute:: ANALYSIS_DISASSEMBLY
    .. js:autoattribute:: ANALYSIS_OPERANDS
    .. js:autoattribute:: ANALYSIS_SYMBOL

Register values
---------------

The size of a general register depends of the architecture.
QBDI uses a custom type (:js:data:`rword`) to represent a register value.

This binding provides a common interface (``.toRword()``) to cast values into JS types compatible
with the C :js:data:`rword` type.


.. js:autoattribute:: rword

.. js:function:: NativePointer.prototype.toRword()

    Convert a NativePointer into a type with the size of a register (``Number`` or ``UInt64``).

.. js:function:: Number.prototype.toRword()

   Convert a number into a type with the size of a register (``Number`` or ``UInt64``).
   Can't be used for numbers > 32 bits, would cause weird results due to IEEE-754.

.. js:function:: UInt64.prototype.toRword()

   An identity function (returning the same ``UInt64`` object).
   It exists only to provide a unified **toRword** interface.


Helpers
-------

Some functions helpful to interact with Frida interface and write scripts.

.. js:autofunction:: QBDI#getModuleNames

.. js:autofunction:: QBDI#newInstCallback

.. js:autofunction:: QBDI#newVMCallback

.. js:autoattribute:: QBDI#version

.. js:autofunction:: hexPointer

