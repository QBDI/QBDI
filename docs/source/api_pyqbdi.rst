.. currentmodule:: pyqbdi

PyQBDI API
==========

Introduction
------------

We offer bindings for Python, the whole C/C++ API is available through them, but they are also backed up by plenty of helpers that fluidify the script writing.

On Linux and macOS, PyQBDI supports QBDIPreload as :ref:`PyQBDIPreload <pyqbdipreload_api>`.

However, you must be aware that PyQBDI has some limitations:

- The library can handle only one :py:class:`VM` at a time.
- The :py:class:`VM` must not be used in the ``atexit`` module.
- 32-bit versions of PyQBDI and Python are needed to instrument 32-bit targets.
- PyQBDI cannot be used to instrument a Python process since both the **host** and the **target** will use the Python runtime.

VM class
--------

.. autoclass:: pyqbdi.VM
    :special-members: __init__
    :members:
    :exclude-members: getGPRState, getFPRState, setGPRState, setFPRState,
                      addInstrumentedRange, addInstrumentedModule, addInstrumentedModuleFromAddr, instrumentAllExecutableMaps,
                      removeInstrumentedRange, removeInstrumentedModule, removeInstrumentedModuleFromAddr, removeAllInstrumentedRanges,
                      addCodeCB, addCodeAddrCB, addCodeRangeCB, addMnemonicCB, addVMEventCB, addMemAccessCB, addMemAddrCB, addMemRangeCB,
                      recordMemoryAccess, addInstrRule, addInstrRuleRange, deleteInstrumentation, deleteAllInstrumentations, run, call,
                      getInstAnalysis, getCachedInstAnalysis, getInstMemoryAccess, getBBMemoryAccess, precacheBasicBlock, clearCache, clearAllCache

.. _state-management-pyqbdi:

State management
++++++++++++++++

.. autofunction:: pyqbdi.VM.getGPRState

.. autofunction:: pyqbdi.VM.getFPRState

.. autofunction:: pyqbdi.VM.setGPRState

.. autofunction:: pyqbdi.VM.setFPRState

.. _instrumentation-range-pyqbdi:

Instrumentation range
+++++++++++++++++++++

Addition
^^^^^^^^

.. autofunction:: pyqbdi.VM.addInstrumentedRange

.. autofunction:: pyqbdi.VM.addInstrumentedModule

.. autofunction:: pyqbdi.VM.addInstrumentedModuleFromAddr

.. autofunction:: pyqbdi.VM.instrumentAllExecutableMaps

Removal
^^^^^^^

.. autofunction:: pyqbdi.VM.removeInstrumentedRange

.. autofunction:: pyqbdi.VM.removeInstrumentedModule

.. autofunction:: pyqbdi.VM.removeInstrumentedModuleFromAddr

.. autofunction:: pyqbdi.VM.removeAllInstrumentedRanges

Callback management
+++++++++++++++++++

.. _instcallback-management-pyqbdi:

InstCallback
^^^^^^^^^^^^

.. autofunction:: pyqbdi.VM.addCodeCB

.. autofunction:: pyqbdi.VM.addCodeAddrCB

.. autofunction:: pyqbdi.VM.addCodeRangeCB

.. autofunction:: pyqbdi.VM.addMnemonicCB

.. _vmcallback-management-pyqbdi:

VMEvent
^^^^^^^

.. autofunction:: pyqbdi.VM.addVMEventCB

.. _memorycallback-management-pyqbdi:

MemoryAccess
^^^^^^^^^^^^

.. autofunction:: pyqbdi.VM.addMemAccessCB

.. autofunction:: pyqbdi.VM.addMemAddrCB

.. autofunction:: pyqbdi.VM.addMemRangeCB

.. _instrrulecallback-management-pyqbdi:

InstrRuleCallback
^^^^^^^^^^^^^^^^^

.. autofunction:: pyqbdi.VM.addInstrRule

.. autofunction:: pyqbdi.VM.addInstrRuleRange

Removal
^^^^^^^

.. autofunction:: pyqbdi.VM.deleteInstrumentation

.. autofunction:: pyqbdi.VM.deleteAllInstrumentations

Run
+++

.. autofunction:: pyqbdi.VM.run

.. autofunction:: pyqbdi.VM.call

.. _instanalysis-getter-pyqbdi:

InstAnalysis
++++++++++++

.. autofunction:: pyqbdi.VM.getInstAnalysis

.. autofunction:: pyqbdi.VM.getCachedInstAnalysis

.. _memaccess-getter-pyqbdi:

MemoryAccess
++++++++++++

.. autofunction:: pyqbdi.VM.getInstMemoryAccess

.. autofunction:: pyqbdi.VM.getBBMemoryAccess

.. autofunction:: pyqbdi.VM.recordMemoryAccess

Cache management
++++++++++++++++

.. autofunction:: pyqbdi.VM.precacheBasicBlock

.. autofunction:: pyqbdi.VM.clearCache

.. autofunction:: pyqbdi.VM.clearAllCache

.. _register-state-pyqbdi:

Register state
--------------

.. autoclass:: pyqbdi.GPRState
    :members:
    :undoc-members:
    :special-members: __getitem__, __setitem__

.. autoclass:: pyqbdi.FPRState
    :members:
    :undoc-members:

.. data:: pyqbdi.REG_RETURN

.. data:: pyqbdi.REG_BP

.. data:: pyqbdi.REG_SP

.. data:: pyqbdi.REG_PC

.. data:: pyqbdi.NUM_GPR

.. _callback-pyqbdi:

Callback
--------

.. function:: pyqbdi.InstCallback(vm: pyqbdi.VM, gpr: pyqbdi.GPRState, fpr: pyqbdi.FPRState, data: object) -> pyqbdi.VMAction

    This is the prototype of a function callback for:

    - :py:func:`pyqbdi.VM.addCodeCB`, :py:func:`pyqbdi.VM.addCodeAddrCB` and :py:func:`pyqbdi.VM.addCodeRangeCB`
    - :py:func:`pyqbdi.VM.addMnemonicCB`
    - :py:func:`pyqbdi.VM.addMemAccessCB`, :py:func:`pyqbdi.VM.addMemAddrCB` and :py:func:`pyqbdi.VM.addMemRangeCB`
    - :py:class:`pyqbdi.InstrRuleDataCBK`.

    :param VM       vm:    The current QBDI object
    :param GPRState gpr:   The current GPRState
    :param FPRState fpr:   The current FPRState
    :param Object   data:  A user-defined object.

    :return: the :py:data:`VMAction` to continue or stop the execution

.. function:: pyqbdi.VMCallback(vm: pyqbdi.VM, vmState: pyqbdi.VMState, gpr: pyqbdi.GPRState, fpr: pyqbdi.FPRState, data: object) -> pyqbdi.VMAction

    This is the prototype of a function callback for :py:func:`pyqbdi.VM.addVMEventCB`.

    :param VM       vm:      The current QBDI object
    :param VMState  vmState: A structure containing the current state of the VM.
    :param GPRState gpr:     The current GPRState
    :param FPRState fpr:     The current FPRState
    :param Object   data:    A user-defined object

    :return: the :py:data:`VMAction` to continue or stop the execution

.. function:: pyqbdi.InstrRuleCallback(vm: pyqbdi.VM, ana: pyqbdi.InstAnalysis, data: object) -> List[pyqbdi.InstrRuleDataCBK]

    This is the prototype of a function callback for :py:func:`pyqbdi.VM.addInstrRule` and :py:func:`pyqbdi.VM.addInstrRuleRange`.

    :param VM           vm:   The current QBDI object
    :param InstAnalysis ana:  The current QBDI object
    :param Object       data: A user-defined object

    :return: A list of :py:class:`pyqbdi.InstrRuleDataCBK`

.. autoclass:: pyqbdi.InstrRuleDataCBK
    :special-members: __init__
    :members:

.. autodata:: pyqbdi.InstPosition

.. autodata:: pyqbdi.VMAction

.. _instanalysis-pyqbdi:

InstAnalysis
------------

.. autodata:: pyqbdi.AnalysisType

.. autoclass:: pyqbdi.InstAnalysis
    :members:

.. autodata:: pyqbdi.ConditionType

.. autoclass:: pyqbdi.OperandAnalysis
    :members:

.. autodata:: pyqbdi.OperandType

.. autodata:: pyqbdi.OperandFlag

.. autodata:: pyqbdi.RegisterAccessType

.. _memaccess-pyqbdi:

MemoryAccess
------------

.. autoclass:: pyqbdi.MemoryAccess
    :members:

.. autodata:: pyqbdi.MemoryAccessType

.. autodata:: pyqbdi.MemoryAccessFlags

.. _vmevent-pyqbdi:

VMEvent
-------

.. autodata:: pyqbdi.VMEvent

.. autoclass:: pyqbdi.VMState
    :members:

Memory management
-----------------

Allocation
++++++++++

.. autofunction:: pyqbdi.alignedAlloc

.. autofunction:: pyqbdi.allocateVirtualStack

.. autofunction:: pyqbdi.alignedFree

.. autofunction:: pyqbdi.simulateCall

Exploration
+++++++++++

.. autofunction:: pyqbdi.getModuleNames

.. autofunction:: pyqbdi.getCurrentProcessMaps

.. autofunction:: pyqbdi.getRemoteProcessMaps

.. autoclass:: pyqbdi.MemoryMap
    :members:

.. autodata:: pyqbdi.Permission

Other globals
-------------

.. autodata:: pyqbdi.Options

.. autodata:: pyqbdi.VMError

Miscellaneous
-------------

Version & info
++++++++++++++

.. data:: pyqbdi.__arch__

.. data:: pyqbdi.__platform__

.. data:: pyqbdi.__preload__

    Library load with pyqbdipreload

.. data:: pyqbdi.__version__

    Version of QBDI

Log
+++

.. autodata:: pyqbdi.LogPriority

.. autofunction:: pyqbdi.addLogFilter

Range
+++++

.. autoclass:: pyqbdi.Range
    :members:

Memory helpers
--------------

.. autofunction:: pyqbdi.readMemory
.. autofunction:: pyqbdi.readRword
.. autofunction:: pyqbdi.writeMemory
.. autofunction:: pyqbdi.writeRword
.. autofunction:: pyqbdi.allocateRword
.. autofunction:: pyqbdi.allocateMemory
.. autofunction:: pyqbdi.freeMemory

Float helpers
-------------

.. autofunction:: pyqbdi.encodeFloat
.. autofunction:: pyqbdi.decodeFloat
.. autofunction:: pyqbdi.encodeFloatU
.. autofunction:: pyqbdi.decodeFloatU
.. autofunction:: pyqbdi.encodeDouble
.. autofunction:: pyqbdi.decodeDouble
.. autofunction:: pyqbdi.encodeDoubleU
.. autofunction:: pyqbdi.decodeDoubleU

For more conversion utilities, check out the Python's `struct library <https://docs.python.org/3/library/struct.html>`_.
