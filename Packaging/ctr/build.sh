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
	local -ra defs=(
		-DBINARY_RELEASE=ON
		-DUSE_SDL1=ON
		-DSDL1_VIDEO_MODE_FLAGS='SDL_SWSURFACE|SDL_CONSOLEBOTTOM'
		-DNONET=ON
		-DPREFILL_PLAYER_NAME=ON
		-DHAS_KBCTRL=1
		-DKBCTRL_BUTTON_DPAD_LEFT=SDLK_LEFT
		-DKBCTRL_BUTTON_DPAD_RIGHT=SDLK_RIGHT
		-DKBCTRL_BUTTON_DPAD_UP=SDLK_UP
		-DKBCTRL_BUTTON_DPAD_DOWN=SDLK_DOWN
		-DKBCTRL_BUTTON_B=SDLK_a
		-DKBCTRL_BUTTON_A=SDLK_b
		-DKBCTRL_BUTTON_Y=SDLK_y
		-DKBCTRL_BUTTON_X=SDLK_x
		-DKBCTRL_BUTTON_RIGHTSHOULDER=SDLK_r
		-DKBCTRL_BUTTON_LEFTSHOULDER=SDLK_l
		-DKBCTRL_BUTTON_START=SDLK_RETURN
		-DKBCTRL_BUTTON_BACK=SDLK_ESCAPE
		-DKBCTRL_MODIFIER_KEY=SDLK_END
	)
	DEVKITPRO="$DEVKITPRO" cmake .. "${defs[@]}" \
		-DCMAKE_TOOLCHAIN_FILE=../CMake/ctr/devkitarm-libctru.cmake
	DEVKITPRO="$DEVKITPRO" make -j "$(nproc)"
	cd -
}

install_deps() {
	"$DEVKITPRO/pacman/bin/pacman" -S --needed --noconfirm --quiet \
		3ds-sdl 3ds-sdl_ttf 3ds-sdl_mixer \
		3ds-freetype 3ds-libogg 3ds-libvorbisidec 3ds-mikmod \
		libctru citro3d picasso devkitARM general-tools 3dslink 3dstools devkitpro-pkgbuild-helpers
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
