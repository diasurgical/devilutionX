# Installing

First, you will need access to the game's MPQ files.
- Locate `DIABDAT.MPQ` on your CD, or in the [GoG](https://www.gog.com/game/diablo) installation (or [extract it from the GoG installer](https://github.com/diasurgical/devilutionX/wiki/Extracting-the-.MPQs-from-the-GoG-installer)).
- For the Diablo: Hellfire expansion you will also need `hellfire.mpq`, `hfmonk.mpq`, `hfmusic.mpq`, `hfvoice.mpq`.
- DevilutionX comes with [devilutionx.mpq](https://github.com/diasurgical/devilutionx-assets/releases/download/v1/devilutionx.mpq) which is required to run the game properly.
- Chinese, Korean, and Japanese users will also need [fonts.mpq](https://github.com/diasurgical/devilutionx-assets/releases/download/v1/fonts.mpq) or the text will be missing.
- For Polish voice support you need [pl.mpq](https://github.com/diasurgical/devilutionx-assets/releases/download/v1/pl.mpq)

Download the latest [DevilutionX release](https://github.com/diasurgical/devilutionX/releases) for your system (if available) and extract the contents to a location of your choosing, or [build from source](building.md). Then follow the system-specific instructions below.

<details><summary>Android</summary>

First install the App via one of these 3 methods:
  - [Google Play](https://play.google.com/store/apps/details?id=org.diasurgical.devilutionx)
  - Copy the APK file to the device and tap on it in the device's file explorer and follow the instructions
  - Install via `adb install` (if USB debugging is enabled on the device)

Then launch the App, this will let it create the folder where you need to place the MPQ files.

Connect the device to your computer via USB cable, and allow data access from your device:

![image](https://user-images.githubusercontent.com/204594/139543023-3c45bb22-35f7-41af-8b3d-c714a9542d23.png)
  
Open the device's internal storage, and navigate to `Android/data/org.diasurgical.devilutionx/files`, then copy the MPQ-files to this folder.

![image](https://user-images.githubusercontent.com/204594/139542962-4e776854-6ca4-4872-8ed6-6303fc4bf040.png)
  
When the transfer is done you can disconnect your device and press "Check again" in the App to start the game.

![image](https://user-images.githubusercontent.com/204594/139541657-d8c1197d-fbef-42b6-a34f-2b17f1ceab5f.png)

</details>

<details><summary>Windows</summary>

- Copy the MPQ files to the folder containing the DevilutionX exe, or to the data folder. The data folder path may differ, but will normally be `%AppData%\diasurgical\devilution`
- Run `devilutionx.exe`

</details>

<details><summary>Linux</summary>

- Copy the MPQ files to the folder containing the DevilutionX executable, or to the data folder. The data folder path may differ depending on distro, version, and security settings, but will normally be `~/.local/share/diasurgical/devilution/`
- Install [SDL2](https://www.libsdl.org/download-2.0.php):
 - Ubuntu/Debian/Rasbian `sudo apt install libsdl2-2.0-0`
 - Fedora `sudo dnf install SDL2`
- Run `./devilutionx`

</details>

<details><summary>MacOS X</summary>

- Copy the MPQ files to the folder containing the DevilutionX application, or to the data folder. The data folder path may differ, but will normally be `~/Library/Application Support/diasurgical/devilution`
- Double-click `devilutionx`

</details>

<details><summary>Nintendo Switch</summary>

- Copy `devilutionx.nro` in into `/switch/devilutionx`
- Copy the MPQ files to `/switch/devilutionx`.
- Launch `devilutionx.nro` by holding R the installed game. Do not use the album to launch, if you use the album, the homebrew will only have a small amount of memory available, and the touch keyboard won't work. This is true for all homebrew, not just DevilutionX.

</details>

<details><summary>Nintendo 3DS</summary>

### .cia installation

- Copy `devilutionx.cia` to your SD card.
- Copy the Diablo and Hellfire MPQ files to `sd:/3ds/devilutionx/`. You do not need to copy devilutionx.mpq.
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

- Copy [devilutionx-gkd350h.opk](https://github.com/diasurgical/devilutionX/releases/download/1.0.1/devilutionx-gkd350h.opk) to `/media/data/apps` or `/media/sdcard/apps/`.
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
