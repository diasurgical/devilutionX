<p align="center">
<img width="554" src="https://user-images.githubusercontent.com/204594/113575181-c946a400-961d-11eb-8347-a8829fa3830c.png">
</p>

---

[![Downloads](https://img.shields.io/github/downloads/diasurgical/devilutionX/total.svg)](https://github.com/diasurgical/devilutionX/releases)
[![github stars](https://img.shields.io/github/stars/diasurgical/devilutionX.svg)](https://github.com/diasurgical/devilutionX/stargazers)
[![codecov](https://codecov.io/gh/diasurgical/devilutionX/branch/master/graph/badge.svg)](https://codecov.io/gh/diasurgical/devilutionX)
[![CircleCI](https://circleci.com/gh/diasurgical/devilutionX.svg?style=shield)](https://circleci.com/gh/diasurgical/devilutionX) [![Build status](https://ci.appveyor.com/api/projects/status/1a0jus2372qvksht?svg=true)](https://ci.appveyor.com/project/AJenbo/devilutionx)


![Discord Channel](https://avatars3.githubusercontent.com/u/1965106?s=16&v=4) [Discord Chat Channel](https://discord.gg/YQKCAYQ)

<p align="center">
<img width="838" src="https://user-images.githubusercontent.com/204594/113578478-26912400-9623-11eb-9ff6-9bd9717462b6.png">
</p>

<sub>*(The health-bar, and XP-bar are off by default, but can be enabled in the [ini-file](https://github.com/diasurgical/devilutionX/wiki/DevilutionX-diablo.ini-configuration-guide). Widescreen and transparency can also be disabled if preferred)*</sub>

# How To Install:
 - Download [the latest DevilutionX release](https://github.com/diasurgical/devilutionX/releases), or build from source
 - Copy diabdat.mpq from your CD or GoG installation (or [extract it from the GoG installer](https://github.com/diasurgical/devilutionX/wiki/Extracting-the-DIABDAT.MPQ-from-the-GoG-installer)) to the DevilutionX install folder or data folder. The DevilutionX install folder is the one that contains the DevilutionX executable. The data folder path may differ depending on OS version and security settings, but will normally be as follows:
    - macOS `~/Library/Application Support/diasurgical/devilution`
    - Linux `~/.local/share/diasurgical/devilution/`
    - Windows `%AppData%\Roaming\diasurgical\devilution`
 - Install [SDL2](https://www.libsdl.org/download-2.0.php), [SDL2_mixer](https://www.libsdl.org/projects/SDL_mixer/) and [SDL2_ttf](https://www.libsdl.org/projects/SDL_ttf/) (included in macOS and Windows releases):
    - Ubuntu/Debian/Rasbian `sudo apt-get install libsdl2-ttf-2.0-0 libsdl2-mixer-2.0-0`
 - Run `./devilutionx`

## Installig on Ubuntu via PPA

 - Installing and playing on **Ubuntu**
    - Add [DevilutionX stable](https://launchpad.net/~devilutionx/+archive/ubuntu/stable) or [DevilutionX git](https://launchpad.net/~devilutionx/+archive/ubuntu/dev) PPA repository
      ```
      sudo add-apt-repository ppa:devilutionx/stable
      sudo add-apt-repository ppa:devilutionx/dev
      ```
    - Install **DeviliutionX**
      ```
      sudo apt update
      sudo apt install devilutionx
      ```
    - For **Diablo I**: copy diabdat.mpq file to `~/.local/share/diasurgical/devilution/` folder

    - For **Diablo Hellfire**: copy hellfire.mpq, hfmonk.mpq, hfmusic.mpq, hfvoice.mpq files to `~/.local/share/diasurgical/devilution/` folder

    - Additionally for better widescreen support you can copy [devilutionx.mpq](https://github.com/diasurgical/devilutionX/raw/master/Packaging/resources/devilutionx.mpq) file to `~/.local/share/diasurgical/devilution/` folder

    - Run **DevilutionX** or **DevilutionX Hellfire** from menu to start game

# Building from Source

Want to compile the program by yourself, great! Simply follow the [build instructions](./docs/building.md);

# Multiplayer
 - TCP/IP only requires the host to expose port 6112
 - UDP/IP requires that all players expose port 6112

All games are encrypted and password protected.

# Controller support

DevilutionX supports gamepad controls.

Default controller mappings (A/B/X/Y as in Nintendo layout, so the rightmost button is attack; A ○, B ×, X △, Y □):

- Left analog or D-Pad: move hero
- A: attack nearby enemies, talk to townspeople and merchants, pickup/place items in the inventory, OK while in main menu
- B: select spell, back while in menus
- X: pickup items, open nearby chests and doors, use item in the inventory
- Y: cast spell, delete character while in main menu
- L1: use health item from belt
- R1: use mana potion from belt
- L2: character sheet (alt: Start + L1 or ←)
- R2: inventory (alt: Start + L2 or →)
- Left analog click: toggle automap (alt: Start + ↓)
- Start + Select: game menu (alt: Start + ↑)
- Select + A/B/X/Y: Spell hotkeys
- Right analog: move automap or simulate mouse
- Right analog click: left mouse click (alt: Select + L1)
- Select + Right analog click: right mouse click (alt: Select + R1)
- Select + L2: quest log (alt: Start + Y)
- Select + R2: spell book (alt: Start + B)

For now, they can be re-mapped by changing `SourceX/controls` or by setting the `SDL_GAMECONTROLLERCONFIG` environment
variable (see
[SDL_GameControllerDB](https://github.com/gabomdq/SDL_GameControllerDB)).

# Contributing
[Guidelines](docs/CONTRIBUTING.md)

# Mods

[List of known mods based on DevilutionX](docs/mods.md)

# F.A.Q.
> Wow, does this mean I can download and play Diablo for free now?

No, you'll need access to the data from the original game. If you don't have an original CD then you can [buy Diablo from GoG.com](https://www.gog.com/game/diablo). Alternately you can use `spawn.mpq` from the [shareware](http://ftp.blizzard.com/pub/demos/diablosw.exe) version to play the shareware portion of the game.
> What game changes does DevilutionX provide

DevilutionX's main focus is to make the game work on multiple platforms. An additional goal is to make the engine mod friendly. As such, there are no changes to gameplay, but we will be making some enhancments to the engine itself. For example, the engine now has upscaling, unlocked fps, controller support, and multiplayer via TCP.

For a full list of changes see our [changelog](docs/CHANGELOG.md).

> Is 1080p supported?

Yes, the game will automatically adjust to your screen. This can be further adjusted in the game ini file.
> What about Hellfire?

Yes, you can play both Hellfire, regular Diablo, or the shareware version all from the same installation of DevilutionX.
> Does it work with Battle.net?

Battle.net is a service provided by Blizzard. We are not associated with them, so we have not worked on intergrating with their service.
</details>

# Credits
- The original Devilution project [Devilution](https://github.com/diasurgical/devilution#credits)
- [Everyone](https://github.com/diasurgical/devilutionX/graphs/contributors) who worked on Devilution/DevilutionX
- [Nikolay Popov](https://www.instagram.com/nikolaypopovz/) who provided new backgrounds and icons.
- And a thanks to all who support the project, report bugs and help spread the word <3

# Legal
DevilutionX is released to the Public Domain. The documentation and functionality provided by DevilutionX may only be utilized with assets provided by ownership of Diablo.

The source code in this repository is for non-commerical use only. If you use the source code you may not charge others for access to it or any derivative work thereof.

Diablo® - Copyright © 1996 Blizzard Entertainment, Inc. All rights reserved. Diablo and Blizzard Entertainment are trademarks or registered trademarks of Blizzard Entertainment, Inc. in the U.S. and/or other countries.

DevilutionX and any of its maintainers are in no way associated with or endorsed by Blizzard Entertainment®.
