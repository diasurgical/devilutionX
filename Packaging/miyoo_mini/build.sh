#!/usr/bin/env bash

cd "$(dirname "${BASH_SOURCE[0]}")/../.."

declare -r CFLAGS="-O3 -marm -mtune=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -march=armv7ve -Wall"
declare -r LDFLAGS="-lSDL -lSDL_ttf -lmi_sys -lmi_gfx -s -lSDL -lSDL_image"
declare -r BUILD_DIR="build-miyoo-mini"

main(){
	rm -f "$BUILD_DIR/CMakeCache.txt"
	cmake_configure -DCMAKE_BUILD_TYPE=Release
	cmake_build
	package
}

cmake_configure() {
	# libzt uses `-fstack-protector` GCC flag by default.
	# We disable `-fstack-protector` because it isn't supported by target libc.
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

package(){
	mkdir $BUILD_DIR/SDROOT
	cp -r Packaging/miyoo_mini/skeleton/* $BUILD_DIR/SDROOT
	cp $BUILD_DIR/devilutionx $BUILD_DIR/SDROOT/Emu/PORTS/Binaries/Diablo.port/devilutionx
}

main