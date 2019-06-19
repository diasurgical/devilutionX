# Nintendo Switch Port of DevilutionX (Diablo)

### How To Play:
- Put `devilutionx.nro` in into `/switch/devilutionx`
- Copy `DIABDAT.MPQ` from the original Diablo game disc or GOG version into `/switch/devilutionx`
- Launch `devilutionx.nro` (do not use album to launch, see the note below)
- *Note:* Hold R on any installed game and launch it. Do not use album to launch. If you use album, the homebrew only has very little memory available, and the touch keyboard doesn't work. This is true for all homebrew, not just Diablo-NX.

### Compiling On Linux

- ```install devkitproA64, libzip, libpng, libjpeg, switch-freetype, switch-mesa, switch-glad, switch-glm, switch-sdl2, switch-sdl2_ttf, switch-sdl2_mixer, switch-libvorbis, switch-libmikmod```

- ```make```

### Compiling On Windows

- Install [devkitpro](https://sourceforge.net/projects/devkitpro/)
- Open ```Start Button > DevKitPro > MSys2```
- Type in ```pacman -S switch-freetype switch-mesa switch-glad switch-glm switch-sdl2 switch-sdl2_ttf switch-sdl2_mixer switch-libvorbis switch-libmikmod```
- Type in ```make```

### Compiling On MacOS

- Install [devkitpro](https://devkitpro.org/wiki/Getting_Started#macOS)
- Open Terminal
- Type in ```dkp-pacman -S switch-freetype switch-mesa switch-glad switch-glm switch-sdl2 switch-sdl2_ttf switch-sdl2_mixer switch-libvorbis switch-libmikmod```
- Type in ```make```

- .nro lives in release. Test with an emulator (RyuJinx) or real hardware.

### Credits

- Initial Switch Port by [MVG](https://github.com/lantus) in 2019

And a special thanks to all the support and people who work on Devilution to make it possible! <3
