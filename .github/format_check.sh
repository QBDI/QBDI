#!/usr/bin/env bash

set -e

cd $(dirname "$0")
RETURN_CODE=0
BASEDIR=$(pwd -P)
GITDIR=$(git rev-parse --show-toplevel)
cd $GITDIR

for file in $(git ls-files | grep -v -f "${GITDIR}/.clang-format.exclude-list.txt" | grep '\.\(cpp\|c\|h\|hpp\)$'); do
  if [[ -n "${CLANG_FORMAT_VERSION}" ]]; then
    clang-format-"${CLANG_FORMAT_VERSION}" -Werror --style=file --verbose -n "${file}" || RETURN_CODE=1
  else
    clang-format -Werror --style=file --verbose -n "${file}" || RETURN_CODE=1
  fi
done

cmake-format -c .cmake_format.conf.py -l info --check $(git ls-files | grep -v -f "${GITDIR}/.clang-format.exclude-list.txt" | grep '\(CMakeLists.txt\|\.cmake\)$')


exit ${RETURN_CODE}
