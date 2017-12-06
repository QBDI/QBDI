PyQBDI
======

PyQBDI bindings are quite close to the original C/C++ API. 
All you need to be able to use the bindings is to : **import pyqbdi**

API bindings
------------


State Management
^^^^^^^^^^^^^^^^

.. doxygenfunction:: QBDI::Bindings::Python::vm_getGPRState
   :project: TOOLS

.. doxygenfunction:: QBDI::Bindings::Python::vm_getFPRState
   :project: TOOLS

.. doxygenfunction:: QBDI::Bindings::Python::vm_setGPRState
   :project: TOOLS

.. doxygenfunction:: QBDI::Bindings::Python::vm_setFPRState
   :project: TOOLS
   

State Initialization
^^^^^^^^^^^^^^^^^^^^

   .. doxygenfunction:: QBDI::Bindings::Python::pyqbdi_alignedAlloc
      :project: TOOLS

   .. doxygenfunction:: QBDI::Bindings::Python::pyqbdi_allocateVirtualStack
      :project: TOOLS

   .. doxygenfunction:: QBDI::Bindings::Python::pyqbdi_simulateCall
      :project: TOOLS
    
Execution
^^^^^^^^^

.. doxygenfunction:: QBDI::Bindings::Python::vm_run
   :project: TOOLS

.. doxygenfunction:: QBDI::Bindings::Python::vm_call
   :project: TOOLS

    
Instrumentation
^^^^^^^^^^^^^^^


.. doxygenfunction:: QBDI::Bindings::Python::vm_addCodeCB
   :project: TOOLS
   
.. doxygenfunction:: QBDI::Bindings::Python::vm_addCodeAddrCB
   :project: TOOLS
   
.. doxygenfunction:: QBDI::Bindings::Python::vm_addCodeRangeCB
   :project: TOOLS
   
.. doxygenfunction:: QBDI::Bindings::Python::vm_addMnemonicCB
   :project: TOOLS
   
.. doxygenfunction:: QBDI::Bindings::Python::vm_deleteInstrumentation
   :project: TOOLS
   
.. doxygenfunction:: QBDI::Bindings::Python::vm_deleteAllInstrumentations
   :project: TOOLS


Memory Callback
^^^^^^^^^^^^^^^

.. doxygenfunction:: QBDI::Bindings::Python::vm_addMemAddrCB
   :project: TOOLS
   
.. doxygenfunction:: QBDI::Bindings::Python::vm_addMemRangeCB
   :project: TOOLS
   
.. doxygenfunction:: QBDI::Bindings::Python::vm_addMemAccessCB
   :project: TOOLS
   
.. doxygenfunction:: QBDI::Bindings::Python::vm_recordMemoryAccess
   :project: TOOLS
   
Analysis
^^^^^^^^
    
.. doxygenfunction:: QBDI::Bindings::Python::vm_getInstAnalysis
   :project: TOOLS
   
.. doxygenfunction:: QBDI::Bindings::Python::vm_getInstMemoryAccess
   :project: TOOLS
   
.. doxygenfunction:: QBDI::Bindings::Python::vm_getBBMemoryAccess
   :project: TOOLS


Cache management
^^^^^^^^^^^^^^^^

.. doxygenfunction:: QBDI::Bindings::Python::vm_precacheBasicBlock
   :project: TOOLS

.. doxygenfunction:: QBDI::Bindings::Python::vm_clearCache
   :project: TOOLS
   
.. doxygenfunction:: QBDI::Bindings::Python::vm_clearAllCache
   :project: TOOLS

VM Events
^^^^^^^^^

.. doxygenfunction:: QBDI::Bindings::Python::vm_addVMEventCB
   :project: TOOLS

