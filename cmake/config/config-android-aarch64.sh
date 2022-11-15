#!/bin/sh
set -e

if [ -z "${NDK_PATH}" ]; then
  if [ -z "${ANDROID_SDK_ROOT}" ]; then
    echo "ANDROID_SDK_ROOT or NDK_PATH variable should be set to configure cmake."
    exit 1
  fi
  if [ ! -d "${ANDROID_SDK_ROOT}" ]; then
    echo "'${ANDROID_SDK_ROOT}' is not valid!"
    exit 1
  fi
  export NDK_PATH="${ANDROID_SDK_ROOT}/ndk-bundle/"
fi

if [ ! -d "${NDK_PATH}" ]; then
  echo "'${NDK_PATH}' is not valid!"
  exit 1
fi

cmake  ..                                \
      -DQBDI_PLATFORM=android            \
      -DQBDI_ARCH=AARCH64                \
      -DCMAKE_BUILD_TYPE=RelWithDebInfo  \
      -DCMAKE_TOOLCHAIN_FILE="${NDK_PATH}/build/cmake/android.toolchain.cmake" \
      -DANDROID_ABI="arm64-v8a"          \
      -DANDROID_PLATFORM=24              \
      -G Ninja

