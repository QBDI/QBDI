#!/usr/bin/env bash

set -ex

cd "$(dirname "$0")"
BASEDIR="$(pwd -P)"
GITDIR="$(git rev-parse --show-toplevel)"

TAG_PREFIX="qbdi-linux/qbdi_test"
QBDI_PLATFORM="linux"
QBDI_ARCH="$1"

./images/img_build.sh "${QBDI_ARCH}"

IMG_BUILD="${TAG_PREFIX}:dockcross_${QBDI_PLATFORM}_${QBDI_ARCH}"

#1 get dockcross script
SCRIPT_PATH="${BASEDIR}/.dockcross-${QBDI_PLATFORM}-${QBDI_ARCH}-latest"
docker run --rm "${IMG_BUILD}" > "${SCRIPT_PATH}"
chmod +x "${SCRIPT_PATH}"

#2 Don't mount .ssh in docker
export SSH_DIR=/var/empty/.ssh

#3 use docker to run the package
export OCI_EXE=docker

#4 compile and test QBDI in dockcross
pushd "${GITDIR}"
"${SCRIPT_PATH}" \
    -i "${IMG_BUILD}" \
    -a "-v ${HOME}/.ccache:${HOME}/.ccache" \
    -- env QBDI_PLATFORM="${QBDI_PLATFORM}" \
           QBDI_ARCH="${QBDI_ARCH}" \
           ./docker/ci_linux_arm/docker_internal_script/build-test-linux.sh
popd

