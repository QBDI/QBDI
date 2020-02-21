PyQBDI Usage
============

Limitations
-----------

pyQBDI has some limitations that need to be considered before using those bindings :

- The library can handle only one VM at time.
- The VM must not be used in atexit module.

PyQBDI as a preloaded library
-----------------------------

PyQBDI can be used as a preloaded library to instrument a whole binary, as QBDIPreload.

PyQBDI provided the helper script ``pyqbdipreload`` that preload the library in the new process.
``pyqbdipreload`` is currently supported on Linux and macOS.

.. code-block:: bash

    $ python3 -m pyqbdipreload ./example.py cat anyfile

The following actions are performed with the API of QBDIPreload :

- :cpp:func:`QBDI::qbdipreload_on_main`:
  The python script is loaded. The program arguments can be found in ``sys.argv``.

- :cpp:func:`QBDI::qbdipreload_on_run`: The method ``pyqbdipreload_on_run`` is called.
  This method must be implemented in the specified script.

- :cpp:func:`QBDI::qbdipreload_on_exit`: The module ``atexit`` is triggered once.

Example
~~~~~~~

Here is a simple example, where we define a callback set on all instructions that displays
their address and disassembly.

.. code-block:: python

    #!/usr/bin/env python
    # -*- coding: utf-8 -*-

    import pyqbdi

    def mycb(vm, gpr, fpr, data):
        inst = vm.getInstAnalysis()
        print("0x{:x}: {}".format(inst.address, inst.disassembly))
        return pyqbdi.CONTINUE

    def pyqbdipreload_on_run(vm, start, stop):
        vm.addCodeCB(pyqbdi.PREINST, mycb, None)
        vm.run(start, stop)

PyQBDI as a python library
--------------------------

When PyQBDI is used as a python library, the instrumentation code can be loaded
in the process with ``ctypes`` python module.

Example
~~~~~~~

This example defines multiple callbacks, based on events, instructions, and mnemonics.
Those are instrumenting the **sin()** native function and its result is
being compared with the python result.

.. literalinclude:: ../../examples/pyqbdi/trace_sin.py
    :language: python




