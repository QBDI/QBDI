#!/usr/bin/env bash

set -ex

cd $(dirname "$0")
BASEDIR=$(pwd -P)
GITDIR=$(git rev-parse --show-toplevel)
TAG_PREFIX="pyqbdi/wheel_arm_building"
BASE_DEBIAN="buster"
PYTHON_VERSION="$1"

./images/build_docker_img.sh "${PYTHON_VERSION}"

build_wheel() {
  QBDI_PLATFORM="linux"
  QBDI_ARCH="ARM"
  PYTHON_VERSION="$1"

  docker run --rm -it \
      -e QBDI_PLATFORM="${QBDI_PLATFORM}" \
      -e QBDI_ARCH="${QBDI_ARCH}" \
      --mount type=bind,source="${GITDIR}",target=/home/docker/QBDI \
      --mount type=bind,source="${HOME}/.ccache",target=/home/docker/.ccache \
      "${TAG_PREFIX}:${QBDI_PLATFORM}_${QBDI_ARCH}_${PYTHON_VERSION}" \
      bash /home/docker/QBDI/docker/python_linux_arm/docker_internal_script/build_wheel.sh
}


if [[ -n "$PYTHON_VERSION" ]]; then
  build_wheel "$PYTHON_VERSION"
else
  build_wheel 3.8
  build_wheel 3.9
  build_wheel 3.10
  build_wheel 3.11
fi

