#!/usr/bin/env bash

set -e

cd $(dirname "$0")
BASEDIR=$(pwd -P)
GITDIR=$(git rev-parse --show-toplevel)

cd $GITDIR

RETURN_CODE=0

for file in $(git ls-files | grep '\.\(cpp\|c\|h\|hpp\)$'); do
    clang-format -Werror --style=file -n "${file}" || RETURN_CODE=1
done

exit ${RETURN_CODE}
