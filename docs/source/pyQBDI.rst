PyQBDI
======

PyQBDI is a set of bindings of QBDI for python. You can use them like any python library or like
a preloaded library (with **LD_PRELOAD** or **DYLD_INSERT_LIBRARIES**).

PyQBDI offers you a way to script your instrumentation, allowing a fast and easy way to design tools.
It also lets you interact with python environment, so you can for example, post process any data
you collected during the instrumentation.

PyQBDI is available on `Pypi <https://pypi.org/project/PyQBDI/>`_ and can be installed with the following command:

.. code-block:: bash

    pip install --user PyQBDI

.. note::
   If you use PyQBDI like a preloaded library, the library has the same limitations as QBDIPreload
   which are describred in :ref:`qbdipreload`.
   Additionnaly, the shared library libpython3.x.so must be installed (it is not included within the python package in some linux distribution).

.. note::
   It is not possible to instrument a python process using PyQBDI because there will be a conflict
   between the **host** and the **guest** both trying to use the python runtime as described in
   :ref:`intro_limitations`. We would recommend directly using :ref:`qbdipreload`.

.. note::
   Only python3 is supported. If you want to use python2, please use QBDI 0.7.0.

.. note::
   A version 32 bits of python is needed for PyQBDI in x86.

We provide examples along with the API documentation in the following sections.

.. toctree::
   :maxdepth: 2

   Get started      <pyQBDI_usage>
   Bindings         <pyQBDI_bindings>
