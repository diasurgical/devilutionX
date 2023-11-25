#pragma once

#include "levels/gendung.h"

namespace devilution {

extern _difficulty nDifficulty;

void selgame_GameSelection_Init();
void selgame_GameSelection_Focus(size_t value);
void selgame_GameSelection_Select(size_t value);
void selgame_GameSelection_Esc();
void selgame_Diff_Focus(size_t value);
void selgame_Diff_Select(size_t value);
void selgame_Diff_Esc();
void selgame_GameSpeedSelection();
void selgame_Speed_Focus(size_t value);
void selgame_Speed_Select(size_t value);
void selgame_Speed_Esc();
void selgame_Password_Init(size_t value);
void selgame_Password_Select(size_t value);
void selgame_Password_Esc();

} // namespace devilution
