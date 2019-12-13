PyQBDI Bindings
===============

PyQBDI bindings are quite close to the original C/C++ API.
All you need to add to your script be able to use the bindings is:

.. code-block:: python

    import pyqbdi

    # if used as a preload library
    def pyqbdipreload_on_run(vm, start, stop):
        pass

PyQBDI API
----------

The PyQBDI API is almost the same as the :ref:`C++ API <ref_api_cpp>`.

When the library is load as a preload library, the variable ``pyqbdi.__preload__`` is set to ``True``.

PyQBDI Helpers
--------------

In addition of C++ API, some helper are available:

Memory helpers
~~~~~~~~~~~~~~

.. automodule:: pyqbdi
    :members: readMemory, readRword, writeMemory, writeRword, allocateRword, allocateMemory, freeMemory

Float helpers
~~~~~~~~~~~~~

.. automodule:: pyqbdi
    :members: encodeFloat, decodeFloat, encodeFloatU, decodeFloatU, encodeDouble, decodeDouble, encodeDoubleU, decodeDoubleU

For more conversion, you may want to used the `struct library <https://docs.python.org/3/library/struct.html>`_ of Python.
