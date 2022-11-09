.. highlight:: javascript

Frida/QBDI
==========

QBDI can team up with Frida to be even more powerful together.
To be able to use QBDI bindings while injected into a process with Frida,
it is necessary to understand how to use Frida to perform some common tasks beforehand.
Through this simple example based on *qbdi-frida-template* (see :ref:`qbdi-frida-template`), we will explain a basic usage of Frida and QBDI.

Common tasks
------------

This section solely shows a few basic actions and gives you a general overview of what you can do with Frida.
Keep in mind that Frida offers many more awesome features, all listed in the `Javascript API documentation <https://frida.re/docs/javascript-api/>`_.

Read memory
+++++++++++

Sometimes it may be necessary to have a look at a buffer or specific part of the memory. We rely on Frida to do it.

.. code-block:: javascript

    var arrayPtr = ptr(0xDEADBEEF)
    var size = 0x80
    var buffer = Memory.readByteArray(arrayPtr, size)


Write memory
++++++++++++

We also need to be able to write memory:

.. code-block:: javascript

    var arrayPtr = ptr(0xDEADBEEF)
    var size = 0x80
    var toWrite = new Uint8Array(size);
    // fill your buffer eventually
    Memory.writeByteArray(arrayPtr, toWrite)


Allocate an array
+++++++++++++++++

If you have a function that takes a buffer or a string as an input, you might need to allocate a new buffer using Frida:

.. code-block:: javascript

    // allocate and write a 2-byte buffer
    var buffer = Memory.alloc(2);
    Memory.writeByteArray(buffer, [0x42, 0x42])
    // allocate and write an UTF8 string
    var str = Memory.allocUtf8String("Hello World !");


Initialise a VM object
++++++++++++++++++++++

If `frida-qbdi.js` (or a script requiring it) is successfully loaded in Frida,
a new :js:class:`VM` object becomes available.
It provides an object oriented access to the framework features.

.. code-block:: javascript

    // initialise QBDI
    var vm = new VM();
    console.log("QBDI version is " + vm.version.string);
    var state = vm.getGPRState();


Instrument a function with QBDI
+++++++++++++++++++++++++++++++

You can instrument a function using QBDI bindings. They are really close to the C++ ones,
further details about the exposed APIs are available in the :ref:`frida-qbdi-api` documentation.

.. code-block:: javascript

    var functionPtr = DebugSymbol.fromName("function_name").address;
    vm.addInstrumentedModule("demo.bin");

    var InstructionCallback = vm.newInstCallback(function(vm, gpr, fpr, data) {
        inst = vm.getInstAnalysis();
        gpr.dump(); // display the context
        console.log("0x" + inst.address.toString(16) + " " + inst.disassembly); // display the instruction
        return VMAction.CONTINUE;
    });
    var iid = vm.addCodeCB(InstPosition.PREINST, instructionCallback, NULL);

    vm.call(functionPtr, []);


If you ever want to pass custom arguments to your callback, this can be done via the **data** argument:

.. code-block:: javascript

    // this callback is used to count the number of basic blocks executed
    var userData = { counter: 0};
    var BasicBlockCallback = vm.newVMCallback(function(vm, evt, gpr, fpr, data) {
        data.counter++;
        return VMAction.CONTINUE;
    });
    vm.addVMEventCB(VMEvent.BASIC_BLOCK_ENTRY, BasicBlockCallback, userData);
    console.log(userData.counter);


Scripts
+++++++

Bindings can simply be used in Frida REPL, or imported in a Frida script, empowering
the bindings with all the `nodejs` ecosystem.

.. code-block:: javascript

    import { VM } from "./frida-qbdi.js";

    var vm = new VM();
    console.log("QBDI version is " + vm.version.string);

It will be possible to load it in Frida in place of `frida-qbdi.js`, allowing to
easily create custom instrumentation tools with in-process scripts written in
JavaScript and external control in Python (or any language supported by `Frida`).

Compilation
+++++++++++

In order to actually import QBDI bindings into your project, your script needs be *compiled* with the `frida-compile <https://www.npmjs.com/package/frida-compile>`_ utility.
Installing it requires you to have ``npm`` installed. The `babelify package <https://www.npmjs.com/package/babelify>`_ might be also needed.
Otherwise, you will not be able to successfully compile/load it and some errors will show up once running it with Frida.

Before running frida-compile, be sure that the script ``frida-qbdi.js`` is
inside you current directory.

.. code:: bash

    find <archive_extracted_path> -name frida-qbdi.js -exec cp {} . \;

    # if frida-compile is not already installed
    npm install frida-compile babelify
    ./node_modules/.bin/frida-compile MyScript.js -o MyScriptCompiled.js
    # else
    frida-compile MyScript.js -o MyScriptCompiled.js

Run Frida/QBDI on a workstation
+++++++++++++++++++++++++++++++

To use QBDI on an already existing process you can use the following syntax:

.. code:: bash

    frida -n processName -l MyScriptCompiled.js

You can also spawn the process using Frida to instrument it with QBDI as soon as it starts:

.. code:: bash

    frida -f binaryPath Arguments -l MyScriptCompiled.js

Run Frida/QBDI on an Android device
+++++++++++++++++++++++++++++++++++

Since Frida provides a great interface to instrument various types of target, we can also rely on it to use QBDI on Android, especially when it comes to inspecting applications.
Nevertheless, it has some specificities you need to be aware of.
Before running your script, make sure that:

- a Frida server is running on the remote device and is reachable from your workstation
- the ``libQBDI.so`` library has been placed in ``/data/local/tmp`` (available in `Android packages <https://github.com/QBDI/QBDI/releases/>`_)
- SELinux has been turned into `permissive` mode (through ``setenforce 0``)

Then, you should be able to inject your script into a specific Android application:

.. code:: bash

    # if the application is already running
    frida -Un com.app.example -l MyScriptCompiled.js

    # if you want to spawn the application
    frida -Uf com.app.example -l MyScriptCompiled.js

Concrete example
----------------

If you have already had a look at the default instrumentation of the template generated with *qbdi-frida-template* (see :ref:`qbdi-frida-template`), you are probably familiar with the following example.
Roughly speaking, what it does is creating a native call to the *Secret()* function, and instrument it looking for *XOR*.

Source code
+++++++++++

.. literalinclude:: ../../templates/qbdi_frida_template/SimpleXOR.c
    :language: C


Frida/QBDI script
+++++++++++++++++

.. literalinclude:: ../../templates/qbdi_frida_template/FridaQBDI_sample.js
    :language: javascript

.. _qbdi-frida-template:

Generate a template
-------------------

A QBDI template can be considered as a baseline project, a minimal component you can modify and build your instrumentation tool on.
They are provided to help you effortlessly start off a new QBDI based project.
If you want to get started using QBDI bindings, you can create a brand-new default project doing:

.. code:: bash

    make NewProject
    cd NewProject
    qbdi-frida-template

    # if you want to build the demo binary
    mkdir build && cd build
    cmake ..
    make

    # if frida-compile is not already installed
    npm install frida-compile babelify
    ./node_modules/.bin/frida-compile ../FridaQBDI_sample.js -o RunMe.js
    # else
    frida-compile ../FridaQBDI_sample.js -o RunMe.js

    frida -f ./demo.bin -l ./RunMe.js
