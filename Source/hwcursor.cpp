#include "hwcursor.hpp"

#include <cstdint>
#include <tuple>

#if SDL_VERSION_ATLEAST(2, 0, 0)
#include <SDL_mouse.h>
#include <SDL_render.h>
#include <SDL_surface.h>
#endif

#include "DiabloUI/diabloui.h"

#include "cursor.h"
#include "engine.h"
#include "utils/display.h"
#include "utils/sdl_ptrs.h"

namespace devilution {
namespace {
CursorInfo CurrentCursorInfo;

#if SDL_VERSION_ATLEAST(2, 0, 0)
SDLCursorUniquePtr CurrentCursor;

void SetHardwareCursor(SDL_Surface *surface)
{
	float scaleX;
	float scaleY;
	if (renderer != nullptr) {
		SDL_RenderGetScale(renderer, &scaleX, &scaleY);
	}

	SDLCursorUniquePtr newCursor;
	if (renderer == nullptr || (scaleX == 1.0F && scaleY == 1.0F)) {
		newCursor = SDLCursorUniquePtr { SDL_CreateColorCursor(surface, 0, 0) };
	} else {
		// SDL does not support BlitScaled from 8-bit to RGBA.
		SDLSurfaceUniquePtr converted { SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0) };

		const int scaledW = surface->w * scaleX; // NOLINT(bugprone-narrowing-conversions)
		const int scaledH = surface->h * scaleY; // NOLINT(bugprone-narrowing-conversions)
		SDLSurfaceUniquePtr scaledSurface { SDL_CreateRGBSurfaceWithFormat(0, scaledW, scaledH, 32, SDL_PIXELFORMAT_ARGB8888) };
		SDL_BlitScaled(converted.get(), nullptr, scaledSurface.get(), nullptr);
		newCursor = SDLCursorUniquePtr { SDL_CreateColorCursor(scaledSurface.get(), 0, 0) };
	}
	SDL_SetCursor(newCursor.get());
	CurrentCursor = std::move(newCursor);
}

void SetHardwareCursorFromSprite(int pcurs)
{
	const int outlineWidth = IsItemSprite(pcurs) ? 1 : 0;

	int width;
	int height;
	std::tie(width, height) = GetInvItemSize(pcurs);
	width += 2 * outlineWidth;
	height += 2 * outlineWidth;

	auto out = CelOutputBuffer::Alloc(width, height);
	SDL_SetSurfacePalette(out.surface, palette);

	// Transparent color must not be used in the sprite itself.
	// Colors 1-127 are outside of the UI palette so are safe to use.
	constexpr std::uint8_t TransparentColor = 1;
	SDL_FillRect(out.surface, nullptr, TransparentColor);
	SDL_SetColorKey(out.surface, 1, TransparentColor);
	CelDrawCursor(out, { outlineWidth, height - outlineWidth }, pcurs);

	SetHardwareCursor(out.surface);
	out.Free();
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
		SetHardwareCursorFromSprite(cursorInfo.id());
		break;
	case CursorType::UserInterface:
		// ArtCursor is null while loading the game on the progress screen,
		// called via palette fade from ShowProgress.
		if (ArtCursor.surface != nullptr)
			SetHardwareCursor(ArtCursor.surface.get());
		break;
	case CursorType::Unknown:
		break;
	}
#endif
}

} // namespace devilution
