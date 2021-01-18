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

.. js:autofunction:: QBDI#addInstrRule

.. js:autofunction:: QBDI#addInstrRuleRange

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

    True if QBDI detects a load for this instruction (if ANALYSIS_INSTRUCTION)

  .. js:attribute:: mayStore

    True if QBDI detects a store for this instruction (if ANALYSIS_INSTRUCTION)

  .. js:attribute:: loadSize

    size of the expected read access (if ANALYSIS_INSTRUCTION)

  .. js:attribute:: storeSize

    size of the expected write access (if ANALYSIS_INSTRUCTION)

  .. js:attribute:: condition

    Condition associated with the instruction (if ANALYSIS_INSTRUCTION)

  .. js:attribute:: mnemonic

    LLVM mnemonic (if ANALYSIS_INSTRUCTION)

  .. js:attribute:: flagsAccess

    Flag access type (noaccess, r, w, rw) (if ANALYSIS_OPERANDS)

  .. js:attribute:: operands

    Structure containing analysis results of an operand provided by the VM (if ANALYSIS_OPERANDS)

  .. js:attribute:: module

    Instruction module name (if ANALYSIS_SYMBOL and found)

  .. js:attribute:: symbol

    Instruction symbol (if ANALYSIS_SYMBOL and found)


.. js:class:: OperandAnalysis

  Structure containing analysis results of an operand provided by the VM.

  .. js:attribute:: type

    Operand type

  .. js:attribute:: flag

    Operand flag

  .. js:attribute:: value

    Operand value (if immediate), or register Id

  .. js:attribute:: size

    Operand size (in bytes)

  .. js:attribute:: regOff

    Sub-register offset in register (in bits)

  .. js:attribute:: regCtxIdx

    Register index in VM state

  .. js:attribute:: regName

    Register name

  .. js:attribute:: regAccess

    Register access type (r, w, rw)


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

  .. js:attribute:: flags

      Memory access flags

.. js:class:: VMState

  Object that describes the current VM state

  .. js:attribute:: event

    The event(s) which triggered the callback (must be checked using a mask: event & BASIC_BLOCK_ENTRY).

  .. js:attribute:: sequenceStart

    The current basic block start address which can also be the execution transfer destination.

  .. js:attribute:: sequenceEnd

    The current basic block end address which can also be the execution transfer destination.

  .. js:attribute:: basicBlockStart

    The current sequence start address which can also be the execution transfer destination.

  .. js:attribute:: basicBlockEnd

    The current sequence end address which can also be the execution transfer destination.

  .. js:attribute:: lastSignal

    Not implemented.

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

.. js:autoclass:: MemoryAccessFlags

    .. js:autoattribute:: MEMORY_NO_FLAGS
    .. js:autoattribute:: MEMORY_UNKNOWN_SIZE
    .. js:autoattribute:: MEMORY_UNKNOWN_VALUE

.. js:autoclass:: SyncDirection

    .. js:autoattribute:: QBDI_TO_FRIDA
    .. js:autoattribute:: FRIDA_TO_QBDI

.. js:autoclass:: OperandType

    .. js:autoattribute:: OPERAND_INVALID
    .. js:autoattribute:: OPERAND_IMM
    .. js:autoattribute:: OPERAND_GPR
    .. js:autoattribute:: OPERAND_PRED
    .. js:autoattribute:: OPERAND_FPR
    .. js:autoattribute:: OPERAND_SEG

.. js:autoclass:: OperandFlag

    .. js:autoattribute:: OPERANDFLAG_NONE
    .. js:autoattribute:: OPERANDFLAG_ADDR
    .. js:autoattribute:: OPERANDFLAG_PCREL
    .. js:autoattribute:: OPERANDFLAG_UNDEFINED_EFFECT
    .. js:autoattribute:: OPERANDFLAG_IMPLICIT

.. js:autoclass:: AnalysisType

    .. js:autoattribute:: ANALYSIS_INSTRUCTION
    .. js:autoattribute:: ANALYSIS_DISASSEMBLY
    .. js:autoattribute:: ANALYSIS_OPERANDS
    .. js:autoattribute:: ANALYSIS_SYMBOL

.. js:autoclass:: ConditionType

  .. js:autoattribute:: CONDITION_NONE
  .. js:autoattribute:: CONDITION_ALWAYS
  .. js:autoattribute:: CONDITION_NEVER
  .. js:autoattribute:: CONDITION_EQUALS
  .. js:autoattribute:: CONDITION_NOT_EQUALS
  .. js:autoattribute:: CONDITION_ABOVE
  .. js:autoattribute:: CONDITION_BELOW_EQUALS
  .. js:autoattribute:: CONDITION_ABOVE_EQUALS
  .. js:autoattribute:: CONDITION_BELOW
  .. js:autoattribute:: CONDITION_GREAT
  .. js:autoattribute:: CONDITION_LESS_EQUALS
  .. js:autoattribute:: CONDITION_GREAT_EQUALS
  .. js:autoattribute:: CONDITION_LESS
  .. js:autoattribute:: CONDITION_EVEN
  .. js:autoattribute:: CONDITION_ODD
  .. js:autoattribute:: CONDITION_OVERFLOW
  .. js:autoattribute:: CONDITION_NOT_OVERFLOW
  .. js:autoattribute:: CONDITION_SIGN
  .. js:autoattribute:: CONDITION_NOT_SIGN

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

.. js:autofunction:: QBDI#newInstrRuleCallback

.. js:autofunction:: QBDI#newInstCallback

.. js:autofunction:: QBDI#newVMCallback

.. js:autoattribute:: QBDI#version

.. js:autofunction:: hexPointer

