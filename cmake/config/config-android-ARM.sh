#!/bin/sh
set -e

DEFAULT_NDK_PATH=${HOME}/android-ndk-r20
QBDI_NDK_PATH=${NDK_PATH:-${DEFAULT_NDK_PATH}}

if [ ! -d "$QBDI_NDK_PATH" ]; then
  echo "'${QBDI_NDK_PATH}' is not valid!"
  exit 1
fi

export NDK_PATH=${QBDI_NDK_PATH}

cmake  .. \
      -DQBDI_PLATFORM=android \
      -DQBDI_ARCH=ARM \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_TOOLCHAIN_FILE="${NDK_PATH}/build/cmake/android.toolchain.cmake" \
      -DANDROID_ABI=armeabi-v7a \
      -DANDROID_PLATFORM=23
