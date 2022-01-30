#!/usr/bin/env bash

set -e
SCRIPTDIR="${BASH_SOURCE[0]}"
SCRIPTDIR="$(dirname "${SCRIPTDIR}")"

cmake -S "${SCRIPTDIR}/../../" \
      -B build-ps4 \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_TOOLCHAIN_FILE="/opt/pacbrew/ps4/openorbis/cmake/ps4.cmake"

cmake --build build-ps4 -j $(getconf _NPROCESSORS_ONLN)

