#include <algorithm>
#include <memory>
#include <vector>

#include "controls/menu_controls.h"
#include "all.h"
#include "display.h"

#include "DiabloUI/diabloui.h"
#include "DiabloUI/credits_lines.h"
#include "DiabloUI/art.h"
#include "DiabloUI/art_draw.h"
#include "DiabloUI/fonts.h"

namespace dvl {

namespace {

const SDL_Rect VIEWPORT = { 0, 114, SCREEN_WIDTH, 251 };
const int SHADOW_OFFSET_X = 2;
const int SHADOW_OFFSET_Y = 2;
const int LINE_H = 22;

// The maximum number of visible lines is the number of whole lines
// (VIEWPORT.h / LINE_H) rounded up, plus one extra line for when
// a line is leaving the screen while another one is entering.
#define MAX_VISIBLE_LINES ((VIEWPORT.h - 1) / LINE_H + 2)

struct SurfaceDeleter {
	void operator()(SDL_Surface *surface)
	{
		SDL_FreeSurface(surface);
	}
};

struct CachedLine {
	CachedLine(std::size_t index, SDL_Surface *surface)
	{
		m_index = index;
		m_surface = surface;
	}

	std::size_t m_index;
	SDL_Surface *m_surface;
	unsigned int palette_version;
};

SDL_Surface *RenderText(const char *text, SDL_Color color)
{
	if (text[0] == '\0')
		return NULL;
	SDL_Surface *result = TTF_RenderUTF8_Solid(font, text, color);
	if (result == NULL)
		SDL_Log(TTF_GetError());
	return result;
}

CachedLine PrepareLine(std::size_t index)
{
	const char *contents = CREDITS_LINES[index];
	if (contents[0] == '\t')
		++contents;

	const SDL_Color shadow_color = { 0, 0, 0, 0 };
	SDL_Surface *text = RenderText(contents, shadow_color);

	// Precompose shadow and text:
	SDL_Surface *surface = NULL;
	if (text != NULL) {
		// Set up the target surface to have 3 colors: mask, text, and shadow.
		surface = SDL_CreateRGBSurfaceWithFormat(0, text->w + SHADOW_OFFSET_X, text->h + SHADOW_OFFSET_Y, 8, SDL_PIXELFORMAT_INDEX8);
		const SDL_Color mask_color = { 0, 255, 0, 0 }; // Any color different from both shadow and text
		const SDL_Color &text_color = palette->colors[224];
		SDL_Color colors[3] = { mask_color, text_color, shadow_color };
		if (SDLC_SetSurfaceColors(surface, colors, 0, 3) <= -1)
			SDL_Log(SDL_GetError());
		SDLC_SetColorKey(surface, 0);

		// Blit the shadow first:
		SDL_Rect shadow_rect = { SHADOW_OFFSET_X, SHADOW_OFFSET_Y, 0, 0 };
		if (SDL_BlitSurface(text, NULL, surface, &shadow_rect) <= -1)
			ErrSdl();

		// Change the text surface color and blit again:
		SDL_Color text_colors[2] = { mask_color, text_color };
		if (SDLC_SetSurfaceColors(text, text_colors, 0, 2) <= -1)
			ErrSdl();
		SDLC_SetColorKey(text, 0);

		if (SDL_BlitSurface(text, NULL, surface, NULL) <= -1)
			ErrSdl();

		SDL_Surface *surface_ptr = surface;
		ScaleSurfaceToOutput(&surface_ptr);
		surface = surface_ptr;
	}

	return CachedLine(index, surface);
}

/**
 * Similar to std::deque<CachedLine> but simpler and backed by a single vector.
 */
class LinesBuffer {
public:
	LinesBuffer(std::size_t capacity)
	{
		data_.reserve(capacity);
		for (std::size_t i = 0; i < capacity; ++i)
			data_.push_back(CachedLine(0, NULL));

		start_ = 0;
		end_ = 0;
		empty_ = true;
	}

	bool empty() const
	{
		return empty_;
	}

	CachedLine &front()
	{
		return data_[start_];
	}

	CachedLine &back()
	{
		return data_[end_];
	}

	CachedLine &operator[](std::size_t i)
	{
		return data_[(start_ + i) % data_.size()];
	}

	std::size_t size() const
	{
		if (empty_)
			return 0;
		return start_ < end_ ? end_ - start_ : data_.size();
	}

	void pop_front()
	{
		start_ = (start_ + 1) % data_.size();
		if (start_ == end_)
			empty_ = true;
	}

	void push_back(CachedLine line)
	{
		end_ = (end_ + 1) % data_.size();
		data_[end_] = line;
		empty_ = false;
	}

private:
	std::size_t start_;
	std::size_t end_;
	bool empty_;
	std::vector<CachedLine> data_;
};

class CreditsRenderer {

public:
	CreditsRenderer()
	    : lines_(MAX_VISIBLE_LINES)
	{
		LoadBackgroundArt("ui_art\\credits.pcx");
		LoadTtfFont();
		ticks_begin_ = SDL_GetTicks();
		prev_offset_y_ = 0;
		finished_ = false;
	}

	~CreditsRenderer()
	{
		ArtBackground.Unload();
		UnloadTtfFont();
	}

	void Render();

	bool Finished() const
	{
		return finished_;
	}

private:
	LinesBuffer lines_;
	bool finished_;
	Uint32 ticks_begin_;
	int prev_offset_y_;
};

void CreditsRenderer::Render()
{
	const int offset_y = -VIEWPORT.h + (SDL_GetTicks() - ticks_begin_) / 40;
	if (offset_y == prev_offset_y_)
		return;
	prev_offset_y_ = offset_y;

	SDL_FillRect(GetOutputSurface(), NULL, 0x000000);
	DrawArt(PANEL_LEFT + 0, 0, &ArtBackground);
	if (font == NULL)
		return;

	const std::size_t lines_begin = std::max(offset_y / LINE_H, 0);
	const std::size_t lines_end = std::min(lines_begin + MAX_VISIBLE_LINES, CREDITS_LINES_SIZE);

	if (lines_begin >= lines_end) {
		if (lines_end == CREDITS_LINES_SIZE)
			finished_ = true;
		return;
	}

	while (!lines_.empty() && lines_.front().m_index != lines_begin)
		lines_.pop_front();
	if (lines_.empty())
		lines_.push_back(PrepareLine(lines_begin));
	while (lines_.back().m_index + 1 != lines_end)
		lines_.push_back(PrepareLine(lines_.back().m_index + 1));

	SDL_Rect viewport = VIEWPORT;
	ScaleOutputRect(&viewport);
	SDL_SetClipRect(GetOutputSurface(), &viewport);

	// We use unscaled coordinates for calculation throughout.
	Sint16 dest_y = VIEWPORT.y - (offset_y - lines_begin * LINE_H);
	for (std::size_t i = 0; i < lines_.size(); ++i, dest_y += LINE_H) {
		CachedLine &line = lines_[i];
		if (line.m_surface == NULL)
			continue;

		// Still fading in: the cached line was drawn with a different fade level.
		if (line.palette_version != pal_surface_palette_version)
			line = PrepareLine(line.m_index);

		Sint16 dest_x = PANEL_LEFT + VIEWPORT.x + 31;
		if (CREDITS_LINES[line.m_index][0] == '\t')
			dest_x += 40;

		SDL_Rect dst_rect = { dest_x, dest_y, 0, 0 };
		ScaleOutputRect(&dst_rect);
		dst_rect.w = line.m_surface->w;
		dst_rect.h = line.m_surface->h;
		if (SDL_BlitSurface(line.m_surface, NULL, GetOutputSurface(), &dst_rect) < 0)
			ErrSdl();
	}
	SDL_SetClipRect(GetOutputSurface(), NULL);
}

} // namespace

BOOL UiCreditsDialog(int a1)
{
	CreditsRenderer credits_renderer;
	bool endMenu = false;

	SDL_Event event;
	do {
		credits_renderer.Render();
		UiFadeIn();
		while (SDL_PollEvent(&event)) {
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
	} while (!endMenu && !credits_renderer.Finished());

	return true;
}

} // namespace dvl
