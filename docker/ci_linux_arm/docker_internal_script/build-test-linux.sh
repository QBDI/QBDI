#!/usr/bin/env bash

set -ex

mkdir -p "build-docker-${QBDI_PLATFORM}-${QBDI_ARCH}"
cd "build-docker-${QBDI_PLATFORM}-${QBDI_ARCH}"

# generate clean toolchains file
TOOLCHAIN_FILE="QBDI_${QBDI_ARCH}_Toolchain.cmake"
cat "${CMAKE_TOOLCHAIN_FILE}" > "${TOOLCHAIN_FILE}"
sed -e "s#\$ENV{CROSS_TRIPLE}#${CROSS_TRIPLE}#" \
    -e "s#\$ENV{CROSS_ROOT}#${CROSS_ROOT}#" \
    -e "s#\$ENV{CC}#${CC}#" \
    -e "s#\$ENV{CXX}#${CXX}#" \
    -e "s#\$ENV{FC}#${FC}#" \
    -i "${TOOLCHAIN_FILE}"

# create an empty toolchains to break the propagation of crosscompile
# environment when compile llvm-tblgen
EMPTY_TOOLCHAINS="$(pwd)/QBDI_empty_toolchains.cmake"
touch "${EMPTY_TOOLCHAINS}"

# need to have a clear env to avoid crosscompiler
# to be used to compile llvm-tblgen
clean_env() {
  env -i \
    PATH="${PATH}" \
    HOME="${HOME}" \
    CLICOLOR_FORCE="1" \
    "$@"
}

clean_env \
    cmake -G Ninja \
      -DCMAKE_TOOLCHAIN_FILE="${TOOLCHAIN_FILE}" \
      -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -DQBDI_LLVM_TABLEN_TOOLSCHAIN="${EMPTY_TOOLCHAINS}" \
      -DQBDI_PLATFORM="${QBDI_PLATFORM}" \
      -DQBDI_ARCH="${QBDI_ARCH}" \
      -DQBDI_EXAMPLES=ON \
      -DQBDI_TOOLS_VALIDATOR=OFF \
      -DQBDI_TOOLS_PYQBDI=OFF \
      ..

clean_env ninja

if [[ "${QBDI_ARCH}" = "ARM" ]]; then
  qemu-arm-static ./test/QBDITest
elif [[ "${QBDI_ARCH}" = "AARCH64" ]]; then
  qemu-aarch64-static ./test/QBDITest
fi

clean_env cpack

exit 0
