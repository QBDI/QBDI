#!/usr/bin/bash

set -e
set -x

BASEDIR=$(cd $(dirname "$0") && pwd -P)
GITDIR=$(git rev-parse --show-toplevel)

. "${BASEDIR}/../common.sh"

ARCH="X86_64"
DOCKER_IMG="quay.io/pypa/manylinux2010_x86_64"

if [[ "$1" = "X86" || "$1" = "x86" ]]; then
    ARCH="X86"
    DOCKER_IMG="quay.io/pypa/manylinux2010_i686"
fi

CMAKE_ARGUMENT="$2"

DOCKER_TAG="pyqbdi:x${ARCH: -2}"

prepare_archive

docker build "${BASEDIR}" -t "${DOCKER_TAG}" --build-arg DOCKER_IMG="${DOCKER_IMG}" \
                                             --build-arg QBDI_ARCH="$ARCH" \
                                             --build-arg CMAKE_ARGUMENT="${CMAKE_ARGUMENT}"

delete_archive

echo "Success build ${DOCKER_TAG}"
