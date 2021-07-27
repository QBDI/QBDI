#!/usr/bin/env bash

set -ex

cd $(dirname "$0")
BASEDIR=$(pwd -P)
GITDIR=$(git rev-parse --show-toplevel)

if [[ "${QBDI_ARCH}" = "X86_64" ]]; then
    DOCKER_IMG="amd64/debian:10"
elif [[ "${QBDI_ARCH}" = "X86" ]]; then
    DOCKER_IMG="i386/debian:10"
else
    echo "Unknown QBDI_ARCH : ${QBDI_ARCH}"
    exit 1
fi

docker build "${BASEDIR}" -t qbdi_build:base_${QBDI_ARCH} --build-arg USER_ID="$(id -u)" --build-arg DOCKER_IMG="${DOCKER_IMG}" -f "${BASEDIR}/Dockerfile"
