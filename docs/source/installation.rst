Installation
============

QBDI can be installed using our prebuilt packages or from the source. For a more relaxing experience, we 
recommend using the stable release’s prebuilt packages, following per-platform steps detailed on this page. In order to install 
from source please refer to the developer documentation: :ref:`developer-compilation`.

Using Pre-Built Packages
------------------------

Two different kinds of packages exist:

- systemwide installation packages
- archive packages

Systemwide packages install headers and binaries in system directories. Archives are created to be 
extracted and used in a local folder. They are provided for targets where systemwide install is not 
supported yet or doesn't make sense (like cross-compiled targets).

Debian / Ubuntu
+++++++++++++++

Debian and Ubuntu packages are provided for stable and LTS releases, and can be installed using ``dpkg``:

.. code-block:: bash

    $ dpkg -i QBDI-*-*-X86_64.deb


Arch Linux
++++++++++

Archlinux packages can be installed using ``pacman``:

.. code-block:: bash

    $ pacman -U QBDI-*-*-X86_64.tar.xz


macOS
+++++

A software installer is provided for macOS. Opening the ``.pkg`` in Finder and following the 
instructions should install QBDI seamlessly.


Windows
+++++++

A software installer is provided for Windows. Running the ``.exe`` and following the instructions 
should install QBDI seamlessly.

.. warning:: Do not modify the default installation path. If you do so you will need to fix frida-qbdi.js with the new path.


Testing Your Installation
-------------------------

A QBDI template project is distributed along binaries and headers. On archive package targets, it is 
located in the ``template`` root-directory. On systemwide targets, a shell command is provided to 
populate a directory with this template:

.. code-block:: bash

    $ mkdir test
    $ cd test
    $ qbdi-template

The template consists in a sample annotated source code with a basic CMake build script. A  
``README`` is also provided with simple steps to compile the template. Successful compilation of 
the template and execution of the resulting binary should produce something similar:

.. code-block:: bash

    $ ./qbdi_template 
    0x556a4a023e0a:     push    rbp
    0x556a4a023e0b:     mov rbp, rsp
    0x556a4a023e0e:     mov dword ptr [rbp - 4], edi
    0x556a4a023e11:     mov eax, dword ptr [rbp - 4]
    0x556a4a023e14:     xor eax, 92
    0x556a4a023e17:     pop rbp
    0x556a4a023e18:     ret
    [*] retval=0x2c6
