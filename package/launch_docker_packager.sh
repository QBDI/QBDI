#!/bin/sh
set -e
set -x

BASEDIR=$(cd $(dirname "$0") && pwd -P)
GITDIR=$(git rev-parse --show-toplevel)

. "${GITDIR}/docker/common.sh"

mkdir -p "${BASEDIR}/package-${QBDI_VERSION}/python"

build_ubuntu_debian() {
    OS="$1"
    TAG="$2"
    TARGET="$3"
    CMAKE_ARGUMENT="$4"

    if [[ "$TARGET" = "X86" ]]; then
        DOCKER_IMG="i386/${OS}:${TAG}"
    else
        DOCKER_IMG="${OS}:${TAG}"
    fi

    docker build "${BASEDIR}" -t qbdi:package \
                              -f "${GITDIR}/docker/ubuntu_debian/Dockerfile" \
                              --build-arg DOCKER_IMG="${DOCKER_IMG}" \
                              --build-arg QBDI_PLATFORM="linux-$TARGET" \
                              --build-arg CMAKE_ARGUMENT="$CMAKE_ARGUMENT" \
                              --pull \
                              --target builder

    # deb is already create, but need to create the tar.gz
    docker run -w "$DOCKER_BUILD_DIR" --name package qbdi:package cpack
    docker cp "package:${DOCKER_BUILD_DIR}/QBDI-${QBDI_VERSION}-linux-${TARGET}.deb" "${BASEDIR}/package-${QBDI_VERSION}/QBDI-${QBDI_VERSION}-${OS}${TAG}-${TARGET}.deb"
    docker cp "package:${DOCKER_BUILD_DIR}/QBDI-${QBDI_VERSION}-linux-${TARGET}.tar.gz" "${BASEDIR}/package-${QBDI_VERSION}/QBDI-${QBDI_VERSION}-${OS}${TAG}-${TARGET}.tar.gz"
    docker rm package
}

build_archlinux () {
    TARGET="$1"

    docker build "${BASEDIR}" -t qbdi:package \
                              -f "${GITDIR}/docker/archlinux/Dockerfile.${TARGET}" \
                              --build-arg QBDI_PLATFORM="linux-${TARGET}" \
                              --pull

    docker create --name package qbdi:package
    docker cp "package:${DOCKER_BUILD_DIR}/QBDI-${TARGET}-${QBDI_VERSION//-/_}-1-x86_64.pkg.tar.xz" "${BASEDIR}/package-${QBDI_VERSION}/QBDI-${QBDI_VERSION}-archlinux$(date +%F)-${TARGET}.pkg.tar.xz"
    docker rm package
}


build_android () {
    PLATEFORM="$1"
    docker build "${BASEDIR}" -t qbdi:package \
                              -f "${GITDIR}/package/android_${PLATEFORM}_docker_packager/Dockerfile" \
                              --pull

    docker create --name package qbdi:package
    docker cp "package:${DOCKER_BUILD_DIR}/QBDI-${QBDI_VERSION}-android-${PLATEFORM}.tar.gz" "${BASEDIR}/package-${QBDI_VERSION}/QBDI-${QBDI_VERSION}-android-${PLATEFORM}.tar.gz"
    docker rm package
}

build_python () {
    TARGET="$1"
    CMAKE_ARGUMENT="$2"

    if [[ "$TARGET" = "X86" ]]; then
        DOCKER_IMG="quay.io/pypa/manylinux2010_i686"
    else
        DOCKER_IMG="quay.io/pypa/manylinux2010_x86_64"
    fi

    docker build "${BASEDIR}" -t qbdi:package \
                              -f "${GITDIR}/docker/python/Dockerfile" \
                              --build-arg DOCKER_IMG="${DOCKER_IMG}" \
                              --build-arg QBDI_PLATFORM="linux-$TARGET" \
                              --build-arg CMAKE_ARGUMENT="$CMAKE_ARGUMENT" \
                              --pull

    docker create --name package qbdi:package
    docker cp "package:${DOCKER_BUILD_DIR}/../outwheel" "${BASEDIR}/package-${QBDI_VERSION}/python"
    docker rm package

}

prepare_archive

# debian10 x86
build_ubuntu_debian debian 10 X86

# debian10 x64
build_ubuntu_debian debian 10 X86_64

# ubuntu lts x86
build_ubuntu_debian ubuntu 18.04 X86

# ubuntu lts x64
build_ubuntu_debian ubuntu 18.04 X86_64

# ubuntu 19.04 x86
build_ubuntu_debian ubuntu 19.10 X86

# ubuntu 19.04 x64
build_ubuntu_debian ubuntu 19.10 X86_64

# archlinux x64
build_archlinux X86_64

# archlinux x86
build_archlinux X86

# android-ARM
build_android ARM

# android-x86
build_android X86

# android-x86_64
build_android X86_64

## Python

# x64
build_python X86_64

# x86
build_python X86


delete_archive

