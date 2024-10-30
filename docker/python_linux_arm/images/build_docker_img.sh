#!/usr/bin/env bash

set -ex

cd "$(dirname "$0")"
BASEDIR="$(pwd -P)"
TAG_PREFIX="pyqbdi/wheel_arm_building"
BASE_DEBIAN="bookworm"
PYTHON_VERSION="$1"

build_python_image() {
  QBDI_PLATFORM="linux"
  QBDI_ARCH="ARM"
  PYTHON_VERSION="$1"
  BASE_IMG="arm32v7/python:${PYTHON_VERSION}-${BASE_DEBIAN}"
  docker build "${BASEDIR}" -f "${BASEDIR}/Dockerfile" -t "${TAG_PREFIX}:${QBDI_PLATFORM}_${QBDI_ARCH}_${PYTHON_VERSION}" --build-arg DOCKER_IMG="${BASE_IMG}"
}

if [[ -n "$PYTHON_VERSION" ]]; then
  build_python_image "$PYTHON_VERSION"
else
  build_python_image 3.8
  build_python_image 3.9
  build_python_image 3.10
  build_python_image 3.11
  build_python_image 3.12
  build_python_image 3.13
fi


