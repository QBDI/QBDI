#!/usr/bin/env bash

# change python host platform to armv7 arch
# This is needed when compile on X86 or AARCH64 platform, inside a ARM docker
export _PYTHON_HOST_PLATFORM="linux-armv7l"

# run with --skip-dependency-check and --no-isolation to
# avoid compile cmake.whl and ninja.whl that isn't prebuild for armv7
python -m build --skip-dependency-check --no-isolation -w
