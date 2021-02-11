#!/usr/bin/env bash

set -ex

cd ~/qbdi/deps/llvm/

python3 build.py prepare ${QBDI_PLATFORM}-${QBDI_ARCH}
python3 build.py build ${QBDI_PLATFORM}-${QBDI_ARCH}
python3 build.py package ${QBDI_PLATFORM}-${QBDI_ARCH}
python3 build.py clean ${QBDI_PLATFORM}-${QBDI_ARCH}
