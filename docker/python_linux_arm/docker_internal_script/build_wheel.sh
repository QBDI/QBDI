#!/usr/bin/env bash

# change python host platform to armv7 arch
# This is needed when compile on X86 or AARCH64 platform, inside a ARM docker
export _PYTHON_HOST_PLATFORM="linux-armv7l"

python -m build -w
