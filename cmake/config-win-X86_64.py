import subprocess

subprocess.check_call(["cmake", "..",
                       "-G", "Visual Studio 14 2015 Win64",
                       "-DCMAKE_BUILD_TYPE=Release",
                       "-DCMAKE_CROSSCOMPILING=FALSE",
                       "-DPLATFORM=win-X86_64"])
