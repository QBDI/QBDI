#!/usr/bin/bash

set -e
set -x

BASEDIR=$(cd $(dirname "$0") && pwd -P)
GITDIR=$(cd "${BASEDIR}/../.." && pwd -P)

. "${BASEDIR}/../../docker/common.sh"

DOCKER_TAG="qbdi:android-ARM"

prepare_archive

docker build "${BASEDIR}" -t "${DOCKER_TAG}"

delete_archive

echo "Success build ${DOCKER_TAG}"
