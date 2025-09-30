#!/usr/bin/env bash

set -ex

cd ~/qbdi

PYTHON_VERSION="${1:-all}"

if [[ "${PYTHON_VERSION}" = "all" ]] || [[ "${PYTHON_VERSION}" = "3.8" ]]; then
  /opt/python/cp38-cp38/bin/python -m build -w
fi
if [[ "${PYTHON_VERSION}" = "all" ]] || [[ "${PYTHON_VERSION}" = "3.9" ]]; then
  /opt/python/cp39-cp39/bin/python -m build -w
fi
if [[ "${PYTHON_VERSION}" = "all" ]] || [[ "${PYTHON_VERSION}" = "3.10" ]]; then
  /opt/python/cp310-cp310/bin/python -m build -w
fi
if [[ "${PYTHON_VERSION}" = "all" ]] || [[ "${PYTHON_VERSION}" = "3.11" ]]; then
  /opt/python/cp311-cp311/bin/python -m build -w
fi
if [[ "${PYTHON_VERSION}" = "all" ]] || [[ "${PYTHON_VERSION}" = "3.12" ]]; then
  /opt/python/cp312-cp312/bin/python -m build -w
fi
if [[ "${PYTHON_VERSION}" = "all" ]] || [[ "${PYTHON_VERSION}" = "3.13" ]]; then
  /opt/python/cp313-cp313/bin/python -m build -w
fi
if [[ "${PYTHON_VERSION}" = "all" ]] || [[ "${PYTHON_VERSION}" = "3.14" ]]; then
  /opt/python/cp314-cp314/bin/python -m build -w
fi

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

