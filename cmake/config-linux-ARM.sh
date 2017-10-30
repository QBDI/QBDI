#!/bin/sh

ARM_ARCH=armv6
ARM_C_INCLUDE=/usr/arm-linux-gnueabi/include/
ARM_CXX_INCLUDE=/usr/arm-linux-gnueabi/include/c++/6/

export AS=arm-linux-gnueabi-as
export CC=arm-linux-gnueabi-gcc
export CXX=arm-linux-gnueabi-g++
export STRIP=arm-linux-gnueabi-strip

cmake .. \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CROSSCOMPILING=True \
      -DCMAKE_C_FLAGS="-march=${ARM_ARCH} -I${ARM_C_INCLUDE}" \
      -DCMAKE_CXX_FLAGS="-march=${ARM_ARCH} -I${ARM_C_INCLUDE} -I${ARM_CXX_INCLUDE}" \
      -DSTRIP_PATH="${STRIP}" \
      -DPLATFORM=linux-ARM \
      -DARM_ARCH=${ARM_ARCH} \
      -DAS_BINARY=${AS}
