import subprocess
import os
import shutil
import sys
import tarfile

try:
    from urllib.request import urlopen
except ImportError:
    raise Exception("Must be using Python 3")

VERSION = "1.7.0"
SOURCE_URL = "https://github.com/google/googletest/archive/release-" + \
    VERSION + ".tar.gz"

if len(sys.argv) < 2:
    fmt = 'Usage: {} prepare|build|package|clean'
    print(fmt.format(sys.argv[0]))
    sys.exit(1)

if sys.argv[1] == "prepare":
    if os.path.exists("release-" + VERSION + ".tar.gz"):
        os.remove("release-" + VERSION + ".tar.gz")

    # FIXME: We should use a specific path
    print("Downloading gtest ...")
    f = urlopen(SOURCE_URL)
    with open(os.path.basename(SOURCE_URL), "wb") as tmp_f:
        tmp_f.write(f.read())

    print("Extract gtest ...")
    with tarfile.open(os.path.basename(SOURCE_URL), 'r|gz') as tar:
        tar.extractall()

elif sys.argv[1] == "build":
    if os.path.exists("build"):
        shutil.rmtree("build")
    os.mkdir("build")
    subprocess.check_call(["cmake", "../googletest-release-" + VERSION,
                           "-G", "Ninja",
                           "-DCMAKE_BUILD_TYPE=Release",
                           "-DCMAKE_CXX_FLAGS=/D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING",
                           "-Dgtest_force_shared_crt=On"], cwd="build")
    subprocess.check_call(["ninja"], cwd="build")
elif sys.argv[1] == "package":
    if os.path.exists("lib"):
        shutil.rmtree("lib")
    if os.path.exists("include"):
        shutil.rmtree("include")
    os.makedirs("lib")
    for file_ in ("gtest.lib", "gtest_main.lib"):
        shutil.copy(os.path.join("build", file_), "lib")
    shutil.copytree(os.path.join("googletest-release-" + VERSION, "include"),
                    "include")
elif sys.argv[1] == "clean":
    if os.path.exists("googletest-release-" + VERSION):
        shutil.rmtree("googletest-release-" + VERSION)
    if os.path.exists("build"):
        shutil.rmtree("build")
    if os.path.exists("release-" + VERSION + ".tar.gz"):
        os.remove("release-" + VERSION + ".tar.gz")
else:
    raise RuntimeError("Fail")
