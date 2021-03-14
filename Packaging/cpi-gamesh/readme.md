
# devilutionX package for ClockworkPi GameShell
For more information about this device see [here](https://www.clockworkpi.com/gameshell).

## Install devilutionX on the CPi GameShell

To install devilutionX, just copy the [\_\_init__.py](__init__.py) to a newly created folder under /home/cpi/apps/Menu and run it from the menu. The folder then symbolizes the devilutionX icon.
From this menu, you can press 'X' to clone the git repository for devilutionX and compile the code. Dependencies are installed automatically (cmake and SDL development packages).
Once installed, 'X' pulls the updated code and does the compiling. Note that any changes made locally to the source are reverted before pulling.

When the compile is finished and the diabdat.mpq is in place at '/home/cpi/.local/share/diasurgical/devilution/', you can play the game. Enjoy!

## Play devilutionX on the CPi GameShell

When the game is compiled and the diabdat.mpq is in place, you can press "A" in the devilutionX menu to play.

The following key mapping is in place.
Unfortunately, the mapping is not trivial as devilutionX and GameShell use their own not(yet) compatible shift mechanism.
The mapping is based on the standard GameShell keyboard layout, devilutionX uses the Nintento mapping.

| GameShell Key                | Keyboard Key            | devilutionX Key            | devilutionX Action |
| ---------------------     | ---------------         | ------------------------- | ------------------ |
| D-Pad                        | Arrows                  | D-Pad                        | move hero |
| Select + D-Pad            | Space + Arrows          | Back + D-Pad                | simulate mouse |
| B                            | K                       | A                            | attack nearby enemies, talk to townspeople and merchants, pickup/place items in the inventory, OK while in main menu |
| A                            | J                       | B                            | select spell, back while in menus |
| Y                            | I                       | X                            | pickup items, open nearby chests and doors, use item in the inventory |
| X                            | U                       | Y                            | cast spell, delete character while in main menu |
| Select                    | Space                   | Select                    | Select |
| Start                        | Enter                   | Start                        | game menu, skip movie |
| LK1                        | H                       | L1                        | use health item from belt  |
| Start + Left        | Enter + Left      | Start + Left            | character sheet |
| Start + Right        | Enter + Right     | Start + Right        | inventory |
| LK5                        | L                       | R1                        | use mana item from belt |
| Start + Down                | Enter + Down              | Start + Down                | automap |
| Select + LK1                | Space + H                  | Select + L1                | left mouse click |
| Start + X    | Enter + U    | Start + Y    | quest log |
| Start + A    | Enter + J    | Start + B    | spell book |
| Select + LK5                | Space + L                  | Select + R1                | right mouse click |
| Select + A/B/X/Y            | Space + J/K/U/I         | Select + A/B/X/Y            | hot spell |
