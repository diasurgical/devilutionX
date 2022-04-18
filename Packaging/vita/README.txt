# devilutionX PS Vita port

## How To Play:
 - Install VPK
 - Copy diabdat.mpq from your CD or GoG installation (or [extract it from the GoG installer](https://github.com/diasurgical/devilutionX/wiki/Extracting-the-.MPQs-from-the-GoG-installer)) to the `ux0:/data/diasurgical/devilution/`.
 - For Chinese, Japanese, and Korean text support download https://github.com/diasurgical/devilutionx-assets/releases/download/v2/fonts.mpq and add it to the game folder.
 - For the Polish voice pack download https://github.com/diasurgical/devilutionx-assets/releases/download/v2/pl.mpq.
 - For the Russian voice pack download https://github.com/diasurgical/devilutionx-assets/releases/download/v2/ru.mpq.

# Building from Source

```
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=${VITASDK}/share/vita.toolchain.cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

# Multiplayer
 - Not supported yet

# Controls

## Default

- Left analog or D-Pad: move hero
- ○: attack nearby enemies, talk to townspeople and merchants, pickup/place items in the inventory, OK while in main menu
- ×: select spell, back while in menus
- △: pickup items, open nearby chests and doors, use item in the inventory
- □: cast spell, delete character while in main menu
- L1: use health item from belt
- R1: use mana potion from belt
- Left back touch panel: character sheet (alt: Start + ←, alt: L2 on ds4)
- Right back touch panel: inventory (alt: Start + →, alt: R2 on ds4)
- Start + ↓: toggle automap
- Start + Select: game menu (alt: Start + ↑)
- Select + ×/○/□/△: Spell hotkeys
- Right analog: move automap or simulate mouse
- Select + L1: left mouse click
- Select + R1: right mouse click
- Start + □: quest log
- Start + △: spell book

## Options

There's special section `controls` in diablo.ini file, that allows you to adjust controls:
```
[controls]
switch_potions_and_clicks=0
dpad_hotkeys=0
enable_second_touchscreen=1
sdl2_controller_mapping=50535669746120436f6e74726f6c6c65,PSVita Controller,y:b0,b:b1,a:b2,x:b3,leftshoulder:b4,rightshoulder:b5,dpdown:b6,dpleft:b7,dpup:b8,dpright:b9,back:b10,start:b11,leftx:a0,lefty:a1,rightx:a2,righty:a3,lefttrigger:a4,righttrigger:a5,leftstick:b14,rightstick:b15,
```

- **dpad_hotkeys:** dpad works as hotkeys without holding Start button
- **switch_potions_and_clicks:** L1/R1 works as left/right mouse clicks by debault, and as health/mana potion while holding Select
- **sdl2_controller_mapping:** allows you to remap controls. It uses https://github.com/gabomdq/SDL_GameControllerDB syntax
- **enable_second_touchscreen:** enable/disable back touch mapping to L2/R2
