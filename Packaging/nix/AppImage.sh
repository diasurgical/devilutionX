#!/usr/bin/env bash
set -euo pipefail
set -x

BUILD_DIR="${1-build}"
cmake --install "$BUILD_DIR" --prefix "${BUILD_DIR}/AppDir/usr"
mv "$BUILD_DIR"/AppDir/usr/share/diasurgical/devilutionx/devilutionx.mpq "$BUILD_DIR"/AppDir/usr/bin/devilutionx.mpq

APPIMAGE_BUILDER="${APPIMAGE_BUILDER:-linuxdeploy-x86_64.AppImage}"
if ! which "$APPIMAGE_BUILDER"; then
	if ! [[ -f linuxdeploy-x86_64.AppImage ]]; then
		wget -q https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage -N
		chmod +x linuxdeploy-x86_64.AppImage
	fi
	APPIMAGE_BUILDER=./linuxdeploy-x86_64.AppImage
fi
"$APPIMAGE_BUILDER" --appimage-extract-and-run --appdir="$BUILD_DIR"/AppDir --custom-apprun=Packaging/nix/AppRun -d Packaging/nix/devilutionx.desktop -o appimage

mv DevilutionX*.AppImage devilutionx.appimage
