#include <algorithm>
#include <memory>
#include <vector>

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
constexpr int LINE_H = 22;

struct SurfaceDeleter {
	void operator()(SDL_Surface *surface)
	{
		SDL_FreeSurface(surface);
	}
};

struct CachedLine {
	CachedLine() = default;

	explicit CachedLine(std::size_t index, SDL_Surface *text, SDL_Surface *shadow)
	    : index(index)
	    , text(text)
	    , shadow(shadow)
	    , palette_version(pal_surface_palette_version)
	{
	}

	std::size_t index;
	std::unique_ptr<SDL_Surface, SurfaceDeleter> text;
	std::unique_ptr<SDL_Surface, SurfaceDeleter> shadow;
	decltype(pal_surface_palette_version) palette_version;
};

SDL_Surface *RenderText(const char *text, SDL_Color color)
{
	if (text[0] == '\0')
		return nullptr;
	SDL_Surface *result = TTF_RenderUTF8_Solid(font, text, color);
	if (result == nullptr)
		SDL_Log(TTF_GetError());
	return result;
}

CachedLine PrepareLine(std::size_t index)
{
	const char *contents = CREDITS_LINES[index];
	if (contents[0] == '\t')
		++contents;
	return CachedLine(index, RenderText(contents, palette->colors[224]), RenderText(contents, { 0, 0, 0, 0 }));
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

void BlitToViewport(SDL_Surface *surface, int x, int y)
{
	SDL_Rect dest_rect = {
		static_cast<decltype(SDL_Rect().x)>(x), static_cast<decltype(SDL_Rect().y)>(y), 0, 0
	};
	if (SDL_BlitSurface(surface, nullptr, pal_surface, &dest_rect) <= -1) {
		SDL_Log(SDL_GetError());
	}
}

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
	int dest_y = VIEWPORT.y - (offset_y - lines_begin * LINE_H);
	for (std::size_t i = 0; i < lines_.size(); ++i, dest_y += LINE_H) {
		auto &line = lines_[i];
		if (line.text == nullptr)
			continue;

		// Still fading in: the cached line was drawn with a different fade level.
		if (line.palette_version != pal_surface_palette_version)
			line = PrepareLine(line.index);

		decltype(SDL_Rect().x) dest_x = VIEWPORT.x + 31;
		if (CREDITS_LINES[line.index][0] == '\t')
			dest_x += 40;

		BlitToViewport(line.shadow.get(), dest_x + 2, dest_y + 2);
		BlitToViewport(line.text.get(), dest_x, dest_y);
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
			}
		}
		SDL_Delay(1);
	} while (!endMenu && !credits_renderer.Finished());
	BlackPalette();

	return true;
}

} // namespace dvl
