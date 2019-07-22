#pragma once

#include "devilution.h"

namespace dvl {
BOOL UiSelHeroDelYesNoDialog(BOOL (*fnremove)(_uiheroinfo *), _uiheroinfo *selectHero, bool isMultiplayer);
void selyesno_Free();
void selyesno_Select(int value);
void selyesno_Esc();

}
