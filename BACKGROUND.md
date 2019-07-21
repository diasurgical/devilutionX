# Project background

To give further background, the Devilution team has primarily relied on these resources:

1. The Japanese Playstation port with debug symbols contained in `DIABPSX.SYM`. (see [[1]]).

Example debug info of the Cathedral dungeon generation algorithm:

```cpp
// address: 0x801259D0
// line start: 612
// line end:   624
void DRLG_L1Floor__Fv() {
// register: 19
register int i;
// register: 20
register int j;
// register: 3
register long rv;
}
```

2. The debug release of the PE executable, which contained assert strings (see [[2]]).

Example assert string:

```cpp
"plr[myplr].InvGrid[i] <= plr[myplr]._pNumInv"
```

3. The Rich header of the PE executable, which details the exact version of the original compilers and linkers used to build `Diablo.exe` (see [[3]], [[4]]).

Example information recovered from the Rich header of `Diablo.exe`:

```
Id  Build  Count  Name       Description
 0      0    155  Unknown    [---] Number of imported functions (old)
 1      0    229  Import0    [---] Number of imported functions
 6   1668      1  Cvtres500  [RES] VS97 (5.0) SP3 cvtres 5.00.1668
 2   7303     29  Linker510  [IMP] VS97 (5.0) SP3 link 5.10.7303
 3   7303      1  Cvtomf510        VS97 (5.0) SP3 cvtomf 5.10.7303
 4   8447      2  Linker600  [LNK] VC++ 6.0 SP3,SP4,SP5,SP6 link 6.00.8447
48   9044     72  Utc12_2_C  [---] VC++ 6.0 SP5 Processor Pack
19   9049     12  Linker512        Microsoft LINK 5.12.9049
```

4. Discovery of the original set of compiler flags used to build `Diablo.exe` (see [[5]]).

Primarily "/O1" was used, but there are also peculiarities such as the use of both Microsoft Visual Studio 6 and Microsoft Visual Code 5 for linking the game.

5. The heartfelt dedication of a team of people. GalaXyHaXz did the initial heavy lifting and succeeded in the tremendous task of getting the decompiled source code of Diablo 1 compiling with the original toolchain. Later on she released the project open source and a community of open source collaborators formed. Most of us have never met in real life prior to joining the project, which stands to show that there is strength in online collaboration that transcend both culture and borders.

6. The Beta release and the Alpha4 release of Diablo 1 has also proved invaluable resources for cross-validation as the compiler optimization level was not set to release mode for these binaries.

Interestingly, in the process a number of bugs in the original implementation of Diablo 1 were discovered. These have been documented in the source code of Devilution with `// BUGFIX: foo` comments, and have also been detailed in [[6]].

To track the progress of the project, the "Binary identical functions" milestone has been used in tandem with an assembly diffing tool developed in Rust (see [[7]], [[8]]).

Anecdotally, it was an incredible moment when we first managed to run the cross-platform port of Diablo 1 (DevilutionX, see [[9]]) natively on Linux and succeeded in playing a multiplayer game connecting our computers in Korea and Denmark. It is equally thrilling to see the modding and porting community picking up the torch and already succeeding in porting Diablo 1 to Nintendo Switch!

The main reason for conducting this bit of software archeology is to preserve the classic title that is Diablo 1, for generations to come. And to revive it for modern hardware platforms and make it more mod-friendly in the age of open source software.

Happy coding! - The Devilution Team

P.S. the project README explicitly states that to play the game, you still need to have access to the original game assets released on the Diablo 1 CD. To acquire a legal copy, please refer to https://www.gog.com/game/diablo

P.P.S. for the verification process, there have been proposals that are both ambitious at a level of PhD research (see [[10]]) and that made us feel warm and fuzzy <3 In the end, many of the techniques outlined were discussed mostly on a design level, some were included as Proof of Concepts, but most of the work in reverse engineering Diablo 1 was from tender labour of a team that care for Diablo 1 the way you would your firstborn child.

[1]: https://github.com/diasurgical/scalpel/blob/master/psx/_dump_/_dump_merge_c_src_/diabpsx/source/drlg_l1.cpp

[2]: http://diablo1.se/notes/debug.html

[3]: https://github.com/diasurgical/devilution/issues/111#issuecomment-426059660

[4]: http://bytepointer.com/articles/the_microsoft_rich_header.htm

[5]: https://github.com/diasurgical/devilution/issues/111

[6]: https://github.com/diasurgical/devilution/issues/64

[7]: https://github.com/diasurgical/devilution/milestone/3

[8]: https://github.com/diasurgical/devilution-comparer

[9]: https://github.com/diasurgical/devilutionX

[10]: https://github.com/diasurgical/devilution/issues/171
