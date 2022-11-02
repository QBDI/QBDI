#!/usr/bin/env bash

set -ex

cd "$(dirname "$0")"
BASEDIR="$(pwd -P)"
GITDIR="$(git rev-parse --show-toplevel)"

TAG_PREFIX="qbdi-aarch64/qbdi_aarch64_test"
QBDI_PLATFORM="linux"
QBDI_ARCH="ARM"

./images/img_build.sh build_linux "${QBDI_ARCH}"

IMG_BUILD="${TAG_PREFIX}:dockcross_${QBDI_PLATFORM}_${QBDI_ARCH}"

#1 get dockcross script
SCRIPT_PATH="${BASEDIR}/.dockcross-${QBDI_PLATFORM}-${QBDI_ARCH}-latest"
docker run --rm "${IMG_BUILD}" > "${SCRIPT_PATH}"
chmod +x "${SCRIPT_PATH}"

#2 Don't mount .ssh in docker
export SSH_DIR=/var/empty/.ssh

#3 compile and test QBDI in dockcross
pushd "${GITDIR}"
"${SCRIPT_PATH}" \
    -i "${IMG_BUILD}" \
    -a "-v ${HOME}/.ccache:${HOME}/.ccache -it" \
    -- env QBDI_PLATFORM="${QBDI_PLATFORM}" \
           QBDI_ARCH="${QBDI_ARCH}" \
           QBDI_LOG_DEBUG=ON \
           ./docker/ci_aarch64/docker_internal_script/build-test-linux.sh
popd

