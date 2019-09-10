PyQBDI
======

PyQBDI is a set of bindings that you can interact with through the use
of a preloaded library using **LD_PRELOAD** (linux) or **DYLD_INSERT_LIBRARIES** (macOS).
PyQBDI offers you a way to script your instrumentation, allowing a fast and easy way to design tools.
It also lets you interact with python environment, so you can, for example, post process any data
you collected during the instrumentation.

.. note::
   Because PyQBDI is a bindings on top of QBDIPreload, it has the same limitations as QBDIPreload
   which are describred in :ref:`qbdipreload`.

.. note::
   It is not possible to instrument a python process using PyQBDI because there will be a conflict
   between the **host** and the **guest** both trying to use the python runtime as described in
   :ref:`intro_limitations`. We would recommend directly using :ref:`qbdipreload`.

.. note::
   Only python2 is currently supported. A version 32 bits of python is needed for
   PyQBDI in x86.

We provide examples along with the API documentation in the following sections.

.. toctree::
   :maxdepth: 2

   Get started      <pyQBDI_usage>
   Bindings         <pyQBDI_bindings>

