#!/usr/bin/env bash

set -ex

cd $(dirname "$0")
BASEDIR=$(pwd -P)
GITDIR=$(git rev-parse --show-toplevel)

./img_build.sh

docker run --rm \
    -e QBDI_PLATFORM="${QBDI_PLATFORM}" \
    -e QBDI_ARCH="${QBDI_ARCH}" \
    --mount type=bind,source="${GITDIR}",target=/home/docker/qbdi \
    --mount type=bind,source="${HOME}/.ccache",target=/home/docker/.ccache \
    --cap-add=SYS_PTRACE --security-opt seccomp:unconfined \
    qbdi_build:base_${QBDI_ARCH} \
    /bin/bash /home/docker/qbdi/docker/ci_linux/build-qbdi.sh


