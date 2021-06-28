/**
* @file xpbar.cpp
*
* Adds XP bar QoL feature
*/
#include "xpbar.h"

#include <array>

#include <fmt/format.h>

#include "DiabloUI/art_draw.h"
#include "common.h"
#include "control.h"
#include "engine/point.hpp"
#include "options.h"
#include "utils/language.h"

namespace devilution {

namespace {

constexpr int BAR_WIDTH = 307;

using ColorGradient = std::array<Uint8, 12>;
constexpr ColorGradient GOLD_GRADIENT = { 0xCF, 0xCE, 0xCD, 0xCC, 0xCB, 0xCA, 0xC9, 0xC8, 0xC7, 0xC6, 0xC5, 0xC4 };
constexpr ColorGradient SILVER_GRADIENT = { 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0xF9, 0xF8, 0xF7, 0xF6, 0xF5, 0xF4, 0xF3 };

constexpr int BACK_WIDTH = 313;
constexpr int BACK_HEIGHT = 9;

Art xpbarArt;

void DrawBar(const CelOutputBuffer &out, Point screenPosition, int width, const ColorGradient &gradient)
{
	UnsafeDrawHorizontalLine(out, screenPosition + Point { 0, 1 }, width, gradient[gradient.size() * 3 / 4 - 1]);
	UnsafeDrawHorizontalLine(out, screenPosition + Point { 0, 2 }, width, gradient[gradient.size() - 1]);
	UnsafeDrawHorizontalLine(out, screenPosition + Point { 0, 3 }, width, gradient[gradient.size() / 2 - 1]);
}

void DrawEndCap(const CelOutputBuffer &out, Point point, int idx, const ColorGradient &gradient)
{
	out.SetPixel({ point.x, point.y + 1 }, gradient[idx * 3 / 4]);
	out.SetPixel({ point.x, point.y + 2 }, gradient[idx]);
	out.SetPixel({ point.x, point.y + 3 }, gradient[idx / 2]);
}

} // namespace

void InitXPBar()
{
	if (sgOptions.Gameplay.bExperienceBar) {
		LoadMaskedArt("data\\xpbar.pcx", &xpbarArt, 1, 1);

		if (xpbarArt.surface == nullptr) {
			app_fatal("%s", _("Failed to load UI resources.\n"
			                  "\n"
			                  "Make sure devilutionx.mpq is in the game folder and that it is up to date."));
		}
	}
}

void FreeXPBar()
{
	xpbarArt.Unload();
}

void DrawXPBar(const CelOutputBuffer &out)
{
	if (!sgOptions.Gameplay.bExperienceBar)
		return;

	const auto &player = plr[myplr];

	const Point back = { PANEL_LEFT + PANEL_WIDTH / 2 - 155, PANEL_TOP + PANEL_HEIGHT - 11 };
	const Point position = back + Point { 3, 2 };

	DrawArt(out, back, &xpbarArt);

	const int charLevel = player._pLevel;

	if (charLevel == MAXCHARLEVEL - 1) {
		// Draw a nice golden bar for max level characters.
		DrawBar(out, position, BAR_WIDTH, GOLD_GRADIENT);

		return;
	}

	const int prevXp = ExpLvlsTbl[charLevel - 1];
	if (player._pExperience < prevXp)
		return;

	uint64_t prevXpDelta_1 = player._pExperience - prevXp;
	uint64_t prevXpDelta = ExpLvlsTbl[charLevel] - prevXp;
	uint64_t fullBar = BAR_WIDTH * prevXpDelta_1 / prevXpDelta;

	// Figure out how much to fill the last pixel of the XP bar, to make it gradually appear with gained XP
	uint64_t onePx = prevXpDelta / BAR_WIDTH + 1;
	uint64_t lastFullPx = fullBar * prevXpDelta / BAR_WIDTH;

	const uint64_t fade = (prevXpDelta_1 - lastFullPx) * (SILVER_GRADIENT.size() - 1) / onePx;

	// Draw beginning of bar full brightness
	DrawBar(out, position, fullBar, SILVER_GRADIENT);

	// End pixels appear gradually
	DrawEndCap(out, position + Point { static_cast<int>(fullBar), 0 }, fade, SILVER_GRADIENT);
}

bool CheckXPBarInfo()
{
	if (!sgOptions.Gameplay.bExperienceBar)
		return false;

	const int backX = PANEL_LEFT + PANEL_WIDTH / 2 - 155;
	const int backY = PANEL_TOP + PANEL_HEIGHT - 11;

	if (MousePosition.x < backX || MousePosition.x >= backX + BACK_WIDTH || MousePosition.y < backY || MousePosition.y >= backY + BACK_HEIGHT)
		return false;

	const auto &player = plr[myplr];

	const int charLevel = player._pLevel;

	strcpy(tempstr, fmt::format(_("Level {:d}"), charLevel).c_str());
	AddPanelString(tempstr);

	if (charLevel == MAXCHARLEVEL - 1) {
		// Show a maximum level indicator for max level players.
		infoclr = UIS_GOLD;

		strcpy(tempstr, _("Experience: "));
		PrintWithSeparator(tempstr + SDL_arraysize("Experience: ") - 1, ExpLvlsTbl[charLevel - 1]);
		AddPanelString(tempstr);

		AddPanelString(_("Maximum Level"));

		return true;
	}

	infoclr = UIS_SILVER;

	strcpy(tempstr, _("Experience: "));
	PrintWithSeparator(tempstr + SDL_arraysize("Experience: ") - 1, player._pExperience);
	AddPanelString(tempstr);

	strcpy(tempstr, _("Next Level: "));
	PrintWithSeparator(tempstr + SDL_arraysize("Next Level: ") - 1, ExpLvlsTbl[charLevel]);
	AddPanelString(tempstr);

	strcpy(PrintWithSeparator(tempstr, ExpLvlsTbl[charLevel] - player._pExperience), fmt::format(_(" to Level {:d}"), charLevel + 1).c_str());
	AddPanelString(tempstr);

	return true;
}

} // namespace devilution
