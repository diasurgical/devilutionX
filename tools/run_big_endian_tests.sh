#!/usr/bin/env bash

cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.."
PARALLELISM="$(getconf _NPROCESSORS_ONLN)"

set -xeuo pipefail

if [[ "$(docker images -q devilutionx-s390x-test)" = "" ]]; then
	docker run --rm --privileged multiarch/qemu-user-static --reset -p yes
	docker build -f tools/Dockerfile.s390x -t devilutionx-s390x-test tools/
fi

# We disable ASAN and UBSAN for now because of:
# https://gitlab.alpinelinux.org/alpine/aports/-/issues/14435
docker run --platform linux/s390x -u "$(id -u "$USER"):$(id -g "$USER")" --rm --mount "type=bind,source=${PWD},target=/host" devilutionx-s390x-test sh -c "cd /host && \
export CCACHE_DIR=/host/.s390x-ccache && \
cmake -S. -Bbuild-s390x-test -G Ninja -DASAN=OFF -DUBSAN=OFF -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++ \
  -DNONET=ON -DNOSOUND=ON && \
ln -sf /opt/spawn.mpq /host/build-s390x-test/spawn.mpq && \
cmake --build build-s390x-test -j ${PARALLELISM} && \
ctest --test-dir build-s390x-test --output-on-failure -j ${PARALLELISM}"
