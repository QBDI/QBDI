import subprocess

subprocess.check_call(["cmake", "..",
                       "-G", "Visual Studio 16 2019",
                       "-A", "x64",
                       "-DCMAKE_BUILD_TYPE=Release",
                       "-DCMAKE_CROSSCOMPILING=FALSE",
                       "-DPLATFORM=win-X86_64"])
