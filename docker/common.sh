
QBDI_VERSION="0.8.1-devel"
DOCKERHUB_REPO="qbdi/qbdi"
DOCKER_BUILD_DIR="/home/docker/qbdi/build"

rebuild_archive() {
    ARCHIVE="$1"
    TMPDIR="$(mktemp -d)"

    tar -xf "$ARCHIVE" -C "$TMPDIR"
    rm "$ARCHIVE"
    tar --sort=name --no-acls --no-selinux --no-xattrs --mtime=1970-01-01 -C "$TMPDIR" -czf "$ARCHIVE" .
    rm -r "$TMPDIR"
}

prepare_archive (){
    pushd "$(git rev-parse --show-toplevel)" >/dev/null

    git archive -o "${BASEDIR}/qbdi.tar.gz" --prefix=qbdi/ HEAD .

    git archive -o "${BASEDIR}/qbdi-deps.tar.gz" --prefix=qbdi-deps/ $(git rev-list -1 HEAD -- deps) deps
    # need a deterministic archive, to keep docker cache if file content doesn't change
    rebuild_archive "${BASEDIR}/qbdi-deps.tar.gz"

    popd >/dev/null
}

delete_archive() {
    rm -f "${BASEDIR}/qbdi.tar.gz" "${BASEDIR}/qbdi-deps.tar.gz"
}

