#!/usr/bin/env bash

set -ex

mkdir -p "build-docker-native-${QBDI_PLATFORM}-${QBDI_ARCH}"
cd "build-docker-native-${QBDI_PLATFORM}-${QBDI_ARCH}"


cmake -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DQBDI_PLATFORM="${QBDI_PLATFORM}" \
  -DQBDI_ARCH="${QBDI_ARCH}" \
  -DQBDI_EXAMPLES=ON \
  -DQBDI_TOOLS_VALIDATOR=ON \
  -DQBDI_TOOLS_PYQBDI=ON \
  ..

ninja

./test/QBDITest

cpack

cd ../tools/validation_runner
#./ValidationRunner.py coverage.cfg -l "../../build-docker-native-${QBDI_PLATFORM}-${QBDI_ARCH}/tools/validator/libvalidator.so"

exit 0
