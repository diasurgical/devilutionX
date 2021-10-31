# DevilutionX (Diablo 1) for Nintendo 3DS

## Installation
 - For Chinese, Japanese, and Korean text support download https://github.com/diasurgical/devilutionx-assets/releases/download/v1/fonts.mpq and add it to the game folder.
 - For the Polish voice pack download https://github.com/diasurgical/devilutionx-assets/releases/download/v1/pl.mpq.

### .3dsx installation

#### Install DevilutionX: Diablo
1. Extract `devilutionx.3dsx` and put it into `sd:/3ds/devilutionx/`.
2. Copy `diabdat.mpq` from your Diablo CD (or GoG install folder) to `sd:/3ds/devilutionx/`.

#### Install DevilutionX: Diablo - Hellfire
3. Copy `hellfire.mpq` `hfmonk.mpq` `hfmusic.mpq` and `hfvoice.mpq` from your Hellfire CD (or GoG install folder) to `sd:/3ds/devilutionx/`.

### .cia installation

#### Install DevilutionX: Diablo
1. Extract `devilutionx.cia` and place it on your SD card.
2. Copy `diabdat.mpq` from your Diablo CD (or GoG install folder) to `sd:/3ds/devilutionx/`.
3. Put the SD card back into the 3DS and install `devilutionx.cia` using a title manager (e.g. [FBI](https://github.com/Steveice10/FBI)).
    1. `devilutionx.cia` can be removed after being installed.

##### Install DevilutionX: Diablo - Hellfire
4. Copy `hellfire.mpq` `hfmonk.mpq` `hfmusic.mpq` and `hfvoice.mpq` from your Hellfire CD (or GoG install folder) to `sd:/3ds/devilutionx/`.

## Usage

When using .3dsx:

- Launch DevilutionX with the [Homebrew Launcher](https://github.com/fincs/new-hbmenu).

or, when using .cia:

- Launch Diablo from your 3DS Homemenu.

## Controls

- Circle-Pad or D-Pad: move hero
- A: attack nearby enemies, talk to townspeople and merchants, pickup/place items in the inventory, OK while in main menu
- B: select spell, back while in menus
- X: pickup items, open nearby chests and doors, use item in the inventory
- Y: cast spell, delete character while in main menu
- L: use health item from belt
- R: use mana potion from belt
- Start + ↑: game menu (alt: Start + Select)
- Start + ↓: toggle automap
- Start + ←: character sheet (alt: ZL or Start + L)
- Start + →: inventory (alt: ZR or Start + R)
- Start + X: toggle zoom
- Select + D-Pad: move automap or simulate mouse
- Select + A/B/X/Y: Spell hotkeys
- Select + ZL: quest log (alt: Start + Y)
- Select + ZR: spell book (alt: Start + B)

## Touchpad

- Single finger drag: move the mouse pointer (pointer jumps to finger)
- Single short tap: left mouse click

## Tips

- For improved performance, change the game's resolution to 640x480. This may be necessary on old 3DS models which may otherwise run slower than the game's intended framerate.
    - Open diablo.ini located in sd:/3ds/devilutionx.
    - Update Graphics settings by changing to `Width=640`.
    - By default, the game will scale to fill the entire top screen. To keep the aspect ratio when scaling, change to `Fit to Screen=0` in Graphics settings as well.

## Resources

* Discord: https://discord.gg/YQKCAYQ
* GitHub: https://github.com/diasurgical/devilutionX
