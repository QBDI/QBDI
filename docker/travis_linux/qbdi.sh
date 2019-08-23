#!/usr/bin/bash

set -e
set -x

BASEDIR=$(cd $(dirname "$0") && pwd -P)
GITDIR=$(git rev-parse --show-toplevel)

if [[ "X86_64" = "${QBDI_ARCH}" ]]; then
    docker build "${GITDIR}" -t qbdi_build:base -f "${BASEDIR}/base_X86_64.dockerfile"
elif [[ "X86" = "${QBDI_ARCH}" ]]; then
    docker build "${GITDIR}" -t qbdi_build:base -f "${BASEDIR}/base_X86.dockerfile"
else
    echo "Unknown QBDI_ARCH : ${QBDI_ARCH}"
    exit 1
fi

docker build "${GITDIR}" -t qbdi_build:qbdi -f "${BASEDIR}/qbdi.dockerfile"
docker run -it --rm qbdi_build:qbdi ./qbdi/build/test/QBDITest

set +e

docker run -it --name validator --cap-add=SYS_PTRACE --security-opt seccomp:unconfined -w "/home/docker/qbdi" qbdi_build:qbdi python tools/validation_runner/ValidationRunner.py tools/validation_runner/travis.cfg

mkdir -p "${GITDIR}/tools/validation_runner/travis_db/"
rm -f "${GITDIR}/tools/validation_runner/travis_db/travis.db"

docker cp "validator:/home/docker/qbdi/tools/validation_runner/travis_db/travis.db" "${GITDIR}/tools/validation_runner/travis_db/"

docker rm validator
