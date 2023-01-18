#!/usr/bin/env python3

# This file is part of pyQBDI (python binding for QBDI).
#
# Copyright 2017 - 2022 Quarkslab
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

class OSXLibFixup:

    def __init__(self, libpath):
        self.libpath = libpath

    def read_header(self):
        p = subprocess.run(["otool", "-l", self.libpath],
                           check=True,
                           capture_output=True)
        newCommand = False
        getRPath = False
        rpath = []
        dlib = []
        for l in p.stdout.decode("utf8").split("\n"):
            l = l.strip(' ')
            if l.startswith("Load command"):
                newCommand = True
                getRPath = False
                continue
            if newCommand and l.startswith("cmd"):
                newCommand = False
                getRPath = l.endswith("LC_RPATH")
                continue
            if getRPath and l.startswith("path"):
                getRPath = False
                newPath = l.split(' ', 1)[1]
                if newPath[-1] == ")":
                    newPath = newPath[:newPath.rindex(' (')]
                rpath.append(newPath)
        return rpath

    def addRpath(self, path):
        subprocess.run(["install_name_tool", "-add_rpath", path, self.libpath],
                       check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNUL)

    def removeRpath(self, path):
        subprocess.run(["install_name_tool", "-delete_rpath", path, self.libpath],
                       check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNUL)

    def resign(self):
        subprocess.run(["codesign", "--force", "--sign", "-", self.libpath],
                       check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

    def fixMachOLib(self):

        rpaths = self.read_header()
        found_base_prefix = False
        needResign = False

        for rpath in rpaths:
            if rpath == sys.base_prefix:
                found_base_prefix = True
            else:
                self.removeRpath(rpath)

        if not found_base_prefix:
            self.addRpath(sys.base_prefix)
            needResign = True

        if needResign:
            self.resign()

def run():

    parser = argparse.ArgumentParser()

    parser.add_argument("script", type=str, help="PyQBDI script")
    parser.add_argument("target", type=str, help="command to instrument")
    parser.add_argument("args", type=str, help="command arguments", nargs='*')

    args = parser.parse_args()

    script = args.script
    binary = args.target
    args = [args.target] + args.args
    environ = os.environ.copy()

    preloadlib = os.path.join(
            os.path.dirname(pyqbdi.__file__),
            os.path.basename(pyqbdi.__file__).replace("pyqbdi", "pyqbdipreloadlib"))

    if not os.path.isfile(preloadlib):
        print("Cannot found pyqbdi preload library : {}".format(preloadlib))
        exit(1)

    # add LD_PRELOAD or DYLD_INSERT_LIBRARIES
    if platform.system() == 'Darwin':
        OSXLibFixup(preloadlib).fixMachOLib()

        environ["DYLD_INSERT_LIBRARIES"] = preloadlib
        environ["DYLD_BIND_AT_LAUNCH"] = "1"
    elif platform.system() == 'Linux':
        libpythonname = "python{}.{}".format(sys.version_info.major, sys.version_info.minor)
        libpython = ctypesutil.find_library(libpythonname)
        if not libpython:
            libpythonname = "python{}.{}{}".format(sys.version_info.major, sys.version_info.minor, sys.abiflags)
            libpython = ctypesutil.find_library(libpythonname)
            if not libpython:
                print("PyQBDI in PRELOAD mode need lib{}.so".format(libpythonname))
                exit(1)

        environ["LD_PRELOAD"] = os.pathsep.join([libpython, preloadlib])
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
    if '/' in binary:
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
