#!/usr/bin/env bash

set -ex

cd ~/qbdi

/opt/python/cp38-cp38/bin/python setup.py bdist_wheel
/opt/python/cp39-cp39/bin/python setup.py bdist_wheel
/opt/python/cp310-cp310/bin/python setup.py bdist_wheel

for i in dist/*; do
    auditwheel repair $i -w outwheel;
done

