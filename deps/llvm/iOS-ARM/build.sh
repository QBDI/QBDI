#!/bin/sh

ARM_ARCH=armv7

VERSION="5.0.0"
SOURCE_URL="http://llvm.org/releases/${VERSION}/llvm-${VERSION}.src.tar.xz"
TARGET="ARM"
LIBRAIRIES="libLLVMSelectionDAG.a libLLVMAsmPrinter.a libLLVMBinaryFormat.a libLLVMCodeGen.a libLLVMScalarOpts.a libLLVMProfileData.a libLLVMInstCombine.a libLLVMTransformUtils.a libLLVMAnalysis.a libLLVMTarget.a libLLVMObject.a libLLVMMCParser.a libLLVMBitReader.a libLLVMMCDisassembler.a libLLVMMC.a libLLVMCore.a libLLVMSupport.a libLLVMDemangle.a"

case "$1" in

    prepare)
        rm -rf "llvm-"$VERSION".src"
        rm -f "llvm-"$VERSION".src.tar.xz"
        wget $SOURCE_URL
        tar xf "llvm-"$VERSION".src.tar.xz"
    ;;
    build)
        rm -rf build
        mkdir build
        cd build
        export CC=`xcrun --sdk iphoneos -f clang`
        export CXX=`xcrun --sdk iphoneos -f clang++`
        export SDK=`xcrun --sdk iphoneos --show-sdk-path`

        cmake "../llvm-"$VERSION".src" \
              -DCMAKE_BUILD_TYPE=Release \
              -DCMAKE_CROSSCOMPILING=True \
              -DCMAKE_INSTALL_PREFIX=./build \
              -DLLVM_TABLEGEN=/opt/local/bin/llvm-tblgen-mp-5.0 \
              -DLLVM_DEFAULT_TARGET_TRIPLE=arm-apple-darwin10 \
              -DLLVM_TARGET_ARCH=${TARGET} \
              -DLLVM_TARGETS_TO_BUILD=${TARGET} \
              -DLLVM_ENABLE_THREADS=Off \
              -DLLVM_BUILD_TOOLS=Off \
              -DLLVM_BUILD_TESTS=Off \
              -DLLVM_INCLUDE_TESTS=Off \
              -DCMAKE_C_FLAGS="-arch ${ARM_ARCH} -isysroot ${SDK} -fvisibility=hidden" \
              -DCMAKE_CXX_FLAGS="-arch ${ARM_ARCH} -isysroot ${SDK} -fvisibility=hidden"
        make -j4
    ;;
    package)
        rm -rf lib/
        rm -rf include/
        for f in $(find llvm-${VERSION}.src/include/ -type f \( -iname \*.def -o -iname \*.h \)); do
            nf=${f#*/}
            mkdir -p ${nf%/*}
            cp $f $nf
        done
        for f in $(find build/include/ -type f \( -iname \*.def -o -iname \*.h -o -iname \*.gen \)); do
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
        for f in $(find llvm-${VERSION}.src/lib/Target/ARM/ -type f \( -iname \*.def -o -iname \*.h \)); do
            nf=${f#*/}
            mkdir -p ${nf%/*}
            cp $f $nf
        done
        # Fix libtool warnings
        RENAME_OBJECT=rename-object.py
        if [ -f $RENAME_OBJECT ];
        then
            python $RENAME_OBJECT "`pwd`/lib/libLLVMAnalysis.a" Analysis.cpp.o
            python $RENAME_OBJECT "`pwd`/lib/libLLVMSupport.a" Memory.cpp.o
            python $RENAME_OBJECT "`pwd`/lib/libLLVMSupport.a" Error.cpp.o
            python $RENAME_OBJECT "`pwd`/lib/libLLVMSupport.a" LowLevelType.cpp.o
            python $RENAME_OBJECT "`pwd`/lib/libLLVMSupport.a" Parallel.cpp.o 1
        fi
    ;;
    clean)
        rm -rf "llvm-"$VERSION".src" build "llvm-"$VERSION".src.tar.xz"
    ;;

esac

exit
