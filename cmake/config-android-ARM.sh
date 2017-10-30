#!/bin/sh

export NDK_PATH=${HOME}/android-ndk-r13b
export NDK_BIN_PATH=${NDK_PATH}/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/bin/
export NDK_STL=${NDK_PATH}/sources/cxx-stl/gnu-libstdc++/4.9
export PATH=$PATH:${NDK_BIN_PATH}
export API_LEVEL=23

export AS=arm-linux-androideabi-as
export CC=arm-linux-androideabi-gcc
export CXX=arm-linux-androideabi-g++
export STRIP=arm-linux-androideabi-strip

cmake  .. \
      -DPLATFORM=android-ARM \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CROSSCOMPILING=True \
      -DCMAKE_INSTALL_PREFIX=./builded \
      -DCMAKE_C_FLAGS="-march=armv7-a -mcpu=cortex-a9 -I${NDK_STL}/libs/armeabi-v7a/include -I${NDK_STL}/include/ -I${NDK_PATH}/platforms/android-${API_LEVEL}/arch-arm/usr/include/ -L${NDK_STL}/libs/armeabi-v7a/ -L${NDK_PATH}/platforms/android-${API_LEVEL}/arch-arm/usr/lib/ -L${NDK_PATH}/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/lib/ --sysroot=${NDK_PATH}/platforms/android-${API_LEVEL}/arch-arm -fpie" \
      -DCMAKE_CXX_FLAGS="-march=armv7-a -mcpu=cortex-a9 -I${NDK_PATH}/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a/include -I${NDK_PATH}/sources/cxx-stl/gnu-libstdc++/4.9/include/ -I${NDK_PATH}/platforms/android-${API_LEVEL}/arch-arm/usr/include/ -L${NDK_PATH}/sources/cxx-stl/gnu-libstdc++/4.9/libs/armeabi-v7a/ -L${NDK_PATH}/platforms/android-${API_LEVEL}/arch-arm/usr/lib/ -L${NDK_PATH}/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/lib/ --sysroot=${NDK_PATH}/platforms/android-${API_LEVEL}/arch-arm -lgnustl_static -fpie" \
      -DSTRIP_PATH="${NDK_BIN_PATH}${STRIP}" \
      -DCMAKE_EXE_LINKER_FLAGS='-fpie -pie' \
      -DAS_BINARY=${AS}
