#!/usr/bin/env bash

set -euo pipefail

declare -r DIR="$(dirname "${BASH_SOURCE[0]}")"
cd "$DIR"
declare -r ABSDIR="$(pwd)"

BUILDROOT="${BUILDROOT:-$HOME/buildroot-rg350-devilutionx}"

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
	git clone --depth=1 https://github.com/tonyjih/RG350_buildroot.git "$BUILDROOT"
	cp buildroot_rg350_defconfig "$BUILDROOT/configs/rg350_devilutionx_defconfig"
	cd "$BUILDROOT"
	make rg350_devilutionx_defconfig
	BR2_JLEVEL=0 make
	cd -
}

build() {
	mkdir -p ../../build
	cd ../../build
	rm -f CMakeCache.txt
	cmake .. -DDINGUX=ON -DBINARY_RELEASE=ON -DNONET=ON \
		-DCMAKE_TOOLCHAIN_FILE="$BUILDROOT/output/host/usr/share/buildroot/toolchainfile.cmake"
	make -j $(nproc)
	cd -
}

package() {
	./package-opk.sh ../../build/devilutionx-rg350.opk
}

main
