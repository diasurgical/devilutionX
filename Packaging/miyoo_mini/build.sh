#!/usr/bin/env bash

progdir=`cd -- "$(dirname "$0")" >/dev/null 2>&1; pwd -P`

# ensure we are in devilutionx root
cd "$progdir/../.."

declare -r CFLAGS="-O3 -marm -mtune=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -march=armv7ve -Wall"
declare -r LDFLAGS="-lSDL -lmi_sys -lmi_gfx -s -lSDL -lSDL_image"
declare -r BUILD_DIR="build-miyoo-mini"
declare -r MIYOO_CUSTOM_SDL_REPO="https://github.com/Brocky/SDL-1.2-miyoo-mini.git"
declare -r MIYOO_CUSTOM_SDL_BRANCH="miniui-miyoomini"

main(){
	rm -f "$BUILD_DIR/CMakeCache.txt"
	cmake_configure -DCMAKE_BUILD_TYPE=Release
	cmake_build
	if [ $? -eq 0 ];
	then
		package_onion
		package_miniui
	fi
}

cmake_configure() {
	cmake -S. -B"$BUILD_DIR" \
		"-DTARGET_PLATFORM=miyoo_mini" \
		-DCMAKE_C_COMPILER=arm-linux-gnueabihf-gcc \
		-DCMAKE_CXX_COMPILER=arm-linux-gnueabihf-g++ \
		-DBUILD_TESTING=OFF \
		-DCMAKE_FIND_ROOT_PATH="/opt/miyoomini-toolchain/arm-linux-gnueabihf/sysroot" \
		"$@"
}

cmake_build(){
	cmake --build "$BUILD_DIR"
}

build_custom_sdl(){
	# make clean folder for custom SDL build
	if [[ -d "$BUILD_DIR/CustomSDL" ]];
	then
		rm -rf $BUILD_DIR/CustomSDL
	fi
	mkdir $BUILD_DIR/CustomSDL
	# clone the repo and build the lib
	cd $BUILD_DIR/CustomSDL
	git clone $MIYOO_CUSTOM_SDL_REPO --branch $MIYOO_CUSTOM_SDL_BRANCH --single-branch .
	./make.sh
	# change back to devilutionx root
	cd "$progdir/../.."
	yes | cp -rfL "$BUILD_DIR/CustomSDL/build/.libs/libSDL-1.2.so.0" "$BUILD_DIR/OnionOS/Emu/PORTS/Binaries/Diablo.port/lib/libSDL-1.2.so.0"
}

prepare_onion_skeleton(){
	if [[ ! -d "$BUILD_DIR/OnionOS" ]];
	then
		mkdir $BUILD_DIR/OnionOS
	fi
	
	# Copy basic skeleton
	yes | cp -rf  Packaging/miyoo_mini/skeleton_OnionOS/* $BUILD_DIR/OnionOS
	
	# ensure devilutionx asset dir
	if [[ ! -d "$BUILD_DIR/OnionOS/Emu/PORTS/Binaries/Diablo.port/assets" ]];
	then
		mkdir -p $BUILD_DIR/OnionOS/Emu/PORTS/Binaries/Diablo.port/assets
	fi
	
	# ensure lib dir for custom SDL
	if [[ ! -d "$BUILD_DIR/OnionOS/Emu/PORTS/Binaries/Diablo.port/lib" ]];
	then
		mkdir -p $BUILD_DIR/OnionOS/Emu/PORTS/Binaries/Diablo.port/lib
	fi
	
	# ensure config dir
	if [[ ! -d "$BUILD_DIR/OnionOS/Saves/CurrentProfile/config/DevilutionX" ]];
	then
		mkdir -p $BUILD_DIR/OnionOS/Saves/CurrentProfile/config/DevilutionX
	fi
	# ensure save dir
	if [[ ! -d "$BUILD_DIR/OnionOS/Saves/CurrentProfile/saves/DevilutionX" ]];
	then
		mkdir -p $BUILD_DIR/OnionOS/Saves/CurrentProfile/saves/DevilutionX
	fi
}

package_onion(){
	prepare_onion_skeleton
	build_custom_sdl
	# copy assets
	yes | cp -rf $BUILD_DIR/assets/* $BUILD_DIR/OnionOS/Emu/PORTS/Binaries/Diablo.port/assets
	# copy executable
	yes | cp -rf $BUILD_DIR/devilutionx $BUILD_DIR/OnionOS/Emu/PORTS/Binaries/Diablo.port/devilutionx
	
	if [[ -f "$BUILD_DIR/onion.zip" ]];
	then
		rm -rf $BUILD_DIR/onion.zip
	fi
	cd $BUILD_DIR/OnionOS
	zip -r ../onion.zip .
	cd "$progdir/../.."
}

prepare_miniui_skeleton(){
	if [[ ! -d "$BUILD_DIR/MiniUI" ]];
	then
		mkdir $BUILD_DIR/MiniUI
	fi
	
	# Copy basic skeleton
	yes | cp -rf  Packaging/miyoo_mini/skeleton_MiniUI/* $BUILD_DIR/MiniUI
	
	# ensure devilutionx asset dir
	if [[ ! -d "$BUILD_DIR/OnionOS/MiniUI/Diablo/assets" ]];
	then
		mkdir -p $BUILD_DIR/OnionOS/MiniUI/Diablo/assets
	fi
}

package_miniui(){
	prepare_miniui_skeleton
	# copy assets
	yes | cp -rf $BUILD_DIR/assets/* $BUILD_DIR/OnionOS/MiniUI/Diablo/assets
	# copy executable
	yes | cp -rf $BUILD_DIR/devilutionx $BUILD_DIR/OnionOS/MiniUI/Diablo/devilutionx
	
	if [[ -f "$BUILD_DIR/miniui.zip" ]];
	then
		rm -rf $BUILD_DIR/miniui.zip
	fi
	cd $BUILD_DIR/MiniUI
	zip -r ../miniui.zip .
	cd "$progdir/../.."
}

main