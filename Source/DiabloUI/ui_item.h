#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "DiabloUI/art.h"
#include "DiabloUI/ui_flags.hpp"
#include "engine/render/text_render.hpp"
#include "utils/enum_traits.h"
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

class UiItemBase {
public:
	virtual ~UiItemBase() = default;

	[[nodiscard]] UiType GetType() const
	{
		return type_;
	}

	[[nodiscard]] bool IsType(UiType testType) const
	{
		return type_ == testType;
	}

	[[nodiscard]] UiFlags GetFlags() const
	{
		return uiFlags_;
	}

	[[nodiscard]] bool IsHidden() const
	{
		return HasAnyOf(uiFlags_, UiFlags::ElementHidden);
	}

	[[nodiscard]] bool IsNotInteractive() const
	{
		return HasAnyOf(uiFlags_, UiFlags::ElementHidden | UiFlags::ElementDisabled);
	}

	void Hide()
	{
		uiFlags_ |= UiFlags::ElementHidden;
	}

	void Show()
	{
		uiFlags_ &= ~UiFlags::ElementHidden;
	}

protected:
	UiItemBase(UiType type, SDL_Rect rect, UiFlags flags)
	    : type_(type)
	    , m_rect(rect)
	    , uiFlags_(flags)
	{
	}

	void SetFlags(UiFlags flags)
	{
		uiFlags_ = flags;
	}

private:
	UiType type_;

public:
	SDL_Rect m_rect;

private:
	UiFlags uiFlags_;
};

//=============================================================================

class UiImage : public UiItemBase {
public:
	UiImage(Art *art, SDL_Rect rect, UiFlags flags = UiFlags::None, bool animated = false, int frame = 0)
	    : UiItemBase(UiType::Image, rect, flags)
	    , art_(art)
	    , animated_(animated)
	    , frame_(frame)
	{
	}

	[[nodiscard]] bool IsCentered() const
	{
		return HasAnyOf(GetFlags(), UiFlags::AlignCenter);
	}

	[[nodiscard]] Art *GetArt() const
	{
		return art_;
	}

	[[nodiscard]] bool IsAnimated() const
	{
		return animated_;
	}

	[[nodiscard]] int GetFrame() const
	{
		return frame_;
	}

	void SetFrame(int frame)
	{
		frame_ = frame;
	}

private:
	Art *art_;
	bool animated_;
	int frame_;
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
	    , text_(text)
	    , spacing_(spacing)
	    , lineHeight_(lineHeight)
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
	    , textPointer_(ptext)
	    , spacing_(spacing)
	    , lineHeight_(lineHeight)
	{
	}

	[[nodiscard]] string_view GetText() const
	{
		if (text_ != nullptr)
			return text_;
		return *textPointer_;
	}

	[[nodiscard]] int GetSpacing() const
	{
		return spacing_;
	}

	[[nodiscard]] int GetLineHeight() const
	{
		return lineHeight_;
	}

private:
	const char *text_ = nullptr;
	const char **textPointer_ = nullptr;
	int spacing_;
	int lineHeight_;
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

	// private:
	Art *m_bg;
	Art *m_thumb;
	Art *m_arrow;
};

//=============================================================================

class UiArtTextButton : public UiItemBase {
public:
	using Callback = void (*)();

	UiArtTextButton(const char *text, Callback action, SDL_Rect rect, UiFlags flags = UiFlags::None)
	    : UiItemBase(UiType::ArtTextButton, rect, flags)
	    , text_(text)
	    , action_(action)
	{
	}

	void SetFlags(UiFlags flags)
	{
		UiItemBase::SetFlags(flags);
	}

	[[nodiscard]] string_view GetText() const
	{
		return text_;
	}

	void Activate() const
	{
		action_();
	}

private:
	const char *text_;
	Callback action_;
};

//=============================================================================

class UiEdit : public UiItemBase {
public:
	UiEdit(const char *hint, char *value, std::size_t maxLength, bool allowEmpty, SDL_Rect rect, UiFlags flags = UiFlags::None)
	    : UiItemBase(UiType::Edit, rect, flags)
	    , m_hint(hint)
	    , m_value(value)
	    , m_max_length(maxLength)
	    , m_allowEmpty(allowEmpty)
	{
	}

	// private:
	const char *m_hint;
	char *m_value;
	std::size_t m_max_length;
	bool m_allowEmpty;
};

//=============================================================================

// Plain text

class UiText : public UiItemBase {
public:
	UiText(const char *text, SDL_Rect rect, UiFlags flags = UiFlags::ColorDialogWhite)
	    : UiItemBase(UiType::Text, rect, flags)
	    , text_(text)
	{
	}

	[[nodiscard]] string_view GetText() const
	{
		return text_;
	}

private:
	const char *text_;
};

//=============================================================================

// A button (uses Diablo sprites)

class UiButton : public UiItemBase {
public:
	using Callback = void (*)();

	UiButton(Art *art, const char *text, Callback action, SDL_Rect rect, UiFlags flags = UiFlags::None)
	    : UiItemBase(UiType::Button, rect, flags)
	    , art_(art)
	    , text_(text)
	    , action_(action)
	    , pressed_(false)
	{
	}

	[[nodiscard]] int GetFrame() const
	{
		// Frame 1 is a held button sprite, frame 0 is the default
		return IsPressed() ? 1 : 0;
	}

	[[nodiscard]] Art *GetArt() const
	{
		return art_;
	}

	[[nodiscard]] string_view GetText() const
	{
		return text_;
	}

	void Activate() const
	{
		action_();
	}

	[[nodiscard]] bool IsPressed() const
	{
		return pressed_;
	}

	void Press()
	{
		pressed_ = true;
	}

	void Release()
	{
		pressed_ = false;
	}

private:
	Art *art_;

	const char *text_;
	Callback action_;

	// State
	bool pressed_;
};

//=============================================================================

class UiListItem {
public:
	UiListItem(const char *text = "", int value = 0, UiFlags uiFlags = UiFlags::None)
	    : m_text(text)
	    , m_value(value)
	    , uiFlags(uiFlags)
	{
	}

	UiListItem(const char *text, std::vector<DrawStringFormatArg> &args, int value = 0, UiFlags uiFlags = UiFlags::None)
	    : m_text(text)
	    , args(args)
	    , m_value(value)
	    , uiFlags(uiFlags)
	{
	}

	// private:
	const char *m_text;
	std::vector<DrawStringFormatArg> args;
	int m_value;
	UiFlags uiFlags;
};

class UiList : public UiItemBase {
public:
	using vUiListItem = std::vector<std::unique_ptr<UiListItem>>;

	UiList(const vUiListItem &vItems, size_t viewportSize, Sint16 x, Sint16 y, Uint16 item_width, Uint16 item_height, UiFlags flags = UiFlags::None, int spacing = 1)
	    : UiItemBase(UiType::List, { x, y, item_width, static_cast<Uint16>(item_height * viewportSize) }, flags)
	    , viewportSize(viewportSize)
	    , m_x(x)
	    , m_y(y)
	    , m_width(item_width)
	    , m_height(item_height)
	    , spacing_(spacing)
	{
		for (const auto &item : vItems)
			m_vecItems.push_back(item.get());
	}

	[[nodiscard]] SDL_Rect itemRect(int i) const
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

	[[nodiscard]] UiListItem *GetItem(std::size_t i) const
	{
		return m_vecItems[i];
	}

	[[nodiscard]] int GetSpacing() const
	{
		return spacing_;
	}

	// private:
	size_t viewportSize;
	Sint16 m_x, m_y;
	Uint16 m_width, m_height;
	std::vector<UiListItem *> m_vecItems;

private:
	int spacing_;
};
} // namespace devilution
