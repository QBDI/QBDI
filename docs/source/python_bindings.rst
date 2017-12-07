PyQBDI
======

PyQBDI bindings are quite close to the original C/C++ API. 
All you need to be able to use the bindings is to : **import pyqbdi**

API bindings
------------

State Management
^^^^^^^^^^^^^^^^

.. autoclass:: pyqbdi.vm
   :members: getGPRState, getFPRState, setGPRState, setFPRState


State Initialization
^^^^^^^^^^^^^^^^^^^^

.. autoclass:: pyqbdi.pyqbdi
   :members: alignedAlloc, allocateVirtualStack, simulateCall

    
Execution
^^^^^^^^^

.. autoclass:: pyqbdi.vm
   :members: run, call

    
Instrumentation
^^^^^^^^^^^^^^^

.. autoclass:: pyqbdi.vm
   :members: addCodeCB, addCodeAddrCB, addCodeRangeCB, addMnemonicCB, deleteInstrumentation, deleteAllInstrumentations

.. autoclass:: pyqbdi.vm
   :members: addInstrumentedRange, addInstrumentedModule, addInstrumentedModuleFromAddr, removeInstrumentedRange, removeInstrumentedModule, removeInstrumentedModuleFromAddr

.. autoclass:: pyqbdi.pyqbdi
   :members: getModuleNames
   

Memory Callback
^^^^^^^^^^^^^^^

.. autoclass:: pyqbdi.vm
   :members: addMemAddrCB, addMemRangeCB, addMemAccessCB, recordMemoryAccess

   
Analysis
^^^^^^^^

.. autoclass:: pyqbdi.vm
   :members: getInstAnalysis, getInstMemoryAccess, getBBMemoryAccess


Cache management
^^^^^^^^^^^^^^^^

.. autoclass:: pyqbdi.vm
   :members: precacheBasicBlock, clearCache, clearAllCache


VM Events
^^^^^^^^^

.. autoclass:: pyqbdi.vm
   :members: addVMEventCB


Helpers
^^^^^^^

.. autoclass:: pyqbdi.pyqbdi
   :members: readMemory, writeMemory, decodeFloat, encodeFloat
   
Objects
^^^^^^^

.. autodata:: pyqbdi.InstAnalysis_Object

.. autodata:: pyqbdi.GPRState_Object

.. autodata:: pyqbdi.FPRState_Object

.. autodata:: pyqbdi.MemoryAccess_Object

.. autodata:: pyqbdi.OperandAnalysis_Object

.. autodata:: pyqbdi.VMInstance_Object

.. autodata:: pyqbdi.VMState_Object
