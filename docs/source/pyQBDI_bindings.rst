.. currentmodule:: pyqbdi

PyQBDI Bindings API
===================

The PyQBDI API is almost the same as the :ref:`C++ API <ref_api_cpp>`.

When the library is loaded as a preloaded library, the variable ``pyqbdi.__preload__`` is set to ``True``.

State
-----

The state available depends of the architecture (X86 or X86-64).

.. autoclass:: pyqbdi.GPRState
    :members:
    :undoc-members:
    :special-members: __getitem__, __setitem__

.. autoclass:: pyqbdi.FPRState
    :members:
    :undoc-members:

VM
--

.. autoclass:: pyqbdi.VM
    :special-members: __init__
    :members:

.. autodata:: pyqbdi.Options

Callback
--------

.. autodata:: pyqbdi.InstPosition
.. autoclass:: pyqbdi.MemoryAccess
    :members:
.. autodata:: pyqbdi.MemoryAccessType
.. autodata:: pyqbdi.VMAction
.. autodata:: pyqbdi.VMEvent
.. autoclass:: pyqbdi.VMState
    :members:

InstAnalysis
------------

.. autodata:: pyqbdi.AnalysisType

.. autoclass:: pyqbdi.InstAnalysis
    :members:

.. autoclass:: pyqbdi.OperandAnalysis
    :members:

.. autodata:: pyqbdi.RegisterAccessType
.. autodata:: pyqbdi.OperandType
.. autodata:: pyqbdi.OperandFlag

Memory and process map
----------------------

.. autofunction:: getModuleNames

.. autofunction:: getCurrentProcessMaps
.. autofunction:: getRemoteProcessMaps

.. autodata:: pyqbdi.Permission

.. autoclass:: pyqbdi.MemoryMap
    :members:

.. autofunction:: alignedAlloc
.. autofunction:: alignedFree
.. autofunction:: allocateVirtualStack
.. autofunction:: simulateCall

Range
-----

.. autoclass:: pyqbdi.Range
    :members:

Instrument Rule Data
--------------------

.. autoclass:: pyqbdi.InstrumentDataCBK
    :special-members: __init__
    :members:

Miscellaneous
-------------

.. data:: __arch__

.. data:: __platform__

.. data:: __preload__

    Library load with pyqbdipreload

.. data:: __version__

    Version of QBDI


PyQBDI Bindings Helpers
=======================

In addition of C++ API, some helper are available:

Memory helpers
--------------

.. autofunction:: readMemory
.. autofunction:: readRword
.. autofunction:: writeMemory
.. autofunction:: writeRword
.. autofunction:: allocateRword
.. autofunction:: allocateMemory
.. autofunction:: freeMemory

Float helpers
-------------

.. autofunction:: encodeFloat
.. autofunction:: decodeFloat
.. autofunction:: encodeFloatU
.. autofunction:: decodeFloatU
.. autofunction:: encodeDouble
.. autofunction:: decodeDouble
.. autofunction:: encodeDoubleU
.. autofunction:: decodeDoubleU

For more conversion, you may want to use the `struct library <https://docs.python.org/3/library/struct.html>`_ of Python.
