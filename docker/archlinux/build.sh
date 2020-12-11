#!/usr/bin/bash

set -e
set -x

BASEDIR=$(cd $(dirname "$0") && pwd -P)
GITDIR=$(git rev-parse --show-toplevel)

. "${BASEDIR}/../common.sh"

ARCH="X86_64"

if [[ "$1" = "X86" || "$1" = "x86" ]]; then
    ARCH="X86"
fi

DOCKER_TAG="qbdi:x${ARCH: -2}_archlinux"

prepare_archive

docker build "${BASEDIR}" -t "${DOCKER_TAG}" -f "${BASEDIR}/Dockerfile.${ARCH}"

delete_archive

echo "Success build ${DOCKER_TAG}"
