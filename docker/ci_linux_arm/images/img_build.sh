#!/usr/bin/env bash

set -ex

cd "$(dirname "$0")"
BASEDIR="$(pwd -P)"
TAG_PREFIX="qbdi-linux/qbdi_test"
TARGET_ARCH="$1"

if [[ -n "$TARGET_ARCH" ]] && [[ "$TARGET_ARCH" != "AARCH64" ]] && [[ "$TARGET_ARCH" != "ARM" ]]; then
  echo "Unrecognized architecture."
  echo "Supported : AARCH64, ARM"
  exit 1
fi

build_linux() {
  QBDI_PLATFORM="linux"
  QBDI_ARCH="$1"
  if [[ "$QBDI_ARCH" = "AARCH64" ]]; then
    DOCKCROSS_IMG="dockcross/linux-arm64"
  elif [[ "$QBDI_ARCH" = "ARM" ]]; then
    DOCKCROSS_IMG="dockcross/linux-armv7"
  fi
  docker pull "${DOCKCROSS_IMG}"
  docker build "${BASEDIR}" -f "${BASEDIR}/Dockerfile.dockcross" -t "${TAG_PREFIX}:dockcross_${QBDI_PLATFORM}_${QBDI_ARCH}" --build-arg DOCKER_IMG="${DOCKCROSS_IMG}"
}

if [[ -n "$TARGET_ARCH" ]]; then
  build_linux "$TARGET_ARCH"
else
  build_linux AARCH64
  build_linux ARM
fi

