Repository Organization
=======================

Root Tree
---------

The root of the source tree is organized as follows:

``cmake/``
   Contains files required by the CMake build system.

``docker/``
   Contains the Dockerfile for the docker image and the CI.

``docs/``
   Contains this documentation source and configuration.

``examples/``
   Contains various QBDI usage examples.

``include/``
   Contains the public include files.

``package/``
   Contains the package generation scripts.

``src/``
   Contains QBDI source code. This tree is described further in section :ref:`source-tree`.

``templates/``
   Contains QBDI usage templates.

``test/``
   Contains the functional test suite.

``third-party/``
   Contains the third party dependency downloaded by cmake.

``tools/``
   Contains QBDI development tools: the validator and the validation runner.

.. _source-tree:

Source Tree
-----------

The source files are organized as follows:

``src/Engine``
   Contains code related to the VM API and underlying Engine controlling the overall execution.

``src/ExecBlock``
   Contains code used to generate, store and execute JITed code inside ExecBlocks which form a code
   cache managed by the ExecBlockManager.

``src/ExecBroker``
   Contains code implementing the execution brokering mechanism allowing to switch between
   instrumented execution and real execution.

``src/Patch``
   Contains the PatchDSL implementation and per architecture support.

``src/Utility``
   Contains various utility class and functions.
