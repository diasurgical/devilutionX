#pragma once

#include <cstdint>

namespace dvl {

extern bool selhero_isMultiPlayer;
extern bool selhero_endMenu;

void selhero_Init();
void selhero_List_Init();
void selhero_List_Focus(int value);
void selhero_List_Select(int value);
bool selhero_List_DeleteYesNo();
void selhero_List_Esc();
void selhero_ClassSelector_Focus(int value);
void selhero_ClassSelector_Select(int value);
void selhero_ClassSelector_Esc();
void selhero_UiFocusNavigationYesNo();
void selhero_Name_Select(int value);
void selhero_Name_Esc();
void selhero_Load_Focus(int value);
void selhero_Load_Select(int value);

} // namespace dvl
