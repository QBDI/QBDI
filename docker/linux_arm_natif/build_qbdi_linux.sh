#!/usr/bin/env bash

set -ex

cd "$(dirname "$0")"
BASEDIR="$(pwd -P)"
GITDIR="$(git rev-parse --show-toplevel)"

TAG_PREFIX="qbdi-linux/qbdi_test"
QBDI_PLATFORM="linux"
QBDI_ARCH="$1"

if [[ "$QBDI_ARCH" != "AARCH64" ]] && [[ "$QBDI_ARCH" != "ARM" ]]; then
  echo "Unrecognized architecture."
  echo "Supported : AARCH64, ARM"
  exit 1
fi

./images/build_docker_img.sh "${QBDI_ARCH}"

IMG_BUILD="${TAG_PREFIX}:native_${QBDI_PLATFORM}_${QBDI_ARCH}"

docker run -it --rm \
    -v "${HOME}/.ccache:/home/docker/.ccache" \
    -v "${GITDIR}:/home/docker/QBDI" \
    -e QBDI_PLATFORM="${QBDI_PLATFORM}" \
    -e QBDI_ARCH="${QBDI_ARCH}" \
    "${IMG_BUILD}" \
    /home/docker/QBDI/docker/linux_arm_natif/docker_internal_script/build-test-linux.sh

