#!/bin/sh
set -e

if [ -z "${ANDROID_SDK}" ]; then
  echo "ANDROID_SDK variable should be set to the path of android sdk."
  exit 1
fi
if [ ! -d "${ANDROID_SDK}" ]; then
  echo "'${ANDROID_SDK}' is not valid!"
  exit 1
fi

cmake  .. \
      -DQBDI_PLATFORM=android \
      -DQBDI_ARCH=X86_64 \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_TOOLCHAIN_FILE="${ANDROID_SDK}/ndk-bundle/build/cmake/android.toolchain.cmake" \
      -DANDROID_ABI=x86 \
      -DANDROID_PLATFORM=23


