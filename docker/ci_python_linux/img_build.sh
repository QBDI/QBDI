#!/usr/bin/env bash

set -ex

cd $(dirname "$0")
BASEDIR=$(pwd -P)
GITDIR=$(git rev-parse --show-toplevel)

if [[ "${QBDI_ARCH}" = "X86_64" ]]; then
    DOCKER_IMG="quay.io/pypa/manylinux2014_x86_64"
elif [[ "${QBDI_ARCH}" = "X86" ]]; then
    DOCKER_IMG="quay.io/pypa/manylinux2014_i686"
elif [[ "${QBDI_ARCH}" = "AARCH64" ]]; then
    DOCKER_IMG="quay.io/pypa/manylinux2014_aarch64"
else
    echo "Unknown QBDI_ARCH : ${QBDI_ARCH}"
    exit 1
fi


docker build "${BASEDIR}" -t pyqbdi_build:base_${QBDI_ARCH} \
        --build-arg DOCKER_IMG="${DOCKER_IMG}" \
        --build-arg USER_ID="$(id -u)" \
        -f "${BASEDIR}/Dockerfile"

