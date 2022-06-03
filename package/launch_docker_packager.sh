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
    IMG_TAG="qbdi:package_${OS}_${TAG}_${TARGET}"

    if [[ "$TARGET" = "X86" ]]; then
        DOCKER_IMG="i386/${OS}:${TAG}"
    else
        DOCKER_IMG="${OS}:${TAG}"
    fi

    if [[ "${OS}" = "ubuntu" && "${TAG}" = "18.04" ]]; then
      DOCKERFILE="${GITDIR}/docker/ubuntu_debian/Dockerfile.ubuntu18_04"
    else
      DOCKERFILE="${GITDIR}/docker/ubuntu_debian/Dockerfile"
    fi

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

    docker build "${BASEDIR}" -t ${IMG_TAG} \
                              -f "${GITDIR}/docker/archlinux/Dockerfile.${TARGET}" \
                              --build-arg QBDI_ARCH="$TARGET" \
                              --pull

    docker create --name package ${IMG_TAG}
    docker cp "package:${DOCKER_BUILD_DIR}/QBDI-${TARGET}-${QBDI_VERSION//-/_}-1-x86_64.pkg.tar.zst" "${BASEDIR}/package-${QBDI_VERSION}/QBDI-${QBDI_VERSION}-archlinux$(date +%F)-${TARGET}.pkg.tar.zst"
    docker rm package
}

prepare_archive

# debian11 x86
build_ubuntu_debian debian 11 X86

# debian11 x64
build_ubuntu_debian debian 11 X86_64

# ubuntu 18.04 x86
build_ubuntu_debian ubuntu 18.04 X86

# ubuntu lts x64
build_ubuntu_debian ubuntu 22.04 X86_64

# ubuntu 21.10 x64
build_ubuntu_debian ubuntu 21.10 X86_64

# archlinux x64
build_archlinux X86_64

# archlinux x86
build_archlinux X86


delete_archive

