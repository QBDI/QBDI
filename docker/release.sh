#!/usr/bin/bash

set -e

HASHFILE="image.hash"
PUSH_IMAGE=0
TARGET_ARCH="$1"

BASEDIR=$(cd $(dirname "$0") && pwd -P)
GITDIR=$(git rev-parse --show-toplevel)

touch "$HASHFILE"
. "${BASEDIR}/common.sh"

docker_login() {
    if [[ "${PUSH_IMAGE}" -ne 0 ]]; then
        docker login
    fi
}

push_image() {
    if [[ "${PUSH_IMAGE}" -ne 0 ]]; then
        docker push "$1"
    fi
}

docker_logout() {
    if [[ "${PUSH_IMAGE}" -ne 0 ]]; then
        docker logout
    fi
}

push_images() {
    TAG="$1"
    shift
    while [[ -n "$1" ]]; do
        docker tag "$TAG" "${DOCKERHUB_REPO}:$1"
        push_image "${DOCKERHUB_REPO}:$1"
        if [[ "$1" != "latest" ]]; then
            docker tag "$TAG" "${DOCKERHUB_REPO}:${QBDI_VERSION}_$1"
            push_image "${DOCKERHUB_REPO}:${QBDI_VERSION}_$1"
        fi
        shift
    done
}

print_hash() {
    IMG="${DOCKERHUB_REPO}:${QBDI_VERSION}_$1"
    echo -n "${IMG} " >> "$HASHFILE"
    docker inspect --format='{{.Id}}' "${IMG}" >> "$HASHFILE"
}

perform_action() {
    ACTION="$1"; shift
    ARCH="$1"; shift
    DOCKER_IMG_BASE="$1"; shift

    INTERNAL_DOCKER_TAG="qbdi:${ARCH}_${DOCKER_IMG_BASE%%:*}_${DOCKER_IMG_BASE##*:}"
    TARGET_DOCKER_TAG="$1"

    if [[ -n "$TARGET_ARCH" ]] && [[ "$ARCH" != "$TARGET_ARCH" ]]; then
        return
    fi

    if [[ "${ACTION}" = "build" ]]; then
        "${BASEDIR}/ubuntu_debian/build.sh" "${ARCH}" "${DOCKER_IMG_BASE}"
    elif [[ "${ACTION}" = "push" ]]; then
        push_images "${INTERNAL_DOCKER_TAG}" "$@"
    elif [[ "${ACTION}" = "hash" ]]; then
        print_hash "${TARGET_DOCKER_TAG}"
    else
        echo "Unknown action ${ACTION}"
        exit 1
    fi
}


perform_action_by_image() {
    ACTION="$1"

    perform_action "$ACTION" "ARM" "debian:${DEBIAN_TARGET}" "armv7_debian_${DEBIAN_TARGET}" "armv7_debian" "armv7"
    perform_action "$ACTION" "AARCH64" "debian:${DEBIAN_TARGET}" "arm64_debian_${DEBIAN_TARGET}" "arm64_debian" "arm64"
    perform_action "$ACTION" "X86" "debian:${DEBIAN_TARGET}" "x86_debian_${DEBIAN_TARGET}" "x86_debian" "x86"
    perform_action "$ACTION" "X86_64" "debian:${DEBIAN_TARGET}" "x64_debian_${DEBIAN_TARGET}" "x64_debian" "x64" "latest"

    perform_action "$ACTION" "ARM" "ubuntu:${UBUNTU_LTS_TARGET}" "armv7_ubuntu_${UBUNTU_LTS_TARGET}" "armv7_ubuntu_lts" "armv7_ubuntu"
    perform_action "$ACTION" "AARCH64" "ubuntu:${UBUNTU_LTS_TARGET}" "arm64_ubuntu_${UBUNTU_LTS_TARGET}" "arm64_ubuntu_lts" "arm64_ubuntu"
    perform_action "$ACTION" "X86_64" "ubuntu:${UBUNTU_LTS_TARGET}" "x64_ubuntu_${UBUNTU_LTS_TARGET}" "x64_ubuntu_lts" "x64_ubuntu"

    perform_action "$ACTION" "ARM" "ubuntu:${UBUNTU_LAST_TARGET}" "armv7_ubuntu_${UBUNTU_LAST_TARGET}"
    perform_action "$ACTION" "AARCH64" "ubuntu:${UBUNTU_LAST_TARGET}" "arm64_ubuntu_${UBUNTU_LAST_TARGET}"
    perform_action "$ACTION" "X86_64" "ubuntu:${UBUNTU_LAST_TARGET}" "x64_ubuntu_${UBUNTU_LAST_TARGET}"
}

perform_action_by_image "build"
if [[ "${PUSH_IMAGE}" -ne 0 ]]; then
    docker_login
fi
perform_action_by_image "push"
if [[ "${PUSH_IMAGE}" -ne 0 ]]; then
    docker_logout
fi
perform_action_by_image "hash"

cat "$HASHFILE"

