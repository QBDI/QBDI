#!/usr/bin/bash

set -e
set -x

BASEDIR=$(cd $(dirname "$0") && pwd -P)
GITDIR=$(cd "${BASEDIR}/../.." && pwd -P)

if [[ -n "$(find "${GITDIR}/deps/llvm/${QBDI_PLATFORM}/lib" -type f -print -quit)" &&
      -n "$(find "${GITDIR}/deps/llvm/${QBDI_PLATFORM}/include" -type f -print -quit)" ]]; then
    exit 0;
fi

mkdir -p "${GITDIR}/deps/llvm/${QBDI_PLATFORM}/"
mkdir -p "${GITDIR}/deps/llvm/${QBDI_PLATFORM}/"

if [[ "linux-X86_64" = "${QBDI_PLATFORM}" ]]; then
    docker build "${GITDIR}" -t qbdi_build:base -f "${BASEDIR}/base_X86_64.dockerfile"
elif [[ "linux-X86" = "${QBDI_PLATFORM}" ]]; then
    docker build "${GITDIR}" -t qbdi_build:base -f "${BASEDIR}/base_X86.dockerfile"
else
    echo "Unknown QBDI_PLATFORM : ${QBDI_PLATFORM}"
    exit 1
fi

docker build "${GITDIR}" -t qbdi_build:llvm -f "${BASEDIR}/llvm.dockerfile"
docker create --name llvm qbdi_build:llvm

docker cp "llvm:/home/docker/qbdi/deps/llvm/${QBDI_PLATFORM}/include" "${GITDIR}/deps/llvm/${QBDI_PLATFORM}/"
docker cp "llvm:/home/docker/qbdi/deps/llvm/${QBDI_PLATFORM}/lib" "${GITDIR}/deps/llvm/${QBDI_PLATFORM}/"

docker rm llvm
