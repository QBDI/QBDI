.. currentmodule:: pyqbdi

.. _get_started-pyqbdipreload:

Get started with PyQBDI
=======================

PyQBDIPreload is an implementation of QBDIPreload for PyQBDI. It allows
injecting the python runtime in the target process and to execute an instruction script.
The limitations are the same as QBDIPreload and PyQBDI:

- Only Linux and MacOS are currently supported;
- The executable should be injectable with ``LD_PRELOAD`` or ``DYLD_INSERT_LIBRARIES``;
- PyQBDIPreload cannot be injected in a Python process;
- The Python runtime must have the same architecture as the target.
- The :class:`VM` mustn't be created. A ready-to-run :class:`VM` will be provided to the method :func:`pyqbdipreload_on_run`.

.. note::

    The python library ``libpython3.x.so`` must be installed.

Main hook process
-----------------

Unlike QBDIPreload, the hook of the main function cannot be customize the hook process. The
Python interpreter is initialized once the main function is hooked and the script is loaded.
The environment of the interpreter is modified before the script is loaded and :

- ``sys.argv`` are the argument of the executable
- ``LD_PRELOAD`` or ``DYLD_INSERT_LIBRARIES`` are removed from ``os.environ``
- ``pyqbdi.__preload__`` is set to ``True``

Instrumentation
---------------

Once the script is loaded, the method :func:`pyqbdipreload_on_run` is called with a ready-to-run :class:`VM`.
Any user callback should be added to the :class:`VM`, then the VM can be run with :func:`pyqbdi.VM.run`.

.. code:: python

    import pyqbdi

    def instCallback(vm, gpr, fpr, data):
        # User code ...
        return pyqbdi.CONTINUE

    def pyqbdipreload_on_run(vm, start, stop):
        vm.addCodeCB(pyqbdi.PREINST, showInstruction, None)
        vm.run(start, stop)

Exit Hook
---------

The ``atexit`` module is triggered when the execution is finished, or when ``exit`` or ``_exit`` are called.

.. note::

    Any :class:`VM` object is invalided when the ``atexit`` module is triggered and should never be used.

Execution
---------

A script ``pyqbdipreload.py`` is provided to setup the environment and run the executable.
The first parameter must be the preload script. The second is the binary followed by his parameters.

.. code:: bash

    $ python3 -m pyqbdipreload <script> <executable> [<parameters> ...]


Fully working example
---------------------

.. include:: ../../examples/pyqbdi/trace_preload.py
   :code:

