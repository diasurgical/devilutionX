#pragma once

#include <cstdint>

#include "utils/enum_traits.h"

namespace devilution {

enum class UiFlags : uint32_t {
	// clang-format off
	None               = 0,

	FontSize12         = 1 << 0,
	FontSize24         = 1 << 1,
	FontSize30         = 1 << 2,
	FontSize42         = 1 << 3,
	FontSize46         = 1 << 4,
	FontSizeDialog     = 1 << 5,

	ColorUiGold        = 1 << 6,
	ColorUiSilver      = 1 << 7,
	ColorUiGoldDark    = 1 << 8,
	ColorUiSilverDark  = 1 << 9,
	ColorDialogWhite   = 1 << 10,
	ColorYellow        = 1 << 11,
	ColorGold          = 1 << 12,
	ColorBlack         = 1 << 13,
	ColorWhite         = 1 << 14,
	ColorWhitegold     = 1 << 15,
	ColorRed           = 1 << 16,
	ColorBlue          = 1 << 17,
	ColorOrange        = 1 << 18,
	ColorButtonface    = 1 << 19,
	ColorButtonpushed  = 1 << 20,

	AlignCenter        = 1 << 21,
	AlignRight         = 1 << 22,
	VerticalCenter     = 1 << 23,

	KerningFitSpacing  = 1 << 24,

	ElementDisabled    = 1 << 25,
	ElementHidden      = 1 << 26,

	PentaCursor        = 1 << 27,
	TextCursor         = 1 << 28,
	Outlined           = 1 << 29,

	/** @brief Ensures that the if current element is active that the next element is also visible. */
	NeedsNextElement   = 1 << 30,
	// clang-format on
};
use_enum_as_flags(UiFlags);

} // namespace devilution
