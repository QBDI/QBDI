#!/bin/sh
set -e

BASEDIR="$(cd "$(dirname "$0")" && pwd -P)"
GITDIR="$(realpath "${BASEDIR}/../..")"

cmake "${GITDIR}"                        \
      -DCMAKE_BUILD_TYPE=Release         \
      -DCMAKE_CROSSCOMPILING=FALSE       \
      -DQBDI_PLATFORM=osx                \
      -DQBDI_ARCH=AARCH64                \
      -G Ninja
