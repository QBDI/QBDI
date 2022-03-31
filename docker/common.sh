
QBDI_VERSION="0.9.1-devel"
DOCKERHUB_REPO="qbdi/qbdi"
DOCKER_BUILD_DIR="/home/docker/qbdi/build"

prepare_archive (){
    pushd "$(git rev-parse --show-toplevel)" >/dev/null

    git archive -o "${BASEDIR}/qbdi.tar.gz" --prefix=qbdi/ HEAD .

    popd >/dev/null
}

delete_archive() {
    rm -f "${BASEDIR}/qbdi.tar.gz"
}

