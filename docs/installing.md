# Installing

First, you will need access to the game MPQ files.
- First, locate `DIABDAT.MPQ` on your CD or in the GoG installation (or [extract it from the GoG installer](https://github.com/diasurgical/devilutionX/wiki/Extracting-the-.MPQs-from-the-GoG-installer)).
- For the Diablo: Hellfire expansion you will also need `hellfire.mpq`, `hfmonk.mpq`, `hfmusic.mpq`, `hfvoice.mpq`.
- Lastly, DevilutionX comes with [devilutionx.mpq](https://github.com/diasurgical/devilutionX/raw/master/Packaging/resources/devilutionx.mpq) which you will also need.

Download the latest [DevilutionX release](https://github.com/diasurgical/devilutionX/releases) for your system (if available) and extract the contents to a location of your choosing, or [build from source](building.md). Then follow the system-specific instructions below.

<details><summary>Windows</summary>

- Copy the MPQ files to the folder containing the DevilutionX exe, or to the data folder. The data folder path may differ, but will normally be `%AppData%\diasurgical\devilution`
- Run `devilutionx.exe`

</details>

<details><summary>Linux</summary>

- Copy the MPQ files to the folder containing the DevilutionX executable, or to the data folder. The data folder path may differ depending on distro, version, and security settings, but will normally be `~/.local/share/diasurgical/devilution/`
- Install [SDL2](https://www.libsdl.org/download-2.0.php) and [SDL2_ttf](https://www.libsdl.org/projects/SDL_ttf/):
 - Ubuntu/Debian/Rasbian `sudo apt install libsdl2-2.0-0 libsdl2-ttf-2.0-0`
 - Fedora `sudo dnf install SDL2 SDL2_ttf`
- Run `./devilutionx`

</details>

<details><summary>Ubuntu PPA</summary>

- Add [DevilutionX stable](https://launchpad.net/~devilutionx/+archive/ubuntu/stable)

```bash
sudo add-apt-repository ppa:devilutionx/stable
```

- Install DeviliutionX

```bash
sudo apt update
sudo apt install devilutionx
```

- Copy the MPQ files to `~/.local/share/diasurgical/devilution/`
- Run DevilutionX or DevilutionX Hellfire from the menu to start the game

</details>

<details><summary>MacOS X</summary>

- Copy the MPQ files to the folder containing the DevilutionX application, or to the data folder. The data folder path may differ, but will normally be `~/Library/Application Support/diasurgical/devilution`
- Double-click `devilutionx`

</details>

<details><summary>Nintendo Switch</summary>

- Copy `devilutionx.nro` and `CharisSILB.ttf` in into `/switch/devilutionx`
- Copy the MPQ files to `/switch/devilutionx`.
- Launch `devilutionx.nro` by holding R the installed game. Do not use the album to launch, if you use the album, the homebrew will only have a small amount of memory available, and the touch keyboard won't work. This is true for all homebrew, not just DevilutionX.

</details>

<details><summary>Android</summary>

- Copy the APK file to device and tap on it on device's file explorer or install via `adb install` (if USB debugging is enabled on device).
- Copy the MPQ files to `devilutionx` folder in root of phone's internal memory or SD card. Create the folder if it doesn't exist.

</details>

<details><summary>New Nintendo 3DS</summary>

### .3dsx installation

- Copy `devilutionx.3dsx` into `sd:/3ds/devilutionx/`.
- Copy the MPQ files into `sd:/3ds/devilutionx/`.
- Copy the `CharisSILB.ttf` font file into `sd:/3ds/devilutionx/`.
- Launch `devilutionx.3dsx` with the [Homebrew Launcher](https://github.com/fincs/new-hbmenu).
    - *Note:* When the Hellfire .mpqs are installed you can still launch regular Diablo by passing `--diablo` to `devilutionx.3dsx`.

### .cia installation

- Copy `devilutionx.cia` to your SD card.
- Copy the MPQ files to `sd:/3ds/devilutionx/`.
- Copy the `CharisSILB.ttf` font file into `sd:/3ds/devilutionx/`.
- Install `devilutionx.cia` with a title manager (e.g. [FBI](https://github.com/Steveice10/FBI)).
    - `devilutionx.cia` can be removed after being installed.
- Launch Diablo from your 3DS Homemenu.

</details>

<details><summary>Playstation Vita</summary>

 - Install devilutionx.vpk
 - Copy the MPQ files to `ux0:/data/diasurgical/devilution/`.

</details>

<details><summary>ClockworkPi GameShell</summary>

- Copy the `__init__.py` to a newly created folder under /home/cpi/apps/Menu and run it from the menu. The folder then symbolizes the devilutionX icon.
- From this menu, you can press 'X' to clone the git repository for devilutionX and compile the code. Dependencies are installed automatically (cmake and SDL development packages).
- Once installed, 'X' pulls the updated code and does the compiling. Note that any changes made locally to the source are reverted before pulling.
- When the compile is finished, Copy the MPQ files to `/home/cpi/.local/share/diasurgical/devilution/`
- You can now play the game from the same icon.

</details>

<details><summary>GKD350h</summary>

- Copy [devilutionx-gkd350h.opk](https://github.com/diasurgical/devilutionX/releases/latest/download/devilutionx-gkd350h.opk) to `/media/data/apps` or `/media/sdcard/apps/`.
- Copy the MPQ files to `/usr/local/home/.local/share/diasurgical/devilution/`

</details>

<details><summary>RetroFW</summary>

**Requires RetroFW 2.0+.**

- Copy [devilutionx-retrofw.opk](https://github.com/diasurgical/devilutionX/releases/latest/download/devilutionx-retrofw.opk) to the apps directory.
- Copy the MPQ files to `~/.local/share/diasurgical/devilution`

`~` is your home directory, `/home/retrofw` by default.

</details>

<details><summary>RG350</summary>

**Requires firmware v1.5+**

- Copy [devilutionx-rg350.opk](https://github.com/diasurgical/devilutionX/releases/latest/download/devilutionx-rg350.opk) to `/media/sdcard/APPS/`.
- Copy the MPQ files to `/media/home/.local/share/diasurgical/devilution/`
-
 **NOTE:** You can copy the MPQ files to sdcard instead and create a symlink at the expected location. To do this, SSH into your RG350 and run:

 ~~~bash
 ln -sf /media/sdcard/<path_to_MPQ> /media/home/.local/share/diasurgical/devilution/<MPQ>
 ~~~

</details>
