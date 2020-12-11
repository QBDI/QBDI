#!/usr/bin/bash

set -ex

cd $(dirname "$0")
BASEDIR=$(pwd -P)
GITDIR=$(git rev-parse --show-toplevel)

if [[ -n "$(find "${GITDIR}/deps/llvm/${QBDI_PLATFORM}-${QBDI_ARCH}/lib" -type f -print -quit)" &&
      -n "$(find "${GITDIR}/deps/llvm/${QBDI_PLATFORM}-${QBDI_ARCH}/include" -type f -print -quit)" ]]; then
    exit 0;
fi

if [[ "X86_64" = "${QBDI_ARCH}" ]]; then
    docker build "${BASEDIR}" -t qbdi_build:base --build-arg USER_ID="$(id -u)" -f "${BASEDIR}/base_X86_64.dockerfile"
elif [[ "X86" = "${QBDI_ARCH}" ]]; then
    docker build "${BASEDIR}" -t qbdi_build:base --build-arg USER_ID="$(id -u)" -f "${BASEDIR}/base_X86.dockerfile"
else
    echo "Unknown QBDI_ARCH : ${QBDI_ARCH}"
    exit 1
fi

docker run --rm -it \
    -e QBDI_PLATFORM="${QBDI_PLATFORM}" \
    -e QBDI_ARCH="${QBDI_ARCH}" \
    --mount type=bind,source="${GITDIR}",target=/home/docker/qbdi \
    --mount type=bind,source="${HOME}/.ccache",target=/home/docker/.ccache \
    qbdi_build:base \
    /bin/bash /home/docker/qbdi/docker/travis_linux/build-llvm.sh

