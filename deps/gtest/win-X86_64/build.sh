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
        cmake "../googletest-release-${VERSION}" \
              -G "Visual Studio 14 2015 Win64" \
              -DCMAKE_BUILD_TYPE=Release \
              -Dgtest_force_shared_crt=On
        MSBuild.exe ALL_BUILD.vcxproj /p:Configuration=Release,Platform=X64
    ;;
    package)
        rm -rf lib
        mkdir -p lib
        cp build/Release/gtest.lib build/Release/gtest_main.lib lib/
        cp -r "googletest-release-${VERSION}/include" .
    ;;
    clean)
        rm -rf "googletest-release-${VERSION}" build "release-${VERSION}.tar.gz"
    ;;

esac

exit
