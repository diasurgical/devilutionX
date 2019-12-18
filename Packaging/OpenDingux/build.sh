#!/usr/bin/env bash

set -euo pipefail

usage() {
	echo "Usage: build.sh [target]"
	echo "	target: target architecture. Either rg350 or retrofw"
}

if [[ $# -ne 1 ]]; then
	echo "Error: target is missing"
	usage
	exit 1
fi

if [[ "$1" != "rg350" ]] && [[ "$1" != "retrofw" ]]; then
	echo "Error: invalid target"
	usage
	exit 1
fi

declare -r TARGET="${1}"
echo "Building for target: ${TARGET}"

declare -r DIR="$(dirname "${BASH_SOURCE[0]}")"
cd "$DIR"
declare -r ABSDIR="$(pwd)"

if [[ "$TARGET" == "retrofw" ]]; then
	declare -r BUILDROOT_VER=buildroot-2018.02.9
	declare -r BUILDROOT_ARCHIVE="$HOME/${BUILDROOT_VER}.tar.gz"
fi

BUILDROOT="${BUILDROOT:-$HOME/buildroot-${TARGET}-devilutionx}"

main() {
	set -x
	prepare_buildroot
	make_buildroot
	build
	package
}

prepare_buildroot() {
	if [[ -d $BUILDROOT ]]; then
		return
	fi
	if [[ "$TARGET" == "rg350" ]]; then
		git clone --depth=1 https://github.com/tonyjih/RG350_buildroot.git "$BUILDROOT"
	else # retrofw
		if [[ ! -f $BUILDROOT_ARCHIVE ]]; then
			\curl https://buildroot.org/downloads/${BUILDROOT_VER}.tar.gz -o "$BUILDROOT_ARCHIVE"
		fi
		tar xf "$BUILDROOT_ARCHIVE" -C "$(dirname "$BUILDROOT_ARCHIVE")"
		mv "${BUILDROOT_ARCHIVE%.tar.gz}" "$BUILDROOT"
	fi
}

make_buildroot() {
	cp buildroot_${TARGET}_defconfig "$BUILDROOT/configs/${TARGET}_devilutionx_defconfig"
	cd "$BUILDROOT"
	make ${TARGET}_devilutionx_defconfig
	if [[ "$TARGET" == "rg350" ]]; then
		BR2_JLEVEL=0 make
	else
		BR2_JLEVEL=0 make toolchain libzip sdl sdl_mixer sdl_ttf
	fi
	cd -
}

build() {
	mkdir -p ../../build
	cd ../../build
	rm -f CMakeCache.txt

	local -a defs=(-DBINARY_RELEASE=ON)
	if [[ "$TARGET" == "rg350" ]]; then
		defs+=(-DRG350=ON)
	else # retrofw
		defs+=(-DRETROFW=ON)
	fi
	cmake .. ${defs[@]} \
		-DCMAKE_TOOLCHAIN_FILE="$BUILDROOT/output/host/usr/share/buildroot/toolchainfile.cmake"
	make -j $(getconf _NPROCESSORS_ONLN)
	cd -
}

package() {
	if [[ "$TARGET" == "retrofw" ]]; then
		./package-ipk.sh ../../build/devilutionx-retrofw.ipk
	else
		./package-opk.sh ../../build/devilutionx-${TARGET}.opk
	fi
}

main
