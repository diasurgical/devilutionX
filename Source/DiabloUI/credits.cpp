#include <algorithm>
#include <memory>
#include <vector>

#include "DiabloUI/art.h"
#include "DiabloUI/art_draw.h"
#include "DiabloUI/credits_lines.h"
#include "DiabloUI/diabloui.h"
#include "DiabloUI/fonts.h"
#include "DiabloUI/support_lines.h"
#include "control.h"
#include "controls/menu_controls.h"
#include "hwcursor.hpp"
#include "utils/display.h"
#include "utils/language.h"
#include "utils/log.hpp"
#include "utils/sdl_compat.h"
#include "utils/sdl_ptrs.h"

namespace devilution {

namespace {

const SDL_Rect VIEWPORT = { 0, 114, 640, 251 };
const int ShadowOffsetX = 2;
const int ShadowOffsetY = 2;
const int LINE_H = 22;

char const *const *text;
std::size_t textLines;

// The maximum number of visible lines is the number of whole lines
// (VIEWPORT.h / LINE_H) rounded up, plus one extra line for when
// a line is leaving the screen while another one is entering.
#define MAX_VISIBLE_LINES ((VIEWPORT.h - 1) / LINE_H + 2)

struct CachedLine {

	CachedLine()
	{
		m_index = 0;
		palette_version = pal_surface_palette_version;
	}

	CachedLine(std::size_t index, SDLSurfaceUniquePtr surface)
	{
		m_index = index;
		m_surface = std::move(surface);
		palette_version = pal_surface_palette_version;
	}

	std::size_t m_index;
	SDLSurfaceUniquePtr m_surface;
	unsigned int palette_version;
};

SDL_Surface *RenderText(const char *text, SDL_Color color)
{
	if (text[0] == '\0')
		return nullptr;
	SDL_Surface *result = TTF_RenderText_Solid(font, text, color);
	if (result == nullptr)
		Log("{}", TTF_GetError());
	return result;
}

CachedLine PrepareLine(std::size_t index)
{
	const char *contents = _(text[index]);
	while (contents[0] == '\t')
		++contents;

	const SDL_Color shadowColor = { 0, 0, 0, 0 };
	SDLSurfaceUniquePtr text { RenderText(contents, shadowColor) };

	// Precompose shadow and text:
	SDLSurfaceUniquePtr surface;
	if (text != nullptr) {
		// Set up the target surface to have 3 colors: mask, text, and shadow.
		surface = SDLSurfaceUniquePtr { SDL_CreateRGBSurfaceWithFormat(0, text->w + ShadowOffsetX, text->h + ShadowOffsetY, 8, SDL_PIXELFORMAT_INDEX8) };
		const SDL_Color maskColor = { 0, 255, 0, 0 }; // Any color different from both shadow and text
		const SDL_Color &textColor = palette->colors[224];
		SDL_Color colors[3] = { maskColor, textColor, shadowColor };
		if (SDLC_SetSurfaceColors(surface.get(), colors, 0, 3) <= -1)
			Log("{}", SDL_GetError());
		SDLC_SetColorKey(surface.get(), 0);

		// Blit the shadow first:
		SDL_Rect shadowRect = { ShadowOffsetX, ShadowOffsetY, 0, 0 };
		if (SDL_BlitSurface(text.get(), nullptr, surface.get(), &shadowRect) <= -1)
			ErrSdl();

		// Change the text surface color and blit again:
		SDL_Color textColors[2] = { maskColor, textColor };
		if (SDLC_SetSurfaceColors(text.get(), textColors, 0, 2) <= -1)
			ErrSdl();
		SDLC_SetColorKey(text.get(), 0);

		if (SDL_BlitSurface(text.get(), nullptr, surface.get(), nullptr) <= -1)
			ErrSdl();

		surface = ScaleSurfaceToOutput(std::move(surface));
	}
	return CachedLine(index, std::move(surface));
}

class CreditsRenderer {

public:
	CreditsRenderer()
	{
		LoadTtfFont();
		ticks_begin_ = SDL_GetTicks();
		prev_offset_y_ = 0;
		finished_ = false;
	}

	~CreditsRenderer()
	{
		ArtBackgroundWidescreen.Unload();
		ArtBackground.Unload();
		UnloadTtfFont();
		lines_.clear();
	}

	void Render();

	[[nodiscard]] bool Finished() const
	{
		return finished_;
	}

private:
	std::vector<CachedLine> lines_;
	bool finished_;
	Uint32 ticks_begin_;
	int prev_offset_y_;
};

void CreditsRenderer::Render()
{
	const int offsetY = -VIEWPORT.h + (SDL_GetTicks() - ticks_begin_) / 40;
	if (offsetY == prev_offset_y_)
		return;
	prev_offset_y_ = offsetY;

	SDL_FillRect(DiabloUiSurface(), nullptr, 0x000000);
	DrawArt({ PANEL_LEFT - 320, UI_OFFSET_Y }, &ArtBackgroundWidescreen);
	DrawArt({ PANEL_LEFT, UI_OFFSET_Y }, &ArtBackground);
	if (font == nullptr)
		return;

	const std::size_t linesBegin = std::max(offsetY / LINE_H, 0);
	const std::size_t linesEnd = std::min(linesBegin + MAX_VISIBLE_LINES, textLines);

	if (linesBegin >= linesEnd) {
		if (linesEnd == textLines)
			finished_ = true;
		return;
	}

	while (linesEnd > lines_.size())
		lines_.push_back(PrepareLine(lines_.size()));

	SDL_Rect viewport = VIEWPORT;
	viewport.x += PANEL_LEFT;
	viewport.y += UI_OFFSET_Y;
	ScaleOutputRect(&viewport);
	SDL_SetClipRect(DiabloUiSurface(), &viewport);

	// We use unscaled coordinates for calculation throughout.
	Sint16 destY = UI_OFFSET_Y + VIEWPORT.y - (offsetY - linesBegin * LINE_H);
	for (std::size_t i = linesBegin; i < linesEnd; ++i, destY += LINE_H) {
		CachedLine &line = lines_[i];
		if (line.m_surface == nullptr)
			continue;

		// Still fading in: the cached line was drawn with a different fade level.
		if (line.palette_version != pal_surface_palette_version) {
			line = PrepareLine(line.m_index);
		}

		Sint16 destX = PANEL_LEFT + VIEWPORT.x + 31;
		int j = 0;
		while (text[line.m_index][j++] == '\t')
			destX += 40;

		SDL_Rect dstRect = { destX, destY, 0, 0 };
		ScaleOutputRect(&dstRect);
		dstRect.w = line.m_surface->w;
		dstRect.h = line.m_surface->h;
		if (SDL_BlitSurface(line.m_surface.get(), nullptr, DiabloUiSurface(), &dstRect) < 0)
			ErrSdl();
	}
	SDL_SetClipRect(DiabloUiSurface(), nullptr);
}

bool TextDialog()
{
	CreditsRenderer creditsRenderer;
	bool endMenu = false;

	if (IsHardwareCursor())
		SetHardwareCursorVisible(false);

	SDL_Event event;
	do {
		creditsRenderer.Render();
		UiFadeIn();
		while (SDL_PollEvent(&event) != 0) {
			switch (event.type) {
			case SDL_KEYDOWN:
			case SDL_MOUSEBUTTONDOWN:
				endMenu = true;
				break;
			default:
				switch (GetMenuAction(event)) {
				case MenuAction_BACK:
				case MenuAction_SELECT:
					endMenu = true;
					break;
				default:
					break;
				}
			}
			UiHandleEvents(&event);
		}
	} while (!endMenu && !creditsRenderer.Finished());

	return true;
}

} // namespace

bool UiCreditsDialog()
{
	text = CREDITS_LINES;
	textLines = CREDITS_LINES_SIZE;

	LoadArt("ui_art\\creditsw.pcx", &ArtBackgroundWidescreen);
	LoadBackgroundArt("ui_art\\credits.pcx");

	return TextDialog();
}

bool UiSupportDialog()
{
	text = SUPPORT_LINES;
	textLines = SUPPORT_LINES_SIZE;

	if (gbIsHellfire) {
		LoadArt("ui_art\\supportw.pcx", &ArtBackgroundWidescreen);
		LoadBackgroundArt("ui_art\\support.pcx");
	} else {
		LoadArt("ui_art\\creditsw.pcx", &ArtBackgroundWidescreen);
		LoadBackgroundArt("ui_art\\credits.pcx");
	}

	return TextDialog();
}

} // namespace devilution
