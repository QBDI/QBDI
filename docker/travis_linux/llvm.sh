#!/usr/bin/bash

set -e
set -x

BASEDIR=$(cd $(dirname "$0") && pwd -P)
GITDIR=$(git rev-parse --show-toplevel)

if [[ -n "$(find "${GITDIR}/deps/llvm/${QBDI_PLATFORM}-${QBDI_ARCH}/lib" -type f -print -quit)" &&
      -n "$(find "${GITDIR}/deps/llvm/${QBDI_PLATFORM}-${QBDI_ARCH}/include" -type f -print -quit)" ]]; then
    exit 0;
fi

mkdir -p "${GITDIR}/deps/llvm/${QBDI_PLATFORM}-${QBDI_ARCH}/"
mkdir -p "${GITDIR}/deps/llvm/${QBDI_PLATFORM}-${QBDI_ARCH}/"

if [[ "X86_64" = "${QBDI_ARCH}" ]]; then
    docker build "${GITDIR}" -t qbdi_build:base -f "${BASEDIR}/base_X86_64.dockerfile"
elif [[ "X86" = "${QBDI_ARCH}" ]]; then
    docker build "${GITDIR}" -t qbdi_build:base -f "${BASEDIR}/base_X86.dockerfile"
else
    echo "Unknown QBDI_ARCH : ${QBDI_ARCH}"
    exit 1
fi

docker build "${GITDIR}" -t qbdi_build:llvm -f "${BASEDIR}/llvm.dockerfile"
docker create --name llvm qbdi_build:llvm

docker cp "llvm:/home/docker/qbdi/deps/llvm/${QBDI_PLATFORM}-${QBDI_ARCH}/include" "${GITDIR}/deps/llvm/${QBDI_PLATFORM}-${QBDI_ARCH}/"
docker cp "llvm:/home/docker/qbdi/deps/llvm/${QBDI_PLATFORM}-${QBDI_ARCH}/lib" "${GITDIR}/deps/llvm/${QBDI_PLATFORM}-${QBDI_ARCH}/"

docker rm llvm

