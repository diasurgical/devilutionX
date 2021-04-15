/**
 * @file town.h
 *
 * Interface of functionality for rendering the town, towners and calling other render routines.
 */
#pragma once

#include "gendung.h"
#include "interfac.h"

namespace devilution {

void TownOpenHive();
void TownOpenGrave();
void CreateTown(lvl_entry entry);

}
