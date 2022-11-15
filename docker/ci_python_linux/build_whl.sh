#!/usr/bin/env bash

set -ex

cd ~/qbdi

/opt/python/cp38-cp38/bin/python -m build -w
/opt/python/cp39-cp39/bin/python -m build -w
/opt/python/cp310-cp310/bin/python -m build -w
/opt/python/cp311-cp311/bin/python -m build -w

for i in dist/*; do
    auditwheel repair $i -w outwheel;
done

