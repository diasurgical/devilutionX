/**
 * @file hwcursor.hpp
 *
 * Hardware cursor (SDL2 only).
 */
#include <SDL_version.h>

#include "options.h"

namespace devilution {

inline bool IsHardwareCursorEnabled()
{
#if SDL_VERSION_ATLEAST(2, 0, 0)
	return sgOptions.Graphics.bHardwareCursor;
#else
	return false;
#endif
}

/**
 * @return Whether the cursor was previously visible.
 */
inline bool SetHardwareCursorVisible(bool visible)
{
#if SDL_VERSION_ATLEAST(2, 0, 0)
	return SDL_ShowCursor(visible ? SDL_ENABLE : SDL_DISABLE) == 1;
#else
	return false;
#endif
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
	{
	}

	CursorType type_ = CursorType::Unknown;

	// ID for CursorType::Game
	int id_;
};

CursorInfo GetCurrentCursorInfo();

void SetHardwareCursor(CursorInfo cursorInfo);

inline void ReinitializeHardwareCursor()
{
	SetHardwareCursor(GetCurrentCursorInfo());
}

} // namespace devilution
