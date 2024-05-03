#!/usr/bin/env bash
set -e
set -x

BASEDIR=$(cd $(dirname "$0") && pwd -P)
GITDIR=$(git rev-parse --show-toplevel)

TARGET_ARCH="$1"

. "${GITDIR}/docker/common.sh"

mkdir -p "${BASEDIR}/package-${QBDI_VERSION}/python"

build_ubuntu_debian() {
    OS="$1"
    TAG="$2"
    TARGET="$3"
    CMAKE_ARGUMENT="$4"
    IMG_TAG="qbdi:package_${OS}_${TAG}_${TARGET}"

    if [[ -n "$TARGET_ARCH" ]] && [[ "$TARGET" != "$TARGET_ARCH" ]]; then
        return
    fi

    if [[ "$TARGET" = "X86" ]]; then
        DOCKER_IMG="i386/${OS}:${TAG}"
    elif [[ "$TARGET" = "ARM" ]]; then
        DOCKER_IMG="arm32v7/${OS}:${TAG}"
    elif [[ "$TARGET" = "AARCH64" ]]; then
        DOCKER_IMG="arm64v8/${OS}:${TAG}"
    else
        DOCKER_IMG="${OS}:${TAG}"
    fi

    DOCKERFILE="${GITDIR}/docker/ubuntu_debian/Dockerfile"

    docker build "${BASEDIR}" -t ${IMG_TAG} -f "${DOCKERFILE}" \
                              --build-arg DOCKER_IMG="${DOCKER_IMG}" \
                              --build-arg QBDI_ARCH="$TARGET" \
                              --build-arg CMAKE_ARGUMENT="$CMAKE_ARGUMENT" \
                              --pull \
                              --target builder

    # deb is already create, but need to create the tar.gz
    docker run -w "$DOCKER_BUILD_DIR" --name package ${IMG_TAG} cpack
    docker cp "package:${DOCKER_BUILD_DIR}/QBDI-${QBDI_VERSION}-linux-${TARGET}.deb" "${BASEDIR}/package-${QBDI_VERSION}/QBDI-${QBDI_VERSION}-${OS}${TAG}-${TARGET}.deb"
    docker cp "package:${DOCKER_BUILD_DIR}/QBDI-${QBDI_VERSION}-linux-${TARGET}.tar.gz" "${BASEDIR}/package-${QBDI_VERSION}/QBDI-${QBDI_VERSION}-${OS}${TAG}-${TARGET}.tar.gz"
    docker rm package
}

build_archlinux () {
    TARGET="$1"
    IMG_TAG="qbdi:package_archlinux_${TARGET}"

    if [[ -n "$TARGET_ARCH" ]] && [[ "$TARGET" != "$TARGET_ARCH" ]]; then
        return
    fi

    docker build "${BASEDIR}" -t ${IMG_TAG} \
                              -f "${GITDIR}/docker/archlinux/Dockerfile.${TARGET}" \
                              --build-arg QBDI_ARCH="$TARGET" \
                              --pull

    docker create --name package ${IMG_TAG}
    docker cp "package:${DOCKER_BUILD_DIR}/QBDI-${TARGET}-${QBDI_VERSION//-/_}-1-x86_64.pkg.tar.zst" "${BASEDIR}/package-${QBDI_VERSION}/QBDI-${QBDI_VERSION}-archlinux$(date +%F)-${TARGET}.pkg.tar.zst"
    docker rm package
}

prepare_archive

# debian12 ARM
build_ubuntu_debian debian "${DEBIAN_TARGET}" ARM

# debian12 AARCH64
build_ubuntu_debian debian "${DEBIAN_TARGET}" AARCH64

# debian12 x86
build_ubuntu_debian debian "${DEBIAN_TARGET}" X86

# debian12 x64
build_ubuntu_debian debian "${DEBIAN_TARGET}" X86_64

# ubuntu lts x64
build_ubuntu_debian ubuntu "${UBUNTU_LTS_TARGET}" X86_64

# ubuntu lts ARM
build_ubuntu_debian ubuntu "${UBUNTU_LTS_TARGET}" ARM

# ubuntu lts AARCH64
build_ubuntu_debian ubuntu "${UBUNTU_LTS_TARGET}" AARCH64

# ubuntu 23.10 x64
build_ubuntu_debian ubuntu "${UBUNTU_LAST_TARGET}" X86_64

# ubuntu 23.10 ARM
build_ubuntu_debian ubuntu "${UBUNTU_LAST_TARGET}" ARM

# ubuntu 23.10 AARCH64
build_ubuntu_debian ubuntu "${UBUNTU_LAST_TARGET}" AARCH64

# archlinux x64
build_archlinux X86_64

# archlinux x86
build_archlinux X86


delete_archive

