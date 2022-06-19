/**
 * @file gmenu.cpp
 *
 * Implementation of the in-game navigation and interaction.
 */
#include "gmenu.h"

#include "DiabloUI/ui_flags.hpp"
#include "control.h"
#include "controls/axis_direction.h"
#include "controls/controller_motion.h"
#include "engine.h"
#include "engine/cel_sprite.hpp"
#include "engine/load_cel.hpp"
#include "engine/render/cel_render.hpp"
#include "engine/render/text_render.hpp"
#include "options.h"
#include "stores.h"
#include "utils/language.h"
#include "utils/stdcompat/optional.hpp"
#include "utils/ui_fwd.h"

namespace devilution {

namespace {

std::optional<OwnedCelSprite> optbar_cel;
std::optional<OwnedCelSprite> PentSpin_cel;
std::optional<OwnedCelSprite> option_cel;
std::optional<OwnedCelSprite> sgpLogo;
bool mouseNavigation;
TMenuItem *sgpCurrItem;
int LogoAnim_tick;
BYTE LogoAnim_frame;
void (*gmenu_current_option)();
int sgCurrentMenuIdx;

void GmenuUpDown(bool isDown)
{
	if (sgpCurrItem == nullptr) {
		return;
	}
	mouseNavigation = false;
	int i = sgCurrentMenuIdx;
	if (sgCurrentMenuIdx != 0) {
		while (i != 0) {
			i--;
			if (isDown) {
				sgpCurrItem++;
				if (sgpCurrItem->fnMenu == nullptr)
					sgpCurrItem = &sgpCurrentMenu[0];
			} else {
				if (sgpCurrItem == sgpCurrentMenu)
					sgpCurrItem = &sgpCurrentMenu[sgCurrentMenuIdx];
				sgpCurrItem--;
			}
			if ((sgpCurrItem->dwFlags & GMENU_ENABLED) != 0) {
				if (i != 0)
					PlaySFX(IS_TITLEMOV);
				return;
			}
		}
	}
}

void GmenuLeftRight(bool isRight)
{
	if ((sgpCurrItem->dwFlags & GMENU_SLIDER) == 0)
		return;

	uint16_t step = sgpCurrItem->dwFlags & 0xFFF;
	uint16_t steps = (sgpCurrItem->dwFlags & 0xFFF000) >> 12;
	if (isRight) {
		if (step == steps)
			return;
		step++;
	} else {
		if (step == 0)
			return;
		step--;
	}
	sgpCurrItem->dwFlags &= 0xFFFFF000;
	sgpCurrItem->dwFlags |= step;
	sgpCurrItem->fnMenu(false);
}

void GmenuClearBuffer(const Surface &out, int x, int y, int width, int height)
{
	BYTE *i = out.at(x, y);
	while ((height--) != 0) {
		memset(i, 205, width);
		i -= out.pitch();
	}
}

int GmenuGetLineWidth(TMenuItem *pItem)
{
	if ((pItem->dwFlags & GMENU_SLIDER) != 0)
		return 490;

	return GetLineWidth(_(pItem->pszStr), GameFont46, 2);
}

void GmenuDrawMenuItem(const Surface &out, TMenuItem *pItem, int y)
{
	int w = GmenuGetLineWidth(pItem);
	if ((pItem->dwFlags & GMENU_SLIDER) != 0) {
		int uiPositionX = GetUIRectangle().position.x;
		int x = 16 + w / 2;
		CelDrawTo(out, { x + uiPositionX, y + 40 }, CelSprite { *optbar_cel }, 0);
		uint16_t step = pItem->dwFlags & 0xFFF;
		uint16_t steps = std::max<uint16_t>((pItem->dwFlags & 0xFFF000) >> 12, 2);
		uint16_t pos = step * 256 / steps;
		GmenuClearBuffer(out, x + 2 + uiPositionX, y + 38, pos + 13, 28);
		CelDrawTo(out, { x + 2 + pos + uiPositionX, y + 38 }, CelSprite { *option_cel }, 0);
	}

	int x = (gnScreenWidth - w) / 2;
	UiFlags style = (pItem->dwFlags & GMENU_ENABLED) != 0 ? UiFlags::ColorGold : UiFlags::ColorBlack;
	DrawString(out, _(pItem->pszStr), Point { x, y }, style | UiFlags::FontSize46, 2);
	if (pItem == sgpCurrItem) {
		CelSprite sprite { *PentSpin_cel };
		CelDrawTo(out, { x - 54, y + 51 }, sprite, PentSpn2Spin());
		CelDrawTo(out, { x + 4 + w, y + 51 }, sprite, PentSpn2Spin());
	}
}

void GameMenuMove()
{
	static AxisDirectionRepeater repeater;
	const AxisDirection moveDir = repeater.Get(GetLeftStickOrDpadDirection());
	if (moveDir.x != AxisDirectionX_NONE)
		GmenuLeftRight(moveDir.x == AxisDirectionX_RIGHT);
	if (moveDir.y != AxisDirectionY_NONE)
		GmenuUpDown(moveDir.y == AxisDirectionY_DOWN);
}

bool GmenuMouseNavigation()
{
	int uiPositionX = GetUIRectangle().position.x;
	if (MousePosition.x < 282 + uiPositionX) {
		return false;
	}
	if (MousePosition.x > 538 + uiPositionX) {
		return false;
	}
	return true;
}

int GmenuGetMouseSlider()
{
	int uiPositionX = GetUIRectangle().position.x;
	if (MousePosition.x < 282 + uiPositionX) {
		return 0;
	}
	if (MousePosition.x > 538 + uiPositionX) {
		return 256;
	}
	return MousePosition.x - 282 - uiPositionX;
}

} // namespace

TMenuItem *sgpCurrentMenu;

void gmenu_draw_pause(const Surface &out)
{
	if (leveltype != DTYPE_TOWN)
		RedBack(out);
	if (sgpCurrentMenu == nullptr) {
		LightTableIndex = 0;
		DrawString(out, _("Pause"), { { 0, 0 }, { gnScreenWidth, GetMainPanel().position.y } }, UiFlags::FontSize46 | UiFlags::ColorGold | UiFlags::AlignCenter | UiFlags::VerticalCenter, 2);
	}
}

void FreeGMenu()
{
	sgpLogo = std::nullopt;
	PentSpin_cel = std::nullopt;
	option_cel = std::nullopt;
	optbar_cel = std::nullopt;
}

void gmenu_init_menu()
{
	LogoAnim_frame = 0;
	sgpCurrentMenu = nullptr;
	sgpCurrItem = nullptr;
	gmenu_current_option = nullptr;
	sgCurrentMenuIdx = 0;
	mouseNavigation = false;
	if (gbIsHellfire)
		sgpLogo = LoadCel("Data\\hf_logo3.CEL", 430);
	else
		sgpLogo = LoadCel("Data\\Diabsmal.CEL", 296);
	PentSpin_cel = LoadCel("Data\\PentSpin.CEL", 48);
	option_cel = LoadCel("Data\\option.CEL", 27);
	optbar_cel = LoadCel("Data\\optbar.CEL", 287);
}

bool gmenu_is_active()
{
	return sgpCurrentMenu != nullptr;
}

void gmenu_set_items(TMenuItem *pItem, void (*gmFunc)())
{
	PauseMode = 0;
	mouseNavigation = false;
	sgpCurrentMenu = pItem;
	gmenu_current_option = gmFunc;
	if (gmenu_current_option != nullptr) {
		gmenu_current_option();
	}
	sgCurrentMenuIdx = 0;
	if (sgpCurrentMenu != nullptr) {
		for (int i = 0; sgpCurrentMenu[i].fnMenu != nullptr; i++) {
			sgCurrentMenuIdx++;
		}
	}
	// BUGFIX: OOB access when sgCurrentMenuIdx is 0; should be set to NULL instead. (fixed)
	sgpCurrItem = sgCurrentMenuIdx > 0 ? &sgpCurrentMenu[sgCurrentMenuIdx - 1] : nullptr;
	GmenuUpDown(true);
	if (sgpCurrentMenu == nullptr)
		SaveOptions();
}

void gmenu_draw(const Surface &out)
{
	if (sgpCurrentMenu != nullptr) {
		GameMenuMove();
		if (gmenu_current_option != nullptr)
			gmenu_current_option();
		if (gbIsHellfire) {
			const uint32_t ticks = SDL_GetTicks();
			if ((int)(ticks - LogoAnim_tick) > 25) {
				++LogoAnim_frame;
				if (LogoAnim_frame >= 16)
					LogoAnim_frame = 0;
				LogoAnim_tick = ticks;
			}
		}
		int uiPositionY = GetUIRectangle().position.y;
		CelSprite sprite { *sgpLogo };
		CelDrawTo(out, { (gnScreenWidth - sprite.Width()) / 2, 102 + uiPositionY }, sprite, LogoAnim_frame);
		int y = 110 + uiPositionY;
		TMenuItem *i = sgpCurrentMenu;
		if (sgpCurrentMenu->fnMenu != nullptr) {
			while (i->fnMenu != nullptr) {
				GmenuDrawMenuItem(out, i, y);
				i++;
				y += 45;
			}
		}
	}
}

bool gmenu_presskeys(int vkey)
{
	if (sgpCurrentMenu == nullptr)
		return false;
	switch (vkey) {
	case DVL_VK_RETURN:
		if ((sgpCurrItem->dwFlags & GMENU_ENABLED) != 0) {
			PlaySFX(IS_TITLEMOV);
			sgpCurrItem->fnMenu(true);
		}
		break;
	case DVL_VK_ESCAPE:
		PlaySFX(IS_TITLEMOV);
		gmenu_set_items(nullptr, nullptr);
		break;
	case DVL_VK_SPACE:
		return false;
	case DVL_VK_LEFT:
		GmenuLeftRight(false);
		break;
	case DVL_VK_RIGHT:
		GmenuLeftRight(true);
		break;
	case DVL_VK_UP:
		GmenuUpDown(false);
		break;
	case DVL_VK_DOWN:
		GmenuUpDown(true);
		break;
	}
	return true;
}

bool gmenu_on_mouse_move()
{
	if (!mouseNavigation)
		return false;

	uint16_t step = (sgpCurrItem->dwFlags & 0xFFF000) >> 12;
	step *= GmenuGetMouseSlider();
	step /= 256;

	sgpCurrItem->dwFlags &= 0xFFFFF000;
	sgpCurrItem->dwFlags |= step;
	sgpCurrItem->fnMenu(false);

	return true;
}

bool gmenu_left_mouse(bool isDown)
{
	if (!isDown) {
		if (mouseNavigation) {
			mouseNavigation = false;
			return true;
		}
		return false;
	}

	if (sgpCurrentMenu == nullptr) {
		return false;
	}
	const Point uiPosition = GetUIRectangle().position;
	if (MousePosition.y >= GetMainPanel().position.y) {
		return false;
	}
	if (MousePosition.y - (117 + uiPosition.y) < 0) {
		return true;
	}
	int i = (MousePosition.y - (117 + uiPosition.y)) / 45;
	if (i >= sgCurrentMenuIdx) {
		return true;
	}
	TMenuItem *pItem = &sgpCurrentMenu[i];
	if ((sgpCurrentMenu[i].dwFlags & GMENU_ENABLED) == 0) {
		return true;
	}
	int w = GmenuGetLineWidth(pItem);
	uint16_t screenWidth = GetScreenWidth();
	if (MousePosition.x < screenWidth / 2 - w / 2) {
		return true;
	}
	if (MousePosition.x > screenWidth / 2 + w / 2) {
		return true;
	}
	sgpCurrItem = pItem;
	PlaySFX(IS_TITLEMOV);
	if ((pItem->dwFlags & GMENU_SLIDER) != 0) {
		mouseNavigation = GmenuMouseNavigation();
		gmenu_on_mouse_move();
	} else {
		sgpCurrItem->fnMenu(true);
	}
	return true;
}

void gmenu_enable(TMenuItem *pMenuItem, bool enable)
{
	if (enable)
		pMenuItem->dwFlags |= GMENU_ENABLED;
	else
		pMenuItem->dwFlags &= ~GMENU_ENABLED;
}

void gmenu_slider_set(TMenuItem *pItem, int min, int max, int value)
{
	assert(pItem);
	uint16_t nSteps = std::max<uint16_t>((pItem->dwFlags & 0xFFF000) >> 12, 2);
	pItem->dwFlags &= 0xFFFFF000;
	pItem->dwFlags |= ((max - min - 1) / 2 + (value - min) * nSteps) / (max - min);
}

int gmenu_slider_get(TMenuItem *pItem, int min, int max)
{
	uint16_t step = pItem->dwFlags & 0xFFF;
	uint16_t steps = std::max<uint16_t>((pItem->dwFlags & 0xFFF000) >> 12, 2);
	return min + (step * (max - min) + (steps - 1) / 2) / steps;
}

void gmenu_slider_steps(TMenuItem *pItem, int steps)
{
	pItem->dwFlags &= 0xFF000FFF;
	pItem->dwFlags |= (steps << 12) & 0xFFF000;
}

} // namespace devilution
