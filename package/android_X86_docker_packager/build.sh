#!/usr/bin/bash

set -e
set -x

BASEDIR=$(cd $(dirname "$0") && pwd -P)
GITDIR=$(git rev-parse --show-toplevel)

. "${BASEDIR}/../../docker/common.sh"

DOCKER_TAG="qbdi:android-X86"

prepare_archive

docker build "${BASEDIR}" -t "${DOCKER_TAG}"

delete_archive

echo "Success build ${DOCKER_TAG}"
