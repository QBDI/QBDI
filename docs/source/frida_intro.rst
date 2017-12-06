Introduction
============

Why Frida?
----------

QBDI is quite easy to use when it comes to embedding it inside a binary at compile time (see ``qbdi-template``),
but we had no easy way to instrument an already compiled binary. Even if multiple ways can be used to inject QBDI
in a process (from a simple LD_PRELOAD to a tailored injector), we can also rely on Frida core to inject our framework
into an existing process and manipulate QBDI from Frida interface.
Frida already supports various platforms (iOS, Android, Windows, macOS) making it a good choice for any cross platform tool.
We developed ``frida-qbdi.js`` that makes all QBDI bindings available through Frida.


How to use Frida / QBDI?
------------------------
To use QBDI on an already existing process you can use the following syntax:

::

    frida -n processName -l frida-qbdi.js

You can also spawn the process using Frida to instrument it with QBDI as soon as it starts:

::

    frida -f binaryPath Arguments -l frida-qbdi.js

If you want to get started using QBDI bindings, you can create a new default project doing:

::

    make NewProject
    cd NewProject
    frida-template
    mkdir build && cd build
    cmake ..
    make
    # If frida-compile is not installed
    npm install frida-compile babelify
    ./node_modules/.bin/frida-compile FridaQBDI_sample.js -o RunMe.js
    # else
    frida-compile ../FridaQBDI_sample.js -o ../RunMe.js
    frida ./demo.bin -l ../RunMe.js


You can find all the information about QBDI bindings in the next section along with a writeup that demonstrates what can be achieved using QBDI with Frida injection.

