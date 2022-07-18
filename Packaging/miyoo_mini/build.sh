#!/usr/bin/env bash

cd "$(dirname "${BASH_SOURCE[0]}")/../.."

declare -r CFLAGS="-O3 -marm -mtune=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -march=armv7ve -Wall"
declare -r LDFLAGS="-lSDL -lmi_sys -lmi_gfx -s -lSDL -lSDL_image"
declare -r BUILD_DIR="build-miyoo-mini"

main(){
	rm -f "$BUILD_DIR/CMakeCache.txt"
	cmake_configure -DCMAKE_BUILD_TYPE=Release
	cmake_build
	if [ $? -eq 0 ];
	then
		package_onion
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

package_onion(){
	if [[ ! -d "$BUILD_DIR/SDROOT" ]];
	then
		mkdir $BUILD_DIR/SDROOT
	fi
	yes | cp -rf  Packaging/miyoo_mini/skeleton/* $BUILD_DIR/SDROOT
	if [[ ! -d "$BUILD_DIR/SDROOT/Emu/PORTS/Binaries/Diablo.port/FILES_HERE/assets" ]];
	then
		mkdir $BUILD_DIR/SDROOT/Emu/PORTS/Binaries/Diablo.port/FILES_HERE/assets
	fi
	yes | cp -rf $BUILD_DIR/assets/* $BUILD_DIR/SDROOT/Emu/PORTS/Binaries/Diablo.port/FILES_HERE/assets
	yes | cp -rf $BUILD_DIR/devilutionx $BUILD_DIR/SDROOT/Emu/PORTS/Binaries/Diablo.port/devilutionx
}

main