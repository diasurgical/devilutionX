#!/usr/bin/env bash

# exit when any command fails
set -euo pipefail

#set compiler params
export TARGET='m68k-amigaos'
export SYSROOT=/opt/$TARGET
export M68K_CPU=68040
export M68K_FPU=hard
export M68K_CPU_FPU="-m${M68K_CPU} -m${M68K_FPU}-float"
export M68K_COMMON="-s -ffast-math -fomit-frame-pointer"
export M68K_CFLAGS="${M68K_CPU_FPU} ${M68K_COMMON}"
export M68K_CXXFLAGS="${M68K_CPU_FPU} ${M68K_COMMON}"

declare -ra CMAKE_FLAGS=(
  -DM68K_CPU="$M68K_CPU"
  -DM68K_FPU="$M68K_FPU"
  -DM68K_COMMON="$M68K_COMMON"
  -DBUILD_SHARED_LIBS=OFF
  -DCMAKE_BUILD_TYPE=Release
  -DCMAKE_INSTALL_PREFIX="${SYSROOT}/usr"
)

mkdir -p deps
mkdir -p ${SYSROOT}/usr/lib
mkdir -p ${SYSROOT}/usr/include
cd deps

# ZLIB
wget https://www.zlib.net/zlib-1.2.11.tar.gz -O zlib-1.2.11.tar.gz
tar -xvf zlib-1.2.11.tar.gz
cd zlib-1.2.11
cmake -S. -Bbuild "${CMAKE_FLAGS[@]}" -DM68K_COMMON="${M68K_COMMON} -O3 -fno-exceptions -w -noixemul -DBIG_ENDIAN -DAMIGA -fpermissive -std=c++14"
cmake --build build -j$(getconf _NPROCESSORS_ONLN) --config Release --target install
cd ..

# SDL1.2
wget https://github.com/AmigaPorts/libSDL12/archive/master.tar.gz -O SDL-1.2.tar.gz
tar -xvf SDL-1.2.tar.gz
cd libSDL12-master
make PREFX=${SYSROOT} PREF=${SYSROOT} -j$(getconf _NPROCESSORS_ONLN)
mkdir -p ${SYSROOT}/usr/lib
mkdir -p ${SYSROOT}/usr/include
cp -fvr libSDL.a ${SYSROOT}/usr/lib/
cp -fvr include/* ${SYSROOT}/usr/include/
cd ..

# SDL_mixer SDL-1.2 branch tip as of 15 Mar 2021
SDL_MIXER_VERSION=d1725fcb7c4e987aeb7ecdc94cb8b6375b702170
wget "https://github.com/libsdl-org/SDL_mixer/archive/${SDL_MIXER_VERSION}.zip" -O SDL_mixer-1.2.zip
unzip SDL_mixer-1.2.zip
cd SDL_mixer-${SDL_MIXER_VERSION}
./autogen.sh
SDL_LIBS='-lSDL -ldebug' SDL_CFLAGS="-I${SYSROOT}/usr/include/SDL -noixemul" CFLAGS="${M68K_CFLAGS}" CXXFLAGS="${M68K_CXXFLAGS}" ./configure --disable-sdltest --disable-shared --enable-static --host=${TARGET} --prefix="${SYSROOT}/usr"
make -j$(getconf _NPROCESSORS_ONLN)
make install
cd ..

# FreeType
wget https://download.savannah.gnu.org/releases/freetype/freetype-2.10.4.tar.xz -O freetype-2.10.4.tar.xz
tar -xvf freetype-2.10.4.tar.xz
cd freetype-2.10.4
cmake -S. -Bbuild "${CMAKE_FLAGS[@]}" -DCMAKE_C_FLAGS="-Wno-attributes"
cmake --build build -j$(getconf _NPROCESSORS_ONLN) --config Release --target install
cd ..

# SDL_ttf SDL-1.2 branch tip as of 18 Feb 2021
SDL_TTF_VERSION=70b2940cc75e92aab02a67d2f827caf2836a2c74
wget "https://github.com/libsdl-org/SDL_ttf/archive/${SDL_TTF_VERSION}.zip" -O SDL_ttf-1.2.zip
unzip SDL_ttf-1.2.zip
cd SDL_ttf-${SDL_TTF_VERSION}/
./autogen.sh
LDFLAGS="-L${SYSROOT}/usr/lib" SDL_LIBS='-lSDL -ldebug' SDL_CFLAGS="-L${SYSROOT}/usr/lib -I${SYSROOT}/usr/include/SDL -noixemul"  CFLAGS="${M68K_CFLAGS}" CXXFLAGS="${M68K_CXXFLAGS}" FT2_CFLAGS="-L${SYSROOT}/usr/lib -I${SYSROOT}/usr/include/freetype2" FT2_LIBS="-lfreetype -lzlib" ./configure --disable-shared --enable-static --host=${TARGET} --prefix=${SYSROOT}/usr
make -j$(getconf _NPROCESSORS_ONLN)
make install
cd ..
