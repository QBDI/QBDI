PyQBDI Usage
============

Limitations
-----------

To avoid error when use PyQBDI, the binding must be use with the follow restriction:

- The library can handle only one VM at time.
- The VM must not be used in atexit module.

PyQBDI as a preloaded library
-----------------------------

PyQBDI can be used as a preloaded library to instrument a whole binary, as QBDIPreload.

The follow action are performed with the API of QBDIPreload :

- :cpp:func:`QBDI::qbdipreload_on_main`:
  The scipt specified in ``PYQBDI_TOOL`` is loaded with ``__name__`` set to ``pyqbdi.preload_script``. The program argument can be found in ``sys.argv``.
  To avoid infinite loop with some python library, the environment variable ``LD_PRELOAD`` or ``DYLD_INSERT_LIBRARIES`` is removed from ``os.environ``
  before the loading of the script.

- :cpp:func:`QBDI::qbdipreload_on_run`: The method ``pyqbdipreload_on_run`` is called. This method must be implemented in the specified script.

- :cpp:func:`QBDI::qbdipreload_on_exit`: The module ``atexit`` is trigger once.

Exemple
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

How to run it?
~~~~~~~~~~~~~~

To be able to run pyQBDI you need to preload **pyqbdi** library (using either ``LD_PRELOAD`` under
**linux** or ``DYLD_INSERT_LIBRARIES`` under **macOS**).
You also need to specify the path to your python tool using the environment variable **PYQBDI_TOOL**.

On linux:

.. code-block:: bash

    LD_PRELOAD=/usr/local/lib/python3.x/site-package/pyqbdi.so PYQBDI_TOOL=./example.py /usr/bin/id


On macOS:

.. code-block:: bash

    DYLD_INSERT_LIBRARIES=/usr/local/lib/python3.x/site-package/pyqbdi.so PYQBDI_TOOL=./example.py /usr/bin/id

The location of **pyqbdi** library can be found with the command:

.. code-block:: bash

    python3 -c 'import pyqbdi; print(pyqbdi.__file__)'


.. note:: We will soon provide a loader script, making the use of pyQBDI easier.


PyQBDI as a python library
--------------------------

When PyQBDI is used as a python library, the code to instrument can be load in the process with ``ctypes`` python module.

Exemple
~~~~~~~

This example define multiple callbacks, based on events, instructions, and mnemonic. Those are instrumenting the **sin()** native function and its result is being compared with the python result.

.. literalinclude:: ../../examples/pyqbdi/trace_sin.py
    :language: python




