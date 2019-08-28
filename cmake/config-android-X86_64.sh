#!/bin/sh

export NDK_PATH=${HOME}/android-ndk-r20

cmake  .. \
      -DPLATFORM=android-X86_64 \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_TOOLCHAIN_FILE="${NDK_PATH}/build/cmake/android.toolchain.cmake" \
      -DANDROID_ABI=x86_64 \
      -DANDROID_PLATFORM=23


