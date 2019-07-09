
QBDI_VERSION="0.6.2"
DOCKERHUB_REPO="nsurbay/test-qbdi"

prepare_archive (){
    pushd "${GITDIR}" >/dev/null

    git archive -o "${BASEDIR}/qbdi-deps.tar.gz" --prefix=qbdi-deps/ $(git rev-list -1 HEAD -- deps) deps
    git archive -o "${BASEDIR}/qbdi.tar.gz" --prefix=qbdi/ HEAD .

    popd >/dev/null
}

delete_archive() {
    rm -f "${BASEDIR}/qbdi.tar.gz" "${BASEDIR}/qbdi-deps.tar.gz"
}

