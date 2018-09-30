### Comments
- `BUGFIX` known bugs in original (vanilla) code
- `/* */` block comments are things to be fixed/checked
- `FIX_ME` bad data

### Known Bugs
Serious bugs (crash/fault)
- TBA

Minor bugs (noticeable but can be avoided)
- Server commands are broken and have been disabled `msgcmd.cpp`

Code issues (incorrect code that still works)
- Critical sections should be constructors using `CCritSect`
- Some code uses macros such as `__PAIR__` or `__ROL4__`
- Some functions/structures have incorrect signing (signed/unsigned BYTE)
- Function `GetLevelMTypes`, decompile and check `monster.cpp`
- Function `SetAutomapView`, decompile and check `automap.cpp`
- Function `engine_draw_automap_pixels`, decompile and check `engine.cpp`
- Double check `UseMana` references
- Double check `LOBYTE` of function `random(int, int)`
