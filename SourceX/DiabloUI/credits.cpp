#include <algorithm>
#include <memory>
#include <vector>

#include "controls/menu_controls.h"
#include "devilution.h"
#include "miniwin/ddraw.h"

#include "DiabloUI/diabloui.h"
#include "DiabloUI/credits_lines.h"
#include "DiabloUI/art.h"
#include "DiabloUI/art_draw.h"
#include "DiabloUI/fonts.h"

namespace dvl {

namespace {

const SDL_Rect VIEWPORT = { SCREEN_X, SCREEN_Y + 114, SCREEN_WIDTH, 251 };
constexpr int SHADOW_OFFSET_X = 2;
constexpr int SHADOW_OFFSET_Y = 2;
constexpr int LINE_H = 22;

struct SurfaceDeleter {
	void operator()(SDL_Surface *surface)
	{
		SDL_FreeSurface(surface);
	}
};

using SurfacePtr = std::unique_ptr<SDL_Surface, SurfaceDeleter>;

struct CachedLine {
	CachedLine() = default;

	explicit CachedLine(std::size_t index, SurfacePtr surface)
	    : index(index)
	    , surface(std::move(surface))
	    , palette_version(pal_surface_palette_version)
	{
	}

	std::size_t index;
	SurfacePtr surface;
	decltype(pal_surface_palette_version) palette_version;
};

SurfacePtr RenderText(const char *text, SDL_Color color)
{
	if (text[0] == '\0')
		return nullptr;
	SDL_Surface *result = TTF_RenderUTF8_Solid(font, text, color);
	if (result == nullptr)
		SDL_Log(TTF_GetError());
	return SurfacePtr(result);
}

CachedLine PrepareLine(std::size_t index)
{
	const char *contents = CREDITS_LINES[index];
	if (contents[0] == '\t')
		++contents;

	const SDL_Color shadow_color = { 0, 0, 0, 0 };
	auto text = RenderText(contents, shadow_color);

	// Precompose shadow and text:
	SurfacePtr surface;
	if (text != nullptr) {
		// Set up the target surface to have 3 colors: mask, text, and shadow.
		surface.reset(
		    SDL_CreateRGBSurfaceWithFormat(0, text->w + SHADOW_OFFSET_X, text->h + SHADOW_OFFSET_Y, 8, SDL_PIXELFORMAT_INDEX8));
		const SDL_Color &mask_color = { 0, 255, 0, 0 }; // Any color different from both shadow and text
		const SDL_Color &text_color = palette->colors[224];
		SDL_Color colors[3] = { mask_color, text_color, shadow_color };
		if (SDLC_SetSurfaceColors(surface.get(), colors, 0, 3) <= -1)
			SDL_Log(SDL_GetError());
		SDLC_SetColorKey(surface.get(), 0);

		// Blit the shadow first:
		SDL_Rect shadow_rect = { SHADOW_OFFSET_X, SHADOW_OFFSET_Y, 0, 0 };
		if (SDL_BlitSurface(text.get(), nullptr, surface.get(), &shadow_rect) <= -1)
			ErrSdl();

		// Change the text surface color and blit again:
		SDL_Color text_colors[2] = { mask_color, text_color };
		if (SDLC_SetSurfaceColors(text.get(), text_colors, 0, 2) <= -1)
			ErrSdl();
		SDLC_SetColorKey(text.get(), 0);

		if (SDL_BlitSurface(text.get(), nullptr, surface.get(), nullptr) <= -1)
			ErrSdl();
	}

	return CachedLine(index, std::move(surface));
}

/**
 * Similar to std::deque<CachedLine> but simpler and backed by a single vector.
 */
class LinesBuffer {
public:
	LinesBuffer(std::size_t capacity)
	    : start_(0)
	    , end_(0)
	    , empty_(true)
	{
		data_.reserve(capacity);
		for (std::size_t i = 0; i < capacity; ++i)
			data_.push_back(CachedLine());
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

	void push_back(CachedLine &&line)
	{
		end_ = (end_ + 1) % data_.size();
		data_[end_] = std::move(line);
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
	    : lines_(VIEWPORT.h / LINE_H + 1)
	    , finished_(false)
	    , prev_offset_y_(0)

	{
		LoadBackgroundArt("ui_art\\credits.pcx");
		LoadTtfFont();
		ticks_begin_ = SDL_GetTicks();
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
	decltype(SDL_GetTicks()) ticks_begin_;
	int prev_offset_y_;
};

void CreditsRenderer::Render()
{
	const int offset_y = -(VIEWPORT.y - LINE_H) + (SDL_GetTicks() - ticks_begin_) / 40;
	if (offset_y == prev_offset_y_)
		return;
	prev_offset_y_ = offset_y;

	DrawArt(0, 0, &ArtBackground);
	if (font == nullptr)
		return;

	const std::size_t lines_begin = std::max(offset_y / LINE_H, 0);
	const std::size_t lines_end = std::min(lines_begin + (VIEWPORT.h - 1) / LINE_H + 1, CREDITS_LINES_SIZE);

	if (lines_begin >= lines_end) {
		if (lines_end == CREDITS_LINES_SIZE)
			finished_ = true;
		return;
	}

	while (!lines_.empty() && lines_.front().index != lines_begin)
		lines_.pop_front();
	if (lines_.empty())
		lines_.push_back(PrepareLine(lines_begin));
	while (lines_.back().index + 1 != lines_end)
		lines_.push_back(PrepareLine(lines_.back().index + 1));

	SDL_SetClipRect(pal_surface, &VIEWPORT);
	decltype(SDL_Rect().y) dest_y = VIEWPORT.y - (offset_y - lines_begin * LINE_H);
	for (std::size_t i = 0; i < lines_.size(); ++i, dest_y += LINE_H) {
		auto &line = lines_[i];
		if (line.surface == nullptr)
			continue;

		// Still fading in: the cached line was drawn with a different fade level.
		if (line.palette_version != pal_surface_palette_version)
			line = PrepareLine(line.index);

		decltype(SDL_Rect().x) dest_x = VIEWPORT.x + 31;
		if (CREDITS_LINES[line.index][0] == '\t')
			dest_x += 40;

		SDL_Rect dest_rect = { dest_x, dest_y, 0, 0 };
		if (SDL_BlitSurface(line.surface.get(), nullptr, pal_surface, &dest_rect) <= -1)
			ErrSdl();
	}
	SDL_SetClipRect(pal_surface, nullptr);
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
			case SDL_QUIT:
				exit(0);
			default:
				switch (GetMenuAction(event)) {
				case MenuAction::BACK:
				case MenuAction::SELECT:
					endMenu = true;
					break;
				default:
					break;
				}
			}
		}
	} while (!endMenu && !credits_renderer.Finished());
	BlackPalette();

	return true;
}

} // namespace dvl
