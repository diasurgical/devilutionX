#!/bin/bash

echo "============= Getting Libs ============="

curl -O https://www.libsdl.org/release/SDL2-2.0.9.tar.gz
curl -O https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.4.tar.gz
curl -O https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-2.0.15.tar.gz

echo "============= Decompress Libs ============="

tar -zxvf SDL2-2.0.9.tar.gz
tar -zxvf SDL2_mixer-2.0.4.tar.gz
tar -zxvf SDL2_ttf-2.0.15.tar.gz

echo "============= Remove temp files ============="

rm SDL2-2.0.9.tar.gz
rm SDL2_mixer-2.0.4.tar.gz
rm SDL2_ttf-2.0.15.tar.gz

mv SDL2-2.0.9 ../SDL
mv SDL2_mixer-2.0.4 ../SDL2_mixer
mv SDL2_ttf-2.0.15 ../SDL2_ttf
