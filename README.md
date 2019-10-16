# IMPORTANT: Work in progress.

[![Downloads](https://img.shields.io/github/downloads/gokuhs/devilutionX/total.svg)](https://github.com/gokuhs/devilutionX/releases)
[![github stars](https://img.shields.io/github/stars/gokuhs/devilutionX.svg)](https://github.com/gokuhs/devilutionX/stargazers)

DevilutionX original ![Discord Channel](https://avatars3.githubusercontent.com/u/1965106?s=16&v=4) [Discord Chat Channel](https://discord.gg/aQBQdDe)

# How To Play:
 - Copy diabdat.mpq from your CD, or GoG install folder, to ux0:/data/DVLX00001/data ; Make sure it is all lowercase.
 - [Download DevilutionX](https://github.com/gokuhs/devilutionX/releases), or build from source
 - Install it in PS Vita using your favorite method

Please keep in mind that this is still being worked on and is missing parts of UI and some minor bugs, see [milestone 1](https://github.com/diasurgical/devilutionX/milestone/1) for a full list of known issues.

# Building from Source
<details><summary>VitaSDK</summary>
	
### Installing vitasdk toolchain

[Go to vitasdk and follow instructions](https://vitasdk.org/)

### Compiling
```
mkdir build
cd build
cmake ..
cmake --build . -j $(nproc)
```
</details>



# Contributing
[Guidelines](docs/CONTRIBUTING.md)


# F.A.Q.
<details><summary>Click to reveal</summary>

> Wow, does this mean I can download and play Diablo for free now?

No, you'll need access to the data from the original game. If you don't have an original CD then you can [buy Diablo from GoG.com](https://www.gog.com/game/diablo). Alternatively you can also use `spawn.mpq` from the [http://ftp.blizzard.com/pub/demos/diablosw.exe](shareware) version and compile the with the SPAWN flag defined.
> Cool, so I fired your mod up, but there's no 1080p or new features?

We're working on it.
> What about Hellfire?

Hellfire was a bit of a flop on the developer's part. Support may come in the future once the base game is finished.
</details>

# Credits
- [devnoname120](https://github.com/devnoname120) - for supporting me :)
- Thank the people around me for enduring my obsession to get this through.
- Original developpers and collaborators for maintain this project <3

# Original Credits
- Reverse engineered by GalaXyHaXz in 2018
- [sanctuary](https://github.com/sanctuary) - extensively documenting Diablo's game engine
- [BWAPI Team](https://github.com/bwapi) - providing library API to work with Storm
- [Ladislav Zezula](https://github.com/ladislav-zezula) - reversing PKWARE library, further documenting Storm
- [fearedbliss](https://github.com/fearedbliss) - being awe-inspiring
- Climax Studios & Sony - secretly helping with their undercover QA :P
- Blizzard North - wait, this was a typo!
- Depression - reason to waste four months of my life doing this ;)

And a special thanks to all the support and people who work on Devilution to make it possible! <3

# Legal
Devilution is released to the Public Domain. The documentation and function provided by Devilution may only be utilized with assets provided by ownership of Diablo.

Battle.net(R) - Copyright (C) 1996 Blizzard Entertainment, Inc. All rights reserved. Battle.net and Blizzard Entertainment are trademarks or registered trademarks of Blizzard Entertainment, Inc. in the U.S. and/or other countries.

Diablo(R) - Copyright (C) 1996 Blizzard Entertainment, Inc. All rights reserved. Diablo and Blizzard Entertainment are trademarks or registered trademarks of Blizzard Entertainment, Inc. in the U.S. and/or other countries.

Devilution and any of its' maintainers are in no way associated with or endorsed by Blizzard Entertainment(R).
