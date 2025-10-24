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

import os
import re
import sys
import platform
import subprocess
import shutil
try:
    import ninja
    HAS_NINJA = True
except:
    HAS_NINJA = False

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from packaging.version import Version

def detect_QBDI_platform():
    current_os = None
    arch = None
    if hasattr(sys.implementation, "_multiarch"):
        if '-' in sys.implementation._multiarch:
            base_arch, base_os = sys.implementation._multiarch.split('-')[:2]
        else:
            base_arch = platform.machine()
            base_os = sys.implementation._multiarch
    else:
        base_arch = platform.machine()
        base_os = platform.system()

    base_arch = base_arch.lower()
    base_os = base_os.lower()

    if base_os == 'darwin':
        current_os = 'osx'
    elif base_os == 'windows':
        current_os = 'windows'
    elif base_os == 'linux':
        current_os = 'linux'

    if base_arch in ['amd64', 'amd', 'x64', 'x86_64', 'x86', 'i386', 'i686']:
        # intel arch
        if sys.maxsize > 2**32:
            arch = "X86_64"
        else:
            arch = "X86"

    elif base_arch in ['aarch64', 'arm64', 'aarch64_be', 'armv8b', 'armv8l']:
        assert sys.maxsize > 2**32
        arch = "AARCH64"

    elif base_arch == "arm" or \
         platform.machine().startswith('armv4') or \
         platform.machine().startswith('armv5') or \
         platform.machine().startswith('armv6') or \
         platform.machine().startswith('armv7'):
        assert sys.maxsize < 2**32
        arch = "ARM"

    if current_os and arch:
        return (current_os, arch)

    raise RuntimeError("Cannot determine the QBDI platform : system={}, machine={}, is64bits={}".format(
                            base_arch, base_os, sys.maxsize > 2**32))


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)

class CMakeBuild(build_ext):

    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        detected_platform, detected_arch = detect_QBDI_platform()
        python_version = Version(platform.python_version()).base_version

        cmake_args = ['-G', 'Ninja',
                      '-DPYQBDI_OUTPUT_DIRECTORY={}'.format(extdir),
                      '-DPython_EXECUTABLE={}'.format(os.path.realpath(sys.executable)),
                      '-DPython3_EXECUTABLE={}'.format(os.path.realpath(sys.executable)),
                      '-DCMAKE_BUILD_TYPE=Release',
                      '-DQBDI_PLATFORM={}'.format(detected_platform),
                      '-DQBDI_ARCH={}'.format(detected_arch),
                      '-DQBDI_BENCHMARK=OFF',
                      '-DQBDI_INSTALL=OFF',
                      '-DQBDI_INCLUDE_DOCS=OFF',
                      '-DQBDI_INCLUDE_PACKAGE=OFF',
                      '-DQBDI_SHARED_LIBRARY=OFF',
                      '-DQBDI_TEST=OFF',
                      '-DQBDI_TOOLS_FRIDAQBDI=OFF',
                      '-DQBDI_TOOLS_PYQBDI=ON',
                      '-DQBDI_TOOLS_PYQBDI_TARGET_PYTHON_VERSION={}'.format(python_version),
                     ]
        if os.getenv('CMAKE_OSX_ARCHITECTURES') == 'arm64e':
            cmake_args += ['-DQBDI_PTRAUTH=ON']
        build_args = ['--config', 'Release', '--']

        if HAS_NINJA:
            ninja_executable_path = os.path.abspath(os.path.join(ninja.BIN_DIR,
                    "ninja.exe" if detected_platform == 'windows' else "ninja"))

            if not os.path.isfile(ninja_executable_path):
                raise RuntimeError("Compile Error : Cannot found ninja binary.")

            cmake_args.append('-DCMAKE_MAKE_PROGRAM:FILEPATH={}'.format(ninja_executable_path))
        elif not bool(shutil.which('ninja')):
            raise RuntimeError("Compile Error : Cannot found ninja binary.")

        env = os.environ.copy()
        env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(env.get('CXXFLAGS', ''),
                                                              self.distribution.get_version())
        for var in ["Python3_ROOT_DIR", "Python_ROOT_DIR"]:
            if var in env:
                cmake_args.append("-D{}={}".format(var, env[var]))
            elif var.upper() in env:
                cmake_args.append("-D{}={}".format(var, env[var.upper()]))

        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)

        subprocess.check_call(['cmake', ext.sourcedir] + cmake_args, cwd=self.build_temp, env=env)
        subprocess.check_call(['cmake', '--build', '.'] + build_args, cwd=self.build_temp)

        if not os.path.isdir(extdir):
            raise RuntimeError("Compile Error : No library available.")

setup(
    ext_modules=[CMakeExtension('pyqbdi')],
    cmdclass={
        "build_ext": CMakeBuild,
    },
)
