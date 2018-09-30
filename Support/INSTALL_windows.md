# Installation

## Dependencies and Initial Environment Configuration

* Install [MSYS2](https://www.msys2.org/)

```bash
# Start the *MSYS2 MinGW 32-bit* terminal.

# If this is the first time, go ahead and update all of your components, and
# follow and instructions about restarting the terminal and running the update again:
pacman -Syu

# After everything is updated, let's download all of the components needed
# to build it and set up any dependency symlinks:
pacman -Sy git make mingw-w64-i686-gcc mingw-w64-i686-binutils

ln -s /mingw32/i686-w64-mingw32/bin/dlltool.exe /usr/bin/i686-w64-mingw32-dlltool.exe
ln -s /mingw32/i686-w64-mingw32/bin/as.exe /usr/bin/i686-w64-mingw32-as.exe
ln -s /mingw32/bin/windres.exe /usr/bin/i686-w64-mingw32-windres.exe
```

## Building

```bash
git clone https://github.com/galaxyhaxz/devilution
cd devilution
cp /path/to/diablo_game_dir/diabloui.dll .
cp /path/to/diablo_game_dir/storm.dll .

# If you only have a single core machine or if building in parallel
# causes issues, simply run the following command:
make

# If you want to compile faster, then use the following command (Substitute # for
# your number of processors + 1. So if you have an 8 core processor, use 9:
make -j#
```

## Install

```bash
# The Devilution executable will be placed in the root of your Devilution repository.
# Simply copy this over to your Diablo installation folder:
cp devilution.exe /path/to/diablo_game_dir/
```