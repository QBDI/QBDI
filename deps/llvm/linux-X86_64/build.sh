#!/bin/sh

set -xe

VERSION="7.0.0"
SOURCE_URL="http://llvm.org/releases/${VERSION}/llvm-${VERSION}.src.tar.xz"
TARGET="X86"
LIBRAIRIES="libLLVMSelectionDAG.a libLLVMAsmPrinter.a libLLVMBinaryFormat.a libLLVMCodeGen.a libLLVMScalarOpts.a libLLVMProfileData.a libLLVMInstCombine.a libLLVMTransformUtils.a libLLVMAnalysis.a libLLVMTarget.a libLLVMObject.a libLLVMMCParser.a libLLVMBitReader.a libLLVMMCDisassembler.a libLLVMMC.a libLLVMX86Utils.a libLLVMCore.a libLLVMSupport.a libLLVMDemangle.a libLLVM${TARGET}Utils.a libLLVM${TARGET}Info.a libLLVM${TARGET}Disassembler.a libLLVM${TARGET}Desc.a libLLVM${TARGET}CodeGen.a libLLVM${TARGET}AsmPrinter.a libLLVM${TARGET}AsmParser.a"

case "$1" in

    prepare)
        rm -f "llvm-"$VERSION".src.tar.xz"
        wget $SOURCE_URL
        tar xf "llvm-"$VERSION".src.tar.xz"
    ;;
    build)
        rm -rf build
        mkdir build
        cd build
        cmake "../llvm-"$VERSION".src" \
              -DCMAKE_BUILD_TYPE=Release \
              -DLLVM_TARGETS_TO_BUILD=$TARGET \
              -DLLVM_BUILD_TOOLS=Off \
              -DLLVM_BUILD_TESTS=Off \
              -DLLVM_INCLUDE_TESTS=Off \
              -DCMAKE_C_FLAGS="-fvisibility=hidden" \
              -DCMAKE_CXX_FLAGS="-fvisibility=hidden"

        for t in $LIBRAIRIES; do
            F=$(echo $t | cut -c 4-)
            make -j4 ${F%.*}
        done
    ;;
    package)
        rm -rf lib/
        rm -rf include/
        for f in $(find llvm-${VERSION}.src/include/ -type f \( -iname \*.def -o -iname \*.h \)); do
            nf=${f#*/}
            mkdir -p ${nf%/*}
            cp $f $nf
        done
        for f in $(find build/include/ -type f \( -iname \*.def -o -iname \*.h -o -iname \*.gen -o -iname \*.inc \)); do
            nf=${f#*/}
            mkdir -p ${nf%/*}
            cp $f $nf
        done
        mkdir -p lib/Target/$TARGET
        for l in $LIBRAIRIES; do
            cp build/lib/$l lib/
        done
        cp build/lib/Target/${TARGET}/*.inc lib/Target/${TARGET}/
        for f in $(find llvm-${VERSION}.src/lib/Target/X86/ -type f \( -iname \*.def -o -iname \*.h \)); do
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
        fi
    ;;
    clean)
        rm -rf "llvm-"$VERSION".src" build "llvm-"$VERSION".src.tar.xz"
    ;;

esac

exit
