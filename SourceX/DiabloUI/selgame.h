#pragma once

namespace dvl {

struct _uiheroinfo;

void selgame_GameSelection_Init();
void selgame_GameSelection_Focus(int value);
void selgame_GameSelection_Select(int value);
void selgame_GameSelection_Esc();
bool IsSPDifficultyChosen();
void selgame_Diff_SP_Init(_uiheroinfo *pInfo);
void selgame_Diff_SP_Select(int value);
void selgame_Diff_SP_Esc();
void selgame_Diff_Focus(int value);
void selgame_Diff_Select(int value);
void selgame_Diff_Esc();
void selgame_Password_Init(int value);
void selgame_Password_Select(int value);
void selgame_Password_Esc();

}
