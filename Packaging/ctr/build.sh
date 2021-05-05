#!/usr/bin/env bash

set -euo pipefail

DEVKITPRO="${DEVKITPRO:-/opt/devkitpro}"

declare -r DIR="$(dirname "${BASH_SOURCE[0]}")"
cd "$DIR"

main() {
	prepare_devkitpro
	set -x
	install_deps
	build
}

build() {
	mkdir -p ../../build
	cd ../../build
	rm -f CMakeCache.txt
	DEVKITPRO="$DEVKITPRO" cmake .. -DBINARY_RELEASE=ON \
		-DCMAKE_TOOLCHAIN_FILE=/opt/devkitpro/3ds.cmake
	DEVKITPRO="$DEVKITPRO" make -j "$(nproc)"
	cd -
}

install_deps() {
	"$DEVKITPRO/pacman/bin/pacman" -S --needed --noconfirm --quiet \
		devkitARM general-tools 3dstools devkitpro-pkgbuild-helpers \
		libctru citro3d 3ds-sdl 3ds-sdl_ttf 3ds-freetype
}

prepare_devkitpro() {
	if [[ -d $DEVKITPRO ]]; then
		return;
	fi
	if which dpkg > /dev/null; then
		install_devkitpro_debian
	else
		>&2 printf "Please set DEVKITPRO:\nhttps://devkitpro.org/wiki/Getting_Started\n"
		exit 1
	fi
}

install_devkitpro_debian() {
	>&2 echo 'Installing devkitpro-pacman.deb from GitHub...'
	local -r dpkg_path=/tmp/devkitpro-pacman.deb
	set -x
	\curl -L https://github.com/devkitPro/pacman/releases/download/devkitpro-pacman-1.0.1/devkitpro-pacman.deb -o "$dpkg_path"
	sudo dpkg -i "$dpkg_path"
	rm "$dpkg_path"
	{ set +x; } 2>/dev/null
}

main
