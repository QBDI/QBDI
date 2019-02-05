PyQBDI Usage
============

Simple example
--------------

Using python bindings allows you to process information from the instrumentation in python.
Here is a simple example, where we define a callback set on all instructions that displays
their address and disassembly.

.. code-block:: python

    #!/usr/bin/env python2
    # -*- coding: utf-8 -*-

    import pyqbdi

    def mycb(vm, gpr, fpr, data):
        inst = vm.getInstAnalysis()
        print "0x%x: %s" % (inst.address, inst.disassembly)
        return pyqbdi.CONTINUE

    def pyqbdipreload_on_run(vm, start, stop):
        vm.addCodeCB(pyqbdi.PREINST, mycb, None)
        vm.run(start, stop)


How to run it?
--------------

To be able to run pyQBDI you need to preload **libpyqbdi** library (using either ``LD_PRELOAD`` under
**linux** or ``DYLD_INSERT_LIBRARIES`` under **macOS**).
You also need to specify the path to your python tool using the environment variable **PYQBDI_TOOL**.

On linux:

.. code-block:: bash

    LD_PRELOAD=/usr/local/lib/libpyqbdi.so PYQBDI_TOOL=./example.py /usr/bin/id


On macOS:

.. code-block:: bash

    DYLD_INSERT_LIBRARIES=/usr/local/lib/libpyqbdi.dylib PYQBDI_TOOL=./example.py /usr/bin/id


.. note:: We will soon provide a loader script, making the use of pyQBDI easier.


An in-depth example
-------------------

If the previous example was not enough, here is an extensive test code showcasing current pyqbdi capabilities.
This example define multiple callbacks, based on events, instructions, and mnemonic. Those are instrumenting the **sin()** native function and its result is being compared with the python result.

.. literalinclude:: ../../examples/pyqbdi/trace_sin.py
    :language: python
