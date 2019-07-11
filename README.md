[![Downloads](https://img.shields.io/github/downloads/diasurgical/devilutionX/total.svg)](https://github.com/diasurgical/devilutionX/releases)
[![github stars](https://img.shields.io/github/stars/diasurgical/devilutionX.svg)](https://github.com/diasurgical/devilutionX/stargazers)

Status | Platform
---:| ---
[![CircleCI](https://circleci.com/gh/diasurgical/devilutionX.svg?style=svg)](https://circleci.com/gh/diasurgical/devilutionX) | Linux 32bit & 64bit, Windows 32bit
[![Build Status](https://travis-ci.org/diasurgical/devilutionX.svg?branch=master)](https://travis-ci.org/diasurgical/devilutionX) | macOS 32bit & 64bit
[![Build status](https://ci.appveyor.com/api/projects/status/1a0jus2372qvksht?svg=true)](https://ci.appveyor.com/project/AJenbo/devilutionx) | Windows MSVC

![Discord Channel](https://avatars3.githubusercontent.com/u/1965106?s=16&v=4) [Discord Chat Channel](https://discord.gg/aQBQdDe)

# How To Play:
 - Copy diabdat.mpq from your CD, or GoG install folder, to the DevilutionX game directory ; Make sure it is all lowercase.
 - [Download DevilutionX](https://github.com/diasurgical/devilutionX/releases), or build from source
 - Install SDL2 (including SDL2_mixer and SDL2_ttf) make sure to get the appropriate 32bit or 64bit version.
 - Run `./devilutionx`

Please keep in mind that this is still being worked on and is missing parts of UI and some minor bugs, see [milestone 1](https://github.com/diasurgical/devilutionX/milestone/1) for a full list of known issues.

# Building from Source
## 32-bit building on 64-bit platforms
<details><summary>Linux</summary>

### Installing dependencies on Debian and Ubuntu
```
sudo apt-get install cmake g++-multilib libsdl2-mixer-dev:i386 libsdl2-ttf-dev:i386 libsodium-dev libsodium-dev:i386
```
### Installing dependencies on Fedora
```
sudo dnf install cmake glibc-devel.i686 SDL2-devel.i686 SDL2_ttf-devel.i686 SDL2_mixer-devel.i686 libsodium-devel.i686 libasan.i686
```
### Compiling
```
mkdir build
cd build
linux32 cmake -DCMAKE_TOOLCHAIN_FILE=../CMake/32bit.cmake ..
linux32 make -j$(nproc)
```
</details>
<details><summary>macOS</summary>

### Installing dependencies
Install [Xcode 9.4.1 and Xcode Command Line tools](https://developer.apple.com/download/more/?=xcode%209.4.1), this is the last version with **32 bits** support.

Note: Be sure that your to select the command line Xcode if you have more then one installed:
```
$ sudo xcode-select --switch /Applications/Xcode.app
```

Verify that you don't have FreeType and HarfBuzz installed from
[Homebrew](https://brew.sh/), which will cause build errors for SDL2_ttf:

```
brew info freetype
brew info harfbuzz
```

If you do have those installed, you'll have to temporarily uninstall them:

```
brew uninstall --ignore-dependencies freetype
brew uninstall --ignore-dependencies harfbuzz
```

You can install FreeType and HarfBuzz from Homebrew again after devilutionX builds successfully.

Now you can run the build script:
```
./macos-build.sh --build-all-x86
```
</details>
<details><summary>Windows via MinGW</summary>

### Installing dependencies on Debian and Ubuntu

Download and place the 32bit MinGW Development Libraries of [SDL2](https://www.libsdl.org/download-2.0.php), [SDL2_mixer](https://www.libsdl.org/projects/SDL_mixer/), [SDL2_ttf](https://www.libsdl.org/projects/SDL_ttf/) and [Libsodium](https://github.com/jedisct1/libsodium/releases) in `/user/i686-w64-mingw32`.

```
sudo apt-get install cmake gcc-mingw-w64-i686 g++-mingw-w64-i686
```
### Compiling
```
mkdir build
cd build
cmake -DASAN=OFF -DCMAKE_TOOLCHAIN_FILE=../CMake/mingwcc.cmake ..
make -j$(nproc)
```
</details>
<details><summary>Windows via Visual Studio</summary>

### Installing dependencies
Make sure to install the `C++ CMake tools for Windows` component for Visual Studio.

* **Using vcpkg (recommended)**
1. Install vcpkg following the instructions from https://github.com/microsoft/vcpkg#quick-start.

	Don't forget to perform _user-wide integration_ step for additional convenience.
2. Install required dependencies by executing the following command (via cmd or powershell):

	`vcpkg install sdl2:x86-windows sdl2-mixer:x86-windows sdl2-ttf:x86-windows libsodium:x86-windows`

	_Note: this command installs libraries compiled for x86 but it's not hard to do the same for x64 libraries if you need it_

* **Manually**
1. Download and place the 32bit MSVC Development Libraries of [SDL2](https://www.libsdl.org/download-2.0.php), [SDL2_mixer](https://www.libsdl.org/projects/SDL_mixer/), [SDL2_ttf](https://www.libsdl.org/projects/SDL_ttf/) and [Libsodium](https://github.com/jedisct1/libsodium/releases) in `%USERPROFILE%\AppData\Local\Microsoft\WindowsApps\`.
2. If dependencies are not found or you wish to place them in other location - configure required path variables in _"Manage Configurations..."_ dialog inside Visual Studio or in _cmake-gui_.

### Compiling

* **Through Open->CMake in Visual Studio**
1. Go to `File -> Open -> CMake`, select `CMakeLists.txt` from the project root.
2. Select `Build devilution.exe` from the `Build` menu.

	_Note: By default Visual Studio only creates configuration x64-Debug, to add the new configuration (e.g. x86-Debug) please click "Manage Configurations..." in combo box on the top and then on button wih green plus to add a new configuration_

* **Through cmake-gui**

1. Input the path to devilutionx source directory at `Where is the source code:` field.
2. Input the path where the binaries would be placed at `Where to build the binaries:` field. If you want to place them inside source directory it's preferable to do so inside directory called `build` to avoid the binaries being added to the source tree.
3. It's recommended to input `Win32` in `Optional Platform for Generator`, otherwise it will default to x64 build.
4. In case you're using `vcpkg` select `Specify toolchain file for cross-compiling` and select the file `scripts/buildsystems/vcpkg.cmake` from `vcpkg` directory otherwise just go with `Use default native compilers`.
5. In case you need to select any paths to dependencies manually do this right in cmake-gui window.
6. Press `Generate` and open produced `.sln` file using Visual Studio.
7. Use build/debug etc. commands inside Visual Studio Solution like with any normal Visual Studio project.
</details>

## Building for the native platform
*Note: Since 64-bit builds are currently not in a playable state, it is advised to build in a 32-bit environment. Another possibility is a 32-bit build on a 64-bit system (see above).*
<details><summary>Linux</summary>

### Installing dependencies on Debian and Ubuntu
```
sudo apt-get install cmake g++ libsdl2-mixer-dev libsdl2-ttf-dev libsodium-dev
```
### Installing dependencies on Fedora
```
sudo dnf install cmake glibc-devel SDL2-devel SDL2_ttf-devel SDL2_mixer-devel libsodium-devel libasan
```
### Compiling
```
mkdir build
cd build
cmake ..
make -j$(nproc)
```
</details>
<details><summary>macOS</summary>

Make sure you have [Homebrew](https://brew.sh/) installed, then run:

```
brew bundle
mkdir build
cd build
cmake ..
make -j$(sysctl -n hw.physicalcpu)
```
</details>
<details><summary>FreeBSD</summary>
*Note: At the moment this only appears to work from a 32bit system.*

### Installing dependencies
```
pkg install cmake gcc8 sdl2_mixer sdl2_ttf libsodium
```
### Compiling
```
mkdir build
cd build
cmake -DCMAKE_C_COMPILER=/usr/local/bin/gcc8 -DCMAKE_CXX_COMPILER=/usr/local/bin/g++8 ..
make -j$(sysctl -n hw.ncpu)
```
</details>
<details><summary>Haiku</summary>

### Installing dependencies on 32 bit Haiku
```
pkgman install cmake_x86 devel:libsdl2_x86 devel:libsdl2_mixer_x86 devel:libsdl2_ttf_x86 devel:libsodium_x86
```
### Installing dependencies on 64 bit Haiku
```
pkgman install cmake devel:libsdl2 devel:libsdl2_mixer devel:libsdl2_ttf devel:libsodium
```
### Compiling on 32 bit Haiku
```
mkdir build
cd build
cmake -DCMAKE_C_COMPILER=gcc-x86 -DCMAKE_CXX_COMPILER=g++-x86 -DBINARY_RELEASE=ON ..
make -j$(nproc)
```
### Compiling on 64 bit Haiku
```
mkdir build
cd build
cmake ..
make -j$(nproc)
```
</details>

## CMake arguments
### General
The default build type is `Debug`. This can be changed with `-DBINARY_RELEASE=ON`. Independently of this, the debug mode of the Diablo engine is always enabled by default. It can be disabled with `-DDEBUG=OFF`. Finally, in debug builds the address sanitizer is enabled by default. This can be disabled with `-DASAN=OFF`.
### mingw32
Use `-DCROSS_PREFIX=/path/to/prefix` if the `i686-w64-mingw32` directory is not in `/usr`.


# Multiplayer
 - TCP/IP only requires the host to expose port 6112
 - UDP/IP requires that all players expose port 6112

All games are encrypted and password protected.

# Contributing
[Guidelines](docs/CONTRIBUTING.md)

# Modding
Below are a few examples of some simple improvements made to the game. It is planned in the future to create tools for designing dungeons and graphics.

![Screenshot 1: Monster lifebar+items](https://github.com/diasurgical/scalpel/blob/master/screens/mod1.png)

![Screenshot 2: New trade screen](https://github.com/diasurgical/scalpel/blob/master/screens/mod2.png)

# F.A.Q.
<details><summary>Click to reveal</summary>

> Wow, does this mean I can download and play Diablo for free now?

No, you'll need access to the data from the original game. If you don't have an original CD then you can [buy Diablo from GoG.com](https://www.gog.com/game/diablo).
> Cool, so I fired your mod up, but there's no 1080p or new features?

We're working on it.
> After looking at the code some of it is a disaster

That is the result of decompiled code. Whenever a program is compiled, much of the source is optimized and stripped away, so it's nearly impossible to decompile it back. Have patience. Everything will be cleaned up eventually. :)
> What about Hellfire?

Hellfire was a bit of a flop on the developer's part. Support may come in the future once the base game is finished.
</details>

# Credits
- Reverse engineered by GalaXyHaXz in 2018
- [sanctuary](https://github.com/sanctuary) - extensively documenting Diablo's game engine
- [BWAPI Team](https://github.com/bwapi) - providing library API to work with Storm
- [Ladislav Zezula](https://github.com/ladislav-zezula) - reversing PKWARE library, further documenting Storm
- [fearedbliss](https://github.com/fearedbliss) - being awe-inspiring
- Climax Studios & Sony - secretly helping with their undercover QA :P
- Blizzard North - wait, this was a typo!
- Depression - reason to waste four months of my life doing this ;)

And a special thanks to all the support and people who work on Devilution to make it possible! <3

# Legal
Devilution is released to the Public Domain. The documentation and function provided by Devilution may only be utilized with assets provided by ownership of Diablo.

Battle.net(R) - Copyright (C) 1996 Blizzard Entertainment, Inc. All rights reserved. Battle.net and Blizzard Entertainment are trademarks or registered trademarks of Blizzard Entertainment, Inc. in the U.S. and/or other countries.

Diablo(R) - Copyright (C) 1996 Blizzard Entertainment, Inc. All rights reserved. Diablo and Blizzard Entertainment are trademarks or registered trademarks of Blizzard Entertainment, Inc. in the U.S. and/or other countries.

Devilution and any of its' maintainers are in no way associated with or endorsed by Blizzard Entertainment(R).
