# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

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
