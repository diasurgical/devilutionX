# devilutionX PS Vita port

## How To Play:
 - Install VPK
 - Copy diabdat.mpq from your CD or GoG installation (or [extract it from the GoG installer](https://github.com/diasurgical/devilutionX/wiki/Extracting-the-.MPQs-from-the-GoG-installer)) to the `ux0:/data/diasurgical/devilution/`.

# Building from Source

```
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=${VITASDK}/share/vita.toolchain.cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

# Multiplayer
 - Supported via direct TCP connections

# Controls

## Default

- Left analog or D-Pad: move hero
- ○: attack nearby enemies, talk to townspeople and merchants, pickup/place items in the inventory, back in menus
- ×: select spell, OK while in menus
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

There's special section `Controller` in diablo.ini file, that allows you to adjust controls:
```
[Controller]
Mapping=
Enable Rear Touchpad=1
```

- **Mapping:** allows you to somewhat remap controls. It uses https://github.com/gabomdq/SDL_GameControllerDB syntax. Deprecated, use padmapper menu in-game if you can.
- **Enable Rear Touchpad:** enable/disable back touch mapping to L2/R2
