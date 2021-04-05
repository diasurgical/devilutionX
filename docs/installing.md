# Installing

Download the latest [DevilutionX release](https://github.com/diasurgical/devilutionX/releases) and extract the contents to a location of your choosing or [build from source](./docs/building.md).

<details><summary>Windows</summary>

- Copy `diabdat.mpq` from your CD or GoG installation (or [extract it from the GoG installer](https://github.com/diasurgical/devilutionX/wiki/Extracting-the-.MPQs-from-the-GoG-installer)) to the  folder containing the DevilutionX executable or to the data folder. The data folder path may differ, but will normally be `%AppData%\diasurgical\devilution`
- To run the Hellfire expansion of Diablo you will need to copy `hellfire.mpq`, `hfmonk.mpq`, `hfmusic.mpq`, `hfvoice.mpq` into the same location as diabdat.mpq.
- Run `.\devilutionx.exe`

</details>

<details><summary>Linux</summary>

- Copy `diabdat.mpq` from your CD or GoG installation (or [extract it from the GoG installer](https://github.com/diasurgical/devilutionX/wiki/Extracting-the-.MPQs-from-the-GoG-installer)) to the  folder containing the DevilutionX executable or to the data folder. The data folder path may differ depending on distro, version and security settings, but will normally be  `~/.local/share/diasurgical/devilution/`
- To run the Hellfire expansion of Diablo you will need to copy `hellfire.mpq`, `hfmonk.mpq`, `hfmusic.mpq`, `hfvoice.mpq` into the same location as diabdat.mpq.
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
- To run the Hellfire expansion of Diablo you will need to copy `hellfire.mpq`, `hfmonk.mpq`, `hfmusic.mpq`, `hfvoice.mpq` into the same location as diabdat.mpq.
- Run DevilutionX or DevilutionX Hellfire from menu to start game

</details>

<details><summary>MacOS X</summary>

- Copy `diabdat.mpq` from your CD or GoG installation (or [extract it from the GoG installer](https://github.com/diasurgical/devilutionX/wiki/Extracting-the-.MPQs-from-the-GoG-installer)) to the  folder containing the DevilutionX executable or to the data folder. The data folder path may differ, but will normally be `~/Library/Application Support/diasurgical/devilution`
- To run the Hellfire expansion of Diablo you will need to copy `hellfire.mpq`, `hfmonk.mpq`, `hfmusic.mpq`, `hfvoice.mpq` into the same location as diabdat.mpq.
- Run `./devilutionx`

</details>

<details><summary>Nintendo Switch</summary>

- Copy `devilutionx.nro` and `CharisSILB.ttf` in into `/switch/devilutionx`
- Copy `diabdat.mpq` from your CD or GoG installation (or [extract it from the GoG installer](https://github.com/diasurgical/devilutionX/wiki/Extracting-the-.MPQs-from-the-GoG-installer)) to `/switch/devilutionx`.
- To run the Hellfire expansion of Diablo you will need to copy `hellfire.mpq`, `hfmonk.mpq`, `hfmusic.mpq`, `hfvoice.mpq` into the same location as diabdat.mpq.
- Launch `devilutionx.nro`. Hold R on any installed game and launch it. Do not use album to launch, if you use album, the homebrew will only have a small amount memory available, and the touch keyboard won't work. This is true for all homebrew, not just DevilutionX.

</details>

<details><summary>New Nintendo 3DS</summary>

<details><summary>.3dsx installation</summary>

- Copy `devilutionx.3dsx` into `sd:/3ds/devilutionx/`.
- Copy `diabdat.mpq` from your CD or GoG installation (or [extract it from the GoG installer](https://github.com/diasurgical/devilutionX/wiki/Extracting-the-.MPQs-from-the-GoG-installer)) to `sd:/3ds/devilutionx/`.
- To run the Hellfire expansion of Diablo you will need to copy `hellfire.mpq`, `hfmonk.mpq`, `hfmusic.mpq`, `hfvoice.mpq` into the same location as diabdat.mpq.
- Launch `devilutionx.3dsx` with the [Homebrew Launcher](https://github.com/fincs/new-hbmenu).
  - *Note:* When the Hellfire .mpqs are installed you can still launch regular Diablo by passing `--diablo` to `devilutionx.3dsx`.

</details>

<details><summary>.cia installation</summary>

- Copy `devilutionx.cia` to your SD card and install with a title manager, e.g. [FBI](https://github.com/Steveice10/FBI). `devilutionx.cia` can be removed after being installed.
- Copy `diabdat.mpq` from your CD or GoG installation (or [extract it from the GoG installer](https://github.com/diasurgical/devilutionX/wiki/Extracting-the-.MPQs-from-the-GoG-installer)) to `sd:/3ds/devilutionx/`.
- To run the Hellfire expansion of Diablo you will need to copy `hellfire.mpq`, `hfmonk.mpq`, `hfmusic.mpq`, `hfvoice.mpq` into the same location as diabdat.mpq.
- Launch Diablo from your 3DS Homemenu.

</details>

</details>

<details><summary>Playstation Vita</summary>

 - Install devilutionx.vpk
 - Copy `diabdat.mpq` from your CD or GoG installation (or [extract it from the GoG installer](https://github.com/diasurgical/devilutionX/wiki/Extracting-the-.MPQs-from-the-GoG-installer)) to `ux0:/data/diasurgical/devilution/`.
- To run the Hellfire expansion of Diablo you will need to copy `hellfire.mpq`, `hfmonk.mpq`, `hfmusic.mpq`, `hfvoice.mpq` into the same location as diabdat.mpq.

</details>

<details><summary>ClockworkPi GameShell</summary>

- Copy the `__init__.py` to a newly created folder under /home/cpi/apps/Menu and run it from the menu. The folder then symbolizes the devilutionX icon.
- From this menu, you can press 'X' to clone the git repository for devilutionX and compile the code. Dependencies are installed automatically (cmake and SDL development packages).
- Once installed, 'X' pulls the updated code and does the compiling. Note that any changes made locally to the source are reverted before pulling.
- When the compile is finished, copy `diabdat.mpq` from your CD or GoG installation (or [extract it from the GoG installer](https://github.com/diasurgical/devilutionX/wiki/Extracting-the-.MPQs-from-the-GoG-installer)) to `/home/cpi/.local/share/diasurgical/devilution/`
- To run the Hellfire expansion of Diablo you will need to copy `hellfire.mpq`, `hfmonk.mpq`, `hfmusic.mpq`, `hfvoice.mpq` into the same location as diabdat.mpq.
- You can now play the game from the same icon.

</details>

<details><summary>GKD350h</summary>

- Copy [devilutionx-gkd350h.opk](https://github.com/diasurgical/devilutionX/releases/latest/download/devilutionx-gkd350h.opk) to `/media/data/apps` or `/media/sdcard/apps/`.
- Copy `diabdat.mpq` from your CD or GoG installation (or [extract it from the GoG installer](https://github.com/diasurgical/devilutionX/wiki/Extracting-the-.MPQs-from-the-GoG-installer)) to `/usr/local/home/.local/share/diasurgical/devilution/diabdat.mpq`
- To run the Hellfire expansion of Diablo you will need to copy `hellfire.mpq`, `hfmonk.mpq`, `hfmusic.mpq`, `hfvoice.mpq` into the same location as diabdat.mpq.

</details>

<details><summary>RetroFW</summary>

**Requires RetroFW 2.0+.**

-  Copy [devilutionx-retrofw.opk](https://github.com/diasurgical/devilutionX/releases/latest/download/devilutionx-retrofw.opk) to the apps directory.
- Copy `diabdat.mpq` from your CD or GoG installation (or [extract it from the GoG installer](https://github.com/diasurgical/devilutionX/wiki/Extracting-the-.MPQs-from-the-GoG-installer))  to `~/.local/share/diasurgical/devilution`
- To run the Hellfire expansion of Diablo you will need to copy `hellfire.mpq`, `hfmonk.mpq`, `hfmusic.mpq`, `hfvoice.mpq` into the same location as diabdat.mpq.

`~` is your home directory, `/home/retrofw` by default.

</details>

<details><summary>RG350</summary>

**Requires firmware v1.5+**

- Copy [devilutionx-rg350.opk](https://github.com/diasurgical/devilutionX/releases/latest/download/devilutionx-rg350.opk) to `/media/sdcard/APPS/`.
- Copy `diabdat.mpq` from your CD or GoG installation (or [extract it from the GoG installer](https://github.com/diasurgical/devilutionX/wiki/Extracting-the-.MPQs-from-the-GoG-installer))  to `/media/home/.local/share/diasurgical/devilution/diabdat.mpq`
- To run the Hellfire expansion of Diablo you will need to copy `hellfire.mpq`, `hfmonk.mpq`, `hfmusic.mpq`, `hfvoice.mpq` into the same location as diabdat.mpq.
- 
   **NOTE:** You can copy `diabdat.mpq` to sdcard instead and create a symlink at the expected location. To do this, SSH into your RG350 and run:

   ~~~bash
   ln -sf /media/sdcard/<path_to_diabdat.mpq> /media/home/.local/share/diasurgical/devilution/diabdat.mpq
   ~~~

</details>

## Optional

For better widescreen support you can copy [devilutionx.mpq](https://github.com/diasurgical/devilutionX/raw/master/Packaging/resources/devilutionx.mpq) to the same location as `diabdat.mpq`
