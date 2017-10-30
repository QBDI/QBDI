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
        mkdir -p build
        cd build
        export NDK_PATH=${HOME}/android-ndk-r13b
        export PATH=$PATH:${NDK_PATH}/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/bin/
        export CC=arm-linux-androideabi-gcc
        export CXX=arm-linux-androideabi-g++
        export API_LEVEL=23
        cmake "../googletest-release-${VERSION}" \
              -DCMAKE_CROSSCOMPILING=True \
              -DCMAKE_C_FLAGS="-march=armv7-a -mcpu=cortex-a9 -I${NDK_PATH}/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a/include -I${NDK_PATH}/sources/cxx-stl/gnu-libstdc++/4.9/include/ -I${NDK_PATH}/platforms/android-${API_LEVEL}/arch-arm/usr/include/ -L${NDK_PATH}/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a/ -L${NDK_PATH}/platforms/android-${API_LEVEL}/arch-arm/usr/lib/ -L${NDK_PATH}/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/lib/ --sysroot=${NDK_PATH}/platforms/android-${API_LEVEL}/arch-arm -fpie" \
              -DCMAKE_CXX_FLAGS="-march=armv7-a -mcpu=cortex-a9 -I${NDK_PATH}/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a/include -I${NDK_PATH}/sources/cxx-stl/gnu-libstdc++/4.9/include/ -I${NDK_PATH}/platforms/android-${API_LEVEL}/arch-arm/usr/include/ -L${NDK_PATH}/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a/ -L${NDK_PATH}/platforms/android-${API_LEVEL}/arch-arm/usr/lib/ -L${NDK_PATH}/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/lib/ --sysroot=${NDK_PATH}/platforms/android-${API_LEVEL}/arch-arm -lgnustl_shared -fpie" \
              -DCMAKE_EXE_LINKER_FLAGS='-fpie -pie'
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
