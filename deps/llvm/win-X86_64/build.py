from urllib.request import urlopen
import subprocess
import glob
import os
import shutil
import sys
import tarfile

#FIXME: Add check for python version, we need python3 for xz extraction

VERSION="5.0.0"
SOURCE_URL="http://llvm.org/releases/" + VERSION + "/llvm-" + VERSION + ".src.tar.xz"
TARGET="X86"
LIBRAIRIES=["LLVMSelectionDAG.lib", "LLVMAsmPrinter.lib",
            "LLVMBinaryFormat.lib", "LLVMCodeGen.lib", "LLVMScalarOpts.lib",
            "LLVMProfileData.lib", "LLVMInstCombine.lib",
            "LLVMTransformUtils.lib", "LLVMAnalysis.lib", "LLVMTarget.lib",
            "LLVMObject.lib", "LLVMMCParser.lib", "LLVMBitReader.lib",
            "LLVMMCDisassembler.lib", "LLVMMC.lib", "LLVMX86Utils.lib",
            "LLVMCore.lib", "LLVMSupport.lib"]

if sys.argv[1] == "prepare":
    if os.path.exists("llvm-" + VERSION + ".src.tar.xz"):
        os.remove("llvm-" + VERSION + ".src.tar.xz")

    # FIXME: We should use a specific path
    print("Downloading llvm ...")
    f = urlopen(SOURCE_URL)
    with open(os.path.basename(SOURCE_URL), "wb") as tmp_f:
        tmp_f.write(f.read())

    print("Extract llvm ...")
    with tarfile.open(os.path.basename(SOURCE_URL), 'r|xz') as tar:
        tar.extractall()

elif sys.argv[1] == "build":
    if os.path.exists("build"):
        shutil.rmtree("build")
    os.mkdir("build")
    subprocess.check_call(["cmake", "../llvm-" + VERSION + ".src",
                           "-G", "Visual Studio 14 2015 Win64",
                           "-DCMAKE_BUILD_TYPE=Release",
                           "-DLLVM_TARGETS_TO_BUILD=" + TARGET,
                           "-DLLVM_BUILD_TOOLS=Off",
                           "-DLLVM_BUILD_TESTS=Off",
                           "-DLLVM_INCLUDE_TESTS=Off"], cwd="build")
    subprocess.check_call(["MSBuild.exe", "ALL_BUILD.vcxproj",
                           "/p:Configuration=Release,Platform=X64"],
                          cwd="build")
elif sys.argv[1] == "package":
    if os.path.exists("lib"):
        shutil.rmtree("lib")
    if os.path.exists("include"):
        shutil.rmtree("include")
    prefix = os.path.join("llvm-" + VERSION + ".src", "include")
    for root, dirs, files in os.walk(prefix):
        print(root, dirs, files)
        for file_ in files:
            if os.path.splitext(file_)[1] in (".h", ".def"):
                new_root = os.path.abspath(root[len("llvm-" + VERSION + ".src")+1:])
                if not os.path.exists(new_root):
                    os.makedirs(new_root)
                shutil.copy(os.path.join(root, file_), new_root)

    prefix = os.path.join("build", "include")
    for root, dirs, files in os.walk(prefix):
        for file_ in files:
            if os.path.splitext(file_)[1] in (".h", ".def", ".gen"):
                new_root = os.path.abspath(root[len("build")+1:])
                if not os.path.exists(new_root):
                    os.makedirs(new_root)
                shutil.copy(os.path.join(root, file_), new_root)

    os.makedirs(os.path.join("lib", "Target", TARGET))
    for l in LIBRAIRIES:
        shutil.copy(os.path.join("build", "Release", "lib", l), "lib")

    for path in glob.glob(os.path.join("build", "Release", "lib", "LLVM" + TARGET + "*")):
        shutil.copy(path, "lib")

    prefix = os.path.join("llvm-" + VERSION + ".src", "lib", "Target", TARGET)
    for root, dirs, files in os.walk(prefix):
        for file_ in files:
            if os.path.splitext(file_)[1] in (".h", ".def"):
                new_root = os.path.abspath(root[len("llvm-" + VERSION + ".src")+1:])
                if not os.path.exists(new_root):
                    os.makedirs(new_root)
                shutil.copy(os.path.join(root, file_), new_root)

    prefix = os.path.join("build", "lib", "Target", TARGET)
    for root, dirs, files in os.walk(prefix):
        for file_ in files:
            if os.path.splitext(file_)[1] in (".inc"):
                new_root = os.path.abspath(root[len("build")+1:])
                if not os.path.exists(new_root):
                    os.makedirs(new_root)
                shutil.copy(os.path.join(root, file_), new_root)

    print("listdir: ", os.listdir("."))
elif sys.argv[1] == "clean":
    if os.path.exists("llvm-" + VERSION + ".src"):
        shutil.rmtree("llvm-" + VERSION + ".src")
    if os.path.exists("build"):
        shutil.rmtree("build")
    if os.path.exists("llvm-" + VERSION + ".src.tar.xz"):
        os.remove("llvm-" + VERSION + ".src.tar.xz")
else:
    raise RuntimeError("FAIL")
