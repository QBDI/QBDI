#!/usr/bin/env bash

set -ex

cd ~/qbdi

ORIGIN_PATH="${PATH}"

if [[ -z "$(find "deps/llvm/${QBDI_PLATFORM}-${QBDI_ARCH}/lib" -type f -print -quit)" ]] ||
   [[ -z "$(find "deps/llvm/${QBDI_PLATFORM}-${QBDI_ARCH}/include" -type f -print -quit)" ]]; then

    pushd deps/llvm/

    export PATH="${PYTHON_OPT}/cp37-cp37m/bin:${ORIGIN_PATH}"

    python3 build.py prepare ${QBDI_PLATFORM}-${QBDI_ARCH}
    python3 build.py build ${QBDI_PLATFORM}-${QBDI_ARCH}
    python3 build.py package ${QBDI_PLATFORM}-${QBDI_ARCH}
    python3 build.py clean ${QBDI_PLATFORM}-${QBDI_ARCH}

    popd
fi

export PATH="${ORIGIN_PATH}"

/opt/python/cp36-cp36m/bin/python setup.py bdist_wheel
/opt/python/cp37-cp37m/bin/python setup.py bdist_wheel
/opt/python/cp38-cp38/bin/python setup.py bdist_wheel
/opt/python/cp39-cp39/bin/python setup.py bdist_wheel

for i in dist/*; do
    auditwheel repair $i -w outwheel;
done

