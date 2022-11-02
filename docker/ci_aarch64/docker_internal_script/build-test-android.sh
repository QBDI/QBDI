#!/usr/bin/env bash

set -ex

mkdir -p "build-docker-${QBDI_PLATFORM}-${QBDI_ARCH}"
cd "build-docker-${QBDI_PLATFORM}-${QBDI_ARCH}"


cmake -G Ninja \
      -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -DQBDI_PLATFORM="${QBDI_PLATFORM}" \
      -DQBDI_ARCH="${QBDI_ARCH}" \
      -DQBDI_EXAMPLES=OFF \
      -DQBDI_TOOLS_VALIDATOR=OFF \
      -DQBDI_TOOLS_PYQBDI=OFF \
      -DQBDI_LOG_DEBUG="${QBDI_LOG_DEBUG}" \
      -DANDROID_ABI="${ANDROID_ABI}" \
      -DANDROID_PLATFORM="${ANDROID_PLATFORM}" \
      -DCMAKE_TOOLCHAIN_FILE="${ANDROID_NDK}/build/cmake/android.toolchain.cmake" \
      ..

ninja package

exit 0
