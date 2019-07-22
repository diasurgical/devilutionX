#include "diablo.h"

DEVILUTION_BEGIN_NAMESPACE

BYTE *optbar_cel;
BOOLEAN mouseNavigation;
BYTE *PentSpin_cel;
TMenuItem *sgpCurrItem;
BYTE *BigTGold_cel;
int PentSpin_tick;
BYTE PentSpin_frame;
void (*dword_63447C)(TMenuItem *);
TMenuItem *sgpCurrentMenu;
BYTE *option_cel;
BYTE *sgpLogo;
int sgCurrentMenuIdx;

const BYTE lfontframe[127] = {
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
	24, 25, 26, 20, 0, 21, 0
};
const BYTE lfontkern[56] = {
	18, 33, 21, 26, 28, 19, 19, 26, 25, 11,
	12, 25, 19, 34, 28, 32, 20, 32, 28, 20,
	28, 36, 35, 46, 33, 33, 24, 11, 23, 22,
	22, 21, 22, 21, 21, 21, 32, 10, 20, 36,
	31, 17, 13, 12, 13, 18, 16, 11, 20, 21,
	11, 10, 12, 11, 21, 23
};

void gmenu_draw_pause()
{
	if (currlevel != 0)
		RedBack();
	if (!sgpCurrentMenu) {
		light_table_index = 0;
		gmenu_print_text(316, 336, "Pause");
	}
}

void gmenu_print_text(int x, int y, char *pszStr)
{
	BYTE c;

	while (*pszStr) {
		c = gbFontTransTbl[(BYTE)*pszStr++];
		c = lfontframe[c];
		if (c)
			CelDecodeLightOnly(x, y, BigTGold_cel, c, 46);
		x += lfontkern[c] + 2;
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
	PentSpin_frame = 1;
	sgpCurrentMenu = 0;
	sgpCurrItem = 0;
	dword_63447C = 0;
	sgCurrentMenuIdx = 0;
	mouseNavigation = FALSE;
	sgpLogo = LoadFileInMem("Data\\Diabsmal.CEL", NULL);
	BigTGold_cel = LoadFileInMem("Data\\BigTGold.CEL", NULL);
	PentSpin_cel = LoadFileInMem("Data\\PentSpin.CEL", NULL);
	option_cel = LoadFileInMem("Data\\option.CEL", NULL);
	optbar_cel = LoadFileInMem("Data\\optbar.CEL", NULL);
}

BOOL gmenu_exception()
{
	return sgpCurrentMenu != 0;
}

void gmenu_call_proc(TMenuItem *pItem, void (*gmFunc)(TMenuItem *))
{
	int i;

	PauseMode = 0;
	mouseNavigation = FALSE;
	sgpCurrentMenu = pItem;
	dword_63447C = gmFunc;
	if (gmFunc) {
		dword_63447C(sgpCurrentMenu);
		pItem = sgpCurrentMenu;
	}
	sgCurrentMenuIdx = 0;
	if (sgpCurrentMenu) {
		for (i = 0; sgpCurrentMenu[i].fnMenu; i++) {
			sgCurrentMenuIdx++;
		}
	}
	sgpCurrItem = &sgpCurrentMenu[sgCurrentMenuIdx - 1];
	gmenu_up_down(TRUE);
}

void gmenu_up_down(BOOL isDown)
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

void gmenu_draw()
{
	int y;
	TMenuItem *i;
	DWORD ticks;

	if (sgpCurrentMenu) {
		if (dword_63447C)
			dword_63447C(sgpCurrentMenu);
		CelDecodeOnly((SCREEN_WIDTH - 296) / 2 + SCREEN_X, 102 + SCREEN_Y, sgpLogo, 1, 296);
		y = 160 + SCREEN_Y;
		i = sgpCurrentMenu;
		if (sgpCurrentMenu->fnMenu) {
			while (i->fnMenu) {
				gmenu_draw_menu_item(i, y);
				i++;
				y += 45;
			}
		}

		ticks = GetTickCount();
		if ((int)(ticks - PentSpin_tick) > 25) {
			PentSpin_frame++;
			if (PentSpin_frame == 9)
				PentSpin_frame = 1;
			PentSpin_tick = ticks;
		}
	}
}

void gmenu_draw_menu_item(TMenuItem *pItem, int y)
{
	DWORD x, w, nSteps, step, pos, t;
	t = y - 2;
	w = gmenu_get_lfont(pItem);
	if (pItem->dwFlags & GMENU_SLIDER) {
		x = 16 + w / 2 + SCREEN_X;
		CelDecodeOnly(x, t - 8, optbar_cel, 1, 287);
		step = pItem->dwFlags & 0xFFF;
		nSteps = (pItem->dwFlags & 0xFFF000) >> 12;
		if (nSteps < 2)
			nSteps = 2;
		pos = step * 256 / nSteps;
		gmenu_clear_buffer(x + 2, t - 10, pos + 13, 28);
		CelDecodeOnly(x + 2 + pos, y - 12, option_cel, 1, 27);
	}
	x = SCREEN_WIDTH / 2 - w / 2 + SCREEN_X;
	light_table_index = (pItem->dwFlags & GMENU_ENABLED) ? 0 : 15;
	gmenu_print_text(x, y, pItem->pszStr);
	if (pItem == sgpCurrItem) {
		CelDecodeOnly(x - 54, y + 1, PentSpin_cel, PentSpin_frame, 48);
		CelDecodeOnly(x + 4 + w, y + 1, PentSpin_cel, PentSpin_frame, 48);
	}
}

void gmenu_clear_buffer(int x, int y, int width, int height)
{
	BYTE *i;

	for (i = gpBuffer + PitchTbl[y] + x; height; height--) {
		memset(i, 205, width);
		i -= BUFFER_WIDTH;
	}
}

int gmenu_get_lfont(TMenuItem *pItem)
{
	char *text;
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

BOOL gmenu_presskeys(int vkey)
{
	if (!sgpCurrentMenu)
		return 0;
	switch (vkey) {
	case VK_RETURN:
		if ((sgpCurrItem->dwFlags & GMENU_ENABLED) != 0) {
			PlaySFX(IS_TITLEMOV);
			sgpCurrItem->fnMenu(TRUE);
		}
		break;
	case VK_ESCAPE:
		PlaySFX(IS_TITLEMOV);
		gmenu_call_proc(0, 0);
		break;
	case VK_SPACE:
		return FALSE;
	case VK_LEFT:
		gmenu_left_right(FALSE);
		break;
	case VK_RIGHT:
		gmenu_left_right(TRUE);
		break;
	case VK_UP:
		gmenu_up_down(FALSE);
		break;
	case VK_DOWN:
		gmenu_up_down(TRUE);
		break;
	}
	return TRUE;
}

void gmenu_left_right(BOOL isRight)
{
	int step;

	if (sgpCurrItem->dwFlags & GMENU_SLIDER) {
		step = sgpCurrItem->dwFlags & 0xFFF;
		if (isRight) {
			if (step == (int)(sgpCurrItem->dwFlags & 0xFFF000) >> 12)
				return;
			step++;
		} else {
			if (!step)
				return;
			step--;
		}
		sgpCurrItem->dwFlags &= 0xFFFFF000;
		sgpCurrItem->dwFlags |= step;
		sgpCurrItem->fnMenu(FALSE);
	}
}

BOOL gmenu_on_mouse_move()
{
	int step, nSteps;

	if (!mouseNavigation)
		return FALSE;
	gmenu_valid_mouse_pos(&step);
	nSteps = (int)(sgpCurrItem->dwFlags & 0xFFF000) >> 12;
	step *= nSteps;
	step /= 256;

	sgpCurrItem->dwFlags &= 0xFFFFF000;
	sgpCurrItem->dwFlags |= step;
	sgpCurrItem->fnMenu(FALSE);
	return TRUE;
}

BOOLEAN gmenu_valid_mouse_pos(int *plOffset)
{
	*plOffset = 282;
	if (MouseX < 282) {
		*plOffset = 0;
		return 0;
	}
	if (MouseX > 538) {
		*plOffset = 256;
		return 0;
	}
	*plOffset = MouseX - 282;
	return 1;
}

BOOL gmenu_left_mouse(BOOL isDown)
{
	BOOL result;
	TMenuItem *pItem;
	DWORD i, w;
	int dummy;

	if (!isDown) {
		if (mouseNavigation) {
			mouseNavigation = FALSE;
			result = TRUE;
		} else {
			result = FALSE;
		}

	} else {

		if (!sgpCurrentMenu) {
			return FALSE;
		}
		if (MouseY >= PANEL_TOP) {
			return FALSE;
		}
		if (MouseY - 117 >= 0) {
			i = (MouseY - 117) / 45;
			if (i < sgCurrentMenuIdx) {
				pItem = &sgpCurrentMenu[i];
				if ((sgpCurrentMenu[i].dwFlags & GMENU_ENABLED) != 0) {
					w = gmenu_get_lfont(pItem);
					if (MouseX >= SCREEN_WIDTH / 2 - w / 2) {
						if (MouseX <= SCREEN_WIDTH / 2 + w / 2) {
							sgpCurrItem = pItem;
							PlaySFX(IS_TITLEMOV);
							if (pItem->dwFlags & GMENU_SLIDER) {
								mouseNavigation = gmenu_valid_mouse_pos(&dummy);
								gmenu_on_mouse_move();
							} else {
								sgpCurrItem->fnMenu(TRUE);
							}
							result = TRUE;
						} else {
							result = TRUE;
						}
					} else {
						result = TRUE;
					}
				} else {
					result = TRUE;
				}
			} else {
				result = TRUE;
			}
		} else {
			result = TRUE;
		}
	}
	return result;
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

	/// ASSERT: assertassert(pItem, "gmenu.cpp", 445);
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
