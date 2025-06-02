#!/usr/bin/env python3

# This file is part of pyQBDI (python binding for QBDI).
#
# Copyright 2017 - 2025 Quarkslab
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import platform
import pyqbdi
from ctypes import util as ctypesutil
import sys
import os
import argparse
import subprocess

def get_preloadlib():
    if platform.system() == 'Windows':
        import importlib
        # ['.py', '.pyw', '.pyc', '.cp310-win_amd64.pyd', '.pyd']
        # find correct extension on windows like .cp310-win_amd64.pyd
        for ext in importlib.machinery.all_suffixes():
            preloadlib = os.path.join(
                os.path.dirname(pyqbdi.__file__),
                f"pyqbdipreloadlib{ext}")
            if os.path.isfile(preloadlib):
                return preloadlib
    else:
        preloadlib = os.path.join(
            os.path.dirname(pyqbdi.__file__),
            os.path.basename(pyqbdi.__file__).replace("pyqbdi", "pyqbdipreloadlib"))

    if not os.path.isfile(preloadlib):
        print("Cannot find pyqbdi preload library : {}".format(preloadlib))
        exit(1)
    return preloadlib

def find_binary(environ, binary, canFail=False):
    # https://docs.python.org/3.8/library/os.html#os.execve:
    # "execve will not use the PATH variable to locate the executable; path must contain an appropriate absolute or relative path."
    # seach the binary path in PATH if needed
    binarypath = None
    if '/' in binary or '\\' in binary:
        # absolute or relative path
        binarypath = binary
    else:
        if os.path.isfile(binary):  # case for current dir on windows
            return binary
        if "PATH" in environ:
            for p in environ["PATH"].split(os.pathsep):
                if os.path.isfile(os.path.join(p, binary)):
                    binarypath = os.path.join(p, binary)
                    break

    if not binarypath or not os.path.isfile(binarypath):
        if canFail:
            return None
        print("Cannot find binary {} make sure its in your PATH env".format(binary))
        exit(1)

    return binarypath

def find_windows_inject_binary(environ):

    listPreloader = [
            os.path.join(os.path.dirname(pyqbdi.__file__), "pyqbdiWinPreloader.exe"),
            "QBDIWinPreloader.exe"]

    for binary in listPreloader:
        binarypath = find_binary(environ, binary, True)
        if binarypath is not None:
            return binarypath

    print("Cannot find Windows injector binary (within {}), make sure one of them are available".format(listPreloader))
    exit(1)


def run():

    parser = argparse.ArgumentParser()

    parser.add_argument("script", type=str, help="PyQBDI script")
    parser.add_argument("target", type=str, help="command to instrument")
    parser.add_argument("args", type=str, help="command arguments", nargs='*')

    args = parser.parse_args()

    script = args.script
    binary = args.target
    execargs = [args.target] + args.args
    environ = os.environ.copy()

    preloadlib = get_preloadlib()
    binarypath = find_binary(environ, binary)

    # add LD_PRELOAD or DYLD_INSERT_LIBRARIES
    if platform.system() == 'Darwin':

        environ["DYLD_INSERT_LIBRARIES"] = preloadlib
        environ["DYLD_LIBRARY_PATH"] = os.path.join(sys.base_prefix, 'lib')
        environ["DYLD_BIND_AT_LAUNCH"] = "1"
    elif platform.system() == 'Linux':
        libpythonname = "python{}.{}".format(
            sys.version_info.major, sys.version_info.minor)
        libpython = ctypesutil.find_library(libpythonname)
        if not libpython:
            libpythonname = "python{}.{}{}".format(
                sys.version_info.major, sys.version_info.minor, sys.abiflags)
            libpython = ctypesutil.find_library(libpythonname)
            if not libpython:
                print("PyQBDI in PRELOAD mode need lib{}.so".format(libpythonname))
                exit(1)

        for s in [libpython, preloadlib]:
            for c in [" ", ";", os.pathsep]:
                if c in s:
                    print(f"Error: found character '{c}' in {s}")
                    print(f"PyQBDIPrelaod is based on ld.so, which didn't accept '{c}' in LD_PRELOAD.")
                    print("Please check your installation to avoid ' ' and ';' in file path")
                    exit(1)


        environ["LD_PRELOAD"] = os.pathsep.join([libpython, preloadlib])
        environ["LD_BIND_NOW"] = "1"
    elif platform.system() == 'Windows':
        targetbinary = binarypath
        binarypath = find_windows_inject_binary(environ)
        execargs = [binarypath, preloadlib, targetbinary] + execargs[1:]
    else:
        print("PyQBDI in PRELOAD mode is not supported on this platform")
        exit(1)

    # add PYQBDI_TOOL
    if not os.path.isfile(script):
        print("Cannot find {} script".format(script))
        exit(1)
    else:
        environ["PYQBDI_TOOL"] = script

    if platform.system() == 'Windows':
        proc = subprocess.run(execargs, env=environ)
        exit(proc.returncode)
    else:
        os.execve(binarypath, execargs, environ)
        print("Fail execve")
        exit(1)


if __name__ == "__main__":
    run()
