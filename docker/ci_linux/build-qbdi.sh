#!/usr/bin/env bash

set -ex

cd "${HOME}/qbdi/"
mkdir -p build
cd build

cmake .. \
      -G Ninja \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CROSSCOMPILING=FALSE \
      -DQBDI_PLATFORM=${QBDI_PLATFORM} \
      -DQBDI_ARCH=${QBDI_ARCH} \
      -DQBDI_LLVM_PREFIX="${HOME}/qbdi/deps/LLVM-${QBDI_LLVM_VERSION}-${QBDI_PLATFORM}-${QBDI_ARCH}/" \
      -DCMAKE_INSTALL_PREFIX=$PREFIX \
      -DQBDI_EXAMPLES=ON \
      -DQBDI_TOOLS_VALIDATOR=ON \
      -DQBDI_TOOLS_PYQBDI=ON

ninja

cpack

./test/QBDITest

set +e

cd "${HOME}/qbdi/"
mkdir -p tools/validation_runner/travis_db

python3 tools/validation_runner/ValidationRunner.py tools/validation_runner/travis.cfg

exit 0
