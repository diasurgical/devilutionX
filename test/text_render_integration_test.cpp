#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>

#include <SDL.h>
#include <expected.hpp>
#include <function_ref.hpp>

#include "engine/load_file.hpp"
#include "engine/palette.h"
#include "engine/point.hpp"
#include "engine/rectangle.hpp"
#include "engine/render/primitive_render.hpp"
#include "engine/render/text_render.hpp"
#include "engine/size.hpp"
#include "engine/surface.hpp"
#include "utils/paths.h"
#include "utils/sdl_wrap.h"
#include "utils/surface_to_png.hpp"

namespace devilution {
namespace {

constexpr char FixturesPath[] = "../test/fixtures/text_render_integration_test/";

SDLPaletteUniquePtr LoadPalette()
{
	struct Color {
		uint8_t r, g, b;
	};
	std::array<Color, 256> palData;
	LoadFileInMem("ui_art\\diablo.pal", palData);
	SDLPaletteUniquePtr palette = SDLWrap::AllocPalette(256);
	for (unsigned i = 0; i < palData.size(); i++) {
		palette->colors[i] = SDL_Color {
			palData[i].r, palData[i].g, palData[i].b, SDL_ALPHA_OPAQUE
		};
	}
	return palette;
}

std::vector<std::byte> ReadFile(const std::string &path)
{
	SDL_RWops *rwops = SDL_RWFromFile(path.c_str(), "rb");
	std::vector<std::byte> result;
	if (rwops == nullptr) return result;
	const size_t size = SDL_RWsize(rwops);
	result.resize(size);
	SDL_RWread(rwops, result.data(), size, 1);
	SDL_RWclose(rwops);
	return result;
}

void DrawWithBorder(const Surface &out, const Rectangle &area, tl::function_ref<void(const Rectangle &)> fn)
{
	const uint8_t debugColor = PAL8_RED;
	DrawHorizontalLine(out, area.position, area.size.width, debugColor);
	DrawHorizontalLine(out, area.position + Displacement { 0, area.size.height - 1 }, area.size.width, debugColor);
	DrawVerticalLine(out, area.position, area.size.height, debugColor);
	DrawVerticalLine(out, area.position + Displacement { area.size.width - 1, 0 }, area.size.height, debugColor);
	fn(Rectangle {
	    Point { area.position.x + 1, area.position.y + 1 },
	    Size { area.size.width - 2, area.size.height - 2 } });
}

TEST(TextRenderIntegrationTest, GoldenTest)
{
	SDLPaletteUniquePtr palette = LoadPalette();
	OwnedSurface out { Size { 200, 140 } };
	SDL_SetSurfacePalette(out.surface, palette.get());
	ASSERT_NE(out.surface, nullptr);

	int y = -15;
	DrawWithBorder(out, Rectangle { Point { 0, y += 15 }, Size { 96, 15 } }, [&](const Rectangle &rect) {
		DrawString(out, "DrawString", rect,
		    TextRenderOptions { .flags = UiFlags::ColorUiGold });
	});
	DrawWithBorder(out, Rectangle { Point { 0, y += 15 }, Size { 120, 15 } }, [&](const Rectangle &rect) {
		DrawString(out, "KerningFitSpacing", rect,
		    TextRenderOptions { .flags = UiFlags::KerningFitSpacing | UiFlags::ColorUiSilver });
	});
	DrawWithBorder(out, Rectangle { Point { 0, y += 15 }, Size { 170, 15 } }, [&](const Rectangle &rect) {
		DrawString(out, "KerningFitSpacing | AlignCenter", rect,
		    TextRenderOptions { .flags = UiFlags::KerningFitSpacing | UiFlags::AlignCenter | UiFlags::ColorUiSilver });
	});
	DrawWithBorder(out, Rectangle { Point { 0, y += 15 }, Size { 170, 15 } }, [&](const Rectangle &rect) {
		DrawString(out, "KerningFitSpacing | AlignRight", rect,
		    TextRenderOptions { .flags = UiFlags::KerningFitSpacing | UiFlags::AlignRight | UiFlags::ColorUiSilver });
	});
	y += 4;
	DrawWithBorder(out, Rectangle { Point { 0, y += 15 }, Size { 186, 15 } }, [&](const Rectangle &rect) {
		DrawStringWithColors(out, "{}{}{}{}",
		    { { "Draw", UiFlags::ColorUiSilver },
		        { "String", UiFlags::ColorUiGold },
		        { "With", UiFlags::ColorUiSilverDark },
		        { "Colors", UiFlags::ColorUiGoldDark } },
		    rect);
	});
	DrawWithBorder(out, Rectangle { Point { 0, y += 15 }, Size { 120, 15 } }, [&](const Rectangle &rect) {
		DrawStringWithColors(out, "{}{}{}",
		    { { "Kerning", UiFlags::ColorUiSilver },
		        { "Fit", UiFlags::ColorUiGold },
		        { "Spacing", UiFlags::ColorUiSilverDark } },
		    rect,
		    TextRenderOptions { .flags = UiFlags::KerningFitSpacing });
	});
	DrawWithBorder(out, Rectangle { Point { 0, y += 15 }, Size { 170, 15 } }, [&](const Rectangle &rect) {
		DrawStringWithColors(out, "{}{}{}",
		    { { "KerningFitSpacing", UiFlags::ColorUiSilver },
		        { " | ", UiFlags::ColorUiGold },
		        { "AlignCenter", UiFlags::ColorUiSilverDark } },
		    rect,
		    TextRenderOptions { .flags = UiFlags::KerningFitSpacing | UiFlags::AlignCenter | UiFlags::ColorUiSilver });
	});
	DrawWithBorder(out, Rectangle { Point { 0, y += 15 }, Size { 170, 15 } }, [&](const Rectangle &rect) {
		DrawStringWithColors(out, "{}{}{}",
		    { { "KerningFitSpacing", UiFlags::ColorUiSilver },
		        { " | ", UiFlags::ColorUiGold },
		        { "AlignRight", UiFlags::ColorUiSilverDark } },
		    rect,
		    TextRenderOptions { .flags = UiFlags::KerningFitSpacing | UiFlags::AlignRight | UiFlags::ColorUiSilver });
	});

	const std::string actualPath = paths::BasePath() + FixturesPath + "actual.png";
	const std::string expectedPath = paths::BasePath() + FixturesPath + "expected.png";
	SDL_RWops *actual = SDL_RWFromFile(actualPath.c_str(), "wb");
	ASSERT_NE(actual, nullptr) << SDL_GetError();
	ASSERT_TRUE(WriteSurfaceToFilePng(out, actual).has_value());
	EXPECT_EQ(ReadFile(actualPath), ReadFile(expectedPath)) << "\n"
	                                                        << expectedPath << "\n"
	                                                        << actualPath;
}

} // namespace
} // namespace devilution
