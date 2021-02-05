.. _pyqbdipreload_api:

PyQBDIPreload API
=================

Introduction
------------

PyQBDIPreload consists of two main components:

- The QBDIPreload script included in the PyQBDI library.
  It will load the Python runtime and execute the user script in order to instrument the target.
- An injector script called `pyqbdipreload.py` that sets up the environment variables to inject the library and the Python runtime.

.. note::
    PyQBDIPreload has the same limitations as QBDIPreload and PyQBDI.

User callback
-------------

.. function:: pyqbdipreload_on_run(vm: pyqbdi.VM, start: int, end: int)

    This function must be present in the preloaded script

    :param VM vm:     VM Instance
    :param int start: Start address of the range (included).
    :param int end:   End address of the range (excluded).

QBDIPreload steps
-----------------

:cpp:func:`qbdipreload_on_start` and :cpp:func:`qbdipreload_on_premain` are not available on PyQBDIPreload.

The Python interpreter and the preloaded script are loaded while executing :cpp:func:`qbdipreload_on_main`.

The :py:func:`pyqbdipreload_on_run` method is called while executing :cpp:func:`qbdipreload_on_main`.

The interpreter is shut down while executing :cpp:func:`qbdipreload_on_exit`, after calling all the registered methods of
`atexit <https://docs.python.org/fr/3/library/atexit.html>`_ module.

PyQBDIPreload script
--------------------

You can use `pyqbdipreload.py` as follows:

.. code-block:: bash

    $ python3 -m pyqbdipreload <script> <target> [<args> ...]

with:

- ``script``: The Python script
- ``target``: The binary you are targeting
- ``args``: Argument(s) to be passed to the binary (if any)
