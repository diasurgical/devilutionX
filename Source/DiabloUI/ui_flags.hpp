#pragma once

#include "utils/enum_traits.h"

namespace devilution {

enum class UiFlags {
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
	ColorDialogYellow  = 1 << 11,
	ColorGold          = 1 << 12,
	ColorBlack         = 1 << 13,
	ColorWhite         = 1 << 14,
	ColorWhitegold     = 1 << 15,
	ColorRed           = 1 << 16,
	ColorBlue          = 1 << 17,
	ColorButtonface    = 1 << 18,
	ColorButtonpushed  = 1 << 19,

	AlignCenter        = 1 << 20,
	AlignRight         = 1 << 21,
	VerticalCenter     = 1 << 22,

	KerningFitSpacing  = 1 << 23,

	ElementDisabled    = 1 << 24,
	ElementHidden      = 1 << 25,

	PentaCursor        = 1 << 26,
	TextCursor         = 1 << 27,

	/** @brief Ensures that the if current element is active that the next element is also visible. */
	NeedsNextElement   = 1 << 28,
	// clang-format on
};
use_enum_as_flags(UiFlags);

} // namespace devilution
