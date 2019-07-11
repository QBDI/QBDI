#!/bin/sh

export NDK_PATH=${HOME}/android-ndk-r20

cmake  .. \
      -DPLATFORM=android-ARM \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_TOOLCHAIN_FILE="${NDK_PATH}/build/cmake/android.toolchain.cmake"
