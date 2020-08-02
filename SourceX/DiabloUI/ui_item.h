#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>

#include "all.h"
#include "stubs.h"

#include "DiabloUI/art.h"
#include "DiabloUI/text_draw.h"

namespace dvl {

enum UiType {
	UI_TEXT,
	UI_ART_TEXT,
	UI_ART_TEXT_BUTTON,
	UI_IMAGE,
	UI_BUTTON,
	UI_LIST,
	UI_SCROLLBAR,
	UI_EDIT,
};

enum UiFlags {
	UIS_SMALL = 1 << 0,
	UIS_MED = 1 << 1,
	UIS_BIG = 1 << 2,
	UIS_HUGE = 1 << 3,
	UIS_CENTER = 1 << 4,
	UIS_RIGHT = 1 << 5,
	UIS_VCENTER = 1 << 6,
	UIS_SILVER = 1 << 7,
	UIS_GOLD = 1 << 8,
	UIS_DISABLED = 1 << 9,
	UIS_HIDDEN = 1 << 10,
};

struct UiItemBase {
	constexpr UiItemBase(SDL_Rect rect, int flags)
	    : rect(rect)
	    , flags(flags)
	{
	}

	bool has_flag(UiFlags flag) const
	{
		return flags & flag;
	}

	bool has_any_flag(int flags) const
	{
		return (this->flags & flags) != 0;
	}

	void add_flag(UiFlags flag)
	{
		flags |= flag;
	}

	void remove_flag(UiFlags flag)
	{
		flags &= ~flag;
	}

	SDL_Rect rect;
	int flags;
};

struct UiImage : public UiItemBase {
	constexpr UiImage(Art *art, bool animated, int frame, SDL_Rect rect, int flags = 0)
	    : UiItemBase(rect, flags)
	    , art(art)
	    , animated(animated)
	    , frame(frame)
	{
	}

	constexpr UiImage(Art *art, int frame, SDL_Rect rect, int flags = 0)
	    : UiImage(art, /*animated=*/false, frame, rect, flags)
	{
	}

	constexpr UiImage(Art *art, SDL_Rect rect, int flags = 0)
	    : UiImage(art, /*frame=*/0, rect, flags)
	{
	}

	Art *art;
	bool animated;
	int frame;
};

// Plain text (TTF).
struct UiText : public UiItemBase {
	constexpr UiText(const char *text, SDL_Color color, SDL_Rect rect, int flags = 0)
	    : UiItemBase(rect, flags)
	    , color(color)
	    , shadow_color { 0, 0, 0, 0 }
	    , text(text)
	    , render_cache(NULL)
	{
	}

	constexpr UiText(const char *text, SDL_Rect rect, int flags = 0)
	    : UiText(text, SDL_Color { 243, 243, 243, 0 }, rect, flags)
	{
	}

	SDL_Color color;
	SDL_Color shadow_color;
	const char *text;

	// State:
	TtfSurfaceCache *render_cache;

	void FreeCache()
	{
		delete render_cache;
		render_cache = NULL;
	}
};

// Text drawn with Diablo sprites.
struct UiArtText : public UiItemBase {
	constexpr UiArtText(const char *text, SDL_Rect rect, int flags = 0)
	    : UiItemBase(rect, flags)
	    , text(text)
	{
	}

	const char *text;
};

// Clickable Diablo sprites text.
struct UiArtTextButton : public UiItemBase {
	constexpr UiArtTextButton(const char *text, void (*action)(), SDL_Rect rect, int flags = 0)
	    : UiItemBase(rect, flags)
	    , text(text)
	    , action(action)
	{
	}

	const char *text;
	void (*action)();
};

// A button (uses Diablo sprites).
struct UiButton : public UiItemBase {
	enum FrameKey {
		DEFAULT = 0,
		PRESSED,
		DISABLED
	};

	constexpr UiButton(Art *art, const char *text, void (*action)(), SDL_Rect rect, int flags = 0)
	    : UiItemBase(rect, flags)
	    , art(art)
	    , text(text)
	    , action(action)
	    , pressed(false)
	    , render_cache(NULL)
	{
	}

	Art *art;

	const char *text;
	void (*action)();

	// State
	bool pressed;
	TtfSurfaceCache *render_cache;

	void FreeCache()
	{
		delete render_cache;
		render_cache = NULL;
	}
};

struct UiListItem {
	constexpr UiListItem(const char *text = "", int value = 0)
	    : text(text)
	    , value(value)
	{
	}
	const char *text;
	int value;
};

struct UiList : public UiItemBase {
	template <std::size_t N>
	constexpr UiList(
	    UiListItem (&items)[N],
	    Sint16 x,
	    Sint16 y,
	    Uint16 item_width,
	    Uint16 item_height,
	    int flags)
	    : UiItemBase({ x, y, item_width, static_cast<Uint16>(item_height * N) }, flags)
	    , x(x)
	    , y(y)
	    , item_width(item_width)
	    , item_height(item_height)
	    , items(items)
	    , length(N)
	{
	}

	Sint16 x;
	Sint16 y;
	Uint16 item_width;
	Uint16 item_height;
	UiListItem *items;
	std::size_t length;

	SDL_Rect itemRect(std::size_t i) const
	{
		return { x, static_cast<Sint16>(y + item_height * i), item_width, item_height };
	}

	UiListItem *itemAt(Sint16 y) const
	{
		ASSERT(y >= rect.y);
		const std::size_t index = (y - rect.y) / item_height;
		ASSERT(index < length);
		return &items[index];
	}
};

struct UiScrollBar : public UiItemBase {
	constexpr UiScrollBar(Art *bg, Art *thumb, Art *arrow, SDL_Rect rect, int flags = 0)
	    : UiItemBase(rect, flags)
	    , bg(bg)
	    , thumb(thumb)
	    , arrow(arrow)
	{
	}

	Art *bg;
	Art *thumb;
	Art *arrow;
};

struct UiEdit : public UiItemBase {
	constexpr UiEdit(char *value, std::size_t max_length, SDL_Rect rect, int flags)
	    : UiItemBase(rect, flags)
	    , value(value)
	    , max_length(max_length)

	{
	}

	char *value;
	std::size_t max_length;
};

struct UiItem {
	constexpr UiItem(UiText text)
	    : type(UI_TEXT)
	    , text(text)
	{
	}

	constexpr UiItem(UiArtText text)
	    : type(UI_ART_TEXT)
	    , art_text(text)
	{
	}

	constexpr UiItem(UiArtTextButton art_text_button)
	    : type(UI_ART_TEXT_BUTTON)
	    , art_text_button(art_text_button)
	{
	}

	constexpr UiItem(UiImage image)
	    : type(UI_IMAGE)
	    , image(image)
	{
	}

	constexpr UiItem(UiButton button)
	    : type(UI_BUTTON)
	    , button(button)
	{
	}

	constexpr UiItem(UiList list)
	    : type(UI_LIST)
	    , list(list)
	{
	}

	constexpr UiItem(UiScrollBar scrollbar)
	    : type(UI_SCROLLBAR)
	    , scrollbar(scrollbar)
	{
	}

	constexpr UiItem(UiEdit edit)
	    : type(UI_EDIT)
	    , edit(edit)
	{
	}

	UiType type;
	union {
		UiText text;
		UiArtText art_text;
		UiImage image;
		UiArtTextButton art_text_button;
		UiButton button;
		UiList list;
		UiScrollBar scrollbar;
		UiEdit edit;
		UiItemBase common;
	};

	bool has_flag(UiFlags flag) const
	{
		return common.has_flag(flag);
	}

	bool has_any_flag(int flags) const
	{
		return common.has_any_flag(flags);
	}

	const SDL_Rect &rect() const
	{
		return common.rect;
	}

	void FreeCache()
	{
		switch (type) {
		case UI_BUTTON:
			button.FreeCache();
			break;
		case UI_TEXT:
			text.FreeCache();
			break;
		default:
			break;
		}
	}
};

} // namespace dvl
