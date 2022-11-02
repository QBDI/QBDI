#!/usr/bin/env bash

set -ex

cd "$(dirname "$0")"
BASEDIR="$(pwd -P)"
GITDIR="$(git rev-parse --show-toplevel)"

TAG_PREFIX="qbdi-aarch64/qbdi_aarch64_test"
QBDI_PLATFORM="android"
QBDI_ARCH="ARM"
ANDROID_PLATFORM=24
ANDROID_ABI="arm7-v7"

./images/img_build.sh build_android

IMG_BUILD="${TAG_PREFIX}:debian_${QBDI_PLATFORM}_${QBDI_ARCH}"

pushd "${GITDIR}"
docker run -it --rm \
           -v ${HOME}/.ccache:/home/docker/.ccache \
           -v ${GITDIR}:/home/docker/qbdi \
           -e QBDI_PLATFORM="${QBDI_PLATFORM}" \
           -e QBDI_ARCH="${QBDI_ARCH}" \
           -e QBDI_LOG_DEBUG=ON \
           -e ANDROID_ABI="${ANDROID_ABI}" \
           -e ANDROID_PLATFORM="${ANDROID_PLATFORM}" \
           "${IMG_BUILD}" \
           ./docker/ci_aarch64/docker_internal_script/build-test-android.sh
popd
