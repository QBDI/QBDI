#!/usr/bin/env python3

import platform
import pyqbdi
from ctypes import util as ctypesutil
import sys
import os


def run():

    if len(sys.argv) <= 2 or sys.argv[1] in ['-h', '--help']:
        print("Usage: python -m {} <script> <target> [<args> ...]".format(sys.argv[0]))
        exit(1)

    script = sys.argv[1]
    binary = sys.argv[2]
    args = sys.argv[2:]
    environ = os.environ.copy()

    # add LD_PRELOAD or DYLD_INSERT_LIBRARIES
    if platform.system() == 'Darwin':
        environ["DYLD_INSERT_LIBRARIES"] = pyqbdi.__file__
    elif platform.system() == 'Linux':
        libpythonname = "python{}.{}".format(sys.version_info.major, sys.version_info.minor)
        libpython = ctypesutil.find_library(libpythonname)
        if not libpython:
            libpythonname = "python{}.{}{}".format(sys.version_info.major, sys.version_info.minor, sys.abiflags)
            libpython = ctypesutil.find_library(libpythonname)
            if not libpython:
                print("PyQBDI in PRELOAD mode need lib{}.so".format(libpythonname))
                exit(1)

        environ["LD_PRELOAD"] = os.pathsep.join([libpython, pyqbdi.__file__])
        environ["LD_BIND_NOW"] = "1"
    else:
        print("PyQBDI in PRELOAD mode is not supported on this platform")
        exit(1)

    # add PYQBDI_TOOL
    if not os.path.isfile(script):
        print("Cannot find {} script".format(script))
        exit(1)
    else:
        environ["PYQBDI_TOOL"] = script

    ## https://docs.python.org/3.8/library/os.html#os.execve:
    # "execve will not use the PATH variable to locate the executable; path must contain an appropriate absolute or relative path."
    # seach the binary path in PATH if needed
    binarypath = None
    if (    binary.startswith('/') or
            binary.startswith('./') or
            binary.startswith('../')):
        # absolute or relative path
        binarypath = binary
    else:
        if "PATH" in environ:
            for p in environ["PATH"].split(os.pathsep):
                if os.path.isfile(os.path.join(p, binary)):
                    binarypath = os.path.join(p, binary)
                    break

    if not binarypath or not os.path.isfile(binarypath):
        print("Cannot find binary {}".format(binary))
        exit(1)

    os.execve(binarypath, args, environ)

    print("Fail execve")
    exit(1)

if __name__ == "__main__":
    run()
