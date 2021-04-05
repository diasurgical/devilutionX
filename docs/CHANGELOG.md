# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## DevilutionX 1.2.0 (unreleased)
### Features
#### Gameplay
- Hellfire support
- Shareware support
- Fully migrate saves between Diablo and Hellfire
- Gold picked goes to stack even when inventory is open
- Pepin automatically heals
- Show other players on the map
- Preserve list position when buying and selling items in stores
- Preserve hotkeys and active spell across games
- Set default active spell for new heroes
#### Controls
- Use belt via the numpad
- Close panels when pressing <kbd>ESC</kbd>
- <kbd>Shift-click</kbd> to spend all stat points
- <kbd>Shift-click</kbd> to clear readied spell
- <kbd>Shift-click</kbd> consumables to move them between the belt and inventory
- <kbd>Shift-click</kbd> equipment equip/unequip them
- <kbd>D-Pad</kbd> / Left stick navigation now works in all menus and is more responsive.
- <kbd>START</kbd> + <kbd>SELECT</kbd> now opens the main menu.
- Right stick mouse emulation has been improved.
- Gamepads plugged in while the game is running can be used.
- All connected gamepads can be used (previously, only the first one).
#### Graphics / Audio
- Widescreen menus
- Scroll subtitles at the same speed as narration reader
- Apply sound volume to videos
#### Stability / Performance / System
- Document [ini-file options](https://github.com/diasurgical/devilutionX/wiki/DevilutionX-diablo.ini-configuration-guide)
- Reorganize ini-file
- Allow for up to 99 save games of each type
- Much lower memory usage (now in line with the original)
- Performance improvements
- DIABDAT.MPQ may now be upper case
- Support for portable installs
- Support file paths longer than 259 characters
- Add `--ttf-dir` and `--ttf-name` to allow for specifying a different UI font
- Switch Windows releases to 64-bit
- Added support for PlayStation Vita
- Added support for Nintendo 3DS
- Added support for DragonFly BSD
- Added AppImage for Linux

### Optional Features
#### Gameplay
- Experience bar
- Monster health bar
- Auto equip all item types on pickup or purchase
- Auto pickup gold
- Run in town (also in multiplayer)
- Disable friendly fire
- Disable quest randomization
- Adria refills mana
- Barbarian and Bard heroes in Diablo
#### Controls
- Controller mapping
#### Graphics / Audio
- Improved transparency
- Auto-equip sounds
- Show monster type
- Disable walking sounds
#### Stability / Performance / System
- Configurable network port

### Bugfixes
#### Gameplay
- Catacombs doors never having traps
#### Graphics / Audio
- Incorrect graphics under cave doors in multiplayer
- Minor color issues in some videos
- Command-line output missing on Windows
- Correct visuals and function of the joining-game-dialog
#### Controls
- Mouse jumping to the top of the windows when using the menu
- Mouse not releasing when exiting the window
- Dragging on touch devices
#### Stability / Performance / System
- Multiplayer host crashing after beating the game

### Bugfixes for original Diablo bugs
#### Gameplay
- Base damage not being updated when leveling up
- Player vision radius not updating properly before changing level
- Tiles not properly being marked as out of visibility when walking
- Scavengers/Gravediggers taking damage when eating on higher difficulties
- Flash only dealing 10% damage in some directions
- Not getting XP for the first monster placed on a level
- Not getting XP from monsters hit by golems
- Monsters going inactive when fighting golems offscreen
- Monsters with hiding ability fleeing in the wrong direction from golems
- Fallen fleeing in the wrong direction
- Monsters charge directions being biased for some angles
- Monsters not being able to properly judge the location of firewalls
- Some unique items morphing on a new game
- Incorrectly seeing legit items as duplicates
- Items being destroyed if held in cursor when entering dungeon
- Being able to waste gold on red scrolls when not having room for them
- Gaining mana from reading books despite wearing an item with corruption
- Gold piles will be filled to their max, before starting a new one
- Inconsistent repair prices caused by rounding errors
- Arkaine's Valor brought from previous games breaking the game state
- Several issues causing Arkaine's Valor to enter a broken state
- Black Mushroom quest not resetting between games
- Randomly being teleported from level 15 to Lazarus' chamber
- Several bugs in Lazarus quest when re-entering the teleport
- Town portals appearing on two levels when cast in a quest level
#### Controls
- Inconsistent mouse behavior when opening/closing various panels
#### Graphics / Audio
- Gold icons graphics are always correct
- Arrows graphics not aligning with the move direction
- Wrong death sound when monster/golem kills another monster
- Lights not always following unique monsters
- Lights added on map update not working (Arkaine's Valor, etc.)
- Player light radius not readjusting when a player is knocked back
- Loadscreen colors on entering the Poison Water quest
- Lava pools sometimes having roof tiles in them
- Doors sometimes missing in Halls of The Blind
- Broken outline on some inventory items
#### Stability / Performance / System
- Fix more stability issues
#### Bugfixes from Hellfire 1.00 - 1.01
- Monster hitpoints overflowing in Nightmare/Hell difficulty
- Gharbad the Weak not dropping his item when killed by a golem
- Some stability issues

### Bugfixes for original Hellfire bugs
#### Gameplay
- Firewall damage calculation being too low
- Firewall/Lightningwall damage being based on the dungion level instead of spell level for the first player
- Right-hand item's decay suffix being applied to the left-hand item in multiplayer
- Town Shrine would always cast TP from the first player instead of the user
- Oily Shrine and Sparkling Shrine would cast their traps as if the first player was doing it
- The farmer would always think he was talking to the local player
- Scavenger/Gravedigger AI being stuck for a round after eating
- Sync available quests in multiplayer
- Correct spell level being ignored for Search and Rage
#### Graphics / Audio
- Monster attack sound sometimes not playing
- Missing sounds when bards and barbarians talk to cows
- "0 Gold" sometimes spawning on The Cornerstone
- Minor typography corrections
#### Bugfixes from Diablo 1.03 - 1.09
- The Hidden Shrine freezes when not holding an applicable item
- Memory corruption when casting town portal
- Crashes associated with Black Death
- Players getting stuck in a wall when using Town Portals
- Some Hell difficulty monsters being too easy to hit
- Mana Shield bugs that could make a player invulnerable and/or invisible
- Disarming skill not working on levels 13 through 15
- Mana Shield permanetly using up level missiles
- Corrected behaviour of Thieves, Speed/Haste, Balance/Stability/Harmony, and Piercing/Bashing/Puncturing
- Mana Shield becomming less effective with increasing spell level
- Excessive damage when a Mana Shield expired due to an attack
- Bows with fire damage prevented the Gargoyles from healing
- Right-clicking belt item 5-8 not working while the Spellbook is open
- Mana Shield and Nova not appearing for other players
- Shields disappearing when worn after wielding two-handed weapons
- Adria's books appear white even when the player can't read them
- Equipped shields not appear properly to other players
- Monster health not appearing correctly on Nightmare and Hell difficulty
- Diablo's scream being muted if you load a save game during his death throes
- Multiplayer character files are now located in the data directory, and may be moved between computers
- Some stability issues

## DevilutionX 1.1.0
### Features
- Proper widescreen (and other aspects) support
- Select difficulty in single player (remembered in the save game)
- Adjustable game speed in-game
- V-sync can now be disabled in the ini
- Better line drawing function used for the automap
- Support for loading custom Hell level maps
- Make all objects and monsters available to custom maps
- Added support for joining a game via hostname instead of IP

### Bugfixes
- You can now enter and exit fullscreen also when the game is paused
- [Amiga] Fix double-clicking in menu

### Original Diablo bugs
- Fix some monsters AC and to-hit values on Nightmare/Hell ending up as 0
- Fix player base block resetting to 0 after loading a save game
- Correctly render trees in front of the player
- Fully render sprites that are larger than the floor tiles
- Fix objects disappearing when walking on the opposite wall
- Duping via belt has been fixed
- Correctly detect if the mouse is clicking the world or UI in some areas
- Minor typography corrections
- Corrected name of Chamber of Bone in automap
- Correctly align the inventory slots

## DevilutionX 1.0.3
### Bugfixes
- Fix keyboard input in the menu

## DevilutionX 1.0.2
### Features
- Support for integer scaling
- Controller: Map Start + L1/R1 to char/inventory
- Added `--save-dir` to allow for specifying the save game folder
- Correctly scale the game on systems with a non-standard DPI setting

### Bugfixes
- Some cave levels were generated with an incorrect layout
- Firebolt mana cost is now correct
- Allow players to join Nightmare/Hell games without first creating a game
- Correctly load the full white color
- Correct navigation in the difficulty select screen
- The window can now be resized after exiting fullscreen mode
- Fix memory leak in audio code
- Fix hanging for 20 sec on the error screen

## DevilutionX 1.0.1
### Features
- Toggle fullscreen at any point by hitting alt+enter (mouse grab can be disabled in diablo.ini)
- Controller: B button closes the currently active panel.
- Added a navigation menu for controllers
- Focus on the exit item before quitting.
- Minor performance improvements (save/load time and main menu).
- Add a list of [known mods](https://github.com/diasurgical/devilutionX/blob/master/docs/mods.md)

### Platforms
- Added support for 32bit PowerPc
- Added support for Amiga
- Added support Clockwork PI GameShell
- Added support for GKD350h
- Switch to OPK for RetroFW (requires 2.0)
- Update OpendDingus/RetroFW build root

### Bugfixes
- Game crashing on systems that do not report monitor refresh rate.
- Fixed belt not working correctly when holding shift on us keyboard layouts
- Always keep track of the latest hero level when selecting difficulty.
- Correct screenshot path
- Turn the screen red for 200ms when taking screenshots.
- Fix town load screen missing on the first load.
- Fix minor HOM issue at some houses in town.
- Allow for space as text input
- Fixed a couple of typos in error messages

### Original Diablo bugs
- Fix belt not working correctly when holding shift on *non-*us keyboard layouts
- Fix one more rare crash.

## DevilutionX 1.0.0
### Features
- FPS no longer capped at 20hz; smoother mouse and transitions
- Full-featured gamepad support
- All movies can now be skipped via ESC
- Implement `--help`, `--version` and `--data-dir`
- Implement FPS counter (`-f`)
- Force windowed mode with `-x`
- Skip startup movies with `-n`
- Implement scroll wheel navigation
- Implement touch support
- Load assets from the same folder as the mpq
- Faster loading

### Platforms
- Added Nintendo Switch support
- Added Retro Gamer Handheld (OpenDingux/Retrofw) support
- Added OpenBSD support
- Windows version is now build as GUI an app and has an icon

### Bugfixes
- Fixed random missing sounds
- Fixed NPCs not talking about the right quest
- Error messages implemented in GUI
- Fixed having to restart the game between network sessions
- Fixed game not working on some Radeon GPUs
- Fixed panel missing after minimizing game if upscaling is disabled
- Fixed clicks in the left letterbox being incorrectly handled
- Fixed end movie not looping

### Original Diablo bugs
- Fixed a large number of rare stability issues from the original game
- Correctly take open panels into account when casting spells
- Fix sound some times being muted after Diablo dies

## DevilutionX 0.5.0
### Features
- Sound is now accurate to the original
- All in-game issues fixed
- Delete hero, inline dialogs and scrollbars are now implemented
- Screenshots now have different names
- Multiple simultaneous dialogs fixed
- All builds are now 64bit (except for Windows and Raspberry Pi)
- Memory leaks and crashes fixed
- All keys are now mapped
- UI text now has correct shadows
- Much lower CPU usage
- diabdat.mpq can now be loaded with read-only access

### Known issues
- Error dialogs not implemented in main UI
- The game must restart after hosting multiplayer

## [0.10.0](https://github.com/diasurgical/devilution/compare/0.9.6...0.10.0)
### June 22, 2019
- [All functions are now binary identical](https://github.com/diasurgical/devilution/milestone/3) to the 1.09b version
- Fix buying from Wirt
- Replace many magic numbers with constants
- Fix a handful of minor issues
- Add toggle fullscreen with alt+enter in debug builds

### June 21, 2019
- All functions are now [binary identical](https://github.com/diasurgical/devilution/milestone/3) to Diablo 1.09b

### June 1, 2019
- MVG posts [a video about the project](https://www.youtube.com/watch?v=5tADL_fmsHQ) and releases a Nintendo Switch port

## DevilutionX 0.4.0
### May 20, 2019
- Fixed flickering mouse in caves
- 32bit ARM build (Raspberry Pi)
- 32bit FreeBSD build
- 32bit Haiku support (see HaikuDepot)
- Included font for rendering credits and progress screen
- Upscaling quality can be adjusted or fully disabled in diablo.ini
- Windowed mode can be set in diablo.ini
- Mouse capture can be set in diablo.ini
- Direct file access implemented (mods won't need to pack their files in an MPQ)
- Music and speech memory leaks fixed

## [0.9.6](https://github.com/diasurgical/devilution/compare/0.9.0...0.9.6)
### May 19, 2019
- [96% of functions are now binary identical](https://github.com/diasurgical/devilution/milestone/3) to the 1.09b version
- Fix several item corruption issues introduced in 0.9.0

## [0.9.0](https://github.com/diasurgical/devilution/compare/0.8.0...0.9.0)
### May 2, 2019
- [90% of functions are now binary identical](https://github.com/diasurgical/devilution/milestone/3) to the 1.09b version
- Mute buttons now work correctly

### April 15, 2019
- Code is once again compiled as C++ as some parts appear to require despite the indications in Rich header

## [0.8.0](https://github.com/diasurgical/devilution/compare/0.7.0...0.8.0)
### April 12, 2019
- [80% of functions are now binary identical](https://github.com/diasurgical/devilution/milestone/3) to the 1.09b version
- Fixes a few minor issues with generated items

## [0.7.0](https://github.com/diasurgical/devilution/compare/0.6.0...0.7.0)
### April 9, 2019
- [70% of functions are now binary identical](https://github.com/diasurgical/devilution/milestone/3) to the 1.09b version

### April 9, 2019
- The last of the compiler flags are figured out

### March 22, 2019
- Devilution appears on [Phoronix](https://www.phoronix.com/scan.php?page=news_item&px=DeviluitionX-Open-Diablo)

## DevilutionX 0.3.0
### March 20, 2019
- Fix dialog volume
- 32bit macOS build
- Fix crash in town during multiplayer
- Screenshot implemented

## [0.6.0](https://github.com/diasurgical/devilution/compare/v0.5.0...0.6.0)
### March 19, 2019
- [60% of functions are now binary identical](https://github.com/diasurgical/devilution/milestone/3) to the 1.09b version
- Added a guide for people wanting to join in [Cleaning the code](https://github.com/diasurgical/devilution/wiki/Cleaning-Code)
- File size is now only 968 bytes (0.13%) larger than the original Diablo 1.09b.

## DevilutionX 0.2.0
### March 17, 2019
- Fully implemented audio (all issues from 0.1.0 fixed)
- Fully implemented multiplayer
- Windows 32bit build
- Gameplay is fully featured and plays like the original
- Memory leaks fixed
- Most known crashes have been fixed

### March 7, 2019
- [GOG re-release Diablo](https://www.gog.com/news/release_diablo)

## DevilutionX 0.1.0
### February 27, 2019
- Linux 32bit build
- Basic audio
- Video playback
- Basic menus
- Graphics
- Gameplay
- Basic multiplayer over TCP/UDP
- Persistent settings
- Encrypted network connection with password protection

## [0.5.0](https://github.com/diasurgical/devilution/compare/0.4...v0.5.0)
### January 14, 2019
- [50% of functions are now binary identical](https://github.com/diasurgical/devilution/milestone/3) to the 1.09b version
- [#456](https://github.com/diasurgical/devilution/pull/456) Assets can now be loaded directly form disk (no need for MPQ-files when modding)
- [#528](https://github.com/diasurgical/devilution/pull/528) Code ported to C (can still be compiled as C++)
- [#111](https://github.com/diasurgical/devilution/pull/111) Rich Header no longer contains incorrect sections
- [#182](https://github.com/diasurgical/devilution/pull/182) defined a [Code Style](https://github.com/diasurgical/devilution/wiki/Code-style-guide) with accompanying clang-format definition
- `Diabloui.dll` is now also part of the source tree
- Added [Contribution Guide](https://github.com/diasurgical/devilution/blob/master/docs/CONTRIBUTING.md)
- Added PDB build option for comparing with [devilution-comparer](https://github.com/diasurgical/devilution-comparer)
- CI now runs the original build chain
- Most magic numbers are now replaced by enums
- The code was reduced by 10,000 lines
- Fix a few issues, mostly relating to multiplayer

### November 17, 2018
- An older and more original PSX symbol file is discovered

### October 1, 2018
- Compiler version is confirmed to be correct by discovery of the [Rich header](http://bytepointer.com/articles/the_microsoft_rich_header.htm)

### September 18, 2018
- Merge nightly back in to devilution

### September 3, 2018
- Travis is configured to report the overall project delta to 1.09b on every change

### September 1, 2018
- Devilution-comparer is developed for comparing binary diff in compiled functions

### August 28, 2018
- The correct compiler combination is found by trial and error plus a bit of luck

## [0.4.0](https://github.com/diasurgical/devilution/compare/0.3...0.4)
### September 16, 2018
- Fix crash
- Get the first functions bin exact
- Fix render issues
- Introduce debug functions from the 1.00 debug release
- Fix missiles
- More consts, sizeof and defines
- Fix several issues with dungeon generation code
- Fix multiplayer
- Fix error messages
- Correct names based on PSX symbols and DX SDK

### August 20, 2018
- Created nightly fork where code clean up can take place until major bugs are fixed in the main project

### July 6, 2018
- Setup a Discord channel

### July 1, 2018
- Diablo 1.09b is determined to have been compiled with the /O1 flag

## [0.3.0](https://github.com/diasurgical/devilution/compare/0.1.0...0.3)
### June 28, 2018
- Windows binary can now be compiled under Linux and Mac OS X
- Windows binary can now be compiled under VS 5.10
- Fix multiple crashes
- Remove cheesy copyright notice
- Set up continuous building via Travis and AppVeyor
- Icon added
- Fix Zhar quest, monster squelching and golems
- Use consts for various values

### June 20, 2018
- The [media](https://www.pcgamer.com/a-coder-spent-1200-hours-reverse-engineering-diablos-source-code/) catches wind and [several](https://bloody-disgusting.com/video-games/3505673/fan-completes-reverse-engineering-source-code-diablo/) [articles](https://kotaku.com/coder-spends-1-200-hours-piecing-together-diablos-sourc-1827001247) [appear](https://www.diabloii.net/blog/comments/reverse-engineered-diablo-source-code-released)

### June 18, 2018
- Devilution gets posted on [Y Combinator](https://news.ycombinator.com/item?id=17338886)

## 0.1.0
### June 6, 2018
- Devilution is unleashed upon the world! Version 0.1.0!

### June 3, 2018
- Polishing things up for final release
- Added a cheesy fake copyright notice to dissuade monetary gain
- Properly integrated Storm and DiabloUI into the project

### May 28, 2018
- Fixed bugs with save files
- You can now load Devilution saves in the vanilla game

### May 25, 2018
- Finally! Figured it out and now monsters spawn correctly
- The game can be completed from start to finish with a few tricks

### May 21, 2018
- Took a week break, begin working on monster code again
- Nearly all quests work now
- Fixed a bug with Adria

### May 8, 2018
- Fix bugs with character drawing
- Fix bugs relating to item affix generation
- Towners no longer crash the game

### May 7, 2018
- Port debugging functions from the debug release
- Still can't figure out the zombie problem

### May 5, 2018
- Begin fixing quest code and testing completion

### April 26, 2018
- Zombies are spawning in all dungeon types... sigh

### April 20, 2018
- Split code from IDA's C file into separate CPP files
- All dungeon types can now be entered
- Objects are now mostly working
- Begin uncommenting monster code and fixing them

### April 11, 2018
- Begin fixing up dungeon generation and objects

### April 4, 2018
- Fixed many crashing bugs when in town and dungeon
- Items, missiles, and spells are now drawn

### April 1, 2018
- Finally fixed the render bug, everything draws correctly!
- Character animation now draws correctly
- The cathedral is now mostly working

### March 29, 2018
- Fixed tons of bugs
- You can now walk around in town
- Entering the dungeon almost always crashes

### March 27, 2018
- Uncommented and fixed lots of broken code
- The game screen now appears, although very glitchy

### March 22, 2018
- Control panel and inventory now work almost flawlessly

### March 21, 2018
- Temporarily commented out tons of broken code
- You can now get past the loading screen and into town
- Music also works
- Control panel mostly works but game screen is black

### March 18, 2018
- The title screen now works
- Freezes during the loading screen

### March 16, 2018
- Fixed enough bugs that you can now launch binary
- Crashes during title screen

### March 14, 2018
- Fix remaining errors in code
- Code now compiles and produces a non-working binary

### March 13, 2018 -- *!  SPECIAL DAY  !*
- Dump the database to C code via IDA

### March 8, 2018
- Correct various function signatures
- Correct struct names and types
- Plug in enumerates
- Finish correcting and documenting data sections

### February 26, 2018
- Finish documenting functions
- Begin correcting names to match PSX

### February 18, 2018
- Begin adding enumerates
- Add more minor structs
- Clean up data sections

### February 15, 2018
- Almost finished adding every function
- Begin working on major structs

### February 8, 2018
- Add more functions
- Begin adding data from Sanctuary project

### February 4, 2018
- IDA disassembly begin
- Start adding function names from [Sanctuary project](https://github.com/sanctuary/notes)

### January 15, 2018
- The concept of Devilution is born
- Research into Diablo's code and mechanics
- Research from [Jarulf's guide](http://www.bigd-online.com/JG/JGFrame.html)
