/**
 * @file gmenu.h
 *
 * Interface of the in-game navigation and interaction.
 */
#pragma once

#include "engine.h"

namespace devilution {

struct TMenuItem {
	Uint32 dwFlags;
	const char *pszStr;
	void (*fnMenu)(bool);
};

extern TMenuItem *sgpCurrentMenu;

void gmenu_draw_pause(CelOutputBuffer out);
void FreeGMenu();
void gmenu_init_menu();
bool gmenu_is_active();
void gmenu_set_items(TMenuItem *pItem, void (*gmFunc)(TMenuItem *));
void gmenu_draw(CelOutputBuffer out);
bool gmenu_presskeys(int vkey);
bool gmenu_on_mouse_move();
bool gmenu_left_mouse(bool isDown);
void gmenu_enable(TMenuItem *pMenuItem, bool enable);
void gmenu_slider_set(TMenuItem *pItem, int min, int max, int gamma);
int gmenu_slider_get(TMenuItem *pItem, int min, int max);
void gmenu_slider_steps(TMenuItem *pItem, int dwTicks);

}
