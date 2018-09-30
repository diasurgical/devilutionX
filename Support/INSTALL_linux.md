# Installation

## Dependencies

Ubuntu
```bash
sudo apt install g++-mingw-w64-i686
```

Arch Linux
```bash
pacman -S mingw-w64-gcc mingw-w64-binutils
```

Fedora 28:
```bash
sudo dnf install mingw32-gcc-c++ wine
```

elementary OS:
```bash
sudo apt install mingw-w64 wine
```


## Building

```bash
git clone https://github.com/galaxyhaxz/devilution
cd devilution
cp /path/to/diablo_game_dir/diabloui.dll .
cp /path/to/diablo_game_dir/storm.dll .
make
```

On a 32-bit host, `$ make MINGW32=mingw32` should be used, to specify the 32-bit
toolchain.

## Install

```bash
cp devilution.exe /path/to/diablo_game_dir/
```

## Run

```bash
wine devilution.exe
```
