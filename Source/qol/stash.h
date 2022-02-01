/**
 * @file qol/stash.h
 *
 * Interface of player stash.
 */
#pragma once

#include <cstdint>
#include <map>
#include <vector>

#include "engine/point.hpp"
#include "items.h"

namespace devilution {

struct StashStruct {
	void RemoveStashItem(uint16_t iv);
	std::map<int, std::array<std::array<uint16_t, 10>, 10>> stashGrids;
	std::vector<Item> stashList;
	int gold;
	/** Current Page */
	int page;
	bool dirty = false;
};

constexpr Point InvalidStashPoint { -1, -1 };

extern bool IsStashOpen;
extern StashStruct Stash;

extern bool IsWithdrawGoldOpen;
extern int WithdrawGoldValue;

Point GetStashSlotCoord(Point slot);
void InitStash();
void FreeStashGFX();
/**
 * @brief Render the inventory panel to the given buffer.
 */
void DrawStash(const Surface &out);
void CheckStashItem(Point mousePosition, bool isShiftHeld = false, bool isCtrlHeld = false);
bool UseStashItem(uint16_t cii);
uint16_t CheckStashHLight(Point mousePosition);
void CheckStashButtonRelease(Point mousePosition);
void CheckStashButtonPress(Point mousePosition);

void WithdrawGoldKeyPress(char vkey);
void DrawGoldWithdraw(const Surface &out, int amount);
void CloseGoldWithdraw();
void GoldWithdrawNewText(string_view text);

} // namespace devilution
