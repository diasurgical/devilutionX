/**
 * @file gmenu.cpp
 *
 * Implementation of the in-game navigation and interaction.
 */
#include "all.h"

#include "../SourceX/controls/axis_direction.h"
#include "../SourceX/controls/controller_motion.h"

DEVILUTION_BEGIN_NAMESPACE

BYTE *optbar_cel;
BOOLEAN mouseNavigation;
BYTE *PentSpin_cel;
TMenuItem *sgpCurrItem;
BYTE *BigTGold_cel;
int LogoAnim_tick;
BYTE LogoAnim_frame;
int PentSpin_tick;
void (*gmenu_current_option)(TMenuItem *);
TMenuItem *sgpCurrentMenu;
BYTE *option_cel;
BYTE *sgpLogo;
int sgCurrentMenuIdx;

/** Maps from font index to bigtgold.cel frame number. */
const BYTE lfontframe[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 37, 49, 38, 0, 39, 40, 47,
	42, 43, 41, 45, 52, 44, 53, 55, 36, 27,
	28, 29, 30, 31, 32, 33, 34, 35, 51, 50,
	0, 46, 0, 54, 0, 1, 2, 3, 4, 5,
	6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
	26, 42, 0, 43, 0, 0, 0, 1, 2, 3,
	4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
	14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
	24, 25, 26, 20, 0, 21, 0, 0
};

/** Maps from bigtgold.cel frame number to character width. */
const BYTE lfontkern[] = {
	18, 33, 21, 26, 28, 19, 19, 26, 25, 11,
	12, 25, 19, 34, 28, 32, 20, 32, 28, 20,
	28, 36, 35, 46, 33, 33, 24, 11, 23, 22,
	22, 21, 22, 21, 21, 21, 32, 10, 20, 36,
	31, 17, 13, 12, 13, 18, 16, 11, 20, 21,
	11, 10, 12, 11, 21, 23
};

static void gmenu_print_text(CelOutputBuffer out, int x, int y, const char *pszStr)
{
	BYTE c;

	while (*pszStr) {
		c = gbFontTransTbl[(BYTE)*pszStr++];
		c = lfontframe[c];
		if (c != 0)
			CelDrawLightTo(out, x, y, BigTGold_cel, c, 46, NULL);
		x += lfontkern[c] + 2;
	}
}

void gmenu_draw_pause(CelOutputBuffer out)
{
	if (currlevel != 0)
		RedBack(out);
	if (!sgpCurrentMenu) {
		light_table_index = 0;
		gmenu_print_text(out, 252 + PANEL_LEFT, 176, "Pause");
	}
}

void FreeGMenu()
{
	MemFreeDbg(sgpLogo);
	MemFreeDbg(BigTGold_cel);
	MemFreeDbg(PentSpin_cel);
	MemFreeDbg(option_cel);
	MemFreeDbg(optbar_cel);
}

void gmenu_init_menu()
{
	LogoAnim_frame = 1;
	sgpCurrentMenu = NULL;
	sgpCurrItem = NULL;
	gmenu_current_option = NULL;
	sgCurrentMenuIdx = 0;
	mouseNavigation = FALSE;
	if (gbIsHellfire)
		sgpLogo = LoadFileInMem("Data\\hf_logo3.CEL", NULL);
	else
		sgpLogo = LoadFileInMem("Data\\Diabsmal.CEL", NULL);
	BigTGold_cel = LoadFileInMem("Data\\BigTGold.CEL", NULL);
	PentSpin_cel = LoadFileInMem("Data\\PentSpin.CEL", NULL);
	option_cel = LoadFileInMem("Data\\option.CEL", NULL);
	optbar_cel = LoadFileInMem("Data\\optbar.CEL", NULL);
}

BOOL gmenu_is_active()
{
	return sgpCurrentMenu != NULL;
}

static void gmenu_up_down(BOOL isDown)
{
	int i;

	if (!sgpCurrItem) {
		return;
	}
	mouseNavigation = FALSE;
	i = sgCurrentMenuIdx;
	if (sgCurrentMenuIdx) {
		while (i) {
			i--;
			if (isDown) {
				sgpCurrItem++;
				if (!sgpCurrItem->fnMenu)
					sgpCurrItem = &sgpCurrentMenu[0];
			} else {
				if (sgpCurrItem == sgpCurrentMenu)
					sgpCurrItem = &sgpCurrentMenu[sgCurrentMenuIdx];
				sgpCurrItem--;
			}
			if ((sgpCurrItem->dwFlags & GMENU_ENABLED) != 0) {
				if (i)
					PlaySFX(IS_TITLEMOV);
				return;
			}
		}
	}
}

static void gmenu_left_right(BOOL isRight)
{
	int step, steps;

	if (!(sgpCurrItem->dwFlags & GMENU_SLIDER))
		return;

	step = sgpCurrItem->dwFlags & 0xFFF;
	steps = (int)(sgpCurrItem->dwFlags & 0xFFF000) >> 12;
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
	sgpCurrItem->fnMenu(FALSE);
}

void gmenu_set_items(TMenuItem *pItem, void (*gmFunc)(TMenuItem *))
{
	int i;

	PauseMode = 0;
	mouseNavigation = FALSE;
	sgpCurrentMenu = pItem;
	gmenu_current_option = gmFunc;
	if (gmFunc) {
		gmenu_current_option(sgpCurrentMenu);
		pItem = sgpCurrentMenu;
	}
	sgCurrentMenuIdx = 0;
	if (sgpCurrentMenu) {
		for (i = 0; sgpCurrentMenu[i].fnMenu; i++) {
			sgCurrentMenuIdx++;
		}
	}
	// BUGFIX: OOB access when sgCurrentMenuIdx is 0; should be set to NULL instead. (fixed)
	sgpCurrItem = sgCurrentMenuIdx > 0 ? &sgpCurrentMenu[sgCurrentMenuIdx - 1] : NULL;
	gmenu_up_down(TRUE);
}

static void gmenu_clear_buffer(CelOutputBuffer out, int x, int y, int width, int height)
{
	BYTE *i = out.at(x, y);
	while (height--) {
		memset(i, 205, width);
		i -= out.pitch();
	}
}

static int gmenu_get_lfont(TMenuItem *pItem)
{
	const char *text;
	int i;
	BYTE c;

	if (pItem->dwFlags & GMENU_SLIDER)
		return 490;
	text = pItem->pszStr;
	i = 0;
	while (*text) {
		c = gbFontTransTbl[(BYTE)*text++];
		i += lfontkern[lfontframe[c]] + 2;
	}
	return i - 2;
}

static void gmenu_draw_menu_item(CelOutputBuffer out, TMenuItem *pItem, int y)
{
	DWORD w, x, nSteps, step, pos;
	w = gmenu_get_lfont(pItem);
	if (pItem->dwFlags & GMENU_SLIDER) {
		x = 16 + w / 2;
		CelDrawTo(out, x + PANEL_LEFT, y - 10, optbar_cel, 1, 287);
		step = pItem->dwFlags & 0xFFF;
		nSteps = (pItem->dwFlags & 0xFFF000) >> 12;
		if (nSteps < 2)
			nSteps = 2;
		pos = step * 256 / nSteps;
		gmenu_clear_buffer(out, x + 2 + PANEL_LEFT, y - 12, pos + 13, 28);
		CelDrawTo(out, x + 2 + pos + PANEL_LEFT, y - 12, option_cel, 1, 27);
	}
	x = gnScreenWidth / 2 - w / 2;
	light_table_index = (pItem->dwFlags & GMENU_ENABLED) ? 0 : 15;
	gmenu_print_text(out, x, y, pItem->pszStr);
	if (pItem == sgpCurrItem) {
		CelDrawTo(out, x - 54, y + 1, PentSpin_cel, PentSpn2Spin(), 48);
		CelDrawTo(out, x + 4 + w, y + 1, PentSpin_cel, PentSpn2Spin(), 48);
	}
}

static void GameMenuMove()
{
	static AxisDirectionRepeater repeater;
	const AxisDirection move_dir = repeater.Get(GetLeftStickOrDpadDirection());
	if (move_dir.x != AxisDirectionX_NONE)
		gmenu_left_right(move_dir.x == AxisDirectionX_RIGHT);
	if (move_dir.y != AxisDirectionY_NONE)
		gmenu_up_down(move_dir.y == AxisDirectionY_DOWN);
}

void gmenu_draw(CelOutputBuffer out)
{
	int y;
	TMenuItem *i;
	DWORD ticks;

	if (sgpCurrentMenu) {
		GameMenuMove();
		if (gmenu_current_option)
			gmenu_current_option(sgpCurrentMenu);
		if (gbIsHellfire) {
			ticks = SDL_GetTicks();
			if ((int)(ticks - LogoAnim_tick) > 25) {
				LogoAnim_frame++;
				if (LogoAnim_frame > 16)
					LogoAnim_frame = 1;
				LogoAnim_tick = ticks;
			}
			CelDrawTo(out, (gnScreenWidth - 430) / 2, 102 + UI_OFFSET_Y, sgpLogo, LogoAnim_frame, 430);
		} else {
			CelDrawTo(out, (gnScreenWidth - 296) / 2, 102 + UI_OFFSET_Y, sgpLogo, 1, 296);
		}
		y = 160 + UI_OFFSET_Y;
		i = sgpCurrentMenu;
		if (sgpCurrentMenu->fnMenu) {
			while (i->fnMenu) {
				gmenu_draw_menu_item(out, i, y);
				i++;
				y += 45;
			}
		}
	}
}

BOOL gmenu_presskeys(int vkey)
{
	if (!sgpCurrentMenu)
		return FALSE;
	switch (vkey) {
	case DVL_VK_RETURN:
		if ((sgpCurrItem->dwFlags & GMENU_ENABLED) != 0) {
			PlaySFX(IS_TITLEMOV);
			sgpCurrItem->fnMenu(TRUE);
		}
		break;
	case DVL_VK_ESCAPE:
		PlaySFX(IS_TITLEMOV);
		gmenu_set_items(NULL, NULL);
		break;
	case DVL_VK_SPACE:
		return FALSE;
	case DVL_VK_LEFT:
		gmenu_left_right(FALSE);
		break;
	case DVL_VK_RIGHT:
		gmenu_left_right(TRUE);
		break;
	case DVL_VK_UP:
		gmenu_up_down(FALSE);
		break;
	case DVL_VK_DOWN:
		gmenu_up_down(TRUE);
		break;
	}
	return TRUE;
}

static BOOLEAN gmenu_get_mouse_slider(int *plOffset)
{
	*plOffset = 282;
	if (MouseX < 282 + PANEL_LEFT) {
		*plOffset = 0;
		return FALSE;
	}
	if (MouseX > 538 + PANEL_LEFT) {
		*plOffset = 256;
		return FALSE;
	}
	*plOffset = MouseX - 282 - PANEL_LEFT;
	return TRUE;
}

BOOL gmenu_on_mouse_move()
{
	int step, nSteps;

	if (!mouseNavigation)
		return FALSE;
	gmenu_get_mouse_slider(&step);
	nSteps = (int)(sgpCurrItem->dwFlags & 0xFFF000) >> 12;
	step *= nSteps;
	step /= 256;

	sgpCurrItem->dwFlags &= 0xFFFFF000;
	sgpCurrItem->dwFlags |= step;
	sgpCurrItem->fnMenu(FALSE);
	return TRUE;
}

BOOL gmenu_left_mouse(BOOL isDown)
{
	TMenuItem *pItem;
	int i, w, dummy;

	if (!isDown) {
		if (mouseNavigation) {
			mouseNavigation = FALSE;
			return TRUE;
		} else {
			return FALSE;
		}
	}

	if (!sgpCurrentMenu) {
		return FALSE;
	}
	if (MouseY >= PANEL_TOP) {
		return FALSE;
	}
	if (MouseY - (117 + UI_OFFSET_Y) < 0) {
		return TRUE;
	}
	i = (MouseY - (117 + UI_OFFSET_Y)) / 45;
	if (i >= sgCurrentMenuIdx) {
		return TRUE;
	}
	pItem = &sgpCurrentMenu[i];
	if (!(sgpCurrentMenu[i].dwFlags & GMENU_ENABLED)) {
		return TRUE;
	}
	w = gmenu_get_lfont(pItem);
	if (MouseX < gnScreenWidth / 2 - w / 2) {
		return TRUE;
	}
	if (MouseX > gnScreenWidth / 2 + w / 2) {
		return TRUE;
	}
	sgpCurrItem = pItem;
	PlaySFX(IS_TITLEMOV);
	if (pItem->dwFlags & GMENU_SLIDER) {
		mouseNavigation = gmenu_get_mouse_slider(&dummy);
		gmenu_on_mouse_move();
	} else {
		sgpCurrItem->fnMenu(TRUE);
	}
	return TRUE;
}

void gmenu_enable(TMenuItem *pMenuItem, BOOL enable)
{
	if (enable)
		pMenuItem->dwFlags |= GMENU_ENABLED;
	else
		pMenuItem->dwFlags &= ~GMENU_ENABLED;
}

/**
 * @brief Set the TMenuItem slider position based on the given value
 */
void gmenu_slider_set(TMenuItem *pItem, int min, int max, int value)
{
	int nSteps;

	assert(pItem);
	nSteps = (int)(pItem->dwFlags & 0xFFF000) >> 12;
	if (nSteps < 2)
		nSteps = 2;
	pItem->dwFlags &= 0xFFFFF000;
	pItem->dwFlags |= ((max - min - 1) / 2 + (value - min) * nSteps) / (max - min);
}

/**
 * @brief Get the current value for the slider
 */
int gmenu_slider_get(TMenuItem *pItem, int min, int max)
{
	int nSteps, step;

	step = pItem->dwFlags & 0xFFF;
	nSteps = (int)(pItem->dwFlags & 0xFFF000) >> 12;
	if (nSteps < 2)
		nSteps = 2;
	return min + (step * (max - min) + (nSteps - 1) / 2) / nSteps;
}

/**
 * @brief Set the number of steps for the slider
 */
void gmenu_slider_steps(TMenuItem *pItem, int steps)
{
	pItem->dwFlags &= 0xFF000FFF;
	pItem->dwFlags |= (steps << 12) & 0xFFF000;
}

DEVILUTION_END_NAMESPACE
