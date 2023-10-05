#!/usr/bin/env bash

set -ex

cd ~/qbdi

/opt/python/cp38-cp38/bin/python -m build -w
/opt/python/cp39-cp39/bin/python -m build -w
/opt/python/cp310-cp310/bin/python -m build -w
/opt/python/cp311-cp311/bin/python -m build -w
/opt/python/cp312-cp312/bin/python -m build -w

if [[ "${QBDI_ARCH}" = "X86_64" ]]; then
    for i in dist/*_x86_64.whl; do
        auditwheel repair $i -w outwheel;
    done
elif [[ "${QBDI_ARCH}" = "X86" ]]; then
    for i in dist/*_i686.whl; do
        auditwheel repair $i -w outwheel;
    done
elif [[ "${QBDI_ARCH}" = "AARCH64" ]]; then
    for i in dist/*_aarch64.whl; do
        auditwheel repair $i -w outwheel;
    done
else
    echo "Unknown QBDI_ARCH : ${QBDI_ARCH}"
    exit 1
fi

