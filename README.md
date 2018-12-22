[![Build Status](https://travis-ci.org/diasurgical/devilution.svg?branch=master)](https://travis-ci.org/diasurgical/devilution)
[![Build status](https://ci.appveyor.com/api/projects/status/ssk0xjhoka1uu940?svg=true)](https://ci.appveyor.com/project/galaxyhaxz/devilution)

[Discord](https://discord.gg/XEKDxHK)

# Devilution
Diablo devolved - magic behind the 1996 computer game

Reverse engineered by GalaXyHaXz in 2018

# Introduction
While most titles from Blizzard receive years of love and support, Diablo stayed in the shadows. Abandoned in favor of a sequel, it remained full of bugs and unfinished potential. The game was last patched in 2001 before being discontinued altogether, a problem I wanted to fix. I played Diablo extensively as a teenager, but as time passed it became difficult to run the game on newer hardware. The lack of many improvements found in the sequel also kept it from aging well. At first the game appeared to be a lost cause, but thankfully a little oversight in 1997 made it not so.

With Diablo's development team moving on the source code was given to **Synergistic Software** to handle the expansion. Less known however is that it was also given to **Climax Studios** to create a PlayStation port. Now Sony has long been known for letting things slide; _especially_ in Japan. Anything from leaking prototypes to full source code and Diablo was no exception. Symbolic information was accidentally left on the Japanese port. Normally used for debugging, a symbol file contains a map of everything generated during compile time. This includes file names, functions, structures, variables, and more! To top it all off a special build is hidden on the PC release in `DIABDAT.MPQ -> D1221A.MPQ -> DIABLO.EXE`! This build contains debug tools and assert strings further giving away code information.

After months of piecing these mistakes together, Devilution was born. I present to you a reconstructed form of Diablo's original source code! Once more shall the heroes of Sanctuary return to the depths below!

# Purpose
Having the source code makes Diablo much easier to update and maintain. For years mod-makers had to rely on tedious code editing and memory injection. A few even went further and reversed most or all of the game. The problem is that they rarely shared their work. Usually being a one-person job, they move on with their lives due to the amount of time required or lack of interest. This brings us back to square one having to do countless hours of work all over again. Devilution aims to fix this by finally making the source code open to the community.

In order to ensure that everything is preserved, Devilution keeps everything as it was originally designed. This goes as far as bugs and badly written code in the original game. With that it serves as a base for developers to work with making it much easier than before to update, fix, and port the game to other platforms.

As a side goal Devilution tries to document the unused and cut content from the final game. Development of Diablo was rushed near the end--many ideas were scrapped and multiplayer was quickly hacked in. By examining the source, we can see various quirks of planned development.

# Compiling
Diablo was developed on Windows 95 using Visual C++ 4.20 and later 5.10 and 6 for newer patches. Devilution is optimized for the same tools originally used but is also compatible with modern setups.

### Building with Visual C++ 6
- Open the project workspace `Diablo.dsw`, choose `Debug` or `Release`, and then `Build Diablo.exe`.

To build a binary with functions compiled as close as possible to the original, use [Visual C++ 6](https://winworldpc.com/product/visual-c/6x) with Service Pack 5 and the [Processor Pack](https://download.microsoft.com/download/vb60ent/update/6/w9x2kxp/en-us/vcpp5.exe) (**important for proper code genration!**)

If you aim to build a binary as close as possible to the original one you will also need [Visual C++ 5](https://winworldpc.com/product/visual-c/5x) with [Service Pack 3](http://www.mediafire.com/file/jw4j4sd5dnzze4p/VS97SP3.zip), since the original binary as linked with the older linker from that. Sadly, you cannot use the old linker right out of VC6, so you'll need to link manually or via the `MakefileVC` in the project root.

### Building with Visual Studio 2010-2017
- Open the project solution `Diablo.sln`, choose `Debug` or `Release`, and then `Build Solution`.

Make sure to disable Data Execution Prevention. `Storm.dll` uses dynamic compilation to improve rendering performance but fails to mark the resulting memory page as executable, leading to a protection fault when trying to draw.
- Configuration options -> Linker -> Advanced -> Data Execution Prevention (DEP).
- Set this value to: No (/NXCOMPAT: NO).

You will also need the following dependencies installed if you are using Visual Studio 2017.
Make sure to enable these when installing (or modify your installation):
- Requires "Windows 8.1 SDK" (Target Platform)
- Requires "Visual C++ MFC for x86 and x64" (For afxres.h)
- Requires "Windows Universal CRT SDK" (For ctype.h)

### Building XDiablo (For Linux)
Install the following libs on your machine.
-  apt-get install g++-multilib libc6-dev-i386
-  apt-get install libsdl2-dev:i386
-  apt-get install libsdl2-ttf-dev:i386
-  apt-get install libsdl2-image-dev:i386
-  apt-get install libsdl2-mixer-dev:i386
-  apt-get install cmake

Edit File that is linking all the LIBS for CMAKE.
For me it was located in...
/usr/lib/i386-linux-gnu/cmake/SDL2/sdl2-config.cmake

The last line will have something like...
 set(SDL2_LIBRARIES "-L${SDL2_LIBDIR}  -lSDL2
Change it to ...
set(SDL2_LIBRARIES "-L${SDL2_LIBDIR}  -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf ")


Copy the "diablodat.mpq" from your Diablo CD to the Diablo Directory and make sure it is LOWERCASE.
COPY THE *prealpha.mpq* from this directory TO YOUR DIABLO DIRECTORY. 


Keep in mind please that this is still being worked on and is missing parts of UI and SoundEffects are not properly playing now.






### Building with MinGW
- Execute `make MINGW32=1` for **MinGW32** or `make` for **MinGW64**. Optionally add `debug` to build with debug features.

To compile with MinGW64 on different platforms, refer to the respective documentation: [Linux](Support/INSTALL_linux.md) | [Windows](Support/INSTALL_windows.md) | [Mac](Support/INSTALL_mac.md).

[Debug Build Features](Support/debug.md)
| [Compatibility Matrix](Support/compatibility_matrix.md)
| [Troubleshooting](Support/troubleshooting.md)

# Installing
Once compiled, the Devilution binary will serve as a replacement for `Diablo.exe`. The following files from the original game patched to 1.09(b) need to be present: `DIABDAT.MPQ`, `DiabloUI.dll`, `SmackW32.dll`, `Standard.snp`, and `Storm.dll`. If `COPYPROT` was defined when compiling, the Diablo CD will also be required.

Additionally, Strange Bytes' [DirectDraw patch](http://www.strangebytes.com/index.php/projects/1-diablo-1-windows-7-vista-patch) is recommended to help fix compatibility issues and run the game in windowed mode.

# Multiplayer
TODO

# Contributing
[Guidelines](Support/CONTRIBUTING.md)

# Modding
Here are some screenshots of a few things I tinkered around with, to demonstrate the relative ease of improving the game:

![Screenshot 1: Monster lifebar+items](https://s33.postimg.cc/6xnnhhlmn/diabuimon.png "Monster lifebar+items")

![Screenshot 2: New trade screen](https://s22.postimg.cc/5i5k91vht/diabstore.png "New trade screen, items that couldn't spawn")

# F.A.Q.
> Wow, does this mean I can download and play Diablo for free now?

No, you'll need access to the data from the original game. Blizzard has discontinued Diablo, but there's plenty of used copies floating around. (I'm still using an original 1996-disc in 2018 without problems)
> Cool, so I fired your mod up, but there's no 1080p or new features?

Devilution aims to keep the original code unaltered, for documentation purposes.
> So will you ever add cross-platform support or new features in the future?

Yes! However, this will be a **_side project_** based on Devilution. I have yet to announce the project.
> When and what can I expect from the upcoming project?

Honestly I have no idea. More than 1,200 hours went into creating Devilution, and I have other things going on right now. Maybe in 6-12 months? The goal is to create a native Linux port, convert to OpenGL, modernize the UI, etc. you get the drill. There has to be some surprises. ;)
> Ok, so I'm playing Devilution now and all the sudden it crashed. NOW WHAT??

Open an issue and provide as much information as possible (OS version, etc.) including any crash logs.
> I thought I'd fix the crash myself, but after looking at the code its a disaster. Do you speak v2-34-v8?

That is the result of decompiled code. Whenever a program is compiled, much of the source is optimized and stripped away, so it's nearly impossible to decompile it back. Have patience. Everything will be cleaned up eventually. :)
> Will you be reverse engineering Diablo II next? Ooooh please!

Absolutely not. Diablo II would require far more work and is still supported by Blizzard. Setting that aside, there are rumors that the game will be remastered which takes the point out of it.
> Are you interested in working for me? I have this game I want you to reverse...

Sorry, but no. This project is time consuming enough as it is, and it's just a hobby.
> I think that's about all, but is Devilution even legal?

That's a tricky question. Under the DMCA, reverse-engineering has exceptions for the purpose of documentation and interoperability. Devilution provides the necessary documentation needed to achieve the latter. However, it falls into an entirely gray area. The real question is whether or not Blizzard deems it necessary to take action.

# Credits
- [sanctuary](https://github.com/sanctuary) - extensively documenting Diablo's game engine
- [BWAPI Team](https://github.com/bwapi) - providing library API to work with Storm
- [Ladislav Zezula](https://github.com/ladislav-zezula) - reversing PKWARE library, further documenting Storm
- [fearedbliss](https://github.com/fearedbliss) - being awe-inspiring
- Climax Studios & Sony - secretly helping with their undercover QA :P
- Blizzard North - wait, this was a typo!
- Depression - reason to waste four months of my life doing this ;)

# Legal
This software is being released to the Public Domain. No assets of Diablo are being provided. You must own a copy of Diablo and have access to the assets beforehand in order to use this software.

Battle.net(R) - Copyright (C) 1996 Blizzard Entertainment, Inc. All rights reserved. Battle.net and Blizzard Entertainment are trademarks or registered trademarks of Blizzard Entertainment, Inc. in the U.S. and/or other countries.

Diablo(R) - Copyright (C) 1996 Blizzard Entertainment, Inc. All rights reserved. Diablo and Blizzard Entertainment are trademarks or registered trademarks of Blizzard Entertainment, Inc. in the U.S. and/or other countries.

This software is in no way associated with or endorsed by Blizzard Entertainment(R).
