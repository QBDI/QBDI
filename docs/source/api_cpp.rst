.. highlight:: c++

C++ API
=======

Introduction
------------

The C++ API is the primary API of QBDI. The API is compatible with :ref:`QBDIPreload <qbdipreload_api>` on Linux and macOS.


VM class
--------

.. doxygenclass:: QBDI::VM
    :members: VM, operator=

Options
+++++++

.. doxygenfunction:: QBDI::VM::getOptions

.. doxygenfunction:: QBDI::VM::setOptions

.. _state-management-cpp:

State management
++++++++++++++++

.. doxygenfunction:: QBDI::VM::getGPRState

.. doxygenfunction:: QBDI::VM::getFPRState

.. doxygenfunction:: QBDI::VM::setGPRState

.. doxygenfunction:: QBDI::VM::setFPRState

.. _instrumentation-range-cpp:

Instrumentation range
+++++++++++++++++++++

Addition
^^^^^^^^

.. doxygenfunction:: QBDI::VM::addInstrumentedRange

.. doxygenfunction:: QBDI::VM::addInstrumentedModule

.. doxygenfunction:: QBDI::VM::addInstrumentedModuleFromAddr

.. doxygenfunction:: QBDI::VM::instrumentAllExecutableMaps

Removal
^^^^^^^

.. doxygenfunction:: QBDI::VM::removeInstrumentedRange

.. doxygenfunction:: QBDI::VM::removeInstrumentedModule

.. doxygenfunction:: QBDI::VM::removeInstrumentedModuleFromAddr

.. doxygenfunction:: QBDI::VM::removeAllInstrumentedRanges

Callback management
+++++++++++++++++++

.. _instcallback-management-cpp:

InstCallback
^^^^^^^^^^^^

.. doxygenfunction:: QBDI::VM::addCodeCB(InstPosition pos, InstCallback cbk, void*data, int priority)
.. doxygenfunction:: QBDI::VM::addCodeCB(InstPosition pos, InstCbLambda &&cbk, int priority)
.. doxygenfunction:: QBDI::VM::addCodeCB(InstPosition pos, const InstCbLambda &cbk, int priority)

.. doxygenfunction:: QBDI::VM::addCodeAddrCB(rword address, InstPosition pos, InstCallback cbk, void*data, int priority)
.. doxygenfunction:: QBDI::VM::addCodeAddrCB(rword address, InstPosition pos, InstCbLambda &&cbk, int priority)
.. doxygenfunction:: QBDI::VM::addCodeAddrCB(rword address, InstPosition pos, const InstCbLambda &cbk, int priority)

.. doxygenfunction:: QBDI::VM::addCodeRangeCB(rword start, rword end, InstPosition pos, InstCallback cbk, void*data, int priority)
.. doxygenfunction:: QBDI::VM::addCodeRangeCB(rword start, rword end, InstPosition pos, InstCbLambda &&cbk, int priority)
.. doxygenfunction:: QBDI::VM::addCodeRangeCB(rword start, rword end, InstPosition pos, const InstCbLambda &cbk, int priority)

.. doxygenfunction:: QBDI::VM::addMnemonicCB(const char*mnemonic, InstPosition pos, InstCallback cbk, void*data, int priority)
.. doxygenfunction:: QBDI::VM::addMnemonicCB(const char*mnemonic, InstPosition pos, InstCbLambda &&cbk, int priority)
.. doxygenfunction:: QBDI::VM::addMnemonicCB(const char*mnemonic, InstPosition pos, const InstCbLambda &cbk, int priority)


.. _vmcallback-management-cpp:

VMEvent
^^^^^^^

.. doxygenfunction:: QBDI::VM::addVMEventCB(VMEvent mask, VMCallback cbk, void*data)
.. doxygenfunction:: QBDI::VM::addVMEventCB(VMEvent mask, VMCbLambda &&cbk)
.. doxygenfunction:: QBDI::VM::addVMEventCB(VMEvent mask, const VMCbLambda &cbk)

.. _memorycallback-management-cpp:

MemoryAccess
^^^^^^^^^^^^

.. doxygenfunction:: QBDI::VM::addMemAccessCB(MemoryAccessType type, InstCallback cbk, void*data, int priority)
.. doxygenfunction:: QBDI::VM::addMemAccessCB(MemoryAccessType type, InstCbLambda &&cbk, int priority)
.. doxygenfunction:: QBDI::VM::addMemAccessCB(MemoryAccessType type, const InstCbLambda &cbk, int priority)


.. doxygenfunction:: QBDI::VM::addMemAddrCB(rword address, MemoryAccessType type, InstCallback cbk, void*data)
.. doxygenfunction:: QBDI::VM::addMemAddrCB(rword address, MemoryAccessType type, InstCbLambda &&cbk)
.. doxygenfunction:: QBDI::VM::addMemAddrCB(rword address, MemoryAccessType type, const InstCbLambda &cbk)


.. doxygenfunction:: QBDI::VM::addMemRangeCB(rword start, rword end, MemoryAccessType type, InstCallback cbk, void*data)
.. doxygenfunction:: QBDI::VM::addMemRangeCB(rword start, rword end, MemoryAccessType type, InstCbLambda &&cbk)
.. doxygenfunction:: QBDI::VM::addMemRangeCB(rword start, rword end, MemoryAccessType type, const InstCbLambda &cbk)


.. _instrrulecallback-management-cpp:

InstrRuleCallback
^^^^^^^^^^^^^^^^^

.. doxygenfunction:: QBDI::VM::addInstrRule(InstrRuleCallback cbk, AnalysisType type, void* data)
.. doxygenfunction:: QBDI::VM::addInstrRule(InstrRuleCbLambda &&cbk, AnalysisType type)
.. doxygenfunction:: QBDI::VM::addInstrRule(const InstrRuleCbLambda &cbk, AnalysisType type)

.. doxygenfunction:: QBDI::VM::addInstrRuleRange(rword start, rword end, InstrRuleCallback cbk, AnalysisType type, void* data)
.. doxygenfunction:: QBDI::VM::addInstrRuleRange(rword start, rword end, InstrRuleCbLambda &&cbk, AnalysisType type)
.. doxygenfunction:: QBDI::VM::addInstrRuleRange(rword start, rword end, const InstrRuleCbLambda &cbk, AnalysisType type)

.. doxygenfunction:: QBDI::VM::addInstrRuleRangeSet(RangeSet<rword> range, InstrRuleCallback cbk, AnalysisType type, void*data)
.. doxygenfunction:: QBDI::VM::addInstrRuleRangeSet(RangeSet<rword> range, InstrRuleCbLambda &&cbk, AnalysisType type)
.. doxygenfunction:: QBDI::VM::addInstrRuleRangeSet(RangeSet<rword> range, const InstrRuleCbLambda &cbk, AnalysisType type)


Removal
^^^^^^^

.. doxygenfunction:: QBDI::VM::deleteInstrumentation

.. doxygenfunction:: QBDI::VM::deleteAllInstrumentations

Run
+++

.. doxygenfunction:: QBDI::VM::run

.. doxygenfunction:: QBDI::VM::call

.. doxygenfunction:: QBDI::VM::callA

.. doxygenfunction:: QBDI::VM::callV

.. _instanalysis-getter-cpp:

InstAnalysis
++++++++++++

.. doxygenfunction:: QBDI::VM::getInstAnalysis

.. doxygenfunction:: QBDI::VM::getCachedInstAnalysis

.. _memaccess-getter-cpp:

MemoryAccess
++++++++++++

.. doxygenfunction:: QBDI::VM::getInstMemoryAccess

.. doxygenfunction:: QBDI::VM::getBBMemoryAccess

.. doxygenfunction:: QBDI::VM::recordMemoryAccess

Cache management
++++++++++++++++

.. doxygenfunction:: QBDI::VM::precacheBasicBlock

.. doxygenfunction:: QBDI::VM::clearCache

.. doxygenfunction:: QBDI::VM::clearAllCache

.. _register-state-cpp:

Register state
--------------

.. cpp:type:: QBDI::rword

    An integer of the size of a register

    - uint32_t for X86
    - uint64_t for X86_64

.. cpp:struct:: QBDI::GPRState

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

.. cpp:struct:: QBDI::FPRState

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

.. doxygenstruct:: QBDI::MMSTReg
    :members:
    :undoc-members:

.. doxygenstruct:: QBDI::FPControl
    :members:
    :undoc-members:

.. doxygenstruct:: QBDI::FPStatus
    :members:
    :undoc-members:

.. data:: QBDI::REG_RETURN

.. data:: QBDI::REG_BP

.. data:: QBDI::REG_SP

.. data:: QBDI::REG_PC

.. data:: QBDI::NUM_GPR

.. _callback-cpp:

Callback
--------

.. doxygentypedef:: QBDI::VMInstanceRef

.. doxygentypedef:: QBDI::InstCallback

.. doxygentypedef:: QBDI::InstCbLambda

.. doxygentypedef:: QBDI::VMCallback

.. doxygentypedef:: QBDI::VMCbLambda

.. doxygentypedef:: QBDI::InstrRuleCallback

.. doxygentypedef:: QBDI::InstrRuleCbLambda

.. doxygenstruct:: QBDI::InstrRuleDataCBK
    :members:

.. doxygenenum:: QBDI::InstPosition

.. doxygenenum:: QBDI::CallbackPriority

.. doxygenenum:: QBDI::VMAction

.. _instanalysis-cpp:

InstAnalysis
------------

.. doxygenenum:: QBDI::AnalysisType

.. doxygenstruct:: QBDI::InstAnalysis
    :members:

.. doxygenenum:: QBDI::ConditionType

.. doxygenstruct:: QBDI::OperandAnalysis
    :members:

.. doxygenenum:: QBDI::OperandType

.. doxygenenum:: QBDI::OperandFlag

.. doxygenenum:: QBDI::RegisterAccessType

.. _memaccess-cpp:

MemoryAccess
------------

.. doxygenstruct:: QBDI::MemoryAccess
    :members:

.. doxygenenum:: QBDI::MemoryAccessType

.. doxygenenum:: QBDI::MemoryAccessFlags

.. _vmevent-cpp:

VMEvent
-------

.. doxygenenum:: QBDI::VMEvent

.. doxygenstruct:: QBDI::VMState
    :members:

Memory management
-----------------

Allocation
++++++++++

.. doxygenfunction:: QBDI::alignedAlloc

.. doxygenfunction:: QBDI::allocateVirtualStack

.. doxygenfunction:: QBDI::alignedFree

.. doxygenfunction:: QBDI::simulateCall

.. doxygenfunction:: QBDI::simulateCallV

.. doxygenfunction:: QBDI::simulateCallA

Exploration
+++++++++++

.. doxygenfunction:: QBDI::getModuleNames

.. doxygenfunction:: QBDI::getCurrentProcessMaps

.. doxygenfunction:: QBDI::getRemoteProcessMaps

.. doxygenstruct:: QBDI::MemoryMap
    :members: range, permission, name

.. doxygenenum:: QBDI::Permission

Other globals
-------------

.. doxygenenum:: QBDI::Options

.. doxygenenum:: QBDI::VMError

Miscellaneous
-------------

Version
+++++++

.. doxygenfunction:: QBDI::getVersion

Log
+++

.. doxygenenum:: QBDI::LogPriority

.. doxygenfunction:: QBDI::setLogFile

.. doxygenfunction:: QBDI::setLogPriority

.. doxygenfunction:: QBDI::setLogConsole

.. doxygenfunction:: QBDI::setLogDefault

Range
+++++

.. doxygenclass:: QBDI::Range
    :members:
    :undoc-members:

.. doxygenclass:: QBDI::RangeSet
    :members:
    :undoc-members:

