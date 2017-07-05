#!/usr/bin/env bash

wget http://libsdl.org/release/SDL2-2.0.10.tar.gz
tar -xzf SDL2-2.0.10.tar.gz
rm SDL2-2.0.10/CMakeLists.txt
mv SDL2-2.0.10/* SDL2
rm -rf SDL2-2.0.10 SDL2-2.0.10.tar.gz
