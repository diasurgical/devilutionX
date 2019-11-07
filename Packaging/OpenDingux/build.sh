#!/usr/bin/env bash

set -euo pipefail

usage() {
	echo "Usage: build.sh [target]"
	echo "	target: target architecture. Either rs90, rg350 or retrofw"
}

if [[ $# -ne 1 ]]; then
	echo "Error: target is missing"
	usage
	exit 1
fi

if [[ "$1" != "rs90" ]] && [[ "$1" != "rg350" ]] && [[ "$1" != "retrofw" ]]; then
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
	elif [[ "$TARGET" == "rs90" ]]; then
		git clone --depth=1 -b od-rs90 https://github.com/OpenDingux/buildroot.git "$BUILDROOT"
	else
		if [[ ! -f $BUILDROOT_ARCHIVE ]]; then
			\curl https://buildroot.org/downloads/${BUILDROOT_VER}.tar.gz -o "$BUILDROOT_ARCHIVE"
		fi
		tar xf "$BUILDROOT_ARCHIVE" -C "$(dirname "$BUILDROOT_ARCHIVE")"
		mv "${BUILDROOT_ARCHIVE%.tar.gz}" "$BUILDROOT"
	fi
	cp buildroot_${TARGET}_defconfig "$BUILDROOT/configs/${TARGET}_devilutionx_defconfig"
}

make_buildroot() {
	cd "$BUILDROOT"
	if [[ "$TARGET" != "rg350" ]]; then
		if ! grep 'enable-static' package/libsodium/libsodium.mk > /dev/null; then
			echo 'LIBSODIUM_CONF_OPTS += --enable-static' >> package/libsodium/libsodium.mk
		fi
	fi
	make ${TARGET}_devilutionx_defconfig
	if [[ "$TARGET" == "rg350" ]]; then
		BR2_JLEVEL=0 make
	else
		BR2_JLEVEL=0 make toolchain libsodium libzip sdl sdl_mixer sdl_ttf
	fi
	cd -
}

build() {
	mkdir -p ../../build
	cd ../../build
	rm -f CMakeCache.txt

	local -a defs=(-DDINGUX=ON -DBINARY_RELEASE=ON -DPREFILL_PLAYER_NAME=ON)
	if [[ "$TARGET" == "rg350" ]]; then
		defs+=(
			-DNONET=ON
			-DUSE_SDL1=1
			-DJOY_AXIS_LEFTX=0
			-DJOY_AXIS_LEFTY=1
			-DJOY_AXIS_RIGHTX=2
			-DJOY_AXIS_RIGHTY=3
			-DJOY_HAT_DPAD_UP_HAT=0
			-DJOY_HAT_DPAD_UP=1
			-DJOY_HAT_DPAD_DOWN_HAT=0
			-DJOY_HAT_DPAD_DOWN=4
			-DJOY_HAT_DPAD_LEFT_HAT=0
			-DJOY_HAT_DPAD_LEFT=8
			-DJOY_HAT_DPAD_RIGHT_HAT=0
			-DJOY_HAT_DPAD_RIGHT=2
			-DJOY_BUTTON_A=0
			-DJOY_BUTTON_B=1
			-DJOY_BUTTON_Y=2
			-DJOY_BUTTON_X=3
			-DJOY_BUTTON_LEFTSTICK=10
			-DJOY_BUTTON_RIGHTSTICK=11
			-DJOY_BUTTON_RIGHTSHOULDER=5
			-DJOY_BUTTON_LEFTSHOULDER=4
			-DJOY_BUTTON_TRIGGERLEFT=6
			-DJOY_BUTTON_TRIGGERRIGHT=7
			-DJOY_BUTTON_START=9
			-DJOY_BUTTON_BACK=8
		)
	elif [[ "$TARGET" == "rs90" ]]; then
		defs+=(-DUSE_SDL1=ON)
	else
		defs+=(
			-DUSE_SDL1=ON
			-DRETROFW=ON
			-DHAS_KBCTRL=1
			-DKBCTRL_BUTTON_DPAD_LEFT=SDLK_LEFT
			-DKBCTRL_BUTTON_DPAD_RIGHT=SDLK_RIGHT
			-DKBCTRL_BUTTON_DPAD_UP=SDLK_UP
			-DKBCTRL_BUTTON_DPAD_DOWN=SDLK_DOWN
			-DKBCTRL_BUTTON_B=SDLK_LCTRL
			-DKBCTRL_BUTTON_A=SDLK_LALT
			-DKBCTRL_BUTTON_Y=SDLK_SPACE
			-DKBCTRL_BUTTON_X=SDLK_LSHIFT
			-DKBCTRL_BUTTON_RIGHTSHOULDER=SDLK_BACKSPACE
			-DKBCTRL_BUTTON_LEFTSHOULDER=SDLK_TAB
			-DKBCTRL_BUTTON_START=SDLK_RETURN
			-DKBCTRL_BUTTON_LEFTSTICK=SDLK_END # Suspend
			-DKBCTRL_MODIFIER_KEY=SDLK_ESCAPE # Select
			-DKBCTRL_IGNORE_1=SDLK_3 # Backlight
		)
	fi
	cmake .. ${defs[@]} \
		-DCMAKE_TOOLCHAIN_FILE="$BUILDROOT/output/host/usr/share/buildroot/toolchainfile.cmake"
	make -j $(nproc)
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
