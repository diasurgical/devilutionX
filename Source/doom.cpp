/**
 * @file doom.cpp
 *
 * Implementation of the map of the stars quest.
 */
#include "doom.h"

#include "control.h"
#include "engine.h"
#include "engine/clx_sprite.hpp"
#include "engine/load_cel.hpp"
#include "engine/render/clx_render.hpp"
#include "utils/stdcompat/optional.hpp"

namespace devilution {
namespace {
OptionalOwnedClxSpriteList DoomSprite;
} // namespace

bool DoomFlag;

void doom_init()
{
	DoomSprite = LoadCel("items\\map\\mapztown.cel", 640);
	DoomFlag = true;
}

void doom_close()
{
	DoomFlag = false;
	DoomSprite = std::nullopt;
}

void doom_draw(const Surface &out)
{
	if (!DoomFlag) {
		return;
	}

	ClxDraw(out, GetUIRectangle().position + Displacement { 0, 352 }, (*DoomSprite)[0]);
}

} // namespace devilution
