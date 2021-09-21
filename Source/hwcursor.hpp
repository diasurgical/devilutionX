/**
 * @file hwcursor.hpp
 *
 * Hardware cursor (SDL2 only).
 */
#include "options.h"

namespace devilution {

// Whether the hardware cursor is enabled in settings.
inline bool IsHardwareCursorEnabled()
{
	return sgOptions.Graphics.bHardwareCursor;
}

/**
 * @return Whether the cursor was previously visible.
 */
inline bool SetHardwareCursorVisible(bool visible)
{
	return SDL_ShowCursor(visible ? SDL_ENABLE : SDL_DISABLE) == 1;
}

enum class CursorType {
	Unknown,
	UserInterface,
	Game,
};

class CursorInfo {
public:
	CursorInfo() = default;

	static CursorInfo UserInterfaceCursor()
	{
		return CursorInfo { CursorType::UserInterface };
	}

	static CursorInfo GameCursor(int gameSpriteId)
	{
		return CursorInfo { CursorType::Game, gameSpriteId };
	}

	[[nodiscard]] CursorType type() const
	{
		return type_;
	}

	[[nodiscard]] int id() const
	{
		return id_;
	}

	[[nodiscard]] bool Enabled() const
	{
		return enabled_;
	}

	void SetEnabled(bool value)
	{
		enabled_ = value;
	}

	bool operator==(const CursorInfo &other) const
	{
		return type_ == other.type_ && (type_ != CursorType::Game || id_ == other.id_);
	}
	bool operator!=(const CursorInfo &other) const
	{
		return !(*this == other);
	}

private:
	explicit CursorInfo(CursorType type, int id = 0)
	    : type_(type)
	    , id_(id)
	    , enabled_(false)
	{
	}

	CursorType type_ = CursorType::Unknown;

	// ID for CursorType::Game
	int id_;

	bool enabled_ = false;
};

CursorInfo GetCurrentCursorInfo();

// Whether the current cursor is a hardware cursor.
inline bool IsHardwareCursor()
{
	return GetCurrentCursorInfo().Enabled();
}

void SetHardwareCursor(CursorInfo cursorInfo);

inline void ReinitializeHardwareCursor()
{
	SetHardwareCursor(GetCurrentCursorInfo());
}

} // namespace devilution
