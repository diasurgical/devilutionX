#pragma once

#include <vector>

#include "engine/point.hpp"
#include "engine/surface.hpp"
#include "spelldat.h"

namespace devilution {

struct SpellListItem {
	Point location;
	spell_type type;
	spell_id id;
	bool isSelected;
};

void DrawSpell(const Surface &out);
void DrawSpellList(const Surface &out);
std::vector<SpellListItem> GetSpellListItems();
void SetSpell();
void SetSpeedSpell(int slot);
void ToggleSpell(int slot);
void DoSpeedBook();

} // namespace devilution
