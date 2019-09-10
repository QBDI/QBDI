#!/usr/bin/bash

set -e

HASHFILE="image.hash"

BASEDIR=$(cd $(dirname "$0") && pwd -P)
GITDIR=$(cd "${BASEDIR}/../.." && pwd -P)

. "${BASEDIR}/common.sh"

# debian x86 => qbdi:x86_debian_buster
"${BASEDIR}/ubuntu_debian/build.sh" "X86" "debian:buster"

# debian x64 => qbdi:x64_debian_buster
"${BASEDIR}/ubuntu_debian/build.sh" "X64" "debian:buster"

# ubuntu x86 => qbdi:x86_ubuntu_18.04
"${BASEDIR}/ubuntu_debian/build.sh" "X86" "ubuntu:18.04"

# ubuntu x64 => qbdi:x64_ubuntu_18.04
"${BASEDIR}/ubuntu_debian/build.sh" "X64" "ubuntu:18.04"

# ubuntu x86 => qbdi:x86_ubuntu_19.04
"${BASEDIR}/ubuntu_debian/build.sh" "X86" "ubuntu:19.04"

# ubuntu x64 => qbdi:x64_ubuntu_19.04
"${BASEDIR}/ubuntu_debian/build.sh" "X64" "ubuntu:19.04"


# push image

docker login
push_image() {
    TAG="$1"
    shift
    while [[ -n "$1" ]]; do
        docker tag "$TAG" "${DOCKERHUB_REPO}:$1"
        docker push "${DOCKERHUB_REPO}:$1"
        docker tag "$TAG" "${DOCKERHUB_REPO}:${QBDI_VERSION}_$1"
        docker push "${DOCKERHUB_REPO}:${QBDI_VERSION}_$1"
        shift
    done
}

push_image "qbdi:x86_debian_buster" \
    "x86_debian_buster" \
    "x86_debian"
                
push_image "qbdi:x64_debian_buster" \
    "x64_debian_buster" \
    "x64_debian"

push_image "qbdi:x86_ubuntu_18.04" \
    "x86_ubuntu_18.04" \
    "x86_ubuntu" \
    "x86"

push_image "qbdi:x64_ubuntu_18.04" \
    "x64_ubuntu_18.04" \
    "x64_ubuntu" \
    "x64"

push_image "qbdi:x86_ubuntu_19.04" \
    "x86_ubuntu_19.04"

push_image "qbdi:x64_ubuntu_19.04" \
    "x64_ubuntu_19.04"

docker tag "qbdi:x64_ubuntu_18.04" "${DOCKERHUB_REPO}:latest"
docker push "${DOCKERHUB_REPO}:latest"

docker logout

# result Hash

print_hash() {
    IMG="${DOCKERHUB_REPO}:${QBDI_VERSION}_$1"
    echo -n "${IMG} " >> "$HASHFILE"
    docker inspect --format='{{.Id}}' "${IMG}" >> "$HASHFILE"
}

echo -n "" > "$HASHFILE"
print_hash "x86_debian_buster"
print_hash "x64_debian_buster"
print_hash "x86_ubuntu_18.04"
print_hash "x64_ubuntu_18.04"
print_hash "x86_ubuntu_19.04"
print_hash "x64_ubuntu_19.04"

cat "$HASHFILE"

