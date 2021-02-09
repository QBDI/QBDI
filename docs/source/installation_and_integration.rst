Installation and Integration
============================


C/C++ API installation
----------------------

When a new stable version of QBDI is released, prebuilt packages can be downloaded through the `release page <https://github.com/QBDI/QBDI/releases>`_ on Github. In order to make sure you have downloaded the right package, you can verify its integrity with the file ``SHA256``.

A GPG signature of ``SHA256`` by our `developer key (2763 2215 DED8 D717 AD08 477D 874D 3F16 4D45 2193) <https://qbdi.quarkslab.com/qbdi.asc>`_ is available in ``SHA256.sig``.

.. code-block:: bash

    # check the hash of the prebuilt package
    sha256 -c SHA256

    # verify the signature
    wget https://qbdi.quarkslab.com/qbdi.asc -O - | gpg --import -
    gpg --verify SHA256.sig


Debian / Ubuntu
+++++++++++++++

Debian and Ubuntu packages are provided for stable and LTS releases, and can be installed using ``dpkg``:

.. code-block:: bash

    dpkg -i QBDI-*-*-X86_64.deb


Arch Linux
++++++++++

Arch Linux packages can be installed using ``pacman``:

.. code-block:: bash

    pacman -U QBDI-*-*-X86_64.tar.zst


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

The Android package is an archive you solely need to extract.
Afterwards, you have to manually push the files onto the device.

Devel packages
++++++++++++++

Devel packages embed the latest features the developers are currently working on for the next release (available on the `dev-next branch <https://github.com/QBDI/QBDI/tree/dev-next/>`_).
It's worth mentioning that since some parts are still under development, those are likely to be **unstable** -- you must be aware that it may contain some bugs and are not as reliable as release packages.

- `Windows devel packages <https://ci.appveyor.com/project/QBDI/qbdi/branch/dev-next>`_
- `Linux (Ubuntu) devel packages <https://github.com/QBDI/QBDI/actions?query=workflow%3A%22Tests+and+Package+Linux%22+branch%3Adev-next>`_
- `Android devel packages <https://github.com/QBDI/QBDI/actions?query=workflow%3A%22Package+Android%22+branch%3Adev-next>`_
- `OSX devel packages <https://github.com/QBDI/QBDI/actions?query=workflow%3A%22Tests+and+Package+OSX%22+branch%3Adev-next>`_

PyQBDI installation
-------------------

Every time a new stable release of PyQBDI is available, it is automatically pushed onto the `PyPI platform <https://pypi.org/project/PyQBDI/>`_, thus can be easily installed with ``pip`` (>= 19.3).

.. code-block:: bash

    pip install --user --update pip
    pip install --user PyQBDI

If you want to use a devel version, download the corresponding prebuilt wheel file and run the following commands:

.. code-block:: bash

    pip install --user --update pip
    pip install --user PyQBDI-*.whl

The devel wheel files which contain the latest versions of the `dev-next branch <https://github.com/QBDI/QBDI/tree/dev-next/>`_ are available at:

- `PyQBDI for Windows <https://ci.appveyor.com/project/QBDI/qbdi/branch/dev-next>`_
- `PyQBDI for Linux <https://github.com/QBDI/QBDI/actions?query=workflow%3A%22PyQBDI+Linux+package%22+branch%3Adev-next>`_
- `PyQBDI for OSX <https://github.com/QBDI/QBDI/actions?query=workflow%3A%22PyQBDI+OSX+package%22+branch%3Adev-next>`_

.. note::
   Only Python3 is supported. If you need to use Python2, we recommend using QBDI 0.7.0 instead.

.. note::
   A 32-bit version of Python is needed if you want to use PyQBDI on x86 targets.

Frida/QBDI installation
-----------------------

QBDI can be used alongside Frida to make it even more powerful. This feature is included in the C/C++ package.
Using it requires having Frida installed (>= 14.0) on your workstation as well as `frida-compile <https://www.npmjs.com/package/frida-compile>`_ for compiling your scripts.

.. code-block:: bash

    # install Frida
    pip install frida-tools

    # install frida-compile and add the binary directory to your PATH env variable
    npm install frida-compile babelify
    export PATH=$PATH:$(pwd)/node_modules/.bin

Android target
++++++++++++++

In order to use Frida/QBDI on an Android device, the Frida server must be running on the target device and the ``libQBDI.so`` library have to be placed in ``/data/local/tmp``.

.. note::
   Latest Frida server binaries are available on the `Frida official release page <https://github.com/frida/frida/releases>`_.

Docker Linux images
-------------------

The Docker image is available on `Docker Hub <https://hub.docker.com/r/qbdi/qbdi>`_.
It has been built to keep it as small as possible so it does not contain any compiler.
You have to install the needed application or modify the following Dockerfile according to your needs.

.. literalinclude:: ../../examples/Dockerfile
   :language: dockerfile

To run the container, we recommend allowing the usage of PTRACE which is mandatory to use QBDIPreload.

.. code-block:: bash

    docker run -it --rm --cap-add=SYS_PTRACE --security-opt seccomp:unconfined <image> bash

Compilation from source code
----------------------------

.. include:: ../../README.rst
    :start-after: .. compil
    :end-before: .. compil-end

CMake integration
-----------------

Single architecture
+++++++++++++++++++

If you want to use only one architecture of QBDI in your CMake project, you can import the ``QBDI`` and ``QBDIPreload`` packages:

.. code-block:: cmake

    find_package(QBDI REQUIRED)
    find_package(QBDIPreload REQUIRED) # if available for your current platform
    # or
    find_package(QBDI REQUIRED HINTS "${EXTRACT_DIRECTORY}" NO_DEFAULT_PATH)
    find_package(QBDIPreload REQUIRED HINTS "${EXTRACT_DIRECTORY}" NO_DEFAULT_PATH)

Once the CMake package is found, you can link your executable either with the dynamic or the static library:

.. code-block:: cmake

    add_executable(example example.c)

    target_link_libraries(example QBDI::QBDI)
    # or
    target_link_libraries(example QBDI::QBDI_static)

    add_executable(example_preload example_preload.c)
    target_link_libraries(example_preload QBDI::QBDI_static QBDIPreload::QBDIPreload)

Multi-architecture
++++++++++++++++++

If two or more architectures of QBDI are needed within the same project, you should import the package ``QBDI<arch>`` and ``QBDIPreload<arch>``.

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
