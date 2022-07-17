/**
 * @file gmenu.h
 *
 * Interface of the in-game navigation and interaction.
 */
#pragma once

#include <cstdint>

#include "engine.h"

namespace devilution {

#define GMENU_SLIDER 0x40000000
#define GMENU_ENABLED 0x80000000

struct TMenuItem {
	uint32_t dwFlags;
	const char *pszStr;
	void (*fnMenu)(bool);
};

extern TMenuItem *sgpCurrentMenu;

void gmenu_draw_pause(const Surface &out);
void FreeGMenu();
void gmenu_init_menu();
bool gmenu_is_active();
void gmenu_set_items(TMenuItem *pItem, void (*gmFunc)());
void gmenu_draw(const Surface &out);
bool gmenu_presskeys(SDL_Keycode vkey);
bool gmenu_on_mouse_move();
bool gmenu_left_mouse(bool isDown);
void gmenu_enable(TMenuItem *pMenuItem, bool enable);

/**
 * @brief Set the TMenuItem slider position based on the given value
 */
void gmenu_slider_set(TMenuItem *pItem, int min, int max, int value);

/**
 * @brief Get the current value for the slider
 */
int gmenu_slider_get(TMenuItem *pItem, int min, int max);

/**
 * @brief Set the number of steps for the slider
 */
void gmenu_slider_steps(TMenuItem *pItem, int steps);

} // namespace devilution
