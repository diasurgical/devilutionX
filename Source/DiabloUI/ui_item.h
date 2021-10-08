#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "DiabloUI/art.h"
#include "utils/stubs.h"

namespace devilution {

enum class UiType {
	Text,
	ArtText,
	ArtTextButton,
	Image,
	Button,
	List,
	Scrollbar,
	Edit,
};

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
	// clang-format on
};

enum class UiPanels {
	Main,
	Quest,
	Character,
	Spell,
	Inventory,
};

inline UiFlags operator|(UiFlags lhs, UiFlags rhs)
{
	using T = std::underlying_type_t<UiFlags>;
	return static_cast<UiFlags>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

inline UiFlags operator|=(UiFlags &lhs, UiFlags rhs)
{
	lhs = lhs | rhs;
	return lhs;
}

inline UiFlags operator&(UiFlags lhs, UiFlags rhs)
{
	using T = std::underlying_type_t<UiFlags>;
	return static_cast<UiFlags>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

inline UiFlags operator&=(UiFlags &lhs, UiFlags rhs)
{
	lhs = lhs & rhs;
	return lhs;
}

inline UiFlags operator~(UiFlags value)
{
	using T = std::underlying_type_t<UiFlags>;
	return static_cast<UiFlags>(~static_cast<T>(value));
}

inline bool HasAnyOf(UiFlags lhs, UiFlags test)
{
	return (lhs & test) != UiFlags::None;
}

class UiItemBase {
public:
	UiItemBase(UiType type, SDL_Rect rect, UiFlags flags)
	    : m_type(type)
	    , m_rect(rect)
	    , m_iFlags(flags)
	{
	}

	virtual ~UiItemBase() {};

	bool has_flag(UiFlags flag) const
	{
		return (m_iFlags & flag) == flag;
	}

	bool has_any_flag(UiFlags flags) const
	{
		return HasAnyOf(m_iFlags, flags);
	}

	void add_flag(UiFlags flag)
	{
		m_iFlags |= flag;
	}

	void remove_flag(UiFlags flag)
	{
		m_iFlags &= ~flag;
	}

	//protected:
	UiType m_type;
	SDL_Rect m_rect;
	UiFlags m_iFlags;
};

//=============================================================================

class UiImage : public UiItemBase {
public:
	UiImage(Art *art, SDL_Rect rect, UiFlags flags = UiFlags::None, bool animated = false, int frame = 0)
	    : UiItemBase(UiType::Image, rect, flags)
	    , m_art(art)
	    , m_animated(animated)
	    , m_frame(frame)
	{
	}

	~UiImage() {};

	//private:
	Art *m_art;
	bool m_animated;
	int m_frame;
};

//=============================================================================

class UiArtText : public UiItemBase {
public:
	/**
	 * @brief Constructs a UI element containing a (presumed to be) static line of text
	 * @param text Pointer to the first character of a c-string
	 * @param rect screen region defining the area to draw the text
	 * @param flags UiFlags controlling color/alignment/size
	 */
	UiArtText(const char *text, SDL_Rect rect, UiFlags flags = UiFlags::None, int spacing = 1, int lineHeight = -1)
	    : UiItemBase(UiType::ArtText, rect, flags)
	    , m_text(text)
	    , m_spacing(spacing)
	    , m_lineHeight(lineHeight)
	{
	}

	/**
	 * @brief Constructs a UI element containing a line of text that may change between frames
	 * @param ptext Pointer to a c-string (pointer to a pointer to the first character)
	 * @param rect screen region defining the area to draw the text
	 * @param flags UiFlags controlling color/alignment/size
	 */
	UiArtText(const char **ptext, SDL_Rect rect, UiFlags flags = UiFlags::None, int spacing = 1, int lineHeight = -1)
	    : UiItemBase(UiType::ArtText, rect, flags)
	    , m_ptext(ptext)
	    , m_spacing(spacing)
	    , m_lineHeight(lineHeight)
	{
	}

	const char *text() const
	{
		if (m_text != nullptr)
			return m_text;
		return *m_ptext;
	}

	int spacing() const
	{
		return m_spacing;
	}

	int lineHeight() const
	{
		return m_lineHeight;
	}

	~UiArtText() {};

private:
	const char *m_text = nullptr;
	const char **m_ptext = nullptr;
	int m_spacing = 1;
	int m_lineHeight = -1;
};

//=============================================================================

class UiScrollbar : public UiItemBase {
public:
	UiScrollbar(Art *bg, Art *thumb, Art *arrow, SDL_Rect rect, UiFlags flags = UiFlags::None)
	    : UiItemBase(UiType::Scrollbar, rect, flags)
	    , m_bg(bg)
	    , m_thumb(thumb)
	    , m_arrow(arrow)
	{
	}

	//private:
	Art *m_bg;
	Art *m_thumb;
	Art *m_arrow;
};

//=============================================================================

class UiArtTextButton : public UiItemBase {
public:
	UiArtTextButton(const char *text, void (*action)(), SDL_Rect rect, UiFlags flags = UiFlags::None)
	    : UiItemBase(UiType::ArtTextButton, rect, flags)
	    , m_text(text)
	    , m_action(action)
	{
	}

	//private:
	const char *m_text;
	void (*m_action)();
};

//=============================================================================

class UiEdit : public UiItemBase {
public:
	UiEdit(const char *hint, char *value, std::size_t max_length, SDL_Rect rect, UiFlags flags = UiFlags::None)
	    : UiItemBase(UiType::Edit, rect, flags)
	    , m_hint(hint)
	    , m_value(value)
	    , m_max_length(max_length)
	{
	}

	//private:
	const char *m_hint;
	char *m_value;
	std::size_t m_max_length;
};

//=============================================================================

// Plain text

class UiText : public UiItemBase {
public:
	UiText(const char *text, SDL_Rect rect, UiFlags flags = UiFlags::ColorDialogWhite)
	    : UiItemBase(UiType::Text, rect, flags)
	    , m_text(text)
	{
	}

	//private:
	const char *m_text;
};

//=============================================================================

// A button (uses Diablo sprites)

class UiButton : public UiItemBase {
public:
	UiButton(Art *art, const char *text, void (*action)(), SDL_Rect rect, UiFlags flags = UiFlags::None)
	    : UiItemBase(UiType::Button, rect, flags)
	    , m_art(art)
	    , m_text(text)
	    , m_action(action)
	    , m_pressed(false)
	{
	}

	enum FrameKey : uint8_t {
		DEFAULT,
		PRESSED,
	};

	//private:
	Art *m_art;

	const char *m_text;
	void (*m_action)();

	// State
	bool m_pressed;
};

//=============================================================================

class UiListItem {
public:
	UiListItem(const char *text = "", int value = 0)
	    : m_text(text)
	    , m_value(value)
	{
	}

	~UiListItem() {};

	//private:
	const char *m_text;
	int m_value;
};

typedef std::vector<std::unique_ptr<UiListItem>> vUiListItem;

class UiList : public UiItemBase {
public:
	UiList(const vUiListItem &vItems, Sint16 x, Sint16 y, Uint16 item_width, Uint16 item_height, UiFlags flags = UiFlags::None, int spacing = 1)
	    : UiItemBase(UiType::List, { x, y, item_width, static_cast<Uint16>(item_height * vItems.size()) }, flags)
	    , m_x(x)
	    , m_y(y)
	    , m_width(item_width)
	    , m_height(item_height)
	    , m_spacing(spacing)
	{
		for (auto &item : vItems)
			m_vecItems.push_back(item.get());
	}

	~UiList() {};

	SDL_Rect itemRect(int i) const
	{
		SDL_Rect tmp;
		tmp.x = m_x;
		tmp.y = m_y + m_height * i;
		tmp.w = m_width;
		tmp.h = m_height;

		return tmp;
	}

	[[nodiscard]] size_t indexAt(Sint16 y) const
	{
		ASSERT(y >= m_rect.y);
		const size_t index = (y - m_rect.y) / m_height;
		ASSERT(index < m_vecItems.size());
		return index;
	}

	UiListItem *GetItem(int i) const
	{
		return m_vecItems[i];
	}

	int spacing() const
	{
		return m_spacing;
	}

	//private:
	Sint16 m_x, m_y;
	Uint16 m_width, m_height;
	std::vector<UiListItem *> m_vecItems;
	int m_spacing;
};
} // namespace devilution
