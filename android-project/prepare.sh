#!/usr/bin/env bash

cp ../Packaging/resources/CharisSILB.ttf app/src/main/assets

wget http://libsdl.org/release/SDL2-2.0.10.tar.gz
tar -xzf SDL2-2.0.10.tar.gz
rm SDL2-2.0.10/CMakeLists.txt
mv SDL2-2.0.10/* SDL2
rm -rf SDL2-2.0.10 SDL2-2.0.10.tar.gz

wget https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-2.0.15.tar.gz
tar -xzf SDL2_ttf-2.0.15.tar.gz
rm SDL2_ttf-2.0.15/CMakeLists.txt
mv SDL2_ttf-2.0.15/* SDL2_ttf
rm -rf SDL2_ttf-2.0.15 SDL2_ttf-2.0.15.tar.gz
