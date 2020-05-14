#!/bin/sh

ARM_ARCH=armv7

VERSION="1.8.1"
SOURCE_URL="https://github.com/google/googletest/archive/release-${VERSION}.tar.gz"

case "$1" in

    prepare)
        wget $SOURCE_URL
        tar xf "release-${VERSION}.tar.gz"
    ;;
    build)
        mkdir -p build
        cd build
        export CC=`xcrun --sdk iphoneos -f clang`
        export CXX=`xcrun --sdk iphoneos -f clang++`
        export SDK=`xcrun --sdk iphoneos --show-sdk-path`

        cmake "../googletest-release-${VERSION}/googletest" \
              -DCMAKE_CROSSCOMPILING=True \
              -DCMAKE_C_FLAGS="-arch ${ARM_ARCH} -isysroot ${SDK}" \
              -DCMAKE_CXX_FLAGS="-arch ${ARM_ARCH} -isysroot ${SDK}"
        make -j4
    ;;
    package)
        mkdir -p lib
        cp build/libgtest.a build/libgtest_main.a lib/
        cp -r "googletest-release-${VERSION}/googletest/include" .
    ;;
    clean)
        rm -rf "googletest-release-${VERSION}" build "release-${VERSION}.tar.gz"
    ;;

esac

exit
