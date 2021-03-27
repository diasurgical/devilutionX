#!/usr/bin/env bash

# exit when any command fails
set -euo pipefail

wget https://www.libsdl.org/release/SDL2-devel-2.0.14-mingw.tar.gz
tar -xzf SDL2-devel-2.0.14-mingw.tar.gz
wget https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-devel-2.0.15-mingw.tar.gz
tar -xzf SDL2_ttf-devel-2.0.15-mingw.tar.gz
wget https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-devel-2.0.4-mingw.tar.gz
tar -xzf SDL2_mixer-devel-2.0.4-mingw.tar.gz
wget https://github.com/jedisct1/libsodium/releases/download/1.0.18-RELEASE/libsodium-1.0.18-mingw.tar.gz
tar -xzf libsodium-1.0.18-mingw.tar.gz --no-same-owner
sudo cp -r libsodium-win32/* /usr/i686-w64-mingw32
sudo cp -r SDL2*/i686-w64-mingw32 /usr
