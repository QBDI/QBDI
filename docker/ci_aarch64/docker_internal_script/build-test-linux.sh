#!/usr/bin/env bash

set -ex

mkdir -p "build-docker-${QBDI_PLATFORM}-${QBDI_ARCH}"
cd "build-docker-${QBDI_PLATFORM}-${QBDI_ARCH}"

cmake -G Ninja \
      -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -DQBDI_PLATFORM="${QBDI_PLATFORM}" \
      -DQBDI_ARCH="${QBDI_ARCH}" \
      -DQBDI_EXAMPLES=ON \
      -DQBDI_TOOLS_VALIDATOR=OFF \
      -DQBDI_TOOLS_PYQBDI=OFF \
      -DQBDI_LOG_DEBUG="${QBDI_LOG_DEBUG}" \
      ..

ninja
./test/QBDITest

#ninja package

#./test/QBDITest

exit 0
