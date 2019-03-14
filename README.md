[![CircleCI](https://circleci.com/gh/diasurgical/devilutionX.svg?style=svg)](https://circleci.com/gh/diasurgical/devilutionX)
[![Build Status](https://travis-ci.org/diasurgical/devilutionX.svg?branch=master)](https://travis-ci.org/diasurgical/devilutionX)
[![Downloads](https://img.shields.io/github/downloads/diasurgical/devilutionX/total.svg)](https://github.com/diasurgical/devilutionX/releases)

[Discord Channel](https://discord.gg/aQBQdDe)

# DevilutionX
Diablo build for modern operating systems

**Note**, DevilutionX requires an original copy of `diabdat.mpq`. None of the Diablo 1 game assets are provided by this project. To get a legitimate copy of the game assets, please refer to the [GoG release of Diablo 1](https://www.gog.com/game/diablo).

# How To Play:
 - Copy diabdat.mpq from your CD, or GoG install folder, to the DevilutionX game directory ; Make sure it is all lowercase.
 - [Download DevilutionX](https://github.com/diasurgical/devilutionX/releases), or build from source
 - Install SDL2 (including SDL2_mixer and SDL2_ttf) make sure to get the appropriate 32bit or 64bit version.
 - Run `./devilutionx`

### Building devilutionX (native Linux build)
Note: Since 64-bit builds are currently not in a playable state, it is advised to build in a 32-bit environment. Another possibility is a 32-bit build on a multilib system (see below).

Install the dependencies on your machine:
```
sudo apt-get install cmake g++ libsdl2-dev libsdl2-mixer-dev libsdl2-ttf-dev libsodium-dev
```

Now run the following commands:
```
mkdir build
cd build
cmake ..
make -j$(nproc)
```

*Arguments to cmake:*

The default build type is `Debug`. This can be changed with `-DCMAKE_BUILD_TYPE=Release`. Independently of this, the debug mode of the Diablo engine is always enabled by default. It can be disabled with `-DDEBUG=OFF`. Finally, in debug builds the address sanitizer is enabled by default. This can be disabled with `-DASAN=OFF`.

*To run the game:*

 - Copy the compiled devilution executable to your Diablo directory.
 - Copy the "diablodat.mpq" from your Diablo CD to the Diablo directory and make sure it is LOWERCASE.
 - Run `./devilution`

Please keep in mind that this is still being worked on and is missing parts of UI and SoundEffects are not properly playing now.

### Building deviltuionX 32-bit on 64-bit Linux (multilib) platforms
```
sudo apt-get install cmake g++-multilib libsdl2-dev:i386 libsdl2-mixer-dev:i386 libsdl2-ttf-dev:i386 libsodium-dev libsodium-dev:i386
```

Now run the following commands:
```
mkdir build32
cd build32
linux32 cmake -DCMAKE_TOOLCHAIN_FILE=../CMake/32bit.cmake ..
linux32 make -j$(nproc)
```

### Building devilutionX on macOS

#### Building with Xcode (x86 only)

Install Xcode 9.4 and Xcode Command Line tools, this is the last version with support for **32 bits**

Install the dependencies using [Homebrew](https://brew.sh/):
```
brew install automake autoconf libtool
```

Build **libsodium** from our 3rdParty folder
```
cd ./3rdParty/libsodium/
chmod +x autogen.sh
sudo ./autogen.sh
sh ./dist-build/osx.sh
```
Now go to devilutionX/Xcode folder and open the workspace **devilutionX.xcworkspace**

Then select **devilutionX** in targets and pres **CMD + B** to build

#### Building with Xcode from terminal (x86 only)

Note: You need build **libsodium** before

Inside devilutionX folder execute:
```
xcodebuild -workspace "./Xcode/devilutionX.xcworkspace" -scheme "devilutionX" build -configuration Debug
```

#### Building using the bash script (x86 only)

Execute the bash script saved in **.travis/xcode-build.sh** this will build **libsodium** and make devilution project and dependencies.

```
$ ./.travis/xcode-build.sh
```

#### Building for x64 with CMake
Note: macOS is 64-bit only so this is not fully playable at this point.

Install the dependencies using [Homebrew](https://brew.sh/):
```
brew install cmake sdl2_mixer sdl2_ttf libsodium pkg-config
```

Now run the following commands:
```
mkdir build
cd build
cmake ..
make -j$(sysctl -n hw.physicalcpu)
```

# Installing
Once compiled, the devilutionx binary will serve as a replacement for `Diablo.exe`. The following files from the original game patched to 1.09(b) need to be present: `diabdat.mpq`. If `COPYPROT` was defined when compiling, the Diablo CD will also be required.

# Multiplayer
 - TCP/IP only requires the host to expose port 6112
 - UPD/IP requires that all players expose port 6112

All games are encrypted and password protected.

# Contributing
[Guidelines](docs/CONTRIBUTING.md)

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

Absolutely not. Diablo II would require far more work and is still supported by Blizzard. Setting that aside, there are rumours that the game will be remastered which takes the point out of it.
> Are you interested in working for me? I have this game I want you to reverse...

Sorry, but no. This project is time consuming enough as it is, and it's just a hobby.
> I think that's about all, but is Devilution even legal?

That's a tricky question. Under the DMCA, reverse-engineering has exceptions for the purpose of documentation and interoperability. Devilution provides the necessary documentation needed to achieve the latter. However, it falls into an entirely grey area. The real question is whether or not Blizzard deems it necessary to take action.

# Credits
- [sanctuary](https://github.com/sanctuary) - extensively documenting Diablo's game engine
- [BWAPI Team](https://github.com/bwapi) - providing library API to work with Storm
- [Ladislav Zezula](https://github.com/ladislav-zezula) - reversing PKWARE library, further documenting Storm
- [fearedbliss](https://github.com/fearedbliss) - being awe-inspiring
- Climax Studios & Sony - secretly helping with their undercover QA :P
- Blizzard North - wait, this was a typo!
- Depression - reason to waste four months of my life doing this ;)

# Changelog
[From the beginning until release](docs/CHANGELOG.md)

# Legal
This software is being released to the Public Domain. No assets of Diablo are being provided. You must own a copy of Diablo and have access to the assets beforehand in order to use this software.

Battle.net(R) - Copyright (C) 1996 Blizzard Entertainment, Inc. All rights reserved. Battle.net and Blizzard Entertainment are trademarks or registered trademarks of Blizzard Entertainment, Inc. in the U.S. and/or other countries.

Diablo(R) - Copyright (C) 1996 Blizzard Entertainment, Inc. All rights reserved. Diablo and Blizzard Entertainment are trademarks or registered trademarks of Blizzard Entertainment, Inc. in the U.S. and/or other countries.

This software is in no way associated with or endorsed by Blizzard Entertainment(R).
