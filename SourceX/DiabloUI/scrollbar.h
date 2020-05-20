#pragma once

#include "DiabloUI/art.h"
#include "DiabloUI/ui_item.h"

namespace dvl {

extern Art ArtScrollBarBackground;
const Uint16 SCROLLBAR_BG_WIDTH = 25;

extern Art ArtScrollBarArrow;
enum ScrollBarArrowFrame {
	ScrollBarArrowFrame_UP_ACTIVE = 0,
	ScrollBarArrowFrame_UP,
	ScrollBarArrowFrame_DOWN_ACTIVE,
	ScrollBarArrowFrame_DOWN,
};

extern Art ArtScrollBarThumb;
const Uint16 SCROLLBAR_ARROW_WIDTH = 25;

inline SDL_Rect UpArrowRect(const UiScrollBar &sb)
{
	return {
		sb.rect.x,
		sb.rect.y,
		SCROLLBAR_ARROW_WIDTH,
		static_cast<Uint16>(sb.arrow->h()),
	};
}

inline SDL_Rect DownArrowRect(const UiScrollBar &sb)
{
	return {
		sb.rect.x,
		static_cast<Sint16>(sb.rect.y + sb.rect.h) - sb.arrow->h(),
		SCROLLBAR_ARROW_WIDTH,
		static_cast<Uint16>(sb.arrow->h()),
	};
}

inline Uint16 BarHeight(const UiScrollBar &sb)
{
	return sb.rect.h - 2 * sb.arrow->h();
}

inline SDL_Rect BarRect(const UiScrollBar &sb)
{
	return {
		sb.rect.x,
		static_cast<Sint16>(sb.rect.y + sb.arrow->h()),
		SCROLLBAR_BG_WIDTH,
		BarHeight(sb),
	};
}

inline SDL_Rect ThumbRect(const UiScrollBar &sb, std::size_t selected_index, std::size_t num_items)
{
	const int THUMB_OFFSET_X = 3;
	const int thumb_max_y = BarHeight(sb) - sb.thumb->h();
	const int thumb_y = (selected_index * thumb_max_y / (num_items - 1));
	return {
		static_cast<Sint16>(sb.rect.x + THUMB_OFFSET_X),
		static_cast<Sint16>(sb.rect.y + sb.arrow->h() + thumb_y),
		static_cast<Uint16>(sb.rect.w - THUMB_OFFSET_X),
		static_cast<Uint16>(sb.thumb->h()),
	};
}

constexpr UiScrollBar MakeScrollBar(SDL_Rect rect)
{
	return UiScrollBar(&ArtScrollBarBackground, &ArtScrollBarThumb, &ArtScrollBarArrow, rect);
}

void LoadScrollBar();
void UnloadScrollBar();

} // namespace dvl
