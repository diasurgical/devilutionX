#include "diablo.h"

DEVILUTION_BEGIN_NAMESPACE

void *optbar_cel;
BOOLEAN mouseNavigation; // weak
void *PentSpin_cel;
TMenuItem *sgpCurrItem;
void *BigTGold_cel;
int PentSpin_tick; // weak
char PentSpin_frame; // weak
void(*dword_63447C)(TMenuItem *);
TMenuItem *sgpCurrentMenu; // idb
void *option_cel;
void *sgpLogo;
int sgCurrentMenuIdx; // weak

const unsigned char lfontframe[127] = {
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
const unsigned char lfontkern[56] = {
	18, 33, 21, 26, 28, 19, 19, 26, 25, 11,
	12, 25, 19, 34, 28, 32, 20, 32, 28, 20,
	28, 36, 35, 46, 33, 33, 24, 11, 23, 22,
	22, 21, 22, 21, 21, 21, 32, 10, 20, 36,
	31, 17, 13, 12, 13, 18, 16, 11, 20, 21,
	11, 10, 12, 11, 21, 23
};

void gmenu_draw_pause()
{
	if (currlevel)
		RedBack();
	if (!sgpCurrentMenu) {
		light_table_index = 0;
		gmenu_print_text(316, 336, "Pause");
	}
}
// 69BEF8: using guessed type int light_table_index;

void gmenu_print_text(int x, int y, char *pszStr)
{
	BYTE c;

	while (*pszStr) {
		c = gbFontTransTbl[(BYTE)*pszStr++];
		c = lfontframe[c];
		if (c)
			CelDecodeLightOnly(x, y, (BYTE *)BigTGold_cel, c, 46);
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
	mouseNavigation = 0;
	sgpLogo = LoadFileInMem("Data\\Diabsmal.CEL", 0);
	BigTGold_cel = LoadFileInMem("Data\\BigTGold.CEL", 0);
	PentSpin_cel = LoadFileInMem("Data\\PentSpin.CEL", 0);
	option_cel = LoadFileInMem("Data\\option.CEL", 0);
	optbar_cel = LoadFileInMem("Data\\optbar.CEL", 0);
}
// 634464: using guessed type char mouseNavigation;
// 634478: using guessed type char PentSpin_frame;
// 63448C: using guessed type int sgCurrentMenuIdx;

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
			if ((sgpCurrItem->dwFlags & 0x80000000) != 0) {
				if (i)
					PlaySFX(IS_TITLEMOV);
				return;
			}
		}
	}
}
// 634464: using guessed type char mouseNavigation;
// 63448C: using guessed type int sgCurrentMenuIdx;

void gmenu_draw()
{
	int y;
	TMenuItem *i;
	DWORD ticks;

	if (sgpCurrentMenu) {
		if (dword_63447C)
			dword_63447C(sgpCurrentMenu);
		CelDecodeOnly(236, 262, (BYTE *)sgpLogo, 1, 296);
		y = 320;
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

void gmenu_draw_menu_item(TMenuItem *pItem, int a2)
{
	int v2;           // edi
	TMenuItem *v3;    // ebx
	unsigned int v4;  // eax
	unsigned int v5;  // ebp
	int v6;           // esi
	unsigned int v7;  // ecx
	unsigned int v8;  // eax
	int v9;           // ecx
	unsigned int v10; // ebp
	int v11;          // esi
	int v12;          // eax
	int v13;          // edi
	unsigned int v14; // [esp+10h] [ebp-4h]

	v2 = a2;
	v3 = pItem;
	v4 = gmenu_get_lfont(pItem);
	v5 = v4;
	v14 = v4;
	if (v3->dwFlags & 0x40000000) {
		v6 = (v4 >> 1) + 80;
		CelDecodeOnly(v6, v2 - 10, (BYTE *)optbar_cel, 1, 287);
		v7 = (v3->dwFlags >> 12) & 0xFFF;
		if (v7 < 2)
			v7 = 2;
		v8 = ((v3->dwFlags & 0xFFF) << 8) / v7;
		v9 = (v5 >> 1) + 82;
		v10 = v8;
		gmenu_clear_buffer(v9, v2 - 12, v8 + 13, 28);
		CelDecodeOnly(v6 + v10 + 2, v2 - 12, (BYTE *)option_cel, 1, 27);
		v5 = v14;
	}
	v11 = 384 - (v5 >> 1);
	v12 = -((v3->dwFlags & 0x80000000) != 0);
	_LOBYTE(v12) = v12 & 0xF1;
	light_table_index = v12 + 15;
	gmenu_print_text(384 - (v5 >> 1), v2, v3->pszStr);
	if (v3 == sgpCurrItem) {
		v13 = v2 + 1;
		CelDecodeOnly(v11 - 54, v13, (BYTE *)PentSpin_cel, (unsigned char)PentSpin_frame, 48);
		CelDecodeOnly(v11 + v5 + 4, v13, (BYTE *)PentSpin_cel, (unsigned char)PentSpin_frame, 48);
	}
}
// 634478: using guessed type char PentSpin_frame;
// 69BEF8: using guessed type int light_table_index;

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

	if (pItem->dwFlags & 0x40000000)
		return 490;
	text = pItem->pszStr;
	i = 0;
	while (*text) {
		c = gbFontTransTbl[(BYTE)*text++];
		i += lfontkern[lfontframe[c]] + 2;
	}
	return i - 2;
}

BOOL gmenu_presskeys(int a1)
{
	if (!sgpCurrentMenu)
		return 0;
	switch (a1) {
	case VK_RETURN:
		if ((sgpCurrItem->dwFlags & 0x80000000) != 0) {
			PlaySFX(IS_TITLEMOV);
			sgpCurrItem->fnMenu(TRUE);
		}
		break;
	case VK_ESCAPE:
		PlaySFX(IS_TITLEMOV);
		gmenu_call_proc(0, 0);
		break;
	case VK_SPACE:
#ifdef SWITCH
		if ((sgpCurrItem->dwFlags & 0x80000000) != 0) {
			PlaySFX(IS_TITLEMOV);
			((void(__fastcall *)(signed int))sgpCurrItem->fnMenu)(1);
		}
		break;
#else
		return FALSE;
#endif
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
	int plOffset;

	if (sgpCurrItem->dwFlags & 0x40000000) {
		plOffset = sgpCurrItem->dwFlags & 0xFFF;
		if (isRight) {
			if (plOffset == (int)(sgpCurrItem->dwFlags & 0xFFF000) >> 12)
				return;
			plOffset++;
		} else {
			if (!plOffset)
				return;
			plOffset--;
		}
		sgpCurrItem->dwFlags &= 0xFFFFF000;
		sgpCurrItem->dwFlags |= plOffset;
		sgpCurrItem->fnMenu(FALSE);
	}
}

BOOL gmenu_on_mouse_move()
{
	int plOffset, v;

	if (!mouseNavigation)
		return FALSE;
	gmenu_valid_mouse_pos(&plOffset);
	v = (sgpCurrItem->dwFlags & 0xFFF000);
	v >>= 12;
	plOffset *= v;
	plOffset /= 256;

	sgpCurrItem->dwFlags &= 0xFFFFF000;
	sgpCurrItem->dwFlags |= plOffset;
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
	TMenuItem *v3;
	DWORD v4, v5;
	int dummy;

	if (!isDown) {
		if (mouseNavigation) {
			mouseNavigation = 0;
			result = TRUE;
		} else {
			result = FALSE;
		}

	} else {

		if (!sgpCurrentMenu) {
			return FALSE;
		}
		if (MouseY >= VIEWPORT_HEIGHT) {
			return FALSE;
		}
		if (MouseY - 117 >= 0) {
			v4 = (MouseY - 117) / 45;
			//v4 /= 45;
			if (v4 < sgCurrentMenuIdx) {
				v3 = &sgpCurrentMenu[v4];
				if ((sgpCurrentMenu[v4].dwFlags & 0x80000000) != 0) {
					v5 = gmenu_get_lfont(v3);
					if (MouseX >= 320 - (v5 >> 1)) {
						if (MouseX <= (v5 >> 1) + 320) {
							sgpCurrItem = v3;
							PlaySFX(IS_TITLEMOV);
							if (v3->dwFlags & 0x40000000) {
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
		pMenuItem->dwFlags |= 0x80000000;
	else
		pMenuItem->dwFlags &= ~0x80000000;
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
	nSteps = pItem->dwFlags;
	nSteps >>= 12;
	nSteps &= 0xFFF;
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
