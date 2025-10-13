
QBDI_VERSION="0.11.1-devel"
DOCKERHUB_REPO="qbdi/qbdi"
DOCKER_BUILD_DIR="/home/docker/qbdi/build"

DEBIAN_TARGET="trixie"
UBUNTU_LTS_TARGET="24.04"
UBUNTU_LAST_TARGET="25.10"

prepare_archive (){
    pushd "$(git rev-parse --show-toplevel)" >/dev/null

    git archive -o "${BASEDIR}/qbdi.tar.gz" --prefix=qbdi/ HEAD .

    popd >/dev/null
}

delete_archive() {
    rm -f "${BASEDIR}/qbdi.tar.gz"
}

