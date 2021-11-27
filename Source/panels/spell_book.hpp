#pragma once

#include "engine/surface.hpp"

namespace devilution {

void InitSpellBook();
void FreeSpellBook();
void CheckSBook();
void DrawSpellBook(const Surface &out);

} // namespace devilution
