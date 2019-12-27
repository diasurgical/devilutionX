#pragma once

#include "devilution.h"

namespace dvl {
void UiSelHeroDelYesNoDialog(BOOL (*fnremove)(_uiheroinfo *), _uiheroinfo *selectHero, bool isMultiplayer);
void selyesno_Free();
void selyesno_Select(int value);
void selyesno_Esc();

}
