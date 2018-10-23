#!/bin/sh

VERSION="7.0.0"
SOURCE_URL="http://llvm.org/releases/${VERSION}/llvm-${VERSION}.src.tar.xz"
TARGET="ARM"
LIBRAIRIES="libLLVMSelectionDAG.a libLLVMAsmPrinter.a libLLVMBinaryFormat.a libLLVMCodeGen.a libLLVMScalarOpts.a libLLVMProfileData.a libLLVMInstCombine.a libLLVMTransformUtils.a libLLVMAnalysis.a libLLVMTarget.a libLLVMObject.a libLLVMMCParser.a libLLVMBitReader.a libLLVMMCDisassembler.a libLLVMMC.a libLLVMCore.a libLLVMSupport.a"

case "$1" in

    prepare)
        rm -rf "llvm-"$VERSION".src"
        rm -f "llvm-"$VERSION".src.tar.xz"
        wget $SOURCE_URL
        tar xf "llvm-"$VERSION".src.tar.xz"
        patch -p0 < nolibfuzzer.patch
    ;;
    build)
        rm -rf build
        mkdir build
        cd build
        export NDK_PATH=${HOME}/android-ndk-r13b
        export PATH=$PATH:${NDK_PATH}/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/bin/
        export CC=arm-linux-androideabi-gcc
        export CXX=arm-linux-androideabi-g++
        export API_LEVEL=23
        cmake  -G Ninja "../llvm-"$VERSION".src" \
              -DCMAKE_BUILD_TYPE=Release \
              -DCMAKE_CROSSCOMPILING=True \
              -DCMAKE_INSTALL_PREFIX=./builded \
              -DLLVM_TABLEGEN=/usr/bin/llvm-tblgen \
              -DLLVM_DEFAULT_TARGET_TRIPLE=arm-linux-gnueabihf \
              -DLLVM_TARGET_ARCH=${TARGET} \
              -DLLVM_TARGETS_TO_BUILD=${TARGET} \
              -DLLVM_BUILD_TOOLS=Off \
              -DLLVM_BUILD_TESTS=Off \
              -DLLVM_INCLUDE_UTILS=Off \
              -DLLVM_INCLUDE_TESTS=Off \
              -DCMAKE_C_FLAGS="-fvisibility=hidden -march=armv7-a -mcpu=cortex-a9 -I${NDK_PATH}/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a/include -I${NDK_PATH}/sources/cxx-stl/gnu-libstdc++/4.9/include/ -I${NDK_PATH}/platforms/android-${API_LEVEL}/arch-arm/usr/include/ -L${NDK_PATH}/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a/ -L${NDK_PATH}/platforms/android-${API_LEVEL}/arch-arm/usr/lib/ -L${NDK_PATH}/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/lib/ --sysroot=${NDK_PATH}/platforms/android-${API_LEVEL}/arch-arm -fpie" \
              -DCMAKE_CXX_FLAGS="-fvisibility=hidden -march=armv7-a -mcpu=cortex-a9 -I${NDK_PATH}/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a/include -I${NDK_PATH}/sources/cxx-stl/gnu-libstdc++/4.9/include/ -I${NDK_PATH}/platforms/android-${API_LEVEL}/arch-arm/usr/include/ -L${NDK_PATH}/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a/ -L${NDK_PATH}/platforms/android-${API_LEVEL}/arch-arm/usr/lib/ -L${NDK_PATH}/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/lib/ --sysroot=${NDK_PATH}/platforms/android-${API_LEVEL}/arch-arm -lgnustl_shared -fpie" \
              -DCMAKE_EXE_LINKER_FLAGS='-fpie -pie'
        ninja
    ;;
    package)
        rm -rf lib
        rm -rf include
        for f in $(find llvm-${VERSION}.src/include/ -iregex '.*\.\(h\|def\)'); do 
            nf=${f#*/}
            mkdir -p ${nf%/*}
            cp $f $nf
        done
        for f in $(find build/include/ -iregex '.*\.\(h\|def\|gen\|inc\)'); do 
            nf=${f#*/}
            mkdir -p ${nf%/*}
            cp $f $nf
        done
        mkdir -p lib/Target/$TARGET
        for l in $LIBRAIRIES; do
            cp build/lib/$l lib/
        done
        cp build/lib/libLLVM${TARGET}* lib/
        cp build/lib/Target/${TARGET}/*.inc lib/Target/${TARGET}/
        for f in $(find llvm-${VERSION}.src/lib/Target/${TARGET}/ -iregex '.*\.\(h\|def\)'); do 
            nf=${f#*/}
            mkdir -p ${nf%/*}
            cp $f $nf
        done
    ;;
    clean)
        rm -rf "llvm-"$VERSION".src" build "llvm-"$VERSION".src.tar.xz"
    ;;

esac

exit 0
