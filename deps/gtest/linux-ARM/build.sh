#!/bin/sh

ARM_ARCH=armv6
ARM_C_INCLUDE=/usr/arm-linux-gnueabi/include/
ARM_CXX_INCLUDE=/usr/arm-linux-gnueabi/include/c++/6/

VERSION="1.8.1"
SOURCE_URL="https://github.com/google/googletest/archive/release-${VERSION}.tar.gz"

case "$1" in

    prepare)
        rm -f "release-${VERSION}.tar.gz"
        wget $SOURCE_URL
        tar xf "release-${VERSION}.tar.gz"
    ;;
    build)
        rm -rf build
        mkdir -p build
        cd build
        export CC=arm-linux-gnueabi-gcc
        export CXX=arm-linux-gnueabi-g++
        cmake "../googletest-release-${VERSION}/googletest" \
              -DCMAKE_CROSSCOMPILING=True \
              -DCMAKE_C_FLAGS="-march=${ARM_ARCH} -I${ARM_C_INCLUDE}" \
              -DCMAKE_CXX_FLAGS="-march=${ARM_ARCH} -I${ARM_C_INCLUDE} -I${ARM_CXX_INCLUDE}"
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
