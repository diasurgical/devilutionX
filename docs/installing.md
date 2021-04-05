# Installing

Download the latest [DevilutionX release](https://github.com/diasurgical/devilutionX/releases), or [build from source](./docs/building.md), and extract the contents to a location of your choosing.

<details><summary>Windows</summary>

- Copy `diabdat.mpq` from your CD or GoG installation (or [extract it from the GoG installer](https://github.com/diasurgical/devilutionX/wiki/Extracting-the-.MPQs-from-the-GoG-installer)) to the  folder containing the DevilutionX executable or to the data folder. The data folder path may differ, but will normally be `%AppData%\diasurgical\devilution`
- To run the Hellfire expansion of Diablo you will need to put `hellfire.mpq`, `hfmonk.mpq`, `hfmusic.mpq`, `hfvoice.mpq` into the same location as diabdat.mpq.
- Run `.\devilutionx.exe`

</details>

<details><summary>Linux</summary>

- Copy `diabdat.mpq` from your CD or GoG installation (or [extract it from the GoG installer](https://github.com/diasurgical/devilutionX/wiki/Extracting-the-.MPQs-from-the-GoG-installer)) to the  folder containing the DevilutionX executable or to the data folder. The data folder path may differ depending on distro, version and security settings, but will normally be  `~/.local/share/diasurgical/devilution/`
- To run the Hellfire expansion of Diablo you will need to put `hellfire.mpq`, `hfmonk.mpq`, `hfmusic.mpq`, `hfvoice.mpq` into the same location as diabdat.mpq.
- Install [SDL2](https://www.libsdl.org/download-2.0.php), [SDL2_mixer](https://www.libsdl.org/projects/SDL_mixer/) and [SDL2_ttf](https://www.libsdl.org/projects/SDL_ttf/):
  - Ubuntu/Debian/Rasbian `sudo apt install libsdl2-2.0-0 libsdl2-ttf-2.0-0 libsdl2-mixer-2.0-0`
  - Fedora `sudo dnf install SDL2 SDL2_ttf SDL2_mixer`
- Run `./devilutionx`

</details>

<details><summary>Ubuntu PPA</summary>

- Add [DevilutionX stable](https://launchpad.net/~devilutionx/+archive/ubuntu/stable) or [DevilutionX git](https://launchpad.net/~devilutionx/+archive/ubuntu/dev) PPA repository

```bash
sudo add-apt-repository ppa:devilutionx/stable
sudo add-apt-repository ppa:devilutionx/dev
```

- Install DeviliutionX

```bash
sudo apt update
sudo apt install devilutionx
```

- Copy `diabdat.mpq` from your CD or GoG installation (or [extract it from the GoG installer](https://github.com/diasurgical/devilutionX/wiki/Extracting-the-.MPQs-from-the-GoG-installer)) to `~/.local/share/diasurgical/devilution/`
- To run the Hellfire expansion of Diablo you will need to put `hellfire.mpq`, `hfmonk.mpq`, `hfmusic.mpq`, `hfvoice.mpq` into the same location as diabdat.mpq.

- Run DevilutionX or DevilutionX Hellfire from menu to start game

</details>

<details><summary>MacOS X</summary>

- Copy `diabdat.mpq` from your CD or GoG installation (or [extract it from the GoG installer](https://github.com/diasurgical/devilutionX/wiki/Extracting-the-.MPQs-from-the-GoG-installer)) to the  folder containing the DevilutionX executable or to the data folder. The data folder path may differ, but will normally be `~/Library/Application Support/diasurgical/devilution`
- To run the Hellfire expansion of Diablo you will need to put `hellfire.mpq`, `hfmonk.mpq`, `hfmusic.mpq`, `hfvoice.mpq` into the same location as diabdat.mpq.
- Run `./devilutionx`

</details>

<details><summary>Nintendo Switch</summary>

- Put `devilutionx.nro` and `CharisSILB.ttf` in into `/switch/devilutionx`
- Copy `diabdat.mpq` from your CD or GoG installation (or [extract it from the GoG installer](https://github.com/diasurgical/devilutionX/wiki/Extracting-the-.MPQs-from-the-GoG-installer)) to `/switch/devilutionx`.
 - Launch `devilutionx.nro`. Hold R on any installed game and launch it. Do not use album to launch, if you use album, the homebrew will only have a small amount memory available, and the touch keyboard won't work. This is true for all homebrew, not just DevilutionX.

</details>

<details><summary>ClockworkPi GameShell</summary>

- Copy the `__init__.py` to a newly created folder under /home/cpi/apps/Menu and run it from the menu. The folder then symbolizes the devilutionX icon.
- From this menu, you can press 'X' to clone the git repository for devilutionX and compile the code. Dependencies are installed automatically (cmake and SDL development packages).
- Once installed, 'X' pulls the updated code and does the compiling. Note that any changes made locally to the source are reverted before pulling.
- When the compile is finished, copy `diabdat.mpq` from your CD or GoG installation (or [extract it from the GoG installer](https://github.com/diasurgical/devilutionX/wiki/Extracting-the-.MPQs-from-the-GoG-installer)) to `/home/cpi/.local/share/diasurgical/devilution/`
- To run the Hellfire expansion of Diablo you will need to put `hellfire.mpq`, `hfmonk.mpq`, `hfmusic.mpq`, `hfvoice.mpq` into the same location as diabdat.mpq.
- You can now play the game from the same icon.

</details>

## Optional

For better widescreen support you can copy [devilutionx.mpq](https://github.com/diasurgical/devilutionX/raw/master/Packaging/resources/devilutionx.mpq) to the same location as `diabdat.mpq`
