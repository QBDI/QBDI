#!/bin/sh

SYSROOT=~/sysroot-glibc-linaro/
ARM_ARCH=armv7

LINARO_TOOLCHAIN=~/gcc-linaro/bin
export AS=${LINARO_TOOLCHAIN}/arm-linux-gnueabihf-as
export CC=${LINARO_TOOLCHAIN}/arm-linux-gnueabihf-gcc
export CXX=${LINARO_TOOLCHAIN}/arm-linux-gnueabihf-g++
export STRIP=${LINARO_TOOLCHAIN}/arm-linux-gnueabihf-strip

cmake .. \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CROSSCOMPILING=True \
      -DCMAKE_C_FLAGS="-march=${ARM_ARCH} --sysroot=${SYSROOT}" \
      -DCMAKE_CXX_FLAGS="-march=${ARM_ARCH} --sysroot=${SYSROOT}" \
      -DPLATFORM=linux-ARM \
      -DARM_ARCH=${ARM_ARCH} \
      -DSTRIP_PATH=${STRIP} \
      -DAS_BINARY=${AS}
