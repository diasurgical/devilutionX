# Nintendo Switch Port of DevilutionX (Diablo)

### How To Play:
- Extract contents of diablo-nx.zip release into `/switch/diablo-nx`
- Copy `DIABDAT.MPQ` from original Diablo game disc or GOG version into `/switch/diablo-nx`
- Launch `diablo-nx.nro` (do not use album to launch, see the note below)
- *Note:* Hold R on any installed game and launch it. Do not use album to launch. If you use album, the homebrew only has very little memory available, and the touch keyboard doesn't work. This is true for all homebrew, not just Diablo-NX.
- Enjoy :)

### Joycon Controls

- Left analog : move hero
- Right analog : simulate mouse
- B : attack nearby enemies, talk to towns people and merchants, pickup & drop items in inventory, OK in main menu
- Y : pickup gold, potions & equipment from ground, open chests and doors that are nearby, use item when in inventory (useful to read books etc.)
- X : cast spell, go to previous screen when talking to people and in shops
- A : Select spell, cancel while in main menu
- R : inventory
- L : character
- ZR : drink mana potion
- ZL : drink health potion
- Left analog click : quest log
- Right analog click : left mouse click
- Minus : automap
- Plus : game Menu, skip intro

### Touch Controls

- Single finger drag : move the mouse pointer (pointer jumps to finger)
- Single short tap : left mouse click
- Single short tap while holding a second finger down : right mouse click
- Dual finger drag : drag'n'drop (left mouse button is held down)
- Three finger drag : drag'n'drop (right mouse button is held down)

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

- Initial Switch Port by MVG in 2019
- Control improvements and bug fixes by [rsn8887](https://github.com/rsn8887) in 2019
- Controller code by [Jacob Fliss](https://github.com/erfg12)
- RetroArch team for the Switch mman.h file
- AJenbo for upstreaming Switch code and many code fixes
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
