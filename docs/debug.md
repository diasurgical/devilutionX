There are debug features available through both in-game and through the command-line. These have been ported from the 12-21-96 debug build. Note that not all of them are available yet.

Command-line parameters
- `-^` : enable god mode and debug tools
- `-$` : enable god mode with less stuff (further documenting needed) [NOT YET IMPLEMENTED]
- `-b` : enable item drop log [NOT YET IMPLEMENTED]
- `-d` : disable startup video + increased item drops [PARTIALLY IMPLEMENTED]
- `-f` : display frames per second
- `-i` : disable network timeout
- `-n` : disable startup video
- `-s` : unused
- `-v` : draw yellow debug tiles
- `-w` : enable cheats
- `-x` : disable exclusive DirectDraw access [NOT YET IMPLEMENTED]
- `-j <##>` : init trigger at level [NOT YET IMPLEMENTED]
- `-l <#> <##>` : start in level as type
- `-m <###>` : add debug monster, up to 10 allowed
- `-q <#>` : force a certain quest
- `-r <##########>` : set map seed to
- `-t <##>` : sets current quest level

In-game hotkeys
- `?` -> enter quest text mode [NOT YET IMPLEMENTED]
  - `-`/`_` -> decrease message number/speed
  - `+`/`=` -> increase message number/speed
  - `Enter` -> play selected message
  - `Esc` -> stop quest text mode
- `Shift` -> while holding, use the mouse to scroll screen
- `F2` -> display dungeon information [NOT YET IMPLEMENTED]
- `F3` -> display number of items on the ground/cursor item
- `F4` -> display quest status information
- `0`/`)` -> cycle between regular/magic arrows
- `8`/`*` -> level up character
- `~` -> refresh vendor items (Griswold premium and Adria)
- `]` -> all spells level 10
- `:` -> all spells preset level
- `[` -> delete all gold in inventory
- `|` -> fill inventory with gold (5000 piece piles)
- `.` -> display dungeon Y/sum [NOT YET IMPLEMENTED]
- `a` -> increase level of the last spell casted and enable `Teleport` in town
- `A` -> display "Mid" monster related
- `d` -> print debug player info
- `D` -> switch current debug player
- `e` -> display "EFlag"
- `l`/`L` -> toggle lighting in dungeon
- `m` -> print debug monster info
- `M` -> switch current debug monster
- `r`/`R` -> display game seeds
- `t`/`T` -> display player and cursor coordinates

Multiplayer hotkeys [NOT YET IMPLEMENTED]
- `Ctrl`+`C` -> trigger breakpoint
- `Ctrl`+`P` -> print mouse clicks and frame counter for each player
- `Ctrl`+`S` -> sleep the network thread
