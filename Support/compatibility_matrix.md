# Compatibility Matrix, Compilations, Platform Statuses, Etc

Please use UTC times for all entries. The Z ending represents UTC time.

## Status Cheat Sheet

[Modeled after Wine HQ's Rating System](https://wiki.winehq.org/AppDB_Rating_Definitions)

| Rank | Description |
| --- | --- |
| Platinum | Works perfectly right after compilation either better or equal to Native Diablo Executable. |
| Gold | Works right after compilation with no crashes during gameplay but workarounds needed. |
| Silver | Works right after compilation with no crashes during gameplay but issues exist where no workarounds exist. |
| Bronze | Mostly works but there are still some problems remaining that prevent a full playthrough.| 
| Trash | Game has severe problems and cannot be played. |

## Windows

| Date | Status | OS | Bitness | Version (OS) | Build (OS) | Compiler | Build Platform | User | Workaround |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 2018-06-24 @ 17:05 Z| Gold | 10 | x64 | 1803 | 17134.112 | i686-w64-mingw32-gcc-7.3.0 | MSYS 2 i686 | fearedbliss | Needed to use ddraw patch. |
| 2018-06-24 @ 12:52 Z| Platinum | 7 | x64 | 6.1 | 7601 | Visual C++ 6.0 | VC++ | Sergi4UA | None |
| 2018-06-24 @ 01:00 Z| Platinum | 7 | x64 | 6.1 | 7601 | Visual C++ 5.10 | VC++ | galaxyhaxz | None |
| 2018-06-24 @ 18:00 Z| Gold | 10 | x64 | 1803 | 17134.112 | Visual Studio 2017 (Community) | VC++ | MadHed | Disable DEP in linker options |
| 2018-06-24 @ 16:00 Z| Gold | 7 | x64 | 6.1 | 7601 | Visual Studio 2017 (Community) | VC++ | StephenCWills | Disable DEP in linker options |
| 2018-06-26 @ 19:30 Z| Platinum | 7 | x64 | 6.1 | 7601 | i686-w64-mingw32-g++ (GCC) 6.4.0 | Cygwin | StephenCWills | None |
| 2018-07-05 @ 23:54 Z| Gold | 10 | x64 | 1803 | 17134.112 | Visual Studio 2017 (Community) | VC++ | fearedbliss | Disable DEP in linker options |

## Linux

| Date | Status | OS | Bitness | Version (OS) | Build (OS) | Compiler | Build Platform | User | Workaround |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 2018-08-20 @ 12:05 Z| Gold | Ubuntu (WSL) | x64 | xenial | 16.04.4 LTS | i686-w64-mingw32-g++ (GCC) 5.3.1 20160211 | Mingw64-x86 | ChaosMarc | Needed to use ddraw patch. |
| 2018-08-20 @ 12:05 Z| Trash | Ubuntu (WSL) | x64 | bionic | 18.04 LTS | i686-w64-mingw32-g++ (GCC) 7.3-win32 20180312 | Mingw64-x86 | ChaosMarc | Crashes on startup (#107) |

## Mac OS X

| Date | Status | OS | Bitness | Version (OS) | Build (OS) | Compiler | Build Platform | User | Workaround |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
