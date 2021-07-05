#include "hwcursor.hpp"

#include <cstdint>
#include <tuple>

#if SDL_VERSION_ATLEAST(2, 0, 0)
#include <SDL_mouse.h>
#include <SDL_render.h>
#include <SDL_surface.h>
#endif

#include "DiabloUI/diabloui.h"

#include "appfat.h"
#include "cursor.h"
#include "engine.h"
#include "utils/display.h"
#include "utils/sdl_ptrs.h"

namespace devilution {
namespace {
CursorInfo CurrentCursorInfo;

#if SDL_VERSION_ATLEAST(2, 0, 0)
SDLCursorUniquePtr CurrentCursor;

enum class HotpointPosition {
	TopLeft,
	Center,
};

Point GetHotpointPosition(const SDL_Surface &surface, HotpointPosition position)
{
	switch (position) {
	case HotpointPosition::TopLeft:
		return { 0, 0 };
	case HotpointPosition::Center:
		return { surface.w / 2, surface.h / 2 };
	}
	app_fatal("Unhandled enum value");
}

bool SetHardwareCursor(SDL_Surface *surface, HotpointPosition hotpointPosition)
{
	float scaleX;
	float scaleY;
	if (renderer != nullptr) {
		SDL_RenderGetScale(renderer, &scaleX, &scaleY);
	}

	SDLCursorUniquePtr newCursor;
	if (renderer == nullptr || (scaleX == 1.0F && scaleY == 1.0F)) {
		const Point hotpoint = GetHotpointPosition(*surface, hotpointPosition);
		newCursor = SDLCursorUniquePtr { SDL_CreateColorCursor(surface, hotpoint.x, hotpoint.y) };
	} else {
		// SDL does not support BlitScaled from 8-bit to RGBA.
		SDLSurfaceUniquePtr converted { SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0) };

		const int scaledW = static_cast<int>(surface->w * scaleX);
		const int scaledH = static_cast<int>(surface->h * scaleY);
		SDLSurfaceUniquePtr scaledSurface { SDL_CreateRGBSurfaceWithFormat(0, scaledW, scaledH, 32, SDL_PIXELFORMAT_ARGB8888) };
		SDL_BlitScaled(converted.get(), nullptr, scaledSurface.get(), nullptr);
		const Point hotpoint = GetHotpointPosition(*scaledSurface, hotpointPosition);
		newCursor = SDLCursorUniquePtr { SDL_CreateColorCursor(scaledSurface.get(), hotpoint.x, hotpoint.y) };
	}
	if (newCursor == nullptr)
		return false;
	SDL_SetCursor(newCursor.get());
	CurrentCursor = std::move(newCursor);
	return true;
}

bool SetHardwareCursorFromSprite(int pcurs)
{
	const bool isItem = IsItemSprite(pcurs);
	const int outlineWidth = isItem ? 1 : 0;

	auto size = GetInvItemSize(pcurs);
	size.width += 2 * outlineWidth;
	size.height += 2 * outlineWidth;

	auto out = Surface::Alloc(size.width, size.height);
	SDL_SetSurfacePalette(out.surface, Palette);

	// Transparent color must not be used in the sprite itself.
	// Colors 1-127 are outside of the UI palette so are safe to use.
	constexpr std::uint8_t TransparentColor = 1;
	SDL_FillRect(out.surface, nullptr, TransparentColor);
	SDL_SetColorKey(out.surface, 1, TransparentColor);
	CelDrawCursor(out, { outlineWidth, size.height - outlineWidth }, pcurs);

	const bool result = SetHardwareCursor(out.surface, isItem ? HotpointPosition::Center : HotpointPosition::TopLeft);
	out.Free();
	return result;
}
#endif

} // namespace

CursorInfo GetCurrentCursorInfo()
{
	return CurrentCursorInfo;
}

void SetHardwareCursor(CursorInfo cursorInfo)
{
#if SDL_VERSION_ATLEAST(2, 0, 0)
	CurrentCursorInfo = cursorInfo;
	switch (cursorInfo.type()) {
	case CursorType::Game:
		CurrentCursorInfo.SetEnabled(SetHardwareCursorFromSprite(cursorInfo.id()));
		break;
	case CursorType::UserInterface:
		// ArtCursor is null while loading the game on the progress screen,
		// called via palette fade from ShowProgress.
		if (ArtCursor.surface != nullptr)
			CurrentCursorInfo.SetEnabled(SetHardwareCursor(ArtCursor.surface.get(), HotpointPosition::TopLeft));
		break;
	case CursorType::Unknown:
		CurrentCursorInfo.SetEnabled(false);
		break;
	}
#endif
}

} // namespace devilution
