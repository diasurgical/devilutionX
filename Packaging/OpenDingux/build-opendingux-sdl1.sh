#!/usr/bin/env bash

set -euo pipefail

declare -r DIR="$(dirname "${BASH_SOURCE[0]}")"
cd "$DIR"
declare -r ABSDIR="$(pwd)"

declare -r BUILDROOT_VER=buildroot-2018.02.9
BUILDROOT="${BUILDROOT:-$HOME/${BUILDROOT_VER}-opendingux-musl}"

declare -r BUILDROOT_ARCHIVE="$HOME/${BUILDROOT_VER}.tar.gz"

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
	if [[ ! -f $BUILDROOT_ARCHIVE ]]; then
		\curl https://buildroot.org/downloads/${BUILDROOT_VER}.tar.gz -o "$BUILDROOT_ARCHIVE"
	fi

	tar xf "$BUILDROOT_ARCHIVE" -C "$(dirname "$BUILDROOT_ARCHIVE")"
	mv "${BUILDROOT_ARCHIVE%.tar.gz}" "$BUILDROOT"
	cp buildroot_opendingux_musl_defconfig "$BUILDROOT/configs/opendingux_musl_defconfig"
	cd "$BUILDROOT"
	echo 'LIBSODIUM_CONF_OPTS += --enable-static' >> package/libsodium/libsodium.mk
	make opendingux_musl_defconfig
	BR2_JLEVEL=0 make toolchain libsodium libzip sdl sdl_mixer sdl_ttf
	cd -
}

build() {
	mkdir -p ../../build
	cd ../../build
	rm -f CMakeCache.txt
	cmake .. -DDINGUX=ON -DUSE_SDL1=ON -DBINARY_RELEASE=ON \
		-DCMAKE_TOOLCHAIN_FILE="$BUILDROOT/output/host/share/buildroot/toolchainfile.cmake"
	cd -
}

package() {
	./package.sh ../../build/devilutionx-opendingux-musl-sdl1.ipk
}

main
