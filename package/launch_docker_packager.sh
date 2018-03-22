#!/bin/sh
VERSION=0.6.1
RELEASE_VERSION=$VERSION

BASEDIR=$(cd $(dirname "$0") && pwd -P)
cd "$BASEDIR/.."
# Create an archive with only deps (with mtime based on latest commit in subdirectory)
git archive -o "$BASEDIR/qbdi-deps.tar.gz" --prefix=qbdi-deps/ $(git rev-list -1 HEAD -- deps) deps
# Create a full archive using HEAD
git archive -o "$BASEDIR/qbdi.tar.gz" --prefix=qbdi/ HEAD .
cd "$BASEDIR"

# Arch Linux
docker build -t qbdi_archlinux_snapshot:2017-10-21 -f archlinux/Dockerfile .
docker run --rm qbdi_archlinux_snapshot:2017-10-21 cat qbdi/build/QBDI-$VERSION-1-x86_64.pkg.tar.xz > QBDI-$RELEASE_VERSION-archlinux20171021-X86_64.pkg.tar.xz

# Debian stable
docker build -t qbdi_debian_latest -f debian_docker_packager/Dockerfile .
docker run --rm qbdi_debian_latest cat qbdi/build/QBDI-$VERSION-linux-X86_64.deb > QBDI-$RELEASE_VERSION-debian9-X86_64.deb

# Ubuntu stable
docker build -t qbdi_ubuntu_latest -f ubuntu_latest_docker_packager/Dockerfile .
docker run --rm qbdi_ubuntu_latest cat qbdi/build/QBDI-$VERSION-linux-X86_64.deb > QBDI-$RELEASE_VERSION-ubuntu17.10-X86_64.deb

# Ubuntu LTS
docker build -t qbdi_ubuntu_lts -f ubuntu_lts_docker_packager/Dockerfile .
docker run --rm qbdi_ubuntu_lts cat qbdi/build/QBDI-$VERSION-linux-X86_64.deb > QBDI-$RELEASE_VERSION-ubuntu16.04-X86_64.deb

# Linux ARM
docker build -t qbdi_linux_arm -f linux_ARM_docker_packager/Dockerfile .
docker run --rm qbdi_linux_arm cat qbdi/build/QBDI-$VERSION-linux-ARM.tar.gz > QBDI-$RELEASE_VERSION-linux-ARM.tar.gz

# Android ARM
docker build -t qbdi_android_arm -f android_ARM_docker_packager/Dockerfile .
docker run --rm qbdi_android_arm cat qbdi/build/QBDI-$VERSION-android-ARM.tar.gz > QBDI-$RELEASE_VERSION-android-ARM.tar.gz
