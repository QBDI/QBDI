#!/bin/sh

VERSION="1.7.0"
SOURCE_URL="https://github.com/google/googletest/archive/release-${VERSION}.tar.gz"

DEFAULT_NDK_PATH=${HOME}/android-ndk-r20
QBDI_NDK_PATH=${NDK_PATH:-${DEFAULT_NDK_PATH}}

if [ ! -d "$QBDI_NDK_PATH" ]; then
  echo "'${QBDI_NDK_PATH}' is not valid!"
  exit 1
fi

export NDK_PATH=${QBDI_NDK_PATH}

case "$1" in

    prepare)
        rm -f "release-${VERSION}.tar.gz"
        wget $SOURCE_URL
        tar xf "release-${VERSION}.tar.gz"
    ;;
    build)
        mkdir -p build
        cd build
        cmake "../googletest-release-${VERSION}" \
              -DCMAKE_TOOLCHAIN_FILE="${NDK_PATH}/build/cmake/android.toolchain.cmake" \
              -DANDROID_ABI=x86_64 \
              -DANDROID_PLATFORM=23
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

exit 0
