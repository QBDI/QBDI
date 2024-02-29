.. currentmodule:: pyqbdi

.. _get_started-pyqbdipreload:

PyQBDIPreload
=============

PyQBDIPreload is an implementation of QBDIPreload for PyQBDI.
It allows users to inject the Python runtime into a target process and execute their own script in it.
The limitations are pretty much the same as those we face on QBDIPreload and PyQBDI:

- For Linux and macOS the executable should be injectable with ``LD_PRELOAD`` or ``DYLD_INSERT_LIBRARIES``
- PyQBDIPreload cannot be injected in a Python process
- The Python runtime and the target must share the same architecture
- An extra :class:`VM` must not be created. An already prepared :class:`VM` is provided to :func:`pyqbdipreload_on_run`.

.. note::

    For Linux and macOS the Python library ``libpython3.x.so`` must be installed.

Main hook process
-----------------

Unlike QBDIPreload, the hook of the main function cannot be customised so you are unable to alter the hook process.
The Python interpreter is only initialised once the main function is hooked and the script is loaded.
Furthermore, some modifications are made on the environment of the interpreter before the user script loading:

- ``sys.argv`` are the argument of the executable
- ``LD_PRELOAD`` or ``DYLD_INSERT_LIBRARIES`` are removed from ``os.environ``
- ``pyqbdi.__preload__`` is set to ``True``

Instrumentation
---------------

Once the script is loaded, the :func:`pyqbdipreload_on_run` function is called with a ready-to-run :class:`VM`.
Any user callbacks should be registered to the :class:`VM`, then the VM can be run with :func:`pyqbdi.VM.run`.

.. code:: python

    import pyqbdi

    def instCallback(vm, gpr, fpr, data):
        # User code ...
        return pyqbdi.CONTINUE

    def pyqbdipreload_on_run(vm, start, stop):
        vm.addCodeCB(pyqbdi.PREINST, instCallback, None)
        vm.run(start, stop)

Exit hook
---------

The ``atexit`` module is triggered when the execution is finished, or when ``exit`` or ``_exit`` are called.

.. note::

    Any :class:`VM` object is invalided when the ``atexit`` module is triggered and should never be used.

Execution
---------

A script called ``pyqbdipreload.py`` is brought to set the environment up and run the executable.
The first parameter is the PyQBDIPreload script. Next are the binary followed by its respective arguments if any.

.. code:: bash

    python3 -m pyqbdipreload <script> <executable> [<arguments> ...]

Full example
------------

Merging everything we have learnt throughout this tutorial, we are now able to write our Python script
which prints the instructions that are being executed by the target executable.

.. include:: ../../examples/pyqbdi/trace_preload.py
   :code:
