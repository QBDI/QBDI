import subprocess

subprocess.check_call(["cmake", "..",
                       "-G", "Visual Studio 16 2019", "-A", "Win32",
                       "-DCMAKE_CXX_FLAGS=/D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING",
                       "-DCMAKE_BUILD_TYPE=Release",
                       "-DCMAKE_CROSSCOMPILING=FALSE",
                       "-DPLATFORM=win-X86"])
