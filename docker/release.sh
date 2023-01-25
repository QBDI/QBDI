#!/usr/bin/bash

set -e

HASHFILE="image.hash"

BASEDIR=$(cd $(dirname "$0") && pwd -P)
GITDIR=$(git rev-parse --show-toplevel)

. "${BASEDIR}/common.sh"

# debian x86 => qbdi:x86_debian_bullseye
"${BASEDIR}/ubuntu_debian/build.sh" "X86" "debian:bullseye"

# debian x64 => qbdi:x64_debian_bullseye
"${BASEDIR}/ubuntu_debian/build.sh" "X64" "debian:bullseye"

# ubuntu x86 => qbdi:x86_ubuntu_18.04
"${BASEDIR}/ubuntu_debian/build.sh" "X86" "ubuntu:18.04"

# ubuntu x64 => qbdi:x64_ubuntu_22.10
"${BASEDIR}/ubuntu_debian/build.sh" "X64" "ubuntu:22.10"

# ubuntu x64 => qbdi:x64_ubuntu_22.04
"${BASEDIR}/ubuntu_debian/build.sh" "X64" "ubuntu:22.04"

# push image

docker login

push_image() {
  docker push "$1"
}


push_images() {
    TAG="$1"
    shift
    while [[ -n "$1" ]]; do
        docker tag "$TAG" "${DOCKERHUB_REPO}:$1"
        push_image "${DOCKERHUB_REPO}:$1"
        docker tag "$TAG" "${DOCKERHUB_REPO}:${QBDI_VERSION}_$1"
        push_image "${DOCKERHUB_REPO}:${QBDI_VERSION}_$1"
        shift
    done
}

push_images "qbdi:x86_debian_bullseye" \
    "x86_debian_bullseye" \
    "x86_debian" \
    "x86"

push_images "qbdi:x64_debian_bullseye" \
    "x64_debian_bullseye" \
    "x64_debian"

push_images "qbdi:x86_ubuntu_18.04" \
    "x86_ubuntu_18.04" \
    "x86_ubuntu"

push_images "qbdi:x64_ubuntu_22.04" \
    "x64_ubuntu_lts" \
    "x64_ubuntu_22.04" \
    "x64_ubuntu" \
    "x64"

push_images "qbdi:x64_ubuntu_22.10" \
    "x64_ubuntu_22.10"

docker tag "qbdi:x64_ubuntu_22.04" "${DOCKERHUB_REPO}:latest"
push_image "${DOCKERHUB_REPO}:latest"

docker logout

# result Hash

print_hash() {
    IMG="${DOCKERHUB_REPO}:${QBDI_VERSION}_$1"
    echo -n "${IMG} " >> "$HASHFILE"
    docker inspect --format='{{.Id}}' "${IMG}" >> "$HASHFILE"
}

echo -n "" > "$HASHFILE"
print_hash "x86_debian_bullseye"
print_hash "x64_debian_bullseye"
print_hash "x86_ubuntu_18.04"
print_hash "x64_ubuntu_22.10"
print_hash "x64_ubuntu_22.04"

cat "$HASHFILE"

