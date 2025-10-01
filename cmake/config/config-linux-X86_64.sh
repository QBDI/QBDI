#!/bin/sh
set -e

BASEDIR="$(cd "$(dirname "$0")" && pwd -P)"
GITDIR="$(realpath "${BASEDIR}/../..")"

cmake "${GITDIR}"                        \
      -DCMAKE_BUILD_TYPE=Release         \
      -DCMAKE_CROSSCOMPILING=FALSE       \
      -DQBDI_PLATFORM=linux              \
      -DQBDI_ARCH=X86_64                 \
      -G Ninja
