#!/usr/bin/env bash

set -euo pipefail

cd "$(dirname "${BASH_SOURCE[0]}")/../.."

source Packaging/OpenDingux/targets.sh
source Packaging/OpenDingux/package-opk.sh

usage() {
  echo "Usage: build.sh [target]"
  usage_target
}

if ! check_target "$@"; then
  usage
  exit 64
fi

declare -r TARGET="$1"
declare -r BUILD_DIR="build-${TARGET}"
declare -rA BUILDROOT_REPOS=(
	[retrofw]=https://github.com/retrofw/buildroot.git
	[rg350]=https://github.com/tonyjih/RG350_buildroot.git
)
declare -rA BUILDROOT_DEFCONFIGS=(
	[retrofw]='RetroFW_defconfig BR2_EXTERNAL=retrofw'
	[rg350]='rg350_defconfig BR2_EXTERNAL=board/opendingux'
)

declare BUILDROOT_TARGET="$TARGET"
if [[ $TARGET == gkd350h ]]; then
	# Use the rg350 buildroot for gkd350h because gkd350h buildroot is not open-source.
	BUILDROOT_TARGET=rg350
fi
BUILDROOT="${BUILDROOT:-$HOME/devilutionx-buildroots/$BUILDROOT_TARGET}"

main() {
	>&2 echo "Building for target ${TARGET} in ${BUILD_DIR}"
	set -x
	prepare_buildroot
	make_buildroot
	build
	package_opk
}

prepare_buildroot() {
	if [[ -d $BUILDROOT ]]; then
		return
	fi
	git clone --depth=1 "${BUILDROOT_REPOS[$BUILDROOT_TARGET]}" "$BUILDROOT"
	cd "$BUILDROOT"

	# Work around a BR2_EXTERNAL initialization bug in older buildroots.
	mkdir -p output
	touch output/.br-external.mk

	make ${BUILDROOT_DEFCONFIGS[$BUILDROOT_TARGET]}
	cd -
}

make_buildroot() {
	cd "$BUILDROOT"
	BR2_JLEVEL=0 make toolchain libzip sdl sdl_mixer sdl_ttf
	cd -
}

build() {
	mkdir -p "$BUILD_DIR"
	cd "$BUILD_DIR"
	rm -f CMakeCache.txt
	cmake .. -DBINARY_RELEASE=ON "-DTARGET_PLATFORM=$TARGET" \
		-DCMAKE_TOOLCHAIN_FILE="$BUILDROOT/output/host/usr/share/buildroot/toolchainfile.cmake"
	make -j $(getconf _NPROCESSORS_ONLN)
	cd -
}

main
