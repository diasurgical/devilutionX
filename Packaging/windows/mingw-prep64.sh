#!/usr/bin/env bash

# exit when any command fails
set -euo pipefail

MINGW_PREFIX=/usr/x86_64-w64-mingw32/

wget https://www.libsdl.org/release/SDL2-devel-2.0.14-mingw.tar.gz
tar -xzf SDL2-devel-2.0.14-mingw.tar.gz
wget https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-devel-2.0.15-mingw.tar.gz
tar -xzf SDL2_ttf-devel-2.0.15-mingw.tar.gz
wget https://www.libsdl.org/projects/SDL_net/release/SDL2_net-devel-2.0.1-mingw.tar.gz
tar -xzf SDL2_net-devel-2.0.1-mingw.tar.gz
sudo cp -r SDL2*/x86_64-w64-mingw32 /usr

wget https://github.com/jedisct1/libsodium/releases/download/1.0.18-RELEASE/libsodium-1.0.18-mingw.tar.gz
tar -xzf libsodium-1.0.18-mingw.tar.gz --no-same-owner
sudo cp -r libsodium-win64/* /usr/x86_64-w64-mingw32

# Fixup pkgconfig prefix:
find "${MINGW_PREFIX}/lib/pkgconfig/" -name '*.pc' -exec \
  sed -i "s|^prefix=.*|prefix=${MINGW_PREFIX}|" '{}' \;

# Fixup CMake prefix:
find "$MINGW_PREFIX" -name '*.cmake' -exec \
  sed -i "s|/opt/local/x86_64-w64-mingw32|${MINGW_PREFIX}|" '{}' \;

# Fixup SDL2's cmake file
# See https://github.com/libsdl-org/SDL/issues/3665
sed "s|@prefix@|${MINGW_PREFIX}|;
s|@exec_prefix@|\${prefix}|;
s|@libdir@|\${exec_prefix}/lib|;
s|@includedir@|\${prefix}/include|;
s|@SDL_RLD_FLAGS@||;
s|@SDL_LIBS@||;
" "$(dirname "$0")"/sdl2-config.cmake.in > "${MINGW_PREFIX}/lib/cmake/SDL2/sdl2-config.cmake"
