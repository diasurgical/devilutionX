#!/usr/bin/env bash
set -euo pipefail

declare -r PACKAGING_DIR=`cd -- "$(dirname "$0")" >/dev/null 2>&1; pwd -P`
declare -r CFLAGS="-O3 -marm -mtune=cortex-a9 -mfpu=neon-vfpv4 -mfloat-abi=soft -march=armv7-a -Wall"
declare -r LDFLAGS="-lSDL -lmi_sys -lmi_gfx -s -lSDL -lSDL_image"
declare -r BUILD_DIR="build-rg35xx-garlic"

main() {
	# ensure we are in devilutionx root
	cd "$PACKAGING_DIR/../.."

	rm -f "$BUILD_DIR/CMakeCache.txt"
	cmake_configure -DCMAKE_BUILD_TYPE=Release
	cmake_build
	package_garlic
}

cmake_configure() {
	cmake -S. -B"$BUILD_DIR" \
		-DTARGET_PLATFORM=rg35xx_garlic \
		-DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_TOOLCHAIN_FILE="${PACKAGING_DIR}/toolchainfile.cmake" \
		-DBUILD_TESTING=OFF \
		"$@"
}

cmake_build() {
	cmake --build "$BUILD_DIR" -j $(getconf _NPROCESSORS_ONLN)
}

prepare_garlic_skeleton() {
	mkdir -p $BUILD_DIR/GarlicOS

	# Copy basic skeleton
	cp -rf  Packaging/rg35xx_garlic/skeleton_GarlicOS/* $BUILD_DIR/GarlicOS

	# ensure devilutionx asset dir
	mkdir -p $BUILD_DIR/GarlicOS/ROMS/PORTS/Diablo/assets

	# ensure lib dir for custom SDL -- Not Needed
	#mkdir -p $BUILD_DIR/GarlicOS/ROMS/PORTS/Diablo/lib
}

package_garlic() {
	prepare_garlic_skeleton
	# copy assets
	cp -rf $BUILD_DIR/assets/* $BUILD_DIR/GarlicOS/ROMS/PORTS/Diablo/assets
	# copy executable
	cp -f $BUILD_DIR/devilutionx $BUILD_DIR/GarlicOS/ROMS/PORTS/Diablo/devilutionx
	# copy SDL1.2 -- EDIT: Apparently not needed
	# cp -rfL "/opt/miyoo/lib/libSDL-1.2.so.0" "$BUILD_DIR/GarlicOS/Roms/PORTS/Diablo/lib/libSDL-1.2.so.0"

	rm -f $BUILD_DIR/devilutionx-rg35xx-garlic-os.zip

	cd $BUILD_DIR/GarlicOS
	zip -r ../devilutionx-rg35xx-garlic-os.zip .
	cd "$PACKAGING_DIR/../.."
}

main
