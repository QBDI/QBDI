#!/usr/bin/env python3

import subprocess
import glob
import os
import shutil
import sys
import tarfile
import itertools
from pathlib import Path
try:
    from urllib.request import urlopen
except ImportError:
    raise Exception("Must be using Python 3")

VERSION = "7.0.0"
SOURCE_URL = "http://llvm.org/releases/" + VERSION + "/llvm-" + \
    VERSION + ".src.tar.xz"

TARGET_DIR = Path(os.path.dirname(__file__))


def get_libraries(target_arch, prefix="", ext=""):
    """
    Get the list of libraries required from LLVM for QBDI.

    It is only libraries names, provide a prefix and an extension if required.
    """
    # FIXME: Do we need Demangle for every platform?
    LIBRAIRIES = ["LLVMSelectionDAG", "LLVMAsmPrinter",
                  "LLVMBinaryFormat", "LLVMCodeGen",
                  "LLVMScalarOpts", "LLVMProfileData",
                  "LLVMInstCombine", "LLVMTransformUtils",
                  "LLVMAnalysis", "LLVMTarget", "LLVMObject",
                  "LLVMMCParser", "LLVMBitReader",
                  "LLVMMCDisassembler", "LLVMMC",
                  "LLVMCore", "LLVMSupport", "LLVMDemangle"]
    # Add target specific libraries
    for target_lib in ("Utils", "Info", "Disassembler", "Desc", "CodeGen",
                       "AsmPrinter", "AsmParser"):
        LIBRAIRIES.append("LLVM" + target_arch + target_lib)
    return [prefix + l + ext for l in LIBRAIRIES]


def get_llvm(url, dest):
    """Download and extract llvm."""
    # Force removal even if download should remove it too.
    base_name = dest / os.path.basename(url)
    if base_name.exists():
        os.remove(base_name)

    # FIXME: We should use a specific target path
    print("Downloading llvm ...")
    f = urlopen(url)
    with base_name.open("wb") as tmp_f:
        tmp_f.write(f.read())

    print("Extract llvm ...")
    with tarfile.open(str(base_name), 'r|xz') as tar:
        tar.extractall(str(dest))


def build_llvm(llvm_dir, build_dir, arch, platform, arch_opt=None):

    if not build_dir.exists():
        build_dir.mkdir()

    # set platform specific arguments.
    if platform == "win" and arch_opt == "i386":
        cmake_specific_option = ["-G", "Visual Studio 14 2015",
                                 "-DLLVM_BUILD_32_BITS=On"]
    elif platform == "win":
        cmake_specific_option = ["-G", "Visual Studio 14 2015 Win64",
                                 "-Thost=x64"]
    elif platform == "iOS":
        cc = subprocess.check_output(["xcrun", "--sdk", "iphoneos", "-f", "clang"])
        cxx = subprocess.check_output(["xcrun", "--sdk", "iphoneos", "-f", "clang++"])
        sdk = subprocess.check_output(["xcrun", "--sdk", "iphoneos", "--show-sdk-path"])
        cmake_specific_option = ["-DCMAKE_CROSSCOMPILING=True",
                                 "-DLLVM_TABLEGEN=/opt/local/bin/llvm-tblgen-mp-5.0"
                                 "-DLLVM_DEFAULT_TARGET_TRIPLE=arm-apple-darwin10",
                                 "-DLLVM_TARGET_ARCH=" + arch,
                                 "-DLLVM_ENABLE_THREADS=Off",
                                 "-DCMAKE_C_FLAGS=-arch " + arch_opt + " -isysroot " + sdk + " -fvisibility=hidden",
                                 "-DCMAKE_CXX_FLAGS=-arch " + arch_opt + " -isysroot " + sdk + " -fvisibility=hidden",
                                 "-DCMAKE_C_COMPILER=" + cc,
                                 "-DCMAKE_CXX_COMPILER=" + cxx]
    elif platform == "linux" and arch == "ARM":
        ARM_C_INCLUDE="/usr/arm-linux-gnueabi/include/"
        ARM_CXX_INCLUDE="/usr/arm-linux-gnueabi/include/c++/6/"
        cmake_specific_option = [
            "-DCMAKE_CROSSCOMPILING=True",
            "-DLLVM_TABLEGEN=/usr/bin/llvm-tblgen",
            "-DLLVM_DEFAULT_TARGET_TRIPLE=arm-linux-gnueabi",
            "-DLLVM_TARGET_ARCH=" + arch,
            "-DCMAKE_C_FLAGS=-fvisibility=hidden -march=" + arch_opt + " -I" + ARM_C_INCLUDE,
            "-DCMAKE_CXX_FLAGS=-fvisibility=hidden -march=" + arch_opt + " -I" + ARM_C_INCLUDE + " -I" + ARM_CXX_INCLUDE,
        ]
    elif platform == "android":
        defaul_ndk_path = str(Path.home() / "android-ndk-r20")
        ndk_path = os.getenv("NDK_PATH", defaul_ndk_path)

        if not Path(ndk_path).is_dir():
            print("Android NDK path: '{}' is not valid!".format(ndk_path), file=sys.stderr)
            sys.exit(1)

        api_level = "android-23"
        cmake_specific_option = [
                '-DCMAKE_TOOLCHAIN_FILE=' + ndk_path + '/build/cmake/android.toolchain.cmake',
                '-DCMAKE_C_FLAGS=-g0 -fvisibility=hidden -ffunction-sections -fdata-sections -fvisibility-inlines-hidden',
                '-DCMAKE_CXX_FLAGS=-g0 -fvisibility=hidden -ffunction-sections -fdata-sections -fvisibility-inlines-hidden',
                '-DCMAKE_EXE_LINKER_FLAGS=-fpie -pie -Wl,--gc-sections -Wl,--exclude-libs,ALL',
                '-DANDROID_PLATFORM=' + api_level,
                ]
        if arch == "ARM":
            cmake_specific_option += [
                    '-DANDROID_ABI=armeabi-v7a',
                    '-DLLVM_DEFAULT_TARGET_TRIPLE=armv7-linux-androideabi',
                    ]
        elif arch_opt == "i386":
            cmake_specific_option += [
                    '-DANDROID_ABI=x86',
                    '-DLLVM_DEFAULT_TARGET_TRIPLE=i386-pc-linux',
                    '-DLLVM_BUILD_32_BITS=On',
                    ]
        else:
            cmake_specific_option += [
                    '-DANDROID_ABI=x86_64',
                    ]

    elif platform == "macOS" and arch_opt == "i386":
        cmake_specific_option = ['-DLLVM_BUILD_32_BITS=On',
                                 '-DLLVM_DEFAULT_TARGET_TRIPLE=i386-apple-darwin17.7.0',
                                 '-DCMAKE_C_FLAGS=-fvisibility=hidden',
                                 '-DCMAKE_CXX_FLAGS=-fvisibility=hidden']
    elif platform == "linux" and arch_opt == "i386":
        cmake_specific_option = ['-DLLVM_BUILD_32_BITS=On',
                                 '-DLLVM_DEFAULT_TARGET_TRIPLE=i386-pc-linux',
                                 '-DCMAKE_C_FLAGS=-fvisibility=hidden',
                                 '-DCMAKE_CXX_FLAGS=-fvisibility=hidden']
    else:
        cmake_specific_option = ['-DCMAKE_C_FLAGS=-fvisibility=hidden',
                                 '-DCMAKE_CXX_FLAGS=-fvisibility=hidden']

    # Run cmake
    subprocess.check_call(["cmake", str(llvm_dir.resolve()),
                           "-DCMAKE_BUILD_TYPE=Release",
                           "-DLLVM_TARGETS_TO_BUILD=" + arch,
                           "-DLLVM_BUILD_TOOLS=Off",
                           "-DLLVM_BUILD_UTILS=Off",
                           "-DLLVM_BUILD_TESTS=Off",
                           "-DLLVM_BUILD_EXAMPLES=Off"
                           "-DLLVM_INCLUDE_TOOLS=Off"
                           "-DLLVM_INCLUDE_UTILS=Off"
                           "-DLLVM_INCLUDE_TESTS=Off"
                           "-DLLVM_INCLUDE_EXAMPLES=Off",
                           "-DLLVM_ENABLE_TERMINFO=Off",
                           "-DLLVM_ENABLE_BINDINGS=Off"] +
                          cmake_specific_option,
                          cwd=str(build_dir))

    # Compile llvm libraries
    if platform == "win":
        subprocess.check_call(["MSBuild.exe", "ALL_BUILD.vcxproj",
                               "/p:Configuration=Release",
                               "/p:Platform=X86" if arch_opt == "i386" else "/p:Platform=X64",
                               "/m:4"],
                              cwd=str(build_dir))
    else:
        # FIXME: Not always 4, could be 8 on my computer and 2 on travis)
        subprocess.check_call(["make", "-j4"] + get_libraries(arch), cwd=str(build_dir))


def extract_file(source, dest, exts):
    source_len = len(source.parts)
    for header in itertools.chain.from_iterable((source.rglob("*" + ext)
                                                 for ext in exts)):
        new_header = dest / Path(*header.parts[source_len:])
        if not new_header.parent.exists():
            new_header.parent.mkdir(parents=True)
        shutil.copy(str(header), str(new_header))


def install_header_and_lib(llvm_source, build_dir, dest, arch):
    import platform
    shutil.rmtree(str(dest / "lib"), ignore_errors=True)
    shutil.rmtree(str(dest / "include"), ignore_errors=True)

    # Extract include files.
    dest = Path(dest)
    prefix = Path(llvm_source, "include")
    extract_file(prefix, dest / "include", (".h", ".def"))
    prefix = Path(build_dir, "include")
    extract_file(prefix, dest / "include", (".h", ".def", ".gen", ".inc"))
    prefix = Path(llvm_source, "lib", "Target", arch)
    extract_file(prefix, dest / "lib" / "Target" / arch, (".h", ".def"))
    prefix = Path(build_dir, "lib", "Target", arch)
    extract_file(prefix, dest / "lib" / "Target" / arch, (".inc",))

    lib_path = os.path.join(str(build_dir), "Release" if platform.system() == "Windows" else "", "lib")
    PREFIX = "lib" if platform.system() != "Windows" else ""
    SUFFIX = ".lib" if platform.system() == "Windows" else ".a"
    for l in get_libraries(arch, prefix=PREFIX, ext=SUFFIX):
        shutil.copy(os.path.join(lib_path, l), str(dest / "lib"))

    if platform.system() == "Darwin":
        # Fix libtool warning
        import rename_object
        rename_object.rename_object(str(dest / "lib/libLLVMAnalysis.a"), "Analysis.cpp.o")
        rename_object.rename_object(str(dest / "lib/libLLVMSupport.a"), "Memory.cpp.o")
        rename_object.rename_object(str(dest / "lib/libLLVMSupport.a"), "Error.cpp.o")
        rename_object.rename_object(str(dest / "lib/libLLVMSupport.a"), "LowLevelType.cpp.o")
        # FIXME: Should we keep it only for iOS?
        rename_object.rename_object(str(dest / "lib/libLLVMSupport.a"), "Parallel.cpp.o", True)


def clean_llvm_build(llvm_source, build_dir):
    shutil.rmtree(str(llvm_source), ignore_errors=True)
    shutil.rmtree(str(build_dir), ignore_errors=True)
    if os.path.exists(str(llvm_source) + ".tar.xz"):
        os.remove(str(llvm_source) + ".tar.xz")


if __name__ == "__main__":
    if len(sys.argv) < 3:
        fmt = 'Usage: {} prepare|build|package|clean platform'
        print(fmt.format(sys.argv[0]))
        sys.exit(1)

    target = sys.argv[2]
    assert(target in ("android-ARM", "android-X86", "android-X86_64", "iOS-ARM", "linux-ARM", "linux-X86_64", "linux-X86", "macOS-X86_64", "macOS-X86", "win-X86_64", "win-X86"))

    platform = target.split("-")[0]
    arch = target.split("-")[1]

    arch_opt = None
    if arch == "ARM":
        if platform == "linux":
            arch_opt = "armv6"
        elif platform == "iOS":
            arch_opt = "armv7"
        elif platform == "android":
            arch_opt = "armv7-a"
        else:
            raise RuntimeError("Unknown platform")
    elif arch == "X86":
        arch_opt = "i386"
    elif arch == "X86_64":
        arch = "X86"

    if not (TARGET_DIR / target).exists():
        (TARGET_DIR / target).mkdir()

    build_dir = TARGET_DIR / target / "build"
    extracted_llvm_dir = TARGET_DIR / target / ("llvm-" + VERSION + ".src")

    if sys.argv[1] == "prepare":
        get_llvm(SOURCE_URL, TARGET_DIR / target)

    elif sys.argv[1] == "build":
        build_llvm(extracted_llvm_dir,
                   build_dir, arch, platform, arch_opt)

    elif sys.argv[1] == "package":
        install_header_and_lib(extracted_llvm_dir,
                               build_dir,
                               TARGET_DIR / target,
                               arch)

    elif sys.argv[1] == "clean":
        clean_llvm_build(extracted_llvm_dir,
                         build_dir)
    else:
        raise RuntimeError("FAIL")
