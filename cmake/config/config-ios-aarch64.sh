#!/bin/sh
set -e

BASEDIR="$(cd "$(dirname "$0")" && pwd -P)"
GITDIR="$(realpath "${BASEDIR}/../..")"

cmake "${GITDIR}"                        \
      -DQBDI_PLATFORM=ios                \
      -DQBDI_ARCH=AARCH64                \
      -DCMAKE_BUILD_TYPE=Release         \
      -DCMAKE_TOOLCHAIN_FILE="${GITDIR}/cmake/config/ios.toolchain.cmake" \
      -DCMAKE_OSX_DEPLOYMENT_TARGET="15.0" \
      -G Ninja

