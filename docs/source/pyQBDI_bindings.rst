PyQBDI Bindings
===============

PyQBDI bindings are quite close to the original C/C++ API.
All you need to add to your script be able to use the bindings is:

.. code-block:: python

    import pyqbdi

    def pyqbdipreload_on_run(vm, start, stop):
        pass


API
---

State Management
^^^^^^^^^^^^^^^^

.. autoclass:: pyqbdi.vm
   :members: getGPRState, getFPRState, setGPRState, setFPRState


State Initialization
^^^^^^^^^^^^^^^^^^^^

.. automodule:: pyqbdi
   :members: alignedAlloc, allocateVirtualStack, simulateCall
   :member-order: bysource


Execution
^^^^^^^^^

.. autoclass:: pyqbdi.vm
   :members: run, call


Instrumentation
^^^^^^^^^^^^^^^

.. autoclass:: pyqbdi.vm
   :members: addCodeCB, addCodeAddrCB, addCodeRangeCB, addMnemonicCB, deleteInstrumentation, deleteAllInstrumentations,
             addInstrumentedRange, addInstrumentedModule, addInstrumentedModuleFromAddr, removeInstrumentedRange,
             removeInstrumentedModule, removeInstrumentedModuleFromAddr

.. automodule:: pyqbdi
   :members: getModuleNames, getCurrentProcessMaps


Memory Callback
^^^^^^^^^^^^^^^

.. autoclass:: pyqbdi.vm
   :members: addMemAddrCB, addMemRangeCB, addMemAccessCB, recordMemoryAccess


Analysis
^^^^^^^^

.. autoclass:: pyqbdi.vm
   :members: getInstAnalysis, getInstMemoryAccess, getBBMemoryAccess
   :member-order: bysource


Cache management
^^^^^^^^^^^^^^^^

.. autoclass:: pyqbdi.vm
   :members: precacheBasicBlock, clearCache, clearAllCache
   :member-order: bysource


VM Events
^^^^^^^^^

.. autoclass:: pyqbdi.vm
   :members: addVMEventCB


Helpers
^^^^^^^

.. automodule:: pyqbdi
   :members: readMemory, writeMemory, decodeFloat, encodeFloat

Objects
^^^^^^^

.. autoclass:: pyqbdi.MemoryMap
    :members:

.. autoclass:: pyqbdi.InstAnalysis
    :members:

.. autoclass:: pyqbdi.OperandAnalysis
    :members:

.. autoclass:: pyqbdi.VMState
    :members:

.. autoclass:: pyqbdi.MemoryAccess
    :members:

.. autodata:: pyqbdi.GPRState

.. autodata:: pyqbdi.FPRState
