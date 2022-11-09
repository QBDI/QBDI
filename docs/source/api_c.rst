.. highlight:: c

C API
=====

Introduction
------------

The C API offers bindings over the C++ API. The VM class' methods are replaced by C functions that receive an object of type ``VMInstanceRef`` as a first parameter.

This API is compatible with :ref:`QBDIPreload <qbdipreload_api>` on Linux and macOS.

VM object
---------

..
   Doc notes
   Breathe doesn't have an option to force the C domain for project QBDI_C.
   The available options allow forcing a domain per file, but some headers are shared between C and CPP API.

   C API will be in the CPP domain of sphynx, but without the QBDI:: namespace.

.. cpp:type:: VMInstanceRef

    An abstract pointer to the VM object.

.. doxygenfunction:: qbdi_initVM
   :project: QBDI_C

.. doxygenfunction:: qbdi_terminateVM
   :project: QBDI_C

Options
+++++++

.. doxygenfunction:: qbdi_getOptions
    :project: QBDI_C

.. doxygenfunction:: qbdi_setOptions
    :project: QBDI_C

.. _state-management-c:

State management
++++++++++++++++

.. doxygenfunction:: qbdi_getGPRState
    :project: QBDI_C

.. doxygenfunction:: qbdi_getFPRState
    :project: QBDI_C

.. doxygenfunction:: qbdi_setGPRState
    :project: QBDI_C

.. doxygenfunction:: qbdi_setFPRState
    :project: QBDI_C

.. _instrumentation-range-c:

Instrumentation range
+++++++++++++++++++++

Addition
^^^^^^^^

.. doxygenfunction:: qbdi_addInstrumentedRange
    :project: QBDI_C

.. doxygenfunction:: qbdi_addInstrumentedModule
    :project: QBDI_C

.. doxygenfunction:: qbdi_addInstrumentedModuleFromAddr
    :project: QBDI_C

.. doxygenfunction:: qbdi_instrumentAllExecutableMaps
    :project: QBDI_C

Removal
^^^^^^^

.. doxygenfunction:: qbdi_removeInstrumentedRange
    :project: QBDI_C

.. doxygenfunction:: qbdi_removeInstrumentedModule
    :project: QBDI_C

.. doxygenfunction:: qbdi_removeInstrumentedModuleFromAddr
    :project: QBDI_C

.. doxygenfunction:: qbdi_removeAllInstrumentedRanges
    :project: QBDI_C

Callback management
+++++++++++++++++++

.. _instcallback-management-c:

InstCallback
^^^^^^^^^^^^

.. doxygenfunction:: qbdi_addCodeCB
    :project: QBDI_C

.. doxygenfunction:: qbdi_addCodeAddrCB
    :project: QBDI_C

.. doxygenfunction:: qbdi_addCodeRangeCB
    :project: QBDI_C

.. doxygenfunction:: qbdi_addMnemonicCB
    :project: QBDI_C

.. _vmcallback-management-c:

VMEvent
^^^^^^^

.. doxygenfunction:: qbdi_addVMEventCB
    :project: QBDI_C

.. _memorycallback-management-c:

MemoryAccess
^^^^^^^^^^^^

.. doxygenfunction:: qbdi_addMemAccessCB
    :project: QBDI_C

.. doxygenfunction:: qbdi_addMemAddrCB
    :project: QBDI_C

.. doxygenfunction:: qbdi_addMemRangeCB
    :project: QBDI_C

.. _instrrulecallback-management-c:

InstrRuleCallback
^^^^^^^^^^^^^^^^^

.. doxygenfunction:: qbdi_addInstrRule
    :project: QBDI_C

.. doxygenfunction:: qbdi_addInstrRuleRange
    :project: QBDI_C

Removal
^^^^^^^

.. doxygenfunction:: qbdi_deleteInstrumentation
    :project: QBDI_C

.. doxygenfunction:: qbdi_deleteAllInstrumentations
    :project: QBDI_C

Run
+++

.. doxygenfunction:: qbdi_run
    :project: QBDI_C

.. doxygenfunction:: qbdi_call
    :project: QBDI_C

.. doxygenfunction:: qbdi_callA
    :project: QBDI_C

.. doxygenfunction:: qbdi_callV
    :project: QBDI_C

.. _instanalysis-getter-c:

InstAnalysis
++++++++++++

.. doxygenfunction:: qbdi_getInstAnalysis
    :project: QBDI_C

.. doxygenfunction:: qbdi_getCachedInstAnalysis
    :project: QBDI_C

.. _memaccess-getter-c:

MemoryAccess
++++++++++++

.. doxygenfunction:: qbdi_getInstMemoryAccess
    :project: QBDI_C

.. doxygenfunction:: qbdi_getBBMemoryAccess
    :project: QBDI_C

.. doxygenfunction:: qbdi_recordMemoryAccess
    :project: QBDI_C

Cache management
++++++++++++++++

.. doxygenfunction:: qbdi_precacheBasicBlock
    :project: QBDI_C

.. doxygenfunction:: qbdi_clearCache
    :project: QBDI_C

.. doxygenfunction:: qbdi_clearAllCache
    :project: QBDI_C

.. _register-state-c:

Register state
--------------

.. cpp:type:: rword

    An integer of the size of a register

    - uint32_t for X86
    - uint64_t for X86_64

.. cpp:struct:: GPRState

    General Purpose Register context.

    For X86 architecture:

    .. include:: ../../include/QBDI/arch/X86/State.h
       :start-after: SPHINX_X86_GPRSTATE_BEGIN
       :end-before: // SPHINX_X86_GPRSTATE_END
       :code:

    For X86_64 architecture:

    .. include:: ../../include/QBDI/arch/X86_64/State.h
       :start-after: SPHINX_X86_64_GPRSTATE_BEGIN
       :end-before: // SPHINX_X86_64_GPRSTATE_END
       :code:

    For ARM architecture:

    .. include:: ../../include/QBDI/arch/ARM/State.h
       :start-after: SPHINX_ARM_GPRSTATE_BEGIN
       :end-before: // SPHINX_ARM_GPRSTATE_END
       :code:

    For AARCH64 architecture:

    .. include:: ../../include/QBDI/arch/AARCH64/State.h
       :start-after: SPHINX_AARCH64_GPRSTATE_BEGIN
       :end-before: // SPHINX_AARCH64_GPRSTATE_END
       :code:

.. cpp:struct:: FPRState

    Floating Point Register context.

    For X86 architecture:

    .. include:: ../../include/QBDI/arch/X86/State.h
       :start-after: SPHINX_X86_FPRSTATE_BEGIN
       :end-before: // SPHINX_X86_FPRSTATE_END
       :code:

    For X86_64 architecture:

    .. include:: ../../include/QBDI/arch/X86_64/State.h
       :start-after: SPHINX_X86_64_FPRSTATE_BEGIN
       :end-before: // SPHINX_X86_64_FPRSTATE_END
       :code:

    For ARM architecture:

    .. include:: ../../include/QBDI/arch/ARM/State.h
       :start-after: SPHINX_ARM_FPRSTATE_BEGIN
       :end-before: // SPHINX_ARM_FPRSTATE_END
       :code:

    For AARCH64 architecture:

    .. include:: ../../include/QBDI/arch/AARCH64/State.h
       :start-after: SPHINX_AARCH64_FPRSTATE_BEGIN
       :end-before: // SPHINX_AARCH64_FPRSTATE_END
       :code:

.. doxygenstruct:: MMSTReg
    :project: QBDI_C
    :members:
    :undoc-members:

.. doxygenstruct:: FPControl
    :project: QBDI_C
    :members:
    :undoc-members:

.. doxygenstruct:: FPStatus
    :project: QBDI_C
    :members:
    :undoc-members:

.. data:: REG_RETURN

.. data:: REG_BP

.. data:: REG_SP

.. data:: REG_PC

.. data:: NUM_GPR

.. _callback-c:

Callback
--------

.. doxygentypedef:: InstCallback
    :project: QBDI_C

.. doxygentypedef:: VMCallback
    :project: QBDI_C

.. doxygentypedef:: InstrRuleCallbackC
    :project: QBDI_C

.. doxygenfunction:: qbdi_addInstrRuleData
    :project: QBDI_C

.. cpp:type:: InstrRuleDataVec

    An abstract type to append InstCallback for the current instruction

.. doxygenenum:: InstPosition
    :project: QBDI_C

.. doxygenenum:: CallbackPriority
    :project: QBDI_C

.. doxygenenum:: VMAction
    :project: QBDI_C

.. _instanalysis-c:

InstAnalysis
------------

.. doxygenenum:: AnalysisType
    :project: QBDI_C

.. doxygenstruct:: InstAnalysis
    :project: QBDI_C
    :members:

.. doxygenenum:: ConditionType
    :project: QBDI_C

.. doxygenstruct:: OperandAnalysis
    :project: QBDI_C
    :members:

.. doxygenenum:: OperandType
    :project: QBDI_C

.. doxygenenum:: OperandFlag
    :project: QBDI_C

.. doxygenenum:: RegisterAccessType
    :project: QBDI_C

.. _memaccess-c:

MemoryAccess
------------

.. doxygenstruct:: MemoryAccess
    :project: QBDI_C
    :members:

.. doxygenenum:: MemoryAccessType
    :project: QBDI_C

.. doxygenenum:: MemoryAccessFlags
    :project: QBDI_C

.. _vmevent-c:

VMEvent
-------

.. doxygenenum:: VMEvent
    :project: QBDI_C

.. doxygenstruct:: VMState
    :project: QBDI_C
    :members:

Memory management
-----------------

Allocation
++++++++++

.. doxygenfunction:: qbdi_alignedAlloc
    :project: QBDI_C

.. doxygenfunction:: qbdi_allocateVirtualStack
    :project: QBDI_C

.. doxygenfunction:: qbdi_alignedFree
    :project: QBDI_C

.. doxygenfunction:: qbdi_simulateCall
    :project: QBDI_C

.. doxygenfunction:: qbdi_simulateCallV
    :project: QBDI_C

.. doxygenfunction:: qbdi_simulateCallA
    :project: QBDI_C

Exploration
+++++++++++

.. doxygenfunction:: qbdi_getModuleNames
    :project: QBDI_C

.. doxygenfunction:: qbdi_getCurrentProcessMaps
    :project: QBDI_C

.. doxygenfunction:: qbdi_getRemoteProcessMaps
    :project: QBDI_C

.. doxygenfunction:: qbdi_freeMemoryMapArray
    :project: QBDI_C

.. doxygenstruct:: qbdi_MemoryMap
    :project: QBDI_C
    :members: start, end, permission, name

.. doxygenenum:: qbdi_Permission
    :project: QBDI_C

Other globals
-------------

.. cpp:enum:: Options

  Note: some value are available only for some architecture

  Values for all architecture :

  .. cpp:enumerator:: NO_OPT

      Default Value

  .. cpp:enumerator:: OPT_DISABLE_FPR

      Disable all operation on FPU (SSE, AVX, SIMD). May break the execution if the target use the FPU

  .. cpp:enumerator:: OPT_DISABLE_OPTIONAL_FPR

      Disable context switch optimisation when the target execblock doesn't used FPR

  Values for AARCH64 and ARM only :

  .. cpp:enumerator:: OPT_DISABLE_LOCAL_MONITOR

      Disable the local monitor for instruction like strex

  Values for AARCH64 only :

  .. cpp:enumerator:: OPT_BYPASS_PAUTH

      Disable pointeur authentication

  .. cpp:enumerator:: OPT_ENABLE_BTI

      Enable BTI on instrumented code

  Values for ARM only :

  .. cpp:enumerator:: OPT_DISABLE_D16_D31

      Disable the used of D16-D31 register

  .. cpp:enumerator:: OPT_ARMv4

      Change between ARM and Thumb as an ARMv4 CPU

  .. cpp:enumerator:: OPT_ARMv5T_6

      Change between ARM and Thumb as an ARMv5T or ARMv6 CPU

  .. cpp:enumerator:: OPT_ARMv7

      Change between ARM and Thumb as an ARMv7 CPU (default)

  .. cpp:enumerator:: OPT_ARM_MASK

      When apply :cpp:enumerator:`OPT_ARMv4`,
      :cpp:enumerator:`OPT_ARMv5T_6` or
      :cpp:enumerator:`OPT_ARMv7`, this mask must be clear.

  Values for X86 and X86_64 only :

  .. cpp:enumerator:: OPT_ATT_SYNTAX

      Used the AT&T syntax for instruction disassembly

  Values for X86_64 only :

  .. cpp:enumerator:: OPT_ENABLE_FS_GS

      Enable Backup/Restore of FS/GS  segment. This option uses the
      instructions (RD|WR)(FS|GS)BASE that must be supported by the operating
      system

.. doxygenenum:: VMError
    :project: QBDI_C

Miscellaneous
-------------

Version
+++++++

.. doxygenfunction:: qbdi_getVersion
    :project: QBDI_C

Log
+++

.. doxygenenum:: LogPriority
    :project: QBDI_C

.. doxygenfunction:: qbdi_setLogFile
    :project: QBDI_C

.. doxygenfunction:: qbdi_setLogConsole
    :project: QBDI_C

.. doxygenfunction:: qbdi_setLogDefault
    :project: QBDI_C

.. doxygenfunction:: qbdi_setLogPriority
    :project: QBDI_C
