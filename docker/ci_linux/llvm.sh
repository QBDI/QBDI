#!/usr/bin/env bash

set -ex

cd $(dirname "$0")
BASEDIR=$(pwd -P)
GITDIR=$(git rev-parse --show-toplevel)

if [[ -n "$(find "${GITDIR}/deps/LLVM-${QBDI_LLVM_VERSION}-${QBDI_PLATFORM}-${QBDI_ARCH}/build" -type f -print -quit)" ]] &&
   [[ -n "$(find "${GITDIR}/deps/LLVM-${QBDI_LLVM_VERSION}-${QBDI_PLATFORM}-${QBDI_ARCH}/src/qbdi-llvm-deps" -type f -print -quit)" ]]; then
    exit 0;
fi

./img_build.sh

docker run --rm \
    -e QBDI_PLATFORM="${QBDI_PLATFORM}" \
    -e QBDI_ARCH="${QBDI_ARCH}" \
    -e QBDI_LLVM_VERSION="${QBDI_LLVM_VERSION}" \
    --mount type=bind,source="${GITDIR}",target=/home/docker/qbdi \
    --mount type=bind,source="${HOME}/.ccache",target=/home/docker/.ccache \
    qbdi_build:base_${QBDI_ARCH} \
    /bin/bash /home/docker/qbdi/docker/ci_linux/build-llvm.sh

