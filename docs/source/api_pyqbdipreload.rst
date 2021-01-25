.. _pyqbdipreload_api:

PyQBDIPreload API
=================

Introduction
------------

PyQBDIPreload is composed of two components:

- QBDIPreload script included in PyQBDI library.
  It will load the python runtime and executes the user script to instrument the target.
- A injector script `pyqbdipreload.py` that set up the environment variable to inject the library and the Python runtime.

.. note::
    PyQBDIPreload shares the limitations of QBDIPreload and PyQBDI.

User Callback
-------------

.. function:: pyqbdipreload_on_run(vm: pyqbdi.VM, start: int, end: int)

    This function must be present in the preloaded script

    :param VM vm:     VM Instance
    :param int start: Start address of the range (included).
    :param int end:   End address of the range (excluded).

QBDIPreload step
----------------

:cpp:func:`qbdipreload_on_start` and :cpp:func:`qbdipreload_on_premain` are not available with PyQBDIPreload.

The python interpreter and the preloaded script are loaded during :cpp:func:`qbdipreload_on_main`.

The method :py:func:`pyqbdipreload_on_run` is called during :cpp:func:`qbdipreload_on_main`.

The interpreter is shutdown during :cpp:func:`qbdipreload_on_exit`, after calling all registered methods of
`atexit <https://docs.python.org/fr/3/library/atexit.html>`_ module.

PyQBDIPreload script
--------------------

The usage of `pyqbdipreload.py` is the follow:

.. code-block:: bash

    $ python3 -m pyqbdipreload <script> <target> [<args> ...]

with:

- ``script``: The python script to preload.
- ``target``: The binary to execute.
- ``args``: Any arguments to be provided to the binary
