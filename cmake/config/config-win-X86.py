import subprocess
import os

# be sure to run "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x86 before

subprocess.check_call(["cmake", os.path.realpath(os.path.join(os.path.dirname(__file__), ".." , "..")),
                       "-G", "Ninja",
                       "-DCMAKE_CXX_FLAGS=/D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING",
                       "-DCMAKE_BUILD_TYPE=Release",
                       "-DCMAKE_CROSSCOMPILING=FALSE",
                       "-DQBDI_PLATFORM=windows",
                       "-DQBDI_ARCH=X86"])
