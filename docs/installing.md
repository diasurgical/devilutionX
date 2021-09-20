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
