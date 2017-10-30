#!/bin/sh

VERSION="5.0.0"
SOURCE_URL="http://llvm.org/releases/${VERSION}/llvm-${VERSION}.src.tar.xz"
TARGET="X86"
LIBRAIRIES="LLVMSelectionDAG.lib LLVMAsmPrinter.lib LLVMBinaryFormat.lib LLVMCodeGen.lib LLVMScalarOpts.lib LLVMProfileData.lib LLVMInstCombine.lib LLVMTransformUtils.lib LLVMAnalysis.lib LLVMTarget.lib LLVMObject.lib LLVMMCParser.lib LLVMBitReader.lib LLVMMCDisassembler.lib LLVMMC.lib LLVMX86Utils.lib LLVMCore.lib LLVMSupport.lib"

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
              -G "Visual Studio 14 2015 Win64" \
              -DCMAKE_BUILD_TYPE=Release \
              -DLLVM_TARGETS_TO_BUILD=$TARGET \
              -DLLVM_BUILD_TOOLS=Off \
              -DLLVM_BUILD_TESTS=Off \
              -DLLVM_INCLUDE_TESTS=Off
        MSBuild.exe ALL_BUILD.vcxproj /p:Configuration=Release,Platform=X64
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
            cp build/Release/lib/$l lib/
        done
        cp build/Release/lib/LLVM${TARGET}* lib/
        cp build/lib/Target/${TARGET}/*.inc lib/Target/${TARGET}/
        for f in $(find llvm-${VERSION}.src/lib/Target/X86/ -type f \( -iname \*.def -o -iname \*.h \)); do
            nf=${f#*/}
            mkdir -p ${nf%/*}
            cp $f $nf
        done
    ;;
    clean)
        rm -rf "llvm-"$VERSION".src" build "llvm-"$VERSION".src.tar.xz"
    ;;

esac

exit
