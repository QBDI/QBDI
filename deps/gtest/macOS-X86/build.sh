#!/bin/sh

VERSION="1.7.0"
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
        cmake -DCMAKE_CXX_FLAGS="-arch i386" "../googletest-release-${VERSION}"
        make -j4
    ;;
    package)
        mkdir -p lib
        cp build/libgtest.a build/libgtest_main.a lib/
        cp -r "googletest-release-${VERSION}/include" .
    ;;
    clean)
        rm -rf "googletest-release-${VERSION}" build "release-${VERSION}.tar.gz"
    ;;

esac

exit
