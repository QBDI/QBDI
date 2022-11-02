#!/usr/bin/env bash

set -ex

cd "$(dirname "$0")"
BASEDIR="$(pwd -P)"
TAG_PREFIX="qbdi-aarch64/qbdi_aarch64_test"
CMD="$1"
TARGET_ARCH="$2"

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

build_android() {
  DOCKCROSS_IMG="debian:bullseye-backports"
  QBDI_PLATFORM="android"
  QBDI_ARCH="$1"
  docker pull "${DOCKCROSS_IMG}"
  docker build "${BASEDIR}" -f "${BASEDIR}/Dockerfile.android" -t "${TAG_PREFIX}:debian_${QBDI_PLATFORM}_${QBDI_ARCH}" --build-arg DOCKER_IMG="${DOCKCROSS_IMG}"
}

OK="no"

if [[ "${CMD}" = "build_linux" ]] || [[ "${CMD}" = "build_push" ]] || [[ "${CMD}" = "build" ]]; then
  if [[ -n "$TARGET_ARCH" ]]; then
    build_linux "$TARGET_ARCH"
  else
    build_linux AARCH64
    build_linux ARM
  fi
  OK="yes"
fi
if [[ "${CMD}" = "build_android" ]] || [[ "${CMD}" = "build_push" ]] || [[ "${CMD}" = "build" ]]; then
  if [[ -n "$TARGET_ARCH" ]]; then
    build_android "$TARGET_ARCH"
  else
    build_android AARCH64
    build_android ARM
  fi
  OK="yes"
fi

if [[ "$OK" = "no" ]]; then
  echo "Unrecognized argument."
  echo "Supported argument: build_linux, build_android, build"
  exit 1
fi
