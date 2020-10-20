Frida bindings
==============

We propose bindings for Frida, the whole C/C++ API is available through them, but they are also backed up by helpers that fluidify script writing.

.. _frida-bindins-api:

API bindings
------------

Initialization
^^^^^^^^^^^^^^
.. autojs:: ../../tools/frida-qbdi.js
    :exclude-desc: true
    :members: _QBDI
    :member-order: alphabetical

State Management
^^^^^^^^^^^^^^^^

.. autojs:: ../../tools/frida-qbdi.js
    :members: GPRState.prototype.dump, GPRState.prototype.pp, GPRState.prototype.synchronizeRegister, GPRState.prototype.synchronizeContext,
              GPRState.prototype.getRegister, GPRState.prototype.setRegister, GPRState.prototype.getRegisters, GPRState.prototype.setRegisters
    :member-order: bysource

.. autojs:: ../../tools/frida-qbdi.js
    :members: QBDI.prototype.dumpGPRs, QBDI.prototype.ppGPRs, QBDI.prototype.synchronizeRegister, QBDI.prototype.synchronizeContext,
              QBDI.prototype.getGPR, QBDI.prototype.setGPR, QBDI.prototype.getGPRs, QBDI.prototype.setGPRS
    :member-order: bysource

State Initialization
^^^^^^^^^^^^^^^^^^^^

.. autojs:: ../../tools/frida-qbdi.js
    :members: QBDI.prototype.alignedAlloc, QBDI.prototype.allocateVirtualStack, QBDI.prototype.simulateCall,
              QBDI.prototype.getGPRState(state), QBDI.prototype.getFPRState(state), QBDI.prototype.setGPRState, QBDI.prototype.setFPRState
    :member-order: alphabetical

Execution
^^^^^^^^^

.. autojs:: ../../tools/frida-qbdi.js
    :members: QBDI.prototype.run, QBDI.prototype.call, QBDI.prototype.getModuleNames,
              QBDI.prototype.addInstrumentedModule, QBDI.prototype.addInstrumentedRange, QBDI.prototype.addInstrumentedModuleFromAddr, QBDI.prototype.instrumentAllExecutableMaps
              QBDI.prototype.removeInstrumentedModule, QBDI.prototype.removeInstrumentedRange, removeInstrumentedModuleFromAddr, removeAllInstrumentedRanges
    :member-order: alphabetical

Instrumentation
^^^^^^^^^^^^^^^

.. autojs:: ../../tools/frida-qbdi.js
    :members: QBDI.prototype.addCodeCB, QBDI.prototype.addCodeAddrCB, QBDI.prototype.addCodeRangeCB, QBDI.prototype.addMnemonicCB,
              QBDI.prototype.deleteInstrumentation, QBDI.prototype.deleteAllInstrumentations
    :member-order: alphabetical

Memory Callback
^^^^^^^^^^^^^^^

.. autojs:: ../../tools/frida-qbdi.js
    :members: QBDI.prototype.addMemAddrCB, QBDI.prototype.addMemRangeCB, QBDI.prototype.addMemAccessCB, QBDI.prototype.recordMemoryAccess
    :member-order: alphabetical

Analysis
^^^^^^^^

.. autojs:: ../../tools/frida-qbdi.js
    :members: QBDI.prototype.getInstAnalysis, QBDI.prototype.getInstMemoryAccess, QBDI.prototype.getBBMemoryAccess
    :member-order: alphabetical

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

  .. js:attribute:: flags

      Memory Access flags

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

.. autojs:: ../../tools/frida-qbdi.js
    :members: QBDI.prototype.precacheBasicBlock, QBDI.prototype.clearCache, QBDI.prototype.clearAllCache
    :member-order: alphabetical

VM Events
^^^^^^^^^

.. autojs:: ../../tools/frida-qbdi.js
    :members: QBDI.prototype.addVMEventCB, VMError
    :member-order: alphabetical

Globals
-------

.. autojs:: ../../tools/frida-qbdi.js
    :members: QBDI_LIB_FULLPATH, GPR_NAMES, REG_PC, REG_RETURN, REG_SP,
              VMAction, VMEvent, InstPosition, MemoryAccessType, SyncDirection, AnalysisType
    :member-order: bysource


Register values
---------------

The size of a general register depends of the architecture.
QBDI uses a custom type (:js:data:`rword`) to represent a register value.

This binding provides a common interface (``.toRword()``) to cast values into JS types compatible
with the C :js:data:`rword` type.

.. autojs:: ../../tools/frida-qbdi.js
    :members: rword, NativePointer.prototype.toRword, Number.prototype.toRword, UInt64.prototype.toRword
    :member-order: bysource


Helpers
-------

Some functions helpful to interact with Frida interface and write scripts.

.. autojs:: ../../tools/frida-qbdi.js
    :members: QBDI.prototype.getModuleNames,
              QBDI.prototype.newInstCallback, QBDI.prototype.newVMCallback,
              QBDI.version, hexPointer
    :member-order: bysource
