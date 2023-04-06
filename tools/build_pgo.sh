#!/usr/bin/env bash

# Builds a PGO-optimized binary with the profile data gathered by running the test demo.
set -euo pipefail

PARALLELISM="$(getconf _NPROCESSORS_ONLN)"

set -x

rm -rf build-profile-data/config build-profile-data/profile
mkdir -p build-profile-data/config
cd build-profile-data/config
ln -s ../../test/fixtures/timedemo/WarriorLevel1to2/demo_* .
cp ../../test/fixtures/timedemo/WarriorLevel1to2/spawn_* .
cd -

# We build both versions with the same FetchContent base directory because otherwise
# gcc will complain about the source locations for FetchContent dependencies,
# which are stored in the build directory by default.
# Ideally, we would only specify the location for the FetchContent src directories
# but CMake does not support that.
cmake -S. -Bbuild-profile-generate -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DDEVILUTIONX_PROFILE_GENERATE=ON \
  -DDEVILUTIONX_PROFILE_DIR="${PWD}/build-profile-data/profile" \
  -DFETCHCONTENT_BASE_DIR="${PWD}/build-profile-data/fetchcontent-base" \
  -DBUILD_TESTING=OFF "$@"
cmake --build build-profile-generate -j "$PARALLELISM"
build-profile-generate/devilutionx --diablo --spawn --lang en --demo 0 --timedemo \
  --save-dir build-profile-data/config

cmake -S. -Bbuild-profile-use -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DDEVILUTIONX_PROFILE_USE=ON \
  -DDEVILUTIONX_PROFILE_DIR="${PWD}/build-profile-data/profile" \
  -DFETCHCONTENT_BASE_DIR="${PWD}/build-profile-data/fetchcontent-base" \
  -DBUILD_TESTING=OFF "$@"
cmake --build build-profile-use -j "$PARALLELISM"
