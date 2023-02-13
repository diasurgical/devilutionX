#pragma once

#include "levels/gendung.h"

namespace devilution {

extern _difficulty nDifficulty;

void selgame_GameSelection_Init();
void selgame_GameSelection_Focus(int value);
void selgame_GameSelection_Select(int value);
void selgame_GameSelection_Esc();
void selgame_Diff_Focus(int value);
void selgame_Diff_Select(int value);
void selgame_Diff_Esc();
void selgame_GameSpeedSelection();
void selgame_Speed_Focus(int value);
void selgame_Speed_Select(int value);
void selgame_Speed_Esc();
void selgame_Password_Init(int value);
void selgame_Password_Select(int value);
void selgame_Password_Esc();

} // namespace devilution
