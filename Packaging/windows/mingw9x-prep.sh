#!/usr/bin/env bash

SDLDEV_VERS=1.2.15
SODIUM_VERS=1.0.20

# exit when any command fails
set -euo pipefail

MINGW_ARCH=i686-w64-mingw32
SODIUM_ARCH=win32

# set MINGW_PREFIX
MINGW_PREFIX=/usr/${MINGW_ARCH}
if [ ! -d "${MINGW_PREFIX}" ]; then
    echo "MinGW prefix not found (${MINGW_PREFIX})"
    exit 1
else
    echo "Installing to ${MINGW_PREFIX}"
fi

# only use sudo when necessary
if [ `id -u` -ne 0 ]; then
    SUDO=sudo
else
    SUDO=""
fi

rm -rf tmp-mingw9x-prep
mkdir -p tmp-mingw9x-prep
cd tmp-mingw9x-prep

curl --no-progress-meter -OL https://www.libsdl.org/release/SDL-devel-${SDLDEV_VERS}-mingw32.tar.gz
tar -xzf SDL-devel-${SDLDEV_VERS}-mingw32.tar.gz
$SUDO cp -r SDL-*/include/* ${MINGW_PREFIX}/include
$SUDO cp -r SDL-*/lib/* ${MINGW_PREFIX}/lib
$SUDO cp -r SDL-*/bin/* ${MINGW_PREFIX}/bin

wget -q https://github.com/jedisct1/libsodium/releases/download/${SODIUM_VERS}-RELEASE/libsodium-${SODIUM_VERS}-mingw.tar.gz -Olibsodium-${SODIUM_VERS}-mingw.tar.gz
tar -xzf libsodium-${SODIUM_VERS}-mingw.tar.gz --no-same-owner
$SUDO cp -r libsodium-${SODIUM_ARCH}/* ${MINGW_PREFIX}

# Fixup pkgconfig prefix:
find "${MINGW_PREFIX}/lib/pkgconfig/" -name '*.pc' -exec \
  $SUDO sed -i "s|^prefix=.*|prefix=${MINGW_PREFIX}|" '{}' \;

# Fixup CMake prefix:
find "${MINGW_PREFIX}" -name '*.cmake' -exec \
  $SUDO sed -i "s|/opt/local/${MINGW_ARCH}|${MINGW_PREFIX}|" '{}' \;
