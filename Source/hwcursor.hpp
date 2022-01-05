/**
 * @file hwcursor.hpp
 *
 * Hardware cursor (SDL2 only).
 */
#include <SDL_version.h>

#include "options.h"

namespace devilution {

// Whether the hardware cursor is enabled in settings.
inline bool IsHardwareCursorEnabled()
{
#if SDL_VERSION_ATLEAST(2, 0, 0)
	return *sgOptions.Graphics.hardwareCursor && HardwareCursorSupported();
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

	static CursorInfo UnknownCursor()
	{
		return CursorInfo { CursorType::Unknown };
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
