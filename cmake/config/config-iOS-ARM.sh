#!/bin/sh

ARM_ARCH=armv7

export AS=`xcrun --sdk iphoneos -f as`
export CC=`xcrun --sdk iphoneos -f clang`
export CXX=`xcrun --sdk iphoneos -f clang++`
export SDK=`xcrun --sdk iphoneos --show-sdk-path`

cmake .. \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CROSSCOMPILING=True \
      -DCMAKE_C_FLAGS="-arch ${ARM_ARCH} -isysroot ${SDK}" \
      -DCMAKE_CXX_FLAGS="-arch ${ARM_ARCH} -isysroot ${SDK}" \
      -DQBDI_PLATFORM=ios \
      -DQBDI_ARCH=ARM \
      -DARM_ARCH=${ARM_ARCH} \
      -DAS_BINARY=${AS}
