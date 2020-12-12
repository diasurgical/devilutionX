# devilutionX PS Vita port

## How To Play:
 - Install VPK
 - Copy diabdat.mpq from your CD or GoG installation (or [extract it from the GoG installer](https://github.com/diasurgical/devilutionX/wiki/Extracting-the-DIABDAT.MPQ-from-the-GoG-installer)) to the `ux0:/data/diasurgical/devilutionX/`; make sure it is all lowercase.

# Building from Source

```
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DVITA=1 ..
make
```

# Multiplayer
 - Not supported yet

# Controls

- Left analog or D-Pad: move hero
- ○: attack nearby enemies, talk to townspeople and merchants, pickup/place items in the inventory, OK while in main menu
- ×: select spell, back while in menus
- △: pickup items, open nearby chests and doors, use item in the inventory
- □: cast spell, delete character while in main menu
- L1: use health item from belt
- R1: use mana potion from belt
- Start + L1: character sheet (alt: Start + ←)
- Start + L2: inventory (alt: Start + →)
- Start + ↓: toggle automap
- Start + Select: game menu (alt: Start + ↑)
- Select + ×/○/□/△: Spell hotkeys
- Right analog: move automap or simulate mouse
- Select + L1: left mouse click
- Select + R1: right mouse click
- Start + □: quest log
- Start + △: spell book
