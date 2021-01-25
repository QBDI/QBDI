Installation and Integration
============================


C and C++ API Installation
--------------------------

Packages are pre-build at each release and can be downloaded in `github release page <https://github.com/QBDI/QBDI/releases>`_.

You can verify the integrity of your package with the file ``SHA256``. A GPG signature of ``SHA256`` by
our `developer key (2763 2215 DED8 D717 AD08 477D 874D 3F16 4D45 2193) <https://qbdi.quarkslab.com/qbdi.asc>`_ is available in ``SHA256.sig``.

.. code-block:: bash

    # verify the hash of the prebuild package
    sha256 -c SHA256 # verify the hash of your package

    # verify the signature
    wget https://qbdi.quarkslab.com/qbdi.asc -O - | gpg --import -
    gpg --verify SHA256.sig


Debian / Ubuntu
+++++++++++++++

Debian and Ubuntu packages are provided for stable and LTS releases, and can be installed using ``dpkg``:

.. code-block:: bash

    $ dpkg -i QBDI-*-*-X86_64.deb


Arch Linux
++++++++++

Archlinux packages can be installed using ``pacman``:

.. code-block:: bash

    $ pacman -U QBDI-*-*-X86_64.tar.zst


macOS
+++++

A software installer is provided for macOS. Opening the ``.pkg`` in Finder and following the
instructions should install QBDI seamlessly.


Windows
+++++++

A software installer is provided for Windows. Running the ``.exe`` and following the instructions
should install QBDI seamlessly.


Android
+++++++

The android package is an archive to extract in your work directory.

Devel Package
+++++++++++++

Devel packages can be downloaded to test the latest feature before the release.

- `Windows devel package <https://ci.appveyor.com/project/QBDI/qbdi/branch/dev-next>`_
- `Linux (ubuntu based) devel package <https://github.com/QBDI/QBDI/actions?query=workflow%3A%22Tests+and+Package+Linux%22+branch%3Adev-next>`_
- `Android devel package <https://github.com/QBDI/QBDI/actions?query=workflow%3A%22Package+Android%22+branch%3Adev-next>`_

PyQBDI installation
-------------------

The last release of PyQBDI is pushed on Pypi and can be installed with pip (>=19.3)

.. code-block:: bash

    pip install --user --update pip
    pip install --user PyQBDI

To use a devel version, download the associated wheel and run

.. code-block:: bash

    pip install --user --update pip
    pip install --user PyQBDI-*.whl

A devel wheel with last commits of dev-next is available:

- `PyQBDI for windows <https://ci.appveyor.com/project/QBDI/qbdi/branch/dev-next>`_
- `PyQBDI for Linux  <https://github.com/QBDI/QBDI/actions?query=workflow%3A%22PyQBDI+Linux+package%22+branch%3Adev-next>`_

.. note::
   Only python3 is supported. If you want to use python2, please use QBDI 0.7.0.

.. note::
   A version 32 bits of python is needed for PyQBDI with x86 target.

Frida/QBDI installation
-----------------------

Frida/QBDI is included in the C/C++ package.

Frida/QBDI needs an installation of Frida (>= 14.0) on the target and frida-compile to compile the script.

.. code-block:: bash

    pip install frida-tools

    # install frida-compile and add it in the PATH
    npm install frida-compile babelify
    export PATH=$PATH:$(pwd)/node_modules/.bin

Android target
++++++++++++++

To use Frida/QBDI on android device, ``frida-server`` must run on the target device and ``libQBDI.so`` should be placed in ``/data/local/tmp``.


Docker Linux Images
-------------------

The docker image is available in `Docker Hub <https://hub.docker.com/r/qbdi/qbdi>`_. The image is
minimal and does not contain any compiler. You need to install the needed application or adapt the
following dockerfile.

.. literalinclude:: ../../examples/Dockerfile
   :language: dockerfile

To run the container, we recommend allowing the usage of PTRACE that is necessary to use QBDIPreload.

.. code-block:: bash

    $ docker run -it --rm --cap-add=SYS_PTRACE --security-opt seccomp:unconfined <image> bash


Compilation from source code
----------------------------

.. include:: ../../README.rst
    :start-after: .. compil
    :end-before: .. compil-end

CMake Integration
-----------------

One architecture
++++++++++++++++

If you want to use only one architecture of QBDI in your CMake project, you can import the package ``QBDI`` and ``QBDIPreload``:

.. code-block:: cmake

    find_package(QBDI REQUIRED)
    find_package(QBDIPreload REQUIRED) # if available for the platform
    # or
    find_package(QBDI REQUIRED HINTS "${EXTRACT_DIRECTORY}" NO_DEFAULT_PATH)
    find_package(QBDIPreload REQUIRED HINTS "${EXTRACT_DIRECTORY}" NO_DEFAULT_PATH)

Once the package is found, you can link your executable with the dynamic or the static library:

.. code-block:: cmake

    add_executable(example example.c)

    target_link_libraries(example QBDI::QBDI)
    # or
    target_link_libraries(example QBDI::QBDI_static)

    add_executable(example_preload example_preload.c)
    target_link_libraries(example_preload QBDI::QBDI_static QBDIPreload::QBDIPreload)


Multi-architectures
+++++++++++++++++++

If two or more architectures of QBDI are used in the same project, you should import the package ``QBDI<arch>`` and ``QBDIPreload<arch>``.

.. code-block:: cmake

    find_package(QBDIX86 REQUIRED HINTS "${EXTRACT_DIRECTORY_X86}" NO_DEFAULT_PATH)
    find_package(QBDIPreloadX86 REQUIRED HINTS "${EXTRACT_DIRECTORY_X86}" NO_DEFAULT_PATH)

    add_executable(example_preload86 example_preload.c)
    set_target_properties(example_preload86 PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    target_link_libraries(example_preload86 QBDI::X86::QBDI_static QBDIPreload::X86::QBDIPreload)

    find_package(QBDIX86_64 REQUIRED HINTS "${EXTRACT_DIRECTORY_X86_64}" NO_DEFAULT_PATH)
    find_package(QBDIPreloadX86_64 REQUIRED HINTS "${EXTRACT_DIRECTORY_X86_64}" NO_DEFAULT_PATH)

    add_executable(example_preload64 example_preload.c)
    target_link_libraries(example_preload64 QBDI::X86_64::QBDI_static QBDIPreload::X86_64::QBDIPreload)



