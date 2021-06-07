import os
import re
import sys
import platform
import subprocess
import shutil

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from distutils.version import LooseVersion

def detect_QBDI_platform():
    os = None
    arch = None
    if platform.system() == 'Darwin':
        os = 'osx'
    elif platform.system() == 'Windows':
        os = 'windows'
    elif platform.system() == 'Linux':
        os = 'linux'

    if platform.machine() in ['AMD64', 'AMD', 'x64', 'x86_64', 'x86', 'i386', 'i686']:
        # intel arch
        if sys.maxsize > 2**32:
            arch = "X86_64"
        else:
            arch = "X86"

    if os and arch:
        return (os, arch)

    raise RuntimeError("Cannot determine the QBDI platform : system={}, machine={}, is64bits={}".format(
                            platform.system(), platform.machine(), sys.maxsize > 2**32))


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):

    @staticmethod
    def has_ninja():
        return bool(shutil.which('ninja'))

    def run(self):
        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError("CMake must be installed to build the following extensions: " +
                               ", ".join(e.name for e in self.extensions))

        if platform.system() == "Windows":
            cmake_version = LooseVersion(re.search(r'version\s*([\d.]+)', out.decode()).group(1))
            if cmake_version < '3.1.0':
                raise RuntimeError("CMake >= 3.1.0 is required on Windows")

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        detected_platform, detected_arch = detect_QBDI_platform()
        cmake_args = ['-DPYQBDI_OUTPUT_DIRECTORY=' + extdir,
                      '-DPYTHON_EXECUTABLE=' + sys.executable,
                      '-DCMAKE_BUILD_TYPE=Release',
                      '-DQBDI_PLATFORM=' + detected_platform,
                      '-DQBDI_ARCH=' + detected_arch,
                      '-DQBDI_TOOLS_PYQBDI=On',
                      '-DQBDI_TEST=Off',
                      '-DQBDI_BENCHMARK=Off',
                      '-DQBDI_SHARED_LIBRARY=Off',
                     ]
        build_args = ['--config', 'Release', '--']

        if platform.system() == "Windows":
            cmake_args += ["-G", "Ninja"]
        else:
            cmake_args += ['-DQBDI_TOOLS_QBDIPRELOAD=On']
            if self.has_ninja():
                cmake_args += ["-G", "Ninja"]
            else:
                build_args.append('-j4')

        if 'QBDI_LLVM_PREFIX' in os.environ:
            cmake_args.append('-DQBDI_LLVM_PREFIX={}'.format(os.environ.get('QBDI_LLVM_PREFIX')))

        env = os.environ.copy()
        env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(env.get('CXXFLAGS', ''),
                                                              self.distribution.get_version())
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        subprocess.check_call(['cmake', ext.sourcedir] + cmake_args, cwd=self.build_temp, env=env)
        subprocess.check_call(['cmake', '--build', '.'] + build_args + ['llvm'], cwd=self.build_temp)
        subprocess.check_call(['cmake', ext.sourcedir] + cmake_args, cwd=self.build_temp, env=env)
        subprocess.check_call(['cmake', '--build', '.'] + build_args, cwd=self.build_temp)

        if not os.path.isdir(extdir):
            raise RuntimeError("Compile Error : No library available.")

with open("README-pypi.rst", "r") as f:
    long_description = f.read()

setup(
    name='PyQBDI',
    version='0.8.1b0',
    author='Nicolas Surbayrole',
    license = "apache2",
    license_files = ('LICENSE.txt',),
    author_email='qbdi@quarkslab.com',
    description='Python binding for QBDI',
    long_description=long_description,
    long_description_content_type="text/x-rst",
    classifiers=[
        "Development Status :: 5 - Production/Stable",
        "License :: OSI Approved :: Apache Software License",
        "Operating System :: Microsoft :: Windows",
        "Operating System :: MacOS",
        "Operating System :: POSIX :: Linux",
        "Programming Language :: C++",
        "Programming Language :: Python :: 3.6",
        "Programming Language :: Python :: 3.7",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Topic :: Security",
        "Topic :: Software Development :: Debuggers",
    ],
    python_requires='>=3.6',
    project_urls={
        'Documentation': 'https://qbdi.readthedocs.io/',
        'Source': 'https://github.com/QBDI/QBDI',
        'Homepage': 'https://qbdi.quarkslab.com/',
    },
    ext_modules=[CMakeExtension('pyqbdi')],
    cmdclass=dict(build_ext=CMakeBuild),
    zip_safe=False,
)
