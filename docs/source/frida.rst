.. _frida-bindings:

Frida/QBDI
============

`Frida/QBDI` is a full-featured support of `QBDI` inside `Frida`.
It can be used in ways similar to `Frida Stalker`, while leveraging
all the power of `QBDI`.

Integration aims to be as tight as possible with Frida spirit,
with instrumentation callbacks that can be written in JavaScript,
calls performed directly on `Frida NativeFunction` objects, bindings imported
as `nodejs` scripts (`frida-compile` support), etc.

.. note::
   frida-qbdi.js is a ES2019 script. Frida lesser than 14.0 is not supported.

.. toctree::
    :maxdepth: 2

    Introduction <frida_intro>
	Get Started <frida_usage>
    JavaScript API <frida_bindings>

