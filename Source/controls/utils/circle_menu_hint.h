#pragma once

#include "engine/render/text_render.hpp"

namespace devilution {

enum HintIcon : uint8_t {
	IconChar,
	IconInv,
	IconQuests,
	IconSpells,
	IconMap,
	IconMenu,
	IconNull
};

struct CircleMenuHint {
	CircleMenuHint()
	    : top(HintIcon::IconNull)
	    , right(HintIcon::IconNull)
	    , bottom(HintIcon::IconNull)
	    , left(HintIcon::IconNull)
	{
	}

	CircleMenuHint(HintIcon top, HintIcon right, HintIcon bottom, HintIcon left)
	    : top(top)
	    , right(right)
	    , bottom(bottom)
	    , left(left)
	{
	}

	HintIcon top;
	HintIcon right;
	HintIcon bottom;
	HintIcon left;
};

} // namespace devilution
