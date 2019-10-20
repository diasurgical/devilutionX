#!/usr/bin/env bash

set -euo pipefail

declare -r DIR="$(dirname "${BASH_SOURCE[0]}")"
cd "$DIR"
declare -r ABSDIR="$(pwd)"

BUILDROOT="${BUILDROOT:-$HOME/buildroot-rs90-devilutionx}"

set -x

main() {
	set -x
	prepare_buildroot
	build
	package
}

prepare_buildroot() {
	if [[ -d $BUILDROOT ]]; then
		return
	fi
	git clone --depth=1 -b od-rs90 https://github.com/OpenDingux/buildroot.git "$BUILDROOT"
	cp buildroot_rs90_defconfig "$BUILDROOT/configs/rs90_devilutionx_defconfig"
	cd "$BUILDROOT"
	echo 'LIBSODIUM_CONF_OPTS += --enable-static' >> package/libsodium/libsodium.mk
	make rs90_devilutionx_defconfig
	BR2_JLEVEL=0 make toolchain libsodium libzip sdl sdl_mixer sdl_ttf
	cd -
}

build() {
	mkdir -p ../../build
	cd ../../build
	rm -f CMakeCache.txt
	cmake .. -DDINGUX=ON -DUSE_SDL1=ON -DBINARY_RELEASE=ON \
		-DCMAKE_TOOLCHAIN_FILE="$BUILDROOT/output/host/share/buildroot/toolchainfile.cmake"
	make -j $(nproc)
	cd -
}

package() {
	./package-opk.sh ../../build/devilutionx-rs90.opk
}

main
